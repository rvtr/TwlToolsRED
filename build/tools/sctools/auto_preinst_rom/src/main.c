#include <twl.h>
#include <nitro/nvram/nvram.h>
#include <twl/sea.h>
#include <twl/lcfg.h>
#include <twl/na.h>
#include <twl/nam.h>

#include "ecdl.h"

#include <NitroWiFi/nhttp.h>
#include <NitroWiFi/nssl.h>
#include <NitroWiFi/ncfg.h>

#include        "font.h"
#include        "text.h"
#include        "mprintf.h"
#include        "gfx.h"
#include        "key.h"
#include        "my_fs_util.h"
#include        "hwi.h"
#include        "mydata.h"

#include        "error_report.h"

#include        "myfilename.h"

#include        "pre_install.h"
#include        "hatamotolib.h"
#include        "mywlan.h"
#include        "netconnect.h"
#include        "sitedefs.h"
#include        "wcm_control.h"



#define THREAD_COMMAND_INSTALL_APP           1
#define THREAD_COMMAND_INSTALL_TICKET        2
#define THREAD_COMMAND_DELETE_APP_CONTENT    3
#define THREAD_COMMAND_DELETE_APP_COMPLETELY 4
#define THREAD_COMMAND_DELETE_DEVKP          5
#define THREAD_COMMAND_DOWNLOAD_APP          6


static void init_my_thread(void);
static BOOL start_my_thread(u32 command);

static FSEventHook  sSDHook;
static BOOL sd_card_flag = FALSE;

static BOOL development_console_flag = FALSE;

static OSHeapHandle hHeap;
static u32 allocator_total_size = 0;

static u8 org_region = 0;
static u64 org_fuseId = 0;

static BOOL pushed_power_button = FALSE;
static BOOL throw_pushed_power_button = FALSE;

static  LCFGTWLHWNormalInfo hwn_info;
static  LCFGTWLHWSecureInfo hws_info;

static MyData mydata;

static int vram_num_main = 1;
static int vram_num_sub = 0;

static u64 eticket_only_id_buf[100];
static int eticket_only_id_count = 0;

static void fill_command(void)
{
  if( development_console_flag == FALSE ) {
    /* 量産機用 */
    /*
      0x00030004484e474a, 0 , 0 , rom:/tads/TWL-HNGJ-v256.tad,
      0x000300044b32444a, 0 , 0 , rom:/tads/TWL-K2DJ-v0.tad,
      0x000300044b47554a, 0 , 0 , rom:/tads/TWL-KGUJ-v257.tad,

      0x00030004434f5041, 0 , 0 , sdmc:/sdtads/en_CooperationA.tad,
      0x0003001548485741, 0 , 0 , sdmc:/sdtads/en_HHWA.Release.tad,
      0x0003000548504341, 0 , 0 , sdmc:/sdtads/en_HPCA.Release.tad,

    */
#if 0
    /* ROMのやつ */
    eticket_only_id_buf[0] =  0x00030004484e474a;
    eticket_only_id_buf[1] =  0x000300044b32444a;
    eticket_only_id_buf[2] =  0x000300044b47554a;
#else
    /* SDのやつ */
    eticket_only_id_buf[0] =  0x00030004434f5041;
    eticket_only_id_buf[1] =  0x0003001548485741;
    eticket_only_id_buf[2] =  0x0003000548504341;
#endif
    eticket_only_id_count = 3;
  }
  else {
    /* 開発機用 */
    /*
      ROM
      0x0003000434424e41, 0 , 0 , rom:/tads_dev/backupSample.tad,
      0x00030004344a4541, 0 , 0 , rom:/tads_dev/encodeSD.tad,
      0x0003000434534e41, 0 , 0 , rom:/tads_dev/nandAppSample.tad,

      SD
      0x0003000434424e41, 0 , 0 , sdmc:/sdtaddevs/en_backupSample.tad,
      0x0003000444534943, 0 , 0 , sdmc:/sdtaddevs/en_chavitt.tad,
      0x00030004344a4541, 0 , 0 , sdmc:/sdtaddevs/en_encodeSD.tad,
      0x0003000434534e41, 0 , 0 , sdmc:/sdtaddevs/en_nandAppSample.tad,
    */

#if 1
    /* ROMのやつ */
    eticket_only_id_buf[0] =  0x0003000434424e41;
    eticket_only_id_buf[1] =  0x00030004344a4541;
    eticket_only_id_buf[2] =  0x0003000434534e41;
    eticket_only_id_count = 3;
#else
    /* SDのやつ */
    eticket_only_id_buf[0] =  0x0003000434424e41;
    eticket_only_id_buf[1] =  0x0003000434534e41;
    eticket_only_id_count = 2;
#endif

  }
}

static BOOL myTWLCardCallback( void )
{
  return FALSE; // means that not terminate.
}

static PMExitCallbackInfo pmexitcallbackinfo;

static void pmexitcallback(void *arg)
{
#pragma unused(arg)
  /* 処理中（データ書き込み中）ならリセットをブロックしなければならない */
  pushed_power_button = TRUE;
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
  }
  else {
    OS_TPrintf("Invalid wlan cfg file\n");
    //    mfprintf(tc[3],"Invalid wlan cfg file\n");
    mprintf("Invalid wlan cfg file\n");
    return FALSE;
  }
  return TRUE;
}


void TwlMain(void)
{
  int i;
  void* newArenaLo;
  u8 macAddress[6];
  ESError es_error_code;
  u16 keyData;
  int loop_counter = 0;
  RTCDate rtc_date;
  RTCTime rtc_time;
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

  CARD_Init();
  CARD_SetPulledOutCallback( myTWLCardCallback );

  // 必須；SEA の初期化
  SEA_Init();


  development_console_flag = IsThisDevelopmentConsole();
  if(TRUE == development_console_flag ) {
    mprintf("--development console--\n");
  }

  PM_SetAutoExit( FALSE );
  PM_SetExitCallbackInfo( &pmexitcallbackinfo,pmexitcallback, NULL);
  PM_PrependPreExitCallback( &pmexitcallbackinfo );


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


  org_fuseId = SCFG_ReadFuseData();
  OS_TPrintf("eFuseID:   %08X%08X\n", (u32)(org_fuseId >> 32), (u32)(org_fuseId));

  OS_GetMacAddress( macAddress );

OS_TPrintf("%s %s %d\n", __FILE__,__FUNCTION__,__LINE__ );

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
OS_TPrintf("%s %s %d\n", __FILE__,__FUNCTION__,__LINE__ );
  // NAM の初期化
  NAM_Init(&AllocForNAM, &FreeForNAM);


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


  // OS_TPrintf("%s %s %d\n", __FILE__,__FUNCTION__,__LINE__ );

  init_my_thread();


  while( 1 ) {
    Gfx_Render( vram_num_main , vram_num_sub );
    OS_WaitVBlankIntr();
    (void)RTC_GetDate( &rtc_date );
    (void)RTC_GetTime( &rtc_time );

    keyData = m_get_key_trigger();

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
      fill_command();
      (void)start_my_thread(THREAD_COMMAND_INSTALL_APP);
    }
    else if ( keyData & PAD_BUTTON_B ) {
      fill_command();
      (void)start_my_thread(THREAD_COMMAND_INSTALL_TICKET);
    }
    else if ( keyData & PAD_BUTTON_X ) {
      fill_command();
      (void)start_my_thread(THREAD_COMMAND_DELETE_APP_CONTENT);
    }
    else if ( keyData & PAD_BUTTON_Y ) {
      fill_command();
      (void)start_my_thread(THREAD_COMMAND_DELETE_APP_COMPLETELY);
    }
    else if ( keyData & PAD_BUTTON_START ) {
      if( sd_card_flag == TRUE ) {
	(void)start_my_thread(THREAD_COMMAND_DELETE_DEVKP);
      }
    }
    else if ( keyData & PAD_BUTTON_SELECT ) {
      (void)start_my_thread(THREAD_COMMAND_DOWNLOAD_APP);
    }



    mfprintf(tc[1], "\fAuto Pre-install Tool");


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
    if( (loop_counter % 60*5) == 0 ) {
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
    mfprintf(tc[1], "A -> Install App.\n");
    mfprintf(tc[1], "B -> Install Ticket\n");
    mfprintf(tc[1], "X -> Delete App. content\n");
    mfprintf(tc[1], "Y -> Delete App. completely\n");

    mfprintf(tc[1], "START  -> Delete /sys/dev.kp\n");
    mfprintf(tc[1], "SELECT -> Download App.\n");

    if( pushed_power_button == TRUE ) {
      OS_TPrintf("%s Power button pressed!\n",__FUNCTION__);
      PM_ReadyToExit();
      pushed_power_button = FALSE;
    }

    loop_counter++;
  }
  OS_Terminate();

}



static MY_USER_APP_TID title_id_buf_ptr[] = {
  {0x000300044b47554a, 2, FALSE },
  {0x000300044b32444a, 2, FALSE },
  {0x00030004484e474a, 2, FALSE },
  {0x00030004346b6941, 2, FALSE },
  {0x00030004346b6a41, 2, FALSE },
  {0x00030004346b6b41, 2, FALSE },
  {0x00030004346b6c41, 2, FALSE },
  {0x0003000434656241, 2, FALSE },
  {0x0003000434657541, 2, FALSE },
  {0x0003000430484141, 2, FALSE },
  {0x0003000430484941, 2, FALSE },
  {0x000300044b58374a, 2, FALSE },
  {0x000300044b53394a, 2, FALSE }
};

static int title_id_count = sizeof(title_id_buf_ptr)/sizeof(MY_USER_APP_TID);

#ifdef HATAMOTO_LIB
static void ec_download_func(void)
{
  int i;
  ECError rv;
  BOOL ret_flag = TRUE;
  int ec_download_ret;
  char game_code_buf[5];
  int is_personalized;
  u64 tid;


  mprintf("-wireless AP conf. load      ");
  if( TRUE == LoadWlanConfig() ) {
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    
    SetupShopTitleId(); /* エラーはない */
    
    // ？：ユーザ設定がされていないと接続できないので適当に設定
    // SetupUserInfo();
    // 必須：バージョンデータのマウント
    if( FALSE == SetupVerData() ) {
      mprintf("%s failed SetupVerData\n", __FUNCTION__);
      ret_flag = FALSE;
      goto end_log_e;
    }
    
    // 必須：ネットワークへの接続
    if( 0 != NcStart(SITEDEFS_DEFAULTCLASS) ) {
      mprintf("%s failed NcStart\n", __FUNCTION__);
      ret_flag = FALSE;
      goto end_log_e;
    }

    /******** ネットワークにつないだ *************/
    // 必須：HTTP と SSL の初期化
    mprintf("-setup NSSL & NHTTP\n");
    SetupNSSL();
    if( FALSE == SetupNHTTP() ) {
      ret_flag = FALSE;
      mprintf(" %s failed SetupNHTTP\n", __FUNCTION__);
      goto end_nhttp;
    }

      /******** NHTTP & NSSLにつないだ *************/
      // 必須：EC の初期化
    mprintf("-setup EC\n");
    if( FALSE == SetupEC() ) {
      ret_flag = FALSE;
      mprintf(" %s failed SetupEC\n", __FUNCTION__);
      goto end_ec;
    }

    // 必須：デバイス証明書の発行
    if( FALSE == KPSClient() ) {
      ret_flag = FALSE;
      mprintf("%s failed KPSClient\n", __FUNCTION__);
      goto end_ec_f;
    }



    for( i = 0; i < title_id_count ; i++ ) {
      tid = title_id_buf_ptr[i].tid;
      is_personalized = title_id_buf_ptr[i].is_personalized = 2;
      (void)my_fs_Tid_To_GameCode(tid, game_code_buf);
      mprintf(" id %08X %08X [%s] %c\n", (u32)(tid >> 32), (u32)tid, game_code_buf, 
	      (is_personalized == 1)? 'P':'D');
    }



    ec_download_ret = ECDownload_Auto((MY_USER_APP_TID *)title_id_buf_ptr , (u32)title_id_count);

    if( ec_download_ret == ECDOWNLOAD_FAILURE ) {
      ret_flag = FALSE;
      mprintf("%s failed ECDownload 1\n", __FUNCTION__);
    }
    else if( ec_download_ret == ECDOWNLOAD_DUMMY ) {
      ret_flag = FALSE;
      mprintf("%s failed ECDownload 2\n", __FUNCTION__);
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
      mprintf("%s failed EC_Shutdown\n", __FUNCTION__);
    }
    else {
      m_set_palette(tc[0], M_TEXT_COLOR_GREEN );
      mprintf("OK.\n");
    }
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  end_ec:
    // ネットワークからの切断
    mprintf("-LINK DOWN....");
    NHTTP_Cleanup();
  end_nhttp:
    NSSL_Finish();
  end_nssl:
    NcFinish();
  end_nc:
    //miya_log_fprintf(log_fd,"NSSL_Finish() return = %d\n", NSSL_Finish());
    
    TerminateWcmControl();
    
    mprintf("done.\n");
    // EC が自分の Title ID のディレクトリを作成してしまうため、削除する
    DeleteECDirectory();
  end_log_e:
    ;
  }
  else {
    /* mprintf("-Wireless AP conf. loading.. "); */
    ret_flag = FALSE;
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
  }
  m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
}
#endif /* HATAMOTO_LIB */

#define	MY_STACK_SIZE  (1024*128) /* でかいほうがいい */
//#define	MY_THREAD_PRIO        20

#define	MY_THREAD_PRIO        5

static OSThread MyThread;
static u64 MyStack[MY_STACK_SIZE/sizeof(u64)];

static OSMessage MyMesgBuffer_request[1];
static OSMessage MyMesgBuffer_response[1];
static OSMessageQueue MyMesgQueue_request;
static OSMessageQueue MyMesgQueue_response;

static void MyThreadProc(void *arg)
{
#pragma unused(arg)
  OSMessage message;
  BOOL flag;
  u32 command;

  m_set_palette(tc[0], M_TEXT_COLOR_WHITE );

  while( 1 ) {
    //    (void)pre_install_process( NULL, NULL, 0, NULL, 0 , development_console_flag);

    (void)OS_SendMessage(&MyMesgQueue_response, (OSMessage)0, OS_MESSAGE_NOBLOCK);
    (void)OS_ReceiveMessage(&MyMesgQueue_request, &message, OS_MESSAGE_BLOCK);
    flag = TRUE;
    command = (u32)message;
    switch( command ) {
    case THREAD_COMMAND_INSTALL_APP:
      flag = pre_install_command(NULL, eticket_only_id_buf, eticket_only_id_count, 
				 1, development_console_flag );
      break;
    case THREAD_COMMAND_INSTALL_TICKET:
      flag = pre_install_command(NULL, eticket_only_id_buf, eticket_only_id_count, 
				 2, development_console_flag );
      break;
    case THREAD_COMMAND_DELETE_APP_CONTENT:
      flag = pre_install_command(NULL, eticket_only_id_buf, eticket_only_id_count, 
				 3, development_console_flag );
      break;
    case THREAD_COMMAND_DELETE_APP_COMPLETELY:
      flag = pre_install_command(NULL, eticket_only_id_buf, eticket_only_id_count, 
				 4, development_console_flag );
      break;

    case THREAD_COMMAND_DELETE_DEVKP:
#if 0
      {
	char *src_path = "nand:/sys/dev.kp";
	char *dst_path = "sdmc:/dev.kp";
	FSFile f;
	FS_InitFile(&f);
	if( FALSE == FS_OpenFileEx(&f, src_path, FS_FILEMODE_R) ) {
	  if( FALSE == FS_OpenFileEx(&f, dst_path, FS_FILEMODE_R) ) {
	    mprintf("no dev.kp file in NAND & SD\n");
	  }
	  else {
	    FS_CloseFile(&f);
	    (void)CopyFile(src_path, dst_path, NULL );
	    mprintf("copy dev.kp from SD to NAND\n");
	  }
	}
	else {
	  FS_CloseFile(&f);
	  (void)CopyFile(dst_path, src_path, NULL );
	  FS_DeleteFile( src_path );
	  mprintf("move dev.kp from NAND to SD\n");
	}
      }
#endif
      break;
    case THREAD_COMMAND_DOWNLOAD_APP:
#ifdef HATAMOTO_LIB
      ec_download_func();
#endif
      break;

    default:
      flag = FALSE;
      mprintf("%s unknown command!\n",__FUNCTION__);
      break;
    }
    mprintf("\n");
  }
}





static void init_my_thread(void)
{
  OS_InitMessageQueue(&MyMesgQueue_request, &MyMesgBuffer_request[0], 1);
  OS_InitMessageQueue(&MyMesgQueue_response, &MyMesgBuffer_response[0], 1);

  OS_CreateThread(&MyThread, MyThreadProc,
		  NULL, MyStack + MY_STACK_SIZE /sizeof(u64),
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
