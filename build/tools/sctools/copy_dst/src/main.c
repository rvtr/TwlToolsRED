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

static FSEventHook  sSDHook;
static BOOL sd_card_flag = FALSE;

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

  }
  else if (event == FS_EVENT_MEDIA_INSERTED) {
    sd_card_flag = TRUE;
    m_set_palette(tc[0], M_TEXT_COLOR_YELLOW );
    mprintf("SD card:inserted!\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    mprintf("Push A button to start RESTORE\n");
  }
}


static MyData mydata;

static int vram_num_main = 1;
static int vram_num_sub = 0;


static  LCFGTWLHWNormalInfo hwn_info;
static  LCFGTWLHWSecureInfo hws_info;



#define	MY_STACK_SIZE  (1024*16) /* でかいほうがいい */
#define	MY_THREAD_PRIO        20
static OSThread MyThread;
static u64 MyStack[MY_STACK_SIZE / sizeof(u64)];
static void MyThreadProc(void *arg);
static void MyThreadProcNuc(void *arg);

static OSMessage MyMesgBuffer[1];
static OSMessageQueue MyMesgQueue;

static void init_my_thread(void)
{

  OS_InitMessageQueue(&MyMesgQueue, &MyMesgBuffer[0], 1);

  OS_CreateThread(&MyThread, MyThreadProc,
		  NULL, MyStack + MY_STACK_SIZE / sizeof(u64),
		  MY_STACK_SIZE, MY_THREAD_PRIO);
  OS_WakeupThreadDirect(&MyThread);
}

static void init_my_thread_nuc(void)
{

  OS_InitMessageQueue(&MyMesgQueue, &MyMesgBuffer[0], 1);

  OS_CreateThread(&MyThread, MyThreadProcNuc,
		  NULL, MyStack + MY_STACK_SIZE / sizeof(u64),
		  MY_STACK_SIZE, MY_THREAD_PRIO);
  OS_WakeupThreadDirect(&MyThread);
}


static void start_my_thread(void)
{
  (void)OS_SendMessage(&MyMesgQueue, (OSMessage)0, OS_MESSAGE_NOBLOCK);
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
      PrintDeviceInfo();

      OS_TPrintf("--------------------------------\n");

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
  while( 1 ) {
    (void)OS_ReceiveMessage(&MyMesgQueue, &message, OS_MESSAGE_BLOCK);
    flag = TRUE;
    /* MydataLoadはすでにやっているのでいらない。 */
    for( function_counter = 0 ; function_counter < function_table_max ; function_counter++ ) {
      if( FALSE == (function_table[function_counter])() ) {
	flag = FALSE;
      }
    }
    mprintf("\n");
    if( flag == TRUE ) {
      m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
      mprintf("Restore completed.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
      if( TRUE == stream_play_is_end() ) {
	stream_play1();
      }
    }
    else {
      m_set_palette(tc[0], M_TEXT_COLOR_RED );
      mprintf("Restore failed.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
      if( TRUE == stream_play_is_end() ) {
	stream_play2();
      }
    }
    mprintf("\n");
    /* 全部成功したらフォルダを消す */
  }
}


static void MyThreadProcNuc(void *arg)
{
#pragma unused(arg)
  OSMessage message;
  while( 1 ) {
    (void)OS_ReceiveMessage(&MyMesgQueue, &message, OS_MESSAGE_BLOCK);
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
	  stream_play1();
	}

	/* ハードウェアリセットを行い、自分自身を起動します。 */
	OS_Sleep(30000);
	OS_RebootSystem();
      }
      else {
	// NUC_Init() failed, error code=34416 はどこかでNSSL_Initを呼んでるので消す。
	m_set_palette(tc[0], M_TEXT_COLOR_RED );
	mprintf("network update failed!\n");
	OS_TPrintf("Network Update failed!\n");
	m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
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
  if( ptr != NULL )
    {
      OSIntrMode old = OS_DisableInterrupts();
      OS_Free(ptr);
      OS_RestoreInterrupts(old);
    }
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
  int n;
  u8 macAddress[6];
  MY_ENTRY_LIST *mfiler_list_head = NULL;
  BOOL reboot_flag;
  u16 s_major, s_minor;
  u32 s_timestamp;

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

  // 必須；SEA の初期化
  SEA_Init();

  m_set_palette(tc[0], M_TEXT_COLOR_YELLOW );
  mprintf( "Sys-menu ver." );
  m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  if( TRUE == Read_SystemMenuVersion(&s_major, &s_minor, &s_timestamp) ) {
    mprintf( "%d.%d", s_major, s_minor );
    mprintf( " (%08x)\n", s_timestamp );
  }
  else {
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf( "read error!\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }
  mprintf( "\n");
  

  reboot_flag = OS_IsRebooted();

  /* デバッグのために強制的に */
  //  reboot_flag = TRUE;
  (void)m_get_key_trigger();
  keyData = m_get_key_code();
  if ( keyData & PAD_BUTTON_X ) {
    reboot_flag = TRUE;
  }

  if( FALSE == reboot_flag ) {

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


    /* 最初にネットワークアップデート。 */
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


    if( sd_card_flag == TRUE ) {
      start_my_thread();
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
	  start_my_thread();
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
      }
      mfprintf(tc[1], "\f%4d/%02d/%02d %02d:%02d:%02d\n\n", 
	       rtc_date.year + 2000, rtc_date.month , rtc_date.day,
	       rtc_time.hour , rtc_time.minute , rtc_time.second ); 

    }
  }
  else {
    // 不要：NAM の初期化
    NAM_Init(&AllocForNAM, &FreeForNAM);
    
    // 必須：ES の初期化
    ES_InitLib();
    
    
    if( RTC_RESULT_SUCCESS != RTC_GetDate( &rtc_date ) ) {
      m_set_palette(tc[0], 0x1);	/* red  */
      mprintf("rtc date read error.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    }
    if( RTC_RESULT_SUCCESS != RTC_GetTime( &rtc_time ) ) {
      m_set_palette(tc[0], 0x1);	/* red  */
      mprintf("rtc time read error.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    }
    
    //  mprintf("HW Normal Info. read ");
    m_set_palette(tc[0], M_TEXT_COLOR_YELLOW );
    mprintf("Unique ID:\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );

    if( FALSE == MiyaReadHWNormalInfo( &hwn_info ) ) {
      m_set_palette(tc[0], 0x1);	/* red  */
      mprintf("read error!.\n");
      m_set_palette(tc[0], 0xF);
    }
    else {
      mprintf(" 0x");
      for( i =  0; i < LCFG_TWL_HWINFO_MOVABLE_UNIQUE_ID_LEN/2 ; i++ ) {
	mprintf("%02X:", hwn_info.movableUniqueID[i]);
      }
      mprintf("\n 0x");
      for( ; i < LCFG_TWL_HWINFO_MOVABLE_UNIQUE_ID_LEN ; i++ ) {
	mprintf("%02X:", hwn_info.movableUniqueID[i]);
      }
      mprintf("\n");
      //    mprintf(" RTC Adjust data = 0x%02x\n", hwn_info.rtcAdjust );
    }
    mprintf("\n");  


    //  mprintf("HW Secure Info. read ");
    m_set_palette(tc[0], M_TEXT_COLOR_YELLOW );
    mprintf("Serial No. ");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    if( FALSE == MiyaReadHWSecureInfo( &hws_info ) ) {
      m_set_palette(tc[0], 0x1);	/* red  */
      mprintf("read error.\n");
      m_set_palette(tc[0], 0xF);	/* white */
    }
    else {
      mprintf("%s\n", hws_info.serialNo);
    }
    mprintf("\n");
  
    OS_GetMacAddress( macAddress );
    m_set_palette(tc[0], M_TEXT_COLOR_YELLOW );
    mprintf("MAC add.(HEX):");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    for ( i = 0 ; i < 6 ; i++ ) {
      mprintf("%02X", macAddress[i]);
    }
    mprintf("\n\n");

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

    init_my_thread();


    MFILER_CurrentDir_Init();

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
	/* ユーザーデータ吸出しモード */
	if( sd_card_flag == TRUE ) {
	  if( vram_num_main != 1 ) {
	  }
	  else {
	    if( TRUE == MFILER_Is_Cursor_Dir( &mfiler_list_head ) ) {
	      MyFile_SetPathBase("sdmc:/");
	      MyFile_AddPathBase((const char *)MFILER_GetCursorEntryPath( &mfiler_list_head ) );
	      MyFile_AddPathBase("/");
	      if(TRUE == MydataLoadDecrypt( MyFile_GetGlobalInformationFileName(), &mydata, sizeof(MyData), NULL) ) {
		mprintf("Personal data. restore       ");
		m_set_palette(tc[0], 0x2);	/* green  */
		mprintf("OK.\n");
		m_set_palette(tc[0], 0xF);	/* white */
		start_my_thread();
	      }
	      else {
		m_set_palette(tc[0], 0x1);	/* red  */
		mprintf("NG.\n");
		m_set_palette(tc[0], 0xF);	/* white */
	      }
	    }
	    else {
	      mprintf("Not a backup data directory\n");
	      // mprintf("global info. read failed(Not dir.)\n");
	    }
	  }
	}
	else {
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
	if( vram_num_main != 1 ) {
	  n = m_get_display_offset_y(tc[0]);
	  n++;
	  m_set_display_offset_y(tc[0], n);
	}
	else {
	  MFILER_CursorY_Up();
	}
      }
      else if ( keyData & PAD_KEY_DOWN ) {
	if( vram_num_main != 1 ) {
	  n = m_get_display_offset_y(tc[0]);
	  n--;
	  m_set_display_offset_y(tc[0], n);
	}
	else {
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
  label:
      mfprintf(tc[1], "\f%4d/%02d/%02d %02d:%02d:%02d\n\n", 
	       rtc_date.year + 2000, rtc_date.month , rtc_date.day,
	       rtc_time.hour , rtc_time.minute , rtc_time.second ); 

      mfprintf(tc[1], "function no.%d/%d\n\n", function_counter, function_table_max);

      //    mfprintf(tc[1], "cwd = %s\n\n", MFILER_Get_CurrentDir());

      MFILER_ClearDir(&mfiler_list_head);
      MFILER_ReadDir(&mfiler_list_head, MFILER_Get_CurrentDir());
      MFILER_DisplayDir(tc[1], &mfiler_list_head, 0 );

      loop_counter++;

    }
  }

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

