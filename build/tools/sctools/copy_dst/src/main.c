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



#define	MY_STACK_SIZE  (1024*16) /* �ł����ق������� */
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
  /* ��{�I�ɂ�����̋t�����(Get -> Set, Save->Load�Ȃ�) */
  /* 
     ���Ȃ݂ɂ��ł�MydataLoad�֐��͐������Ă�����̂Ƃ���B
     ���������� MyData mydata �ɂ̓f�[�^�������Ă���B
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
  /* ���łɃu�[�g���Ɉ�x hwn_info �̓��[�h���Ă���B */
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
  int i;  /* ���[�U�[�f�[�^�������݃��[�h */
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

  /* hws_info.serialNo�͖߂��Ȃ� */
  /*  */
  // static BOOL SDBackupToSDCard7(void)

  // for DEBUG
  //  mydata.shop_record_flag = TRUE; 


  if( mydata.shop_record_flag == FALSE ) {
    /* �l�b�g���[�N�ɂȂ��Ȃ��Ă������H */
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

      /* nand:/ticket�̓`�P�b�g�����Ń_�E�����[�h */
      // �s�v�F�f�o�C�X���̕\��
      PrintDeviceInfo();

      OS_TPrintf("--------------------------------\n");

      // setup
      // �K�{�F�^�C�g�� ID �̋U��
      SetupShopTitleId();
    
      // �H�F���[�U�ݒ肪����Ă��Ȃ��Ɛڑ��ł��Ȃ��̂œK���ɐݒ�
      SetupUserInfo();
    
      // �K�{�F�o�[�W�����f�[�^�̃}�E���g
      SetupVerData();
    
      // �K�{�F�l�b�g���[�N�ւ̐ڑ�
      NcStart(SITEDEFS_DEFAULTCLASS);
    
      /******** �l�b�g���[�N�ɂȂ��� *************/

      // �K�{�FHTTP �� SSL �̏�����
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

      /******** NHTTP & NSSL�ɂȂ��� *************/
    
      // �K�{�FEC �̏�����
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

      // �K�{�F�f�o�C�X�ؖ����̔��s
      if( FALSE == KPSClient() ) {
	goto end_ec_f;
      }

      if( FALSE == ECDownload((NAMTitleId *)title_id_buf_ptr , (u32)title_id_count) ) {
	goto end_ec_f;
      }

      // �s�v�F�Z�[�u�f�[�^�̈���쐬
      // NAM_Init ��Y��Ă�
      SetupTitlesDataFile((NAMTitleId *)title_id_buf_ptr , (u32)title_id_count);

    end_ec_f:
      // cleanup
      // EC �̏I������
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

    
      // �l�b�g���[�N����̐ؒf
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
      // EC �������� Title ID �̃f�B���N�g�����쐬���Ă��܂����߁A�폜����
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
    /* MydataLoad�͂��łɂ���Ă���̂ł���Ȃ��B */
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
    /* �S������������t�H���_������ */
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
      /* NSSL_Init()�Ă�ł̓_���I */
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

	/* �n�[�h�E�F�A���Z�b�g���s���A�������g���N�����܂��B */
	OS_Sleep(30000);
	OS_RebootSystem();
      }
      else {
	// NUC_Init() failed, error code=34416 �͂ǂ�����NSSL_Init���Ă�ł�̂ŏ����B
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


  // �}�X�^�[���荞�݃t���O������
  (void)OS_EnableIrq();
  
  // IRQ ���荞�݂������܂�
  (void)OS_EnableInterrupts();

  // ARM7�Ƃ̒ʐMFIFO���荞�݋���
  (void)OS_EnableIrqMask(OS_IE_SPFIFO_RECV);

  // �t�@�C���V�X�e��������
  FS_Init( FS_DMA_NOT_USE );


  // ���C���A���[�i�̃A���P�[�g�V�X�e����������
  newArenaLo = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 1);
  OS_SetMainArenaLo(newArenaLo);
  
  // ���C���A���[�i��Ƀq�[�v���쐬
  hHeap = OS_CreateHeap(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi());
  OS_SetCurrentHeap(OS_ARENA_MAIN, hHeap);

  Gfx_Init();

  RTC_Init();

  SCFG_Init();

  NVRAMi_Init();

  SND_Init();
  stream_main();

  // �K�{�GSEA �̏�����
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

  /* �f�o�b�O�̂��߂ɋ����I�� */
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


    /* �ŏ��Ƀl�b�g���[�N�A�b�v�f�[�g�B */
    //  NSSL_Init(&s_sslConfig);
    //    SetupNSSL();
    if (!NA_LoadVersionDataArchive(WorkForNA, NA_VERSION_DATA_WORK_SIZE)) {
      OS_TPrintf("NA load error\n");
      mprintf("NA load error\n");
    }
    else {
      if (!NUC_LoadCert()) {
	// WRAM�Ƀ��[�h
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

      // �`�q�l�V�R�}���h������M
      while (SND_RecvCommandReply(SND_COMMAND_NOBLOCK) != NULL)
	{
	}
      // �R�}���h�t���b�V���i�t���b�V�����đ����Ɏ��s��v���j
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
    // �s�v�FNAM �̏�����
    NAM_Init(&AllocForNAM, &FreeForNAM);
    
    // �K�{�FES �̏�����
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

      // �`�q�l�V�R�}���h������M
      while (SND_RecvCommandReply(SND_COMMAND_NOBLOCK) != NULL)
	{
	}
      // �R�}���h�t���b�V���i�t���b�V�����đ����Ɏ��s��v���j
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
	/* ���[�U�[�f�[�^�z�o�����[�h */
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
// DWC_GetProxySetting �֐��ŁA�ڑ����Ă���ݒ��proxy�ݒ���i�[����\����
typedef struct DWCstProxySetting			// Proxy �ݒ���
{
    u16         authType;					// Proxy �F�،`��
    u16         port;						// Proxy port
    u8          hostName      [ 0x64 ];		// Proxy hostname
    u8          authId        [ 0x20 ];		// proxy basic�F�ؗp ID
    u8          authPass      [ 0x20 ];		// proxy basic�F�ؗp �p�X���[�h
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

