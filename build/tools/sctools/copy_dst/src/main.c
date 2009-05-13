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

#include <NitroWiFi/nhttp.h>
#include <NitroWiFi/ncfg.h>

#include </twl/os/common/ownerInfoEx.h>

#include "nssl.h"

#include        "font.h"
#include        "text.h"
#include        "mprintf.h"
#include        "logprintf.h"

#include        "gfx.h"
#include        "key.h"
#include        "my_fs_util.h"
#include        "mynvram.h"
#include        "stream.h"
#include        "hwi.h"
#include        "hatamotolib.h"
#include        "mywlan.h"
#include        "mydata.h"
#include        "netconnect.h"
#include        "sitedefs.h"
#include        "wcm_control.h"
#include        "nuc.h"
#include        "mynuc.h"
#include        "miya_mcu.h"
#include        "error_report.h"

#include        "myfilename.h"
#include        "mfiler.h"
#include        "menu_version.h"

#include        "ntp.h"
#include        "myimport.h"
#include        "pre_install.h"


// #define PRE_INSTALL 1

//================================================================================
#define THREAD_COMMAND_NUP_FUNCTION               0
#define THREAD_COMMAND_FULL_FUNCTION              1
#define THREAD_COMMAND_WIFI_FUNCTION              2
#define THREAD_COMMAND_USERDATA_AND_WIFI_FUNCTION 3
#define THREAD_COMMAND_REBOOT_FUNCTION            4


#define FREE_REG_POWERON_REBOOT 0x00
#define FREE_REG_RESTOREMODE_REBOOT 0x55
#define FREE_REG_RESTOREMODE_AND_RTC_DONE_REBOOT 0x66


static BOOL only_wifi_config_data_trans_flag = FALSE;
static BOOL user_and_wifi_config_data_trans_flag = FALSE;
static BOOL no_network_flag = FALSE;
static BOOL completed_flag = FALSE;
static FSEventHook  sSDHook;
static BOOL sd_card_flag = FALSE;

static BOOL wlan_active_flag = TRUE;

static BOOL development_console_flag = FALSE;

static u8 org_region = 0;
static u64 org_fuseId = 0;
static volatile BOOL reboot_flag;
static BOOL reboot_rtc_adjust_done_flag;

static int miya_debug_level = 0;

static OSHeapHandle hHeap;
static u32 allocator_total_size = 0;

static MY_USER_APP_TID *title_id_buf_ptr = NULL;
static int title_id_count = 0;


static u8 WorkForNA[NA_VERSION_DATA_WORK_SIZE];



static BOOL pushed_power_button = FALSE;
static BOOL throw_pushed_power_button = FALSE;

static PMExitCallbackInfo pmexitcallbackinfo;

static void pmexitcallback(void *arg)
{
#pragma unused(arg)
  /* 処理中（データ書き込み中）ならリセットをブロックしなければならない */
  pushed_power_button = TRUE;
}


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
//#define	MY_THREAD_PRIO        20

#define	MY_THREAD_PRIO        10

static OSThread MyThread;
static u64 MyStack[MY_STACK_SIZE/sizeof(u64)];
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
		  NULL, MyStack + MY_STACK_SIZE /sizeof(u64),
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


static BOOL start_my_thread(u32 command)
{
  OSMessage message;
  if( TRUE == OS_ReceiveMessage(&MyMesgQueue_response, &message, OS_MESSAGE_NOBLOCK) ) {
    (void)OS_SendMessage(&MyMesgQueue_request, (OSMessage)command, OS_MESSAGE_BLOCK);
    return TRUE;
  }
  return FALSE;
}


/* 
   
   タイトルIDリスト
   「TitleID_Hi」_「TitleID_Lo」
   ------------------------------------------
   システムアプリ
   　　00030017_484E41?? [HNA?]    DSiメニュー
   　　00030015_484E42?? [HNB?]    本体設定
   　　00030015_484E46?? [HNF?]    DSiショップ
   　　0003000F_484E4341 [HNCA]    無線ファーム
   　　0003000F_484E4841 [HNHA]    ホワイトリスト
   　　0003000F_484E4C?? [HNL?]    バージョン情報
   　　00030005_484E4441 [HNDA]    ダウンロードプレイ
   　　00030005_484E4541 [HNEA]    PictChat
   　　00030005_484E49?? [HNI?]    DSiカメラ
   　　00030005_484E4A?? [HNJ?]    ニンテンドーゾーンビューア
   　　00030005_484E4B?? [HNK?]    DSiサウンド
   
   ユーザーアプリ
   　　00030004_484E47?? [HNG?]    DSiブラウザ
   　　00030004_4B4755?? [KGU?]    うごくメモ帳
   
   TitleID_Loの４桁目（言語コード）
   　　?=[J, E, P, U]=[4A, 45, 50, 55]
   
   アプリのインストールされるパス
   　　"nand:/title/<titleID_Hi>/<titleID_Lo>"
   
*/

static void RTC_NTP_SYNC(void)
{
  RTCDate rtc_date;
  RTCTime rtc_time;
  
  if( TRUE == my_ntp_init() ) {
    rtc_date.year = (u32)( my_ntp_get_year() - 2000 );
    rtc_date.month = (u32)(my_ntp_get_month() + 1 );
    rtc_date.day = (u32)my_ntp_get_day();
    rtc_date.week = (RTCWeek)my_ntp_get_weekday();

    rtc_time.hour = (u32)my_ntp_get_hour();
    rtc_time.minute = (u32)my_ntp_get_min();
    rtc_time.second = (u32)my_ntp_get_sec();

    //RTCDate rtc_date;
    //RTCTime rtc_time;
    /*
      typedef enum RTCWeek
      {
      RTC_WEEK_SUNDAY = 0,               // 日曜日
      RTC_WEEK_MONDAY,                   // 月曜日
      RTC_WEEK_TUESDAY,                  // 火曜日
      RTC_WEEK_WEDNESDAY,                // 水曜日
      RTC_WEEK_THURSDAY,                 // 木曜日
      RTC_WEEK_FRIDAY,                   // 金曜日
      RTC_WEEK_SATURDAY,                 // 土曜日
      RTC_WEEK_MAX
      }
      RTCWeek;

      typedef struct RTCDate
      {
      u32     year;                      // 年 ( 0 ~ 99 )
      u32     month;                     // 月 ( 1 ~ 12 )
      u32     day;                       // 日 ( 1 ~ 31 )
      RTCWeek week;                      // 曜日
	  
      }
      RTCDate;
	  
      // 時刻構造体
      typedef struct RTCTime
      {
      u32     hour;                      // 時 ( 0 ~ 23 )
      u32     minute;                    // 分 ( 0 ~ 59 )
      u32     second;                    // 秒 ( 0 ~ 59 )
	  
      }
      RTCTime;
    */
	
    if( RTC_RESULT_SUCCESS != RTC_SetDate( &rtc_date ) ) {
      mprintf("RTC data adjust error\n");
    }
    else {
      mprintf("RTC data adjust success\n");
    }

    if( RTC_RESULT_SUCCESS != RTC_SetTime( &rtc_time ) ) {
      mprintf("RTC time adjust error\n");
    }
    else {
      mprintf("RTC time adjust success\n");
    }
    reboot_rtc_adjust_done_flag = TRUE;
  }
  else {
    mprintf("RTC-NTP sync. NG\n");
  }
}



static BOOL LoadWlanConfig(void)
{
  u8 buf[256];
  int len;
  int i;  /* ユーザーデータ書き込みモード */
  if( TRUE == LoadWlanConfigFile("sdmc:/wlan_cfg.txt") ) {
    OS_TPrintf("SSID = %s\n", GetWlanSSID()); 
    OS_TPrintf("MODE = ");
#if 0
    mfprintf(tc[3],"SSID = %s\n", GetWlanSSID()); 
    mfprintf(tc[3],"MODE = ");
#endif
    switch( GetWlanMode() ) {
    case WCM_WEPMODE_NONE:
      OS_TPrintf("NONE\n");
      //      mfprintf(tc[3],"NONE\n");
      break;
    case WM_WEPMODE_40BIT:
      OS_TPrintf("WEP40\n");
      //      mfprintf(tc[3],"WEP128\n");
      break;
    case WM_WEPMODE_104BIT:
      OS_TPrintf("WEP104\n");
      //      mfprintf(tc[3],"WEP128\n");
      break;
    case WM_WEPMODE_128BIT:
      OS_TPrintf("WEP128\n");
      //      mfprintf(tc[3],"WEP128\n");
      break;
    case WCM_WEPMODE_WPA_TKIP:
      OS_TPrintf("WPA-TKIP\n");
      //      mfprintf(tc[3],"WPA-TKIP\n");
      break;
    case WCM_WEPMODE_WPA2_TKIP:
      OS_TPrintf("WPA2-TKIP\n");
      //      mfprintf(tc[3],"WPA2-TKIP\n");
      break;
    case WCM_WEPMODE_WPA_AES:
      OS_TPrintf("WPA-AES\n");
      //      mfprintf(tc[3],"WPA-AES\n");
      break;
    case WCM_WEPMODE_WPA2_AES :
      OS_TPrintf("WPA2-AES\n");
      //      mfprintf(tc[3],"WPA2-AES\n");
      break;
    defalut:
      OS_TPrintf("Unknow mode..\n");
      //      mfprintf(tc[3],"Unknow mode..\n");
      break;
    }

    if( TRUE == GetKeyModeStr() ) {
      OS_TPrintf("KEY STR = %s\n", GetWlanKEYSTR());
      //      mfprintf(tc[3],"KEY STR = %s\n", GetWlanKEYSTR());
    }
    else {
      len = GetWlanKEYBIN(buf);
      if( len ) {
	OS_TPrintf("KEY BIN = 0x");
	//	mfprintf(tc[3],"KEY BIN = 0x");
	for( i = 0 ; i < len ; i++ ) {
	  OS_TPrintf("%02X",buf[i]);
	  //	  mfprintf(tc[3],"%02X",buf[i]);
	}
	OS_TPrintf("\n");
	//	mfprintf(tc[3],"\n");
      }
    }
    //    mfprintf(tc[3],"\n");

    if( TRUE == GetDhcpMODE() ) {
      //      mfprintf(tc[3],"DHCP client\n");
    }
#if 0
    else {
      u32 addr_temp;
      addr_temp = GetIPAddr();
      mfprintf(tc[3],"IP addr %d.%d.%d.%d\n", (u32)((addr_temp >> 24) & 0xff),(u32)((addr_temp >> 16) & 0xff),
	       (u32)((addr_temp >> 8) & 0xff),(u32)(addr_temp & 0xff) );

      addr_temp = GetNetmask();
      mfprintf(tc[3],"netmask %d.%d.%d.%d\n", (u32)((addr_temp >> 24) & 0xff),(u32)((addr_temp >> 16) & 0xff),
	       (u32)((addr_temp >> 8) & 0xff),(u32)(addr_temp & 0xff) );

      addr_temp = GetGateway();
      mfprintf(tc[3],"gateway %d.%d.%d.%d\n", (u32)((addr_temp >> 24) & 0xff),(u32)((addr_temp >> 16) & 0xff),
	       (u32)((addr_temp >> 8) & 0xff),(u32)(addr_temp & 0xff) );

      addr_temp = GetDNS1();
      mfprintf(tc[3],"DNS1    %d.%d.%d.%d\n", (u32)((addr_temp >> 24) & 0xff),(u32)((addr_temp >> 16) & 0xff),
	       (u32)((addr_temp >> 8) & 0xff),(u32)(addr_temp & 0xff) );

      addr_temp = GetDNS2();
      mfprintf(tc[3],"DNS2    %d.%d.%d.%d\n", (u32)((addr_temp >> 24) & 0xff),(u32)((addr_temp >> 16) & 0xff),
	       (u32)((addr_temp >> 8) & 0xff),(u32)(addr_temp & 0xff) );
    }
    mfprintf(tc[3],"\n");
#endif

  }
  else {
    OS_TPrintf("Invalid wlan cfg file\n");
    //    mfprintf(tc[3],"Invalid wlan cfg file\n");
    mprintf("Invalid wlan cfg file\n");
    return FALSE;
  }
  return TRUE;
}

static BOOL RestoreFromSDCard1(void)
{
  BOOL flag = TRUE;
  /* 基本的にこいつらの逆をやる(Get -> Set, Save->Loadなど) */
  /* 
     ちなみにすでにMydataLoad関数は成功しているものとする。
     したがって MyData mydata にはデータが入っている。
  */

  if( reboot_rtc_adjust_done_flag == TRUE ) {
    mprintf("RTC-NTP sync. already done.  ");
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }
  else if( (mydata.rtc_date_flag == TRUE) && (mydata.rtc_time_flag == TRUE) ) {
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
  }
  else {
    //    mprintf("RTC data restore             ");
    mprintf("No original RTC data\n");
    flag = TRUE;
  }

  return flag;
}

static BOOL RestoreFromSDCard2(void)
{
  if( mydata.uniqueid_flag == TRUE ) {
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
  mprintf("No original Unique ID\n");

  return TRUE;

}


static BOOL RestoreFromSDCard3(void)
{
  // static BOOL SDBackupToSDCard2(void)
  BOOL flag;
  if( mydata.wireless_lan_param_flag == TRUE ) {
    mprintf("WirelessLAN param. restore   ");
    flag = nvram_restore( MyFile_GetWifiParamFileName() );
    if( flag == 3 ) {
      m_set_palette(tc[0], M_TEXT_COLOR_GREEN );
      mprintf("OK*\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    }
    else if( TRUE == flag ) {
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
  }
  else {
    mprintf("no original WirelessLAN param.\n");
  }

  return TRUE;
}

static BOOL RestoreFromSDCard4(void)
{
  // static BOOL SDBackupToSDCard3(void)
  if( mydata.user_settings_flag == TRUE ) {
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
  }
  else {
    mprintf("no original user setting param.\n");
  }
  return TRUE;
}

static BOOL RestoreFromSDCard5(void)
{
  // static BOOL SDBackupToSDCard4(void)
  int list_count;
  int error_count;
  BOOL ret_flag = TRUE;

  Error_Report_Init();

  if( mydata.num_of_shared2_files > 0 ) { 
    mprintf("App. shared files restore    ");
    if( TRUE == RestoreDirEntryList( MyFile_GetAppSharedListFileName(), 
				     MyFile_GetAppSharedRestoreLogFileName(), 
				     &list_count, &error_count )) {
      m_set_palette(tc[0], M_TEXT_COLOR_GREEN );
      mprintf("OK.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    }
    else {
      // error
      m_set_palette(tc[0], M_TEXT_COLOR_RED );
      mprintf("NG.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
      ret_flag = FALSE;
    }
  }
  else if( mydata.num_of_shared2_files == 0 ) {
    mprintf("Original device has no shared file\n");
  }
  else {
    mprintf("Original shared files saving failed\n");
  }

  if( TRUE == Error_Report_Display(tc[0]) ) {
    mprintf("\n");
  }
  Error_Report_End();

  return ret_flag;
}

static BOOL RestoreFromSDCard6(void)
{
  int list_count;
  int error_count;
  BOOL ret_flag = TRUE;

  Error_Report_Init();

  if( mydata.num_of_photo_files  > 0 ) { 
    mprintf("Photo files restore          ");
    if( TRUE == RestoreDirEntryList( MyFile_GetPhotoListFileName() , 
				     MyFile_GetPhotoRestoreLogFileName(),
				     &list_count, &error_count )) {
     
      m_set_palette(tc[0], M_TEXT_COLOR_GREEN );
      mprintf("OK.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    }
    else {
      // error
      m_set_palette(tc[0], M_TEXT_COLOR_RED );
      mprintf("NG.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
      ret_flag = FALSE;
    }
  }
  else if( mydata.num_of_photo_files == 0 ) {
    mprintf("Original device has no photo file\n");
  }
  else {
    mprintf("Original photo files saving failed\n");
  }

  if( TRUE == Error_Report_Display(tc[0]) ) {
    mprintf("\n");
  }
  Error_Report_End();

  return ret_flag;
}



static BOOL RestoreFromSDCard7(void)
{
  int i;
  ECError rv;
  BOOL ret_flag = TRUE;
  FSFile *log_fd;
  int ec_download_ret;
  char game_code_buf[5];
  int is_personalized;
  u64 tid;

  title_id_count = 0;
  if( title_id_buf_ptr != NULL ) {
    OS_Free( title_id_buf_ptr );
    title_id_buf_ptr = NULL;
  }

  title_id_count = 0;
  rv = EC_ERROR_OK;

  /* hws_info.serialNoは戻せない */

  log_fd = hatamotolib_log_start( MyFile_GetEcDownloadLogFileName() );


  if( mydata.num_of_user_download_app > 0 ) {
    if( mydata.num_of_user_download_app > 0 ) {
      miya_log_fprintf(log_fd,"User title list loading\n");
      mprintf("User title list load         ");
      if( TRUE == TitleIDLoad( MyFile_GetDownloadTitleIDFileName(), &title_id_buf_ptr, 
			       &title_id_count, MyFile_GetDownloadTitleIDRestoreLogFileName()) ) {
	
	m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
	mprintf("OK.\n");
	m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
	for( i = 0; i < title_id_count ; i++ ) {
	  /* 
	     インストール成功フラグの初期化
	     とりあえず全部失敗状態にする。
	   */
	  title_id_buf_ptr[i].install_success_flag = FALSE;
	  tid = title_id_buf_ptr[i].tid;
	  is_personalized = title_id_buf_ptr[i].is_personalized;

	  (void)my_fs_Tid_To_GameCode(tid, game_code_buf);
	  mprintf(" id %08X %08X [%s] ", (u32)(tid >> 32), (u32)tid, game_code_buf); 
	  if( is_personalized == 1 ) {
	    mprintf("Pre\n");
	  }
	  else {
	    mprintf("Dwn\n");
	  }

	  miya_log_fprintf(log_fd," id %08X %08X [%s] %c\n", (u32)(tid >> 32), (u32)tid, game_code_buf, 
			   (is_personalized == 1)? 'P':'D');
	}
      }
      else {
	ret_flag = FALSE;
	m_set_palette(tc[0], M_TEXT_COLOR_RED );
	mprintf("NG.\n");
	m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
      }
    }
  }
  else if( mydata.num_of_user_download_app == 0 ) {
    miya_log_fprintf(log_fd,"Original device has no user download app.\n");
    mprintf("Original device has no user download app.\n");
  }
  else {
    miya_log_fprintf(log_fd,"Original user download app. list saving failed\n");
    mprintf("Original user download app. list saving failed\n");
  }

    
  /* 
     miya 2009/5/8
     ここで書き込み側に入ってるプリンインストールものを全部消す！
     もっと前でもいい
  */
  if( my_fs_get_print_debug_flag() == FALSE ) {
    (void)pre_install_Cleanup_User_Titles( log_fd );
  }

  if( no_network_flag == TRUE ) {
    miya_log_fprintf(log_fd,"no network flag ON\n");
    goto pre_install_label;
  }


  /*
    EC downloadの後にNAM_ImportをやらないとLoadCertのSEA_Decryptでこける。
    理由はAESエンジンのスロットをNAM_Importでつぶしちゃうから。
   */

  if( mydata.shop_record_flag == FALSE ) {
    /* ネットワークにつながなくていいか？ */
    miya_log_fprintf(log_fd,"no shop record \n");
    mprintf(" (--no shop record--)\n");
  }
  else {
    miya_log_fprintf(log_fd,"EC download\n");
    mprintf("EC download\n");

    miya_log_fprintf(log_fd,"-wireless AP conf. load.. ");
    mprintf("-wireless AP conf. load      ");
    if( TRUE == LoadWlanConfig() ) {
      miya_log_fprintf(log_fd, "OK.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
      mprintf("OK.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );

      /* nand:/ticketはチケット同期でダウンロード */
      // 不要：デバイス情報の表示
      //      PrintDeviceInfo();
      // setup
      // 必須：タイトル ID の偽装
      SetupShopTitleId(); /* エラーはない */
      miya_log_fprintf(log_fd,"SetupShopTitleId\n");


      // ？：ユーザ設定がされていないと接続できないので適当に設定
      // SetupUserInfo();
      // 必須：バージョンデータのマウント
      if( FALSE == SetupVerData() ) {
	miya_log_fprintf(log_fd, "%s failed SetupVerData\n", __FUNCTION__);
	ret_flag = FALSE;
	goto end_log_e;
      }

      // 必須：ネットワークへの接続
      if( 0 != NcStart(SITEDEFS_DEFAULTCLASS) ) {
	miya_log_fprintf(log_fd, "%s failed NcStart\n", __FUNCTION__);
	ret_flag = FALSE;
	goto end_log_e;
      }

      /******** ネットワークにつないだ *************/
      // 必須：HTTP と SSL の初期化
      miya_log_fprintf(log_fd,"-setup NSSL & NHTTP\n");
      SetupNSSL();
      if( FALSE == SetupNHTTP() ) {
	ret_flag = FALSE;
	miya_log_fprintf(log_fd, "%s failed SetupNHTTP\n", __FUNCTION__);
	mprintf(" %s failed SetupNHTTP\n", __FUNCTION__);
	goto end_nhttp;
      }

      /******** NHTTP & NSSLにつないだ *************/
      // 必須：EC の初期化
      miya_log_fprintf(log_fd,"-setup EC\n");
      if( FALSE == SetupEC() ) {
	ret_flag = FALSE;
	miya_log_fprintf(log_fd, "%s failed SetupEC\n", __FUNCTION__);
	mprintf(" %s failed SetupEC\n", __FUNCTION__);
	goto end_ec;
      }

      // 必須：デバイス証明書の発行
      if( FALSE == KPSClient() ) {
	ret_flag = FALSE;
	miya_log_fprintf(log_fd, "%s failed KPSClient\n", __FUNCTION__);
	goto end_ec_f;
      }

      ec_download_ret = ECDownload((MY_USER_APP_TID *)title_id_buf_ptr , (u32)title_id_count);

      if( ec_download_ret == ECDOWNLOAD_FAILURE ) {
	ret_flag = FALSE;
	miya_log_fprintf(log_fd, "%s failed ECDownload 1\n", __FUNCTION__);
      }
      else if( ec_download_ret == ECDOWNLOAD_DUMMY ) {
	ret_flag = FALSE;
	miya_log_fprintf(log_fd, "%s failed ECDownload 2\n", __FUNCTION__);
      }
      // 不要：セーブデータ領域を作成
      // NAM_Init を忘れてた

      //      SetupTitlesDataFile((NAMTitleId *)title_id_buf_ptr , (u32)title_id_count);

      SetupTitlesDataFile((MY_USER_APP_TID *)title_id_buf_ptr , (u32)title_id_count);

    end_ec_f:
      // cleanup
      // EC の終了処理
      mprintf("-ec shutdown..               ");
      rv = EC_Shutdown();
      if( rv != EC_ERROR_OK ) {
	ret_flag = FALSE;
	m_set_palette(tc[0], M_TEXT_COLOR_RED );
	mprintf("NG.\n");
	m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
	miya_log_fprintf(log_fd, "%s failed EC_Shutdown\n", __FUNCTION__);
      }
      else {
	m_set_palette(tc[0], M_TEXT_COLOR_GREEN );
	mprintf("OK.\n");
      }
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    end_ec:
    
      // ネットワークからの切断
      miya_log_fprintf(log_fd,"-LINK DOWN....");
      mprintf("-LINK DOWN....");

      NHTTP_Cleanup();
    end_nhttp:
      NSSL_Finish();
    end_nssl:
      NcFinish();
    end_nc:
      //miya_log_fprintf(log_fd,"NSSL_Finish() return = %d\n", NSSL_Finish());

      TerminateWcmControl();

      miya_log_fprintf(log_fd,"done.\n");
      mprintf("done.\n");
      // EC が自分の Title ID のディレクトリを作成してしまうため、削除する
      DeleteECDirectory();
    end_log_e:
      ;
    }
    else {
      /* mprintf("-Wireless AP conf. loading.. "); */
      miya_log_fprintf(log_fd, "NG.\n");
      ret_flag = FALSE;
      m_set_palette(tc[0], M_TEXT_COLOR_RED );
      mprintf("NG.\n");
    }
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }

 pre_install_label:

  if( mydata.num_of_user_pre_installed_app > 0 ) {
    /* プリンストール対応 */
    miya_log_fprintf(log_fd,"Import Pre-installed apps.\n");
    mprintf("Import Pre-installed apps..\n");
    if( FALSE == pre_install_process( log_fd, title_id_buf_ptr, title_id_count ) ) {
      ret_flag = FALSE;
    }
  }


  hatamotolib_log_end();

  return ret_flag;
}



static BOOL RestoreFromSDCard8(void)
{
  int list_count;
  int error_count;
  BOOL ret_flag = TRUE;

  Error_Report_Init();

  if( mydata.num_of_app_save_data  > 0 ) { 
    mprintf("App. save data restore       ");
    
    if( TRUE == RestoreDirEntryList_System_And_InstallSuccessApp( MyFile_GetSaveDataListFileName() , 
								  MyFile_GetSaveDataRestoreLogFileName(),
								  &list_count, &error_count,
								  title_id_buf_ptr, title_id_count  )) {
      m_set_palette(tc[0], M_TEXT_COLOR_GREEN );
      mprintf("OK.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    }
    else {
      // error
      m_set_palette(tc[0], M_TEXT_COLOR_RED );
      mprintf("NG.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
      ret_flag = FALSE;
    }
  }
  else if( mydata.num_of_app_save_data == 0 ) {
    mprintf("Original device has no app. save data\n");
  }
  else {
    mprintf("Original app. save data saving failed\n");
  }

  if( title_id_buf_ptr != NULL ) {
    OS_Free( title_id_buf_ptr );
    title_id_buf_ptr = NULL;
  }
  title_id_count = 0;

  m_set_palette(tc[0], M_TEXT_COLOR_YELLOW );

  if( TRUE == Error_Report_Display(tc[0]) ) {
    mprintf("\n");
  }
  m_set_palette(tc[0], M_TEXT_COLOR_WHITE );

  Error_Report_End();

  return ret_flag;
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
  u32 command;

  m_set_palette(tc[0], M_TEXT_COLOR_WHITE );

  while( 1 ) {
    (void)OS_SendMessage(&MyMesgQueue_response, (OSMessage)0, OS_MESSAGE_NOBLOCK);
    (void)OS_ReceiveMessage(&MyMesgQueue_request, &message, OS_MESSAGE_BLOCK);
    flag = TRUE;
    twl_card_validation_flag = TRUE;
    /* MydataLoadはすでにやっているのでいらない。 */
    command = (u32)message;
    switch( command ) {
    case THREAD_COMMAND_FULL_FUNCTION:

      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
      
      if( miya_debug_level == 1 || ( my_fs_get_print_debug_flag() == TRUE )) {
	m_set_palette(tc[0], M_TEXT_COLOR_PINK );
	mprintf("Free mem size %d bytes\n", OS_GetTotalFreeSize(OS_ARENA_MAIN, hHeap));
	m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
      }
      for( function_counter = 0 ; function_counter < function_table_max ; function_counter++ ) {
	if( FALSE == (function_table[function_counter])() ) {
	  flag = FALSE;
	}
	if( miya_debug_level == 1 || ( my_fs_get_print_debug_flag() == TRUE )) {
	  m_set_palette(tc[0], M_TEXT_COLOR_PINK );
	  mprintf("Free mem size %d bytes\n", OS_GetTotalFreeSize(OS_ARENA_MAIN, hHeap));
	  m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
	}
      }
      break;
    case THREAD_COMMAND_WIFI_FUNCTION:
      function_table_max = 1;
      function_counter = 0;
      if( FALSE == RestoreFromSDCard3() ) {
	flag = FALSE;
      }
      function_counter++;
      break;
    case THREAD_COMMAND_USERDATA_AND_WIFI_FUNCTION:
      function_table_max = 2;
      function_counter = 0;
      if( FALSE == RestoreFromSDCard4() ) {
	flag = FALSE;
      }
      function_counter++;
      if( FALSE == RestoreFromSDCard3() ) {
	flag = FALSE;
      }
      function_counter++;
      break;
    case THREAD_COMMAND_REBOOT_FUNCTION:
      mprintf("%s Power button pressed!\n",__FUNCTION__);
      OS_TPrintf("%s Power button pressed!\n",__FUNCTION__);
      MCU_SetFreeRegister( FREE_REG_POWERON_REBOOT );
      PM_ReadyToExit();
      OS_Sleep(100000);
      break;
    default:
      function_table_max = 0;
      function_counter = 0;
      flag = FALSE;
      mprintf("%s unknown command!\n",__FUNCTION__);
      break;
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
#ifdef MIYA_SET_VOL_AND_BRIGHT
	  OS_Sleep(2000);
	  (void)MCU_SetVolume((u8)(mydata.volume));
	  (void)MCU_SetBackLightBrightness((u8)(mydata.backlight_brightness));
	  OS_TPrintf("vol = %d\n",mydata.volume );
	  OS_TPrintf("bright = %d\n", mydata.backlight_brightness);
	  OS_Sleep(20);
#endif
	  break; 
	}
	if( pushed_power_button == TRUE ) {
	  OS_TPrintf("%s Power button pressed!\n",__FUNCTION__);
	  MCU_SetFreeRegister( FREE_REG_POWERON_REBOOT );
	  PM_ReadyToExit();
	  pushed_power_button = FALSE;
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
  FSFile *log_fd;
  BOOL ret_flag;


  while( 1 ) {
    (void)OS_SendMessage(&MyMesgQueue_response, (OSMessage)0, OS_MESSAGE_NOBLOCK);
    (void)OS_ReceiveMessage(&MyMesgQueue_request, &message, OS_MESSAGE_BLOCK);

    if( (u32)message == THREAD_COMMAND_REBOOT_FUNCTION ) {
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
      mprintf("%s Power button pressed!\n",__FUNCTION__);
      OS_TPrintf("%s Power button pressed!\n",__FUNCTION__);
      MCU_SetFreeRegister( FREE_REG_POWERON_REBOOT );
      PM_ReadyToExit();
      pushed_power_button = FALSE;
    }
    else if( (u32)message != THREAD_COMMAND_NUP_FUNCTION ) {
      m_set_palette(tc[0], M_TEXT_COLOR_RED );
      mprintf("%s unknown command!\n",__FUNCTION__);
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
      continue;
    }

    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );

    mprintf("-Wireless AP conf. load      ");
    if( TRUE == LoadWlanConfig() ) {
      m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
      mprintf("OK.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );

      if( 0 != NcStart(SITEDEFS_DEFAULTCLASS) ) {
	mprintf("-connect to the AP           ");
	m_set_palette(tc[0], M_TEXT_COLOR_RED );
	mprintf("NG.\n");
	m_set_palette(tc[0], M_TEXT_COLOR_WHITE );

	Gfx_Set_BG1_Color((u16)M_TEXT_COLOR_DARKRED);
	if( TRUE == stream_play_is_end() ) {
	  stream_play2();	/* ng.aiff */
	}
	mprintf("Network Update failed!\n");
	OS_TPrintf("Network Update failed!\n");
	while( 1 ) {
	  keyData = m_get_key_code();
	  if ( keyData & (PAD_BUTTON_A | PAD_BUTTON_START) ) {
	    OS_RebootSystem();
	  }
	  if( pushed_power_button == TRUE ) {
	    OS_TPrintf("%s Power button pressed!\n",__FUNCTION__);
	    MCU_SetFreeRegister( FREE_REG_POWERON_REBOOT );
	    PM_ReadyToExit();
	    pushed_power_button = FALSE;
	  }
	  OS_Sleep(20);
	}
      }
      else {
	mprintf("-connect to the AP           ");
	m_set_palette(tc[0], M_TEXT_COLOR_GREEN );
	mprintf("OK.\n");
	m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
      }


      RTC_NTP_SYNC();


      /* NSSL_Init()呼んではダメ！ */
      log_fd = my_nuc_log_start( MyFile_GetNupLogFileName() );
      ret_flag = my_numc_proc();
      my_nuc_log_end();

      if( TRUE == ret_flag ) {
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
	if( reboot_rtc_adjust_done_flag == TRUE ) {
	  MCU_SetFreeRegister( FREE_REG_RESTOREMODE_AND_RTC_DONE_REBOOT );
	}
	else {
	  MCU_SetFreeRegister( FREE_REG_RESTOREMODE_REBOOT );
	}
	while( 1 ) {
	  keyData = m_get_key_code();
	  if ( keyData & (PAD_BUTTON_A | PAD_BUTTON_START) ) {
	    OS_RebootSystem();
	  }
	  if( pushed_power_button == TRUE ) {
	    OS_TPrintf("%s Power button pressed!\n",__FUNCTION__);
	    MCU_SetFreeRegister( FREE_REG_POWERON_REBOOT );
	    PM_ReadyToExit();
	    pushed_power_button = FALSE;
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
  u16 s_major, s_minor;
  BOOL s_flag;
  s64 sys_version_org;
  s64 sys_version;
  u32 s_timestamp;
  ESError es_error_code;
  BOOL MydataLoadDecrypt_message_flag = TRUE;
  BOOL MydataLoadDecrypt_dir_flag = TRUE;
  BOOL MydataLoadDecrypt_success_flag = TRUE;
  BOOL dir_select_mode = FALSE;
  int select_mode = 0;

  u8 free_reg;
  u16 BatterylevelBuf = 0;
  BOOL isAcConnectedBuf = FALSE;


  OS_Init();
  OS_InitThread();

  OS_InitTick();
  OS_InitAlarm();


  /* 無線ＬＡＮ設定の強制ＯＮ */
  WLAN_FORCE_ON();


  // マスター割り込みフラグを許可に
  (void)OS_EnableIrq();
  
  // IRQ 割り込みを許可します
  (void)OS_EnableInterrupts();

  // ARM7との通信FIFO割り込み許可
  (void)OS_EnableIrqMask(OS_IE_SPFIFO_RECV);

  // ファイルシステム初期化
  FS_Init( FS_DMA_NOT_USE );

  PM_Init();
  // メインアリーナのアロケートシステムを初期化
  newArenaLo = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 1);
  OS_SetMainArenaLo(newArenaLo);
  
  // メインアリーナ上にヒープを作成
  hHeap = OS_CreateHeap(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi());
  OS_SetCurrentHeap(OS_ARENA_MAIN, hHeap);

  allocator_total_size = OS_GetTotalFreeSize(OS_ARENA_MAIN, hHeap);


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
  /* OS_IsRebootedなんかおかしい・・ */


  MIYA_MCU_Init();

  OS_TPrintf("MCU Free Reg. 0x%02x\n", MCU_GetFreeReg());
  free_reg = MCU_GetFreeReg();
  if( free_reg == FREE_REG_RESTOREMODE_REBOOT ) {
    reboot_flag = TRUE;
    reboot_rtc_adjust_done_flag = FALSE;
  } 
  else if( free_reg == FREE_REG_RESTOREMODE_AND_RTC_DONE_REBOOT ) {
    reboot_flag = TRUE;
    reboot_rtc_adjust_done_flag = TRUE;
  }
  else {
    reboot_flag = FALSE;    
    reboot_rtc_adjust_done_flag = FALSE;
  }

  development_console_flag = IsThisDevelopmentConsole();
  if(TRUE == development_console_flag ) {
    mprintf("--development console--\n");
  }

  PM_SetAutoExit( FALSE );
  PM_SetExitCallbackInfo( &pmexitcallbackinfo,pmexitcallback, NULL);
  PM_PrependPreExitCallback( &pmexitcallbackinfo );


#ifdef MIYA_SET_VOL_AND_BRIGHT
  MCU_SetVolume( (u8)31 );
  MCU_SetBackLightBrightness( (u8)4 );
#endif    

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
    s_flag = FALSE;
    sys_version = -1;
  }
  else {
    s_flag = TRUE;
    sys_version = (s64)(((u32)s_major) << 16 | (u32)s_minor);
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


  // ニックネームが空なら適当に設定
  if( *LCFG_TSD_GetNicknamePtr() == L'\0' ) {
    LCFG_TSD_SetNickname((const u16*)(L"repair-tool"));
    //    mprintf("Set dummy Nickname\n");
  }



  // region
  org_region = LCFG_THW_GetRegion();
  // ES Device ID

  // 国が選択されていないなら適当に設定
  if( LCFG_TSD_GetCountry() == LCFG_TWL_COUNTRY_UNDEFINED ) {

    mprintf("Set dummy Country code ");

    switch( org_region ) {
    case OS_TWL_REGION_JAPAN:
      LCFG_TSD_SetCountry(LCFG_TWL_COUNTRY_JAPAN);
      mprintf("JPN");
      break;
    case OS_TWL_REGION_AMERICA:
      LCFG_TSD_SetCountry(LCFG_TWL_COUNTRY_UNITED_STATES);
      mprintf("USA");
      break;
    case OS_TWL_REGION_EUROPE:
      LCFG_TSD_SetCountry(LCFG_TWL_COUNTRY_UNITED_KINGDOM);
      mprintf("UK");
      break;
    case OS_TWL_REGION_AUSTRALIA:
      LCFG_TSD_SetCountry(LCFG_TWL_COUNTRY_AUSTRALIA);
      mprintf("AUS");
      break;
    case OS_TWL_REGION_CHINA:
      LCFG_TSD_SetCountry(LCFG_TWL_COUNTRY_CHINA);
      mprintf("CHN");
      break;
    case OS_TWL_REGION_KOREA:
      LCFG_TSD_SetCountry(LCFG_TWL_COUNTRY_SOUTH_KOREA);
      mprintf("KOR");
      break;
    case OS_TWL_REGION_MAX:
    default:
      mprintf("JPN");
      LCFG_TSD_SetCountry(LCFG_TWL_COUNTRY_JAPAN);
      break;
    }
    mprintf("\n");
  }

  /******************

    static inline LCFGTWLCountryCode LCFG_TSD_GetCountry( void )
    {
      return  (LCFGTWLCountryCode)LCFGi_GetTSD()->country;
    }

	// 国コードのセット。
    static inline void LCFG_TSD_SetCountry( LCFGTWLCountryCode country )
    {
        LCFGi_GetTSD()->country = (u8)country;
	LCFGi_GetTSD_OS()->country = (u8)country;
    }

    extern LCFGTWLSettingsData s_settings;
    #define LCFGi_GetTSD()      ( &s_settings )
    #define LCFGi_GetTSD_OS()	( (OSTWLSettingsData*)HW_PARAM_TWL_SETTINGS_DATA )



  *******************/


  org_fuseId = SCFG_ReadFuseData();
  OS_TPrintf("eFuseID:   %08X%08X\n", (u32)(org_fuseId >> 32), (u32)(org_fuseId));

  OS_GetMacAddress( macAddress );

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

  if( FALSE == OS_IsAvailableWireless() ) {
    wlan_active_flag = FALSE;
  }


  if( FALSE == reboot_flag ) {
    mprintf("Network update mode\n");
    /* 最初はネットワークアップデート。 */

    if( FALSE == wlan_active_flag ) {
      //    m_set_palette(tc[0], M_TEXT_COLOR_YELLOW );
      mprintf("Error:WLAN Enable flag = OFF\n");
      OS_TPrintf("WLAN Enable flag OFF\n");
      if( TRUE == stream_play_is_end() ) {
	stream_play2();	/* ng.aiff */
      }
      Gfx_Set_BG1_Color((u16)M_TEXT_COLOR_DARKRED);
      goto loop_start;
    } 

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
      (void)NA_UnloadVersionDataArchive();
    }

    init_my_thread_nuc();

    if( sd_card_flag == TRUE ) {
      text_blink_current_line(tc[0]);
      /* 自動スタートはいらない */
      mprintf("press A button to start UPDATE\n\n");
    }
    else {
      text_blink_current_line(tc[0]);
      mprintf("insert SD card\n");
    }
   
  }
  /* FALSE == reboot_flag ここまで */
  else {
    /* ここからリブート後 */
    if( FALSE == wlan_active_flag ) {
      select_mode = 1;	
      no_network_flag = TRUE;
    }


    dir_select_mode = TRUE;

    mprintf("user data restore mode\n");
    // NAM の初期化
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

 loop_start:

  Gfx_Set_BG1_line_Color(1, 2, (u16)M_TEXT_COLOR_ORANGE);

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
    else if ( keyData & (PAD_BUTTON_A | PAD_BUTTON_START) ) {
      if( sd_card_flag == TRUE ) {
	if( FALSE == reboot_flag ) {
	  if( wlan_active_flag != FALSE ) {
	    /* ネットワークアップデート */
	    text_blink_clear(tc[0]);
	    start_my_thread( THREAD_COMMAND_NUP_FUNCTION );
	  }
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

	      if( only_wifi_config_data_trans_flag == TRUE ) {
		/* 無線設定のみリストアする */
		mydata.wireless_lan_param_flag = TRUE;
		vram_num_sub = 0;
		MydataLoadDecrypt_message_flag = TRUE;
		MydataLoadDecrypt_dir_flag = TRUE;
		MydataLoadDecrypt_success_flag = TRUE;

#if 1
		start_my_thread( THREAD_COMMAND_WIFI_FUNCTION );
#else
		if( TRUE == RestoreFromSDCard3() ) {
		  if( TRUE == stream_play_is_end() ) {
		    stream_play1(); /* ok.aiff */
		  }
		  Gfx_Set_BG1_Color((u16)M_TEXT_COLOR_DARKGREEN);
		}
		else {
		  if( TRUE == stream_play_is_end() ) {
		    stream_play2();	/* ng.aiff */
		  }
		  Gfx_Set_BG1_Color((u16)M_TEXT_COLOR_DARKRED);
		}
#endif
	      }
	      else if( user_and_wifi_config_data_trans_flag == TRUE ) {
		/* ユーザー設定と無線設定のみリストアする */
		mydata.user_settings_flag = TRUE;
		mydata.wireless_lan_param_flag = TRUE;
		vram_num_sub = 0;
		MydataLoadDecrypt_message_flag = TRUE;
		MydataLoadDecrypt_dir_flag = TRUE;
		MydataLoadDecrypt_success_flag = TRUE;
#if 1
		start_my_thread( THREAD_COMMAND_USERDATA_AND_WIFI_FUNCTION );
#else
		if( (TRUE == RestoreFromSDCard4()) && (TRUE == RestoreFromSDCard3() ) ) {
		  if( TRUE == stream_play_is_end() ) {
		    stream_play1(); /* ok.aiff */
		  }
		  Gfx_Set_BG1_Color((u16)M_TEXT_COLOR_DARKGREEN);
		}
		else {
		  if( TRUE == stream_play_is_end() ) {
		    stream_play2();	/* ng.aiff */
		  }
		  Gfx_Set_BG1_Color((u16)M_TEXT_COLOR_DARKRED);
		}
#endif
	      }
	      else {
		mprintf("Personal data. restore       ");
		MydataLoadDecrypt_success_flag = MydataLoadDecrypt( MyFile_GetGlobalInformationFileName(), 
								    &mydata, sizeof(MyData), NULL);
		/* 
		   ランチャーが古い本体に書き戻す場合ですが、ネットワークアップデートを
		   スキップしてリストアしようとするとデータの整合性からNGになるようにされて
		   いると思います。なのですが、その時の表示がちょっと変です。
		*/

		if( mydata.sys_ver_flag == TRUE ) {
		  sys_version_org = (s64)(((u32)(mydata.sys_ver_major)) << 16 | (u32)(mydata.sys_ver_minor));
		}
		else {
		  sys_version_org = -1;
		}

		if(TRUE == MydataLoadDecrypt_success_flag ) {
		  if( org_region != mydata.region ) {
		    m_set_palette(tc[0], M_TEXT_COLOR_RED );
		    mprintf("NG.\n");
		    m_set_palette(tc[0], M_TEXT_COLOR_YELLOW );
		    mprintf(" invalid region code.\n");
		    mprintf(" \n");
		    m_set_palette(tc[0], 0xF);	/* white */
		    MydataLoadDecrypt_message_flag = FALSE;
		    if( TRUE == stream_play_is_end() ) {
		      stream_play2();	/* ng.aiff */
		    }
		  }
		  else if( mydata.version_major != MY_DATA_VERSION_MAJOR ) {
		    m_set_palette(tc[0], M_TEXT_COLOR_RED );
		    mprintf("NG.\n");
		    m_set_palette(tc[0], M_TEXT_COLOR_YELLOW );
		    mprintf(" invalid tool data format.\n");
		    mprintf(" %s\n version %d.%d\n",MyFile_GetGlobalInformationFileName(),
			    mydata.version_major,mydata.version_minor);
		    mprintf(" \n");
		    m_set_palette(tc[0], 0xF);	/* white */
		    MydataLoadDecrypt_message_flag = FALSE;
		    if( TRUE == stream_play_is_end() ) {
		      stream_play2();	/* ng.aiff */
		    }
		  }
		  else if( mydata.version_minor != MY_DATA_VERSION_MINOR ) {
		    m_set_palette(tc[0], M_TEXT_COLOR_RED );
		    mprintf("NG.\n");
		    m_set_palette(tc[0], M_TEXT_COLOR_YELLOW );
		    mprintf(" invalid tool data format.\n");
		    mprintf(" %s\n version %d.%d\n",MyFile_GetGlobalInformationFileName(),
			    mydata.version_major,mydata.version_minor);
		    mprintf(" \n");
		    m_set_palette(tc[0], 0xF);	/* white */
		    MydataLoadDecrypt_message_flag = FALSE;
		    if( TRUE == stream_play_is_end() ) {
		      stream_play2();	/* ng.aiff */
		    }
		  }
		  else if( s_flag && mydata.sys_ver_flag && (sys_version < sys_version_org) ) {
		    if(TRUE == IsThisDevelopmentConsole()) {
		      /* 開発機ではシステムバージョンアップが微妙なため */
		      goto label1;
		    }
		    m_set_palette(tc[0], M_TEXT_COLOR_RED );
		    mprintf("NG.\n");
		    m_set_palette(tc[0], M_TEXT_COLOR_YELLOW );
		    mprintf(" invalid System menu version.\n");
		    mprintf(" org. version %d.%d\n", mydata.sys_ver_major,mydata.sys_ver_minor);
		    mprintf(" cur. version %d.%d\n", s_major,s_minor);
		    mprintf(" \n");
		    m_set_palette(tc[0], 0xF);	/* white */
		    MydataLoadDecrypt_message_flag = FALSE;
		    if( TRUE == stream_play_is_end() ) {
		      stream_play2();	/* ng.aiff */
		    }
		  }
		  else {
		  label1:
		    m_set_palette(tc[0], 0x2);	/* green  */
		    mprintf("OK.\n");
		    m_set_palette(tc[0], 0xF);	/* white */
		    if(TRUE == development_console_flag ) {
		      mprintf("--development console--\n");
		    }

		    vram_num_sub = 0;
		    MydataLoadDecrypt_message_flag = TRUE;
		    MydataLoadDecrypt_dir_flag = TRUE;
		    MydataLoadDecrypt_success_flag = TRUE;
		    dir_select_mode = FALSE;

		    start_my_thread( THREAD_COMMAND_FULL_FUNCTION );
		  }
		}
		else {	
		  m_set_palette(tc[0], 0x1);	/* red  */
		  mprintf("NG.\n");
		  m_set_palette(tc[0], 0xF);	/* white */
		  MydataLoadDecrypt_message_flag = FALSE;
		  if( TRUE == stream_play_is_end() ) {
		    stream_play2();	/* ng.aiff */
		  }
		}
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
#if 0
    /* スタートボタンはＡボタンと同じ扱い */
    else if ( keyData & PAD_BUTTON_START ) {
    }
#endif
    else if ( keyData & PAD_BUTTON_SELECT ) {
      miya_debug_level ^= 1;
      mprintf("debug level %d\n", miya_debug_level );
    }
    else if ( keyData & PAD_BUTTON_X ) {
      if( TRUE == reboot_flag ) {
	no_network_flag = FALSE;
	only_wifi_config_data_trans_flag = FALSE;
	user_and_wifi_config_data_trans_flag = FALSE;
	Miya_debug_OFF();
	my_fs_print_debug_OFF();

	select_mode++;
	select_mode %= 6;
	switch( select_mode ) {
	case 0:
	  /* restore mode : default */
	  break;
	case 1:
	  no_network_flag = TRUE;
	  break;
	case 2:
	  only_wifi_config_data_trans_flag = TRUE;
	  break;
	case 3:
	  user_and_wifi_config_data_trans_flag = TRUE;
	  break;
	case 4:
	  Miya_debug_ON();
	  break;
	case 5:
	  my_fs_print_debug_ON();
	  break;
	default:
	  break;
	}
      }
    }
    else if ( keyData & PAD_BUTTON_Y ) {
      if( FALSE == reboot_flag ) {
	if( reboot_rtc_adjust_done_flag == TRUE ) {
	  MCU_SetFreeRegister( FREE_REG_RESTOREMODE_AND_RTC_DONE_REBOOT );
	}
	else {
	  MCU_SetFreeRegister( FREE_REG_RESTOREMODE_REBOOT );
	}
	OS_RebootSystem();
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
    else if ( keyData & PAD_KEY_UP ) {
      if( FALSE == reboot_flag ) {
	n = m_get_display_offset_y(tc[0]);
	n++;
	m_set_display_offset_y(tc[0], n);
      }
      else {
	if( dir_select_mode == TRUE ) {
	  if( vram_num_sub == 2 ) {
	    MFILER_CursorY_Up();
	  }
	}
	else {
	  n = m_get_display_offset_y(tc[0]);
	  n++;
	  m_set_display_offset_y(tc[0], n);
	}
      }
    }
    else if ( keyData & PAD_KEY_DOWN ) {
      if( FALSE == reboot_flag ) {
	n = m_get_display_offset_y(tc[0]);
	n--;
	m_set_display_offset_y(tc[0], n);
      }
      else {
	if( dir_select_mode == TRUE ) {
	  if( vram_num_sub == 2 ) {
	    MFILER_CursorY_Down();
	  }
	}
	else {
	  n = m_get_display_offset_y(tc[0]);
	  n--;
	  m_set_display_offset_y(tc[0], n);
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
    mfprintf(tc[1],"%08X%08X\n\n", (u32)(org_fuseId >> 32), (u32)(org_fuseId));


    m_set_palette(tc[1], M_TEXT_COLOR_LIGHTBLUE );
    mfprintf(tc[1],"MAC add.:  ");
    m_set_palette(tc[1], M_TEXT_COLOR_WHITE );
    mfprintf(tc[1],"%02X:%02X:%02X:%02X:%02X:%02X",  macAddress[0],macAddress[1],
	     macAddress[2], macAddress[3], macAddress[4], macAddress[5]);
    mfprintf(tc[1],"\n\n");

    m_set_palette(tc[1], M_TEXT_COLOR_LIGHTBLUE );
    mfprintf(tc[1],"Device ID: ");
    m_set_palette(tc[1], M_TEXT_COLOR_WHITE );

    /* ここで mydata.shop_record_flag　といっしょに */
    if( TRUE == mydata.shop_record_flag ) {
      mfprintf(tc[1],"%s\n", mydata.bmsDeviceId);
    }
    else {
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
	m_set_palette(tc[1], M_TEXT_COLOR_GREEN );
	break;
      }
      mfprintf(tc[1], "%d/5\n\n" , BatterylevelBuf); 
      m_set_palette(tc[1], M_TEXT_COLOR_WHITE );
    }

    if( FALSE == reboot_flag ) {
      mfprintf(tc[1], "press Y button to RESTORE mode\n");
    }
    else {
      switch( select_mode ) {
      case 0:
	/* restore mode : default */
	mfprintf(tc[1],"-- normal mode --\n");
	break;
      case 1:
	m_set_palette(tc[1], M_TEXT_COLOR_YELLOW );
	mfprintf(tc[1],"-- no network connection mode --\n");
	break;
      case 2:
	m_set_palette(tc[1], M_TEXT_COLOR_YELLOW );
	mfprintf(tc[1],"-- only wifi data mode --\n");
	break;
      case 3:
	m_set_palette(tc[1], M_TEXT_COLOR_YELLOW );
	mfprintf(tc[1],"-- only user data mode --\n");
	break;
      case 4:
	m_set_palette(tc[1], M_TEXT_COLOR_RED ); /* red  */
	mfprintf(tc[1],"-- NG mode --\n");
	break;
      case 5:
	m_set_palette(tc[1], M_TEXT_COLOR_YELLOW ); /* red  */
	mfprintf(tc[1],"-- debug mode --\n");
	break;
      default:
	break;  
      }
      m_set_palette(tc[1], M_TEXT_COLOR_WHITE );


      mfprintf(tc[1],"\n");

      mfprintf(tc[1], "function no.%d/%d\n", function_counter, function_table_max);

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
	if( s_flag && mydata.sys_ver_flag && (sys_version < sys_version_org) ) {
	  m_set_palette(tc[2], M_TEXT_COLOR_YELLOW );
	  mfprintf(tc[2], " invalid System menu version.\n");
	  mfprintf(tc[2], " org. version %d.%d\n", mydata.sys_ver_major,mydata.sys_ver_minor);
	  mfprintf(tc[2], " cur. version %d.%d\n", s_major,s_minor);
	  mfprintf(tc[2], " \n");
	  m_set_palette(tc[2], 0xF);	/* white */
	}
	else if( org_region != mydata.region ) {
	  mfprintf(tc[2]," invalid region code.\n");
	  mfprintf(tc[2]," \n");
	  mfprintf(tc[2]," \n");
	  mfprintf(tc[2]," \n");
	  m_set_palette(tc[2], 0xF);	/* white */
	}
	else if( mydata.version_major != MY_DATA_VERSION_MAJOR ) {
	  mfprintf(tc[2]," invalid format version.\n");
	  mfprintf(tc[2]," %s\n",MyFile_GetGlobalInformationFileName());
	  mfprintf(tc[2]," version %d.%d\n", mydata.version_major,mydata.version_minor);
	  m_set_palette(tc[2], 0xF);	/* white */
	  mfprintf(tc[2],"\n");
	}
	else if( mydata.version_minor != MY_DATA_VERSION_MINOR ) {
	  mfprintf(tc[2]," invalid format version.\n");
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

    if( (pushed_power_button == TRUE)  &&  (throw_pushed_power_button == FALSE) ) {
      if( FALSE == start_my_thread((u32)THREAD_COMMAND_REBOOT_FUNCTION) ) {
      }
      else {
	throw_pushed_power_button = TRUE;
      }
    }

    loop_counter++;

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

