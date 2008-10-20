#include <twl.h>
#include        "text.h"
#include        "mprintf.h"
#include        "my_fs_util.h"
#include        "logprintf.h"

/*
  NAND -> SDコピーの時、アトリビュートと時間とパーミッションを合わせる必要あり？


// ファイル名の最大長 (旧仕様のプロシージャに限定)
#define	FS_FILE_NAME_MAX	        127

// エントリ名の最大長 (旧仕様のプロシージャに限定)
#define	FS_ENTRY_SHORTNAME_MAX      16
#define	FS_ENTRY_LONGNAME_MAX	    260

// ReadDirectoryで使用するエントリ属性
#define FS_ATTRIBUTE_IS_DIRECTORY   0x00000100UL
#define FS_ATTRIBUTE_IS_PROTECTED   0x00000200UL
// MS-DOS FATベースのアーカイブに限り意味のある属性
#define FS_ATTRIBUTE_DOS_MASK       0x000000FFUL
#define FS_ATTRIBUTE_DOS_READONLY   0x00000001UL
#define FS_ATTRIBUTE_DOS_HIDDEN     0x00000002UL
#define FS_ATTRIBUTE_DOS_SYSTEM     0x00000004UL
#define FS_ATTRIBUTE_DOS_VOLUME     0x00000008UL
#define FS_ATTRIBUTE_DOS_DIRECTORY  0x00000010UL
#define FS_ATTRIBUTE_DOS_ARCHIVE    0x00000020UL


// OpenFileで使用するアクセスモード
#define FS_FILEMODE_R               0x00000001UL
#define FS_FILEMODE_W               0x00000002UL
#define FS_FILEMODE_L               0x00000004UL
#define FS_FILEMODE_RW              (FS_FILEMODE_R | FS_FILEMODE_W)
#define FS_FILEMODE_RWL             (FS_FILEMODE_R | FS_FILEMODE_W | FS_FILEMODE_L)

// OpenDirectoryで使用するアクセスモード
#define FS_DIRMODE_SHORTNAME_ONLY   0x00001000UL

// CreateFileで使用する権限フラグ
#define FS_PERMIT_R                 0x00000001UL
#define FS_PERMIT_W                 0x00000002UL
#define FS_PERMIT_RW                (FS_PERMIT_R | FS_PERMIT_W)

// ReadDirectoryで使用するエントリ情報
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

// GetPathInfo,SetPathInfoで使用するエントリ情報
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


static const u32 BUF_SIZE = 256;

typedef struct {
  FSResult result;
  char string[32];
} FS_RESUTL_WORD;

/* c:/twlsdk/include/nitro/fs/types.h */

static FS_RESUTL_WORD my_word[] = {
  { FS_RESULT_SUCCESS, "FS_RESULT_SUCCESS" },
  { FS_RESULT_FAILURE, "FS_RESULT_FAILURE" },
  { FS_RESULT_BUSY, "FS_RESULT_BUSY" },
  { FS_RESULT_CANCELED, "FS_RESULT_CANCELED" },
  { FS_RESULT_CANCELLED, "FS_RESULT_CANCELLED" },
  { FS_RESULT_UNSUPPORTED, "FS_RESULT_UNSUPPORTED" },
  { FS_RESULT_ERROR, "FS_RESULT_ERROR" },
  { FS_RESULT_INVALID_PARAMETER, "FS_RESULT_INVALID_PARAMETER" },
  { FS_RESULT_NO_MORE_RESOUCE, "FS_RESULT_NO_MORE_RESOUCE" },
  { FS_RESULT_ALREADY_DONE, "FS_RESULT_ALREADY_DONE" },
  { FS_RESULT_PERMISSION_DENIED, "FS_RESULT_PERMISSION_DENIED" },
  { FS_RESULT_MEDIA_FATAL, "FS_RESULT_MEDIA_FATAL" },
  { FS_RESULT_NO_ENTRY, "FS_RESULT_NO_ENTRY" },
  { FS_RESULT_MEDIA_NOTHING, "FS_RESULT_MEDIA_NOTHING" },
  { FS_RESULT_MEDIA_UNKNOWN, "FS_RESULT_MEDIA_UNKNOWN" },
  { FS_RESULT_BAD_FORMAT, "FS_RESULT_BAD_FORMAT" },
  { FS_RESULT_MAX, "FS_RESULT_MAX" },
    // プロシージャ内で使用する一時的な結果値
  { FS_RESULT_PROC_ASYNC, "FS_RESULT_PROC_ASYNC" },
  { FS_RESULT_PROC_DEFAULT, "FS_RESULT_PROC_DEFAULT" },
  { FS_RESULT_PROC_UNKNOWN, "FS_RESULT_PROC_UNKNOW" },
};


char *my_fs_util_get_fs_result_word( FSResult res )
{
  int i;
  for( i = 0 ; i < 19 ; i++ ) {
    if( my_word[i].result  == res ) {
      return my_word[i].string;
    }
  }
  return my_word[0].string;
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


static BOOL Log_File_Open(FSFile *log_fd, const char *log_file_name)
{
  BOOL bSuccess;
  FSResult res;

  if( log_file_name == NULL ) {
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

static void Log_File_Close(FSFile *log_fd)
{
  FS_FlushFile(log_fd);
  FS_CloseFile(log_fd);
}

/*---------------------------------------------------------------------------*
  Name:         LoadFile

  Description:  内部でメモリを確保しファイルを読み込みます。

  Arguments:    path:   読み込むファイルのパス。

  Returns:      ファイルが存在するならファイルの内容が読み込まれた
                内部で確保したバッファへのポインタを返します。
                このポインタは FS_Free で解放する必要があります。
 *---------------------------------------------------------------------------*/
static BOOL LoadFile(const char* path, char **alloc_ptr, int *alloc_size, FSFile *log_fd)
{
    FSFile f;
    BOOL bSuccess;
    char* pBuffer;
    u32 fileSize;
    s32 readSize = 0;

    if( alloc_ptr == 0 || alloc_size == NULL ) {
      miya_log_fprintf(log_fd, "Failed LoadFile argument error\n");
      return FALSE;
    }

    FS_InitFile(&f);

    bSuccess = FS_OpenFileEx(&f, path, FS_FILEMODE_R);
    if( ! bSuccess ) {
      miya_log_fprintf(log_fd, "Failed Open File %s\n",__FUNCTION__);
      miya_log_fprintf(log_fd, " path=%s\n", path );
      miya_log_fprintf(log_fd, " res=%s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path) ));
      return FALSE;
    }

    fileSize = FS_GetFileLength(&f);
    pBuffer = (char*)OS_Alloc(fileSize + 1);
    if( pBuffer == NULL ) {
      miya_log_fprintf(log_fd, "Mem alloc error: %s\n", __FUNCTION__);
      return FALSE;
    }

    readSize = FS_ReadFile(&f, pBuffer, (s32)fileSize);
    if( readSize != fileSize ) {
      miya_log_fprintf(log_fd, "Failed Read File: %s\n",path);
    }
    
    *alloc_size = (int)readSize;

    bSuccess = FS_CloseFile(&f);
    if( ! bSuccess ) {
      miya_log_fprintf(log_fd, "Failed Close File\n");
      miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path)));
    }

    pBuffer[fileSize] = '\0';
    *alloc_ptr = pBuffer;
    return TRUE;
}

static BOOL SaveFile(const char* path, void* pData, u32 size, FSFile *log_fd)
{
  FSFile f;
  BOOL bSuccess;
  FSResult fsResult;
  s32 writtenSize;

  FS_InitFile(&f);

  bSuccess = FS_OpenFileEx(&f, path, FS_FILEMODE_W);
  if (bSuccess == FALSE) {
    FSResult res = FS_GetArchiveResultCode(path);
    if( res == FS_RESULT_NO_ENTRY ) {
      /* 本来ここで問題なし */
    }
    else {
      miya_log_fprintf(log_fd, "Failed open file:%s %d %s %s\n",
		       __FUNCTION__,__LINE__, path, my_fs_util_get_fs_result_word(res) );
    }
  }
  else {
    FS_CloseFile(&f);
    /* backup バックアップを取っておくべき？？ */
    FS_DeleteFile(path);
  }

  FS_CreateFile(path, (FS_PERMIT_R|FS_PERMIT_W));
  bSuccess = FS_OpenFileEx(&f, path, FS_FILEMODE_W);
  if (bSuccess == FALSE) {
    FSResult res = FS_GetArchiveResultCode(path);
    miya_log_fprintf(log_fd,"Failed open file:%s %d %s %s\n", 
		     __FUNCTION__,__LINE__, path, my_fs_util_get_fs_result_word(res) );
    return FALSE;
  }

  fsResult = FS_SetFileLength(&f, 0);
  if( fsResult != FS_RESULT_SUCCESS ) {
  }

  writtenSize = FS_WriteFile(&f, pData, (s32)size);
  if( writtenSize != size ) {
  }

  FS_FlushFile(&f);
  bSuccess = FS_CloseFile(&f);
  if( bSuccess ) {
      
  }
  return TRUE;
}

static BOOL CopyFile(const char *dst_path, const char *src_path, FSFile *log_fd )
{
  char *alloc_ptr = NULL;
  int alloc_size = 0;

  if( TRUE == LoadFile(src_path, &alloc_ptr, &alloc_size, log_fd) ) {
    if( TRUE == SaveFile(dst_path, alloc_ptr, (u32)alloc_size, log_fd) ) {
      if( alloc_ptr ) {
	OS_Free(alloc_ptr);
      }
      return TRUE;
    }
    if( alloc_ptr ) {
      OS_Free(alloc_ptr);
    }
    return FALSE;
  }
  return FALSE;
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
  /* トップディレクトリの方から順にリストされているので */
  /* リストの後ろ（下位ディレクトリの方）から戻さないといけない */
  /* バックワードで戻していく */

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


static BOOL add_entry_list( MY_DIR_ENTRY_LIST **headp, MY_DIR_ENTRY_LIST *entry_list )
{
  MY_DIR_ENTRY_LIST *list_temp;
  MY_DIR_ENTRY_LIST *list_prev_temp;

  if( entry_list == NULL ) {
    return FALSE;
  }
  if( headp == NULL ) {
    return FALSE;
  }

  if( *headp == NULL ) {
    *headp = (MY_DIR_ENTRY_LIST *)OS_Alloc( sizeof(MY_DIR_ENTRY_LIST) );
    STD_CopyMemory( (void *)(*headp), (void *)entry_list , sizeof(MY_DIR_ENTRY_LIST) );
    (*headp)->prev = NULL;
    (*headp)->next = NULL;
  }
  else {
    for( list_temp = *headp ; list_temp->next != NULL ; list_temp = list_temp->next ) {
      ;
    }
    list_temp->next = (MY_DIR_ENTRY_LIST *)OS_Alloc( sizeof(MY_DIR_ENTRY_LIST) );

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
  if( entry == NULL ) {
    return FALSE;
  }

  /* ファイルの場合 srcがNAND, dstがSD */
  if( !STD_StrCmp( src_path, "nand:" ) ) {
    /* nandのルートディレクトリはスルーする。 */
    return TRUE;
  }


  if( FALSE == FS_GetPathInfo(src_path,&path_info) ) {
    miya_log_fprintf(log_fd, "%s %d: Failed GetPathInfo\n", __FUNCTION__,__LINE__ );
    miya_log_fprintf(log_fd, " %s\n", src_path );
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
    /* なぜがWarningが出る */
OS_TPrintf("%s %d\n",__FUNCTION__,__LINE__);
    if( FALSE == CompareFsDateTime( &(entry->ctime), &(path_info.ctime)) )  {
      miya_log_fprintf(log_fd, "warning ctime\n");
      miya_log_fprintf(log_fd," entry     %d/%02d/%02d ", entry->ctime.year,entry->ctime.month,entry->ctime.day); 
      miya_log_fprintf(log_fd,"%02d:%02d:%02d\n", entry->ctime.hour,entry->ctime.minute,entry->ctime.second);
      miya_log_fprintf(log_fd," path_info %d/%02d/%02d ", path_info.ctime.year,path_info.ctime.month,path_info.ctime.day);
      miya_log_fprintf(log_fd,"%02d:%02d:%02d\n", path_info.ctime.hour,path_info.ctime.minute,path_info.ctime.second);
    }
OS_TPrintf("%s %d\n",__FUNCTION__,__LINE__);
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

  return TRUE;
}
 
BOOL ClearDirEntryList( MY_DIR_ENTRY_LIST **headp )
{
  MY_DIR_ENTRY_LIST *list_temp1 = *headp;
  MY_DIR_ENTRY_LIST *list_temp2;

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
                 | ここの最下位ビットが１のやつがシステムアプリ
                 | 
		 システムアプリはダウンロード対象外

 012345678901234567890123456789
 nand:/title/00030017/484e4141 は ランチャー

  */
  c = path[19];
  if( ('a' <= c) && (c <= 'f') ) {
    num = (int)( c - 'a' + 10 );
  }
  if( ('A' <= c) && (c <= 'F') ) {
    num = (int)( c - 'A' + 10 );
  }
  else if( ('0' <= c) && (c <= '9') ) {
    num = (int)( c - '0' );
  }
  else {
    num = 0;
  }
  
  if( num & 1 ) {
    /* System App. */
    return TRUE;
  }
  else {
    /* User App. */
    return FALSE;
  }
}


void GetDirEntryList( MY_DIR_ENTRY_LIST *head, u64 **pBuffer, int *size)
{
  int i;
  int count = 0;
  MY_DIR_ENTRY_LIST *list_temp;
  u64 *buf = NULL;
  char c;
  u8 hex;

  if( head == NULL ) {
    *pBuffer = buf;
    *size = count;
  }
  else {
    for( list_temp = head ; list_temp->next != NULL ; list_temp = list_temp->next ) {
      if( list_temp->src_path ) {
#ifdef MYDEBUG
	count++;
#else
	if( FALSE == CheckSystemApp( list_temp->src_path) ) {
	  count++;
	}
#endif
      }
    }

    OS_TPrintf("User App. count1 = %d\n", count);    

    if( count ) {
      buf = (u64 *)OS_Alloc( (u32)(count * sizeof(u64)) );
      STD_MemSet((void *)buf, 0, count * sizeof(u64));
    }
    else {

    }
    *pBuffer = buf;
    *size = count;


    count = 0;
    for(  ; list_temp != NULL ; list_temp = list_temp->prev ) {
      if( list_temp->src_path ) {
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
#ifndef MYDEBUG
	if( FALSE == CheckSystemApp( list_temp->src_path ) ) {
#endif
	  count++;

	  /* User App. */
	  for( i =  0 ; i < 8 ; i++ ) {
	    c = list_temp->src_path[12 + i];
	    hex = 0;
	    if( ('a' <= c) && (c <= 'f') ) {
	      hex = (u8)( c - 'a' + 10 );
	    }
	    else if( ('A' <= c) && (c <= 'F') ) {
	      hex = (u8)( c - 'A' + 10 );
	    }
	    else if( ('0' <= c) && (c <= '9') ) {
	      hex = (u8)(c - '0');
	    }
	    else {
	      /* error! */
	      count--;
	      return;
	    }
	    *buf |= (((u64)hex) << ((7-i)*4 + 32 ));
	  }

	  for( i =  0 ; i < 8 ; i++ ) {
	    c = list_temp->src_path[21 + i];
	    hex = 0;
	    if( ('a' <= c) && (c <= 'f') ) {
	      hex = (u8)( c - 'a' + 10 );
	    }
	    else if( ('A' <= c) && (c <= 'F') ) {
	      hex = (u8)( c - 'A' + 10 );
	    }
	    else if( ('0' <= c) && (c <= '9') ) {
	      hex = (u8)(c - '0');
	    }
	    else {
	      /* error! */
	      count--;
	      return;
	    }
	    *buf |= (((u64)hex) << ((7-i)*4 ));
	  }
	  buf++;
	  OS_TPrintf("User App. count2 = %d\n", count);

#ifndef MYDEBUG
	}
#endif
	/*
	  012345678901234567890123456789
	  nand:/title/00030017/484e4141 は ランチャー
	  nand:/title/00030015/484e4641 は shop
	  nand:/title/00030015/484e4241 は 本体設定
	*/
      }
    }
  }
}


void PrintSrcDirEntryListBackward( MY_DIR_ENTRY_LIST *head, FSFile *log_fd)
{
  MY_DIR_ENTRY_LIST *list_temp;
  //  MY_DIR_ENTRY_LIST *list_prev;
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
	/*
	  nand:/title/00030017/484e4141 は ランチャー
	  nand:/title/00030015/484e4641 は shop
	  nand:/title/00030015/484e4241 は 本体設定
	*/
      }
    }
  }
  miya_log_fprintf(log_fd, "PrintSrcDirEntryListBackward-----End\n");
}





int SaveDirEntryList( MY_DIR_ENTRY_LIST *head , char *path )
{
  FSFile f;
  FSFile f_src;
  FSFile f_dst;
  BOOL bSuccess;
  FSResult fsResult;
  s32 writtenSize;
  MY_DIR_ENTRY_LIST *list_temp;
  int list_count = 0;
  
  FSFile log_fd_real;
  FSFile *log_fd;
  BOOL log_active = FALSE;
  //  char *log_file_name = "sdmc:/miya/save_dir_entry_log.txt";
  char *log_file_name = NULL;

  log_fd = &log_fd_real;

  /* ここでSDカードがあるかどうか調べる */
  log_active = Log_File_Open( log_fd, log_file_name );
  if( !log_active ) {
    log_fd = NULL;
  }

  /* 最初にSD側のルートディレクトリのデータを消しとくべきか？
     せっかくファイルリストに記録してるのでもったいない→必要ない */
  FS_InitFile(&f);
  FS_InitFile(&f_src);
  FS_InitFile(&f_dst);
  if( path == NULL ) {
    miya_log_fprintf(log_fd, "%s %d not specify entry save file\n",__FUNCTION__,__LINE__ );
    return -1;
  }
  FS_CreateFileAuto(path, (FS_PERMIT_R|FS_PERMIT_W));
  bSuccess = FS_OpenFileEx(&f, path, FS_FILEMODE_W);
  if (bSuccess == FALSE) {
    fsResult = FS_GetArchiveResultCode(path);
    miya_log_fprintf(log_fd, "Failed create file - dir entry list file:%d\n", fsResult );
    return -1;
  }
  fsResult = FS_SetFileLength(&f, 0);
  if( fsResult != FS_RESULT_SUCCESS ) {
    miya_log_fprintf(log_fd, "Failed set file len - dir entry list file:%d\n", fsResult );
  }

  /* バックワードでファイルに保存 */
  if( head == NULL ) {
  }
  else {
    for( list_temp = head ; list_temp->next != NULL ; list_temp = list_temp->next ) {
      ;
      }
    for(  ; list_temp != NULL ; list_temp = list_temp->prev ) {
      // OS_TPrintf( "name = %s\n", list_temp->src_path );
      /* SDにログを残す場合 */
      if( log_active ) {
	miya_log_fprintf(log_fd, "%s\n", list_temp->src_path);
      }

      writtenSize = FS_WriteFile(&f, (void *)list_temp, (s32)sizeof(MY_DIR_ENTRY_LIST) );
      if( writtenSize != sizeof(MY_DIR_ENTRY_LIST) ) {
	miya_log_fprintf(log_fd, "%s %d: Failed write file\n", __FUNCTION__ , __LINE__ );
	miya_log_fprintf(log_fd, " %s\n", path);
	miya_log_fprintf(log_fd, " entry count %d\n", list_count );
      }
      
      /* SD側にディレクトリの作成とファイルのコピー */
      if( (list_temp->content.attributes & FS_ATTRIBUTE_IS_DIRECTORY) != 0 ) {
	/* ディレクトリの場合 */
	bSuccess = FS_CreateDirectoryAuto(list_temp->dst_path, FS_PERMIT_RW);
	// bSuccess = FS_CreateDirectory(list_temp->dst_path, FS_PERMIT_RW);
	if(!bSuccess) {
	  fsResult = FS_GetArchiveResultCode(list_temp->dst_path);
	  if( fsResult != FS_RESULT_ALREADY_DONE ) {
	    miya_log_fprintf(log_fd, "%s %d: Failed Create DST Directory\n", __FUNCTION__ , __LINE__ );
	    miya_log_fprintf(log_fd, " %s\n", list_temp->dst_path);
	    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( fsResult ) );
	    return -1;
	  }
	}
      }
      else {
	/* ファイルの場合 srcがNAND, dstがSD */
	if( !STD_StrCmp( list_temp->src_path, "nand:" ) ) {
	  /* nandのルートディレクトリはスルーする。 */
	}
	else {
	  /* NANDからSDにコピーする */
	  // CopyFile( dst <= src );
	  CopyFile(list_temp->dst_path, list_temp->src_path, log_fd );
	}
      }
      list_count++;
    }
  }
  
  FS_FlushFile(&f);

  if( FS_CloseFile(&f) == FALSE) {
    miya_log_fprintf(log_fd, "%s %d: Failed Close file\n", __FUNCTION__ , __LINE__ );
    miya_log_fprintf(log_fd, " %s\n", path);
    miya_log_fprintf(log_fd, " %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path) ) );
  }
  if( log_active ) {
    miya_log_fprintf(log_fd, "write entry list count %d\n", list_count);
  }

  if( log_active ) {
    Log_File_Close(log_fd);
  }
  return list_count;
}

/********************************************
 * NANDにディレクトリエントリとファイルを復活させる。
*********************************************/
BOOL RestoreDirEntryList( char *path )
{
  FSFile f;
  FSFile f_dir;
  BOOL bSuccess;
  FSResult fsResult;
  s32 readSize;
  MY_DIR_ENTRY_LIST list_temp;
  FSPathInfo path_info;
  int list_count = 0;
  MY_DIR_ENTRY_LIST *readonly_list_head = NULL;

  FSFile log_fd;
  BOOL log_active = FALSE;
  char *log_file_name = "sdmc:/miya/restore_dir_entry_log.txt";

  /* ここでSDカードがあるかどうか調べる */

  log_active = Log_File_Open( &log_fd, log_file_name );

  FS_InitFile(&f);
  FS_InitFile(&f_dir);

  if( FS_OpenFileEx(&f, path, FS_FILEMODE_R) == FALSE) {
    fsResult = FS_GetArchiveResultCode(path);
    miya_log_fprintf(&log_fd, "%s %d: Failed Open file\n", __FUNCTION__ , __LINE__ );
    miya_log_fprintf(&log_fd, " %s\n", path);
    miya_log_fprintf(&log_fd, " %s\n", my_fs_util_get_fs_result_word( fsResult ) );
    return FALSE;
  }

  while( 1 ) {
    /* リストはルートディレクトリに近い順から入っている */
    readSize = FS_ReadFile(&f, (void *)&list_temp, (s32)sizeof(MY_DIR_ENTRY_LIST) );
    if( readSize == 0 ) {
      miya_log_fprintf(&log_fd, "Read entry count %d\n", list_count );
      break;
    }
    else if( readSize != (s32)sizeof(MY_DIR_ENTRY_LIST) ) {
      miya_log_fprintf(&log_fd, "%s %d: Failed Read file\n", __FUNCTION__ , __LINE__ );
      miya_log_fprintf(&log_fd, " %s\n", path);
      miya_log_fprintf(&log_fd, " read entry count %d\n", list_count );
      break;
    }

    list_count++;

    /* NAND側にディレクトリの作成とファイルのコピー */
    if( (list_temp.content.attributes & FS_ATTRIBUTE_IS_DIRECTORY) != 0 ) {
      /* ディレクトリの場合 */
      if( TRUE == FS_GetPathInfo(list_temp.src_path, &path_info) ) {
	/* 復元される側にすでにディレクトリエントリがある場合 */
	if( (path_info.attributes & FS_ATTRIBUTE_IS_DIRECTORY) == 0 ) {
	  /* ディレクトリでない場合 エラー */
	  /* SDにログを残す場合 */
	  if( log_active ) {
	    miya_log_fprintf(&log_fd, "%s %d: NOT a directory\n", __FUNCTION__ , __LINE__ );
	    miya_log_fprintf(&log_fd, " %s\n", list_temp.src_path );
	  }
	  /* require backup file */
	  /* パニック?? */
	  /* それとも一度デリートする?? */
	}
      }
      else {
	/* 復元される側にディレクトリエントリがない場合 */
	bSuccess = FS_CreateDirectory(list_temp.src_path, FS_PERMIT_RW);
	if(!bSuccess) {
	  fsResult = FS_GetArchiveResultCode(list_temp.src_path);
	  if( fsResult != FS_RESULT_ALREADY_DONE ) {
	    if( log_active ) {
	      miya_log_fprintf(&log_fd, "%s %d: Failed Create NAND Directory\n", __FUNCTION__,__LINE__);
	      miya_log_fprintf(&log_fd, " %s\n", my_fs_util_get_fs_result_word( fsResult ) );
	      miya_log_fprintf(&log_fd, " %s\n", list_temp.src_path);
	    }
	  }
	}
	if( FALSE == FS_GetPathInfo(list_temp.src_path, &path_info) ) {
	  if( log_active ) {
	    miya_log_fprintf(&log_fd, "%s %d: Failed GetPathInfo\n", __FUNCTION__,__LINE__ );
	    miya_log_fprintf(&log_fd, " %s\n", list_temp.src_path );
	  }
	  //  return FALSE;
	}
      }

      /* このディレクトリを記憶しておき、あとで属性をまとめて戻す */
      if( FALSE == add_entry_list( &readonly_list_head, &list_temp ) ) {
	miya_log_fprintf(&log_fd, "%s %d: ERROR: add_entry_list\n", __FUNCTION__,__LINE__ );
	miya_log_fprintf(&log_fd, " %s\n", list_temp.src_path );
      }

      if( (path_info.attributes & FS_ATTRIBUTE_DOS_READONLY) != 0 ) {
	/* リードオンリーの場合,一度リードライト可能にする */
	path_info.attributes &= ~FS_ATTRIBUTE_DOS_READONLY;
	if( FALSE == FS_SetPathInfo( list_temp.src_path, &path_info) ) {
	  fsResult = FS_GetArchiveResultCode(list_temp.src_path);
	  miya_log_fprintf(&log_fd, "%s %d: Failed SetPathInfo\n", __FUNCTION__,__LINE__ );
	  miya_log_fprintf(&log_fd, " %s\n", list_temp.src_path );
	  miya_log_fprintf(&log_fd, " %s\n", my_fs_util_get_fs_result_word( fsResult ) );
	}
      }
    }
    else {
      /* ファイルの場合 srcがSD, dstがNAND */
      if( !STD_StrCmp( list_temp.src_path, "nand:" ) ) {
	/* nandのルートディレクトリはスルーする。 */
      }
      else {

	/* とりあえずデスティネーションファイルがあるかどうか確認して
	   あればSDにバックアップする */
	// CopyFile( dst <= src );
	CopyFile( list_temp.src_path, list_temp.dst_path, &log_fd );

	path_info.attributes  = list_temp.content.attributes;
	path_info.ctime  = list_temp.content.ctime;
	path_info.mtime = list_temp.content.mtime;
	path_info.atime = list_temp.content.atime;
	path_info.id = list_temp.content.id;
	path_info.filesize = list_temp.content.filesize;
	if( FALSE == FS_SetPathInfo( list_temp.src_path, &path_info) ) {
	  fsResult = FS_GetArchiveResultCode(list_temp.src_path);
	  miya_log_fprintf(&log_fd, "%s %d: Failed SetPathInfo\n", __FUNCTION__,__LINE__ );
	  miya_log_fprintf(&log_fd, " %s\n", list_temp.src_path );
	  miya_log_fprintf(&log_fd, " %s\n", my_fs_util_get_fs_result_word( fsResult ) );
	}
      }
    }
  }

  /* add_entry_list( &readonly_list_head, &list_temp );
     でリストにしたエントリーのアトリビュートを逆順で元に戻す。*/
  if( FALSE == restore_entry_list(&readonly_list_head, &log_fd) ) {
    miya_log_fprintf(&log_fd, "%s %d: ERROR: restore_entry_list\n", __FUNCTION__,__LINE__ );
  }

 exit_label:

  //  FS_FlushFile(&f); //リードだからいらない
  if( FS_CloseFile(&f) == FALSE) {
    fsResult = FS_GetArchiveResultCode(path);
    miya_log_fprintf(&log_fd, "%s %d: Failed Close file\n", __FUNCTION__ , __LINE__ );
    miya_log_fprintf(&log_fd, " %s\n", path);
    miya_log_fprintf(&log_fd, " %s\n", my_fs_util_get_fs_result_word( fsResult ) );
  }

  if( log_active ) {
    Log_File_Close(&log_fd);
  }

  return TRUE;
}

// = "nand:/title";

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


BOOL MydataSave(const char *path, void *pData, int size, FSFile *log_fd)
{
#pragma unused(log_fd)

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
  }

  FS_FlushFile(&f);
  bSuccess = FS_CloseFile(&f);
  if( bSuccess ) {
      
  }
  return TRUE;
}


BOOL TitleIDLoad(const char *path, u64 **pBuffer, int *count, FSFile *log_fd)
{
  FSFile f;
  BOOL bSuccess;
  //  u32 fileSize;
  s32 readSize = 0;
  int id_count= 0;
  int size;

  FS_InitFile(&f);
  
  bSuccess = FS_OpenFileEx(&f, path, FS_FILEMODE_R);
  if( ! bSuccess ) {
    miya_log_fprintf(log_fd, "Failed Open File %s\n",__FUNCTION__);
    miya_log_fprintf(log_fd, " path=%s\n", path );
    miya_log_fprintf(log_fd, " res=%s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path) ));
    return FALSE;
  }

  if( sizeof(int) != FS_ReadFile(&f, &id_count, (s32)sizeof(int)) ) {
    miya_log_fprintf(log_fd, "Failed Read File %s\n",__FUNCTION__);
    miya_log_fprintf(log_fd, " path=%s\n", path );
    miya_log_fprintf(log_fd, " res=%s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path) ));
    return FALSE;
  } 


  *count = id_count;
  size = (int)sizeof(u64) * id_count;

  *pBuffer = (u64 *)OS_Alloc( (u32)size );
  if( *pBuffer == NULL ) {
    return FALSE;
  }

  readSize = FS_ReadFile(&f, *pBuffer, (s32)size );
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

BOOL TitleIDSave(const char *path, u64 *pData, int count, FSFile *log_fd)
{
#pragma unused(log_fd)

  FSFile f;
  BOOL bSuccess;
  FSResult res;
  FSResult fsResult;
  //  s32 writtenSize;

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
    res = FS_GetArchiveResultCode( path );
    miya_log_fprintf(NULL, "%s file length error %s\n", __FUNCTION__,path );
    miya_log_fprintf(NULL, " Failed file lenght :%s\n", my_fs_util_get_fs_result_word( res ));
    return FALSE;
  }

  if( sizeof(int) != FS_WriteFile(&f, &count, (s32)sizeof(int)) ) {
    res = FS_GetArchiveResultCode( path );
    miya_log_fprintf(NULL, "%s file write error %s\n", __FUNCTION__,path );
    miya_log_fprintf(NULL, " Failed write file:%s\n", my_fs_util_get_fs_result_word( res ));
    return FALSE;
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

    /* 16文字だから */
  if( (count*sizeof(u64)) != FS_WriteFile(&f, pData, (s32)(count*sizeof(u64)) )) {
    res = FS_GetArchiveResultCode( path );
    miya_log_fprintf(NULL, "%s file write error %s\n", __FUNCTION__,path );
    miya_log_fprintf(NULL, " Failed write file:%s\n", my_fs_util_get_fs_result_word( res ));
    return FALSE;
  }

  FS_FlushFile(&f);
  bSuccess = FS_CloseFile(&f);
  if( bSuccess ) {
      
  }
  return TRUE;
}



/* SDカードがあるかどうか */
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


/* 過去にショップに接続したかどうか */
BOOL CheckShopRecord(FSFile *log_fd)
{
#pragma unused(log_fd)

  FSFile f;
  BOOL bSuccess;
  char path[256];

  FS_InitFile(&f);

  STD_StrCpy(path, "nand:/sys/dev.kp");
  bSuccess = FS_OpenFileEx(&f, path, (FS_FILEMODE_R));
  if( ! bSuccess ) {
    if( FS_RESULT_NO_ENTRY == FS_GetArchiveResultCode(path) ) {
    }
    return FALSE;
  }
  (void)FS_CloseFile(&f);

#if 0
  STD_StrCpy(path, "nand:/sys/log/shop.log");
  bSuccess = FS_OpenFileEx(&f, path, (FS_FILEMODE_R));
  if( ! bSuccess ) {
    if( FS_RESULT_NO_ENTRY == FS_GetArchiveResultCode(path) ) {
    }
    return FALSE;
  }
  (void)FS_CloseFile(&f);
#endif

  //  STD_StrCpy(path, "nand:/title/00030015/484e4641/data/ec.cfg"); /* ショップアカウント情報 */
  STD_StrCpy(path, "nand:/title/00030015/484e464a/data/ec.cfg"); /* ショップアカウント情報 */
  bSuccess = FS_OpenFileEx(&f, path, (FS_FILEMODE_R));
  if( ! bSuccess ) {
    if( FS_RESULT_NO_ENTRY == FS_GetArchiveResultCode(path) ) {
    }
    return FALSE;
  }
  (void)FS_CloseFile(&f);


  return TRUE;

}

int get_title_id(MY_DIR_ENTRY_LIST **headp, const char *path_src, int *save_parent_dir_info_flag, char *log_file_name, int level )
{
  static FSFile log_fd;
  static BOOL log_active = FALSE;

  FSFile f_src;
  FSDirectoryEntryInfo entry_src;
  FSDirectoryEntryInfo entry_current_dir;
  int ret_value = 0;
  //  FSResult fs_ret_value;
  int save_parent_dir_info_flag_temp = 0;

  char *path_src_dir = NULL;
  char *path_src_full = NULL;

  /* ここでSDカードがあるかどうか調べる */

  if( level == 0 ) {
    log_active = Log_File_Open( &log_fd, log_file_name );
  }

  level++;

  // level         1      2        3       4
  //             nand:/title 
  //             nand:/title/00000000/00000000/data/*.sav
  if( level >  4 ) {
    ret_value = 0;
    goto end_process;
  }

  /* ソースディレクトリオープン */
  FS_InitFile(&f_src);

  if(FS_OpenDirectory(&f_src, path_src, FS_PERMIT_R) == FALSE ) {
    if( log_active ) {
      miya_log_fprintf(&log_fd, "%s %d: Failed Open Directory\n", __FUNCTION__ , __LINE__ );
      miya_log_fprintf(&log_fd, " %s\n", path_src);
      miya_log_fprintf(&log_fd, " %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path_src) ) );
    }
    ret_value = -1;
    goto end_process;
  }

  /* ファイル名バッファ初期化 */
  path_src_dir = (char *)OS_Alloc( FILE_PATH_LEN );
  if( path_src_dir == NULL ) {
    miya_log_fprintf(&log_fd, "Error: alloc error src_dir\n");
    ret_value = -1;
    goto end_process;
  }

  path_src_full = (char *)OS_Alloc( FILE_PATH_LEN );
  if( path_src_full == NULL ) {
    miya_log_fprintf(&log_fd, "Error: alloc error src_full\n");
    ret_value = -1;
    goto end_process;
  }

  STD_MemSet((void *)path_src_dir, 0, FILE_PATH_LEN);
  STD_MemSet((void *)path_src_full, 0, FILE_PATH_LEN);
  STD_StrCpy(path_src_dir, path_src);
  STD_StrCat(path_src_dir, "/");

  while( FS_ReadDirectory(&f_src, &entry_src) ) {
    if( STD_StrCmp(entry_src.longname, ".") == 0 ) {
      /* とりあえずカレントディレクトリエントリを残しておく */
      STD_CopyMemory( (void *)&entry_current_dir, (void *)&entry_src ,sizeof(FSDirectoryEntryInfo) );
    }
    else if( STD_StrCmp(entry_src.longname, "..") == 0 ) {
    }
    else {
      STD_StrCpy( path_src_full , path_src_dir );
      STD_StrCat( path_src_full , entry_src.longname );

      /* SDにログを残す場合 */
      if( log_active ) {
	// OS_TPrintf("path = %s len=%d att=0x%08x\n",path_src_full,entry_src.filesize, entry_src.attributes);
	PrintAttributes(entry_src.attributes, &log_fd);
	miya_log_fprintf(&log_fd, " %8d %s\n", entry_src.filesize, path_src_full );
      }

      if( (entry_src.attributes & FS_ATTRIBUTE_IS_DIRECTORY) != 0 ) {
	/* ディレクトリの場合 */
	// level        1      2        3       4
	//             nand:/title 
	//             nand:/title/00000000/00000000/data/*.sav
	if( level == 1 ) {
	  if( my_fs_is_Title_Hi_dir_name( entry_src.longname ) == TRUE ) {
	    get_title_id( headp, path_src_full , &save_parent_dir_info_flag_temp, log_file_name, level );
	  }
	}
	if( level == 2 ) {
	  if( my_fs_is_Title_Lo_dir_name( entry_src.longname ) == TRUE ) {
	    get_title_id( headp, path_src_full , &save_parent_dir_info_flag_temp, log_file_name, level );
	  }
	}
	else if( (level == 3) ) {
	  if( !STD_StrCmp( entry_src.longname, "content" ) ) {
	    get_title_id( headp, path_src_full , &save_parent_dir_info_flag_temp, log_file_name, level );
	  }
	}
      }	
      else {
	// level        1      2        3       4
	//             nand:/title 
	//             nand:/title/00000000/00000000/content/title.tmd
	if( (level == 4) ) {
	  if( !STD_StrCmp( entry_src.longname, "title.tmd" ) ) {
	    /* 目的のファイルを見つけた。 */
#if 0
	    my_fs_add_list(headp, &entry_src, path_src_full, NULL, &log_fd);
#endif
	    save_parent_dir_info_flag_temp = 1;
	  }
	}
      }
    }
  }

  if( save_parent_dir_info_flag_temp == 1 ) {
    // OS_TPrintf("save dir info = %s\n\n",path_src);
    if( level == 3 ) {
      my_fs_add_list( headp, &entry_current_dir, path_src, NULL, &log_fd);
    }
    else {
      /* contentディレクトリなど */

    }
    *save_parent_dir_info_flag = 1;
  }

  /* ソースディレクトリクローズ */
  if( FS_CloseDirectory(&f_src) == FALSE) {
    miya_log_fprintf(&log_fd, "%s %d: Failed Close Directory\n", __FUNCTION__ , __LINE__ );
    miya_log_fprintf(&log_fd, " %s\n", path_src);
    miya_log_fprintf(&log_fd, " %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path_src)));
    ret_value = -1;
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
    if( log_active ) {
      Log_File_Close(&log_fd);
    }
  }

  return ret_value;
}



int find_title_save_data(MY_DIR_ENTRY_LIST **headp, const char *path_dst, const char *path_src, int *save_parent_dir_info_flag, char *log_file_name,  int level )
{
  static FSFile log_fd;
  static BOOL log_active = FALSE;
  //  static char *log_file_name = "sdmc:/miya/nandinfo_find_title_save_data.txt";

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

  /* ここでSDカードがあるかどうか調べる */

  if( level == 0 ) {
    log_active = Log_File_Open( &log_fd, log_file_name );
  }

  level++;

  // level         1      2        3       4
  //                 nand:/title 
  //                 nand:/title/00000000/00000000/data/*.sav
  if( level >  4 ) {
    ret_value = 0;
    goto end_process;
  }

  /* ソースディレクトリオープン */
  FS_InitFile(&f_src);

  if(FS_OpenDirectory(&f_src, path_src, FS_PERMIT_R) == FALSE ) {
    if( log_active ) {
      miya_log_fprintf(&log_fd, "%s %d: Failed Open Directory\n", __FUNCTION__ , __LINE__ );
      miya_log_fprintf(&log_fd, " %s\n", path_src);
      miya_log_fprintf(&log_fd, " %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path_src) ) );
    }
    ret_value = -1;
    goto end_process;
  }

  /* ファイル名バッファ初期化 */
  if( FALSE == Path_Buffers_Init(path_src, path_dst, 
				 &path_src_dir, &path_src_full, &path_dst_dir, &path_dst_full, &log_fd ) )
    {
      ret_value = -1;
      goto end_process;
    }

  while( FS_ReadDirectory(&f_src, &entry_src) ) {

    if( STD_StrCmp(entry_src.longname, ".") == 0 ) {
      /* とりあえずカレントディレクトリエントリを残しておく */
      STD_CopyMemory( (void *)&entry_current_dir, (void *)&entry_src ,sizeof(FSDirectoryEntryInfo) );
    }
    else if( STD_StrCmp(entry_src.longname, "..") == 0 ) {
    }
    else {
      STD_StrCpy( path_src_full , path_src_dir );
      STD_StrCat( path_src_full , entry_src.longname );
      STD_StrCpy( path_dst_full , path_dst_dir );
      STD_StrCat( path_dst_full , entry_src.longname );

      /* SDにログを残す場合 */
      if( log_active ) {
	// OS_TPrintf("path = %s len=%d att=0x%08x\n",path_src_full,entry_src.filesize, entry_src.attributes);
	PrintAttributes(entry_src.attributes, &log_fd);
	miya_log_fprintf(&log_fd, " %8d %s\n", entry_src.filesize, path_src_full );
      }

      if( (entry_src.attributes & FS_ATTRIBUTE_IS_DIRECTORY) != 0 ) {
	/* ディレクトリの場合 */
	// level         1      2        3       4
	//                 nand:/title 
	//                 nand:/title/00000000/00000000/data/*.sav
	if( level == 1 ) {
	  if( my_fs_is_Title_Hi_dir_name( entry_src.longname ) == TRUE ) {
	    find_title_save_data( headp, path_dst_full, path_src_full , &save_parent_dir_info_flag_temp, log_file_name , level );
	  }
	}
	if( level == 2 ) {
	  if( my_fs_is_Title_Lo_dir_name( entry_src.longname ) == TRUE ) {
	    find_title_save_data( headp, path_dst_full, path_src_full , &save_parent_dir_info_flag_temp, log_file_name, level );
	  }
	}
	else if( (level == 3) ) {
	  if( !STD_StrCmp( entry_src.longname, "data" ) ) {
	    find_title_save_data( headp, path_dst_full, path_src_full , &save_parent_dir_info_flag_temp, log_file_name, level );
	  }
	}
      }	
      else {
	// level         1      2        3       4
	//                 nand:/title 
	//                 nand:/title/00000000/00000000/data/*.sav
	if( (level == 4) ) {
	  if( !STD_StrCmp( entry_src.longname, "public.sav" ) ) {
	    /* 目的のファイルを見つけた。 */
	    my_fs_add_list(headp, &entry_src, path_src_full, path_dst_full, &log_fd);
	    save_parent_dir_info_flag_temp = 1;
	  }
	  else if( !STD_StrCmp( entry_src.longname, "private.sav" ) ) {
	    my_fs_add_list(headp, &entry_src, path_src_full, path_dst_full, &log_fd);
	    save_parent_dir_info_flag_temp = 1;
	  }	  
#if 1
	  /* いるのか？ */
	  else if( !STD_StrCmp( entry_src.longname, "banner.sav" ) ) {
	    my_fs_add_list(headp, &entry_src, path_src_full, path_dst_full, &log_fd);
	    save_parent_dir_info_flag_temp = 1;
	  }	  
#endif	  
	}
      }
    }
  }

  if( save_parent_dir_info_flag_temp == 1 ) {
    // OS_TPrintf("save dir info = %s\n\n",path_src);
    my_fs_add_list( headp, &entry_current_dir, path_src, path_dst, &log_fd);
    *save_parent_dir_info_flag = 1;
  }

  /* ソースディレクトリクローズ */
  if( FS_CloseDirectory(&f_src) == FALSE) {
    miya_log_fprintf(&log_fd, "%s %d: Failed Close Directory\n", __FUNCTION__ , __LINE__ );
    miya_log_fprintf(&log_fd, " %s\n", path_src);
    miya_log_fprintf(&log_fd, " %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path_src)));
    ret_value = -1;
  }

 end_process:

  Path_Buffers_Clean( path_src_dir, path_src_full, path_dst_dir, path_dst_full );

  level--;

  if( level == 0 ) {
    if( log_active ) {
      Log_File_Close(&log_fd);
    }
  }

  return ret_value;
}


int find_copy(MY_DIR_ENTRY_LIST **headp, const char *path_dst, const char *path_src, 
	      char *extension, int max_level, int *save_parent_dir_info_flag, char *log_file_name, int level )
{
  static FSFile log_fd;
  static BOOL log_active = FALSE;
  //  static char *log_file_name = "sdmc:/miya/nandinfo_find_copy.txt";

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

  /* ここでSDカードがあるかどうか調べる */

  if( level == 0 ) {
    log_active = Log_File_Open( &log_fd, log_file_name );
  }

  if( level > max_level ) {
    ret_value = 0;
    goto end_process;
  }

  level++;

  /* ソースディレクトリオープン */
  FS_InitFile(&f_src);

  if( FS_OpenDirectory(&f_src, path_src, FS_PERMIT_R) == FALSE ) {
    miya_log_fprintf(&log_fd, "%s %d: Failed Open Directory\n", __FUNCTION__ , __LINE__ );
    miya_log_fprintf(&log_fd, " %s\n", path_src);
    miya_log_fprintf(&log_fd, " %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path_src) ) );
    ret_value = -1;
    goto end_process;
  }

  /* ファイル名バッファ初期化 */
  if( FALSE == Path_Buffers_Init(path_src, path_dst, 
				 &path_src_dir, &path_src_full, &path_dst_dir, &path_dst_full, &log_fd ) )
    {
      ret_value = -1;
      goto end_process;
    }

  while( FS_ReadDirectory(&f_src, &entry_src) ) {

    if( STD_StrCmp(entry_src.longname, ".") == 0 ) {
      /* とりあえずカレントディレクトリエントリを残しておく */
      STD_CopyMemory( (void *)&entry_current_dir, (void *)&entry_src ,sizeof(FSDirectoryEntryInfo) );
    }
    else if( STD_StrCmp(entry_src.longname, "..") == 0 ) {
    }
    else {
      STD_StrCpy( path_src_full , path_src_dir );
      STD_StrCat( path_src_full , entry_src.longname );
      STD_StrCpy( path_dst_full , path_dst_dir );
      STD_StrCat( path_dst_full , entry_src.longname );

      /* SDにログを残す場合 */
      if( log_active ) {
	PrintAttributes(entry_src.attributes, &log_fd);
	miya_log_fprintf(&log_fd, " %8d %s\n", entry_src.filesize, path_src_full );
      }

      if( (entry_src.attributes & FS_ATTRIBUTE_IS_DIRECTORY) != 0 ) {
	find_copy( headp, path_dst_full, path_src_full , extension, max_level, &save_parent_dir_info_flag_temp , log_file_name, level);
      }	
      else {
	/* ここで拡張子比較 */
	{
	  int len;
	  int extension_len = STD_StrLen( extension );
	  len = STD_StrLen( path_src_full );

	  if( len  > (extension_len - 1) ) {
	    // 123456789012345678901234567890123456789012345678901234567890123
	    // ../backup_sample/ARM7.TWL/bin/ARM7-TS.HYB/Release/ltdmain.c.cpp
	    if( !STD_StrNCmp( &(path_src_full[len - (extension_len )]), extension , extension_len+1 ) ) {
	      /* 目的のファイルを見つけた。 */
	      // OS_TPrintf("%s len=%d att=0x%08x\n", path_src_full, entry_src.filesize, entry_src.attributes);
	      // OS_TPrintf("%s\n", path_dst_full );

	      my_fs_add_list(headp, &entry_src, path_src_full, path_dst_full, &log_fd);

	      save_parent_dir_info_flag_temp = 1;
	    }
	  }
	}
      }
    }
  }

  if( save_parent_dir_info_flag_temp == 1 ) {
    // OS_TPrintf("save dir info = %s\n\n",path_src);
    my_fs_add_list( headp, &entry_current_dir, path_src, path_dst, &log_fd );
    *save_parent_dir_info_flag = 1;
  }

  /* ソースディレクトリクローズ */
  if( FS_CloseDirectory(&f_src) == FALSE) {
    miya_log_fprintf(&log_fd, "%s %d: Failed Close Directory\n", __FUNCTION__ , __LINE__ );
    miya_log_fprintf(&log_fd, " %s\n", path_src);
    miya_log_fprintf(&log_fd, " %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path_src)));
    ret_value = -1;
  }

 end_process:

  Path_Buffers_Clean( path_src_dir, path_src_full, path_dst_dir, path_dst_full );

  level--;
  if( level == 0 ) {
    if( log_active ) {
      Log_File_Close(&log_fd);
    }
  }

  return ret_value;
}


int copy_r( MY_DIR_ENTRY_LIST **headp, const char *path_dst, const char *path_src, char *log_file_name, int level )
{
  static FSFile log_fd;
  static BOOL log_active = FALSE;
  //  static char *log_file_name = "sdmc:/nandinfo_copy_r.txt";

  FSFile f_src;
  FSDirectoryEntryInfo entry_src;
  FSDirectoryEntryInfo entry_current_dir;
  int ret_value = 0;

  char *path_src_dir = NULL;
  char *path_src_full = NULL;
  char *path_dst_dir = NULL;
  char *path_dst_full = NULL;

  /* ここでSDカードがあるかどうか調べる */
  if( level == 0 ) {
    log_active = Log_File_Open( &log_fd, log_file_name );
  }

  level++;

  /* ソースディレクトリオープン */
  FS_InitFile(&f_src);

  if( FS_OpenDirectory(&f_src, path_src, FS_PERMIT_R) == FALSE ) {
    if( log_active ) {
      miya_log_fprintf(&log_fd, "%s %d: Failed Open Directory\n", __FUNCTION__ , __LINE__ );
      miya_log_fprintf(&log_fd, " %s\n", path_src);
      miya_log_fprintf(&log_fd, " %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path_src) ) );
    }

    ret_value = -1;
    goto end_process;
  }

  /* ファイル名バッファ初期化 */
  if( FALSE == Path_Buffers_Init(path_src, path_dst, 
				 &path_src_dir, &path_src_full, &path_dst_dir, &path_dst_full, &log_fd ) )
    {
      ret_value = -1;
      goto end_process;
    }

  while( FS_ReadDirectory(&f_src, &entry_src) ) {

    if( STD_StrCmp(entry_src.longname, ".") == 0 ) {
      /* とりあえずカレントディレクトリエントリを残しておく */
      STD_CopyMemory( (void *)&entry_current_dir, (void *)&entry_src ,sizeof(FSDirectoryEntryInfo) );
    }
    else if( STD_StrCmp(entry_src.longname, "..") == 0 ) {

    }
    else {
      STD_StrCpy( path_src_full , path_src_dir );
      STD_StrCat( path_src_full , entry_src.longname );
      STD_StrCpy( path_dst_full , path_dst_dir );
      STD_StrCat( path_dst_full , entry_src.longname );
      /* SDにログを残す場合 */
      if( log_active ) {
	miya_log_fprintf(&log_fd, "path = %s len=%d\n",path_src_full,entry_src.filesize);
      }

      if( (entry_src.attributes & FS_ATTRIBUTE_IS_DIRECTORY) != 0 ) {
	copy_r( headp, path_dst_full, path_src_full, log_file_name, level );
      }	
      else {
	my_fs_add_list(headp, &entry_src, path_src_full, path_dst_full, &log_fd);
      }
    }
  }

  /* ディレクトリエントリをセーブ */

  my_fs_add_list( headp, &entry_current_dir, path_src, path_dst, &log_fd );

  /* ディレクトリクローズ */
  if( FS_CloseDirectory(&f_src) == FALSE) {
    if( log_active ) {
      miya_log_fprintf(&log_fd, "%s %d: Failed Close Directory\n", __FUNCTION__ , __LINE__ );
      miya_log_fprintf(&log_fd, " %s\n", path_src);
      miya_log_fprintf(&log_fd, " %s\n", my_fs_util_get_fs_result_word( FS_GetArchiveResultCode(path_src)));
    }
    ret_value = -1;
  }

 end_process:

  Path_Buffers_Clean( path_src_dir, path_src_full, path_dst_dir, path_dst_full );

  level--;
  if( level == 0 ) {
    if( log_active ) {
      Log_File_Close(&log_fd);
    }
  }

  return ret_value;
}

void write_debug_data(void)
{
  // CopyFile( dst <= src );
  CopyFile("nand:/tmp/m00.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m01.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m02.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m03.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m04.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m05.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m06.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m07.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m08.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m09.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m10.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m11.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m12.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m13.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m14.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m15.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m16.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m17.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m18.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m19.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m20.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m22.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m23.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m24.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m25.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m26.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m27.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m28.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m29.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m30.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m31.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m32.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m33.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/tmp/m34.sav" , "nand:/sys/log/sysmenu.log", NULL);
  CopyFile("nand:/sys/miya.log", "sdmc:/miya.log", NULL);
  CopyFile("nand:/shared1/miya.log", "sdmc:/miya.log", NULL);



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

