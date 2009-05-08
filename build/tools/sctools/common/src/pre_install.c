#include <twl.h>
#include <twl/sea.h>
#include <twl/lcfg.h>
#include <twl/na.h>
#include <twl/nam.h>
#include <es.h>

#include "font.h"
#include "text.h"
#include "mprintf.h"
#include "logprintf.h"

#include "my_fs_util.h"
#include "myimport.h"

#include "pre_install.h"

#define PRE_INSTALL_TABLE_FILE_NAND "rom:/tads/tad_table.txt"
#define PRE_INSTALL_TABLE_FILE_SD "sdmc:/tads/tad_table.txt"

static PRE_INSTALL_FILE *pre_install_file_list = NULL;


BOOL pre_install_check_download_or_pre_install(u64 tid, int *flag, FSFile *log_fd)
{
  ESError rv;
  ESTicketView *ticketViews;  
  u32 numTickets;

  
#if 0
  typedef u32  ESId;                 /* 32-bit device identity */

  typedef struct {
    ESVersion        version;            /* eTicket data structure version */
    ESTicketId       ticketId;           /* eTicket ID */
    ESId             deviceId;           /* device ID   0->common else->personalized */
    ESTitleId        titleId;            /* title ID */
    ESSysAccessMask  sysAccessMask;      /* 16 bit cidx mask */
    u16              ticketVersion;      /* 16 bit ticket version */
    u32              accessTitleId;      /* 32 bit title id for access control*/
    u32              accessTitleMask;    /* 32 bit title id mask */
    u8               licenseType;        /* for infrastructure use */
    ESTicketReserved reserved;           /* 48 bytes reserved info */
    u8               audit;              /* for infrastructure use */
    ESCidxMask       cidxMask;           /* 512 bits of cidx mask */
    ESLpEntry        limits[ES_MAX_LIMIT_TYPE];  /* limit algorithm and limit */
  } ESTicketView;


  ESError ES_GetTicketViews(ESTitleId titleId,  ESTicketView* ticketViewList, u32* ticketViewCnt);
#endif

  if( flag == NULL ) {
    return FALSE;
  }

  *flag = 0;

  rv = ES_GetTicketViews(tid, NULL, &numTickets);
  if (rv != ES_ERR_OK) {
    miya_log_fprintf(log_fd,"ES_GetTicketViews failed: %d tid=0x%08x%08x\n", 
		     rv, (u32)(tid >> 32) , (u32)(tid & 0xffffffff));
    return FALSE;
  }

  if(numTickets) {
    ticketViews = OS_Alloc(sizeof(ESTicketView) * numTickets);
    rv = ES_GetTicketViews(tid, ticketViews, &numTickets);
    if (rv != ES_ERR_OK) {
      miya_log_fprintf(log_fd,"ES_GetTicketViews failed: %d tid=0x%08x%08x\n", 
		       rv, (u32)(tid >> 32) , (u32)(tid & 0xffffffff) );
      return FALSE;

    }
  }


#if 0
  /*  c:/twlsdk/add-ins/es/es-sdk-20090216/twl/build/demos/sysmenu/src/list.c */
  /* Just use first ticket view for now */
  OS_Printf("Attempting to launch %08x\n", (u32)(t->titleId & 0xffffffff));
  rv = ES_CheckLaunchRights(t->titleId, &t->ticketViews[0]);
#endif

  if( ticketViews[0].deviceId == 0 ) {
    /* common */
    *flag = 1;
  }  
  else {
    /* personalized */
    *flag = 2;
  }

  return TRUE;
}


static BOOL pre_install_add_list(u64 tid, u8 region, u8 country_code, char *temp_file_name, FSFile *log_fd)
{
  PRE_INSTALL_FILE *temp_pre_install_file = NULL;
  PRE_INSTALL_FILE *temp_list;

  if( temp_file_name == NULL ) {
    return FALSE;
  }

  temp_pre_install_file = (PRE_INSTALL_FILE *)OS_Alloc( sizeof(PRE_INSTALL_FILE) );
  if( temp_pre_install_file == NULL ) {
    miya_log_fprintf(log_fd,"Error: %s %s %d\n", __FILE__,__FUNCTION__,__LINE__);    
    return FALSE;
  }
  temp_pre_install_file->tid = tid;
  temp_pre_install_file->region = region;
  temp_pre_install_file->country = country_code;
  temp_pre_install_file->next = NULL;

  STD_StrCpy(temp_pre_install_file->file_name, temp_file_name);

  miya_log_fprintf(log_fd,"add list tid=0x%08x %08x %s\n", (u32)(tid >> 32) , (u32)(tid & 0xffffffff) , temp_file_name);

  
  if( pre_install_file_list == NULL ) {
    pre_install_file_list = temp_pre_install_file;
  }
  else {
    for( temp_list = pre_install_file_list ; temp_list->next != NULL ; temp_list = temp_list->next ) {
      ;
    }
    temp_list->next = temp_pre_install_file;
  }
  
  return TRUE;
}


char *pre_install_search_tid(u64 tid, FSFile *log_fd)
{
  PRE_INSTALL_FILE *temp_list;
  
  for( temp_list = pre_install_file_list ; temp_list != NULL ; temp_list = temp_list->next ) {
    if( temp_list->tid == tid ) {
      miya_log_fprintf(log_fd,"tad file entry tid=0x%08x%08x %s\n", 
		       (u32)(tid >> 32) , (u32)(tid & 0xffffffff), temp_list->file_name );

      return temp_list->file_name;
    }
  }
  miya_log_fprintf(log_fd,"%s %s:No entry tid 0x%08x%08x\n",__FILE__,__FUNCTION__,
		   (u32)(tid >> 32) , (u32)(tid & 0xffffffff));
  return NULL;
  
}

BOOL pre_install_discard_list(void)
{
  PRE_INSTALL_FILE *temp_list;
  PRE_INSTALL_FILE *temp_list2;
  
  temp_list = pre_install_file_list;
  while( temp_list != NULL ) {
    temp_list2 = temp_list->next;
    OS_Free( temp_list );
    temp_list = temp_list2;
  }
  return TRUE;
}




static int ReadLine(FSFile *f, char *buf, int buf_size)
{
  char c;
  s32 readSize;
  int count = 0;
  while( 1 ) {
    readSize = FS_ReadFile(f, (void *)&c, (s32)1 );
    if( readSize == 0 ) {
      /* EOF */
      break;
    }
    count++;
    if( c == '\r' ) {
      *buf = '\0';
      break;
    }
    else if(c == '\n' ) {
      *buf = '\0';
      break;
    }

    *buf = c;
    buf++;
    if( count > buf_size ) {
      buf--;
      *buf = '\0';
      break;
    }


  }
  return count;
}




static int my_char_to_hex(char c)
{
  if( '0' <= c && c <= '9' ) {
    return (int)( c - '0');
  }
  else if( 'A' <= c && c <= 'F' ) {
    return (int)( c - 'A' + 10 );
  } 
  else if( 'a' <= c && c <= 'f' ) {
    return (int)( c - 'a' + 10 );
  } 
  return -1; /* error */
}


BOOL pre_install_load_file(char *path, FSFile *log_fd)
{
  FSFile file;
  BOOL bSuccess = TRUE;
  //  s32 result;
  s32 readSize;
  FSResult fsres;

  int buf_counter;
  int buf_state;

#define LINE_BUF_SIZE 512
  char line_buf[LINE_BUF_SIZE];
  //  PRE_INSTALL_FILE temp_pre_install_file;
  char temp_file_name[FS_FILE_NAME_MAX];


  u64 temp_tid;
  u8 temp_region;
  u8 temp_country_code;
  int temp_hex;
  int temp_filename_count;
  char c;

  FS_InitFile(&file);

  bSuccess = FS_OpenFile(&file, path);
  if( ! bSuccess ) {
    fsres = FS_GetArchiveResultCode(path);
    miya_log_fprintf(log_fd,"Error:%s %s open file\n",__FILE__,__FUNCTION__);
    miya_log_fprintf(log_fd, " Failed open file:%s\n", my_fs_util_get_fs_result_word( fsres ));
    return FALSE; /* open error! */
  }

  /*
    title id
    0x00000000 00000000, region , country code, file name,
   */

  while( 1 ) {
  next_line_read:
    buf_state = 0;
    readSize = ReadLine(&file, line_buf, LINE_BUF_SIZE );
    if( readSize == 0 ) {
      /* EOF */
      OS_TPrintf("%s %s %d : End of File\n", __FILE__,__FUNCTION__,__LINE__);
      break;
    }

    if( !STD_StrNCmp( line_buf, "0x" , STD_StrLen("0x")) ) {

      OS_TPrintf("%s\n", line_buf);

      buf_counter = STD_StrLen("0x");
      buf_state = 1;
      temp_tid = 0;
      temp_region = 0;
      temp_country_code = 0;
      temp_filename_count = 0;
      
      while( readSize > buf_counter ) {
	c = line_buf[buf_counter];

	switch( buf_state ) {
	case 1:	/* TID */
	  if( c == ',') {
	    if( buf_counter == 18 ) {
	      // OS_TPrintf("temp_tid=0x%08x %08x\n", (u32)(temp_tid >> 32) , (u32)(temp_tid & 0xffffffff));
	      buf_state = 2;	/* next state */
	    }
	    else {
	      /* format error */
	      miya_log_fprintf(log_fd,"Error:%s %s %d format error \n",__FILE__,__FUNCTION__,__LINE__);
	      goto next_line_read;
	      // break;
	    }
	  }
	  else {
	    temp_hex = my_char_to_hex(c);
	    if( temp_hex != -1 ) {
	      temp_tid |= ( ((u64)temp_hex) << (64 - (4 * (buf_counter-1))) );
	    }
	    else {
	      /* format error */
	      miya_log_fprintf(log_fd,"Error:%s %s %d format error\n",__FILE__,__FUNCTION__,__LINE__);
	      goto next_line_read;
	      // break;
	    }
	  }
	  break;
	case 2:	/* region */
	  if( c == ' ' ) {
	  }
	  else if( c == ',' ) {
	    buf_state = 3; /* next state */
	  }
	  else {
	    temp_hex = my_char_to_hex(c);
	    if( temp_hex != -1 ) {
	      temp_region = (u8)temp_hex;
	    }
	    else {
	      /* format error */
	      miya_log_fprintf(log_fd,"Error:%s %s %d format error\n",__FILE__,__FUNCTION__,__LINE__);
	      goto next_line_read;
	    }
	  }
	  break;
	case 3:	/* country code */
	  if( c == ' ' ) {
	  }
	  else if( c == ',' ) {
	    buf_state = 4; /* next state */
	  }
	  else {
	    temp_hex = my_char_to_hex(c);
	    if( temp_hex != -1 ) {
	      temp_country_code = (u8)temp_hex;
	    }
	    else {
	      /* format error */
	      miya_log_fprintf(log_fd,"Error:%s %s %d format error\n",__FILE__,__FUNCTION__,__LINE__);
	      goto next_line_read;
	    }
	  }
	  break;

	case 4: /* file name */
	  if( c == ',' ) {
	    temp_file_name[temp_filename_count] = '\0';
	    /* add list */
	    if( TRUE != pre_install_add_list(temp_tid, temp_region, temp_country_code, temp_file_name, log_fd) ) {
	      OS_TPrintf("Error: add list error %s %s %d\n", __FILE__,__FUNCTION__,__LINE__);
	    }
	    buf_state = 5; /* next state */	    
	  }
	  else {
	    temp_file_name[temp_filename_count] = c;
	    temp_filename_count++;
	  }
	  break;
	case 5: /* until line end */
	  break;
	default:
	  /* error */
	  break;
	}
	buf_counter++;
      }


    }
    else {
      /* 妙なフォーマットは全部コメント扱い. */
    }
  }

label_last:
  (void)FS_CloseFile(&file);

  return bSuccess;
}



BOOL pre_install_Cleanup_User_Titles( FSFile *log_fd )
{
#define NAM_TITLE_ID_S 128

  NAMTitleId pArray[NAM_TITLE_ID_S];
  s32 i;
  BOOL ret_flag = TRUE;
  s32 num = 0;
  u64 tid;
  char game_code_buf[5];

  num = NAM_GetNumTitles();
  if( num > 0 ) {
    if( NAM_OK !=  NAM_GetTitleList( pArray , NAM_TITLE_ID_S ) ) {
      return FALSE;
    }
    miya_log_fprintf(log_fd, "NAND Installed titles\n");

    mprintf("Clean-up NAND installed titles\n");

    for( i = 0 ; i < num ; i++ ) {
      tid = pArray[i];

      /*
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
      (void)my_fs_Tid_To_GameCode(tid, game_code_buf);
      if( tid & 0x0000000100000000 ) {
	/* system app. */
	miya_log_fprintf(log_fd, " sys.id %08X %08X [%s]\n", (u32)(tid >> 32), (u32)tid, game_code_buf);
      }
      else {
	/* user app. */
	mprintf(" id %08X %08X [%s] ", (u32)(tid >> 32), (u32)tid, game_code_buf);
	miya_log_fprintf(log_fd, " usr.id %08X %08X [%s] ", (u32)(tid >> 32), (u32)tid, game_code_buf);


	if( NAM_OK != NAM_DeleteTitle( tid ) ) {
	  m_set_palette(tc[0], M_TEXT_COLOR_RED );	/* green  */
	  mprintf("NG.\n");
	  m_set_palette(tc[0], M_TEXT_COLOR_WHITE );

	  miya_log_fprintf(log_fd, "NG.\n");
	  miya_log_fprintf(log_fd, " Error: NAM_DeleteTitle id = %08X%08X\n", (u32)(tid >> 32), (u32)tid);
	  ret_flag = FALSE;
	}
	else {
	  m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
	  mprintf("OK.\n");
	  m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
	  miya_log_fprintf(log_fd, "OK.\n");
	}
      }
    }
  }
  return ret_flag;
}


BOOL pre_install_process( FSFile *log_fd, MY_USER_APP_TID *title_id_buf_ptr, int title_id_count )
{
  char *tad_file_name;
  int i;
  u64 tid;
  char game_code_buf[5];
  BOOL ret_flag = TRUE;


#if 0
  /* セキュリティ的にちょっと・・・ */
  (void)pre_install_load_file(PRE_INSTALL_TABLE_FILE_SD, log_fd);
#endif

  (void)pre_install_load_file(PRE_INSTALL_TABLE_FILE_NAND, log_fd);

  for( i = 0 ; i < title_id_count ; i++ ) {
    /* 
       pTitleIds[i].is_personalized = 1 -> common (pre installed)
       pTitleIds[i].is_personalized = 2 -> personalized
    */
    if( title_id_buf_ptr[i].is_personalized == 1 ) {
      /* common e-ticketのやつ */
      /*
	0x00030004484E474A "rom:/tads/TWL-KGUJ-v257.tad.out"
	0x000300044B32444A "rom:/tads/TWL-K2DJ-v0.tad.out"
	0x000300044B47554A "rom:/tads/TWL-HNGJ-v256.tad.out"
      */
      tid = title_id_buf_ptr[i].tid;
      (void)my_fs_Tid_To_GameCode(tid, game_code_buf);
      mprintf(" id %08X %08X [%s] ", (u32)(tid >> 32), (u32)tid, game_code_buf);
      miya_log_fprintf(log_fd, " id %08X %08X [%s] ", (u32)(tid >> 32), (u32)tid, game_code_buf);

      tad_file_name = pre_install_search_tid( tid , log_fd);
      if( tad_file_name ) {
	if( FALSE == myImportTad( tad_file_name , log_fd) ) {
	  miya_log_fprintf(log_fd, "NG.\n");
	  m_set_palette(tc[0], M_TEXT_COLOR_RED );	/* green  */
	  mprintf("NG.\n");
	  m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
	  miya_log_fprintf(log_fd, "error: import tid=0x%08x%08x %s\n", 
			   (u32)(tid >> 32) , (u32)(tid & 0xffffffff), tad_file_name);
	  ret_flag = FALSE;
	}
	else {
	  miya_log_fprintf(log_fd, "OK.\n");
	  m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
	  mprintf("OK.\n");
	  m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
	  title_id_buf_ptr[i].install_success_flag = TRUE;
	}
      }
      else {
	miya_log_fprintf(log_fd, "No file\n");
	m_set_palette(tc[0], M_TEXT_COLOR_RED );	/* green  */
	mprintf("No file\n");
	m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
	miya_log_fprintf(log_fd, "error: no file\n"); 
	ret_flag = FALSE;
      }
    }
    else {
      /* personalized e-ticketのやつ */
    }
  }
  (void)pre_install_discard_list();
  return ret_flag;
}



#if 0

// リージョンコード
typedef enum OSTWLRegionCode
{
    OS_TWL_REGION_JAPAN     = 0,
    OS_TWL_REGION_AMERICA   = 1,
    OS_TWL_REGION_EUROPE    = 2,
    OS_TWL_REGION_AUSTRALIA = 3,
    OS_TWL_REGION_CHINA     = 4,
    OS_TWL_REGION_KOREA     = 5,
    OS_TWL_REGION_MAX
} OSTWLRegion;


// 言語設定コード
typedef enum LCFGTWLCountryCode
{
    LCFG_TWL_COUNTRY_UNDEFINED  = 0,        // 未設定

    // JPNリージョン
    LCFG_TWL_COUNTRY_JAPAN      = 1,        // 日本

    // USAリージョン
    LCFG_TWL_COUNTRY_Anguilla   = 8,        // アンギラ
    LCFG_TWL_COUNTRY_ANTIGUA_AND_BARBUDA,   // アンティグア・バーブーダ
    LCFG_TWL_COUNTRY_ARGENTINA   = 10,      // アルゼンチン
    LCFG_TWL_COUNTRY_ARUBA,                 // アルバ
    LCFG_TWL_COUNTRY_BAHAMAS,               // バハマ
    LCFG_TWL_COUNTRY_BARBADOS,              // バルバドス
    LCFG_TWL_COUNTRY_BELIZE,                // ベリーズ
    LCFG_TWL_COUNTRY_BOLIVIA,               // ボリビア
    LCFG_TWL_COUNTRY_BRAZIL,                // ブラジル
    LCFG_TWL_COUNTRY_BRITISH_VIRGIN_ISLANDS,    // 英領ヴァージン諸島
    LCFG_TWL_COUNTRY_CANADA,                // カナダ
    LCFG_TWL_COUNTRY_CAYMAN_ISLANDS,        // ケイマン諸島
    LCFG_TWL_COUNTRY_CHILE       = 20,      // チリ
    LCFG_TWL_COUNTRY_COLOMBIA,              // コロンビア
    LCFG_TWL_COUNTRY_COSTA_RICA,            // コスタリカ
    LCFG_TWL_COUNTRY_DOMINICA,              // ドミニカ国
    LCFG_TWL_COUNTRY_DOMINICAN_REPUBLIC,    // ドミニカ共和国
    LCFG_TWL_COUNTRY_ECUADOR,               // エクアドル
    LCFG_TWL_COUNTRY_EL_SALVADOR,           // エルサルバドル
    LCFG_TWL_COUNTRY_FRENCH_GUIANA,         // フランス領ギアナ
    LCFG_TWL_COUNTRY_GRENADA,               // グレナダ
    LCFG_TWL_COUNTRY_GUADELOUPE,            // グアドループ
    LCFG_TWL_COUNTRY_GUATEMALA   = 30,      // グアテマラ
    LCFG_TWL_COUNTRY_GUYANA,                // ガイアナ
    LCFG_TWL_COUNTRY_HAITI,                 // ハイチ
    LCFG_TWL_COUNTRY_HONDURAS,              // ホンジュラス
    LCFG_TWL_COUNTRY_JAMAICA,               // ジャマイカ
    LCFG_TWL_COUNTRY_MARTINIQUE,            // マルティニーク
    LCFG_TWL_COUNTRY_MEXICO,                // メキシコ
    LCFG_TWL_COUNTRY_MONTSERRAT,            // モントセラト
    LCFG_TWL_COUNTRY_NETHERLANDS_ANTILLES,  // オランダ領アンティル
    LCFG_TWL_COUNTRY_NICARAGUA,             // ニカラグア
    LCFG_TWL_COUNTRY_PANAMA      = 40,      // パナマ
    LCFG_TWL_COUNTRY_PARAGUAY,              // パラグアイ
    LCFG_TWL_COUNTRY_PERU,                  // ペルー
    LCFG_TWL_COUNTRY_ST_KITTS_AND_NEVIS,    // セントキッツ・ネイビス
    LCFG_TWL_COUNTRY_ST_LUCIA,              // セントルシア
    LCFG_TWL_COUNTRY_ST_VINCENT_AND_THE_GRENADINES, // セントビンセント・グレナディーン
    LCFG_TWL_COUNTRY_SURINAME,              // スリナム
    LCFG_TWL_COUNTRY_TRINIDAD_AND_TOBAGO,   // トリニダード・トバゴ
    LCFG_TWL_COUNTRY_TURKS_AND_CAICOS_ISLANDS,  // タークス・カイコス諸島
    LCFG_TWL_COUNTRY_UNITED_STATES,         // アメリカ
    LCFG_TWL_COUNTRY_URUGUAY     = 50,      // ウルグアイ
    LCFG_TWL_COUNTRY_US_VIRGIN_ISLANDS,     // 米領バージン諸島
    LCFG_TWL_COUNTRY_VENEZUELA,             // ベネズエラ

    // EUR, NAL リージョン
    LCFG_TWL_COUNTRY_ALBANIA     = 64,      // アルバニア
    LCFG_TWL_COUNTRY_AUSTRALIA,             // オーストラリア
    LCFG_TWL_COUNTRY_AUSTRIA,               // オーストリア
    LCFG_TWL_COUNTRY_BELGIUM,               // ベルギー
    LCFG_TWL_COUNTRY_BOSNIA_AND_HERZEGOVINA,    // ボスニア・ヘルツェゴビナ
    LCFG_TWL_COUNTRY_BOTSWANA,              // ボツワナ
    LCFG_TWL_COUNTRY_BULGARIA    = 70,      // ブルガリア
    LCFG_TWL_COUNTRY_CROATIA,               // クロアチア
    LCFG_TWL_COUNTRY_CYPRUS,                // キプロス
    LCFG_TWL_COUNTRY_CZECH_REPUBLIC,        // チェコ
    LCFG_TWL_COUNTRY_DENMARK,               // デンマーク
    LCFG_TWL_COUNTRY_ESTONIA,               // エストニア
    LCFG_TWL_COUNTRY_FINLAND,               // フィンランド
    LCFG_TWL_COUNTRY_FRANCE,                // フランス
    LCFG_TWL_COUNTRY_GERMANY,               // ドイツ
    LCFG_TWL_COUNTRY_GREECE,                // ギリシャ
    LCFG_TWL_COUNTRY_HUNGARY     = 80,      // ハンガリー
    LCFG_TWL_COUNTRY_ICELAND,               // アイスランド
    LCFG_TWL_COUNTRY_IRELAND,               // アイルランド
    LCFG_TWL_COUNTRY_ITALY,                 // イタリア
    LCFG_TWL_COUNTRY_LATVIA,                // ラトビア
    LCFG_TWL_COUNTRY_LESOTHO,               // レソト
    LCFG_TWL_COUNTRY_LIECHTENSTEIN,         // リヒテンシュタイン
    LCFG_TWL_COUNTRY_LITHUANIA,             // リトアニア
    LCFG_TWL_COUNTRY_LUXEMBOURG,            // ルクセンブルク
    LCFG_TWL_COUNTRY_MACEDONIA,             // マケドニア
    LCFG_TWL_COUNTRY_MALTA       = 90,      // マルタ
    LCFG_TWL_COUNTRY_MONTENEGRO,            // モンテネグロ
    LCFG_TWL_COUNTRY_MOZAMBIQUE,            // モザンビーク
    LCFG_TWL_COUNTRY_NAMIBIA,               // ナミビア
    LCFG_TWL_COUNTRY_NETHERLANDS,           // オランダ
    LCFG_TWL_COUNTRY_NEW_ZEALAND,           // ニュージーランド
    LCFG_TWL_COUNTRY_NORWAY,                // ノルウェー
    LCFG_TWL_COUNTRY_POLAND,                // ポーランド
    LCFG_TWL_COUNTRY_PORTUGAL,              // ポルトガル
    LCFG_TWL_COUNTRY_ROMANIA,               // ルーマニア
    LCFG_TWL_COUNTRY_RUSSIA      = 100,     // ロシア
    LCFG_TWL_COUNTRY_SERBIA,                // セルビア
    LCFG_TWL_COUNTRY_SLOVAKIA,              // スロバキア
    LCFG_TWL_COUNTRY_SLOVENIA,              // スロベニア
    LCFG_TWL_COUNTRY_SOUTH_AFRICA,          // 南アフリカ
    LCFG_TWL_COUNTRY_SPAIN,                 // スペイン
    LCFG_TWL_COUNTRY_SWAZILAND,             // スワジランド
    LCFG_TWL_COUNTRY_SWEDEN,                // スウェーデン
    LCFG_TWL_COUNTRY_SWITZERLAND,           // スイス
    LCFG_TWL_COUNTRY_TURKEY,                // トルコ
    LCFG_TWL_COUNTRY_UNITED_KINGDOM = 110,  // イギリス
    LCFG_TWL_COUNTRY_ZAMBIA,                // ザンビア
    LCFG_TWL_COUNTRY_ZIMBABWE,              // ジンバブエ

    // TWNリージョン
    LCFG_TWL_COUNTRY_TAIWAN      = 128,     // 台湾
    
    // KORリージョン
    LCFG_TWL_COUNTRY_SOUTH_KOREA = 136,     // 韓国
    
    // HKGリージョン（Wiiの国リストに存在）
    LCFG_TWL_COUNTRY_HONG_KONG   = 144,     // ホンコン
    LCFG_TWL_COUNTRY_MACAU,                 // マカオ
    
    // ASIリージョン（Wiiの国リストに存在）
    LCFG_TWL_COUNTRY_INDONESIA   = 152,     // インドネシア
    
    // USAリージョン
    LCFG_TWL_COUNTRY_SINGAPORE   = 153,     // シンガポール
    
    // ASIリージョン（再び）
    LCFG_TWL_COUNTRY_THAILAND    = 154,     // タイ
    LCFG_TWL_COUNTRY_PHILIPPINES,           // フィリピン
    LCFG_TWL_COUNTRY_MALAYSIA,              // マレーシア
    
    // 未定義リージョン（IQueリージョン？）
    LCFG_TWL_COUNTRY_CHINA       = 160,     // 中国
    
    // USAリージョン
    LCFG_TWL_COUNTRY_UAE         = 168,     // アラブ首長国連邦
    
    // 未定義リージョン
    LCFG_TWL_COUNTRY_INDIA       = 169,     // インド
    LCFG_TWL_COUNTRY_EGYPT       = 170,     // エジプト
    LCFG_TWL_COUNTRY_OMAN,                  // オマーン
    LCFG_TWL_COUNTRY_QATAR,                 // カタール
    LCFG_TWL_COUNTRY_KUWAIT,                // クウェート
    LCFG_TWL_COUNTRY_SAUDI_ARABIA,          // サウジアラビア
    LCFG_TWL_COUNTRY_SYRIA,                 // シリア
    LCFG_TWL_COUNTRY_BAHRAIN,               // バーレーン
    LCFG_TWL_COUNTRY_JORDAN,                // ヨルダン

    LCFG_TWL_COUNTRY_OTHERS      = 254,
    LCFG_TWL_COUNTRY_UNKNOWN     = 255,
    
    LCFG_TWL_COUNTRY_MAX
    
} LCFGTWLCountryCode;

#endif
