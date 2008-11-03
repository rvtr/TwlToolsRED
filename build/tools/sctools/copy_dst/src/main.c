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
#include        "logprintf.h"

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
#include        "miya_mcu.h"
#include        "error_report.h"

#include        "myfilename.h"
#include        "mfiler.h"
#include        "menu_version.h"


//================================================================================

// #define MIYA_MCU 1

static BOOL no_reboot_flag = FALSE;
static BOOL only_wifi_config_data_trans_flag = FALSE;
static BOOL user_and_wifi_config_data_trans_flag = FALSE;
static BOOL no_network_flag = FALSE;
static BOOL completed_flag = FALSE;
static FSEventHook  sSDHook;
static BOOL sd_card_flag = FALSE;
//static BOOL reboot_flag = FALSE;

static u8 org_region = 0;
static u64 org_fuseId = 0;
static int select_mode = 0;
static volatile BOOL reboot_flag;

static int miya_debug_level = 0;



static u8 WorkForNA[NA_VERSION_DATA_WORK_SIZE];



static BOOL pushed_power_button = FALSE;

static PMExitCallbackInfo pmexitcallbackinfo;

static void pmexitcallback(void *arg)
{
#pragma unused(arg)
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
  if( (mydata.rtc_date_flag == TRUE) && (mydata.rtc_time_flag == TRUE) ) {
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
  if( mydata.wireless_lan_param_flag == TRUE ) {
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

  Error_Report_Init();

  if( mydata.num_of_shared2_files > 0 ) { 
    mprintf("App. shared files restore    ");
    if( TRUE == RestoreDirEntryList( MyFile_GetAppSharedListFileName(), 
				     MyFile_GetAppSharedRestoreLogFileName(), &list_count, &error_count )) {
      m_set_palette(tc[0], M_TEXT_COLOR_GREEN );
      mprintf("OK.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    }
    else {
      // error
      m_set_palette(tc[0], M_TEXT_COLOR_RED );
      mprintf("NG.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
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

  return TRUE;
}

static BOOL RestoreFromSDCard6(void)
{
  int list_count;
  int error_count;


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

  return TRUE;
}


static BOOL RestoreFromSDCard8(void)
{
  int list_count;
  int error_count;

  Error_Report_Init();

  if( mydata.num_of_app_save_data  > 0 ) { 
    mprintf("App. save data restore       ");
    if( no_network_flag == TRUE ) {
      if( TRUE == RestoreDirEntryListSystemBackupOnly( MyFile_GetSaveDataListFileName() , 
						       MyFile_GetSaveDataRestoreLogFileName(),
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
      }
    }
    else {
      if( TRUE == RestoreDirEntryList( MyFile_GetSaveDataListFileName() , 
				       MyFile_GetSaveDataRestoreLogFileName(),
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
      }
    }
  }
  else if( mydata.num_of_app_save_data == 0 ) {
    mprintf("Original device has no app. save data\n");
  }
  else {
    mprintf("Original app. save data saving failed\n");
  }

  if( TRUE == Error_Report_Display(tc[0]) ) {
    mprintf("\n");
  }
  Error_Report_End();

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
    case WCM_WEPMODE_NONE:
      OS_TPrintf("NONE\n");
      mfprintf(tc[3],"NONE\n");
      break;
    case WM_WEPMODE_40BIT:
      OS_TPrintf("WEP128\n");
      mfprintf(tc[3],"WEP128\n");
      break;
    case WM_WEPMODE_104BIT:
      OS_TPrintf("WEP128\n");
      mfprintf(tc[3],"WEP128\n");
      break;
    case WM_WEPMODE_128BIT:
      OS_TPrintf("WEP128\n");
      mfprintf(tc[3],"WEP128\n");
      break;
    case WCM_WEPMODE_WPA_TKIP:
      OS_TPrintf("WPA-TKIP\n");
      mfprintf(tc[3],"WPA-TKIP\n");
      break;
    case WCM_WEPMODE_WPA2_TKIP:
      OS_TPrintf("WPA2-TKIP\n");
      mfprintf(tc[3],"WPA2-TKIP\n");
      break;
    case WCM_WEPMODE_WPA_AES:
      OS_TPrintf("WPA-AES\n");
      mfprintf(tc[3],"WPA-AES\n");
      break;
    case WCM_WEPMODE_WPA2_AES :
      OS_TPrintf("WPA2-AES\n");
      mfprintf(tc[3],"WPA2-AES\n");
      break;
    defalut:
      OS_TPrintf("Unknow mode..\n");
      mfprintf(tc[3],"Unknow mode..\n");
      break;
    }

    if( TRUE == GetKeyModeStr() ) {
      OS_TPrintf("KEY STR = %s\n", GetWlanKEYSTR());
      mfprintf(tc[3],"KEY STR = %s\n", GetWlanKEYSTR());
    }
    else {
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
    mfprintf(tc[3],"\n");

    if( TRUE == GetDhcpMODE() ) {
      mfprintf(tc[3],"DHCP client\n");
    }
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


  }
  else {
    OS_TPrintf("Invalid wlan cfg file\n");
    mfprintf(tc[3],"Invalid wlan cfg file\n");
    mprintf("Invalid wlan cfg file\n");
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
  BOOL ret_flag = TRUE;
  FSFile *log_fd;

  title_id_buf_ptr = NULL;
  title_id_count = 0;
  rv = EC_ERROR_OK;

  /* hws_info.serialNoは戻せない */

  if( no_network_flag == TRUE ) {
    return TRUE;
  }

  log_fd = hatamotolib_log_start( MyFile_GetEcDownloadLogFileName() );


  if( mydata.shop_record_flag == FALSE ) {
    /* ネットワークにつながなくていいか？ */
    miya_log_fprintf(log_fd,"no shop record\n");
    mprintf(" (--no shop record--)\n");
  }
  else {
    miya_log_fprintf(log_fd,"EC download\n");
    mprintf("EC download\n");
    
    if( mydata.num_of_user_download_app > 0 ) {
      miya_log_fprintf(log_fd,"-user title list loading\n");
      mprintf("-user title list load        ");
      if( TRUE == TitleIDLoad( MyFile_GetDownloadTitleIDFileName(), &title_id_buf_ptr, 
			       &title_id_count, MyFile_GetDownloadTitleIDRestoreLogFileName()) ) {
	
	m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
	mprintf("OK.\n");
	m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
	for( i = 0; i < title_id_count ; i++ ) {
	  u64 tid = *(title_id_buf_ptr + i );
	  mprintf(" id %02d %08X %08X\n", i,(u32)(tid >> 32), (u32)tid);
	  miya_log_fprintf(log_fd," id %02d %08X %08X\n", i,(u32)(tid >> 32), (u32)tid);
	}
      }
      else {
	ret_flag = FALSE;
	m_set_palette(tc[0], M_TEXT_COLOR_RED );
	mprintf("NG.\n");
	m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
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

    //    mprintf("                             ");
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

      if( FALSE == ECDownload((NAMTitleId *)title_id_buf_ptr , (u32)title_id_count) ) {
	ret_flag = FALSE;
	miya_log_fprintf(log_fd, "%s failed ECDownload\n", __FUNCTION__);
	goto end_ec_f;
      }

      // 不要：セーブデータ領域を作成
      // NAM_Init を忘れてた
      SetupTitlesDataFile((NAMTitleId *)title_id_buf_ptr , (u32)title_id_count);

    end_ec_f:
      // cleanup
      // EC の終了処理
      mprintf("-ec shutdown..               ");
      rv = EC_Shutdown();
      // SDK_WARNING(rv == EC_ERROR_OK, "Failed to shutdown EC, rv=%d\n", rv);
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

      if( title_id_buf_ptr != NULL && title_id_count != 0 ) {
	OS_Free( title_id_buf_ptr );
      }
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

  hatamotolib_log_end();

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

  while( 1 ) {
    (void)OS_SendMessage(&MyMesgQueue_response, (OSMessage)0, OS_MESSAGE_NOBLOCK);
    (void)OS_ReceiveMessage(&MyMesgQueue_request, &message, OS_MESSAGE_BLOCK);
    flag = TRUE;
    twl_card_validation_flag = TRUE;
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
#ifdef MIYA_SET_VOL_AND_BRIGHT
	  OS_Sleep(2000);
	  (void)MCU_SetVolume((u8)(mydata.volume));
	  (void)MCU_SetBackLightBrightness((u8)(mydata.backlight_brightness));
	  OS_TPrintf("vol = %d\n",mydata.volume );
	  OS_TPrintf("bright = %d\n", mydata.backlight_brightness);
	  OS_Sleep(200000);
#endif
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
  FSFile *log_fd;
  BOOL ret_flag;


  

  while( 1 ) {
    (void)OS_SendMessage(&MyMesgQueue_response, (OSMessage)0, OS_MESSAGE_NOBLOCK);
    (void)OS_ReceiveMessage(&MyMesgQueue_request, &message, OS_MESSAGE_BLOCK);
    mprintf("-Wireless AP conf. load      ");
    if( TRUE == LoadWlanConfig() ) {
      m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
      mprintf("OK.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );

      if( 0 != NcStart(SITEDEFS_DEFAULTCLASS) ) {
	mprintf("connection failed!\n\n");
	while( 1 ) {
	  keyData = m_get_key_code();
	  if ( keyData & (PAD_BUTTON_A | PAD_BUTTON_START) ) {
	    OS_RebootSystem();
	  }
	  OS_Sleep(20);
	}
      }


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
	if( no_reboot_flag == FALSE ) {
	  mprintf("\n");
	  text_blink_current_line(tc[0]);
	  mprintf("press A button to start RESTORE\n\n");
	  MCU_SetFreeRegister( 0x55 );
	  while( 1 ) {
	    keyData = m_get_key_code();
	    if ( keyData & (PAD_BUTTON_A | PAD_BUTTON_START) ) {
	      OS_RebootSystem();
	    }
	    OS_Sleep(20);
	  }
	}
	else {
	  mprintf("\n");
	  // text_blink_current_line(tc[0]);
	  while( 1 ) {
	    keyData = m_get_key_code();
	    if ( keyData & (PAD_BUTTON_A | PAD_BUTTON_START) ) {
	    }
	    OS_Sleep(20);
	  }
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
  int n;
  u8 macAddress[6];
  MY_ENTRY_LIST *mfiler_list_head = NULL;
  u16 s_major, s_minor;
  u32 s_timestamp;
  ESError es_error_code;
  BOOL MydataLoadDecrypt_message_flag = TRUE;
  BOOL MydataLoadDecrypt_dir_flag = TRUE;
  BOOL MydataLoadDecrypt_success_flag = TRUE;
  BOOL dir_select_mode = FALSE;
  u8 free_reg;
  u8 mode;

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

  PM_Init();
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
  /* OS_IsRebootedなんかおかしい・・ */


  MIYA_MCU_Init();

  OS_TPrintf("MCU Free Reg. 0x%02x\n", MCU_GetFreeReg());
  free_reg = MCU_GetFreeReg();
  if( free_reg == 0x55 ) {
    reboot_flag = TRUE;
  } 
  else if( free_reg == 0x66 ) {
    reboot_flag = TRUE;
    no_network_flag = TRUE;
  }
  else if( free_reg == 0x77 ) {
    no_reboot_flag = TRUE;
    mprintf("no_reboot_flag ON\n");
  }
  else if( free_reg == 0x88 ) {
    reboot_flag = TRUE;
    only_wifi_config_data_trans_flag = TRUE;
    mprintf("only_wifi_config_data_trans ON\n");
  }
  else if( free_reg == 0x99 ) {
    reboot_flag = TRUE;
    user_and_wifi_config_data_trans_flag = TRUE;
    mprintf("user_and_wifi_config_data ON\n");
  }
  else {
    reboot_flag = FALSE;    
  }

  /* デバッグのために今だけ強制的にオン(UPDATE mode) */
  /* miya */
  //  reboot_flag = TRUE;




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

#if 1
  // ニックネームが空なら適当に設定
  if( *LCFG_TSD_GetNicknamePtr() == L'\0' ) {
    LCFG_TSD_SetNickname((const u16*)(L"repair-tool"));
    //    mprintf("Set dummy Nickname\n");
  }
#endif  

  // 国が選択されていないなら適当に設定
  if( LCFG_TSD_GetCountry() == LCFG_TWL_COUNTRY_UNDEFINED ) {
    LCFG_TSD_SetCountry(LCFG_TWL_COUNTRY_JAPAN);
    //    mprintf("Set dummy Country code\n");
  }

  // region
  org_region = LCFG_THW_GetRegion();
  // ES Device ID

  org_fuseId = SCFG_ReadFuseData();
  OS_TPrintf("eFuseID:   %08X%08X\n", (u32)(org_fuseId >> 32), (u32)(org_fuseId));

  OS_GetMacAddress( macAddress );

  mydata.shop_record_flag = FALSE;
  es_error_code = ES_GetDeviceId(&mydata.deviceId);
  if( es_error_code == ES_ERR_OK ) {
    if( TRUE == CheckShopRecord( hws_info.region, NULL ) ) {
      mydata.shop_record_flag = TRUE;
    }
    else {
      mprintf("no ec.cfg file\n");
    }
  }
  else {
    OS_TPrintf("es_error_code = %d\n", es_error_code );
  }
  
  // (void)CheckShopRecord( hws_info.region, NULL );
  
  if( TRUE == mydata.shop_record_flag ) {
    snprintf(mydata.bmsDeviceId, sizeof(mydata.bmsDeviceId), "%lld", ((0x3ull << 32) | mydata.deviceId));
    // OS_TPrintf("DeviceID:  %08X %u\n", mydata.deviceId, mydata.deviceId);
    OS_TPrintf("DeviceID: %s\n", mydata.bmsDeviceId);
  }


  if( FALSE == reboot_flag ) {
    mprintf("Network update mode\n");
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
  else {
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

	      if( only_wifi_config_data_trans_flag == TRUE ) {
		/* 無線設定のみリストアする */
		mydata.wireless_lan_param_flag = TRUE;
		vram_num_sub = 0;
		MydataLoadDecrypt_message_flag = TRUE;
		MydataLoadDecrypt_dir_flag = TRUE;
		MydataLoadDecrypt_success_flag = TRUE;
		(void)RestoreFromSDCard3();
	      }
	      else if( user_and_wifi_config_data_trans_flag == TRUE ) {
		mydata.user_settings_flag = TRUE;
		mydata.wireless_lan_param_flag = TRUE;
		vram_num_sub = 0;
		MydataLoadDecrypt_message_flag = TRUE;
		MydataLoadDecrypt_dir_flag = TRUE;
		MydataLoadDecrypt_success_flag = TRUE;
		(void)RestoreFromSDCard4();
		(void)RestoreFromSDCard3();
	      }
	      else {
		mprintf("Personal data. restore       ");
		MydataLoadDecrypt_success_flag = MydataLoadDecrypt( MyFile_GetGlobalInformationFileName(), 
								    &mydata, sizeof(MyData), NULL);
		if(TRUE == MydataLoadDecrypt_success_flag ) {
		  if( org_region != mydata.region ) {
		    m_set_palette(tc[0], M_TEXT_COLOR_RED );
		    mprintf("NG.\n");
		    m_set_palette(tc[0], M_TEXT_COLOR_YELLOW );
		    mprintf(" invalid region code.\n");
		    mprintf(" \n");
		    mprintf(" \n");
		    m_set_palette(tc[0], 0xF);	/* white */
		    MydataLoadDecrypt_message_flag = FALSE;
		  }
		  else if( mydata.version_major != MY_DATA_VERSION_MAJOR ) {
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
		    dir_select_mode = FALSE;
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
      miya_debug_level++;
      miya_debug_level &= 1;
      if( miya_debug_level ) {
	Miya_debug_ON();
	mprintf("debug ON\n");
      }
      else {
	Miya_debug_OFF();
	mprintf("debug OFF\n");
      }
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
      switch( select_mode ) {
      case 1:
	free_reg = mode = 0x55;
	break;
      case 2:
	free_reg = mode = 0x66;
	no_network_flag = TRUE;
	break;
      case 3:
	free_reg = mode = 0x77;
	no_reboot_flag = TRUE;
	break;
      case 4:
	free_reg = mode = 0x88;
	only_wifi_config_data_trans_flag = TRUE;
	break;
      case 5:
	free_reg = mode = 0x99;
	user_and_wifi_config_data_trans_flag = TRUE;
	break;
      default:
	free_reg = mode = 0;
	select_mode = 0;
	break;
      }

    }
    else if ( keyData & PAD_BUTTON_Y ) {
      MCU_SetFreeRegister( mode);
      OS_RebootSystem();
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


    if( FALSE == reboot_flag ) {
      if( free_reg == 0x55 ) {
	mfprintf(tc[1],"restart to RESTORE mode\n");
      }
      else if( free_reg == 0x66 ) {
	mfprintf(tc[1],"network-connection OFF mode\n");
      }
      else if( free_reg == 0x77 ) {
	mfprintf(tc[1],"no_reboot_flag ON\n");
      }
      else if( free_reg == 0x88 ) {
	mfprintf(tc[1],"only wifi config data\n");
      }
      else if( free_reg == 0x99 ) {
	mfprintf(tc[1],"only user settings data\n");
      }
      else {
	mfprintf(tc[1],"just reboot\n");
      }
      mfprintf(tc[1], "press Y button to restart.\n");

    }
    else {
      mfprintf(tc[1], "function no.%d/%d\n\n", function_counter, function_table_max);

      if( free_reg == 0x66 ) {
	mfprintf(tc[1],"network-connection OFF mode\n");
      }
      else if( free_reg == 0x77 ) {
	mfprintf(tc[1],"no_reboot_flag ON\n");
      }
      else if( free_reg == 0x88 ) {
	mfprintf(tc[1],"only wifi config data\n");
      }
      else if( free_reg == 0x99 ) {
	mfprintf(tc[1],"only user settings data\n");
      }
      
	 
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

    if( pushed_power_button == TRUE ) {
      MCU_SetFreeRegister( 0x00 );
      //      OS_TPrintf("ahondara\n");
      PM_ReadyToExit();
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

