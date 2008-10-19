/*---------------------------------------------------------------------------*
  Project:  TWLSDK - demos - MI - ndma-1
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

#include "nuc.h"



//#define  DEBUG_PRINT 1

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

typedef struct {
  RTCDate rtc_date;
  RTCTime rtc_time;
  BOOL shop_record_flag;
  u8 movableUniqueID[ LCFG_TWL_HWINFO_MOVABLE_UNIQUE_ID_LEN ]; // 移行可能なユニークID 16byte
} MyData;

static MyData mydata;

static int vram_num_main = 1;
static int vram_num_sub = 0;

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
  LCFGTWLHWNormalInfo hwn_info;
  LCFGTWLHWSecureInfo hws_info;
  int i;
  int n;
  char path_base[256];
  char path_log[256];
  char path[256];
  u8 macAddress[6];

  OS_Init();
  OS_InitThread();

  OS_InitTick();
  OS_InitAlarm();


  // マスター割り込みフラグを許可に
  (void)OS_EnableIrq();
  
  // IRQ 割り込みを許可します
  (void)OS_EnableInterrupts();


  Gfx_Init();

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

  RTC_Init();

  SCFG_Init();

  NVRAMi_Init();

  SND_Init();
  stream_main();

  // 必須；SEA の初期化
  SEA_Init();

  // 不要：NAM の初期化
  //  NAM_Init(&AllocForNAM, &FreeForNAM);
  
  // 必須：ES の初期化
  ES_InitLib();


  if( FALSE == MiyaReadHWNormalInfo( &hwn_info ) ) {
    mprintf("HW Normal Info. read error\n");
  }
  else {
    mprintf("HW Normal Info. read succeeded.\n");
    mprintf("UniqueID\n 0x");
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
  
  if( FALSE == MiyaReadHWSecureInfo( &hws_info ) ) {
    mprintf("HW Secure Info. - read failed\n");
  }
  else {
    mprintf("HW Secure Info. - read succeeded.\n");
    
    mprintf(" Serial No.\n  0x");
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
    mprintf("\n");
    mprintf("%s\n", hws_info.serialNo);
    
    mprintf(" validLang.bmp = 0x%08x\n", hws_info.validLanguageBitmap );
    mprintf(" wifi disable flag = %d\n", hws_info.flags.forceDisableWireless );
    mprintf(" lchr-TitleIDLo = " );
    for( i = 0 ; i < 4 ; i++ ) {
      mprintf("%02X:", hws_info.launcherTitleID_Lo[i]);
    }
    mprintf("\n Region data = 0x%02x\n\n", hws_info.region );
  }
  



  OS_GetMacAddress( macAddress );
  mprintf("macAddress ");
  for ( i = 0 ; i < 6 ; i++ ) {
    mprintf("%02X:", macAddress[i]);
  }
  mprintf("\n");


  if( FALSE == SDCardValidation() ) {
    sd_card_flag = FALSE;
    mprintf("No SD Card\n");
  }
  else {
    sd_card_flag = TRUE;
    mprintf("Detect SD Card\n");
  }

  FS_RegisterEventHook("sdmc", &sSDHook, SDEvents, NULL);


  STD_StrCpy( path_base , "sdmc:/" );
  STD_StrCat( path_base , (const char *)hws_info.serialNo );
  STD_StrCat( path_base , "/" );




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
    }
    else if ( keyData & PAD_BUTTON_L ) {
    }
    else if ( keyData & PAD_BUTTON_A ) {
      /* ユーザーデータ吸出しモード */
      if( sd_card_flag == TRUE ) {
	
	mprintf("BACKUP to SD Card\n");
	/************************************/

	/* ショップの履歴確認 */
	if( TRUE == CheckShopRecord(NULL) ) {
	  mydata.shop_record_flag = TRUE;
	}
	else {
	  mydata.shop_record_flag = FALSE;
	  mprintf("no shop record\n");
	}



	/* 日時の保存 */
	STD_StrCpy( path , path_base );
	STD_StrCat( path , "twl_rtc.dat" );
	if( RTC_RESULT_SUCCESS != RTC_GetDate( &rtc_date ) ) {
	  mprintf("rtc read date error.\n");
	}
	if( RTC_RESULT_SUCCESS != RTC_GetTime( &rtc_time ) ) {
	  mprintf("rtc read time error.\n");
	}
	
	STD_CopyMemory( (void *)&(mydata.rtc_date), (void *)&rtc_date, sizeof(RTCDate) );
	STD_CopyMemory( (void *)&(mydata.rtc_time), (void *)&rtc_time, sizeof(RTCTime) );
	STD_CopyMemory( (void *)(mydata.movableUniqueID), (void *)hwn_info.movableUniqueID, LCFG_TWL_HWINFO_MOVABLE_UNIQUE_ID_LEN );
	if( TRUE == MydataSave(path, (void *)&mydata, sizeof(MyData), NULL) ) {
	}



	/* nand:/sysディレクトリまわりの保存 */
	STD_StrCpy( path , path_base );
	STD_StrCat( path , "twl_ninfo.dat" );
	if( TRUE == MiyaBackupHWNormalInfo( path ) ) {
	  mprintf("HWInfo Normal backup completed.\n");
	} 

	/* Wifi設定の保存 */
	STD_StrCpy( path , path_base );
	STD_StrCat( path , "twl_nor.bin" );
	if( TRUE == nvram_backup( path ) ) {
	  mprintf("nvram backup completed.\n");
	}

	/* nand:/shared1ディレクトリまわりの保存 */
	STD_StrCpy( path , path_base );
	STD_StrCat( path , "twl_cfg.dat" );
	if( TRUE == MiyaBackupTWLSettings( path ) ) {
	  mprintf("TWL CFG backup completed.\n");
	}


	/* nand:/shared2ディレクトリまわりの保存 */
	STD_StrCpy( path , path_base );
	STD_StrCat( path , "shared2" );
	STD_StrCpy( path_log , path_base );
	STD_StrCat( path_log , "shared2.txt" );
	
	if( 0 == copy_r( &dir_entry_list_head, path, "nand:/shared2" , path_log ) ) {
	  mprintf("copy_r shared2 completed.\n");
	  PrintDirEntryListBackward( dir_entry_list_head, NULL );
	  STD_StrCpy( path , path_base );
	  STD_StrCat( path , "shared2.lst" );
	  SaveDirEntryList( dir_entry_list_head, path );
	}
	(void)ClearDirEntryList( &dir_entry_list_head );


	/* nand2:/photoディレクトリまわりの保存 */
	STD_StrCpy( path , path_base );
	STD_StrCat( path , "photo" );
	STD_StrCpy( path_log , path_base );
	STD_StrCat( path_log , "photolog.txt" );

	if( 0 == copy_r( &dir_entry_list_head, path , "nand2:/photo" , path_log ) ) {
	  mprintf("copy_r photo completed.\n");
	  PrintDirEntryListBackward( dir_entry_list_head, NULL );
	  STD_StrCpy( path , path_base );
	  STD_StrCat( path , "photo.lst" );
	  SaveDirEntryList( dir_entry_list_head, path );
	}
	(void)ClearDirEntryList( &dir_entry_list_head );

	/* nand:/ticketはチケット同期？でうまいこと合わせてくれるんでバックアップ不要 */

	/* nand:/titleディレクトリまわりの保存 */
	/* **.savファイルをすべてバックアップ */

	STD_StrCpy( path , path_base );
	STD_StrCat( path , "title" );
	STD_StrCpy( path_log , path_base );
	STD_StrCat( path_log , "titlelog.txt" );

	if( 0 == find_title_save_data( &dir_entry_list_head, path , "nand:/title", &save_dir_info, path_log ) ) {
	  mprintf("find_title_save_data completed.\n");
	  PrintDirEntryListBackward( dir_entry_list_head, NULL );
	  STD_StrCpy( path , path_base );
	  STD_StrCat( path , "title.lst" );

	  SaveDirEntryList( dir_entry_list_head , path );
	}
	(void)ClearDirEntryList( &dir_entry_list_head );



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
	
	STD_StrCpy( path_log , path_base );
	STD_StrCat( path_log , "title2log.txt" );
	if( 0 == get_title_id( &dir_entry_list_head, "nand:/title", &save_dir_info, path_log ) ) {
	  mprintf("get_title_id completed.\n");

	  // STD_StrCpy( path , path_base );
	  // STD_StrCat( path , "titlelist.txt" );
	  {
	    void *pBuffer;
	    int count;
	    int i,j;
	    char *ptr;
#if 0
	    GetDirEntryList( dir_entry_list_head, &pBuffer, &count);
	    OS_TPrintf("count = %d\n", count );
	    ptr = (char *)pBuffer;

	    if( ptr != NULL && count != 0 )  {
	      for( j = 0 ; j < count ; j++ ) {
		OS_TPrintf("No. %d ",j);
		for( i = 0 ; i < 16 ; i++ ) {
		  OS_TPrintf("%c", *ptr);
		  ptr++;
		}
		OS_TPrintf("\n");
	      }
	      OS_Free(pBuffer);
	    }
#endif

	  }
	  


	  PrintSrcDirEntryListBackward( dir_entry_list_head, NULL );
	}
	(void)ClearDirEntryList( &dir_entry_list_head );

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
    

    

    loop_counter++;

  }
  OS_Terminate();
}


/*====== End of main.c ======*/

