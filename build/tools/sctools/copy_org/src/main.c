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

#if 0
static  char path_base[256];
static  char path_log[256];
static  char path[256];
#endif
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



static BOOL SDBackupToSDCard1(void)
{
  /* nand:/sysディレクトリまわりの保存 */
  mprintf("Unique ID backup ");
  if( TRUE == MiyaBackupHWNormalInfo( MyFile_GetUniqueIDFileName() ) ) {
    m_set_palette(tc[0], 0x2);	/* green  */
    mprintf("OK.\n");
  }
  else {
    m_set_palette(tc[0], 0x1);	/* red  */
    mprintf("NG.\n");
  }
  m_set_palette(tc[0], 0xF);	/* white */
  return TRUE;
}


static BOOL SDBackupToSDCard2(void)
{
  /* Wifi設定の保存 */
  mprintf("WirelessLAN param. backup ");
  if( TRUE == nvram_backup( MyFile_GetWifiParamFileName() ) ) {
    m_set_palette(tc[0], 0x2);	/* green  */
    mprintf("OK.\n");
  }
  else {
    m_set_palette(tc[0], 0x1);	/* red  */
    mprintf("NG.\n");
  }
  m_set_palette(tc[0], 0xF);	/* white */
  return TRUE;
}

static BOOL SDBackupToSDCard3(void)
{

  /* nand:/shared1ディレクトリまわりの保存 */
  mprintf("User setting param. backup ");
  if( TRUE == MiyaBackupTWLSettings( MyFile_GetUserSettingsFileName() ) ) {
    m_set_palette(tc[0], 0x2);	/* green  */
    mprintf("OK.\n");
  }
  else {
    m_set_palette(tc[0], 0x1);	/* red  */
    mprintf("NG.\n");
  }
  m_set_palette(tc[0], 0xF);	/* white */
  return TRUE;
}

static BOOL SDBackupToSDCard4(void)
{
  MY_DIR_ENTRY_LIST *dir_entry_list_head = NULL;
  int save_dir_info = 0;

  /*
    nand:/shared2ディレクトリまわりの保存
    内容はアプリケーション共有ファイル
    nand:/shared2/*
  */
  mprintf("App. shared files backup ");
  if( 0 == copy_r( &dir_entry_list_head, MyFile_GetAppSharedSaveDirName() , "nand:/shared2" , MyFile_GetAppSharedLogFileName(), 0) ) {
    // PrintDirEntryListBackward( dir_entry_list_head, NULL );
    mydata.num_of_shared2_files = SaveDirEntryList( dir_entry_list_head, MyFile_GetAppSharedListFileName() );
    m_set_palette(tc[0], 0x2);	/* green  */
    mprintf("OK.\n");
  }
  else {
    m_set_palette(tc[0], 0x1);	/* red  */
    mprintf("NG.\n");
  }
  m_set_palette(tc[0], 0xF);	/* white */
  (void)ClearDirEntryList( &dir_entry_list_head );
  return TRUE;
}

static BOOL SDBackupToSDCard5(void)
{
  MY_DIR_ENTRY_LIST *dir_entry_list_head = NULL;
  int save_dir_info = 0;

  /* 
     nand2:/photoディレクトリまわりの保存
     内容は写真長のJPEGファイル
     nand2:/photo/*.*
   */
  mprintf("Photo files backup ");
  if( 0 == copy_r( &dir_entry_list_head, MyFile_GetPhotoSaveDirName() , "nand2:/photo" , MyFile_GetPhotoLogFileName(),0 ) ) {
    // PrintDirEntryListBackward( dir_entry_list_head, NULL );
    mydata.num_of_photo_files = SaveDirEntryList( dir_entry_list_head, MyFile_GetPhotoListFileName() );
    m_set_palette(tc[0], 0x2);	/* green  */
    mprintf("OK.\n");
  }
  else {
    m_set_palette(tc[0], 0x1);	/* red  */
    mprintf("NG.\n");
  }
  m_set_palette(tc[0], 0xF);	/* white */
  (void)ClearDirEntryList( &dir_entry_list_head );


  return TRUE;
}

static BOOL SDBackupToSDCard6(void)
{
  MY_DIR_ENTRY_LIST *dir_entry_list_head = NULL;
  int save_dir_info = 0;

  /* nand:/ticketはチケット同期？でうまいこと合わせてくれるんでバックアップ不要 */

  /* 
     nand:/titleディレクトリまわりの保存
     nand:/title/*.savファイルをすべてバックアップ
  */

  mprintf("App. save data backup ");
  if( 0 == find_title_save_data( &dir_entry_list_head, MyFile_GetAppDataSaveDirName(), "nand:/title",
				 &save_dir_info, MyFile_GetAppDataLogFileName(),0 ) )
    {
      // PrintDirEntryListBackward( dir_entry_list_head, NULL );
      mydata.num_of_app_save_data = SaveDirEntryList( dir_entry_list_head , MyFile_GetAppDataListFileName() );
      m_set_palette(tc[0], 0x2);	/* green  */
      mprintf("OK.\n");
    }
  else {
    m_set_palette(tc[0], 0x1);	/* red  */
    mprintf("NG.\n");
  }
  m_set_palette(tc[0], 0xF);	/* white */
  (void)ClearDirEntryList( &dir_entry_list_head );

  return TRUE;
}

static BOOL SDBackupToSDCard7(void)
{
  MY_DIR_ENTRY_LIST *dir_entry_list_head = NULL;
  int save_dir_info = 0;
  u64 *pBuffer;
  int count;
  int j;
  u64 *ptr;

  /* タイトルリストの生成 */
  /* 
     nand:/title/00030004/

     nand:/title/00030004/34626241/content/title.tmd
     nand:/title/00030004/34626241/content/00000000.app
     nand:/title/00030004/34626241/data/

     nand:/title/00030017/484e4141 は ランチャー
     nand:/title/00030015/484e4641 は shop
     nand:/title/00030015/484e4241 は 本体設定


     No. 0 0003000f484e4c41
     No. 1 0003000f484e4841
     No. 2 0003000f484e4341 
     No. 3 00030015484e4241 
     No. 4 00030017484e4141 launcher
     ^
     | ここの最下位ビットが１のやつがシステムアプリ
     | 
     システムアプリはダウンロード対象外
  */
  mprintf("User title list backup ");
  OS_TPrintf("User title list backup \n");
  if( 0 == get_title_id( &dir_entry_list_head, "nand:/title", &save_dir_info, MyFile_GetDownloadTitleIDLogFileName(), 0 ) ) {

    GetDirEntryList( dir_entry_list_head, &pBuffer, &count);

    OS_TPrintf("title id count = %d\n", count );
    ptr = pBuffer;
    mydata.num_of_user_download_app = count;
    
    if( ptr != NULL && count != 0 )  {
      for( j = 0 ; j < count ; j++ ) {
	OS_TPrintf("No. %d %llx\n",j,*ptr);
	mfprintf(tc[2],"No. %d %llx\n",j,*ptr);
	ptr++;
      }
    }
    (void)TitleIDSave( MyFile_GetDownloadTitleIDFileName(), pBuffer, count, NULL);
    OS_Free(pBuffer);
    PrintSrcDirEntryListBackward( dir_entry_list_head, NULL );
    m_set_palette(tc[0], 0x2);	/* green  */
    mprintf("OK.\n");
  }
  else {
    m_set_palette(tc[0], 0x1);	/* red  */
    mprintf("NG.\n");
  }
  m_set_palette(tc[0], 0xF);	/* white */

  (void)ClearDirEntryList( &dir_entry_list_head );

  return TRUE;
}

static BOOL SDBackupToSDCard8(void)
{
  RTCDate rtc_date;
  RTCTime rtc_time;

  /* オリジナルのデータのバックアップ */
  if( TRUE == CheckShopRecord(NULL) ) {
    mydata.shop_record_flag = TRUE;
    OS_TPrintf("shop record exist - you don't have to connect the network.\n");
    mprintf("shop record exist\n");
  }
  else {
    mydata.shop_record_flag = FALSE;
    OS_TPrintf("no shop record\n - you don't have to connect the network.\n");
    mprintf("no shop record\n");
  }

  if( RTC_RESULT_SUCCESS != RTC_GetDate( &rtc_date ) ) {
    mprintf("rtc read date error.\n");
  }
  if( RTC_RESULT_SUCCESS != RTC_GetTime( &rtc_time ) ) {
    mprintf("rtc read time error.\n");
  }

  STD_CopyMemory( (void *)&(mydata.rtc_date), (void *)&rtc_date, sizeof(RTCDate) );
  STD_CopyMemory( (void *)&(mydata.rtc_time), (void *)&rtc_time, sizeof(RTCTime) );
  STD_CopyMemory( (void *)(mydata.movableUniqueID), (void *)hwn_info.movableUniqueID,
		  LCFG_TWL_HWINFO_MOVABLE_UNIQUE_ID_LEN );
  mprintf("org. data backup ");
  if( TRUE == MydataSave( MyFile_GetGlobalInformationFileName(), (void *)&mydata, sizeof(MyData), NULL) ) {
    m_set_palette(tc[0], 0x2);	/* green  */
    mprintf("OK.\n");
  }
  else {
    m_set_palette(tc[0], 0x1);	/* red  */
    mprintf("NG.\n");
  }
  m_set_palette(tc[0], 0xF);	/* white */
  return TRUE;
}


typedef BOOL (*function_ptr)(void);

static function_ptr function_table[] =
{
  SDBackupToSDCard1,
  SDBackupToSDCard2,
  SDBackupToSDCard3,
  SDBackupToSDCard4,
  SDBackupToSDCard5,
  SDBackupToSDCard6,
  SDBackupToSDCard7,
  SDBackupToSDCard8
};

static int function_table_max = sizeof(function_table) / sizeof(*function_table);
static int function_counter = 0;


static void MyThreadProc(void *arg)
{
#pragma unused(arg)
  OSMessage message;
  while( 1 ) {
    (void)OS_ReceiveMessage(&MyMesgQueue, &message, OS_MESSAGE_BLOCK);
    MyFile_SetPathBase("sdmc:/");
    MyFile_AddPathBase((const char *)hws_info.serialNo );
    MyFile_AddPathBase("/");
    for( function_counter = 0 ; function_counter < function_table_max ; function_counter++ ) {
      (void)(function_table[function_counter])();
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
#if 0
    mprintf("  0x");
    for( i =  0; i < LCFG_TWL_HWINFO_SERIALNO_LEN_MAX/2 ; i++ ) {
      mprintf("%02X:", hws_info.serialNo[i]);
    }
    mprintf("\n  0x");
    for( ; i < LCFG_TWL_HWINFO_SERIALNO_LEN_MAX ; i++ ) {
      if( hws_info.serialNo[i] ) {
	mprintf("%02X:", hws_info.serialNo[i]);
      }
      else {
	// #define LCFG_TWL_HWINFO_SERIALNO_LEN_MAX  15 // 本体シリアルNo.長Max(終端付きなので、14bytesまで拡張
	// 終端をみつけたらブレーク
	break;
      }
    }
#endif
    mprintf("\n");
    
#if 0
    mprintf(" validLang.bmp = 0x%08x\n", hws_info.validLanguageBitmap );
    mprintf(" wifi disable flag = %d\n", hws_info.flags.forceDisableWireless );
    mprintf(" lchr-TitleIDLo = " );
    for( i = 0 ; i < 4 ; i++ ) {
      mprintf("%02X:", hws_info.launcherTitleID_Lo[i]);
    }
    mprintf("\n Region data = 0x%02x\n\n", hws_info.region );
#endif
  }
  
  OS_GetMacAddress( macAddress );
  mprintf("MAC Address 0x");
  for ( i = 0 ; i < 6 ; i++ ) {
    mprintf("%02X", macAddress[i]);
  }
  mprintf("\n\n");



  if( FALSE == SDCardValidation() ) {
    sd_card_flag = FALSE;
    m_set_palette(tc[0], 0x1);	/* red  */
    mprintf("No SD Card\n");
  }
  else {
    sd_card_flag = TRUE;
    //  m_set_palette(tc[0], 0x2);	/* green  */
    //  mprintf("Detect SD Card\n");
  }
  m_set_palette(tc[0], 0xF);	/* white */


  FS_RegisterEventHook("sdmc", &sSDHook, SDEvents, NULL);


  init_my_thread();

  if( sd_card_flag == TRUE ) {
    start_my_thread();
  }

  while( 1 ) {
    OS_WaitVBlankIntr();
    Gfx_Render( vram_num_main , vram_num_sub );
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
	start_my_thread();
      }
      else {
	mprintf("insert SD card\n");
      }
    }
    else if ( keyData & PAD_BUTTON_B ) {
      /* ユーザーデータ書き込みモード */
      if( TRUE == LoadWlanConfigFile("sdmc:/wlan_cfg.txt") ) {
	OS_TPrintf("SSID = %s\n", GetWlanSSID()); 
	OS_TPrintf("MODE = "); 
	switch( GetWlanMode() ) {
	case 1:
	  OS_TPrintf("OPEN\n");
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
      nuc_main();


    }
    else if ( keyData & PAD_BUTTON_START ) {
    }
    else if ( keyData & PAD_BUTTON_SELECT ) {
    }
    else if ( keyData & PAD_BUTTON_X ) {
      OS_TPrintf("stream on\n");
      if( TRUE == stream_is_play1_end() ) {
	stream_play1();
      }
    }
    else if ( keyData & PAD_BUTTON_Y ) {
    }
    else if ( keyData & PAD_KEY_UP ) {
      n = m_get_display_offset_y(tc[0]);
      n++;
      m_set_display_offset_y(tc[0], n);
    }
    else if ( keyData & PAD_KEY_DOWN ) {
      n = m_get_display_offset_y(tc[0]);
      n--;
      m_set_display_offset_y(tc[0], n);
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


    //    mfprintf(tc[1], "\f\ncounter = %d\n\n", loop_counter);
    mfprintf(tc[1], "\f\n%4d/%02d/%02d %02d:%02d:%02d\n\n", 
	     rtc_date.year + 2000, rtc_date.month , rtc_date.day,
	     rtc_time.hour , rtc_time.minute , rtc_time.second ); 

    mfprintf(tc[1], "press A -> Store to SD Card\n");
    mfprintf(tc[1], "press B -> Load to NAND Flash\n");
    mfprintf(tc[1], "\n");
    mfprintf(tc[1], "function no.%d/%d\n", function_counter, function_table_max);
    
    loop_counter++;

  }
  OS_Terminate();
}


/*====== End of main.c ======*/

