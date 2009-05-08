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
      /* ���ȃt�H�[�}�b�g�͑S���R�����g����. */
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
                     | �����̍ŉ��ʃr�b�g���P�̂���V�X�e���A�v��
                     | 
  		 �V�X�e���A�v���̓_�E�����[�h�ΏۊO
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
  /* �Z�L�����e�B�I�ɂ�����ƁE�E�E */
  (void)pre_install_load_file(PRE_INSTALL_TABLE_FILE_SD, log_fd);
#endif

  (void)pre_install_load_file(PRE_INSTALL_TABLE_FILE_NAND, log_fd);

  for( i = 0 ; i < title_id_count ; i++ ) {
    /* 
       pTitleIds[i].is_personalized = 1 -> common (pre installed)
       pTitleIds[i].is_personalized = 2 -> personalized
    */
    if( title_id_buf_ptr[i].is_personalized == 1 ) {
      /* common e-ticket�̂�� */
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
      /* personalized e-ticket�̂�� */
    }
  }
  (void)pre_install_discard_list();
  return ret_flag;
}



#if 0

// ���[�W�����R�[�h
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


// ����ݒ�R�[�h
typedef enum LCFGTWLCountryCode
{
    LCFG_TWL_COUNTRY_UNDEFINED  = 0,        // ���ݒ�

    // JPN���[�W����
    LCFG_TWL_COUNTRY_JAPAN      = 1,        // ���{

    // USA���[�W����
    LCFG_TWL_COUNTRY_Anguilla   = 8,        // �A���M��
    LCFG_TWL_COUNTRY_ANTIGUA_AND_BARBUDA,   // �A���e�B�O�A�E�o�[�u�[�_
    LCFG_TWL_COUNTRY_ARGENTINA   = 10,      // �A���[���`��
    LCFG_TWL_COUNTRY_ARUBA,                 // �A���o
    LCFG_TWL_COUNTRY_BAHAMAS,               // �o�n�}
    LCFG_TWL_COUNTRY_BARBADOS,              // �o���o�h�X
    LCFG_TWL_COUNTRY_BELIZE,                // �x���[�Y
    LCFG_TWL_COUNTRY_BOLIVIA,               // �{���r�A
    LCFG_TWL_COUNTRY_BRAZIL,                // �u���W��
    LCFG_TWL_COUNTRY_BRITISH_VIRGIN_ISLANDS,    // �p�̃��@�[�W������
    LCFG_TWL_COUNTRY_CANADA,                // �J�i�_
    LCFG_TWL_COUNTRY_CAYMAN_ISLANDS,        // �P�C�}������
    LCFG_TWL_COUNTRY_CHILE       = 20,      // �`��
    LCFG_TWL_COUNTRY_COLOMBIA,              // �R�����r�A
    LCFG_TWL_COUNTRY_COSTA_RICA,            // �R�X�^���J
    LCFG_TWL_COUNTRY_DOMINICA,              // �h�~�j�J��
    LCFG_TWL_COUNTRY_DOMINICAN_REPUBLIC,    // �h�~�j�J���a��
    LCFG_TWL_COUNTRY_ECUADOR,               // �G�N�A�h��
    LCFG_TWL_COUNTRY_EL_SALVADOR,           // �G���T���o�h��
    LCFG_TWL_COUNTRY_FRENCH_GUIANA,         // �t�����X�̃M�A�i
    LCFG_TWL_COUNTRY_GRENADA,               // �O���i�_
    LCFG_TWL_COUNTRY_GUADELOUPE,            // �O�A�h���[�v
    LCFG_TWL_COUNTRY_GUATEMALA   = 30,      // �O�A�e�}��
    LCFG_TWL_COUNTRY_GUYANA,                // �K�C�A�i
    LCFG_TWL_COUNTRY_HAITI,                 // �n�C�`
    LCFG_TWL_COUNTRY_HONDURAS,              // �z���W�����X
    LCFG_TWL_COUNTRY_JAMAICA,               // �W���}�C�J
    LCFG_TWL_COUNTRY_MARTINIQUE,            // �}���e�B�j�[�N
    LCFG_TWL_COUNTRY_MEXICO,                // ���L�V�R
    LCFG_TWL_COUNTRY_MONTSERRAT,            // �����g�Z���g
    LCFG_TWL_COUNTRY_NETHERLANDS_ANTILLES,  // �I�����_�̃A���e�B��
    LCFG_TWL_COUNTRY_NICARAGUA,             // �j�J���O�A
    LCFG_TWL_COUNTRY_PANAMA      = 40,      // �p�i�}
    LCFG_TWL_COUNTRY_PARAGUAY,              // �p���O�A�C
    LCFG_TWL_COUNTRY_PERU,                  // �y���[
    LCFG_TWL_COUNTRY_ST_KITTS_AND_NEVIS,    // �Z���g�L�b�c�E�l�C�r�X
    LCFG_TWL_COUNTRY_ST_LUCIA,              // �Z���g���V�A
    LCFG_TWL_COUNTRY_ST_VINCENT_AND_THE_GRENADINES, // �Z���g�r���Z���g�E�O���i�f�B�[��
    LCFG_TWL_COUNTRY_SURINAME,              // �X���i��
    LCFG_TWL_COUNTRY_TRINIDAD_AND_TOBAGO,   // �g���j�_�[�h�E�g�o�S
    LCFG_TWL_COUNTRY_TURKS_AND_CAICOS_ISLANDS,  // �^�[�N�X�E�J�C�R�X����
    LCFG_TWL_COUNTRY_UNITED_STATES,         // �A�����J
    LCFG_TWL_COUNTRY_URUGUAY     = 50,      // �E���O�A�C
    LCFG_TWL_COUNTRY_US_VIRGIN_ISLANDS,     // �ė̃o�[�W������
    LCFG_TWL_COUNTRY_VENEZUELA,             // �x�l�Y�G��

    // EUR, NAL ���[�W����
    LCFG_TWL_COUNTRY_ALBANIA     = 64,      // �A���o�j�A
    LCFG_TWL_COUNTRY_AUSTRALIA,             // �I�[�X�g�����A
    LCFG_TWL_COUNTRY_AUSTRIA,               // �I�[�X�g���A
    LCFG_TWL_COUNTRY_BELGIUM,               // �x���M�[
    LCFG_TWL_COUNTRY_BOSNIA_AND_HERZEGOVINA,    // �{�X�j�A�E�w���c�F�S�r�i
    LCFG_TWL_COUNTRY_BOTSWANA,              // �{�c���i
    LCFG_TWL_COUNTRY_BULGARIA    = 70,      // �u���K���A
    LCFG_TWL_COUNTRY_CROATIA,               // �N���A�`�A
    LCFG_TWL_COUNTRY_CYPRUS,                // �L�v���X
    LCFG_TWL_COUNTRY_CZECH_REPUBLIC,        // �`�F�R
    LCFG_TWL_COUNTRY_DENMARK,               // �f���}�[�N
    LCFG_TWL_COUNTRY_ESTONIA,               // �G�X�g�j�A
    LCFG_TWL_COUNTRY_FINLAND,               // �t�B�������h
    LCFG_TWL_COUNTRY_FRANCE,                // �t�����X
    LCFG_TWL_COUNTRY_GERMANY,               // �h�C�c
    LCFG_TWL_COUNTRY_GREECE,                // �M���V��
    LCFG_TWL_COUNTRY_HUNGARY     = 80,      // �n���K���[
    LCFG_TWL_COUNTRY_ICELAND,               // �A�C�X�����h
    LCFG_TWL_COUNTRY_IRELAND,               // �A�C�������h
    LCFG_TWL_COUNTRY_ITALY,                 // �C�^���A
    LCFG_TWL_COUNTRY_LATVIA,                // ���g�r�A
    LCFG_TWL_COUNTRY_LESOTHO,               // ���\�g
    LCFG_TWL_COUNTRY_LIECHTENSTEIN,         // ���q�e���V���^�C��
    LCFG_TWL_COUNTRY_LITHUANIA,             // ���g�A�j�A
    LCFG_TWL_COUNTRY_LUXEMBOURG,            // ���N�Z���u���N
    LCFG_TWL_COUNTRY_MACEDONIA,             // �}�P�h�j�A
    LCFG_TWL_COUNTRY_MALTA       = 90,      // �}���^
    LCFG_TWL_COUNTRY_MONTENEGRO,            // �����e�l�O��
    LCFG_TWL_COUNTRY_MOZAMBIQUE,            // ���U���r�[�N
    LCFG_TWL_COUNTRY_NAMIBIA,               // �i�~�r�A
    LCFG_TWL_COUNTRY_NETHERLANDS,           // �I�����_
    LCFG_TWL_COUNTRY_NEW_ZEALAND,           // �j���[�W�[�����h
    LCFG_TWL_COUNTRY_NORWAY,                // �m���E�F�[
    LCFG_TWL_COUNTRY_POLAND,                // �|�[�����h
    LCFG_TWL_COUNTRY_PORTUGAL,              // �|���g�K��
    LCFG_TWL_COUNTRY_ROMANIA,               // ���[�}�j�A
    LCFG_TWL_COUNTRY_RUSSIA      = 100,     // ���V�A
    LCFG_TWL_COUNTRY_SERBIA,                // �Z���r�A
    LCFG_TWL_COUNTRY_SLOVAKIA,              // �X���o�L�A
    LCFG_TWL_COUNTRY_SLOVENIA,              // �X���x�j�A
    LCFG_TWL_COUNTRY_SOUTH_AFRICA,          // ��A�t���J
    LCFG_TWL_COUNTRY_SPAIN,                 // �X�y�C��
    LCFG_TWL_COUNTRY_SWAZILAND,             // �X���W�����h
    LCFG_TWL_COUNTRY_SWEDEN,                // �X�E�F�[�f��
    LCFG_TWL_COUNTRY_SWITZERLAND,           // �X�C�X
    LCFG_TWL_COUNTRY_TURKEY,                // �g���R
    LCFG_TWL_COUNTRY_UNITED_KINGDOM = 110,  // �C�M���X
    LCFG_TWL_COUNTRY_ZAMBIA,                // �U���r�A
    LCFG_TWL_COUNTRY_ZIMBABWE,              // �W���o�u�G

    // TWN���[�W����
    LCFG_TWL_COUNTRY_TAIWAN      = 128,     // ��p
    
    // KOR���[�W����
    LCFG_TWL_COUNTRY_SOUTH_KOREA = 136,     // �؍�
    
    // HKG���[�W�����iWii�̍����X�g�ɑ��݁j
    LCFG_TWL_COUNTRY_HONG_KONG   = 144,     // �z���R��
    LCFG_TWL_COUNTRY_MACAU,                 // �}�J�I
    
    // ASI���[�W�����iWii�̍����X�g�ɑ��݁j
    LCFG_TWL_COUNTRY_INDONESIA   = 152,     // �C���h�l�V�A
    
    // USA���[�W����
    LCFG_TWL_COUNTRY_SINGAPORE   = 153,     // �V���K�|�[��
    
    // ASI���[�W�����i�Ăсj
    LCFG_TWL_COUNTRY_THAILAND    = 154,     // �^�C
    LCFG_TWL_COUNTRY_PHILIPPINES,           // �t�B���s��
    LCFG_TWL_COUNTRY_MALAYSIA,              // �}���[�V�A
    
    // ����`���[�W�����iIQue���[�W�����H�j
    LCFG_TWL_COUNTRY_CHINA       = 160,     // ����
    
    // USA���[�W����
    LCFG_TWL_COUNTRY_UAE         = 168,     // �A���u�񒷍��A�M
    
    // ����`���[�W����
    LCFG_TWL_COUNTRY_INDIA       = 169,     // �C���h
    LCFG_TWL_COUNTRY_EGYPT       = 170,     // �G�W�v�g
    LCFG_TWL_COUNTRY_OMAN,                  // �I�}�[��
    LCFG_TWL_COUNTRY_QATAR,                 // �J�^�[��
    LCFG_TWL_COUNTRY_KUWAIT,                // �N�E�F�[�g
    LCFG_TWL_COUNTRY_SAUDI_ARABIA,          // �T�E�W�A���r�A
    LCFG_TWL_COUNTRY_SYRIA,                 // �V���A
    LCFG_TWL_COUNTRY_BAHRAIN,               // �o�[���[��
    LCFG_TWL_COUNTRY_JORDAN,                // �����_��

    LCFG_TWL_COUNTRY_OTHERS      = 254,
    LCFG_TWL_COUNTRY_UNKNOWN     = 255,
    
    LCFG_TWL_COUNTRY_MAX
    
} LCFGTWLCountryCode;

#endif
