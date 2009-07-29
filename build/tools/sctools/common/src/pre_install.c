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
#include "my_sign.h"

#include "pre_install.h"

#define PRE_INSTALL_TABLE_FILE_NAND "rom:/tads/tad_table.txt"
#define PRE_INSTALL_TABLE_DEV_FILE_NAND "rom:/taddevs/taddev_table.txt"
#define PRE_INSTALL_TABLE_FILE_SD "sdmc:/sdtads/en_sdtad_table.txt"
#define PRE_INSTALL_TABLE_DEV_FILE_SD "sdmc:/sdtaddevs/en_sdtad_table.txt"

static PRE_INSTALL_FILE *pre_install_file_list = NULL;



#if 1
//char *pre_install_search_tid(u64 tid, FSFile *log_fd);
static BOOL pre_install_discard_list(void);
static void pre_install_print_list(FSFile *log_fd);
#endif


BOOL pre_install_get_version(u64 tid, u16 *version)
{
  NAMTitleInfo titleInfoTmp;

  if ( NAM_ReadTitleInfo(&titleInfoTmp, tid) == NAM_OK ) {
    // NAND�Ɋ��ɃC���X�g�[������Ă��邩�ǂ����m�F����
    if (tid == titleInfoTmp.titleId) {
      if( version != NULL ) {
	*version = titleInfoTmp.version;
#if 0
	OS_TPrintf( "tid=0x%08x%08x version = %d\n",
		    (u32)(titleInfoTmp.titleId >> 32), (u32)(titleInfoTmp.titleId & 0xffffffff), titleInfoTmp.version);
#endif
	return TRUE;
      }
    }
  }
  return FALSE;
}


BOOL pre_install_check_download_or_pre_install(u64 tid, int *flag, u8 *es_ver, u16 *ticket_ver, FSFile *log_fd)
{

#if 0
typedef u64  ESTitleId;            /* 64-bit title identity */
typedef u64  ESTicketId;           /* 64-bit ticket id */
typedef u8   ESVersion;            /* 8-bit data structure version */
typedef u16  ESTitleVersion;       /* 16-bit title version */
typedef ESTitleId ESSysVersion;    /* 64-bit system software version */

typedef struct {
    ESVersion        version;            /* eTicket data structure version */
    ESTicketId       ticketId;           /* eTicket ID */
    ESId             deviceId;           /* device ID */
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
#endif

  ESError rv;
  ESTicketView *ticketViews;  
  u32 numTickets;
  u32 i;

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
    if( ticketViews == NULL ) {
      miya_log_fprintf(log_fd,"%s OS_Alloc failed: tid=0x%08x%08x\n",
		       __FUNCTION__, (u32)(tid >> 32) , (u32)(tid & 0xffffffff)); 
      return FALSE;
    }
    rv = ES_GetTicketViews(tid, ticketViews, &numTickets);
    if (rv != ES_ERR_OK) {
      miya_log_fprintf(log_fd,"ES_GetTicketViews failed: %d tid=0x%08x%08x\n", 
		       rv, (u32)(tid >> 32) , (u32)(tid & 0xffffffff) );
      OS_Free(ticketViews );
      return FALSE;
    }
  }
  else {
    miya_log_fprintf(log_fd,"ES_GetTicketViews numTickets = 0: %d tid=0x%08x%08x\n", 
		     rv, (u32)(tid >> 32) , (u32)(tid & 0xffffffff) );

#if 0
    return FALSE;
#endif
    /* error ??? */
    return TRUE;
  }


#if 0
  /*  c:/twlsdk/add-ins/es/es-sdk-20090216/twl/build/demos/sysmenu/src/list.c */
  /* Just use first ticket view for now */
  OS_Printf("Attempting to launch %08x\n", (u32)(t->titleId & 0xffffffff));
  rv = ES_CheckLaunchRights(t->titleId, &t->ticketViews[0]);
#endif

  for( i = 0 ; i < numTickets ; i++ ) {
    if( ticketViews[i].deviceId == 0 ) {
      /* common */
      *flag = 1;
      /* �P�ł�common�������common�����I�H */
      *es_ver = ticketViews[i].version;
      *ticket_ver = ticketViews[i].ticketVersion;
      break;
    }  
    else {
      /* personalized */
      *flag = 2;
      *es_ver = ticketViews[i].version;
      *ticket_ver = ticketViews[i].ticketVersion;
    }
  }


  if( ticketViews ) {
    OS_Free(ticketViews );
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

static char *pre_install_search_tid(u64 tid, FSFile *log_fd, BOOL *is_in_sd)
{
  PRE_INSTALL_FILE *temp_list;
  
  for( temp_list = pre_install_file_list ; temp_list != NULL ; temp_list = temp_list->next ) {
    if( temp_list->tid == tid ) {
      miya_log_fprintf(log_fd,"\ntad file entry tid=0x%08x%08x\n%s\n", 
		       (u32)(tid >> 32) , (u32)(tid & 0xffffffff), temp_list->file_name );

      if( !STD_StrNCmp( temp_list->file_name, "sdmc:" , STD_StrLen("sdmc:")) ) {
	*is_in_sd = TRUE;
      }
      else {
	*is_in_sd = FALSE;
      }
      return temp_list->file_name;
    }
  }
  miya_log_fprintf(log_fd,"\n%s:No entry\ntid 0x%08x%08x\n",__FUNCTION__,
		   (u32)(tid >> 32) , (u32)(tid & 0xffffffff));
  return NULL;
  
}

static BOOL pre_install_discard_list(void)
{
  PRE_INSTALL_FILE *temp_list;
  PRE_INSTALL_FILE *temp_list2;
  
  temp_list = pre_install_file_list;
  while( temp_list != NULL ) {
    temp_list2 = temp_list->next;
    OS_Free( temp_list );
    temp_list = temp_list2;
  }
  pre_install_file_list = NULL;
  return TRUE;
}

static void pre_install_print_list(FSFile *log_fd)
{
  PRE_INSTALL_FILE *temp_list;
  u64 tid;

  for( temp_list = pre_install_file_list ; temp_list != NULL ; temp_list = temp_list->next ) {
    tid = temp_list->tid;
    miya_log_fprintf(log_fd,"tad file entry tid=0x%08x%08x %s\n", 
		     (u32)(tid >> 32) , (u32)(tid & 0xffffffff), temp_list->file_name );
  }
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

static int my_sign_ReadLine(MY_SIGN_File *f, char *buf, int buf_size)
{
  char c;
  s32 readSize;
  int count = 0;
  while( 1 ) {
    readSize = my_sign_FS_ReadFile(f, (void *)&c, (s32)1 );
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

/* main() -> pre_install_process() -> pre_install_load_file()�ŌĂ΂��B */
static BOOL pre_install_load_file(char *path, FSFile *log_fd)
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
	    if( c != ' ' ) {
	      temp_file_name[temp_filename_count] = c;
	      temp_filename_count++;
	    }
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


static BOOL pre_install_load_sd_file(char *path, FSFile *log_fd)
{
  MY_SIGN_File file;
  BOOL bSuccess = TRUE;
  //  s32 result;
  s32 readSize;
  //  FSResult fsres;

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


  OS_TPrintf("%s %d path=%s\n", __FUNCTION__,__LINE__,path);

  my_sign_FS_InitFile(&file);

  bSuccess = my_sign_FS_OpenFile(&file, path);
  if( ! bSuccess ) {
    //    fsres = FS_GetArchiveResultCode(path);
    miya_log_fprintf(log_fd,"Error:%s %s open file failed %s\n",__FILE__,__FUNCTION__,path);
    //    miya_log_fprintf(log_fd, " Failed open file:%s\n", my_fs_util_get_fs_result_word( fsres ));
    return FALSE; /* open error! */
  }

  /*
    title id
    0x00000000 00000000, region , country code, file name,
   */

  while( 1 ) {
  next_line_read:
    buf_state = 0;
    readSize = my_sign_ReadLine(&file, line_buf, LINE_BUF_SIZE );
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
	    if( c != ' ' ) {
	      temp_file_name[temp_filename_count] = c;
	      temp_filename_count++;
	    }
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
  (void)my_sign_FS_CloseFile(&file);

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

#if 1
  num = NAM_GetNumInstalledTitles();
#else
  num = NAM_GetNumTitles();
#endif

  if( num > 0 ) {
#if 1
    if( NAM_OK !=  NAM_GetInstalledTitleList( pArray , NAM_TITLE_ID_S ) ) {
      return FALSE;
    }
#else
    if( NAM_OK !=  NAM_GetTitleList( pArray , NAM_TITLE_ID_S ) ) {
      return FALSE;
    }
#endif

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


	if( NAM_OK != NAM_DeleteTitleCompletely( tid ) ) {
	  m_set_palette(tc[0], M_TEXT_COLOR_RED );	/* green  */
	  mprintf("NG.\n");
	  m_set_palette(tc[0], M_TEXT_COLOR_WHITE );

	  miya_log_fprintf(log_fd, "NG.\n");
	  miya_log_fprintf(log_fd, " Error: NAM_DeleteTitleCompletely id = %08X%08X\n", (u32)(tid >> 32), (u32)tid);
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



BOOL pre_install_command(FSFile *log_fd, u64 *tid_array,  int tid_count, int command, BOOL development_version_flag )
{
  char *tad_file_name;
  int i;
  u64 tid;
  char game_code_buf[5];
  BOOL ret_flag;
  BOOL is_in_sd = FALSE;
  int org_version =-1;

  if( development_version_flag ) {
    (void)pre_install_load_sd_file(PRE_INSTALL_TABLE_DEV_FILE_SD, log_fd);
    (void)pre_install_load_file(PRE_INSTALL_TABLE_DEV_FILE_NAND, log_fd);
  }
  else {
    (void)pre_install_load_sd_file(PRE_INSTALL_TABLE_FILE_SD, log_fd);
    (void)pre_install_load_file(PRE_INSTALL_TABLE_FILE_NAND, log_fd);
  }

  pre_install_print_list(log_fd);

  switch( command ) {
  case 1:
    mprintf("Install App.\n");
    break;
  case 2:
    mprintf("Install TicketOnly.\n");
    break;
  case 3:
    mprintf("Uninstall App.(except ticket)\n");
    break;
  case 4:
    mprintf("Uninstall App. with ticket\n");
    break;
  default:
    break;
  }



  for( i = 0 ; i < tid_count ; i++ ) {
    ret_flag = TRUE;
    tid = tid_array[i];
    (void)my_fs_Tid_To_GameCode(tid, game_code_buf);
    mprintf(" %08X%08X [%s] ", (u32)(tid >> 32), (u32)tid, game_code_buf);
    miya_log_fprintf(log_fd, " %08X%08X [%s] ", (u32)(tid >> 32), (u32)tid, game_code_buf);
    if( tid == 0 ) {
      /* error�� */
      continue;
    }

    switch( command ) {
    case 1:
      mprintf("IA ");
      miya_log_fprintf(log_fd, "IA ");
      tad_file_name = pre_install_search_tid( tid , log_fd, &is_in_sd);
      if( tad_file_name ) {
	if( is_in_sd == TRUE ) {
	  ret_flag = myImportTad_sign( tad_file_name , org_version, log_fd );
	}
	else {
	  ret_flag = myImportTad( tad_file_name , org_version, log_fd );
	}
      }

      break;
    case 2:
      mprintf("TO ");
      miya_log_fprintf(log_fd, "TO ");
      tad_file_name = pre_install_search_tid( tid , log_fd, &is_in_sd);
      if( tad_file_name ) {
	if( is_in_sd == TRUE ) {
	  ret_flag = my_NAM_ImportTadTicketOnly_sign( tad_file_name );
	}
	else {
	  ret_flag = my_NAM_ImportTadTicketOnly( tad_file_name );
	}
      }
      break;
    case 3:
      mprintf("DA ");
      miya_log_fprintf(log_fd, "DA ");
      ret_flag = myDeleteTitle( tid, 0 , NULL );
      break;
    case 4:
      mprintf("DC ");
      miya_log_fprintf(log_fd, "DC ");
      ret_flag = myDeleteTitle( tid, 1 , NULL );
      break;
    default:
      mprintf("?? ");
      miya_log_fprintf(log_fd, "?? ");
      break;
    }


    if( ret_flag == FALSE ) {
      /* error �`�P�b�g�C���X�g�[�����s�H */
      miya_log_fprintf(log_fd, "NG.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_RED );	/* green  */
      mprintf("NG.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
      
    }
    else {
      miya_log_fprintf(log_fd, "OK.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
      mprintf("OK.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    }

  }

  mprintf("Done.\n");

  (void)pre_install_discard_list();
  return ret_flag;

}


BOOL pre_install_debug(FSFile *log_fd, BOOL development_version_flag )
{
  if( development_version_flag ) {
    (void)pre_install_load_sd_file(PRE_INSTALL_TABLE_DEV_FILE_SD, log_fd);
    (void)pre_install_load_file(PRE_INSTALL_TABLE_DEV_FILE_NAND, log_fd);
  }
  else {
    (void)pre_install_load_sd_file(PRE_INSTALL_TABLE_FILE_SD, log_fd);
    (void)pre_install_load_file(PRE_INSTALL_TABLE_FILE_NAND, log_fd);
  }
  
  pre_install_print_list(NULL);
  return TRUE;
}

BOOL pre_install_process( FSFile *log_fd, MY_USER_APP_TID *title_id_buf_ptr, int title_id_count,
			  MY_USER_ETICKET_TID *ticket_id_array,  int ticket_id_count, BOOL development_version_flag )
{
  char *tad_file_name;
  int i;
  u64 tid;
  char game_code_buf[5];
  BOOL ret_flag = TRUE;
  BOOL is_in_sd = FALSE;
  int version;

  if( development_version_flag ) {
    (void)pre_install_load_sd_file(PRE_INSTALL_TABLE_DEV_FILE_SD, log_fd);
    (void)pre_install_load_file(PRE_INSTALL_TABLE_DEV_FILE_NAND, log_fd);
  }
  else {
    (void)pre_install_load_sd_file(PRE_INSTALL_TABLE_FILE_SD, log_fd);
    (void)pre_install_load_file(PRE_INSTALL_TABLE_FILE_NAND, log_fd);
  }

  
  pre_install_print_list(NULL);

  /* �`�P�b�g�����̃C���X�g�[�� */
  for( i = 0 ; i < ticket_id_count ; i++ ) {
    tid = ticket_id_array[i].tid;
    (void)my_fs_Tid_To_GameCode(tid, game_code_buf);
    mprintf(" TO %08X %08X [%s] ", (u32)(tid >> 32), (u32)tid, game_code_buf);
    miya_log_fprintf(log_fd, " TO %08X %08X [%s] ", (u32)(tid >> 32), (u32)tid, game_code_buf);
    if( tid == 0 ) {
      /* error�� */
      continue;
    }

    tad_file_name = pre_install_search_tid( tid , log_fd, &is_in_sd);
    if( tad_file_name ) {
      if( is_in_sd == TRUE ) {
	ret_flag = my_NAM_ImportTadTicketOnly_sign( tad_file_name );
      }
      else {
	ret_flag = my_NAM_ImportTadTicketOnly( tad_file_name );
      }

      if( FALSE == ret_flag ) {
	/* error �`�P�b�g�C���X�g�[�����s�H */
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
      }
    }
    else {
      /* error �q�n�l�Ƀt�@�C�����Ȃ� */
      miya_log_fprintf(log_fd, "NG.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_RED );	/* green  */
      mprintf("NG.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
      mprintf("No file\n");
      miya_log_fprintf(log_fd, "error: no file\n"); 
      ret_flag = FALSE;
    }
  }


  /* �A�v���icommon eticket�j�̃C���X�g�[�� */
  for( i = 0 ; i < title_id_count ; i++ ) {
    /* 
       pTitleIds[i].is_personalized = 1 -> common (pre installed)
       pTitleIds[i].is_personalized = 2 -> personalized
    */
    if( title_id_buf_ptr[i].is_personalized == 1 /* common�̏ꍇ */ ) {
      /*
	0x00030004484E474A "rom:/tads/TWL-KGUJ-v257.tad.out"
	0x000300044B32444A "rom:/tads/TWL-K2DJ-v0.tad.out"
	0x000300044B47554A "rom:/tads/TWL-HNGJ-v256.tad.out"
      */
      tid = title_id_buf_ptr[i].tid;
      version = title_id_buf_ptr[i].version;


      (void)my_fs_Tid_To_GameCode(tid, game_code_buf);
      mprintf(" AP %08X %08X [%s] ", (u32)(tid >> 32), (u32)tid, game_code_buf);
      miya_log_fprintf(log_fd, " AP %08X %08X [%s] ", (u32)(tid >> 32), (u32)tid, game_code_buf);

      tad_file_name = pre_install_search_tid( tid , log_fd, &is_in_sd);
      if( tad_file_name ) {

	if( is_in_sd == TRUE ) {
	  ret_flag = myImportTad_sign( tad_file_name , version, log_fd);
	}
	else {
	  ret_flag = myImportTad( tad_file_name , version, log_fd);
	}
	if( FALSE == ret_flag ) {
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
	  title_id_buf_ptr[i].install_success_flag = TRUE; /* ��������Ƃ��Ȃ��ƌ�ŃZ�[�u�f�[�^�𕜊����Ȃ� */
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
