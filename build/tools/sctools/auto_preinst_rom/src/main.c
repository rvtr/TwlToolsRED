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
#include        "hwi.h"
#include        "mydata.h"

#include        "error_report.h"

#include        "myfilename.h"

#include        "pre_install.h"

#define THREAD_COMMAND_INSTALL_APP           1
#define THREAD_COMMAND_INSTALL_TICKET        2
#define THREAD_COMMAND_DELETE_APP_CONTENT    3
#define THREAD_COMMAND_DELETE_APP_COMPLETELY 4


static void init_my_thread(void);
static BOOL start_my_thread(u32 command);


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
    /*
      0x00030004484e474a, 0 , 0 , rom:/tads/TWL-HNGJ-v256.tad,
      0x000300044b32444a, 0 , 0 , rom:/tads/TWL-K2DJ-v0.tad,
      0x000300044b47554a, 0 , 0 , rom:/tads/TWL-KGUJ-v257.tad,
    */
    eticket_only_id_buf[0] =  0x00030004484e474a;
    eticket_only_id_buf[1] =  0x000300044b32444a;
    eticket_only_id_buf[2] =  0x000300044b47554a;
    eticket_only_id_count = 3;
  }
  else {
    /*
      0x0003000434424e41, 0 , 0 , rom:/tads_dev/backupSample.tad,
      0x00030004344a4541, 0 , 0 , rom:/tads_dev/encodeSD.tad,
      0x0003000434534e41, 0 , 0 , rom:/tads_dev/nandAppSample.tad,
    */
    eticket_only_id_buf[2] =  0x0003000434424e41;
    eticket_only_id_buf[1] =  0x00030004344a4541;
    eticket_only_id_buf[0] =  0x0003000434534e41;
    eticket_only_id_count = 3;
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

  // NAM の初期化
  NAM_Init(&AllocForNAM, &FreeForNAM);


  init_my_thread();


  {
    double d;
    d = 0.1234567789;
    mprintf("test %f\n", d);
  }


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

    if( pushed_power_button == TRUE ) {
      OS_TPrintf("%s Power button pressed!\n",__FUNCTION__);
      PM_ReadyToExit();
      pushed_power_button = FALSE;
    }

    loop_counter++;
  }
  OS_Terminate();

}



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
