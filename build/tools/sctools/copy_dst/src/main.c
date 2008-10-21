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
#include        "nuc.h"

#include        "myfilename.h"
#include        "mfiler.h"

//================================================================================

static FSEventHook  sSDHook;
static BOOL sd_card_flag = FALSE;


static void SDEvents(void *userdata, FSEvent event, void *arg)
{
  (void)userdata;
  (void)arg;
  if (event == FS_EVENT_MEDIA_REMOVED) {
    sd_card_flag = FALSE;
    mprintf("sdmc:removed!\n");
  }
  else if (event == FS_EVENT_MEDIA_INSERTED) {
    sd_card_flag = TRUE;
    mprintf("sdmc:inserted!\n");
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
  mprintf("RTC restore ");
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
  mprintf("Unique ID restore ");
  /* すでにブート時に一度 hwn_info はリードしている。 */
  STD_CopyMemory( (void *)hwn_info.movableUniqueID, (void *)(mydata.movableUniqueID),
		  LCFG_TWL_HWINFO_MOVABLE_UNIQUE_ID_LEN );

  if ( LCFGi_THW_WriteNormalInfoDirect( &hwn_info )) {
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
    mprintf("OK.\n");
  }
  else {
    // error
    m_set_palette(tc[0], M_TEXT_COLOR_RED ); /* red  */
    mprintf("NG.\n");
  }
  m_set_palette(tc[0], M_TEXT_COLOR_WHITE );

#if 0
  // いらんやんか・・
  // static BOOL SDBackupToSDCard1(void)
  mprintf("Unique ID backup ");
  if( TRUE == MiyaBackupHWNormalInfo( MyFile_GetUniqueIDFileName() ) ) {
  }
#endif
  return TRUE;
}

static BOOL RestoreFromSDCard3(void)
{
  // static BOOL SDBackupToSDCard2(void)
  mprintf("WirelessLAN param. restore ");
  if( TRUE == nvram_restore( MyFile_GetWifiParamFileName() ) ) {
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );
    mprintf("OK.\n");
  }
  else {
    // error
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
  }
  m_set_palette(tc[0], M_TEXT_COLOR_WHITE );

  return TRUE;
}

static BOOL RestoreFromSDCard4(void)
{
  // static BOOL SDBackupToSDCard3(void)
  mprintf("User setting param. restore ");
  if( TRUE == MiyaRestoreTWLSettings( MyFile_GetUserSettingsFileName() ) ) {
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );
    mprintf("OK.\n");
  }
  else {
    // error
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
  }
  m_set_palette(tc[0], M_TEXT_COLOR_WHITE );

  return TRUE;
}

static BOOL RestoreFromSDCard5(void)
{
  // static BOOL SDBackupToSDCard4(void)
  mprintf("App. shared files restore ");
  if( TRUE == RestoreDirEntryList( MyFile_GetAppSharedListFileName(), MyFile_GetAppSharedRestoreLogFileName() )) {
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );
    mprintf("OK.\n");
  }
  else {
    // error
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
  }
  m_set_palette(tc[0], M_TEXT_COLOR_WHITE );

  return TRUE;
}

static BOOL RestoreFromSDCard6(void)
{
  // static BOOL SDBackupToSDCard5(void)
  mprintf("Photo files restore ");
  if( TRUE == RestoreDirEntryList( MyFile_GetPhotoListFileName() , MyFile_GetPhotoRestoreLogFileName() )) {
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );
    mprintf("OK.\n");
  }
  else {
    // error
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
  }
  m_set_palette(tc[0], M_TEXT_COLOR_WHITE );

  return TRUE;
}


static BOOL LoadWlanConfig(void)
{
  /* ユーザーデータ書き込みモード */
  if( TRUE == LoadWlanConfigFile("sdmc:/wlan_cfg.txt") ) {
    OS_TPrintf("SSID = %s\n", GetWlanSSID()); 
    OS_TPrintf("MODE = ");

    switch( GetWlanMode() ) {
    case 1:
      OS_TPrintf("NONE\n");
      break;
    case 2:
      OS_TPrintf("WEP128\n");
      break;
    case 3:
      OS_TPrintf("WPA-TKIP\n");
      break;
    case 4:
      OS_TPrintf("WPA2-TKIP\n");
      break;
    case 5:
      OS_TPrintf("WPA-AES\n");
      break;
    case 6:
      OS_TPrintf("WPA2-AES\n");
      break;
    defalut:
      OS_TPrintf("Unknow mode..\n");
      break;
    }
    OS_TPrintf("KEY STR = %s\n", GetWlanKEYSTR());
    {
      u8 buf[256];
      int len;
      int i;
      len = GetWlanKEYBIN(buf);
      if( len ) {
	OS_TPrintf("KEY BIN = 0x");
	for( i = 0 ; i < len ; i++ ) {
	  OS_TPrintf("%02X",buf[i]);
	}
	OS_TPrintf("\n");
      }
    }
  }
  else {
    OS_TPrintf("Invalid wlan cfg file\n");
  }
  return TRUE;
}


static BOOL RestoreFromSDCard7(void)
{
  u64 *title_id_buf_ptr = NULL;
  int title_id_count = 0;
  /* hws_info.serialNoは戻せない */
  /*  */
  // static BOOL SDBackupToSDCard7(void)

  // for DEBUG
  mydata.shop_record_flag = TRUE; 

  if( mydata.shop_record_flag == FALSE ) {
    /* ネットワークにつながなくていいか？ */
    OS_TPrintf("no shop record\n - you don't have to connect the network.\n");
    mprintf("no shop record\n");
  }
  else {
    mprintf("User title list restore ");
    OS_TPrintf("User title list restore \n");
      // (void)TitleIDSave( MyFile_GetDownloadTitleIDFileName(), pBuffer, count, NULL);
    if( TRUE == TitleIDLoad( MyFile_GetDownloadTitleIDFileName(), &title_id_buf_ptr, 
			     &title_id_count, NULL) ) {

      m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
      mprintf("OK.\n");    }
    else {
      m_set_palette(tc[0], M_TEXT_COLOR_RED );
      mprintf("NG.\n");
    }
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );

    mprintf("Wireless AP conf. loading.. ");
    if( TRUE == LoadWlanConfig() ) {
      m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
      mprintf("OK.\n");

      //      nuc_main();	/* ネットワーク接続 */
      hatamotolib_main();	/* ネットワーク接続 */
      /* nand:/ticketはチケット同期でダウンロード */
    }
    else {
      m_set_palette(tc[0], M_TEXT_COLOR_RED );
      mprintf("NG.\n");
    }
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );

#if 0
    // static BOOL SDBackupToSDCard6(void)
    mprintf("App. save data backup ");
    if( 0 == find_title_save_data( &dir_entry_list_head, MyFile_GetAppDataSaveDirName(), "nand:/title",
				   &save_dir_info, MyFile_GetAppDataLogFileName(),0 ) ) {
    }
#endif

    /* 最後にネットワークアップデート。 */
  }

  return TRUE;
}


typedef BOOL (*function_ptr)(void);

static function_ptr function_table[] =
{
#if 1
  RestoreFromSDCard1,
  RestoreFromSDCard2,
  RestoreFromSDCard3,
  RestoreFromSDCard4,
  RestoreFromSDCard5,
  RestoreFromSDCard6,
#endif
  RestoreFromSDCard7
};

static int function_table_max = sizeof(function_table) / sizeof(*function_table);
static int function_counter = 0;


static void MyThreadProc(void *arg)
{
#pragma unused(arg)
  OSMessage message;
  while( 1 ) {
    (void)OS_ReceiveMessage(&MyMesgQueue, &message, OS_MESSAGE_BLOCK);

    /* MydataLoadはすでにやっているのでいらない。 */
    for( function_counter = 0 ; function_counter < function_table_max ; function_counter++ ) {
      (void)(function_table[function_counter])();
    }
    /* 全部成功したらフォルダを消す */
    if( TRUE == stream_is_play1_end() ) {
      OS_TPrintf("stream play\n"); 
      stream_play1();
    }
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



  /*
     0 -> black
     1 -> red
     2 -> green
     3 -> blue
     4 -> yellow
     5 -> purple
     6 -> sky blue
     7 -> red
     8 -> green
     9 -> blue
     0xA -> yellow
     0xB -> purple
     0xC -> sky blue
     0xD -> white
     0xE -> white
     0xF -> white

 */
  //  m_set_palette(tc[0], 0xF);



  // 必須；SEA の初期化
  SEA_Init();

  // 不要：NAM の初期化
  //  NAM_Init(&AllocForNAM, &FreeForNAM);
  
  // 必須：ES の初期化
  ES_InitLib();


  if( RTC_RESULT_SUCCESS != RTC_GetDate( &rtc_date ) ) {
    mprintf("rtc read date error.\n");
  }
  if( RTC_RESULT_SUCCESS != RTC_GetTime( &rtc_time ) ) {
    mprintf("rtc read time error.\n");
  }

  //  mprintf("HW Normal Info. read ");
  mprintf("Unique ID read ");
  if( FALSE == MiyaReadHWNormalInfo( &hwn_info ) ) {
    m_set_palette(tc[0], 0x1);	/* red  */
    mprintf("NG.\n");
    m_set_palette(tc[0], 0xF);
  }
  else {
    m_set_palette(tc[0], 0x2);	/* green  */
    mprintf("OK.\n");
    m_set_palette(tc[0], 0xF);	/* white */
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
  mprintf("Serial No. read ");
  if( FALSE == MiyaReadHWSecureInfo( &hws_info ) ) {
    m_set_palette(tc[0], 0x1);	/* red  */
    mprintf("NG.\n");
    m_set_palette(tc[0], 0xF);	/* white */
  }
  else {
    m_set_palette(tc[0], 0x2);	/* green  */
    mprintf("OK.\n");
    m_set_palette(tc[0], 0xF);	/* white */
    mprintf(" %s\n", hws_info.serialNo);
  }
  mprintf("\n");
  
  OS_GetMacAddress( macAddress );
  mprintf("MAC Address 0x");
  for ( i = 0 ; i < 6 ; i++ ) {
    mprintf("%02X", macAddress[i]);
  }
  mprintf("\n");


  if( FALSE == SDCardValidation() ) {
    sd_card_flag = FALSE;
    m_set_palette(tc[0], 0x1);	/* red  */
    mprintf("No SD Card\n");
    m_set_palette(tc[0], 0xF);	/* white */
  }
  else {
    sd_card_flag = TRUE;
  }

#if 0
    mprintf("\n\n");
    mprintf( "function no.%d/%d\n", function_counter, function_table_max);
    mprintf("\n\n");
#endif


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
	    if(TRUE == MydataLoad( MyFile_GetGlobalInformationFileName(), &mydata, sizeof(MyData), NULL) ) {
	      mprintf("global info. restore ");
	      m_set_palette(tc[0], 0x2);	/* green  */
	      mprintf("OK.\n");
	      m_set_palette(tc[0], 0xF);	/* white */
	      start_my_thread();
	    }
	    else {
	      mprintf("Not a backup data directory\n");
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

    mfprintf(tc[1], "\f%4d/%02d/%02d %02d:%02d:%02d\n\n", 
	   rtc_date.year + 2000, rtc_date.month , rtc_date.day,
	   rtc_time.hour , rtc_time.minute , rtc_time.second ); 

    mfprintf(tc[1], "function no.%d/%d\n", function_counter, function_table_max);

    //    mfprintf(tc[1], "cwd = %s\n\n", MFILER_Get_CurrentDir());

    MFILER_ClearDir(&mfiler_list_head);
    MFILER_ReadDir(&mfiler_list_head, MFILER_Get_CurrentDir());
    MFILER_DisplayDir(tc[1], &mfiler_list_head, 0 );

    loop_counter++;

  }
  OS_Terminate();
}


/*====== End of main.c ======*/

