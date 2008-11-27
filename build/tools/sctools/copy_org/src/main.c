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
#include <twl/sea.h>
#include <twl/lcfg.h>
#include <twl/na.h>
#include <twl/nam.h>

#include "ecdl.h"

#include        "font.h"
#include        "text.h"
#include        "mprintf.h"
#include        "gfx.h"
#include        "key.h"
#include        "my_fs_util.h"
#include        "mynvram.h"
#include        "stream.h"
#include        "hwi.h"
#include        "mywlan.h"
#include        "mydata.h"
#include        "nuc.h"
#include        "miya_mcu.h"
#include        "error_report.h"

#include        "myfilename.h"
#include        "menu_version.h"

//================================================================================
#define LCD_UPPER_LOWER_FLIP 1

static BOOL completed_flag = FALSE;
static FSEventHook  sSDHook;
static BOOL sd_card_flag = FALSE;
static BOOL no_sd_clean_flag = FALSE;

static BOOL development_console_flag = FALSE;

//static int miya_debug_level = 0;


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
      mprintf("press A button to start BACKUP\n");
    }
  }
}


static MyData mydata;

#ifdef LCD_UPPER_LOWER_FLIP
static int vram_num_main = 1;
static int vram_num_sub = 0;
#else
static int vram_num_main = 1;
static int vram_num_sub = 0;
#endif

static  LCFGTWLHWNormalInfo hwn_info;
static  LCFGTWLHWSecureInfo hws_info;

#define	MY_STACK_SIZE  (1024*16) /* でかいほうがいい */
#define	MY_THREAD_PRIO        20
static OSThread MyThread;
static u64 MyStack[MY_STACK_SIZE / sizeof(u64)];
static void MyThreadProc(void *arg);

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

static BOOL start_my_thread(void)
{
  OSMessage message;
  if( TRUE == OS_ReceiveMessage(&MyMesgQueue_response, &message, OS_MESSAGE_NOBLOCK) ) {
    (void)OS_SendMessage(&MyMesgQueue_request, (OSMessage)0, OS_MESSAGE_NOBLOCK);
    return TRUE;
  }
  return FALSE;
}



static BOOL SDBackupToSDCard2(void)
{
  /* Wifi設定の保存 */
  mprintf("WirelessLAN param. backup    ");
  if( TRUE == nvram_backup( MyFile_GetWifiParamFileName() ) ) {
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    mydata.wireless_lan_param_flag = TRUE;
  }
  else {
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    mydata.wireless_lan_param_flag = FALSE;
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
    mydata.user_settings_flag = TRUE;
  }
  else {
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    mydata.user_settings_flag = FALSE;
    return FALSE;
  }
  return TRUE;
}

static BOOL SDBackupToSDCard4(void)
{
  MY_DIR_ENTRY_LIST *dir_entry_list_head = NULL;
  int save_dir_info = 0;
  BOOL ret_flag = TRUE;
  int list_count;
  int error_count;

  /*
    nand:/shared2ディレクトリまわりの保存
    内容はアプリケーション共有ファイル
    nand:/shared2/*
  */

  Error_Report_Init();


  mprintf("App. shared files backup     ");
  if( 0 == copy_r( &dir_entry_list_head, MyFile_GetAppSharedSaveDirName() , 
		   "nand:/shared2" , MyFile_GetAppSharedLogFileName(), 0) ) {
    // PrintDirEntryListBackward( dir_entry_list_head, NULL );
    if( TRUE == SaveDirEntryList( dir_entry_list_head, MyFile_GetAppSharedListFileName(), 
				  &list_count, &error_count, MyFile_GetAppSharedSaveLogFileName()) ) {
      mydata.num_of_shared2_files = list_count;
      mydata.num_of_error_shared2_files = error_count;

      if( error_count == 0 ) {
	m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
	mprintf("OK.\n");
	m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
      }
      else {
	ret_flag = FALSE;
      }
    }
    else {
      mydata.num_of_shared2_files = -1; /* failed */
      mydata.num_of_error_shared2_files = -1;
      ret_flag = FALSE;
    }
  }
  else {
    mydata.num_of_shared2_files = -1; /* failed */
    mydata.num_of_error_shared2_files = -1;
    ret_flag = FALSE;
  }

  if( ret_flag == FALSE ) {
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }

  (void)ClearDirEntryList( &dir_entry_list_head );
  if( TRUE == Error_Report_Display(tc[0]) ) {
    mprintf("\n");
  }
  Error_Report_End();

  return ret_flag;
}


static BOOL SDBackupToSDCard5(void)
{
  MY_DIR_ENTRY_LIST *dir_entry_list_head = NULL;
  int save_dir_info = 0;
  BOOL ret_flag = TRUE;
  int list_count;
  int error_count;

  /* 
     nand2:/photoディレクトリまわりの保存
     内容は写真長のJPEGファイル
     nand2:/photo/*.*
   */
  Error_Report_Init();

  mprintf("Photo files backup           ");
  if( 0 == copy_r( &dir_entry_list_head, MyFile_GetPhotoSaveDirName() , 
		   "nand2:/photo" , MyFile_GetPhotoLogFileName(),0 ) ) {

    // PrintDirEntryListBackward( dir_entry_list_head, NULL );
    if( TRUE == SaveDirEntryList( dir_entry_list_head, MyFile_GetPhotoListFileName(),
				  &list_count, &error_count, MyFile_GetPhotoSaveLogFileName() ) ) {
      mydata.num_of_photo_files = list_count;
      mydata.num_of_error_photo_files = error_count;
      if( error_count > 0 ) {
	ret_flag = FALSE;
      }
    }
    else {
      mydata.num_of_photo_files = -1; /* failed */
      mydata.num_of_error_photo_files = -1; /* failed */
      ret_flag = FALSE;
    }
  }
  else {
    mydata.num_of_photo_files = -1; /* failed */
    mydata.num_of_error_photo_files = -1; /* failed */
    ret_flag = FALSE;
  }

  if( ret_flag == FALSE ) {
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }
  else {
    m_set_palette(tc[0], 0x2);	/* green  */
    mprintf("OK.\n");
    m_set_palette(tc[0], 0xF);	/* white */

  }
  (void)ClearDirEntryList( &dir_entry_list_head );
  if( TRUE == Error_Report_Display(tc[0]) ) {
    mprintf("\n");
  }
  Error_Report_End();


  return ret_flag;
}

static BOOL SDBackupToSDCard6(void)
{
  MY_DIR_ENTRY_LIST *dir_entry_list_head = NULL;
  int save_dir_info = 0;
  BOOL ret_flag = TRUE;

  int list_count;
  int error_count;


  Error_Report_Init();

  /* nand:/ticketはチケット同期？でうまいこと合わせてくれるんでバックアップ不要 */

  /* 
     nand:/titleディレクトリまわりの保存
     nand:/title/*.savファイルをすべてバックアップ
  */

  mprintf("App. save data backup        ");
  if( 0 == find_title_save_data( &dir_entry_list_head, MyFile_GetSaveDataSaveDirName(), "nand:/title",
				 &save_dir_info, MyFile_GetSaveDataLogFileName(),0 ) ) {
    // PrintDirEntryListBackward( dir_entry_list_head, NULL );
    //    mydata.num_of_app_save_data = SaveDirEntryList( dir_entry_list_head , 
    //						    mydata.num_of_app_save_data = 


    if( TRUE == SaveDirEntryList( dir_entry_list_head , MyFile_GetSaveDataListFileName(),
				  &list_count, &error_count, MyFile_GetSaveDataSaveLogFileName() ) ) {
      mydata.num_of_app_save_data = list_count;
      mydata.num_of_error_app_save_data = error_count;
      if( error_count > 0 ) {
	ret_flag = FALSE;
      }
    }
    else {
      mydata.num_of_app_save_data = -1;
      mydata.num_of_error_app_save_data = -1;
      ret_flag = FALSE;
    }
  }
  else {
    mydata.num_of_app_save_data = -1;
    mydata.num_of_error_app_save_data = -1;
    ret_flag = FALSE;
  }

  if( ret_flag == TRUE ) {
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }
  else {
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }


  (void)ClearDirEntryList( &dir_entry_list_head );

  if( TRUE == Error_Report_Display(tc[0]) ) {
    mprintf("\n");
  }
  Error_Report_End();

  return ret_flag;
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

  Error_Report_Init();

  OS_TPrintf("User title list backup \n");
  mprintf("User title list backup       ");
  if( 0 == get_title_id( &dir_entry_list_head, "nand:/title", &save_dir_info, 
			 MyFile_GetDownloadTitleIDLogFileName(), 0 ) ) {

    flag = GetUserAppTitleList( dir_entry_list_head, &pBuffer, &count, 
				MyFile_GetUserAppTitleListLogFileName()) ;

    if( TRUE == flag ) {
      ptr = pBuffer;
      mydata.num_of_user_download_app = count;
      mydata.num_of_error_user_download_app = 0;
      
      if( ptr != NULL && count != 0 )  {
	for( j = 0 ; j < count ; j++ ) {
	  OS_TPrintf("No. %d 0x%016llx\n",j,*ptr);
	  mfprintf(tc[2],"No. %d 0x%016llx\n",j,*ptr);
	  ptr++;
	}
      }
      
      // PrintSrcDirEntryListBackward( dir_entry_list_head, NULL );
      flag =  TitleIDSave( MyFile_GetDownloadTitleIDFileName(), 
			   pBuffer, count,  MyFile_GetDownloadTitleIDSaveLogFileName());
    }
    if( pBuffer ) {
      OS_Free(pBuffer);
    }
  }

  if( flag == TRUE ) {
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }
  else {
    m_set_palette(tc[0], 0x1);	/* red  */
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    mydata.num_of_user_download_app = -1; /* failed */
    mydata.num_of_error_user_download_app = -1;
  }
  (void)ClearDirEntryList( &dir_entry_list_head );

  if( TRUE == Error_Report_Display(tc[0]) ) {
    mprintf("\n");
  }
  Error_Report_End();

  return flag;
}

static BOOL SDBackupToSDCard8(void)
{
  RTCDate rtc_date;
  RTCTime rtc_time;

  /* オリジナルのデータのバックアップ */
  if( RTC_RESULT_SUCCESS != RTC_GetDate( &rtc_date ) ) {
    mprintf("rtc read date error.\n");
    mydata.rtc_date_flag = FALSE;
  }
  else {
    mydata.rtc_date_flag = TRUE;
  }
  if( RTC_RESULT_SUCCESS != RTC_GetTime( &rtc_time ) ) {
    mprintf("rtc read time error.\n");
    mydata.rtc_time_flag = FALSE;
  }
  else {
    mydata.rtc_time_flag = TRUE;
  }

  STD_CopyMemory( (void *)&(mydata.rtc_date), (void *)&rtc_date, sizeof(RTCDate) );
  STD_CopyMemory( (void *)&(mydata.rtc_time), (void *)&rtc_time, sizeof(RTCTime) );
  STD_CopyMemory( (void *)(mydata.movableUniqueID), (void *)hwn_info.movableUniqueID,
		  LCFG_TWL_HWINFO_MOVABLE_UNIQUE_ID_LEN );

  mprintf("Personal data backup         ");
  /* こいつはフラグを立てられない、セーブできない。あたりまえ */
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
  OSMessage message;
  BOOL flag;
  BOOL twl_card_validation_flag;

  while( 1 ) {
    (void)OS_SendMessage(&MyMesgQueue_response, (OSMessage)0, OS_MESSAGE_NOBLOCK);
    (void)OS_ReceiveMessage(&MyMesgQueue_request, &message, OS_MESSAGE_BLOCK);

    if( no_sd_clean_flag == FALSE ) {
      mprintf("cleaning up SD card.. ");
      (void)CleanSDCardFiles(NULL);
      mprintf("done.\n");
    }

    flag = TRUE;
    twl_card_validation_flag = TRUE;
    MyFile_SetPathBase("sdmc:/");
    MyFile_AddPathBase((const char *)hws_info.serialNo );
    MyFile_AddPathBase("/");

    (void)CopyFile(MyFile_GetProductLogFileName(), "nand:/sys/log/"MY_FILE_NAME_PRODUCT_LOG , NULL);
    (void)CopyFile(MyFile_GetSystemMenuLogFileName(), "nand:/sys/log/"MY_FILE_NAME_SYSMENU_LOG , NULL);
    (void)CopyFile(MyFile_GetShopLogFileName(), "nand:/sys/log/"MY_FILE_NAME_SHOP_LOG , NULL);


    for( function_counter = 0 ; function_counter < function_table_max ; function_counter++ ) {
      if( FALSE == (function_table[function_counter])() ) {
	flag = FALSE;
      }
    }
    mprintf("\n");
    if( flag == TRUE ) {
      completed_flag = TRUE;
      if( TRUE == stream_play_is_end() ) {
	stream_play0();
      }

      /* ＴＷＬカード、ＳＤカード抜いたかをチェックする */
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
	  mprintf("Backup completed.\n");
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
      mprintf("Backup failed.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
      if( TRUE == stream_play_is_end() ) {
	stream_play2(); /* ng.aiff */
      }
      Gfx_Set_BG1_Color((u16)M_TEXT_COLOR_DARKRED);
    }
    mprintf("\n");
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
  int n;
  u8 macAddress[6];
  u16 s_major, s_minor;
  u32 s_timestamp;
  ESError es_error_code;
  int select_mode = 0;
  u16 BatterylevelBuf = 0;
  BOOL isAcConnectedBuf = FALSE;

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

  MIYA_MCU_Init();

  OS_TPrintf("MCU Free Reg. 0x%02x\n",MCU_GetFreeReg());


  STD_MemSet((void *)&mydata, 0, sizeof(MyData));


  if( FALSE == Read_SystemMenuVersion(&s_major, &s_minor, &s_timestamp) ) {
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf( "system menu ver. read error!\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    s_major = 0;
    s_minor = 0;
    s_timestamp = 0;
    mydata.sys_ver_flag = FALSE;
  }
  else {
    mydata.sys_ver_flag = TRUE;
    mydata.sys_ver_major = s_major;
    mydata.sys_ver_minor = s_minor;
  }

  development_console_flag = IsThisDevelopmentConsole();
  if(TRUE == development_console_flag ) {
    mprintf("--development console--\n");
    m_set_palette(tc[0], M_TEXT_COLOR_PINK );
    m_set_palette(tc[0], M_TEXT_COLOR_PURPLE );
    m_set_palette(tc[0], M_TEXT_COLOR_ORANGE );
    m_set_palette(tc[0], M_TEXT_COLOR_BROWN );
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }


  // 不要：NAM の初期化
  //  NAM_Init(&AllocForNAM, &FreeForNAM);
  
  // 必須：ES の初期化
  ES_InitLib();


  if( FALSE == MiyaReadHWNormalInfo( &hwn_info ) ) {
    mydata.uniqueid_flag = FALSE;
    m_set_palette(tc[0], 0x1);	/* red  */
    mprintf("HW Normal Info. read error.\n");
    m_set_palette(tc[0], 0xF);
  }
  else {
    mydata.uniqueid_flag = TRUE;
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

  // typedef s32 ESError;
  // ESError ES_GetDeviceId(ESId* devId);
  /*
    #define ES_ERR_OK                                   0
    #define ES_ERR_INCORRECT_CERT_TYPE                  -1001
    #define ES_ERR_GENKEY_FAILED                        -1002
    #define ES_ERR_VERIFY_SIG_FAILED                    -1003
    #define ES_ERR_CANNOT_OPEN_FILE                     -1004
    #define ES_ERR_INCORRECT_PUBKEY_TYPE                -1005
    #define ES_ERR_CERT_ISSUER_MISMATCH                 -1006
    #define ES_ERR_ENCRYPTION_FAILED                    -1007
    #define ES_ERR_FILE_OPEN_FAILED                     -1008
    #define ES_ERR_FILE_READ_FAILED                     -1009
    #define ES_ERR_FILE_WRITE_FAILED                    -1010
    #define ES_ERR_TMD_NUM_CONTENTS                     -1011
    #define ES_ERR_INCORRECT_SIG_TYPE                   -1012
    #define ES_ERR_INCORRECT_SIG_LENGTH                 -1013
    #define ES_ERR_INCORRECT_CERT_LENGTH                -1014
    #define ES_ERR_DEV                                  -1015
    #define ES_ERR_MAXFD                                -1016
    #define ES_ERR_INVALID                              -1017
    #define ES_ERR_FS_CONNECTION_FAILED                 -1018
    #define ES_ERR_UNSUPPORTED_TRANSFER_SOURCE          -1019
    #define ES_ERR_DEVICE_ID_MISMATCH                   -1020
    #define ES_ERR_INCORRECT_CONTENT_SIZE               -1021
    #define ES_ERR_HASH_MISMATCH                        -1022
    #define ES_ERR_INCORRECT_CONTENT_COUNT              -1023
    #define ES_ERR_OUT_OF_MEMORY                        -1024
    #define ES_ERR_NO_TMD_FILE                          -1025
    #define ES_ERR_NO_RIGHT                             -1026
    #define ES_ERR_ISSUER_NOT_FOUND                     -1027
    #define ES_ERR_NO_TICKET                            -1028
    #define ES_ERR_INCORRECT_TICKET                     -1029
    #define ES_ERR_NOT_ENOUGH_SPACE                     -1030
    #define ES_ERR_INCORRECT_BOOT_VERSION               -1031
    #define ES_ERR_UNKNOWN                              -1032
    #define ES_ERR_EXPIRED                              -1033
    #define ES_ERR_UNUSED                               -1034
    #define ES_ERR_INCORRECT_TITLE_VERSION              -1035
    #define ES_ERR_OS_TICKET_NOEXISTS                   -1036
    #define ES_ERR_OS_CONTENT_NOEXISTS                  -1037
    #define ES_ERR_NOT_EMPTY                            -1038
    // Game is a disc/nand game, but a tmd was not supplied by caller
    #define ES_ERR_DISC_NAND_NO_TMD                     -1039
    #define ES_ERR_NOEXISTS                             -106    Does not exist ISFS_ERROR_NOEXISTS

  */

  mydata.shop_record_flag = FALSE;
  es_error_code = ES_GetDeviceId(&mydata.deviceId);
  if( es_error_code == ES_ERR_OK ) {
    if( TRUE == CheckShopRecord( NULL ) ) {
      mydata.shop_record_flag = TRUE;
    }
    else {
      mprintf("no shop record\n");
    }
  }
  else {
    OS_TPrintf("es_error_code = %d\n", es_error_code );
  }
  
  if( TRUE == mydata.shop_record_flag ) {
    snprintf(mydata.bmsDeviceId, sizeof(mydata.bmsDeviceId), "%lld", ((0x3ull << 32) | mydata.deviceId));
    // OS_TPrintf("DeviceID:  %08X %u\n", mydata.deviceId, mydata.deviceId);
    OS_TPrintf("DeviceID: %s\n", mydata.bmsDeviceId);
  }

  mprintf("\n");

  mydata.volume = (s32)MCU_GetVolume();
  mydata.backlight_brightness = (s32)MCU_GetBackLightBrightness();


#if 0 /* いらないらしい */
  OS_TPrintf("vol = %d\n",mydata.volume );
  OS_TPrintf("bright = %d\n", mydata.backlight_brightness);

  MCU_SetVolume( (u8)31 );
  MCU_SetBackLightBrightness( (u8)4 );
#endif

  if( FALSE == SDCardValidation() ) {
    sd_card_flag = FALSE;
    m_set_palette(tc[0], 0x1);	/* red  */
    mprintf("No SD card\n");
  }
  else {
    sd_card_flag = TRUE;
  }
  m_set_palette(tc[0], 0xF);	/* white */


  FS_RegisterEventHook("sdmc", &sSDHook, SDEvents, NULL);

  mydata.version_major = MY_DATA_VERSION_MAJOR;
  mydata.version_minor = MY_DATA_VERSION_MINOR;

  init_my_thread();

  if( sd_card_flag == TRUE ) {

    text_blink_current_line(tc[0]);
    mprintf("press A button to start BACKUP\n\n");
  }
  else {
    text_blink_current_line(tc[0]);
    mprintf("insert SD card\n");
  }


  if( TRUE == stream_play_is_end() ) {
    stream_play0(); /* cursor.aiff */
  }

  while( 1 ) {
    OS_WaitVBlankIntr();
    Gfx_Render( vram_num_main , vram_num_sub );
    (void)RTC_GetTime( &rtc_time );
    (void)RTC_GetDate( &rtc_date );

    keyData = m_get_key_trigger();

    // ＡＲＭ７コマンド応答受信
    while (SND_RecvCommandReply(SND_COMMAND_NOBLOCK) != NULL)
      {
      }
    // コマンドフラッシュ（フラッシュして即座に実行を要求）
    (void)SND_FlushCommand(SND_COMMAND_NOBLOCK | SND_COMMAND_IMMEDIATE);

    if ( keyData & PAD_BUTTON_R ) {
#ifdef LCD_UPPER_LOWER_FLIP
      vram_num_main ^= 1;
      vram_num_sub  ^= 1;
#else
      vram_num_main++;
      if( vram_num_main > (MAX_VRAM_NUM-1) ) {
	vram_num_main = 0;
      }
#endif
    }
    else if ( keyData & PAD_BUTTON_L ) {
#ifdef LCD_UPPER_LOWER_FLIP
      vram_num_main ^= 1;
      vram_num_sub  ^= 1;
#else
      vram_num_main--;
      if( vram_num_main < 0 ) {
	vram_num_main = (MAX_VRAM_NUM-1);
      }
#endif
    }
    else if ( keyData & (PAD_BUTTON_A | PAD_BUTTON_START) ) {
      /* ユーザーデータ吸出しモード */
      if(completed_flag == FALSE ) {
	if( sd_card_flag == TRUE ) {

	  text_blink_clear(tc[0]);
	  if( FALSE == start_my_thread() ) {
	    OS_TPrintf("\nnow backup..\n\n");
	  }
	}
	else {
	  text_blink_clear(tc[0]);
	  text_blink_current_line(tc[0]);
	  mprintf("insert SD card\n");
	}
      }
    }
    else if ( keyData & PAD_BUTTON_B ) {
    }
#if 0
    /* スタートボタンはＡボタンと同じ扱い */
    else if ( keyData & PAD_BUTTON_START ) {
    }
#endif
    else if ( keyData & PAD_BUTTON_SELECT ) {
    }
    else if ( keyData & PAD_BUTTON_X ) {
      select_mode++;
      select_mode %= 3;

      Miya_debug_OFF();
      no_sd_clean_flag = FALSE;

      if( select_mode == 0 ) {
      }
      else if(select_mode == 1 ) {
	no_sd_clean_flag = TRUE;
      }
      else if(select_mode == 2 ) {
	Miya_debug_ON();
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


    m_set_palette(tc[1], M_TEXT_COLOR_LIGHTBLUE );
    mfprintf(tc[1], "\fRepaire Tool BACKUP");
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
      //      mfprintf(tc[1],"none.\n");
      m_set_palette(tc[1], M_TEXT_COLOR_YELLOW );
      mfprintf(tc[1],"-----------\n");
      m_set_palette(tc[1], M_TEXT_COLOR_WHITE );
    }
    mfprintf(tc[1],"\n");

    if( (loop_counter % 60) == 0 ) {
      // PM_RESULT_SUCCESS 
      (void)PM_GetACAdapter( &isAcConnectedBuf );
      (void)PM_GetBatteryLevel( &BatterylevelBuf );
    }

    mfprintf(tc[1], "%4d/%02d/%02d %02d:%02d:%02d ",
	     rtc_date.year + 2000, rtc_date.month , rtc_date.day,
	     rtc_time.hour , rtc_time.minute , rtc_time.second );

    if( isAcConnectedBuf == TRUE ) {
      m_set_palette(tc[1], M_TEXT_COLOR_BLUE );
      mfprintf(tc[1], "AC.        \n\n");
      m_set_palette(tc[1], M_TEXT_COLOR_WHITE );
    }
    else {
      mfprintf(tc[1], "Batt.Lv ");
      switch( BatterylevelBuf ) {
      case 0:
      case 1:
	m_set_palette(tc[1], M_TEXT_COLOR_RED );
	break;
      case 2:
      case 3:
      case 4:
	m_set_palette(tc[1], M_TEXT_COLOR_YELLOW );
	break;
      default:
	m_set_palette(tc[0], M_TEXT_COLOR_GREEN );
	break;
      }
      mfprintf(tc[1], "%d/5\n\n" , BatterylevelBuf); 
      m_set_palette(tc[1], M_TEXT_COLOR_WHITE );
    }


    if( select_mode == 0 ) {
      mfprintf(tc[1],"-- normal mode(default) --\n");
    }
    else if( select_mode == 1 ) {
      m_set_palette(tc[1], M_TEXT_COLOR_YELLOW );
      mfprintf(tc[1],"-- no SDCard clean mode --\n");
      m_set_palette(tc[1], M_TEXT_COLOR_WHITE );
    }
    else if( select_mode == 2 ) {
      m_set_palette(tc[1], M_TEXT_COLOR_RED ); /* red  */
      mfprintf(tc[1],"-- NG mode --\n");
      m_set_palette(tc[1], M_TEXT_COLOR_WHITE );
    }

    mfprintf(tc[1],"\n");

    mfprintf(tc[1], "function no.%d/%d\n", function_counter, function_table_max);
    
    loop_counter++;

  }
  OS_Terminate();
}


/*====== End of main.c ======*/

