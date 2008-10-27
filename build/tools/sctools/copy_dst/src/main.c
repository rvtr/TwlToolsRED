/*---------------------------------------------------------------------------*
  Project:  
  File:     main.c

  Copyright 2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/
#include <twl.h>
#include <nitro/nvram/nvram.h>
#include "ecdl.h"
#include <twl/sea.h>
#include <twl/lcfg.h>
#include <twl/na.h>
#include <twl/nam.h>
#include <NitroWiFi/nhttp.h>
#include "nssl.h"

#include        "font.h"
#include        "text.h"
#include        "mprintf.h"
#include        "gfx.h"
#include        "key.h"
#include        "my_fs_util.h"
#include        "mynvram.h"
#include        "stream.h"
#include        "hwi.h"
#include        "hatamotolib.h"
#include        "ecdl.h"
#include        "mywlan.h"
#include        "mydata.h"
#include        "netconnect.h"
#include        "sitedefs.h"
#include        "wcm_control.h"
#include        "nuc.h"
#include        "mynuc.h"

#include        "myfilename.h"
#include        "mfiler.h"
#include        "menu_version.h"


//================================================================================

static BOOL completed_flag = FALSE;
static FSEventHook  sSDHook;
static BOOL sd_card_flag = FALSE;
static BOOL reboot_flag = FALSE;

static u8 WorkForNA[NA_VERSION_DATA_WORK_SIZE];


static void SDEvents(void *userdata, FSEvent event, void *arg)
{
  (void)userdata;
  (void)arg;
  if (event == FS_EVENT_MEDIA_REMOVED) {
    sd_card_flag = FALSE;
    m_set_palette(tc[0], M_TEXT_COLOR_YELLOW );
    mprintf("SD card:removed!\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    text_blink_clear(tc[0]);
    if( completed_flag == FALSE ) {
      text_blink_current_line(tc[0]);
      mprintf("insert SD card\n");
    }
  }
  else if (event == FS_EVENT_MEDIA_INSERTED) {
    sd_card_flag = TRUE;
    if( completed_flag == FALSE ) {
      m_set_palette(tc[0], M_TEXT_COLOR_YELLOW );
      mprintf("SD card:inserted!\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
      text_blink_clear(tc[0]);
      text_blink_current_line(tc[0]);
      if( reboot_flag == FALSE ) {
	mprintf("Press A button to start UPDATE\n");
      }
      else {
	mprintf("Press A button to start RESTORE\n");
      }
    }
  }
}



static MyData mydata;

static int vram_num_main = 1;
static int vram_num_sub = 0;


static  LCFGTWLHWNormalInfo hwn_info;
static  LCFGTWLHWSecureInfo hws_info;



#define	MY_STACK_SIZE  (1024*64) /* でかいほうがいい */
#define	MY_THREAD_PRIO        20
static OSThread MyThread;
static u64 MyStack[MY_STACK_SIZE / sizeof(u64)];
static void MyThreadProc(void *arg);
static void MyThreadProcNuc(void *arg);

static OSMessage MyMesgBuffer_request[1];
static OSMessage MyMesgBuffer_response[1];
static OSMessageQueue MyMesgQueue_request;
static OSMessageQueue MyMesgQueue_response;

static void init_my_thread(void)
{

  OS_InitMessageQueue(&MyMesgQueue_request, &MyMesgBuffer_request[0], 1);
  OS_InitMessageQueue(&MyMesgQueue_response, &MyMesgBuffer_response[0], 1);

  OS_CreateThread(&MyThread, MyThreadProc,
		  NULL, MyStack + MY_STACK_SIZE / sizeof(u64),
		  MY_STACK_SIZE, MY_THREAD_PRIO);
  OS_WakeupThreadDirect(&MyThread);
}

static void init_my_thread_nuc(void)
{
  OS_InitMessageQueue(&MyMesgQueue_request, &MyMesgBuffer_request[0], 1);
  OS_InitMessageQueue(&MyMesgQueue_response, &MyMesgBuffer_response[0], 1);

  OS_CreateThread(&MyThread, MyThreadProcNuc,
		  NULL, MyStack + MY_STACK_SIZE / sizeof(u64),
		  MY_STACK_SIZE, MY_THREAD_PRIO);
  OS_WakeupThreadDirect(&MyThread);
}


static BOOL start_my_thread(void)
{
  OSMessage message;
  if( TRUE == OS_ReceiveMessage(&MyMesgQueue_response, &message, OS_MESSAGE_NOBLOCK) ) {
    (void)OS_SendMessage(&MyMesgQueue_request, (OSMessage)0, OS_MESSAGE_NOBLOCK);
    return TRUE;
  }
  return FALSE;
}

static BOOL RestoreFromSDCard1(void)
{
  BOOL flag = TRUE;
  /* 基本的にこいつらの逆をやる(Get -> Set, Save->Loadなど) */
  /* 
     ちなみにすでにMydataLoad関数は成功しているものとする。
     したがって MyData mydata にはデータが入っている。
  */
  // static BOOL SDBackupToSDCard8(void)
  mprintf("RTC data restore             ");
  if( RTC_RESULT_SUCCESS != RTC_SetDate( &(mydata.rtc_date) ) ) {
    flag = FALSE;
  }
  if( RTC_RESULT_SUCCESS != RTC_SetTime( &(mydata.rtc_time) ) ) {
    flag = FALSE;
  }

  if( flag == TRUE ) {
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
    mprintf("OK.\n");
  }
  else {
    m_set_palette(tc[0], M_TEXT_COLOR_RED ); /* red  */
    mprintf("NG.\n");
  }
  m_set_palette(tc[0], M_TEXT_COLOR_WHITE );

  return flag;
}

static BOOL RestoreFromSDCard2(void)
{
  mprintf("Unique ID restore            ");
  /* すでにブート時に一度 hwn_info はリードしている。 */
  STD_CopyMemory( (void *)hwn_info.movableUniqueID, (void *)(mydata.movableUniqueID),
		  LCFG_TWL_HWINFO_MOVABLE_UNIQUE_ID_LEN );

  if ( LCFGi_THW_WriteNormalInfoDirect( &hwn_info )) {
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }
  else {
    // error
    m_set_palette(tc[0], M_TEXT_COLOR_RED ); /* red  */
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    return FALSE;
  }
  return TRUE;
}


static BOOL RestoreFromSDCard3(void)
{
  // static BOOL SDBackupToSDCard2(void)
  mprintf("WirelessLAN param. restore   ");
  if( TRUE == nvram_restore( MyFile_GetWifiParamFileName() ) ) {
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }
  else {
    // error
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    return FALSE;
  }

  return TRUE;
}

static BOOL RestoreFromSDCard4(void)
{
  // static BOOL SDBackupToSDCard3(void)
  mprintf("User setting param. restore  ");
  if( TRUE == MiyaRestoreTWLSettings( MyFile_GetUserSettingsFileName() ) ) {
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }
  else {
    // error
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    return FALSE;
  }

  return TRUE;
}

static BOOL RestoreFromSDCard5(void)
{
  // static BOOL SDBackupToSDCard4(void)
  mprintf("App. shared files restore    ");
  if( TRUE == RestoreDirEntryList( MyFile_GetAppSharedListFileName(), MyFile_GetAppSharedRestoreLogFileName() )) {
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }
  else {
    // error
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    return FALSE;
  }
  return TRUE;
}

static BOOL RestoreFromSDCard6(void)
{
  // static BOOL SDBackupToSDCard5(void)
  mprintf("Photo files restore          ");
  if( TRUE == RestoreDirEntryList( MyFile_GetPhotoListFileName() , MyFile_GetPhotoRestoreLogFileName() )) {
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }
  else {
    // error
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    return FALSE;
  }
  return TRUE;
}


static BOOL RestoreFromSDCard8(void)
{

  // static BOOL SDBackupToSDCard6(void)
  mprintf("App. save data restore       ");
  if( TRUE == RestoreDirEntryList( MyFile_GetAppDataListFileName() , MyFile_GetAppDataRestoreLogFileName() )) {
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }
  else {
    // error
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    return FALSE;
  }
  return TRUE;
}


static BOOL LoadWlanConfig(void)
{
  u8 buf[256];
  int len;
  int i;  /* ユーザーデータ書き込みモード */
  if( TRUE == LoadWlanConfigFile("sdmc:/wlan_cfg.txt") ) {
    OS_TPrintf("SSID = %s\n", GetWlanSSID()); 
    mfprintf(tc[3],"SSID = %s\n", GetWlanSSID()); 
    OS_TPrintf("MODE = ");
    mfprintf(tc[3],"MODE = ");

    switch( GetWlanMode() ) {
    case 1:
      OS_TPrintf("NONE\n");
      mfprintf(tc[3],"NONE\n");
      break;
    case 2:
      OS_TPrintf("WEP128\n");
      mfprintf(tc[3],"WEP128\n");
      break;
    case 3:
      OS_TPrintf("WPA-TKIP\n");
      mfprintf(tc[3],"WPA-TKIP\n");
      break;
    case 4:
      OS_TPrintf("WPA2-TKIP\n");
      mfprintf(tc[3],"WPA2-TKIP\n");
      break;
    case 5:
      OS_TPrintf("WPA-AES\n");
      mfprintf(tc[3],"WPA-AES\n");
      break;
    case 6:
      OS_TPrintf("WPA2-AES\n");
      mfprintf(tc[3],"WPA2-AES\n");
      break;
    defalut:
      OS_TPrintf("Unknow mode..\n");
      mfprintf(tc[3],"Unknow mode..\n");
      break;
    }
    OS_TPrintf("KEY STR = %s\n", GetWlanKEYSTR());
    mfprintf(tc[3],"KEY STR = %s\n", GetWlanKEYSTR());

    len = GetWlanKEYBIN(buf);
    if( len ) {
      OS_TPrintf("KEY BIN = 0x");
      mfprintf(tc[3],"KEY BIN = 0x");
      for( i = 0 ; i < len ; i++ ) {
	OS_TPrintf("%02X",buf[i]);
	mfprintf(tc[3],"%02X",buf[i]);
      }
      OS_TPrintf("\n");
      mfprintf(tc[3],"\n");
    }
  }
  else {
    OS_TPrintf("Invalid wlan cfg file\n");
    mfprintf(tc[3],"Invalid wlan cfg file\n");
    return FALSE;
  }
  return TRUE;
}


static BOOL RestoreFromSDCard7(void)
{
  u64 *title_id_buf_ptr;
  int title_id_count;
  int i;
  ECError rv;

  title_id_buf_ptr = NULL;
  title_id_count = 0;
  rv = EC_ERROR_OK;

  /* hws_info.serialNoは戻せない */
  /*  */
  // static BOOL SDBackupToSDCard7(void)

  // for DEBUG
  //  mydata.shop_record_flag = TRUE; 


  if( mydata.shop_record_flag == FALSE ) {
    /* ネットワークにつながなくていいか？ */
    OS_TPrintf("no shop record\n");
    mprintf(" (--no shop record--)\n");
  }
  else {
    mprintf("Connect to the internet\n");

    mprintf("-user title list loading..   ");
    OS_TPrintf("user title list loading\n");
    if( TRUE == TitleIDLoad( MyFile_GetDownloadTitleIDFileName(), &title_id_buf_ptr, 
			     &title_id_count, NULL) ) {

      m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
      mprintf("OK.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
      for( i = 0; i < title_id_count ; i++ ) {
	u64 tid = *(title_id_buf_ptr + i );
	mprintf(" id %02d %08X %08X\n", i,(u32)(tid >> 32), (u32)tid);
      }
    }
    else {
      m_set_palette(tc[0], M_TEXT_COLOR_RED );
      mprintf("NG.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    }

    //    mprintf("                             ");
    mprintf("-wireless AP conf. loading.. ");
    if( TRUE == LoadWlanConfig() ) {
      m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
      mprintf("OK.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );

      /* nand:/ticketはチケット同期でダウンロード */
      // 不要：デバイス情報の表示
      //      PrintDeviceInfo();

      //      OS_TPrintf("--------------------------------\n");

      // setup
      // 必須：タイトル ID の偽装
      SetupShopTitleId();
    
      // ？：ユーザ設定がされていないと接続できないので適当に設定
      SetupUserInfo();
    
      // 必須：バージョンデータのマウント
      SetupVerData();
    
      // 必須：ネットワークへの接続
      NcStart(SITEDEFS_DEFAULTCLASS);
    
      /******** ネットワークにつないだ *************/

      // 必須：HTTP と SSL の初期化
      OS_TPrintf("start NHTTP\n");
      mprintf("-start NHTTP                 ");
      SetupNSSL();
      if( FALSE == SetupNHTTP() ) {
	m_set_palette(tc[0], M_TEXT_COLOR_RED );
	mprintf("NG.\n");
	m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
	goto end_nhttp;
      }
      else {
	m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
	mprintf("OK.\n");
	m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
      }

      /******** NHTTP & NSSLにつないだ *************/
    
      // 必須：EC の初期化
      OS_TPrintf("start EC\n");
      mprintf("-start EC                    ");
      if( FALSE == SetupEC() ) {
	m_set_palette(tc[0], M_TEXT_COLOR_RED );
	mprintf("NG.\n");
	m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
	goto end_ec;
      }
      else {
	m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
	mprintf("OK.\n");
	m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
      }

      // 必須：デバイス証明書の発行
      if( FALSE == KPSClient() ) {
	goto end_ec_f;
      }

      if( FALSE == ECDownload((NAMTitleId *)title_id_buf_ptr , (u32)title_id_count) ) {
	goto end_ec_f;
      }

      // 不要：セーブデータ領域を作成
      // NAM_Init を忘れてた
      SetupTitlesDataFile((NAMTitleId *)title_id_buf_ptr , (u32)title_id_count);

    end_ec_f:
      // cleanup
      // EC の終了処理
      mprintf("-EC_Shutdown..               ");
      rv = EC_Shutdown();
      // SDK_WARNING(rv == EC_ERROR_OK, "Failed to shutdown EC, rv=%d\n", rv);
      if( rv != EC_ERROR_OK ) {
	m_set_palette(tc[0], M_TEXT_COLOR_RED );
	mprintf("NG.\n");
	m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
      }
      else {
	m_set_palette(tc[0], M_TEXT_COLOR_GREEN );
	mprintf("OK.\n");
      }
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    end_ec:

    
      // ネットワークからの切断
      OS_TPrintf("disconnecting ..\n");
      mprintf("-disconnecting... ");

      NHTTP_Cleanup();
    end_nhttp:
      NSSL_Finish();
    end_nssl:
      NcFinish();
    end_nc:
      //OS_TPrintf("NSSL_Finish() return = %d\n", NSSL_Finish());

      TerminateWcmControl();

      OS_TPrintf("done.\n");
      mprintf("done.\n");

      if( title_id_buf_ptr != NULL && title_id_count != 0 ) {
	OS_Free( title_id_buf_ptr );
      }
      // EC が自分の Title ID のディレクトリを作成してしまうため、削除する
      DeleteECDirectory();
      
    }
    else {
      /* mprintf("-Wireless AP conf. loading.. "); */
      m_set_palette(tc[0], M_TEXT_COLOR_RED );
      mprintf("NG.\n");
    }
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }

  return TRUE;
}


typedef BOOL (*function_ptr)(void);

static function_ptr function_table[] =
{
  RestoreFromSDCard1,
  RestoreFromSDCard2,
  RestoreFromSDCard3,
  RestoreFromSDCard4,
  RestoreFromSDCard5,
  RestoreFromSDCard6,
  RestoreFromSDCard7,
  RestoreFromSDCard8
};

static int function_table_max = sizeof(function_table) / sizeof(*function_table);
static int function_counter = 0;


static void MyThreadProc(void *arg)
{
#pragma unused(arg)
  OSMessage message;
  BOOL flag;
  BOOL twl_card_validation_flag;

  while( 1 ) {
    (void)OS_SendMessage(&MyMesgQueue_response, (OSMessage)0, OS_MESSAGE_NOBLOCK);
    (void)OS_ReceiveMessage(&MyMesgQueue_request, &message, OS_MESSAGE_BLOCK);
    flag = TRUE;
    /* MydataLoadはすでにやっているのでいらない。 */
    for( function_counter = 0 ; function_counter < function_table_max ; function_counter++ ) {
      if( FALSE == (function_table[function_counter])() ) {
	flag = FALSE;
      }
    }
    mprintf("\n");
    if( flag == TRUE ) {
      completed_flag = TRUE;
      if( TRUE == stream_play_is_end() ) {
	stream_play0();	/* cursor.aiff */
      }
      m_set_palette(tc[0], M_TEXT_COLOR_YELLOW );
      mprintf("Pull out DS(DSi) & SD CARDs!\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
      while( 1 ) {
	if( twl_card_validation_flag == TRUE ) {
	  /* なぜか一回しかかからないので・・ */
	  if( FALSE == TWLCardValidation() ) {
	    twl_card_validation_flag = FALSE;
	    mprintf("DS(DSi)CARD pulled out\n");
	  }
	}

	if( (FALSE == twl_card_validation_flag) && (FALSE == SDCardValidation()) ) {
	  m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
	  mprintf("Restore completed.\n");
	  m_set_palette(tc[0], M_TEXT_COLOR_WHITE );

	  if( TRUE == stream_play_is_end() ) {
	    stream_play1(); /* ok.aiff */
	  }
	  Gfx_Set_BG1_Color((u16)M_TEXT_COLOR_DARKGREEN);
	  OS_Sleep(200000);
	  break; 
	}
	OS_Sleep(200);
      }

    }
    else {
      m_set_palette(tc[0], M_TEXT_COLOR_RED );
      mprintf("Restore failed.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
      if( TRUE == stream_play_is_end() ) {
	stream_play2();	/* ng.aiff */
      }
      Gfx_Set_BG1_Color((u16)M_TEXT_COLOR_DARKRED);
    }
    mprintf("\n");
    /* 全部成功したらフォルダを消す */
  }
}


static void MyThreadProcNuc(void *arg)
{
#pragma unused(arg)
  OSMessage message;
  u16 keyData;
  while( 1 ) {
    (void)OS_SendMessage(&MyMesgQueue_response, (OSMessage)0, OS_MESSAGE_NOBLOCK);
    (void)OS_ReceiveMessage(&MyMesgQueue_request, &message, OS_MESSAGE_BLOCK);
    mprintf("-Wireless AP conf. loading.. ");
    if( TRUE == LoadWlanConfig() ) {
      m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
      mprintf("OK.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
      NcStart(SITEDEFS_DEFAULTCLASS);
      /* NSSL_Init()呼んではダメ！ */
      if( TRUE == my_numc_proc() ) {
	m_set_palette(tc[0], M_TEXT_COLOR_GREEN );
	OS_TPrintf("Network Update Completed!\n");
	mprintf("Network Update Completed!\n");

	m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
	NcFinish();
	TerminateWcmControl();
	
	if( TRUE == stream_play_is_end() ) {
	  stream_play0(); /* cursor.aiff */
	}
	/* ハードウェアリセットを行い、自分自身を起動します。 */
	mprintf("\n");
	text_blink_current_line(tc[0]);
	mprintf("press A button to start RESTORE\n\n");

	while( 1 ) {
	  keyData = m_get_key_code();
	  if ( keyData & PAD_BUTTON_A ) {
	    OS_RebootSystem();
	  }
	  OS_Sleep(20);
	}
      }
      else {
	// NUC_Init() failed, error code=34416 はどこかでNSSL_Initを呼んでるので消す。
	m_set_palette(tc[0], M_TEXT_COLOR_RED );
	mprintf("network update failed!\n");
	OS_TPrintf("Network Update failed!\n");
	m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
	Gfx_Set_BG1_Color((u16)M_TEXT_COLOR_DARKRED);
      }
    }
    else {
      /* mprintf("-Wireless AP conf. loading.. "); */
      m_set_palette(tc[0], M_TEXT_COLOR_RED );
      mprintf("NG.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    }
  }
}


static void *AllocForNAM(size_t size)
{
  OSIntrMode old = OS_DisableInterrupts();
  void* p = OS_Alloc(size);
  OS_RestoreInterrupts(old);
  return p;
}

static void FreeForNAM(void* ptr)
{
  if( ptr != NULL ) {
    OSIntrMode old = OS_DisableInterrupts();
    OS_Free(ptr);
    OS_RestoreInterrupts(old);
  }
}

static BOOL myTWLCardCallback( void )
{
  return FALSE; // means that not terminate.
}

void TwlMain(void)
{
  void* newArenaLo;
  OSHeapHandle hHeap;
  u16 keyData;
  int loop_counter = 0;
  int save_dir_info = 0;
  MY_DIR_ENTRY_LIST *dir_entry_list_head = NULL;
  RTCDate rtc_date;
  RTCTime rtc_time;
  int i;
  //  int n;
  u8 macAddress[6];
  MY_ENTRY_LIST *mfiler_list_head = NULL;
  u16 s_major, s_minor;
  u32 s_timestamp;
  ESError es_error_code;
  BOOL MydataLoadDecrypt_message_flag = TRUE;
  BOOL MydataLoadDecrypt_dir_flag = TRUE;
  BOOL MydataLoadDecrypt_success_flag = TRUE;

  OS_Init();
  OS_InitThread();

  OS_InitTick();
  OS_InitAlarm();


  // マスター割り込みフラグを許可に
  (void)OS_EnableIrq();
  
  // IRQ 割り込みを許可します
  (void)OS_EnableInterrupts();

  // ARM7との通信FIFO割り込み許可
  (void)OS_EnableIrqMask(OS_IE_SPFIFO_RECV);

  // ファイルシステム初期化
  FS_Init( FS_DMA_NOT_USE );


  // メインアリーナのアロケートシステムを初期化
  newArenaLo = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 1);
  OS_SetMainArenaLo(newArenaLo);
  
  // メインアリーナ上にヒープを作成
  hHeap = OS_CreateHeap(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi());
  OS_SetCurrentHeap(OS_ARENA_MAIN, hHeap);

  Gfx_Init();

  RTC_Init();

  SCFG_Init();

  NVRAMi_Init();

  SND_Init();
  stream_main();
  CARD_Init();
  CARD_SetPulledOutCallback( myTWLCardCallback );

  // 必須；SEA の初期化
  SEA_Init();

  reboot_flag = OS_IsRebooted();
  /* デバッグのために今だけ強制的にオン(UPDATE mode) */
  /* miya */
  //  reboot_flag = TRUE;


  if( FALSE == SDCardValidation() ) {
    sd_card_flag = FALSE;
    m_set_palette(tc[0], 0x1);	/* red  */
    mprintf("No SD card\n");
    m_set_palette(tc[0], 0xF);	/* white */
  }
  else {
    sd_card_flag = TRUE;
  }
  FS_RegisterEventHook("sdmc", &sSDHook, SDEvents, NULL);


  if( FALSE == Read_SystemMenuVersion(&s_major, &s_minor, &s_timestamp) ) {
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf( "system menu ver. read error!\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    s_major = 0;
    s_minor = 0;
    s_timestamp = 0;
  }


  ES_InitLib();

  if( FALSE == MiyaReadHWNormalInfo( &hwn_info ) ) {
    m_set_palette(tc[0], 0x1);	/* red  */
    mprintf("HW Normal Info. read error.\n");
    m_set_palette(tc[0], 0xF);
  }

  //  mprintf("HW Secure Info. read ");
  if( FALSE == MiyaReadHWSecureInfo( &hws_info ) ) {
    m_set_palette(tc[0], 0x1);	/* red  */
    mprintf("HW Secure Info. read error.\n");
    m_set_palette(tc[0], 0xF);	/* white */
  }

  // region
  mydata.region = LCFG_THW_GetRegion();
  // ES Device ID

  mydata.fuseId = SCFG_ReadFuseData();
  OS_TPrintf("eFuseID:   %08X%08X\n", (u32)(mydata.fuseId >> 32), (u32)(mydata.fuseId));

  OS_GetMacAddress( macAddress );

  es_error_code = ES_GetDeviceId(&mydata.deviceId);
  if( es_error_code == ES_ERR_OK ) {
    mydata.shop_record_flag = TRUE;
  }
  else {
    OS_TPrintf("es_error_code = %d\n", es_error_code );
    mydata.shop_record_flag = FALSE;
  }
  
  // (void)CheckShopRecord( hws_info.region, NULL );
  
  if( TRUE == mydata.shop_record_flag ) {
    snprintf(mydata.bmsDeviceId, sizeof(mydata.bmsDeviceId), "%lld", ((0x3ull << 32) | mydata.deviceId));
    // OS_TPrintf("DeviceID:  %08X %u\n", mydata.deviceId, mydata.deviceId);
    OS_TPrintf("DeviceID: %s\n", mydata.bmsDeviceId);
  }





  (void)m_get_key_trigger();
  keyData = m_get_key_code();
  if ( keyData & PAD_BUTTON_X ) {
    reboot_flag = TRUE;
  }

  if( FALSE == reboot_flag ) {
    /* 最初はネットワークアップデート。 */

    //  NSSL_Init(&s_sslConfig);
    //    SetupNSSL();
    if (!NA_LoadVersionDataArchive(WorkForNA, NA_VERSION_DATA_WORK_SIZE)) {
      OS_TPrintf("NA load error\n");
      mprintf("NA load error\n");
    }
    else {
      if (!NUC_LoadCert()) {
	// WRAMにロード
	OS_TPrintf("Client cert load error\n");
	mprintf("Client cert load error\n");
      }
#if 0
      else {
	OS_TPrintf("Client cert load success\n");
	mprintf("Client cert load success\n");
      }
#endif
      (void)NA_UnloadVersionDataArchive();
    }

    init_my_thread_nuc();


#if 0 /* 自動スタートはいらない */
    if( sd_card_flag == TRUE ) {
      start_my_thread();
    }
#endif
    if( sd_card_flag == TRUE ) {
      text_blink_current_line(tc[0]);
      mprintf("press A button to start UPDATE\n\n");
    }
    else {
      text_blink_current_line(tc[0]);
      mprintf("insert SD card\n");
    }

  }
  else {
    // 不要：NAM の初期化
    NAM_Init(&AllocForNAM, &FreeForNAM);

    /* 書き戻し過程 */
    if( sd_card_flag == TRUE ) {
      text_blink_current_line(tc[0]);
      mprintf("press A button to start RESTORE\n\n");
    }
    else {
      text_blink_current_line(tc[0]);
      mprintf("insert SD card\n");
    }

    init_my_thread();

    // vram_num_main = 1;
    // vram_num_sub = 0;

    vram_num_sub = 2;

    MFILER_CurrentDir_Init();

  }

  if( TRUE == stream_play_is_end() ) {
    stream_play0(); /* cursor.aiff */
  }

  
  while( 1 ) {
    Gfx_Render( vram_num_main , vram_num_sub );
    OS_WaitVBlankIntr();
    (void)RTC_GetDate( &rtc_date );
    (void)RTC_GetTime( &rtc_time );
    keyData = m_get_key_trigger();
    
    // ＡＲＭ７コマンド応答受信
    while (SND_RecvCommandReply(SND_COMMAND_NOBLOCK) != NULL)
      {
      }
    // コマンドフラッシュ（フラッシュして即座に実行を要求）
    (void)SND_FlushCommand(SND_COMMAND_NOBLOCK | SND_COMMAND_IMMEDIATE);
    if ( keyData & PAD_BUTTON_R ) {
      vram_num_main++;
      if( vram_num_main > (MAX_VRAM_NUM-1) ) {
	vram_num_main = 0;
      }
    }
    else if ( keyData & PAD_BUTTON_L ) {
      vram_num_main--;
      if( vram_num_main < 0 ) {
	vram_num_main = (MAX_VRAM_NUM-1);
      }
    }
    else if ( keyData & PAD_BUTTON_A ) {
      if( sd_card_flag == TRUE ) {
	if( FALSE == reboot_flag ) {
	  /* ネットワークアップデート */
	  text_blink_clear(tc[0]);
	  start_my_thread();
	}
	else {
	  /* リストア */
	  if( vram_num_sub == 2 ) {
	    MydataLoadDecrypt_dir_flag = MFILER_Is_Cursor_Dir( &mfiler_list_head );
	    if( TRUE == MydataLoadDecrypt_dir_flag) {
	      text_blink_clear(tc[0]);
	      
	      MyFile_SetPathBase("sdmc:/");
	      MyFile_AddPathBase((const char *)MFILER_GetCursorEntryPath( &mfiler_list_head ) );
	      MyFile_AddPathBase("/");

	      mprintf("Personal data. restore       ");
	      MydataLoadDecrypt_success_flag = MydataLoadDecrypt( MyFile_GetGlobalInformationFileName(), 
								  &mydata, sizeof(MyData), NULL);
	      if(TRUE == MydataLoadDecrypt_success_flag ) {
		if( mydata.version_major != MY_DATA_VERSION_MAJOR ) {
		  m_set_palette(tc[0], M_TEXT_COLOR_RED );
		  mprintf("NG.\n");
		  m_set_palette(tc[0], M_TEXT_COLOR_YELLOW );
		  mprintf(" illegal format version.\n");
		  mprintf(" %s\n version %d.%d\n",MyFile_GetGlobalInformationFileName(),
			  mydata.version_major,mydata.version_minor);
		  m_set_palette(tc[0], 0xF);	/* white */
		  MydataLoadDecrypt_message_flag = FALSE;
  		}
		else if( mydata.version_minor != MY_DATA_VERSION_MINOR ) {
		  m_set_palette(tc[0], M_TEXT_COLOR_RED );
		  mprintf("NG.\n");
		  m_set_palette(tc[0], M_TEXT_COLOR_YELLOW );
		  mprintf(" illegal format version.\n");
		  mprintf(" %s\n version %d.%d\n",MyFile_GetGlobalInformationFileName(),
			  mydata.version_major,mydata.version_minor);
		  m_set_palette(tc[0], 0xF);	/* white */
		  MydataLoadDecrypt_message_flag = FALSE;
		}
		else {
		  m_set_palette(tc[0], 0x2);	/* green  */
		  mprintf("OK.\n");
		  m_set_palette(tc[0], 0xF);	/* white */

		  vram_num_sub = 0;
		  MydataLoadDecrypt_message_flag = TRUE;
		  MydataLoadDecrypt_dir_flag = TRUE;
		  MydataLoadDecrypt_success_flag = TRUE;
		  start_my_thread();
		}

	      }
	      else {	
		m_set_palette(tc[0], 0x1);	/* red  */
		mprintf("NG.\n");
		m_set_palette(tc[0], 0xF);	/* white */
		MydataLoadDecrypt_message_flag = FALSE;
	      }
	    }
	    else {
	      mprintf("Not a backup data directory\n");
	      if( TRUE == stream_play_is_end() ) {
		stream_play2();	/* ng.aiff */
	      }
	      MydataLoadDecrypt_message_flag = FALSE;

	    }
	  }
	}
      }
      else {
	text_blink_clear(tc[0]);
	text_blink_current_line(tc[0]);
	mprintf("insert SD card\n");
      }
    }
    else if ( keyData & PAD_BUTTON_B ) {
    }
    else if ( keyData & PAD_BUTTON_START ) {
    }
    else if ( keyData & PAD_BUTTON_SELECT ) {
    }
    else if ( keyData & PAD_BUTTON_X ) {
    }
    else if ( keyData & PAD_BUTTON_Y ) {
    }
    else if ( keyData & PAD_KEY_UP ) {
      if( FALSE == reboot_flag ) {
      }
      else {
	if( vram_num_sub == 2 ) {
	  MFILER_CursorY_Up();
	}
      }
    }
    else if ( keyData & PAD_KEY_DOWN ) {
      if( FALSE == reboot_flag ) {
      }
      else {
	if( vram_num_sub == 2 ) {
	  MFILER_CursorY_Down();
	}
      }
    }

    m_set_palette(tc[1], M_TEXT_COLOR_LIGHTBLUE );
    mfprintf(tc[1], "\fRepaire Tool RESTORE");
    m_set_palette(tc[1], M_TEXT_COLOR_WHITE );
    mfprintf(tc[1], "  ver.%d.%d \n",MY_DATA_VERSION_MAJOR , MY_DATA_VERSION_MINOR );
    mfprintf(tc[1], "   build:%s %s\n\n",__DATE__,__TIME__);

    m_set_palette(tc[1], M_TEXT_COLOR_LIGHTBLUE );
    mfprintf(tc[1],"Sys-menu ver." );
    m_set_palette(tc[1], M_TEXT_COLOR_WHITE );
    
    mfprintf(tc[1],"%d.%d", s_major, s_minor );
    mfprintf(tc[1]," (%08x)\n\n", s_timestamp );


    /* ユニークＩＤは表示しなくていい？ */
    m_set_palette(tc[1], M_TEXT_COLOR_LIGHTBLUE );
    mfprintf(tc[1], "Unique ID:\n");
    m_set_palette(tc[1], M_TEXT_COLOR_WHITE );
    mfprintf(tc[1], "   ");
    for( i =  0; i < LCFG_TWL_HWINFO_MOVABLE_UNIQUE_ID_LEN/2 ; i++ ) {
      mfprintf(tc[1], "%02X:", hwn_info.movableUniqueID[i]);
    }
    mfprintf(tc[1], "\n   ");
    for( ; i < LCFG_TWL_HWINFO_MOVABLE_UNIQUE_ID_LEN ; i++ ) {
      mfprintf(tc[1], "%02X:", hwn_info.movableUniqueID[i]);
    }
    mfprintf(tc[1], "\n\n");
    
    
    m_set_palette(tc[1], M_TEXT_COLOR_LIGHTBLUE );
    mfprintf(tc[1], "Serial No. ");
    m_set_palette(tc[1], M_TEXT_COLOR_WHITE );
    mfprintf(tc[1], "%s\n", hws_info.serialNo);
    mfprintf(tc[1], "\n");


    m_set_palette(tc[1], M_TEXT_COLOR_LIGHTBLUE );
    mfprintf(tc[1], "eFuse ID:  ");
    m_set_palette(tc[1], M_TEXT_COLOR_WHITE );
    mfprintf(tc[1],"%08X%08X\n\n", (u32)(mydata.fuseId >> 32), (u32)(mydata.fuseId));


    m_set_palette(tc[1], M_TEXT_COLOR_LIGHTBLUE );
    mfprintf(tc[1],"MAC add.:  ");
    m_set_palette(tc[1], M_TEXT_COLOR_WHITE );
    mfprintf(tc[1],"%02X:%02X:%02X:%02X:%02X:%02X",  macAddress[0],macAddress[1],
	     macAddress[2], macAddress[3], macAddress[4], macAddress[5]);
    mfprintf(tc[1],"\n\n");

    m_set_palette(tc[1], M_TEXT_COLOR_LIGHTBLUE );
    mfprintf(tc[1],"Device ID: ");
    m_set_palette(tc[1], M_TEXT_COLOR_WHITE );
    if( TRUE == mydata.shop_record_flag ) {
      mfprintf(tc[1],"%s\n", mydata.bmsDeviceId);
    }
    else {
      m_set_palette(tc[1], M_TEXT_COLOR_YELLOW );
      mfprintf(tc[1],"-----------\n");
      m_set_palette(tc[1], M_TEXT_COLOR_WHITE );
    }
    mfprintf(tc[1],"\n");

    mfprintf(tc[1], "%4d/%02d/%02d %02d:%02d:%02d\n\n", 
	     rtc_date.year + 2000, rtc_date.month , rtc_date.day,
	     rtc_time.hour , rtc_time.minute , rtc_time.second ); 


    //    mfprintf(tc[1], "cwd = %s\n\n", MFILER_Get_CurrentDir());
    
    if( FALSE == reboot_flag ) {
    }
    else {
      mfprintf(tc[1], "function no.%d/%d\n\n", function_counter, function_table_max);

      mfprintf(tc[2],"\f");

      if( MydataLoadDecrypt_dir_flag == FALSE ) {
	mfprintf(tc[2], "Not a backup data directory\n");
	mfprintf(tc[2],"\n\n\n");
      }
      else if( MydataLoadDecrypt_success_flag == FALSE ) {
	mfprintf(tc[2], "file load error\n");
	mfprintf(tc[2],"\n\n\n");
      }
      else if( MydataLoadDecrypt_message_flag == FALSE ) {
	if( mydata.version_major != MY_DATA_VERSION_MAJOR ) {
	  mfprintf(tc[2]," illegal format version.\n");
	  mfprintf(tc[2]," %s\n",MyFile_GetGlobalInformationFileName());
	  mfprintf(tc[2]," version %d.%d\n", mydata.version_major,mydata.version_minor);
	  m_set_palette(tc[2], 0xF);	/* white */
	  mfprintf(tc[2],"\n");
	}
	else if( mydata.version_minor != MY_DATA_VERSION_MINOR ) {
	  mfprintf(tc[2]," illegal format version.\n");
	  mfprintf(tc[2]," %s\n",MyFile_GetGlobalInformationFileName());
	  mfprintf(tc[2]," version %d.%d\n", mydata.version_major,mydata.version_minor);
	  m_set_palette(tc[1], 0xF);	/* white */
	  mfprintf(tc[2],"\n");
	}
      }
      else {
	mfprintf(tc[2],"\n\n\n\n");
      }

      MFILER_ClearDir(&mfiler_list_head);
      MFILER_ReadDir(&mfiler_list_head, MFILER_Get_CurrentDir());
      MFILER_DisplayDir(tc[2], &mfiler_list_head, 0 );
    }
    
    loop_counter++;

  }
#if 0
      else if ( keyData & PAD_KEY_UP ) {
	if( vram_num_main != 1 ) {
	  n = m_get_display_offset_y(tc[0]);
	  n++;
	  m_set_display_offset_y(tc[0], n);
	}
	else if( vram_num_sub == 2 ) {
	  MFILER_CursorY_Up();
	}
      }
      else if ( keyData & PAD_KEY_DOWN ) {
	if( vram_num_main != 1 ) {
	  n = m_get_display_offset_y(tc[0]);
	  n--;
	  m_set_display_offset_y(tc[0], n);
	}
	else if( vram_num_sub == 2 ) {
	  MFILER_CursorY_Down();
	}
      }
      else if ( keyData & PAD_KEY_RIGHT ) {
	n = m_get_display_offset_x(tc[0]);
	n++;
	m_set_display_offset_x(tc[0], n);
      }
      else if ( keyData & PAD_KEY_LEFT ) {
	n = m_get_display_offset_x(tc[0]);
	n--;
	m_set_display_offset_x(tc[0], n);
      }
#endif

  OS_Terminate();
}

//#define ENABLE_PROXY 1

#ifdef ENABLE_PROXY
// DWC_GetProxySetting 関数で、接続している設定のproxy設定を格納する構造体
typedef struct DWCstProxySetting			// Proxy 設定情報
{
    u16         authType;					// Proxy 認証形式
    u16         port;						// Proxy port
    u8          hostName      [ 0x64 ];		// Proxy hostname
    u8          authId        [ 0x20 ];		// proxy basic認証用 ID
    u8          authPass      [ 0x20 ];		// proxy basic認証用 パスワード
} DWCProxySetting;
BOOL DWC_GetProxySetting( DWCProxySetting* proxy );
BOOL DWC_GetProxySetting( DWCProxySetting* proxy )
{
    MI_CpuClear8(proxy, sizeof(proxy));
    STD_StrCpy((char*)proxy->hostName, "proxy.testbox");
    proxy->port = 8080;
    return TRUE;
}
#endif



/*====== End of main.c ======*/

