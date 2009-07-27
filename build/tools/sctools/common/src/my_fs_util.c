#include <twl.h>
#include <nitro/crypto.h>
#include        "text.h"
#include        "mprintf.h"
#include        "my_fs_util.h"
#include        "logprintf.h"
#include        "error_report.h"


#define ATTRIBUTE_BACK 1

#define COPY_FILE_ENCRYPTION 1

#define LIMIT_BUF_ALLOC 1

static BOOL miya_debug_flag = FALSE;
static int miya_debug_counter = 0;

void Miya_debug_ON(void)
{
  miya_debug_flag = TRUE;
}

void Miya_debug_OFF(void)
{
  miya_debug_flag = FALSE;
}


static BOOL print_debug_flag = FALSE;

void my_fs_print_debug_ON(void)
{
  print_debug_flag = TRUE;
}

void my_fs_print_debug_OFF(void)
{
  print_debug_flag = FALSE;
}

BOOL my_fs_get_print_debug_flag(void)
{
  return print_debug_flag;
}




/*
  NAND -> SD�R�s�[�̎��A�A�g���r���[�g�Ǝ��Ԃƃp�[�~�b�V���������킹��K�v����H


// �t�@�C�����̍ő咷 (���d�l�̃v���V�[�W���Ɍ���)
#define	FS_FILE_NAME_MAX	        127

// �G���g�����̍ő咷 (���d�l�̃v���V�[�W���Ɍ���)
#define	FS_ENTRY_SHORTNAME_MAX      16
#define	FS_ENTRY_LONGNAME_MAX	    260

// ReadDirectory�Ŏg�p����G���g������
#define FS_ATTRIBUTE_IS_DIRECTORY   0x00000100UL
#define FS_ATTRIBUTE_IS_PROTECTED   0x00000200UL
// MS-DOS FAT�x�[�X�̃A�[�J�C�u�Ɍ���Ӗ��̂��鑮��
#define FS_ATTRIBUTE_DOS_MASK       0x000000FFUL
#define FS_ATTRIBUTE_DOS_READONLY   0x00000001UL
#define FS_ATTRIBUTE_DOS_HIDDEN     0x00000002UL
#define FS_ATTRIBUTE_DOS_SYSTEM     0x00000004UL
#define FS_ATTRIBUTE_DOS_VOLUME     0x00000008UL
#define FS_ATTRIBUTE_DOS_DIRECTORY  0x00000010UL
#define FS_ATTRIBUTE_DOS_ARCHIVE    0x00000020UL


// OpenFile�Ŏg�p����A�N�Z�X���[�h
#define FS_FILEMODE_R               0x00000001UL
#define FS_FILEMODE_W               0x00000002UL
#define FS_FILEMODE_L               0x00000004UL
#define FS_FILEMODE_RW              (FS_FILEMODE_R | FS_FILEMODE_W)
#define FS_FILEMODE_RWL             (FS_FILEMODE_R | FS_FILEMODE_W | FS_FILEMODE_L)

// OpenDirectory�Ŏg�p����A�N�Z�X���[�h
#define FS_DIRMODE_SHORTNAME_ONLY   0x00001000UL

// CreateFile�Ŏg�p���錠���t���O
#define FS_PERMIT_R                 0x00000001UL
#define FS_PERMIT_W                 0x00000002UL
#define FS_PERMIT_RW                (FS_PERMIT_R | FS_PERMIT_W)

// ReadDirectory�Ŏg�p����G���g�����
typedef struct FSDirectoryEntryInfo
{
    char        shortname[FS_ENTRY_SHORTNAME_MAX];
    u32         shortname_length;
    char        longname[FS_ENTRY_LONGNAME_MAX];
    u32         longname_length;
    u32         attributes;
    FSDateTime  atime;
    FSDateTime  mtime;
    FSDateTime  ctime;
    u32         filesize;
    u32         id;
} FSDirectoryEntryInfo;

typedef struct FSDirectoryEntryInfoW
{
    u32         attributes;
    FSDateTime  atime;
    FSDateTime  mtime;
    FSDateTime  ctime;
    u32         filesize;
    u32         id;
    u32         shortname_length;
    u32         longname_length;
    char        shortname[FS_ENTRY_SHORTNAME_MAX];
    u16         longname[FS_ENTRY_LONGNAME_MAX];
} FSDirectoryEntryInfoW;

// GetPathInfo,SetPathInfo�Ŏg�p����G���g�����
typedef struct FSPathInfo
{
    u32         attributes;
    FSDateTime  ctime;
    FSDateTime  mtime;
    FSDateTime  atime;
    u32         filesize;
    u32         id;
} FSPathInfo;
*/


typedef struct {
  FSResult result;
  char string[32];
} FS_RESUTL_WORD;

/* c:/twlsdk/include/nitro/fs/types.h */



char *my_fs_util_get_fs_result_word( FSResult res )
{
  int i;
  static FS_RESUTL_WORD my_fs_result_word[] = {
    { FS_RESULT_SUCCESS, "FS_RESULT_SUCCESS" },
    { FS_RESULT_FAILURE, "FS_RESULT_FAILURE" },
    { FS_RESULT_BUSY, "FS_RESULT_BUSY" },
    { FS_RESULT_CANCELED, "FS_RESULT_CANCELED" },
    { FS_RESULT_CANCELLED, "FS_RESULT_CANCELLED" },
    { FS_RESULT_UNSUPPORTED, "FS_RESULT_UNSUPPORTED" },
    { FS_RESULT_ERROR, "FS_RESULT_ERROR" },
    { FS_RESULT_INVALID_PARAMETER, "FS_RESULT_INVALID_PARAMETER" },
    { FS_RESULT_NO_MORE_RESOURCE, "FS_RESULT_NO_MORE_RESOURCE" },
    { FS_RESULT_ALREADY_DONE, "FS_RESULT_ALREADY_DONE" },
    { FS_RESULT_PERMISSION_DENIED, "FS_RESULT_PERMISSION_DENIED" },
    { FS_RESULT_MEDIA_FATAL, "FS_RESULT_MEDIA_FATAL" },
    { FS_RESULT_NO_ENTRY, "FS_RESULT_NO_ENTRY" },
    { FS_RESULT_MEDIA_NOTHING, "FS_RESULT_MEDIA_NOTHING" },
    { FS_RESULT_MEDIA_UNKNOWN, "FS_RESULT_MEDIA_UNKNOWN" },
    { FS_RESULT_BAD_FORMAT, "FS_RESULT_BAD_FORMAT" },
    { FS_RESULT_MAX, "FS_RESULT_MAX" },
    // �v���V�[�W�����Ŏg�p����ꎞ�I�Ȍ��ʒl
    { FS_RESULT_PROC_ASYNC, "FS_RESULT_PROC_ASYNC" },
    { FS_RESULT_PROC_DEFAULT, "FS_RESULT_PROC_DEFAULT" },
    { FS_RESULT_PROC_UNKNOWN, "FS_RESULT_PROC_UNKNOW" },
  };

  for( i = 0 ; i < 19 ; i++ ) {
    if( my_fs_result_word[i].result  == res ) {
      return my_fs_result_word[i].string;
    }
  }
  return my_fs_result_word[0].string;
}



BOOL my_fs_Tid_To_GameCode(u64 tid, char *gcode)
{
  u32 code;
  char *str;
  //  OS_TPrintf("tid = %016X\n",tid);
  str = gcode;
  code = (u32)(tid & 0xffffffff);
  *str++ = (char)((code >> 24) & 0xff);
  *str++ = (char)((code >> 16) & 0xff);
  *str++ = (char)((code >> 8) & 0xff);
  *str++ = (char)(code  & 0xff);
  *str  = '\0';
  return TRUE;
}


static void PrintAttributes(u32 attributes, FSFile *log_fd)
{
  char buf[7];
  buf[0] = (char)(( attributes & FS_ATTRIBUTE_DOS_DIRECTORY )? 'd' : '-');
  buf[1] = (char)(( attributes & FS_ATTRIBUTE_DOS_VOLUME )?    'v' : '-');
  buf[2] = (char)(( attributes & FS_ATTRIBUTE_DOS_SYSTEM )?    's' : '-');
  buf[3] = (char)(( attributes & FS_ATTRIBUTE_DOS_ARCHIVE )?   'a' : '-');
  buf[4] = (char)(( attributes & FS_ATTRIBUTE_DOS_HIDDEN )?    'h' : '-');
  buf[5] = (char)(( attributes & FS_ATTRIBUTE_DOS_READONLY )?  'R' : '-');
  buf[6] = '\0';
  miya_log_fprintf(log_fd, "%s", buf );

}

BOOL Log_File_Open(FSFile *log_fd, const char *log_file_name)
{
  BOOL bSuccess;
  FSResult res;

  if( log_fd == NULL ) {
    return FALSE;
  }

  if( log_file_name == NULL ) {
    miya_log_fprintf(NULL, "%s %d: No log file name\n",__FUNCTION__,__LINE__);
    return FALSE;
  }

  FS_InitFile(log_fd);

  bSuccess = FS_OpenFileEx(log_fd, log_file_name, (FS_FILEMODE_W));
  if( ! bSuccess ) {
    FS_CreateFileAuto( log_file_name, FS_PERMIT_W);
    bSuccess = FS_OpenFileEx(log_fd, log_file_name, (FS_FILEMODE_W));
    if( ! bSuccess ) {
      res = FS_GetArchiveResultCode( log_file_name );
      miya_log_fprintf(NULL, "log file open error %s\n", log_file_name );
      miya_log_fprintf(NULL, " Failed open file:%s\n", my_fs_util_get_fs_result_word( res ));
      return FALSE;
    }
  }
  return TRUE;
}

void Log_File_Close(FSFile *log_fd)
{
  if( log_fd ) {
    FS_FlushFile(log_fd);
    FS_CloseFile(log_fd);
  }
}



/*
  void CRYPTO_RC4Encrypt(CRYPTORC4Context* context, const void* in, u32 length, void* out);

  context - CRYPTO_RC4Init() �ŗ\�ߌ���ݒ肵�� CRYPTORC4Context �^�̍\���̂��w�肵�܂��B 
  in      - RC4 �A���S���Y���ɂ��Í���/�������s���Ώۂ̃f�[�^�ւ̃|�C���^���w�肵�܂��B 
  length  - in �Ŏw�肵���f�[�^�̒������w�肵�܂��B 
  out     - �Í���/�������s�������ʂ��i�[�����̃|�C���^���w�肵�܂��B 
  
*/
static u32 my_fs_GetStringLength(char* str)
{
  u32 i;
  for (i = 0; ; i++) {
    if (*(str++) == '\0') {
      return i;
    }
  }
}

static char *my_fs_key = "twl-repair-tool";


s32 my_fs_crypto_read(FSFile *f, void *ptr, s32 size)
{
  s32 readSize;
  CRYPTORC4FastContext context;
  u8 *crypt_buf;

  crypt_buf = (u8 *)OS_Alloc( (u32)size );
  if(crypt_buf == NULL ) {
    return -1;
  }

  readSize = FS_ReadFile(f, (void *)crypt_buf, size);
  if( readSize != size ) {
    /* error! */
    goto end;
  }
  CRYPTO_RC4FastInit(&context, my_fs_key, my_fs_GetStringLength(my_fs_key));
  CRYPTO_RC4FastEncrypt(&context, (char *)crypt_buf, (u32)size, ptr);

 end:  
  if( crypt_buf != NULL ) {
    OS_Free(crypt_buf);
  }
  return readSize;
}

s32 my_fs_crypto_write(FSFile *f, void *ptr, s32 size)
{
  s32 writtenSize;
  CRYPTORC4FastContext context;
  u8 *crypt_buf;

  crypt_buf = (u8 *)OS_Alloc( (u32)size );
  if(crypt_buf == NULL ) {
    return -1;
  }

  CRYPTO_RC4FastInit(&context, my_fs_key, my_fs_GetStringLength(my_fs_key));
  CRYPTO_RC4FastEncrypt(&context, (char *)ptr, (u32)size, crypt_buf);

  writtenSize = FS_WriteFile(f, crypt_buf, size);
  if( writtenSize != size ) {
    /* error */
  }

  if( crypt_buf != NULL ) {
    OS_Free(crypt_buf);
  }
  return writtenSize; 
}


//#define BUF_SIZE (16*1024)
//#define BUF_SIZE (512*1024)
#define BUF_SIZE (1*1024*1024)
#ifdef LIMIT_BUF_ALLOC
static char *pBuffer = NULL;
static char *pBuffer_crypto = NULL;
#else
static char pBuffer[BUF_SIZE];
static char pBuffer_crypto[BUF_SIZE];
#endif	/* LIMIT_BUF_ALLOC */



BOOL CopyFile(const char *dst_path, const char *src_path, FSFile *log_fd )
{
  BOOL ret_flag = TRUE;

  FSFile f_src;
  FSFile f_dst;
  u32 restSize;
  u32 tempSize;
  u32 fileSize;
  s32 readSize = 0;
  FSResult fsResult;
  s32 writtenSize;

  FS_InitFile(&f_src);

  if( FALSE == FS_OpenFileEx(&f_src, src_path, FS_FILEMODE_R) ) {
    miya_log_fprintf(log_fd, "%s Failed Open File\n",__FUNCTION__);
    miya_log_fprintf(log_fd, " path=%s\n", src_path );
    miya_log_fprintf(log_fd, " res=%s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(src_path) ));
    return FALSE;
  }

  fileSize = FS_GetFileLength(&f_src);

  FS_InitFile(&f_dst);
  if( FALSE == FS_OpenFileEx(&f_dst, dst_path, FS_FILEMODE_W) ) {
    FSResult res = FS_GetArchiveResultCode(dst_path);
    if( res == FS_RESULT_NO_ENTRY ) {
      /* �{�������Ŗ��Ȃ� */
    }
    else {
      miya_log_fprintf(log_fd, "%s Failed open file %d:\n", __FUNCTION__,__LINE__);
      miya_log_fprintf(log_fd, " %s\n", dst_path );
      miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word(res) );
    }
  }
  else {
    FS_CloseFile(&f_dst);
    /* backup �o�b�N�A�b�v������Ă����ׂ��H�H */
    FS_DeleteFile(dst_path);
  }
  
  FS_CreateFile(dst_path, (FS_PERMIT_R|FS_PERMIT_W));
  if( FALSE == FS_OpenFileEx(&f_dst, dst_path, FS_FILEMODE_W) ) {
    miya_log_fprintf(log_fd, "%s Failed open file %d:\n", __FUNCTION__,__LINE__);
    miya_log_fprintf(log_fd, " %s\n", dst_path );
    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word(FS_GetArchiveResultCode(dst_path)) );
    ret_flag = FALSE;
    goto exit_label;

  }
  else {
    fsResult = FS_SetFileLength(&f_dst, 0);
    if( fsResult != FS_RESULT_SUCCESS ) {
      miya_log_fprintf(log_fd, "%s Error: Set file len\n", __FUNCTION__);
      miya_log_fprintf(log_fd, " %s\n", dst_path );
      miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word(fsResult) );
      ret_flag = FALSE;
      goto exit_label;
    }
  }

#ifdef LIMIT_BUF_ALLOC
  if( pBuffer == NULL ) {
    pBuffer = (char*)OS_Alloc( BUF_SIZE );
    if( pBuffer == NULL ) {
      miya_log_fprintf(log_fd, "%s Mem alloc error: line %d size %d\n", __FUNCTION__, __LINE__,BUF_SIZE);
      ret_flag = FALSE;
      goto exit_label;
    }
    else {
      //      OS_TPrintf("pBuf addr = 0x%p\n",pBuffer);
    }
  }
  if( pBuffer_crypto == NULL ) {
    pBuffer_crypto = (char*)OS_Alloc( BUF_SIZE );
    if( pBuffer_crypto == NULL ) {
      miya_log_fprintf(log_fd, "%s Mem alloc error: line %d size %d\n", __FUNCTION__, __LINE__,BUF_SIZE);
      ret_flag = FALSE;
      goto exit_label;
    }
    else {
      //      OS_TPrintf("pBufCrypt addr = 0x%p\n",pBuffer_crypto);
    }
  }
#endif

  restSize = fileSize;

  while( 1 ) {
    if( restSize == 0 ) {
      FS_FlushFile(&f_dst);
      ret_flag = TRUE;
      break;
    }
    else if( restSize > BUF_SIZE ) {
      tempSize = BUF_SIZE;
    }
    else {
      tempSize = restSize;
    }
    restSize -= tempSize;

    readSize = FS_ReadFile(&f_src, pBuffer, (s32)tempSize);
    if( readSize != tempSize ) {
      miya_log_fprintf(log_fd, "%s Failed Read File:%s\n",__FUNCTION__ , src_path );
      miya_log_fprintf(log_fd, " request size=%d read size=%d rest size %d\n", tempSize, readSize,restSize);
      ret_flag = FALSE;
      break;
    }
    else {
      /* ��������Write */
      writtenSize = FS_WriteFile(&f_dst, pBuffer, readSize);
      if( writtenSize != readSize ) {
	miya_log_fprintf(log_fd, "%s Failed write file %d:\n", __FUNCTION__,__LINE__);
	miya_log_fprintf(log_fd, " %s\n", dst_path );
	miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word(FS_GetArchiveResultCode(dst_path)) );
	ret_flag = FALSE;
	break;
      }
    }
  }
 exit_label:

  if( FALSE == FS_CloseFile(&f_src) ) {
    miya_log_fprintf(log_fd, "%s Failed Close File\n", __FUNCTION__ );
    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(src_path)));
  }
  if( FALSE == FS_CloseFile(&f_dst) ) {
    miya_log_fprintf(log_fd, "%s Failed Close File\n", __FUNCTION__ );
    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(dst_path)));
  }

  return ret_flag;
}




static BOOL CopyFileCrypto(const char *dst_path, const char *src_path, FSFile *log_fd )
{
  FSFile f_src;
  FSFile f_dst;

  u32 restSize;
  u32 tempSize;

  u32 fileSize;
  s32 readSize = 0;
  FSResult fsResult;
  s32 writtenSize;
  CRYPTORC4FastContext context;

  BOOL ret_flag = FALSE;

  if( miya_debug_flag ) {
    miya_debug_counter++;
    if( miya_debug_counter > 2 ) {
      miya_debug_counter = 0;
      return FALSE;
    }
  }

  FS_InitFile(&f_src);

  if( FALSE == FS_OpenFileEx(&f_src, src_path, FS_FILEMODE_R) ) {
    miya_log_fprintf(log_fd, "%s Failed Open File\n",__FUNCTION__);
    miya_log_fprintf(log_fd, " path=%s\n", src_path );
    miya_log_fprintf(log_fd, " res=%s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(src_path) ));
    return FALSE;
  }

  fileSize = FS_GetFileLength(&f_src);



  FS_InitFile(&f_dst);
  if( FALSE == FS_OpenFileEx(&f_dst, dst_path, FS_FILEMODE_W) ) {
    FSResult res = FS_GetArchiveResultCode(dst_path);
    if( res == FS_RESULT_NO_ENTRY ) {
      /* �{�������Ŗ��Ȃ� */
    }
    else {
      miya_log_fprintf(log_fd, "%s Failed open file %d:\n", __FUNCTION__,__LINE__);
      miya_log_fprintf(log_fd, " %s\n", dst_path );
      miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word(res) );
    }
  }
  else {
    FS_CloseFile(&f_dst);
    /* backup �o�b�N�A�b�v������Ă����ׂ��H�H */
    FS_DeleteFile(dst_path);
  }
  
  FS_CreateFile(dst_path, (FS_PERMIT_R|FS_PERMIT_W));
  if( FALSE == FS_OpenFileEx(&f_dst, dst_path, FS_FILEMODE_W) ) {
    miya_log_fprintf(log_fd, "%s Failed open file %d:\n", __FUNCTION__,__LINE__);
    miya_log_fprintf(log_fd, " %s\n", dst_path );
    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word(FS_GetArchiveResultCode(dst_path)) );
    ret_flag = FALSE;
    goto exit_label;

  }
  else {
    fsResult = FS_SetFileLength(&f_dst, 0);
    if( fsResult != FS_RESULT_SUCCESS ) {
      miya_log_fprintf(log_fd, "%s Error: Set file len\n", __FUNCTION__);
      miya_log_fprintf(log_fd, " %s\n", dst_path );
      miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word(fsResult) );
      ret_flag = FALSE;
      goto exit_label;
    }
  }

#ifdef LIMIT_BUF_ALLOC
  if( pBuffer == NULL ) {
    pBuffer = (char*)OS_Alloc( BUF_SIZE );
    if( pBuffer == NULL ) {
      miya_log_fprintf(log_fd, "%s Mem alloc error: line %d size %d\n", __FUNCTION__, __LINE__,BUF_SIZE);
      ret_flag = FALSE;
      goto exit_label;
    }
    else {
      // OS_TPrintf("pBuf addr = 0x%p\n",pBuffer);
    }

  }
  if( pBuffer_crypto == NULL ) {
    pBuffer_crypto = (char*)OS_Alloc( BUF_SIZE );
    if( pBuffer_crypto == NULL ) {
      miya_log_fprintf(log_fd, "%s Mem alloc error: line %d size %d\n", __FUNCTION__, __LINE__,BUF_SIZE);
      ret_flag = FALSE;
      goto exit_label;
    }
    else {
      //      OS_TPrintf("pBufCrypt addr = 0x%p\n",pBuffer_crypto);
    }
  }
#endif

  CRYPTO_RC4FastInit(&context, my_fs_key, my_fs_GetStringLength(my_fs_key));

  restSize = fileSize;

  while( 1 ) {
    if( restSize == 0 ) {
      FS_FlushFile(&f_dst);
      ret_flag = TRUE;
      break;
    }
    else if( restSize > BUF_SIZE ) {
      tempSize = BUF_SIZE;
    }
    else {
      tempSize = restSize;
    }
    restSize -= tempSize;

    readSize = FS_ReadFile(&f_src, pBuffer, (s32)tempSize);
    if( readSize != tempSize ) {
      miya_log_fprintf(log_fd, "%s Failed Read File:%s\n",__FUNCTION__ , src_path );
      miya_log_fprintf(log_fd, " request size=%d read size=%d rest size %d\n", tempSize, readSize,restSize);
      ret_flag = FALSE;
      break;
    }
    else {
      /* ��������Write */
      CRYPTO_RC4FastEncrypt(&context, pBuffer, (u32)readSize, pBuffer_crypto);

      writtenSize = FS_WriteFile(&f_dst, pBuffer_crypto, readSize);
      if( writtenSize != readSize ) {
	miya_log_fprintf(log_fd, "%s Failed write file %d:\n", __FUNCTION__,__LINE__);
	miya_log_fprintf(log_fd, " %s\n", dst_path );
	miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word(FS_GetArchiveResultCode(dst_path)) );
	ret_flag = FALSE;
	break;
      }
    }
  }

 exit_label:
  if( FALSE == FS_CloseFile(&f_src) ) {
    miya_log_fprintf(log_fd, "%s Failed Close File\n", __FUNCTION__ );
    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(src_path)));
  }
  if( FALSE == FS_CloseFile(&f_dst) ) {
    miya_log_fprintf(log_fd, "%s Failed Close File\n", __FUNCTION__ );
    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(dst_path)));
  }

  return ret_flag;
}

static BOOL CompareFsDateTime( FSDateTime *dt1, FSDateTime *dt2)
{
  if( dt1->year != dt2->year ) {
    return FALSE;
  }
  if( dt1->month != dt2->month ) {
    return FALSE;
  }
  if( dt1->day != dt2->day ) {
    return FALSE;
  }
  if( dt1->hour != dt2->hour ) {
    return FALSE;
  }
  if( dt1->minute != dt2->minute ) {
    return FALSE;
  }
  if( dt1->second != dt2->second ) {
    return FALSE;
  }
  return TRUE;
}

static BOOL restore_entry_list(MY_DIR_ENTRY_LIST **headp, FSFile *log_fd)
{
  MY_DIR_ENTRY_LIST *list_temp;
  MY_DIR_ENTRY_LIST *list_temp2;
  FSPathInfo path_info;
  int count = 0;
  /* �g�b�v�f�B���N�g���̕����珇�Ƀ��X�g����Ă���̂� */
  /* ���X�g�̌��i���ʃf�B���N�g���̕��j����߂��Ȃ��Ƃ����Ȃ� */
  /* �o�b�N���[�h�Ŗ߂��Ă��� */

  if( *headp == NULL ) {
  }
  else {
    for( list_temp = *headp ; list_temp->next != NULL ; list_temp = list_temp->next ) {
      ;
    }
    while( list_temp != NULL ) {
      path_info.attributes  = list_temp->content.attributes;
      path_info.ctime  = list_temp->content.ctime;
      path_info.mtime = list_temp->content.mtime;
      path_info.atime = list_temp->content.atime;
      path_info.id = list_temp->content.id;
      path_info.filesize = list_temp->content.filesize;
      if( FALSE == FS_SetPathInfo( list_temp->src_path, &path_info) ) {
	FSResult fsResult = FS_GetArchiveResultCode(list_temp->src_path);
	miya_log_fprintf(log_fd, "%s %d: Failed SetPathInfo\n", __FUNCTION__,__LINE__ );
	miya_log_fprintf(log_fd, " %s\n", list_temp->src_path );
	miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( fsResult ) );
      }
      count++;
      list_temp2 = list_temp;
      list_temp = list_temp->prev;
      OS_Free( list_temp2 );
    }
  }
  return TRUE;
}


static BOOL add_entry_list( MY_DIR_ENTRY_LIST **headp, MY_DIR_ENTRY_LIST *entry_list, FSFile *log_fd)
{
  MY_DIR_ENTRY_LIST *list_temp;
  MY_DIR_ENTRY_LIST *list_prev_temp;

  if( entry_list == NULL ) {
    miya_log_fprintf(log_fd, "%s ERROR:entry_list ptr -> NULL\n", __FUNCTION__);
    return FALSE;
  }
  if( headp == NULL ) {
    miya_log_fprintf(log_fd, "%s ERROR:headp ptr -> NULL\n", __FUNCTION__);
    return FALSE;
  }

  if( *headp == NULL ) {
    *headp = (MY_DIR_ENTRY_LIST *)OS_Alloc( sizeof(MY_DIR_ENTRY_LIST) );
    if( *headp == NULL ) {
      miya_log_fprintf(log_fd, "%s memory alloc error: *headp size=%d\n",  
		       __FUNCTION__, sizeof(MY_DIR_ENTRY_LIST) );
      return FALSE;
    }
    STD_CopyMemory( (void *)(*headp), (void *)entry_list , sizeof(MY_DIR_ENTRY_LIST) );
    (*headp)->prev = NULL;
    (*headp)->next = NULL;
  }
  else {
    for( list_temp = *headp ; list_temp->next != NULL ; list_temp = list_temp->next ) {
      ;
    }
    list_temp->next = (MY_DIR_ENTRY_LIST *)OS_Alloc( sizeof(MY_DIR_ENTRY_LIST) );
    if( list_temp->next == NULL ) {
      miya_log_fprintf(log_fd, "%s memory alloc error: list_temp->next size=%d\n",  
		       __FUNCTION__, sizeof(MY_DIR_ENTRY_LIST) );
      return FALSE;
    }

    list_prev_temp = list_temp;
    list_temp = list_temp->next;
    STD_CopyMemory( (void *)list_temp, (void *)entry_list , sizeof(MY_DIR_ENTRY_LIST) );
    list_temp->prev = list_prev_temp;
    list_temp->next = NULL;
  }
  return TRUE;
}



static BOOL my_fs_add_list( MY_DIR_ENTRY_LIST **headp, FSDirectoryEntryInfo *entry, 
			    const char *src_path, const char *dst_path, FSFile *log_fd)
{

  MY_DIR_ENTRY_LIST *list_temp;
  MY_DIR_ENTRY_LIST *list_prev_temp;
  FSPathInfo path_info;


  if( (entry == NULL) || (src_path == NULL) ) {
    miya_log_fprintf(log_fd, "%s %d: invalid arg.\n", __FUNCTION__,__LINE__ );
    miya_log_fprintf(log_fd, " entry=0x%08p src_path=0x%-8p\n",entry, src_path);
    return FALSE;
  }

  /* �t�@�C���̏ꍇ src��NAND, dst��SD */
  if( !STD_StrCmp( src_path, "nand:" ) ) {
    /* nand�̃��[�g�f�B���N�g���̓X���[����B */
    miya_log_fprintf(log_fd, "detect nand: root dir\n");
    return TRUE;
  }

  if( FALSE == FS_GetPathInfo(src_path,&path_info) ) {
    FSResult fsResult = FS_GetArchiveResultCode( src_path );
    miya_log_fprintf(log_fd, "%s %d: Failed SetPathInfo\n", __FUNCTION__,__LINE__ );
    miya_log_fprintf(log_fd, " %s\n", src_path );
    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( fsResult ) );
  }
  else {
    if( FALSE == CompareFsDateTime( &(entry->atime), &(path_info.atime)) )  {
      miya_log_fprintf(log_fd, "warning atime\n");
      miya_log_fprintf(log_fd," entry     %d/%02d/%02d ", entry->atime.year,entry->atime.month,entry->atime.day); 
      miya_log_fprintf(log_fd,"%02d:%02d:%02d\n", entry->atime.hour,entry->atime.minute,entry->atime.second);
      miya_log_fprintf(log_fd," path_info %d/%02d/%02d ", path_info.atime.year,path_info.atime.month,path_info.atime.day);
      miya_log_fprintf(log_fd,"%02d:%02d:%02d\n", path_info.atime.hour,path_info.atime.minute,path_info.atime.second);
      entry->atime = path_info.atime;
    }

    if( FALSE == CompareFsDateTime( &(entry->mtime), &(path_info.mtime)) )  {
      miya_log_fprintf(log_fd, "warning mtime\n");
      miya_log_fprintf(log_fd," entry     %d/%02d/%02d ", entry->mtime.year,entry->mtime.month,entry->mtime.day); 
      miya_log_fprintf(log_fd,"%02d:%02d:%02d\n", entry->mtime.hour,entry->mtime.minute,entry->mtime.second);
      miya_log_fprintf(log_fd," path_info %d/%02d/%02d ", path_info.mtime.year,path_info.mtime.month,path_info.mtime.day);
      miya_log_fprintf(log_fd,"%02d:%02d:%02d\n", path_info.mtime.hour,path_info.mtime.minute,path_info.mtime.second);
      entry->mtime = path_info.mtime;
    }

#if 0
    /* �Ȃ���Warning���o�� */
    if( FALSE == CompareFsDateTime( &(entry->ctime), &(path_info.ctime)) )  {
      miya_log_fprintf(log_fd, "warning ctime\n");
      miya_log_fprintf(log_fd," entry     %d/%02d/%02d ", entry->ctime.year,entry->ctime.month,entry->ctime.day); 
      miya_log_fprintf(log_fd,"%02d:%02d:%02d\n", entry->ctime.hour,entry->ctime.minute,entry->ctime.second);
      miya_log_fprintf(log_fd," path_info %d/%02d/%02d ", path_info.ctime.year,path_info.ctime.month,path_info.ctime.day);
      miya_log_fprintf(log_fd,"%02d:%02d:%02d\n", path_info.ctime.hour,path_info.ctime.minute,path_info.ctime.second);
    }
#endif
    //    OS_TPrintf("path info att=0x%08x\n",path_info.attributes);
    if( entry->attributes != path_info.attributes ) {
      miya_log_fprintf(log_fd, "Warning: path  att = 0x%08x\n",path_info.attributes);
      miya_log_fprintf(log_fd, "         entry att = 0x%08x\n",entry->attributes);
    }
    entry->attributes = path_info.attributes;
  }

  if( *headp == NULL ) {
    *headp = (MY_DIR_ENTRY_LIST *)OS_Alloc( sizeof(MY_DIR_ENTRY_LIST) );
    if( *headp == NULL ) {
      miya_log_fprintf(log_fd, "%s memory alloc error: *headp size=%d\n",  
		       __FUNCTION__, sizeof(MY_DIR_ENTRY_LIST) );
      return FALSE;
    }
    STD_MemSet((void *)*headp, 0, sizeof(MY_DIR_ENTRY_LIST) );
    (*headp)->prev = NULL;
    (*headp)->next = NULL;
    STD_CopyMemory( (void *)&((*headp)->content), (void *)entry ,sizeof(FSDirectoryEntryInfo) );
    STD_StrCpy((*headp)->src_path, src_path);
    if( dst_path ) {
      STD_StrCpy((*headp)->dst_path, dst_path);
    }
  }
  else {
    for( list_temp = *headp ; list_temp->next != NULL ; list_temp = list_temp->next ) {
      ;
    }
    list_temp->next = (MY_DIR_ENTRY_LIST *)OS_Alloc( sizeof(MY_DIR_ENTRY_LIST) );
    if( list_temp->next == NULL ) {
      miya_log_fprintf(log_fd, "%s memory alloc error:list_temp->next size=%d\n",  
		       __FUNCTION__, sizeof(MY_DIR_ENTRY_LIST) );
      return FALSE;
    }
    STD_MemSet((void *)list_temp->next, 0, sizeof(MY_DIR_ENTRY_LIST) );
    list_prev_temp = list_temp;
    list_temp = list_temp->next;
    list_temp->prev = list_prev_temp;
    list_temp->next = NULL;
    STD_CopyMemory( (void *)&(list_temp->content), (void *)entry ,sizeof(FSDirectoryEntryInfo) );
    STD_StrCpy(list_temp->src_path, src_path);
    if( dst_path ) {
      STD_StrCpy(list_temp->dst_path, dst_path);
    }
  }
#if 1
  PrintAttributes(entry->attributes, log_fd);
  miya_log_fprintf(log_fd, " %s len=%d\n",src_path,entry->filesize);
#endif

  return TRUE;
}
 
BOOL ClearDirEntryList( MY_DIR_ENTRY_LIST **headp )
{
  MY_DIR_ENTRY_LIST *list_temp1;
  MY_DIR_ENTRY_LIST *list_temp2;

  list_temp1 = *headp;
  *headp = NULL;

  while( list_temp1 ) {
    list_temp2 = list_temp1->next;
    OS_Free( list_temp1 );
    list_temp1 = list_temp2;
  }
  return TRUE;
}

void PrintDirEntryListForward( MY_DIR_ENTRY_LIST *head, FSFile *log_fd )
{
  MY_DIR_ENTRY_LIST *list_temp;
  // char *log_file_name = "sdmc:/miya/save_dir_entry_log.txt";
  miya_log_fprintf(log_fd, "PrintDirEntryListForward-----Start\n");
  if( head == NULL ) {
  }
  else {
    for( list_temp = head ; list_temp != NULL ; list_temp = list_temp->next ) {
      if( list_temp->src_path ) {
	miya_log_fprintf(log_fd, "src name = %s\n", list_temp->src_path );
      }
      if( list_temp->dst_path ) {
	miya_log_fprintf(log_fd, "dst name = %s\n", list_temp->dst_path );
      }
    }
  }
  miya_log_fprintf(log_fd, "PrintDirEntryListForward-----End\n");
}

void PrintDirEntryListBackward( MY_DIR_ENTRY_LIST *head, FSFile *log_fd)
{
  MY_DIR_ENTRY_LIST *list_temp;
  //  MY_DIR_ENTRY_LIST *list_prev;
  miya_log_fprintf(log_fd, "PrintDirEntryListBackword-----Start\n");
  if( head == NULL ) {
  }
  else {
    for( list_temp = head ; list_temp->next != NULL ; list_temp = list_temp->next ) {
      ;
    }
    for(  ; list_temp != NULL ; list_temp = list_temp->prev ) {
      if( list_temp->src_path ) {
	miya_log_fprintf(log_fd, "src name = %s\n", list_temp->src_path );
      }
      if( list_temp->dst_path ) {
	miya_log_fprintf(log_fd, "dst name = %s\n", list_temp->dst_path );
      }
    }
  }
  miya_log_fprintf(log_fd, "PrintDirEntryListBackward-----End\n");
}

static int a_to_int(char c)
{
  if( ('a' <= c) && (c <= 'f') ) {
    return (int)( c - 'a' + 10 );
  }
  else if( ('A' <= c) && (c <= 'F') ) {
    return (int)( c - 'A' + 10 );
  }
  else if( ('0' <= c) && (c <= '9') ) {
    return (int)( c - '0' );
  }
  return -1;
}


static BOOL GetAppGameCode(const char path[], char *code_buf , FSFile *log_fd)
{
  /*
    012345678901234567890123456789
    nand:/title/00030017/484e4141 �� �����`���[
  */
  int c_hi;
  int c_lo;
  int i;
  char *str;
  str = code_buf;

  if( (path == NULL ) || (code_buf == NULL) ) {
    miya_log_fprintf(log_fd, "%s Error:invalid argument\n", __FUNCTION__);
    miya_log_fprintf(log_fd, " path=0x%08x code_buf=0x%08x\n", path, code_buf);
    return FALSE;
  }

  if( STD_StrLen(path) < 28 ) {
    return FALSE;
  }

  for( i = 0 ; i < 8 ; i++ ) {
    if( i % 2 ) {
      c_lo = a_to_int( path[21+i] );
      if( c_lo == -1 ) {
	return FALSE;
      }
      *str = (char)( c_hi << 4 | c_lo );
      str++;
    }
    else {
      c_hi = a_to_int( path[21+i] );
      if( c_hi == -1 ) {
	return FALSE;
      }
    }
  }
  *str = '\0';
  return TRUE;
}

static void AppErrorReport(const char *path, char *msg)
{
  char game_code[5];
  char *dir_name1 = "nand:/shared2";
  char *dir_name2 = "nand2:/photo";
  if( !STD_StrNCmp( path, dir_name1 , STD_StrLen(dir_name1) ) ) {
    (void)Error_Report_Printf(" Shared:%s\n", msg);
  }
  else if( !STD_StrNCmp( path, dir_name2 , STD_StrLen(dir_name2) ) ) {
    (void)Error_Report_Printf(" Photo :%s\n", msg);
  }
  else if( TRUE == GetAppGameCode(path, game_code, NULL ) ) {
    (void)Error_Report_Printf(" [%s]:%s\n", game_code, msg);
  }
  else {
    (void)Error_Report_Printf(" [????]:%s\n   path=%s\n",msg, path);
  }
}


static u64 GetTitleIdFromSrcPath(char path[])
{
  u64 tid = 0;
  int i;

  // 01234567890123456789012345678
  // nand:/title/00030017/484e4141 (�����`���[)

  for( i = 0 ; i < 8 ; i++ ) {
    tid |= ( ((u64)a_to_int( path[12+i] )) << (60-(4*i)) );
  }

  for( i = 0 ; i < 8 ; i++ ) {
    tid |= ( ((u64)a_to_int( path[21+i] )) << (28-(4*i)) );
  }
  return tid;
}

static BOOL CheckInstallSuccessApp( char *path , MY_USER_APP_TID *title_id_buf, int title_id_count )
{
  u64 tid;
  int i;
  MY_USER_APP_TID *temp_title_id_buf;

  temp_title_id_buf = title_id_buf;

  tid = GetTitleIdFromSrcPath( path );

  for( i = 0 ; i < title_id_count ; i++ ) {
    if( tid == temp_title_id_buf[i].tid ) {
      if(temp_title_id_buf[i].install_success_flag == TRUE) {
	/* ���̂Ƃ������o�b�N�A�b�v�𕜊����Ă��B */
	return TRUE;
      }
    }
  }
  return FALSE;
}

static int CheckInstallSuccessAppEx( char *path , MY_USER_APP_TID *title_id_buf, int title_id_count )
{
  u64 tid;
  int i;
  MY_USER_APP_TID *temp_title_id_buf;

  temp_title_id_buf = title_id_buf;

  tid = GetTitleIdFromSrcPath( path );

  for( i = 0 ; i < title_id_count ; i++ ) {
    if( tid == temp_title_id_buf[i].tid ) {
      if(temp_title_id_buf[i].install_success_flag == TRUE) {
	/* ���̂Ƃ������o�b�N�A�b�v�𕜊����Ă��B */
	return 1;
      }
      else {
	return 2;
      }
    }
  }
  return -1; /* ����Ȃh�c�Ȃ� */
}


static BOOL CheckSystemApp(char path[])
{
  char c;
  int num;
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

 012345678901234567890123456789
 nand:/title/00030017/484e4141 �� �����`���[

  */
  c = path[19];
  num = a_to_int( c );

  if( num == -1 ) {
    /* Panic���炢�ł������� */
    miya_log_fprintf(NULL, "%s error: not ascii code-> %c\n",__FUNCTION__, c);
    num = 0;
  }
  if( num & 1 ) { /* System App. */
    return TRUE;
  }
  else { /* User App. */
    return FALSE;
  }
}

//BOOL GetUserAppTitleList( MY_DIR_ENTRY_LIST *head, u64 **pBuffer, int *size, char *log_file_name )
BOOL GetUserAppTitleList( MY_DIR_ENTRY_LIST *head, MY_USER_APP_TID **pBuffer, int *size, char *log_file_name )
{
  int i;
  int count = 0;
  MY_DIR_ENTRY_LIST *list_temp;
  //  u64 *buf = NULL;
  MY_USER_APP_TID *buf = NULL;
  char c;
  u8 hex;
  BOOL log_active = FALSE;
  FSFile *log_fd;
  FSFile log_fd_real;
  BOOL ret_flag = TRUE;

  log_fd = &log_fd_real;
  log_active = Log_File_Open( log_fd, log_file_name );
  if( !log_active ) {
    log_fd = NULL;
  }
  miya_log_fprintf(log_fd, "%s START\n", __FUNCTION__);

  if( head == NULL ) {
    *pBuffer = NULL;
    *size = 0;
  }
  else {
    for( list_temp = head ; list_temp != NULL ; list_temp = list_temp->next ) {
      if( list_temp->src_path ) {
	if( FALSE == CheckSystemApp( list_temp->src_path) ) {
	  count++;
	}
      }
    }
    miya_log_fprintf(log_fd, "User App. count = %d\n", count); 

    if( count ) {
      //      buf = (u64 *)OS_Alloc( (u32)(count * sizeof(u64)) );
      buf = (MY_USER_APP_TID *)OS_Alloc( (u32)(count * sizeof(MY_USER_APP_TID)) );
      if( buf ) {
	//	STD_MemSet((void *)buf, 0, count * sizeof(u64));
	STD_MemSet((void *)buf, 0, count * sizeof(MY_USER_APP_TID));
      }
      else {
	miya_log_fprintf(log_fd, "%s memory allocate error\n",__FUNCTION__);
	ret_flag = FALSE;
	goto function_end;
      }
    }
    else {

    }
    *pBuffer = buf;
    *size = count;

    count = 0;

    for( list_temp = head ; list_temp != NULL ; list_temp = list_temp->next ) {
      if( list_temp->src_path ) {
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
	if( FALSE == CheckSystemApp( list_temp->src_path ) ) {
	  /* User App. */
	  for( i =  0 ; i < 8 ; i++ ) {
	    c = list_temp->src_path[12 + i];
	    hex =  (u8)a_to_int(c);
	    if( hex > -1 ) {
	    }
	    else {
	      /* error! */
	      miya_log_fprintf(log_fd, "%s 1 invalid path name %s\n",__FUNCTION__,list_temp->src_path);
	      if( *size > 0 ) {
		*size = *size - 1;
	      }
	      goto next_loop;
	    }
	    // *buf |= (((u64)hex) << ((7-i)*4 + 32 ));
	    buf->tid |= (((u64)hex) << ((7-i)*4 + 32 ));
	  }
	  for( i =  0 ; i < 8 ; i++ ) {
	    c = list_temp->src_path[21 + i];
	    hex =  (u8)a_to_int(c);
	    if( hex > -1 ) {
	    }
	    else {
	      /* error! */
	      miya_log_fprintf(log_fd, "%s 2 invalid path name %s\n",__FUNCTION__,list_temp->src_path);
	      if( *size > 0 ) {
		*size = *size - 1;
	      }
	      goto next_loop;
	    }
	    // *buf |= (((u64)hex) << ((7-i)*4 ));
	    buf->tid |= (((u64)hex) << ((7-i)*4 ));
	  }
	  buf++;
	  count++;
	}
	/*
	  012345678901234567890123456789
	  nand:/title/00030017/484e4141 �� �����`���[
	  nand:/title/00030015/484e4641 �� shop
	  nand:/title/00030015/484e4241 �� �{�̐ݒ�
	*/
      next_loop:
	;
      }
    }
  }
function_end:
  if( log_active ) {
    miya_log_fprintf(log_fd, "%s END\n\n", __FUNCTION__);
    Log_File_Close(log_fd);
  }
  return ret_flag;
}


void PrintSrcDirEntryListBackward( MY_DIR_ENTRY_LIST *head, FSFile *log_fd)
{
  MY_DIR_ENTRY_LIST *list_temp;
  miya_log_fprintf(log_fd, "PrintSrcDirEntryListBackword-----Start\n");
  if( head == NULL ) {
  }
  else {
    for( list_temp = head ; list_temp->next != NULL ; list_temp = list_temp->next ) {
      ;
    }
    for(  ; list_temp != NULL ; list_temp = list_temp->prev ) {
      if( list_temp->src_path ) {
	miya_log_fprintf(log_fd, "%s\n", list_temp->src_path );
      }
    }
  }
  miya_log_fprintf(log_fd, "PrintSrcDirEntryListBackward-----End\n");
}


BOOL SaveDirEntryList( MY_DIR_ENTRY_LIST *head , char *path, int *list_count, int *error_count, char *log_file_name)
{
  FSFile f;
  FSFile f_src;
  FSFile f_dst;
  FSResult fsResult;
  s32 writtenSize;
  MY_DIR_ENTRY_LIST *list_temp;
  
  FSFile log_fd_real;
  FSFile *log_fd;
  BOOL log_active = FALSE;
  BOOL copy_error_flag;
  
  log_fd = &log_fd_real;

  log_active = Log_File_Open( log_fd, log_file_name );
  if( !log_active ) {
    log_fd = NULL;
  }
  miya_log_fprintf(log_fd, "%s START\n", __FUNCTION__);

  if( (list_count == NULL) || (error_count == NULL) ) {
    miya_log_fprintf(log_fd, "%s Error:invalid argument\n", __FUNCTION__);
    miya_log_fprintf(log_fd, " list ptr=0x%08x error ptr=0x%08x\n", list_count, error_count);
    return FALSE;
  }

  *list_count = 0;
  *error_count = 0;


  /* �ŏ���SD���̃��[�g�f�B���N�g���̃f�[�^�������Ƃ��ׂ����H
     ���������t�@�C�����X�g�ɋL�^���Ă�̂ł��������Ȃ����K�v�Ȃ� */
  FS_InitFile(&f);
  FS_InitFile(&f_src);
  FS_InitFile(&f_dst);

  if( path == NULL ) {
    miya_log_fprintf(log_fd, "%s %d not specify entry save file name\n",__FUNCTION__,__LINE__ );
    return FALSE; /* error */
  }

  FS_CreateFileAuto(path, (FS_PERMIT_R|FS_PERMIT_W));
  if( FALSE == FS_OpenFileEx(&f, path, FS_FILEMODE_W) ) {
    fsResult = FS_GetArchiveResultCode(path);
    miya_log_fprintf(log_fd, "%s %d Failed open-file\n", __FUNCTION__,__LINE__);
    miya_log_fprintf(log_fd, " - dir entry list file:\n");
    miya_log_fprintf(log_fd, "   %s\n", my_fs_util_get_fs_result_word( fsResult ));

    (void)Error_Report_Printf(" ????  :Open file failed.\n   path=%s\n", path);
    return FALSE; /* error */
  }
  fsResult = FS_SetFileLength(&f, 0);
  if( fsResult != FS_RESULT_SUCCESS ) {
    miya_log_fprintf(log_fd, "%s %d Failed set file-len 0\n", __FUNCTION__,__LINE__);
    miya_log_fprintf(log_fd, " - dir entry list file:\n");
    miya_log_fprintf(log_fd, "   %s\n", my_fs_util_get_fs_result_word( fsResult ));

    (void)Error_Report_Printf(" ????  :Open file failed.\n   path=%s\n", path);
    return FALSE; /* error */
  }

  /* �o�b�N���[�h�Ńt�@�C���ɕۑ� */
  if( head == NULL ) {
  }
  else {
    for( list_temp = head ; list_temp->next != NULL ; list_temp = list_temp->next ) {
      ;
      }
    for(  ; list_temp != NULL ; list_temp = list_temp->prev ) {
      copy_error_flag = FALSE;

      miya_log_fprintf(log_fd, "%s\n", list_temp->src_path);

      /* SD���Ƀf�B���N�g���̍쐬�ƃt�@�C���̃R�s�[ */
      if( (list_temp->content.attributes & FS_ATTRIBUTE_IS_DIRECTORY) != 0 ) {
	/* �f�B���N�g���̏ꍇ */
	//	copy_error_flag = FS_CreateDirectoryAuto(list_temp->dst_path, FS_PERMIT_RW);
	//     Create�I�[�g���g������܂����B
	copy_error_flag = FS_CreateDirectory(list_temp->dst_path, FS_PERMIT_RW);

	if(!copy_error_flag ) {
	  fsResult = FS_GetArchiveResultCode(list_temp->dst_path);
	  if( fsResult != FS_RESULT_ALREADY_DONE ) {
	    miya_log_fprintf(log_fd, "%s %d: Failed Create DST Directory\n", __FUNCTION__ , __LINE__ );
	    miya_log_fprintf(log_fd, " %s\n", list_temp->dst_path);
	    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( fsResult ) );
	    //  return -1;
	  }
	  else {
	    copy_error_flag = TRUE; /* �J�b�R���E�E */
	  }
	}
      }
      else {
	/* �t�@�C���̏ꍇ src��NAND, dst��SD */
	if( !STD_StrCmp( list_temp->src_path, "nand:" ) ) {
	  /* nand�̃��[�g�f�B���N�g���̓X���[����B */
	}
	else {
	  /* NAND����SD�ɃR�s�[���� */
	  // CopyFile( dst <= src );
	  // Create�I�[�g���g������܂����B
#ifdef COPY_FILE_ENCRYPTION
	  copy_error_flag = CopyFileCrypto(list_temp->dst_path, list_temp->src_path, log_fd );
#else
	  copy_error_flag = CopyFile(list_temp->dst_path, list_temp->src_path, log_fd );
#endif
	}
      }

      if( copy_error_flag == TRUE ) {
	/* �G���g�����X�g�̃Z�[�u */
	writtenSize = FS_WriteFile(&f, (void *)list_temp, (s32)sizeof(MY_DIR_ENTRY_LIST) );
	if( writtenSize != sizeof(MY_DIR_ENTRY_LIST) ) {
	  miya_log_fprintf(log_fd, "%s %d: Failed write file\n", __FUNCTION__ , __LINE__ );
	  miya_log_fprintf(log_fd, " %s\n\n", path);
	  if( writtenSize > 0 ) {
	    if(FALSE == FS_SeekFile( &f, -writtenSize, FS_SEEK_CUR) ) {
	      fsResult = FS_GetArchiveResultCode(path);
	      miya_log_fprintf(log_fd, "%s %d: Failed seek-file\n", __FUNCTION__ , __LINE__ );
	      miya_log_fprintf(log_fd, " %s\n", path);
	      miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( fsResult ) );
	      /* panic.. */
	    }
	  }
	  (*error_count)++;
	  AppErrorReport(list_temp->src_path, "copy file failed");
	}
	else {
	  (*list_count)++;
	}
      }
      else {
	(*error_count)++;
	AppErrorReport(list_temp->src_path, "copy file failed");
      }

    }
  }
  
  FS_FlushFile(&f);

  if( FS_CloseFile(&f) == FALSE) {
    miya_log_fprintf(log_fd, "%s %d: Failed Close file\n", __FUNCTION__ , __LINE__ );
    miya_log_fprintf(log_fd, " %s\n", path);
    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path) ) );
  }

  miya_log_fprintf(log_fd, "write entry list count  %d\n", *list_count);
  miya_log_fprintf(log_fd, "            error count %d\n", *error_count );
  miya_log_fprintf(log_fd, "%s END\n\n", __FUNCTION__);
  Log_File_Close(log_fd);
  
  if( *error_count > 0 ) {
    return FALSE;
  }
  return TRUE;
}

/********************************************
 * NAND�Ƀf�B���N�g���G���g���ƃt�@�C���𕜊�������B
*********************************************/


BOOL RestoreDirEntryList( char *path , char *log_file_name, int *list_count, int *error_count)
{
  FSFile f;
  FSFile f_dir;
  BOOL bSuccess;
  FSResult fsResult;
  s32 readSize;
  MY_DIR_ENTRY_LIST list_temp;
  FSPathInfo path_info;
  MY_DIR_ENTRY_LIST *readonly_list_head = NULL;
  BOOL copy_error_flag;
  FSFile log_fd_real;
  FSFile *log_fd;
  BOOL log_active = FALSE;
  


  log_fd = &log_fd_real;
  log_active = Log_File_Open( log_fd, log_file_name );
  if( !log_active ) {
    log_fd = NULL;
  }
  miya_log_fprintf(log_fd, "%s START\n", __FUNCTION__);


  if( (list_count == NULL) || (error_count == NULL) ) {
    miya_log_fprintf(log_fd, "%s Error:invalid argument\n", __FUNCTION__);
    miya_log_fprintf(log_fd, " list ptr=0x%08x error ptr=0x%08x\n", list_count, error_count);
    return FALSE;
  }

  *list_count = 0;
  *error_count = 0;

  FS_InitFile(&f);
  FS_InitFile(&f_dir);

  if( FS_OpenFileEx(&f, path, FS_FILEMODE_R) == FALSE) {
    fsResult = FS_GetArchiveResultCode(path);
    miya_log_fprintf(log_fd, "%s %d: Failed Open file\n", __FUNCTION__ , __LINE__ );
    miya_log_fprintf(log_fd, " %s\n", path);
    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( fsResult ) );

    (void)Error_Report_Printf(" ????  :Open file failed.\n   path=%s\n", path);
    return FALSE; /* error */
  }

  while( 1 ) {

    /* ���X�g�̓��[�g�f�B���N�g���ɋ߂�����������Ă��� */
    /* �G���g�����X�g�̃��[�h */
    readSize = FS_ReadFile(&f, (void *)&list_temp, (s32)sizeof(MY_DIR_ENTRY_LIST) );
    if( readSize == 0 ) {
      /* �I��� */
      break;
    }
    else if( readSize != (s32)sizeof(MY_DIR_ENTRY_LIST) ) {
      miya_log_fprintf(log_fd, "%s %d: Failed Read file\n", __FUNCTION__ , __LINE__ );
      miya_log_fprintf(log_fd, " %s\n", path);
      break;
    }

    copy_error_flag = TRUE;


    /* ����������Ƃ悭�l���ď��ԂƂ���蒼�� FS_GetPathInfo�Ƃ����点�����E�E */

    /* NAND���Ƀf�B���N�g���̍쐬�ƃt�@�C���̃R�s�[ */
    if( (list_temp.content.attributes & FS_ATTRIBUTE_IS_DIRECTORY) != 0 ) {
      /* �f�B���N�g���̏ꍇ */
      if( TRUE == FS_GetPathInfo(list_temp.src_path, &path_info) ) {
	/* ��������鑤(NAND)�ɂ��łɉ����t�@�C�����f�B���N�g��������ꍇ */
	if( (path_info.attributes & FS_ATTRIBUTE_IS_DIRECTORY) == 0 ) {
	  /* �f�B���N�g���łȂ��ꍇ �G���[ */
	  /* SD�Ƀ��O���c���ꍇ */
	  miya_log_fprintf(log_fd, "%s %d: NOT a directory\n", __FUNCTION__ , __LINE__ );
	  miya_log_fprintf(log_fd, " %s\n", list_temp.src_path );
	  /* require backup file */
	  /* �p�j�b�N?? */
	  /* ����Ƃ���x�f���[�g����?? */
	  FS_DeleteFile( list_temp.src_path ); /* ������Ɩ�����肩�H */
	  goto label1;
	}
	/* read only�f�B���N�g���������ꍇ�̏����͉��̂ق��ł��B */
      }
      else {
      label1:
	/* ��������鑤(NAND)�Ƀf�B���N�g���G���g�����Ȃ��ꍇ */
	bSuccess = FS_CreateDirectory(list_temp.src_path, FS_PERMIT_RW);
	if(!bSuccess) {
	  fsResult = FS_GetArchiveResultCode(list_temp.src_path);
	  if( fsResult != FS_RESULT_ALREADY_DONE ) {
	    miya_log_fprintf(log_fd, "%s %d: Failed Create NAND Directory\n", __FUNCTION__,__LINE__);
	    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( fsResult ) );
	    miya_log_fprintf(log_fd, " %s\n", list_temp.src_path);
	    copy_error_flag = FALSE;

	  }
	}
	if( FALSE == FS_GetPathInfo(list_temp.src_path, &path_info) ) {
	  miya_log_fprintf(log_fd, "%s %d: Failed GetPathInfo\n", __FUNCTION__,__LINE__ );
	  miya_log_fprintf(log_fd, " %s\n", list_temp.src_path );
	  //  return FALSE;
	}
      }


#ifdef ATTRIBUTE_BACK
      /* ���̃f�B���N�g�����L�����Ă����A���Ƃő������܂Ƃ߂Ė߂� */
      if( FALSE == add_entry_list( &readonly_list_head, &list_temp, log_fd ) ) {
	miya_log_fprintf(log_fd, "%s %d: ERROR: add_entry_list\n", __FUNCTION__,__LINE__ );
	miya_log_fprintf(log_fd, " %s\n", list_temp.src_path );
      }
#endif

      if( (path_info.attributes & FS_ATTRIBUTE_DOS_READONLY) != 0 ) {
	/* ���[�h�I�����[�̏ꍇ,��x���[�h���C�g�\�ɂ��� */
	path_info.attributes &= ~FS_ATTRIBUTE_DOS_READONLY;
	if( FALSE == FS_SetPathInfo( list_temp.src_path, &path_info) ) {
	  fsResult = FS_GetArchiveResultCode(list_temp.src_path);
	  miya_log_fprintf(log_fd, "%s %d: Failed SetPathInfo\n", __FUNCTION__,__LINE__ );
	  miya_log_fprintf(log_fd, " %s\n", list_temp.src_path );
	  miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( fsResult ) );
	}
      }
    }
    else {
      /* �t�@�C���̏ꍇ */
      if( !STD_StrCmp( list_temp.src_path, "nand:" ) ) {
	/* nand�̃��[�g�f�B���N�g���̓X���[����B */
	OS_TPrintf("nand: root detect \n");
      }
      else {

	/* �Ƃ肠�����f�X�e�B�l�[�V�����t�@�C�������邩�ǂ����m�F����
	   �����SD�Ƀo�b�N�A�b�v���� */
	// CopyFile( dst <= src );
#ifdef COPY_FILE_ENCRYPTION
	copy_error_flag = CopyFileCrypto( list_temp.src_path, list_temp.dst_path, log_fd );
#else
	copy_error_flag = CopyFile( list_temp.src_path, list_temp.dst_path, log_fd );
#endif

	if( TRUE == copy_error_flag ) {
	  path_info.attributes  = list_temp.content.attributes;
	  path_info.ctime  = list_temp.content.ctime;
	  path_info.mtime = list_temp.content.mtime;
	  path_info.atime = list_temp.content.atime;
	  path_info.id = list_temp.content.id;
	  path_info.filesize = list_temp.content.filesize;
	  if( FALSE == FS_SetPathInfo( list_temp.src_path, &path_info) ) {
	    fsResult = FS_GetArchiveResultCode(list_temp.src_path);
	    miya_log_fprintf(log_fd, "%s %d: Failed SetPathInfo\n", __FUNCTION__,__LINE__ );
	    miya_log_fprintf(log_fd, " %s\n", list_temp.src_path );
	    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( fsResult ) );
	  }
	}
      }
    }

    /* �t�@�C���ł��f�B���N�g���ł��ǂ����ł�������ʂ� */
    if(  copy_error_flag == FALSE ) {
      (*error_count)++;
      AppErrorReport(list_temp.src_path, "copy file failed");
    }
    else {
      (*list_count)++;
    }
  }

  miya_log_fprintf(log_fd, "%s Read entry count %d error count %d\n",__FUNCTION__ , *list_count, *error_count );

#ifdef ATTRIBUTE_BACK
  /* add_entry_list( &readonly_list_head, &list_temp );
     �Ń��X�g�ɂ����G���g���[�̃A�g���r���[�g���t���Ō��ɖ߂��B*/
  if( FALSE == restore_entry_list(&readonly_list_head, log_fd) ) {
    miya_log_fprintf(log_fd, "%s %d: ERROR: restore_entry_list\n", __FUNCTION__,__LINE__ );
  }
#endif

 exit_label:


  //  (void)ClearDirEntryList( &readonly_list_head );


  //  FS_FlushFile(&f); //���[�h�����炢��Ȃ�
  if( FS_CloseFile(&f) == FALSE) {
    fsResult = FS_GetArchiveResultCode(path);
    miya_log_fprintf(log_fd, "%s %d: Failed Close file\n", __FUNCTION__ , __LINE__ );
    miya_log_fprintf(log_fd, " %s\n", path);
    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( fsResult ) );
  }

  miya_log_fprintf(log_fd, "%s END\n\n", __FUNCTION__);
  if( log_active ) {
    Log_File_Close(log_fd);
  }
  if( *error_count > 0 ) {
    return FALSE;
  }
  return TRUE;
}


BOOL RestoreDirEntryList_System_And_InstallSuccessApp(char *path , char *log_file_name, int *list_count, int *error_count,
						      MY_USER_APP_TID *title_id_buf, int title_id_count )
{
  FSFile f;
  FSFile f_dir;
  BOOL bSuccess;
  FSResult fsResult;
  s32 readSize;
  MY_DIR_ENTRY_LIST list_temp;
  FSPathInfo path_info;
  MY_DIR_ENTRY_LIST *readonly_list_head = NULL;
  BOOL copy_error_flag;
  FSFile log_fd_real;
  FSFile *log_fd;
  BOOL log_active = FALSE;
  u64 tid;
  char game_code_buf[5];

  log_fd = &log_fd_real;
  log_active = Log_File_Open( log_fd, log_file_name );
  if( !log_active ) {
    log_fd = NULL;
  }
  miya_log_fprintf(log_fd, "%s START\n", __FUNCTION__);


  if( (list_count == NULL) || (error_count == NULL) ) {
    miya_log_fprintf(log_fd, "%s Error:invalid argument\n", __FUNCTION__);
    miya_log_fprintf(log_fd, " list ptr=0x%08x error ptr=0x%08x\n", list_count, error_count);
    return FALSE;
  }

  *list_count = 0;
  *error_count = 0;

  FS_InitFile(&f);
  FS_InitFile(&f_dir);

  if( FS_OpenFileEx(&f, path, FS_FILEMODE_R) == FALSE) {
    fsResult = FS_GetArchiveResultCode(path);
    miya_log_fprintf(log_fd, "%s %d: Failed Open file\n", __FUNCTION__ , __LINE__ );
    miya_log_fprintf(log_fd, " %s\n", path);
    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( fsResult ) );
    return FALSE; /* error */
  }

  if( print_debug_flag == TRUE ) {
    mprintf("\n");
  }

  while( 1 ) {
    /* ���X�g�̓��[�g�f�B���N�g���ɋ߂�����������Ă��� */
    readSize = FS_ReadFile(&f, (void *)&list_temp, (s32)sizeof(MY_DIR_ENTRY_LIST) );
    if( readSize == 0 ) {
      /* �I��� */
      break;
    }
    else if( readSize != (s32)sizeof(MY_DIR_ENTRY_LIST) ) {
      miya_log_fprintf(log_fd, "%s %d: Failed Read file\n", __FUNCTION__ , __LINE__ );
      miya_log_fprintf(log_fd, " %s\n", path);
      break;
    }

    copy_error_flag = TRUE;

    /* NAND���Ƀf�B���N�g���̍쐬�ƃt�@�C���̃R�s�[ */
    if( (list_temp.content.attributes & FS_ATTRIBUTE_IS_DIRECTORY) != 0 ) {
      /* �f�B���N�g���̏ꍇ */
      if( TRUE == FS_GetPathInfo(list_temp.src_path, &path_info) ) {
	/* ��������鑤(NAND)�ɂ��łɉ����t�@�C�����f�B���N�g��������ꍇ */
	if( (path_info.attributes & FS_ATTRIBUTE_IS_DIRECTORY) == 0 ) {
	  /* �f�B���N�g���łȂ��ꍇ �G���[ */
	  /* SD�Ƀ��O���c���ꍇ */
	  miya_log_fprintf(log_fd, "%s %d: NOT a directory\n", __FUNCTION__ , __LINE__ );
	  miya_log_fprintf(log_fd, " %s\n", list_temp.src_path );
	  /* require backup file */
	  /* �p�j�b�N?? */
	  /* ����Ƃ���x�f���[�g����?? */
	  FS_DeleteFile( list_temp.src_path ); /* ������Ɩ�����肩�H */
	  goto label1;
	}
	/* read only�f�B���N�g���������ꍇ�̏����͉��̂ق��ł��B */
      }
      else {
      label1:
	/* ��������鑤(NAND)�Ƀf�B���N�g���G���g�����Ȃ��ꍇ */
	bSuccess = FS_CreateDirectory(list_temp.src_path, FS_PERMIT_RW);
	if(!bSuccess) {
	  fsResult = FS_GetArchiveResultCode(list_temp.src_path);
	  if( fsResult != FS_RESULT_ALREADY_DONE ) {
	    miya_log_fprintf(log_fd, "%s %d: Failed Create NAND Directory\n", __FUNCTION__,__LINE__);
	    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( fsResult ) );
	    miya_log_fprintf(log_fd, " %s\n", list_temp.src_path);
	    copy_error_flag = FALSE;
	  }
	}
	if( FALSE == FS_GetPathInfo(list_temp.src_path, &path_info) ) {
	  miya_log_fprintf(log_fd, "%s %d: Failed GetPathInfo\n", __FUNCTION__,__LINE__ );
	  miya_log_fprintf(log_fd, " %s\n", list_temp.src_path );
	  //  return FALSE;
	}
      }


#ifdef ATTRIBUTE_BACK
      /* ���̃f�B���N�g�����L�����Ă����A���Ƃő������܂Ƃ߂Ė߂� */
      if( FALSE == add_entry_list( &readonly_list_head, &list_temp, log_fd ) ) {
	miya_log_fprintf(log_fd, "%s %d: ERROR: add_entry_list\n", __FUNCTION__,__LINE__ );
	miya_log_fprintf(log_fd, " %s\n", list_temp.src_path );
      }
#endif

      if( (path_info.attributes & FS_ATTRIBUTE_DOS_READONLY) != 0 ) {
	/* ���[�h�I�����[�̏ꍇ,��x���[�h���C�g�\�ɂ��� */
	path_info.attributes &= ~FS_ATTRIBUTE_DOS_READONLY;
	if( FALSE == FS_SetPathInfo( list_temp.src_path, &path_info) ) {
	  fsResult = FS_GetArchiveResultCode(list_temp.src_path);
	  miya_log_fprintf(log_fd, "%s %d: Failed SetPathInfo\n", __FUNCTION__,__LINE__ );
	  miya_log_fprintf(log_fd, " %s\n", list_temp.src_path );
	  miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( fsResult ) );
	}
      }
    }
    else {
      /* �t�@�C���̏ꍇ */
      if( !STD_StrCmp( list_temp.src_path, "nand:" ) ) {
	/* nand�̃��[�g�f�B���N�g���̓X���[����B */
	OS_TPrintf("nand: root detect \n");
      }
      else {

	// CopyFile( dst <= src );
	if( (TRUE == CheckSystemApp( list_temp.src_path )) ||
	    (TRUE == CheckInstallSuccessApp(list_temp.src_path, title_id_buf, title_id_count)) ) {

	  /* �ꉞ�g���q(*.sav)���`�F�b�N���Ƃ����ق����������H */
	  miya_log_fprintf(log_fd, "backup %s\n",list_temp.src_path);

	  tid = GetTitleIdFromSrcPath( list_temp.src_path );
	  (void)my_fs_Tid_To_GameCode(tid, game_code_buf);

	  if( print_debug_flag == TRUE ) {
	    mprintf(" id %08X %08X [%s] ", (u32)(tid >> 32), (u32)tid, game_code_buf); 
	  }
	  miya_log_fprintf(log_fd, " id %08X %08X [%s] ", (u32)(tid >> 32), (u32)tid, game_code_buf); 

#ifdef COPY_FILE_ENCRYPTION
	  copy_error_flag = CopyFileCrypto( list_temp.src_path, list_temp.dst_path, log_fd );
#else
	  copy_error_flag = CopyFile( list_temp.src_path, list_temp.dst_path, log_fd );
#endif

	  if( TRUE == copy_error_flag ) {
	    path_info.attributes  = list_temp.content.attributes;
	    path_info.ctime  = list_temp.content.ctime;
	    path_info.mtime = list_temp.content.mtime;
	    path_info.atime = list_temp.content.atime;
	    path_info.id = list_temp.content.id;
	    path_info.filesize = list_temp.content.filesize;
	    if( FALSE == FS_SetPathInfo( list_temp.src_path, &path_info) ) {
	      fsResult = FS_GetArchiveResultCode(list_temp.src_path);
	      miya_log_fprintf(log_fd, "%s %d: Failed SetPathInfo\n", __FUNCTION__,__LINE__ );
	      miya_log_fprintf(log_fd, " %s\n", list_temp.src_path );
	      miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( fsResult ) );
	    }
	    else {
	    }
	    (*list_count)++;

	    /* success */
	    if( print_debug_flag == TRUE ) {
	      m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
	      mprintf("OK.\n");
	      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
	    }
	    miya_log_fprintf(log_fd, "OK.\n");
	  }
	  else {
	    /* fail */
	    if( print_debug_flag == TRUE ) {
	      m_set_palette(tc[0], M_TEXT_COLOR_RED );
	      mprintf("NG.\n");
	      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
	    }
	    miya_log_fprintf(log_fd, "NG.\n");
	  }
	}
	else {
	  /* �C���X�g�[�����s�A�v���̏ꍇ�B */
	  if( 2 == CheckInstallSuccessAppEx(list_temp.src_path, title_id_buf, title_id_count) ) {
	    tid = GetTitleIdFromSrcPath( list_temp.src_path );
	    (void)my_fs_Tid_To_GameCode(tid, game_code_buf);
	    
	    if( print_debug_flag == TRUE ) {
	      mprintf(" id %08X %08X [%s] ", (u32)(tid >> 32), (u32)tid, game_code_buf); 
	      /* fail */
	      m_set_palette(tc[0], M_TEXT_COLOR_RED );
	      mprintf("NG.\n");
	      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
	    }
	    miya_log_fprintf(log_fd, " id %08X %08X [%s] ", (u32)(tid >> 32), (u32)tid, game_code_buf); 
	    miya_log_fprintf(log_fd, "NG.\n");
	    copy_error_flag = FALSE;
	  }

	}
      }
    }

    if( copy_error_flag == FALSE ) {
      (*error_count)++;
      AppErrorReport(list_temp.src_path, "copy file failed");
    }
  }

  miya_log_fprintf(log_fd, "%s Read entry count %d error count %d\n",__FUNCTION__ , *list_count, *error_count );

#ifdef ATTRIBUTE_BACK
  /* add_entry_list( &readonly_list_head, &list_temp );
     �Ń��X�g�ɂ����G���g���[�̃A�g���r���[�g���t���Ō��ɖ߂��B*/
  if( FALSE == restore_entry_list(&readonly_list_head, log_fd) ) {
    miya_log_fprintf(log_fd, "%s %d: ERROR: restore_entry_list\n", __FUNCTION__,__LINE__ );
  }
#endif

 exit_label:

  //  FS_FlushFile(&f); //���[�h�����炢��Ȃ�
  if( FS_CloseFile(&f) == FALSE) {
    fsResult = FS_GetArchiveResultCode(path);
    miya_log_fprintf(log_fd, "%s %d: Failed Close file\n", __FUNCTION__ , __LINE__ );
    miya_log_fprintf(log_fd, " %s\n", path);
    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( fsResult ) );
  }

  //  (void)ClearDirEntryList( &readonly_list_head );

  miya_log_fprintf(log_fd, "%s END\n\n", __FUNCTION__);
  if( log_active ) {
    Log_File_Close(log_fd);
  }
  if( *error_count > 0 ) {
    return FALSE;
  }
  return TRUE;
}




static BOOL my_fs_is_Title_Hi_dir_name(const char *name)
{
  char *str;
  int i;
  str = (char *)name;
  if( STD_StrLen(name) != 8   /* 9 ? */) {
    return FALSE;
  }
  
  for( i = 0 ; i < 8 ; i++ ) {
    if( (( '0' <= *str ) && (* str <= '9')) ||
	(( 'a' <= *str ) && (* str <= 'f')) ||
	(( 'A' <= *str ) && (* str <= 'F')) ) {
    }
    else {
      return FALSE;
    }
    str++;
  }
  return TRUE;
}

/*
  nand:/title/00030017/484e4141 �� �����`���[
  nand:/title/00030015/484e4641 �� shop
  nand:/title/00030015/484e4241 �� �{�̐ݒ�
*/

static BOOL my_fs_is_Title_Lo_dir_name(const char *name)
{
  char *str;
  int i;
  str = (char *)name;
  if( STD_StrLen(name) != 8   /* 9 ? */) {
    return FALSE;
  }

  for( i = 0 ; i < 8 ; i++ ) {
    if( (( '0' <= *str ) && (* str <= '9')) ||
	(( 'a' <= *str ) && (* str <= 'f')) ||
	(( 'A' <= *str ) && (* str <= 'F')) ) {
    }
    else {
      return FALSE;
    }
    str++;
  }
  return TRUE;
}

static BOOL Path_Buffers_Init(const char *path_src, const char *path_dst, 
			      char **path_src_dir, char **path_src_full, 
			      char **path_dst_dir, char **path_dst_full,
			      FSFile *log_fd)
{
  *path_src_dir = (char *)OS_Alloc( FILE_PATH_LEN );
  if( *path_src_dir == NULL ) {
    miya_log_fprintf(log_fd, "Error: alloc error src_dir\n");
    return FALSE;
  }
  *path_src_full = (char *)OS_Alloc( FILE_PATH_LEN );
  if( *path_src_full == NULL ) {
    miya_log_fprintf(log_fd, "Error: alloc error src_full\n");
    return FALSE;
  }
  *path_dst_dir = (char *)OS_Alloc( FILE_PATH_LEN );
  if( *path_dst_dir == NULL ) {
    miya_log_fprintf(log_fd, "Error: alloc error dst_dir\n");
    return FALSE;
  }
  *path_dst_full = (char *)OS_Alloc( FILE_PATH_LEN );
  if( *path_dst_full == NULL ) {
    miya_log_fprintf(log_fd, "Error: alloc error dst_full\n");
    return FALSE;
  }

  STD_MemSet((void *)*path_src_dir, 0, FILE_PATH_LEN);
  STD_MemSet((void *)*path_src_full, 0, FILE_PATH_LEN);
  STD_MemSet((void *)*path_dst_dir, 0, FILE_PATH_LEN);
  STD_MemSet((void *)*path_dst_full, 0, FILE_PATH_LEN);

  STD_StrCpy(*path_src_dir, path_src);
  STD_StrCat(*path_src_dir, "/");
  STD_StrCpy(*path_dst_dir, path_dst);
  STD_StrCat(*path_dst_dir, "/");

  return TRUE;
}

static void Path_Buffers_Clean( char *path_src_dir, char *path_src_full, 
				char *path_dst_dir,char *path_dst_full)
{
  if( path_src_dir != NULL ) {
    OS_Free(path_src_dir);
  }
  if( path_src_full != NULL ) {
    OS_Free(path_src_full);
  }
  if( path_dst_dir != NULL ) {
    OS_Free(path_dst_dir);
  }
  if( path_dst_full != NULL ) {
    OS_Free(path_dst_full);
  }
}




BOOL MydataLoadDecrypt(const char *path, void *pBuffer, int size, FSFile *log_fd)
{
  FSFile f;
  BOOL bSuccess;
  //  u32 fileSize;
  s32 readSize = 0;
  
  FS_InitFile(&f);
  
  bSuccess = FS_OpenFileEx(&f, path, FS_FILEMODE_R);
  if( ! bSuccess ) {
    miya_log_fprintf(log_fd, "Failed Open File %s\n",__FUNCTION__);
    miya_log_fprintf(log_fd, " path=%s\n", path );
    miya_log_fprintf(log_fd, " res=%s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path) ));
    return FALSE;
  }
  readSize = my_fs_crypto_read(&f, pBuffer, (s32)size);
  if( readSize != size ) {
    miya_log_fprintf(log_fd, "%s Failed Read File: %s\n",__FUNCTION__,path);
    (void)FS_CloseFile(&f);
    return FALSE;
  }
  bSuccess = FS_CloseFile(&f);
  if( ! bSuccess ) {
    miya_log_fprintf(log_fd, "Failed Close File\n");
    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path)));
  }
  return TRUE;
 }


BOOL MydataSaveEncrypt(const char *path, void *pData, int size, FSFile *log_fd)
{
  FSFile f;
  //  BOOL flag;
  BOOL bSuccess;
  FSResult res;
  FSResult fsResult;
  s32 writtenSize;

  FS_InitFile(&f);

  bSuccess = FS_OpenFileEx(&f, path, FS_FILEMODE_W);
  if( ! bSuccess ) {
    FS_CreateFileAuto( path, FS_PERMIT_W|FS_PERMIT_R);
    bSuccess = FS_OpenFileEx(&f, path , FS_FILEMODE_W );
    if( ! bSuccess ) {
      res = FS_GetArchiveResultCode( path );
      miya_log_fprintf(log_fd, "%s file open error %s\n", __FUNCTION__,path );
      miya_log_fprintf(log_fd, " Failed open file:%s\n", my_fs_util_get_fs_result_word( res ));
      return FALSE;
    }
  }

  fsResult = FS_SetFileLength(&f, 0);
  if( fsResult != FS_RESULT_SUCCESS ) {
  }

  writtenSize = my_fs_crypto_write(&f, pData, (s32)size);
  if( writtenSize != size ) {
    miya_log_fprintf(log_fd, "%s Failed Write File: %s\n",__FUNCTION__,path);
    return FALSE;
  }

  FS_FlushFile(&f);
  bSuccess = FS_CloseFile(&f);
  if( bSuccess ) {
      
  }
  return TRUE;
}


#if 0
BOOL MydataLoad(const char *path, void *pBuffer, int size, FSFile *log_fd)
{
  FSFile f;
  BOOL bSuccess;
  //  u32 fileSize;
  s32 readSize = 0;
  
  FS_InitFile(&f);
  
  bSuccess = FS_OpenFileEx(&f, path, FS_FILEMODE_R);
  if( ! bSuccess ) {
    miya_log_fprintf(log_fd, "Failed Open File %s\n",__FUNCTION__);
    miya_log_fprintf(log_fd, " path=%s\n", path );
    miya_log_fprintf(log_fd, " res=%s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path) ));
    return FALSE;
  }
  readSize = FS_ReadFile(&f, pBuffer, (s32)size);
  if( readSize != size ) {
    miya_log_fprintf(log_fd, "Failed Read File: %s\n",path);
  }
  bSuccess = FS_CloseFile(&f);
  if( ! bSuccess ) {
    miya_log_fprintf(log_fd, "Failed Close File\n");
    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path)));
  }
  
  return TRUE;
}
#endif

#if 0
BOOL MydataSave(const char *path, void *pData, int size, FSFile *log_fd)
{

  FSFile f;
  //  BOOL flag;
  BOOL bSuccess;
  FSResult res;
  FSResult fsResult;
  s32 writtenSize;

  FS_InitFile(&f);

  bSuccess = FS_OpenFileEx(&f, path, FS_FILEMODE_W);
  if( ! bSuccess ) {
    FS_CreateFileAuto( path, FS_PERMIT_W|FS_PERMIT_R);
    bSuccess = FS_OpenFileEx(&f, path , FS_FILEMODE_W );
    if( ! bSuccess ) {
      res = FS_GetArchiveResultCode( path );
      miya_log_fprintf(NULL, "%s file open error %s\n", __FUNCTION__,path );
      miya_log_fprintf(NULL, " Failed open file:%s\n", my_fs_util_get_fs_result_word( res ));
      return FALSE;
    }
  }

  fsResult = FS_SetFileLength(&f, 0);
  if( fsResult != FS_RESULT_SUCCESS ) {
  }

  writtenSize = FS_WriteFile(&f, pData, (s32)size);
  if( writtenSize != size ) {
    return FALSE;
  }

  FS_FlushFile(&f);
  bSuccess = FS_CloseFile(&f);
  if( bSuccess ) {
      
  }
  return TRUE;
}
#endif

//BOOL TitleIDLoad(const char *path, u64 **pBuffer, int *count, char *log_file_name)
BOOL TitleIDLoad(const char *path, MY_USER_APP_TID **pBuffer, int *count, char *log_file_name)
{
  FSFile f;
  BOOL bSuccess;
  //  u32 fileSize;
  s32 readSize = 0;
  int id_count= 0;
  int size;
  BOOL log_active = FALSE;
  FSFile *log_fd;
  FSFile log_fd_real;
  BOOL ret_flag = TRUE;

  log_fd = &log_fd_real;

  FS_InitFile(&f);

  log_active = Log_File_Open( log_fd, log_file_name );
  if( !log_active ) {
    log_fd = NULL;
  }
  miya_log_fprintf(log_fd, "%s START\n", __FUNCTION__);
  
  bSuccess = FS_OpenFileEx(&f, path, FS_FILEMODE_R);
  if( ! bSuccess ) {
    miya_log_fprintf(log_fd, "Failed Open File %s\n",__FUNCTION__);
    miya_log_fprintf(log_fd, " path=%s\n", path );
    miya_log_fprintf(log_fd, " res=%s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path) ));
    ret_flag = FALSE;
    goto function_end;
  }

  if( sizeof(int) != FS_ReadFile(&f, &id_count, (s32)sizeof(int)) ) {
    miya_log_fprintf(log_fd, "Failed Read File %s\n",__FUNCTION__);
    miya_log_fprintf(log_fd, " path=%s\n", path );
    miya_log_fprintf(log_fd, " res=%s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path) ));
    ret_flag = FALSE;
    goto function_end;
  } 


  *count = id_count;
  //  size = (int)sizeof(u64) * id_count;
  size = (int)sizeof(MY_USER_APP_TID) * id_count;

  //  *pBuffer = (u64 *)OS_Alloc( (u32)size );
  *pBuffer = (MY_USER_APP_TID *)OS_Alloc( (u32)size );
  if( *pBuffer == NULL ) {
    ret_flag = FALSE;
    miya_log_fprintf(log_fd, "%s Failed memory alloc size %d\n",__FUNCTION__, size);
    goto function_end;
  }

  readSize = FS_ReadFile(&f, (void *)*pBuffer, (s32)size );
  if( readSize != size ) {
    miya_log_fprintf(log_fd, "Failed Read File: %s request size %d read size %d\n",path, size, readSize);
    if( readSize != size ) {
      ret_flag = FALSE;
      goto function_end;
    }
  }


 function_end:

  bSuccess = FS_CloseFile(&f);
  if( ! bSuccess ) {
    miya_log_fprintf(log_fd, "Failed Close File\n");
    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path)));
  }
  
  if( log_active ) {
    miya_log_fprintf(log_fd, "%s END\n\n", __FUNCTION__);
    Log_File_Close(log_fd);
  }
  return ret_flag;
}

BOOL TitleIDSave(const char *path, MY_USER_APP_TID *pData, int count, char *log_file_name )
{
  FSFile f;
  BOOL bSuccess;
  FSResult res;
  FSResult fsResult;
  //  s32 writtenSize;
  BOOL log_active = FALSE;
  FSFile *log_fd;
  FSFile log_fd_real;
  BOOL ret_flag = TRUE;

  log_fd = &log_fd_real;

  FS_InitFile(&f);

  //MY_USER_APP_TID

  log_active = Log_File_Open( log_fd, log_file_name );
  if( !log_active ) {
    log_fd = NULL;
  }
  miya_log_fprintf(log_fd, "%s START\n", __FUNCTION__);


  if( count < 1 ) {
    miya_log_fprintf(log_fd, "%s path=%s count=%d pData=0x%08x\n", __FUNCTION__,path,count, pData );
    goto function_end;
  }
  if( path == NULL ) {
    miya_log_fprintf(log_fd, "%s path=%s count=%d pData=0x%08x\n", __FUNCTION__,path,count, pData );
    goto function_end;
  }
  if( pData == NULL ) {
    miya_log_fprintf(log_fd, "%s path=%s count=%d pData=0x%08x\n", __FUNCTION__,path,count, pData );
    goto function_end;
  }

  bSuccess = FS_OpenFileEx(&f, path, FS_FILEMODE_W);
  if( ! bSuccess ) {
    FS_CreateFileAuto( path, FS_PERMIT_W|FS_PERMIT_R);
    bSuccess = FS_OpenFileEx(&f, path , FS_FILEMODE_W );
    if( ! bSuccess ) {
      res = FS_GetArchiveResultCode( path );
      miya_log_fprintf(log_fd, "%s file open error %s\n", __FUNCTION__,path );
      miya_log_fprintf(log_fd, " Failed open file:%s\n", my_fs_util_get_fs_result_word( res ));
      ret_flag = FALSE;
      goto function_end;
    }
  }

  fsResult = FS_SetFileLength(&f, 0);
  if( fsResult != FS_RESULT_SUCCESS ) {
    res = FS_GetArchiveResultCode( path );
    miya_log_fprintf(log_fd, "%s file length error %s\n", __FUNCTION__,path );
    miya_log_fprintf(log_fd, " Failed file lenght :%s\n", my_fs_util_get_fs_result_word( res ));
    ret_flag = FALSE;
    goto function_end;
  }

  if( sizeof(int) != FS_WriteFile(&f, &count, (s32)sizeof(int)) ) {
    res = FS_GetArchiveResultCode( path );
    miya_log_fprintf(log_fd, "%s file write error %s\n", __FUNCTION__,path );
    miya_log_fprintf(log_fd, " Failed write file:%s\n", my_fs_util_get_fs_result_word( res ));
    ret_flag = FALSE;
    goto function_end;
  }
  else {
    miya_log_fprintf(log_fd, "num of title id = %d\n", count);
  }

  /*
    nand:/title/00030005/484e4541
    nand:/title/00030005/484e4441
    nand:/title/0003000f/484e4c4a
    nand:/title/0003000f/484e4841
    nand:/title/0003000f/484e4341
    nand:/title/00030015/484e424a
    nand:/title/00030017/484e414a
  */

  if( ( pData != NULL ) && ( count != 0 ) ) {
    /* 16���������� */
    //    if( (count*sizeof(u64)) != FS_WriteFile(&f, pData, (s32)(count*sizeof(u64)) )) {
    if( (count*sizeof(MY_USER_APP_TID)) != FS_WriteFile(&f, pData, (s32)(count*sizeof(MY_USER_APP_TID)) )) {
      res = FS_GetArchiveResultCode( path );
      miya_log_fprintf(log_fd, "%s file write error %s\n", __FUNCTION__,path );
      miya_log_fprintf(log_fd, " Failed write file:%s\n", my_fs_util_get_fs_result_word( res ));
      ret_flag = FALSE;
      goto function_end;
    }
    else {
      int j;
      // u64 *ptr = pData;
      MY_USER_APP_TID *ptr = pData;
  
      if( ptr != NULL && count > 0 )  {
	for( j = 0 ; j < count ; j++ ) {
	  //	  miya_log_fprintf(log_fd,"No. %d 0x%016llx\n",j,*ptr);
	  miya_log_fprintf(log_fd,"No. %d 0x%016llx\n",j,ptr->tid);
	  ptr++;
	}
      }
    }
  }

  FS_FlushFile(&f);

 function_end:

  bSuccess = FS_CloseFile(&f);
  if( bSuccess ) {
      
  }

  if( log_active ) {
    miya_log_fprintf(log_fd, "%s END\n\n", __FUNCTION__);
    Log_File_Close(log_fd);
  }


  return ret_flag;

}

BOOL TitleIDLoadETicketOnly(const char *path, u64 **pBuffer, int *count, char *log_file_name)
{
  FSFile f;
  BOOL bSuccess;
  //  u32 fileSize;
  s32 readSize = 0;
  int id_count= 0;
  int size;
  BOOL log_active = FALSE;
  FSFile *log_fd;
  FSFile log_fd_real;
  BOOL ret_flag = TRUE;

  log_fd = &log_fd_real;

  FS_InitFile(&f);

  log_active = Log_File_Open( log_fd, log_file_name );
  if( !log_active ) {
    log_fd = NULL;
  }
  miya_log_fprintf(log_fd, "%s START\n", __FUNCTION__);
  
  bSuccess = FS_OpenFileEx(&f, path, FS_FILEMODE_R);
  if( ! bSuccess ) {
    miya_log_fprintf(log_fd, "Failed Open File %s\n",__FUNCTION__);
    miya_log_fprintf(log_fd, " path=%s\n", path );
    miya_log_fprintf(log_fd, " res=%s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path) ));
    ret_flag = FALSE;
    goto function_end;
  }

  if( sizeof(int) != FS_ReadFile(&f, &id_count, (s32)sizeof(int)) ) {
    miya_log_fprintf(log_fd, "Failed Read File %s\n",__FUNCTION__);
    miya_log_fprintf(log_fd, " path=%s\n", path );
    miya_log_fprintf(log_fd, " res=%s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path) ));
    ret_flag = FALSE;
    goto function_end;
  } 

  *count = id_count;
  size = (int)sizeof(u64) * id_count;

  *pBuffer = (u64 *)OS_Alloc( (u32)size );
  if( *pBuffer == NULL ) {
    ret_flag = FALSE;
    miya_log_fprintf(log_fd, "%s Failed memory alloc size %d\n",__FUNCTION__, size);
    goto function_end;
  }
  readSize = FS_ReadFile(&f, (void *)*pBuffer, (s32)size );
  if( readSize != size ) {
    miya_log_fprintf(log_fd, "Failed Read File: %s request size %d read size %d\n",path, size, readSize);
    if( readSize != size ) {
      ret_flag = FALSE;
      goto function_end;
    }
  }
 function_end:

  bSuccess = FS_CloseFile(&f);
  if( ! bSuccess ) {
    miya_log_fprintf(log_fd, "Failed Close File\n");
    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path)));
  }
  
  if( log_active ) {
    miya_log_fprintf(log_fd, "%s END\n\n", __FUNCTION__);
    Log_File_Close(log_fd);
  }
  return ret_flag;
}

BOOL TitleIDSaveETicketOnly(const char *path, u64 *pData, int count, char *log_file_name )
{
  FSFile f;
  BOOL bSuccess;
  FSResult res;
  FSResult fsResult;
  //  s32 writtenSize;
  BOOL log_active = FALSE;
  FSFile *log_fd;
  FSFile log_fd_real;
  BOOL ret_flag = TRUE;

  log_fd = &log_fd_real;

  FS_InitFile(&f);

  log_active = Log_File_Open( log_fd, log_file_name );
  if( !log_active ) {
    log_fd = NULL;
  }
  miya_log_fprintf(log_fd, "%s START\n", __FUNCTION__);

  if( count < 1 ) {
    miya_log_fprintf(log_fd, "%s path=%s count=%d pData=0x%08x\n", __FUNCTION__,path,count, pData );
    goto function_end;
  }
  if( path == NULL ) {
    miya_log_fprintf(log_fd, "%s path=%s count=%d pData=0x%08x\n", __FUNCTION__,path,count, pData );
    goto function_end;
  }
  if( pData == NULL ) {
    miya_log_fprintf(log_fd, "%s path=%s count=%d pData=0x%08x\n", __FUNCTION__,path,count, pData );
    goto function_end;
  }

  bSuccess = FS_OpenFileEx(&f, path, FS_FILEMODE_W);
  if( ! bSuccess ) {
    FS_CreateFileAuto( path, FS_PERMIT_W|FS_PERMIT_R);
    bSuccess = FS_OpenFileEx(&f, path , FS_FILEMODE_W );
    if( ! bSuccess ) {
      res = FS_GetArchiveResultCode( path );
      miya_log_fprintf(log_fd, "%s file open error %s\n", __FUNCTION__,path );
      miya_log_fprintf(log_fd, " Failed open file:%s\n", my_fs_util_get_fs_result_word( res ));
      ret_flag = FALSE;
      goto function_end;
    }
  }

  fsResult = FS_SetFileLength(&f, 0);
  if( fsResult != FS_RESULT_SUCCESS ) {
    res = FS_GetArchiveResultCode( path );
    miya_log_fprintf(log_fd, "%s file length error %s\n", __FUNCTION__,path );
    miya_log_fprintf(log_fd, " Failed file lenght :%s\n", my_fs_util_get_fs_result_word( res ));
    ret_flag = FALSE;
    goto function_end;
  }

  if( sizeof(int) != FS_WriteFile(&f, &count, (s32)sizeof(int)) ) {
    res = FS_GetArchiveResultCode( path );
    miya_log_fprintf(log_fd, "%s file write error %s\n", __FUNCTION__,path );
    miya_log_fprintf(log_fd, " Failed write file:%s\n", my_fs_util_get_fs_result_word( res ));
    ret_flag = FALSE;
    goto function_end;
  }
  else {
    miya_log_fprintf(log_fd, "num of title id = %d\n", count);
  }

  if( ( pData != NULL ) && ( count != 0 ) ) {
    /* 16���������� */
    if( (count*sizeof(u64)) != FS_WriteFile(&f, pData, (s32)(count*sizeof(u64)) )) {
      res = FS_GetArchiveResultCode( path );
      miya_log_fprintf(log_fd, "%s file write error %s\n", __FUNCTION__,path );
      miya_log_fprintf(log_fd, " Failed write file:%s\n", my_fs_util_get_fs_result_word( res ));
      ret_flag = FALSE;
      goto function_end;
    }
    else {
      int j;
      u64 *ptr = pData;
  
      if( ptr != NULL && count > 0 )  {
	for( j = 0 ; j < count ; j++ ) {
	  miya_log_fprintf(log_fd,"No. %d 0x%016llx\n",j, *ptr);
	  ptr++;
	}
      }
    }
  }
  FS_FlushFile(&f);
 function_end:
  bSuccess = FS_CloseFile(&f);
  if( bSuccess ) {
      
  }
  if( log_active ) {
    miya_log_fprintf(log_fd, "%s END\n\n", __FUNCTION__);
    Log_File_Close(log_fd);
  }
  return ret_flag;
}




/* TWL�J�[�h�����邩�ǂ��� */
static BOOL flag_TWLCardValidation = FALSE;

BOOL TWLCardValidation(void)
{
  if( TRUE == OS_IsRunOnDebugger() ) {
    if( flag_TWLCardValidation == FALSE ) {
      flag_TWLCardValidation = TRUE;
      //  OS_TPrintf("%s %d\n",__FUNCTION__,__LINE__);
    }
    return FALSE; /* �J�[�h�͔����Ă��邱�Ƃɂ���B */
  }
  /*
    CARD_IsPulledOut�������ɂ̓A���}�W���̉���������B
  */

  if( TRUE == CARD_IsPulledOut() ) {
    return FALSE;
  }
  return TRUE;
}

/* SD�J�[�h�����邩�ǂ��� */
BOOL SDCardValidation(void)
{
  FSFile f;
  BOOL flag;
  FS_InitFile(&f);
  if(FS_OpenDirectory(&f, "sdmc:/", FS_PERMIT_R) == FALSE ) {
    flag = FALSE;
  }
  else {
    flag = TRUE;
  }
  (void)FS_CloseDirectory(&f);
  return flag;
}


/* 
   �ߋ��ɃV���b�v�ɐڑ��������ǂ���
   ���[�W�����R�[�h�͈ȉ��̃t�@�C���Œ�`
   c:/twlsdk/include/twl/os/common/ownerInfoEx.h
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
*/


BOOL CheckShopRecord(FSFile *log_fd)
{
  FSFile f;
#if 0
  FSResult res;
#endif
  BOOL ret_flag = TRUE;
  BOOL bSuccess;
  char path[64];
  s32 readSize = 0;
  /* "nand:/sys/log/shop.log  */
  miya_log_fprintf(log_fd, "%s START\n", __FUNCTION__);

  FS_InitFile(&f);
  //  STD_StrCpy(path, "nand:/sys/dev.kp");
  STD_StrCpy(path, "nand:/sys/log/shop.log");

  /* 
     shop.log��
     �{�̐ݒ菉�����ł͏����Ȃ��B
     �V���b�v�̗��������ł͏����B
     ���R�A�V���b�v�ɍĐڑ�����������B
 */

  bSuccess = FS_OpenFileEx(&f, path, (FS_FILEMODE_R));
  if( ! bSuccess ) {
    if( FS_RESULT_NO_ENTRY == FS_GetArchiveResultCode(path) ) {
      /* �L�[�y�A�t�@�C�����Ȃ� */
      /* Shop���O�t�@�C�����Ȃ� */
      ret_flag = FALSE;
      //   miya_log_fprintf(log_fd, "No key pair file\n");
      miya_log_fprintf(log_fd, "No shop log file\n");
    }
  }
  else {
    (void)FS_CloseFile(&f);
  }


  /* 
     �{�̏�������ł��V���b�v���R�[�h�͎c���Ă���̂�
     ���̔���͂���Ȃ��B
   */
#if  0
  // STD_StrCpy(path, "nand:/title/00030015/484e464a/data/ec.cfg"); /* �V���b�v�A�J�E���g��� */
  /* �C�O���ƕς���Ă���E�E */
  /* ���[�W�����R�[�h�ƍ��킹��-> ���[�W�����R�[�h�͕ς����Ȃ�����B */
  /*
    J(0x4a) - Japan
    E(0x45) - America    
    P(0x50) - Europe
    U(0x55) - Australia
    C(0x43) - China
    K(0x4b) - Korea
  */
  
  miya_log_fprintf(log_fd, "device region ");

  /*  STD_StrCpy(path, "nand:/title/00030015/484e4641/data/ec.cfg"); ALL Region */
  switch( region ) {
  case OS_TWL_REGION_JAPAN:
    /* J(0x4a) - Japan */
    miya_log_fprintf(log_fd, "Japan");
    STD_StrCpy(path, "nand:/title/00030015/484e464a/data/ec.cfg");
    break;
  case OS_TWL_REGION_AMERICA:
    /* E(0x45) - America  */   
    miya_log_fprintf(log_fd, "US");
    STD_StrCpy(path, "nand:/title/00030015/484e4645/data/ec.cfg");
    break;
  case OS_TWL_REGION_EUROPE:
    /* P(0x50) - Europe */
    miya_log_fprintf(log_fd, "EUROPE");
    STD_StrCpy(path, "nand:/title/00030015/484e4650/data/ec.cfg");
    break;
  case OS_TWL_REGION_AUSTRALIA:
    /* U(0x55) - Australia */
    miya_log_fprintf(log_fd, "Australie");
    STD_StrCpy(path, "nand:/title/00030015/484e4655/data/ec.cfg");
    break;
  case OS_TWL_REGION_CHINA:
    /* C(0x43) - China */
    miya_log_fprintf(log_fd, "China");
    STD_StrCpy(path, "nand:/title/00030015/484e4643/data/ec.cfg");
    break;
  case OS_TWL_REGION_KOREA:
    /* K(0x4b) - Korea */
    miya_log_fprintf(log_fd, "Korea");
    STD_StrCpy(path, "nand:/title/00030015/484e464b/data/ec.cfg");
    break;
  default:
    miya_log_fprintf(log_fd, "Unknown region");
    ret_flag = FALSE;
  }
  miya_log_fprintf(log_fd, "\n");

  if( ret_flag == TRUE ) {
    bSuccess = FS_OpenFileEx(&f, path, (FS_FILEMODE_R));
    if( ! bSuccess ) {
      res = FS_GetArchiveResultCode(path);
      if( FS_RESULT_NO_ENTRY == res ) {
	/* �V���b�v�A�J�E���g���t�@�C�����Ȃ� */
	ret_flag = FALSE;
	miya_log_fprintf(log_fd, "No ec.cfg file %s\n",path);
      }
    }
    (void)FS_CloseFile(&f);
  }
#endif

  miya_log_fprintf(log_fd, "%s END\n\n", __FUNCTION__);

  return ret_flag;
}


//static BOOL 

static BOOL myCleanDirectory(char *path, FSFile *log_fd, int level)
{
  FSFile f;
  BOOL ret_value = TRUE;
  //  FSDirectoryEntryInfo direntry;
  //  char path_full[FILE_PATH_LEN];
  FSDirectoryEntryInfo *direntry;
  char *path_full;

  //  OS_TPrintf("level = %d\n", level); /* 8�ɂȂ�����OpenDirectory�ŃG���[�ɂȂ�B */

  path_full = (char *)OS_Alloc( FILE_PATH_LEN );
  if( path_full == NULL ) {
    miya_log_fprintf(log_fd, "Error: alloc error path_full\n");
    ret_value = FALSE;
    goto end_process;
  }

  direntry = (FSDirectoryEntryInfo *)OS_Alloc( sizeof(FSDirectoryEntryInfo) );
  if( direntry == NULL ) {
    miya_log_fprintf(log_fd, "Error: alloc error FSDirectoryEntryInfo\n");
    ret_value = FALSE;
    goto end_process;
  }

  /* �\�[�X�f�B���N�g���I�[�v�� */
  FS_InitFile(&f);

  if(FS_OpenDirectory(&f, path, FS_PERMIT_R | FS_PERMIT_W ) == FALSE ) {
    //  if(FS_OpenDirectory(&f, path, 0 ) == FALSE ) {

    miya_log_fprintf(log_fd, "%s %d: Failed Open Directory\n", __FUNCTION__ , __LINE__ );
    miya_log_fprintf(log_fd, " %s\n", path);
    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path) ) );
    ret_value = FALSE;
    (void)Error_Report_Printf(" Open directory failed:%s\n",path);
    goto end_process;
  }

  while( FS_ReadDirectory(&f, direntry) ) {
    if( STD_StrCmp(direntry->longname, ".") == 0 ) {
    }
    else if( STD_StrCmp(direntry->longname, "..") == 0 ) {
    }
    else if( direntry->attributes & FS_ATTRIBUTE_DOS_VOLUME ) {
    }
    else {

      STD_StrCpy( path_full , path );
      STD_StrCat( path_full, "/");
      STD_StrCat( path_full , direntry->longname );

      if( (direntry->attributes & FS_ATTRIBUTE_IS_DIRECTORY) != 0 ) {
	if( level >= 6 ) {
	  if( FALSE == FS_DeleteDirectoryAuto( path_full ) ) {
	    OS_TPrintf("FS_DeleteDirectoryAuto failed.: ");
	    PrintAttributes(direntry->attributes, log_fd);
	    OS_TPrintf(" %s\n", path_full);
	    OS_TPrintf(" %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path_full) ) );
	    mprintf("FS_DeleteDirectoryAuto failed. %s\n", path_full);
	    ret_value = FALSE;
	  }
	  else {
	    // OS_TPrintf("done\n");
	  }
	}
	else {
	  (void)myCleanDirectory(path_full, log_fd, level+1 );
	  //	OS_TPrintf("FS_DeleteDirectory ");

	  if( FALSE == FS_DeleteDirectory( path_full ) ) {
	    OS_TPrintf("FS_DeleteDirectory failed.: ");
	    PrintAttributes(direntry->attributes, log_fd);
	    OS_TPrintf(" %s\n", path_full);
	    OS_TPrintf(" %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path_full) ) );
	    mprintf("FS_DeleteDirectory failed. %s\n", path_full);
	    ret_value = FALSE;
	  }
	  else {
	    // OS_TPrintf("done\n");
	  }
	}
      }
      else {
	/* �t�@�C���̏ꍇ */
	//	OS_TPrintf("FS_DeleteFile ");

	if( FALSE == FS_DeleteFile( path_full ) ) {
	  OS_TPrintf("FS_DeleteFile failed.: ");
	  PrintAttributes(direntry->attributes, log_fd);
	  OS_TPrintf(" %s\n", path_full);
	  mprintf("FS_DeleteFile failed. %s\n", path_full);
	  ret_value = FALSE;
	}
	else {
	  //	  OS_TPrintf("done\n");
	}
      }
    }
  }

 end_process:


  if( FS_CloseDirectory(&f) == FALSE) {
    miya_log_fprintf(log_fd, "%s %d: Failed Close Directory\n", __FUNCTION__ , __LINE__ );
    miya_log_fprintf(log_fd, " %s\n", path);
    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path)));
    //    ret_value |= 1; /* ����Ȃ������H���Ƃōl���� */
  }

  if( path_full != NULL ) {
    OS_Free(path_full);
  }

  if( direntry != NULL ) {
    OS_Free(direntry);
  }

  return ret_value;


}


BOOL CleanSDCardFiles(char *log_file_name)
{
  char *path = "sdmc:/";
  FSFile *log_fd;
  FSFile log_fd_real;

  FSFile f;
  FSDirectoryEntryInfo direntry;
  BOOL ret_value = TRUE;
  char path_full[FILE_PATH_LEN];

  log_fd = &log_fd_real;
  if( FALSE == Log_File_Open( log_fd, log_file_name ) ) {
    log_fd = NULL;
  }
  miya_log_fprintf(log_fd, "%s START\n", __FUNCTION__);

  /* �\�[�X�f�B���N�g���I�[�v�� */
  FS_InitFile(&f);

  if(FS_OpenDirectory(&f, path, FS_PERMIT_R | FS_PERMIT_W ) == FALSE ) {
    miya_log_fprintf(log_fd, "%s %d: Failed Open Directory\n", __FUNCTION__ , __LINE__ );
    miya_log_fprintf(log_fd, " %s\n", path);
    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path) ) );
    ret_value = FALSE;
    (void)Error_Report_Printf(" Open directory failed:%s\n",path);
    goto end_process;
  }


  while( FS_ReadDirectory(&f, &direntry) ) {
    if( STD_StrCmp(direntry.longname, ".") == 0 ) {
    }
    else if( STD_StrCmp(direntry.longname, "..") == 0 ) {
    }
    else if( STD_StrCmp(direntry.longname, "wlan_cfg.txt") == 0 ) {
    }
    else if( STD_StrCmp(direntry.longname, "nup_log.txt") == 0 ) {
    }
    else if( STD_StrCmp(direntry.longname, "sdtads") == 0 ) {
    }
    else if( STD_StrCmp(direntry.longname, "sdtaddevs") == 0 ) {
    }
    else if( direntry.attributes & FS_ATTRIBUTE_DOS_VOLUME ) {
    }
    else {

      STD_StrCpy( path_full , path );
      STD_StrCat( path_full , direntry.longname );

      if( (direntry.attributes & FS_ATTRIBUTE_IS_DIRECTORY) != 0 ) {
	/* �f�B���N�g���̏ꍇ */
#if 0
	OS_TPrintf("FS_DeleteDirectoryAuto ");

	if( FALSE == FS_DeleteDirectoryAuto( path_full ) ) {
	  OS_TPrintf("FS_DeleteDirectoryAuto failed.: ");
	  PrintAttributes(direntry.attributes, log_fd);
	  OS_TPrintf(" %s\n", path_full);
	  mprintf("FS_DeleteDirectoryAuto failed. %s\n", path_full);
	  ret_value = FALSE;
	}
	else {
	  OS_TPrintf("done. \n");
	}
#else
	ret_value = myCleanDirectory(path_full, log_fd, 1 /* 0? */ );
	if( FALSE == FS_DeleteDirectory( path_full ) ) {
	  OS_TPrintf("FS_DeleteDirectory failed.: ");
	  PrintAttributes(direntry.attributes, log_fd);
	  OS_TPrintf(" %s\n", path_full);
	  OS_TPrintf(" %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path_full) ) );
	  mprintf("FS_DeleteDirectory failed. %s\n", path_full);
	  ret_value = FALSE;
	}
	else {
	  // OS_TPrintf("done. \n");
	}


#endif

      }
      else {
	/* �t�@�C���̏ꍇ */
	OS_TPrintf("FS_DeleteFile \n");

	if( FALSE == FS_DeleteFile( path_full ) ) {
	  OS_TPrintf("FS_DeleteFile failed.: ");
	  PrintAttributes(direntry.attributes, log_fd);
	  OS_TPrintf(" %s\n", path_full);
	  mprintf("FS_DeleteFile failed. %s\n", path_full);
	  ret_value = FALSE;
	}
	else {
	  OS_TPrintf("done. \n");
	}
      }
    }
  }

  OS_TPrintf("\n");

 end_process:

  /* �\�[�X�f�B���N�g���N���[�Y */
  if( FS_CloseDirectory(&f) == FALSE) {
    miya_log_fprintf(log_fd, "%s %d: Failed Close Directory\n", __FUNCTION__ , __LINE__ );
    miya_log_fprintf(log_fd, " %s\n", path);
    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path)));
    //    ret_value |= 1; /* ����Ȃ������H���Ƃōl���� */
  }



  miya_log_fprintf(log_fd, "%s END\n\n", __FUNCTION__);
  Log_File_Close(log_fd);

  return ret_value;

}


int get_title_id(MY_DIR_ENTRY_LIST **headp, const char *path_src, int *save_parent_dir_info_flag, char *log_file_name, int level )
{
  static FSFile *log_fd;
  static FSFile log_fd_real;
  static BOOL log_active = FALSE;

  FSFile f_src;
  FSDirectoryEntryInfo entry_src;
  FSDirectoryEntryInfo entry_current_dir;
  int ret_value = 0;
  int save_parent_dir_info_flag_temp = 0;

  char *path_src_dir = NULL;
  char *path_src_full = NULL;


  if( level == 0 ) {
    log_fd = &log_fd_real;
    log_active = Log_File_Open( log_fd, log_file_name );
    miya_log_fprintf(log_fd, "%s START\n", __FUNCTION__);
  }

  level++;

  // level         1      2        3       4
  //             nand:/title 
  //             nand:/title/00000000/00000000/data/*.sav
  if( level >  4 ) {
    ret_value = 0;
    goto end_process;
  }

  /* �\�[�X�f�B���N�g���I�[�v�� */
  FS_InitFile(&f_src);

  if(FS_OpenDirectory(&f_src, path_src, FS_PERMIT_R) == FALSE ) {
    miya_log_fprintf(log_fd, "%s %d: Failed Open Directory\n", __FUNCTION__ , __LINE__ );
    miya_log_fprintf(log_fd, " %s\n", path_src);
    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path_src) ) );

    ret_value |= 1;
    AppErrorReport(path_src, "Open directory failed.");
    goto end_process;
  }

  /* �t�@�C�����o�b�t�@������ */
  path_src_dir = (char *)OS_Alloc( FILE_PATH_LEN );
  if( path_src_dir == NULL ) {
    miya_log_fprintf(log_fd, "Error: alloc error src_dir\n");
    ret_value |= 1;
    AppErrorReport(path_src, "Open directory failed.");
    goto end_process;
  }

  path_src_full = (char *)OS_Alloc( FILE_PATH_LEN );
  if( path_src_full == NULL ) {
    miya_log_fprintf(log_fd, "Error: alloc error src_full\n");
    ret_value |= 1;
    AppErrorReport(path_src, "Open directory failed.");
    goto end_process;
  }

  STD_MemSet((void *)path_src_dir, 0, FILE_PATH_LEN);
  STD_MemSet((void *)path_src_full, 0, FILE_PATH_LEN);
  STD_StrCpy(path_src_dir, path_src);
  STD_StrCat(path_src_dir, "/");

  while( FS_ReadDirectory(&f_src, &entry_src) ) {
    if( STD_StrCmp(entry_src.longname, ".") == 0 ) {
      /* �Ƃ肠�����J�����g�f�B���N�g���G���g�����c���Ă��� */
      STD_CopyMemory( (void *)&entry_current_dir, (void *)&entry_src ,sizeof(FSDirectoryEntryInfo) );
    }
    else if( STD_StrCmp(entry_src.longname, "..") == 0 ) {
    }
    else {
      STD_StrCpy( path_src_full , path_src_dir );
      STD_StrCat( path_src_full , entry_src.longname );

      if( (entry_src.attributes & FS_ATTRIBUTE_IS_DIRECTORY) != 0 ) {
	/* �f�B���N�g���̏ꍇ */
	// level        1      2        3       4
	//             nand:/title 
	//             nand:/title/00000000/00000000/data/*.sav
	if( level == 1 ) {
	  if( my_fs_is_Title_Hi_dir_name( entry_src.longname ) == TRUE ) {
	    ret_value |= get_title_id( headp, path_src_full , &save_parent_dir_info_flag_temp, log_file_name, level );
	  }
	}
	if( level == 2 ) {
	  if( my_fs_is_Title_Lo_dir_name( entry_src.longname ) == TRUE ) {
	    ret_value |= get_title_id( headp, path_src_full , &save_parent_dir_info_flag_temp, log_file_name, level );
	  }
	}
	else if( (level == 3) ) {
	  if( !STD_StrCmp( entry_src.longname, "content" ) ) {
	    ret_value |= get_title_id( headp, path_src_full , &save_parent_dir_info_flag_temp, log_file_name, level );
	  }
	}
      }	
      else {
	// level        1      2        3       4
	//             nand:/title 
	//             nand:/title/00000000/00000000/content/title.tmd
	if( (level == 4) ) {
	  if( !STD_StrCmp( entry_src.longname, "title.tmd" ) ) {
	    /* �ړI�̃t�@�C�����������B */
	    save_parent_dir_info_flag_temp = 1;
	  }
	}
      }
    }
  }

  if( save_parent_dir_info_flag_temp == 1 ) {
    if( level == 3 ) {
      if( FALSE == my_fs_add_list( headp, &entry_current_dir, path_src, NULL, log_fd) ) {
	ret_value |= 1;
	AppErrorReport(path_src, "Save directory failed.");
      }
    }
    else {
      /* content�f�B���N�g���Ȃ� */
    }
    *save_parent_dir_info_flag = 1;
  }

  /* �\�[�X�f�B���N�g���N���[�Y */
  if( FS_CloseDirectory(&f_src) == FALSE) {
    miya_log_fprintf(log_fd, "%s %d: Failed Close Directory\n", __FUNCTION__ , __LINE__ );
    miya_log_fprintf(log_fd, " %s\n", path_src);
    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path_src)));
    //    ret_value |= 1; /* ����Ȃ������H���Ƃōl���� */
  }

 end_process:
  if( path_src_dir != NULL ) {
    OS_Free(path_src_dir);
  }
  if( path_src_full != NULL ) {
    OS_Free(path_src_full);
  }

  level--;

  if( level == 0 ) {
    miya_log_fprintf(log_fd, "%s END\n\n", __FUNCTION__);
    Log_File_Close(log_fd);
  }

  return ret_value;
}



int find_title_save_data(MY_DIR_ENTRY_LIST **headp, const char *path_dst, const char *path_src, int *save_parent_dir_info_flag, char *log_file_name,  int level )
{
  //  static FSFile log_fd;
  static FSFile *log_fd;
  static FSFile log_fd_real;
  static BOOL log_active = FALSE;

  FSFile f_src;
  FSDirectoryEntryInfo entry_src;
  FSDirectoryEntryInfo entry_current_dir;
  int ret_value = 0;
  //  FSResult fs_ret_value;
  int save_parent_dir_info_flag_temp = 0;

  char *path_src_dir = NULL;
  char *path_src_full = NULL;
  char *path_dst_dir = NULL;
  char *path_dst_full = NULL;

  /* ������SD�J�[�h�����邩�ǂ������ׂ� */

  if( level == 0 ) {
    log_fd = &log_fd_real;
    log_active = Log_File_Open( log_fd, log_file_name );
    miya_log_fprintf(log_fd, "%s START\n", __FUNCTION__);
  }

  level++;

  // level         1      2        3       4
  //                 nand:/title 
  //                 nand:/title/00000000/00000000/data/*.sav
  if( level >  4 ) {
    ret_value = 0;
    goto end_process;
  }

  /* �\�[�X�f�B���N�g���I�[�v�� */
  FS_InitFile(&f_src);

  if(FS_OpenDirectory(&f_src, path_src, FS_PERMIT_R) == FALSE ) {
    miya_log_fprintf(log_fd, "%s %d: Failed Open Directory\n", __FUNCTION__ , __LINE__ );
    miya_log_fprintf(log_fd, " %s\n", path_src);
    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path_src) ) );
    AppErrorReport(path_src, "Open directory failed.");
    ret_value |= 1;
    goto end_process;
  }

  /* �t�@�C�����o�b�t�@������ */
  if( FALSE == Path_Buffers_Init(path_src, path_dst, &path_src_dir, 
				 &path_src_full, &path_dst_dir, &path_dst_full, log_fd ) ) {
    AppErrorReport(path_src, "Open directory failed.");
    ret_value |= 1;
    goto end_process;
  }

  while( FS_ReadDirectory(&f_src, &entry_src) ) {

    if( STD_StrCmp(entry_src.longname, ".") == 0 ) {
      /* �Ƃ肠�����J�����g�f�B���N�g���G���g�����c���Ă��� */
      STD_CopyMemory( (void *)&entry_current_dir, (void *)&entry_src ,sizeof(FSDirectoryEntryInfo) );
    }
    else if( STD_StrCmp(entry_src.longname, "..") == 0 ) {
    }
    else {
      STD_StrCpy( path_src_full , path_src_dir );
      STD_StrCat( path_src_full , entry_src.longname );
      STD_StrCpy( path_dst_full , path_dst_dir );
      STD_StrCat( path_dst_full , entry_src.longname );

      if( (entry_src.attributes & FS_ATTRIBUTE_IS_DIRECTORY) != 0 ) {
	/* �f�B���N�g���̏ꍇ */
	// level         1      2        3       4
	//                 nand:/title 
	//                 nand:/title/00000000/00000000/data/*.sav
	if( level == 1 ) {
	  if( my_fs_is_Title_Hi_dir_name( entry_src.longname ) == TRUE ) {
	    ret_value |= find_title_save_data( headp, path_dst_full, path_src_full , 
					       &save_parent_dir_info_flag_temp, log_file_name , level );
	  }
	}
	if( level == 2 ) {
	  if( my_fs_is_Title_Lo_dir_name( entry_src.longname ) == TRUE ) {
	    ret_value |= find_title_save_data( headp, path_dst_full, path_src_full , 
					       &save_parent_dir_info_flag_temp, log_file_name, level );
	  }
	}
	else if( (level == 3) ) {
	  if( !STD_StrCmp( entry_src.longname, "data" ) ) {
	    ret_value |= find_title_save_data( headp, path_dst_full, path_src_full , 
					       &save_parent_dir_info_flag_temp, log_file_name, level );
	  }
	}
      }	
      else {
	// level         1      2        3       4
	//                 nand:/title 
	//                 nand:/title/00000000/00000000/data/*.sav
	if( (level == 4) ) {
	  if( !STD_StrCmp( entry_src.longname, "public.sav" ) 
	      || !STD_StrCmp( entry_src.longname, "private.sav" )
	      || !STD_StrCmp( entry_src.longname, "banner.sav" ) ) {
	    /* �ړI�̃t�@�C�����������B */
	    if( FALSE == my_fs_add_list(headp, &entry_src, path_src_full, path_dst_full, log_fd) ) {
	      ret_value |= 1;
	      AppErrorReport(path_src_full, "Save file failed.");
	    }
	    save_parent_dir_info_flag_temp = 1;
	  }
	}
      }
    }
  }

  if( save_parent_dir_info_flag_temp == 1 ) {
    // OS_TPrintf("save dir info = %s\n\n",path_src);
    if( FALSE ==  my_fs_add_list( headp, &entry_current_dir, path_src, path_dst, log_fd) ) {
      ret_value |= 1;
      AppErrorReport(path_src, "Save Directory failed.");
    }
    *save_parent_dir_info_flag = 1;
  }

  /* �\�[�X�f�B���N�g���N���[�Y */
  if( FS_CloseDirectory(&f_src) == FALSE) {
    miya_log_fprintf(log_fd, "%s %d: Failed Close Directory\n", __FUNCTION__ , __LINE__ );
    miya_log_fprintf(log_fd, " %s\n", path_src);
    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path_src)));
    // ret_value |= 1; /* ����Ȃ������H���Ƃōl���� */
  }

 end_process:

  Path_Buffers_Clean( path_src_dir, path_src_full, path_dst_dir, path_dst_full );

  level--;

  if( level == 0 ) {
    miya_log_fprintf(log_fd, "%s END\n\n", __FUNCTION__);
    Log_File_Close(log_fd);
  }

  return ret_value;
}

int copy_r( MY_DIR_ENTRY_LIST **headp, const char *path_dst, const char *path_src, char *log_file_name, int level )
{
  //  static FSFile log_fd;
  static BOOL log_active = FALSE;
  static FSFile *log_fd;
  static FSFile log_fd_real;

  FSFile f_src;
  FSDirectoryEntryInfo entry_src;
  FSDirectoryEntryInfo entry_current_dir;
  int ret_value = 0;

  char *path_src_dir = NULL;
  char *path_src_full = NULL;
  char *path_dst_dir = NULL;
  char *path_dst_full = NULL;

  /* ������SD�J�[�h�����邩�ǂ������ׂ� */
  if( level == 0 ) {
    log_fd = &log_fd_real;
    log_active = Log_File_Open( log_fd, log_file_name );
    miya_log_fprintf(log_fd, "%s START\n", __FUNCTION__);
  }

  level++;

  /* �\�[�X�f�B���N�g���I�[�v�� */
  FS_InitFile(&f_src);

  if( FS_OpenDirectory(&f_src, path_src, FS_PERMIT_R) == FALSE ) {
    if( log_active ) {
      miya_log_fprintf(log_fd, "%s %d: Failed Open Directory\n", __FUNCTION__ , __LINE__ );
      miya_log_fprintf(log_fd, " %s\n", path_src);
      miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path_src) ) );
    }

    ret_value |= 1;
    AppErrorReport(path_src, "Open directory failed.");

    goto end_process;
  }

  /* �t�@�C�����o�b�t�@������ */
  if( FALSE == Path_Buffers_Init(path_src, path_dst, 
				 &path_src_dir, &path_src_full, &path_dst_dir, &path_dst_full, log_fd ) )
    {
      ret_value |= 1;
      AppErrorReport(path_src, "Open directory failed.");

      goto end_process;
    }

  while( FS_ReadDirectory(&f_src, &entry_src) ) {

    if( STD_StrCmp(entry_src.longname, ".") == 0 ) {
      /* �Ƃ肠�����J�����g�f�B���N�g���G���g�����c���Ă��� */
      STD_CopyMemory( (void *)&entry_current_dir, (void *)&entry_src ,sizeof(FSDirectoryEntryInfo) );
    }
    else if( STD_StrCmp(entry_src.longname, "..") == 0 ) {

    }
    else {
      STD_StrCpy( path_src_full , path_src_dir );
      STD_StrCat( path_src_full , entry_src.longname );
      STD_StrCpy( path_dst_full , path_dst_dir );
      STD_StrCat( path_dst_full , entry_src.longname );

      if( (entry_src.attributes & FS_ATTRIBUTE_IS_DIRECTORY) != 0 ) {
	ret_value |= copy_r( headp, path_dst_full, path_src_full, log_file_name, level );
      }	
      else {
	if( FALSE == my_fs_add_list(headp, &entry_src, path_src_full, path_dst_full, log_fd) ) {
	  ret_value |= 1;
	  AppErrorReport(path_src_full, "Save file failed.");
	}
      }
    }
  }

  /* �f�B���N�g���G���g�����Z�[�u */

  if( FALSE == my_fs_add_list( headp, &entry_current_dir, path_src, path_dst, log_fd ) ) {
    AppErrorReport(path_src, "Save directory failed.");
    ret_value |= 1;
  }

  /* �f�B���N�g���N���[�Y */
  if( FS_CloseDirectory(&f_src) == FALSE) {
    miya_log_fprintf(log_fd, "%s %d: Failed Close Directory\n", __FUNCTION__ , __LINE__ );
    miya_log_fprintf(log_fd, " %s\n", path_src);
    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path_src)));
    //    ret_value |= 1; /* ����Ȃ������H���Ƃōl���� */
  }

 end_process:

  Path_Buffers_Clean( path_src_dir, path_src_full, path_dst_dir, path_dst_full );

  level--;
  if( level == 0 ) {
    miya_log_fprintf(log_fd, "%s END\n\n", __FUNCTION__);
    Log_File_Close(log_fd);
  }
  return ret_value;
}


void write_debug_data(void)
{
  // CopyFile( dst <= src );
  CopyFile("sdmc:/m00.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m01.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m02.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m03.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m04.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m05.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m06.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m07.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m08.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m09.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m10.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m11.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m12.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m13.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m14.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m15.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m16.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m17.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m18.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m19.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m20.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m22.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m23.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m24.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m25.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m26.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m27.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m28.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m29.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m30.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m31.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m32.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m33.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("sdmc:/m34.sav" , "nand:/sys/log/sysmenu.log", NULL);

  /*
    PrintDirEntryListBackword-----Start
    src name = nand:/title
    dst name = sdmc:/miya_find_title_save
    src name = nand:/title/0003000f
    dst name = sdmc:/miya_find_title_save/0003000f
    src name = nand:/title/0003000f/484e4341
    dst name = sdmc:/miya_find_title_save/0003000f/484e4341
    src name = nand:/title/0003000f/484e4341/data
    dst name = sdmc:/miya_find_title_save/0003000f/484e4341/data
--    src name = nand:/title/0003000f/484e4341/data/private.sav
    dst name = sdmc:/miya_find_title_save/0003000f/484e4341/data/private.sav
    src name = nand:/title/00030017
    dst name = sdmc:/miya_find_title_save/00030017
    src name = nand:/title/00030017/484e4141
    dst name = sdmc:/miya_find_title_save/00030017/484e4141
    src name = nand:/title/00030017/484e4141/data
    dst name = sdmc:/miya_find_title_save/00030017/484e4141/data
--    src name = nand:/title/00030017/484e4141/data/public.sav
    dst name = sdmc:/miya_find_title_save/00030017/484e4141/data/public.sav
    PrintDirEntryListBackward-----End
  */

#if 0

nand:/title/0003000f
nand:/title/0003000f/484e4341
nand:/title/0003000f/484e4341/content
nand:/title/0003000f/484e4841
nand:/title/0003000f/484e4841/data
nand:/title/0003000f/484e4841/content
nand:/title/0003000f/484e4c41
nand:/title/0003000f/484e4c41/data

nand:/title/00030015
nand:/title/00030015/484e4241
nand:/title/00030015/484e4241/data
nand:/title/00030015/484e4241/content
  CopyFile("nand:/title/00030017/
  CopyFile("nand:/ticket/public.sav" , "nand:/sys/log/sysmenu.log");
  CopyFile("nand:/ticket/00030015
  CopyFile("nand:/ticket/00030015/miya.sav" , "nand:/sys/log/sysmenu.log");
#endif
}

