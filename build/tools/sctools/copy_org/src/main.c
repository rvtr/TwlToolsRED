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
#include        "ecdl.h"
#include        "mywlan.h"
#include        "mydata.h"
#include        "nuc.h"

#include        "myfilename.h"
#include        "menu_version.h"

//================================================================================

static FSEventHook  sSDHook;
static BOOL sd_card_flag = FALSE;


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
    mprintf("push A button to start BACKUP\n");
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



static BOOL SDBackupToSDCard2(void)
{
  /* Wifi設定の保存 */
  mprintf("WirelessLAN param. backup    ");
  if( TRUE == nvram_backup( MyFile_GetWifiParamFileName() ) ) {
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }
  else {
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    return FALSE;
  }
  return TRUE;
}

static BOOL SDBackupToSDCard3(void)
{

  /* nand:/shared1ディレクトリまわりの保存 */
  mprintf("User setting param. backup   ");
  if( TRUE == MiyaBackupTWLSettings( MyFile_GetUserSettingsFileName() ) ) {
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }
  else {
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    return FALSE;
  }
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
  mprintf("App. shared files backup     ");
  if( 0 == copy_r( &dir_entry_list_head, MyFile_GetAppSharedSaveDirName() , 
		   "nand:/shared2" , MyFile_GetAppSharedLogFileName(), 0) ) {

    // PrintDirEntryListBackward( dir_entry_list_head, NULL );
    mydata.num_of_shared2_files = SaveDirEntryList( dir_entry_list_head, MyFile_GetAppSharedListFileName() );
    m_set_palette(tc[0], 0x2);	/* green  */
    mprintf("OK.\n");
    m_set_palette(tc[0], 0xF);	/* white */
  }
  else {
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    return FALSE;
  }
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
  mprintf("Photo files backup           ");
  if( 0 == copy_r( &dir_entry_list_head, MyFile_GetPhotoSaveDirName() , 
		   "nand2:/photo" , MyFile_GetPhotoLogFileName(),0 ) ) {

    // PrintDirEntryListBackward( dir_entry_list_head, NULL );
    mydata.num_of_photo_files = SaveDirEntryList( dir_entry_list_head, MyFile_GetPhotoListFileName() );
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }
  else {
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    return FALSE;
  }
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

  mprintf("App. save data backup        ");
  if( 0 == find_title_save_data( &dir_entry_list_head, MyFile_GetAppDataSaveDirName(), "nand:/title",
				 &save_dir_info, MyFile_GetAppDataLogFileName(),0 ) ) {
    // PrintDirEntryListBackward( dir_entry_list_head, NULL );
    mydata.num_of_app_save_data = SaveDirEntryList( dir_entry_list_head , MyFile_GetAppDataListFileName() );
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }
  else {
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    return FALSE;
  }

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
  BOOL flag = TRUE;

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
  OS_TPrintf("User title list backup \n");
  mprintf("User title list backup       ");
  if( 0 == get_title_id( &dir_entry_list_head, "nand:/title", &save_dir_info, 
			 MyFile_GetDownloadTitleIDLogFileName(), 0 ) ) {

    GetDirEntryList( dir_entry_list_head, &pBuffer, &count);

    ptr = pBuffer;
    mydata.num_of_user_download_app = count;
    
    if( ptr != NULL && count != 0 )  {
      for( j = 0 ; j < count ; j++ ) {
	OS_TPrintf("No. %d 0x%016llx\n",j,*ptr);
	mfprintf(tc[2],"No. %d 0x%016llx\n",j,*ptr);
	ptr++;
      }
    }
    PrintSrcDirEntryListBackward( dir_entry_list_head, NULL );

    if( TRUE == TitleIDSave( MyFile_GetDownloadTitleIDFileName(), pBuffer, count, NULL) ) {
      //MyFile_GetDownloadTitleIDLogFileName() 
      m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
      mprintf("OK.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    }
    else {
      m_set_palette(tc[0], M_TEXT_COLOR_RED );
      mprintf("NG.(save ids)\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
      flag = FALSE;
      // return FALSE;
    }
    if( pBuffer ) {
      OS_Free(pBuffer);
    }
  }
  else {
    m_set_palette(tc[0], 0x1);	/* red  */
    mprintf("NG.(get ids)\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    flag = FALSE;
    // return FALSE;
  }
  (void)ClearDirEntryList( &dir_entry_list_head );

  return flag;
}

static BOOL SDBackupToSDCard8(void)
{
  RTCDate rtc_date;
  RTCTime rtc_time;

  /* オリジナルのデータのバックアップ */
  if( TRUE == CheckShopRecord( hws_info.region, NULL ) ) {
    mydata.shop_record_flag = TRUE;
    OS_TPrintf("shop record exist - you don't have to connect the network.\n");
    mprintf(" (--shop record exist--)\n");
  }
  else {
    mydata.shop_record_flag = FALSE;
    OS_TPrintf("no shop record\n - you don't have to connect the network.\n");
    mprintf(" (--no shop record--)\n");
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

  mprintf("Personal data backup         ");
  if( TRUE == MydataSaveEncrypt( MyFile_GetGlobalInformationFileName(), (void *)&mydata, sizeof(MyData), NULL) ) {
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }
  else {
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    return FALSE;
  }
  return TRUE;
}


typedef BOOL (*function_ptr)(void);

static function_ptr function_table[] =
{
  //  SDBackupToSDCard1,
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
  BOOL flag;
  OSMessage message;
  while( 1 ) {
    (void)OS_ReceiveMessage(&MyMesgQueue, &message, OS_MESSAGE_BLOCK);
    flag = TRUE;
    MyFile_SetPathBase("sdmc:/");
    MyFile_AddPathBase((const char *)hws_info.serialNo );
    MyFile_AddPathBase("/");
    for( function_counter = 0 ; function_counter < function_table_max ; function_counter++ ) {
      if( FALSE == (function_table[function_counter])() ) {
	flag = FALSE;
      }
    }
    mprintf("\n");
    if( flag == TRUE ) {
      m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
      mprintf("Backup completed.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    }
    else {
      m_set_palette(tc[0], M_TEXT_COLOR_RED );
      mprintf("Backup failed.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    }
    mprintf("\n");
    if( TRUE == stream_is_play1_end() ) {
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

  m_set_palette(tc[0], M_TEXT_COLOR_LIGHTBLUE );
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



  // 不要：NAM の初期化
  //  NAM_Init(&AllocForNAM, &FreeForNAM);
  
  // 必須：ES の初期化
  ES_InitLib();


  if( RTC_RESULT_SUCCESS != RTC_GetDate( &rtc_date ) ) {
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("rtc date read error.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );

  }
  if( RTC_RESULT_SUCCESS != RTC_GetTime( &rtc_time ) ) {
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("rtc time read error.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }

  m_set_palette(tc[0], M_TEXT_COLOR_LIGHTBLUE );
  mprintf("Unique ID:\n");
  m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  if( FALSE == MiyaReadHWNormalInfo( &hwn_info ) ) {
    m_set_palette(tc[0], 0x1);	/* red  */
    mprintf(" read error.\n");
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
  }
  mprintf("\n");

  
  //  mprintf("HW Secure Info. read ");
  m_set_palette(tc[0], M_TEXT_COLOR_LIGHTBLUE );
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
  m_set_palette(tc[0], M_TEXT_COLOR_LIGHTBLUE );
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


    mfprintf(tc[1], "function no.%d/%d\n", function_counter, function_table_max);
    
    loop_counter++;

  }
  OS_Terminate();
}


/*====== End of main.c ======*/

