/*
  製品鍵を使った書き込みを行う場合は、
  TwlIPL/build/buildtools/commondefsの
  "FIRM_USE_PRODUCT_KEYS=TRUE"
  を有効にしてください。


  ifneq ($(TWL_IPL_RED_PRIVATE_ROOT),)
  ifdef FIRM_USE_PRODUCT_KEYS
  MAKEROM_FLAGS			+=	-DHWINFO_PRIVKEY='private_HWInfo.der' \
  -DHWID_PRIVKEY='private_HWID.der'
  else
  MAKEROM_FLAGS			+=	-DHWINFO_PRIVKEY='private_HWInfo_dev.der' \
  -DHWID_PRIVKEY='private_HWID_dev.der'
  endif
  endif


-*- mode: grep; default-directory: "c:/twl/TwlIPL/trunk/build/" -*-
Grep started at Tue Apr 07 11:10:33

/Cygwin/bin/find . "(" -name "*.*" ")" | /Cygwin/bin/xargs C:/Cygwin/bin/grep -n FIRM_USE_PRODUCT_KEYS
./gcdfirm/sdmc-launcher/ARM9/main.c:21:#ifdef FIRM_USE_PRODUCT_KEYS
./gcdfirm/sdmc-launcher/ARM9/main.c:199:#ifdef FIRM_USE_PRODUCT_KEYS
./libraries/os/common/os_boot.c:116:#ifndef FIRM_USE_PRODUCT_KEYS
./libraries/os/common/os_boot.c:123:#else  // !FIRM_USE_PRODUCT_KEYS
./libraries/os/common/os_boot.c:130:#endif // !FIRM_USE_PRODUCT_KEYS
./libraries/os/common/os_init_firm.c:170:#ifndef FIRM_USE_PRODUCT_KEYS
./libraries/os/common/os_init_firm.c:177:#else  // !FIRM_USE_PRODUCT_KEYS
./libraries/os/common/os_init_firm.c:184:#endif // !FIRM_USE_PRODUCT_KEYS
./nandfirm/menu-launcher/ARM9/main.c:21:#ifdef FIRM_USE_PRODUCT_KEYS
./nandfirm/sdmc-launcher/ARM9/main.c:26:#ifdef FIRM_USE_PRODUCT_KEYS
./nandfirm/sdmc-launcher/ARM9/main.c:209:#ifdef FIRM_USE_PRODUCT_KEYS
./systemMenu_tools/HWInfoWriter/ARM9/src/hwi.c:28:#ifdef FIRM_USE_PRODUCT_KEYS                                                // 鍵選択スイッチ
./systemMenu_tools/HWInfoWriter/ARM9/src/hwi.c:196:#ifdef FIRM_USE_PRODUCT_KEYS

Grep finished (matches found) at Tue Apr 07 11:12:16


*/

#include <twl.h>
#include <nitro/nvram/nvram.h>

#include <twl/sea.h>
#include <twl/lcfg.h>
#include <twl/na.h>
#include <twl/nam.h>

#include <twl/os/common/format_rom.h>

#include        "font.h"
#include        "text.h"
#include        "mprintf.h"
#include        "logprintf.h"

#include "myimport.h"


// #define MIYA_IMPORT 1



static s32 my_NAM_ImportTad(const char* path);

static BOOL NAMUT_GetSoftBoxCount( u8* installed, u8* free )
{
  u32 installedSoftBoxCount;
  
  // installedSoftBoxCountの取得
  if ( NAM_OK != NAM_GetInstalledSoftBoxCount( &installedSoftBoxCount ) ) {
    return FALSE;
  }
  
  // installed count
  *installed = (u8)installedSoftBoxCount;
  
  // free count
  *free = (u8)(LCFG_TWL_FREE_SOFT_BOX_COUNT_MAX - installedSoftBoxCount);
  
  return TRUE;
}

static BOOL NAMUT_UpdateSoftBoxCount( void )
{
  u8 installedSoftBoxCount;
  u8 freeSoftBoxCount;
  u8 *pBuffer;
  BOOL retval = TRUE;
  
  // InstalledSoftBoxCount, FreeSoftBoxCount を数えなおす
  if (!NAMUT_GetSoftBoxCount(&installedSoftBoxCount, &freeSoftBoxCount)) {
      return FALSE;
    }
  
  // LCFGライブラリの静的変数に対する更新
  LCFG_TSD_SetInstalledSoftBoxCount( installedSoftBoxCount );	
  LCFG_TSD_SetFreeSoftBoxCount( freeSoftBoxCount );
  
  // LCFGライブラリの静的変数の値をNANDに反映
  pBuffer = OS_Alloc( LCFG_WRITE_TEMP );
  if (!pBuffer) { return FALSE; }
  // ミラーリングデータの両方に書き込みを行う。
  retval &= LCFG_WriteTWLSettings( (u8 (*)[ LCFG_WRITE_TEMP ] )pBuffer );
  retval &= LCFG_WriteTWLSettings( (u8 (*)[ LCFG_WRITE_TEMP ] )pBuffer );
  OS_Free( pBuffer );
  
  return retval;
}


/*---------------------------------------------------------------------------*
  Name:         ImportTad

  Description:  .tad ファイルインポート

  Arguments:    no

  Returns:      None.
 *---------------------------------------------------------------------------*/
BOOL myImportTad(char* file_name, FSFile *log_fd)
{
  NAMTadInfo tadInfo;
  NAMTitleInfo titleInfoTmp;
  char full_path[FS_ENTRY_LONGNAME_MAX+6];
  BOOL ret = FALSE;
  s32  nam_result;
  BOOL overwrite = FALSE;

  // フルパスを作成
  STD_StrCpy(full_path, file_name);

  miya_log_fprintf(log_fd,"start myImportTad %s\n",full_path);

  //  mputchar('.');

  // tadファイルの情報取得
  if (NAM_ReadTadInfo(&tadInfo, full_path) != NAM_OK) {
    miya_log_fprintf(log_fd,"Error:NAM_ReadTadInfo failed %s\n",full_path);
    return FALSE;
  }

  //  mputchar('.');
  /* 1851879012 -> 0x6E616E64 */
  /*  HNCAだったらはねる。無線ファーム */

  // NANDの情報を取得
  if ( NAM_ReadTitleInfo(&titleInfoTmp, tadInfo.titleInfo.titleId) == NAM_OK ) {
    // NANDに既にインストールされているかどうか確認する
    if (tadInfo.titleInfo.titleId == titleInfoTmp.titleId) {
      miya_log_fprintf(log_fd,"id=0x%08x already installed\n",titleInfoTmp.titleId);
      miya_log_fprintf(log_fd," (%s)\n",full_path);

#if 1
      miya_log_fprintf(log_fd," delete title..\n");
      if( NAM_OK != NAM_DeleteTitle( titleInfoTmp.titleId ) ) {
	miya_log_fprintf(log_fd," Error: NAM_DeleteTitle id = 0x%08x\n", titleInfoTmp.titleId);
	return FALSE;
      }
#else
      return FALSE;
#endif
    }
  }
  else {
    /* インストールされていない。 */
    //    miya_log_fprintf(log_fd,"Error:NAM_ReadTitleInfo failed 0x%08x\n",tadInfo.titleInfo.titleId);
    //    return FALSE;
  }
  //  mputchar('.');
    
  // NOT_LAUNCH_FLAG または DATA_ONLY_FLAG が立っていないタイトルの場合
  // freeSoftBoxCountに空きがなければインポートしない
  if (!(tadInfo.titleInfo.titleId & (TITLE_ID_NOT_LAUNCH_FLAG_MASK | TITLE_ID_DATA_ONLY_FLAG_MASK))) {
    // 上書きインポートの場合はfreeSoftBoxCountはチェックしない
    miya_log_fprintf(log_fd,"%s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
    if (!overwrite) {
      u8 installed, free;
      if (!NAMUT_GetSoftBoxCount( &installed, &free )) {
	miya_log_fprintf(log_fd,"Error:%s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
	return FALSE;
      }
      
      if (free == 0) {
	miya_log_fprintf(log_fd,"Error:NAND FreeSoftBoxCount == 0");
	return FALSE;
      }
      miya_log_fprintf(log_fd,"%s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
    }
    
    // ESの仕様で古い e-ticket があると新しい e-ticket を使ったインポートができない
    // 暫定対応として該当タイトルを完全削除してからインポートする
    nam_result = NAM_DeleteTitleCompletely(tadInfo.titleInfo.titleId);
    if ( nam_result != NAM_OK ) {
      miya_log_fprintf(log_fd,"Error:NAM_DeleteTitleCompletely RetCode=%x\n", nam_result);
      return FALSE;
    }
  }

  //  mputchar('.');
    
  // Import開始
  miya_log_fprintf(log_fd,"Import %s Start.\n", full_path );
  
#ifdef MIYA_IMPORT
  nam_result = my_NAM_ImportTad( full_path );
#else
  nam_result = NAM_ImportTad( full_path );
#endif

  //  mputchar('.');
  
  if ( nam_result == NAM_OK ) {
    miya_log_fprintf(log_fd, "Import Success!\n");
    ret = TRUE;
  }
  else {
    /* 
       [ES] verify publickey sign failed: 7
       [ES] verify ticket failed rv=7
       Error:NAM_ImportTad RetCode=7
    */

    miya_log_fprintf(log_fd,"Error:NAM_ImportTad RetCode=%d\n", nam_result);
  }

  //  mputchar('.');  

  // InstalledSoftBoxCount, FreeSoftBoxCount の値を現在のNANDの状態に合わせて更新します。
  NAMUT_UpdateSoftBoxCount();

  return ret;
}

/*  */

#ifdef MIYA_IMPORT


#include "ecdl.h"
#include "c:/twlsdk/build/libraries/nam/ARM9.TWL/include/nam_common.h"
#include "c:/twlsdk/build/libraries/ese/common/include/ese_int.h"

#ifdef PRINT_RESULT
#undef PRINT_RESULT
#endif

#define PRINT_RESULT(x) OS_TPrintf("%s: %4d %5d\n", __FILE__, __LINE__, x )
// #define PRINT_RESULT(x) (void)(0)


//#define SD_CARD_TEST
#ifdef SD_CARD_TEST
#define BASE_TICKET_DIR "sdmc:/ticket"
#define BASE_TITLE_DIR  "sdmc:/title"
#define BASE_IMPORT_DIR "sdmc:/import"
#define BASE_TMP_DIR    "sdmc:/tmp"
#else
#define BASE_TICKET_DIR "nand:/ticket"
#define BASE_TITLE_DIR  "nand:/title"
#define BASE_IMPORT_DIR "nand:/import"
#define BASE_TMP_DIR    "nand:/tmp"
#endif


/* *INDENT-OFF* */
static const char  * const fs_result_strings[] =
{
    "FS_RESULT_SUCCESS",
    "FS_RESULT_FAILURE",
    "FS_RESULT_BUSY",
    "FS_RESULT_CANCELED",
    "FS_RESULT_UNSUPPORTED",
    "FS_RESULT_ERROR",
    "FS_RESULT_INVALID_PARAMETER",
    "FS_RESULT_NO_MORE_RESOUCE",
    "FS_RESULT_ALREADY_DONE",
    "FS_RESULT_PERMISSION_DENIED",
    "FS_RESULT_MEDIA_FATAL",
    "FS_RESULT_NO_ENTRY",
};
static const size_t fs_result_string_max = sizeof(fs_result_strings) / sizeof(*fs_result_strings);
/* *INDENT-ON* */

// FSのエラーを出力します
static void ReportLastErrorPath(const char *path)
{
    FSResult    result = FS_GetArchiveResultCode(path);

    if ((result >= 0) && (result < fs_result_string_max))
    {
        OS_TPrintf("FS error:\n    \"%s\"\n    %s\n",
                   path, fs_result_strings[result]);
    }
    else
    {
        OS_TPrintf("FS error:\n    \"%s\"\n    (%s result code:%d)\n",
                   path, ((result >= fs_result_string_max) && (result < FS_RESULT_MAX)) ? "new" : "unknown", result);
    }
}



// 指定フォルダを上から順番に作成します(一度に深いフォルダは作成できません)
static BOOL ESi_CreateDirectory(const char* path)
{
    FSFile   dir[1];
    const char* marker = "/";
    char     tmppath[FS_ENTRY_LONGNAME_MAX];
    
    // ルートフォルダは作らない
    char*    pos =  STD_SearchString(path, marker) + 1;

    FS_InitFile(dir);

    for (;;)
    {
        pos = STD_SearchString(pos, marker);
        if (pos == NULL)
        {
            break;
        }
        pos++;
        (void)STD_CopyLStringZeroFill(tmppath, path, pos - path);// n-1文字コピー

        if (FS_OpenDirectory(dir, tmppath, FS_FILEMODE_R | FS_FILEMODE_W))
        {   // 既にフォルダが存在しているので作らない
            (void)FS_CloseDirectory(dir);
        }
        else if (!FS_CreateDirectory(tmppath, FS_PERMIT_R | FS_PERMIT_W))
        {
            ReportLastErrorPath(tmppath);
            OS_TWarning("FS_CreateDirectory(\"%s\") failed.", tmppath);
            return FALSE;
        }
    }

    return TRUE;
}

// ファイルを新規に作成して書き込みます。
static BOOL ESi_CreateFile(const char* path, 
               const void* src, s32 len)
{
    FSFile   file[1];

    FS_InitFile(file);

    if (!ESi_CreateDirectory(path))
    {
        return FALSE;
    }

    if (FS_OpenFileEx(file, path, FS_PERMIT_W))
    {    // ファイルが存在すれば上書きします
        //OS_TPrintf("\"%s\" already exists.\n", path);
    }
    else if (!FS_CreateFile(path, FS_PERMIT_R | FS_PERMIT_W))
    {
        ReportLastErrorPath(path);
        OS_TWarning("FS_CreateFile(%s) failed.", path);
        return FALSE;
    }
    else
    {
        //OS_TPrintf("FS_CreateFile(%s) succeeded.\n", path);
        if (!FS_OpenFileEx(file, path, FS_PERMIT_W))
        {
            OS_TWarning("FS_OpenFileEx(%s) failed.", path);
            return FALSE;
        }
    }

    if (len > 0 && FS_WriteFile(file, src, len) < 0)
    {
        OS_TWarning("FS_WritFile() failed.");
        return FALSE;
    }
    else
    {
        //OS_TPrintf("FS_WritFile() succeeded.\n");
    }
    (void)FS_CloseFile(file);
    return TRUE;
}


static void ESi_GetTicketPath(char* path, ESTitleId titleId)
{
    char   titleStr[32];
    char   titleIdH[16];
    char   titleIdL[16];

    // titleID取得してHとLに分解
    (void)STD_TSPrintf(titleStr, "%016llx", titleId);
    (void)STD_CopyLStringZeroFill(titleIdH, titleStr, 9);        // (n - 1) 文字コピーします。
    (void)STD_CopyLStringZeroFill(titleIdL, titleStr + 8, 9);    // (n - 1) 文字コピーします。

    (void)STD_TSPrintf(path, "%s/%s/%s.tik", BASE_TICKET_DIR, titleIdH, titleIdL);
}




static ESError ES_ImportTicket(const void* ticket, const void* certList, u32 certSizeInBytes,
                         const void* crlList, u32 crlSizeInBytes, ESTransferType source)
{
#pragma unused(certList, certSizeInBytes, crlList, crlSizeInBytes, source)

    ESError rv = ES_ERR_OK;

    char                    tmppath[FS_ENTRY_LONGNAME_MAX];
    const ESTicket*         esTicket = (ESTicket*)ticket;

#ifdef VERBOSE_MODE
    ESi_DumpCert(certList);
    ESi_DumpTicket((ESTicket*)ticket);
#endif
    
    if (ticket==NULL || certList==NULL || certSizeInBytes==0)
    {
        rv = ES_ERR_INVALID;
        goto out;
    }
#if defined(NEXT_GEN) || defined(BROADWAY_REV)
    if ( ((u32)ticket & CACHE_LINE_MASK) ||
         ((u32)certList & CACHE_LINE_MASK) ||
         ((u32)crlList & CACHE_LINE_MASK) ) {
        rv = ES_ERR_INVALID;
        goto out;
    }
#endif

#if 0
    if (!ESi_Initialized)
    {
        rv = ES_ERR_INVALID;
        goto out;
    }
#endif

    ESi_GetTicketPath(tmppath, MI_BEToH64(esTicket->titleId));
    
    if (!ESi_CreateFile(tmppath, ticket, sizeof(ESTicket)))
    {
        rv = ES_ERR_FILE_WRITE_FAILED;
    }
  out:
    return rv;
}

#define TMD_FILE "title.tmd"
#define CID_FORMAT "%08x.app"

static void ESi_GetTmpPath(char* path)
{
    (void)STD_TSPrintf(path, "%s/%s", BASE_TMP_DIR, TMD_FILE);
}

static ESError ES_GetTmdSize(void* tmd, u32* size)
{
    ESError rv = ES_ERR_OK;

    if (tmd==NULL)
    {
        rv = ES_ERR_INVALID;
        goto out;
    }

    *size = sizeof(IOSCSigRsa2048) + sizeof(ESTitleMetaHeader) + 
            sizeof(ESContentMeta) * (MI_BEToH16(((ESTitleMeta*)tmd)->head.numContents));

out:
    PRINT_RESULT(rv);
    return rv;
}


static ESError ES_ImportTitleInit(const void* tmd, u32 tmdSize,
                           const void* certList, u32 certSizeInBytes,
                           const void* crlList, u32 crlSizeInBytes,
                           ESTransferType source, int safeUpdate) 
{
#pragma unused(certList, certSizeInBytes, crlList, crlSizeInBytes, safeUpdate)

    ESError rv = ES_ERR_OK;
    char  tmppath[FS_ENTRY_LONGNAME_MAX];

    u32 size;
#ifdef VERBOSE_MODE
    ESi_DumpCert(certList);
    ESi_DumpTMD((ESTitleMeta*)tmd);
#endif
    /*if ( ((u32)tmd & CACHE_LINE_MASK) ||
         ((u32)certList & CACHE_LINE_MASK) ||
         ((u32)crlList & CACHE_LINE_MASK) ) {
        rv = ES_ERR_INVALID;
        goto out;
    }*/
#if 0
    if (!ESi_Initialized)
    {
        rv = ES_ERR_INVALID;
        goto out;
    }
#endif

    switch (source)
    {
      case ES_TRANSFER_SERVER:
        if (tmd==NULL || tmdSize==0 || certList==NULL || certSizeInBytes==0)
        {
            rv = ES_ERR_INVALID;
            goto out;
        }
        break;
      default:
        rv = ES_ERR_UNSUPPORTED_TRANSFER_SOURCE;
        goto out;
        break;
    }

    (void)ES_GetTmdSize((void *)tmd, &size);
    if (tmdSize != size)
    {
        rv = ES_ERR_INVALID;
        goto out;
    }
    ESi_GetTmpPath(tmppath);
    if (!ESi_CreateFile(tmppath, tmd, (s32)size))
    {
        rv = ES_ERR_FILE_WRITE_FAILED;
    }
  out:
    PRINT_RESULT(rv);
    return rv;
}


static void ESi_GetImportPath(char* path, ESTitleId titleId, const char* file)
{
    char   titleStr[32];
    char   titleIdH[16];
    char   titleIdL[16];
    
    // titleID取得してHとLに分解
    (void)STD_TSPrintf(titleStr, "%016llx", titleId);
    (void)STD_CopyLStringZeroFill(titleIdH, titleStr, 9);        // (n - 1) 文字コピーします。
    (void)STD_CopyLStringZeroFill(titleIdL, titleStr + 8, 9);    // (n - 1) 文字コピーします。

    (void)STD_TSPrintf(path, "%s/%s/%s/content/%s",
                       BASE_IMPORT_DIR, titleIdH, titleIdL, file);
}


static FSFile  ContentFile[1];
#define DUMMY_FILE_DESC 1

static s32 ES_ImportContentBegin(ESTitleId titleId, ESContentId cid)
{
    ESError rv = ES_ERR_OK;

    char cidStr[32];
    char tmppath[FS_ENTRY_LONGNAME_MAX];
#if 0
    if (!ESi_Initialized)
    {
        rv = ES_ERR_INVALID;
        goto out;
    }
#endif
    (void)STD_TSPrintf(cidStr, CID_FORMAT, cid);
    ESi_GetImportPath(tmppath, titleId, cidStr);
    
    FS_InitFile(ContentFile);
    if (FS_OpenFileEx(ContentFile, tmppath, FS_PERMIT_W))
    {
        (void)FS_CloseFile(ContentFile);
        if (!FS_DeleteFile(tmppath))
        {    // ファイルを削除します。
            ReportLastErrorPath(tmppath);
            OS_TPrintf("FS_DeleteFile(%s) failed.", tmppath);
            return ES_ERR_FILE_OPEN_FAILED;
        }
    }
    
    if (!ESi_CreateFile(tmppath, NULL, 0))
    {
        ReportLastErrorPath(tmppath);
        OS_TPrintf("FS_CreateFile(%s) failed.", tmppath);
        return ES_ERR_FILE_OPEN_FAILED;
    }

    if (!FS_OpenFileEx(ContentFile, tmppath, FS_PERMIT_W))
    {
        ReportLastErrorPath(tmppath);
        OS_TPrintf("FS_OpenFileEx(%s) failed.", tmppath);
        return ES_ERR_FILE_OPEN_FAILED;
    }

out:
    // 暫定的にFSFile構造体のポインタを返す
    return DUMMY_FILE_DESC;
}

static ESError ES_ImportContentData(s32 fd, const void* inBuf, u32 inBufSize)
{
    ESError rv = ES_ERR_OK;

    if (fd<0 || inBuf==NULL || inBufSize==0)
    {
        rv = ES_ERR_INVALID;
        goto out;
    }
#if 0
    if (!ESi_Initialized)
    {
        rv = ES_ERR_INVALID;
        goto out;
    }
#endif
    if (FS_WriteFile(ContentFile, inBuf, (s32)inBufSize) < 0)
    {
        OS_TPrintf("FS_WritFile() failed.");
        rv = ES_ERR_FILE_WRITE_FAILED;
    }

out:
    return rv;
}

/*---------------------------------------------------------------------------*
  Name:         ES_ImportContentEnd

  Description:  The ES_ImportContentEnd function validates the content data
                written with the hash in the TMD. If the data is valid,
                it commits the data to Flash and closes the file. 
 
  Arguments:    None.

  Returns:      ESError
 *---------------------------------------------------------------------------*/
static ESError ES_ImportContentEnd(s32 fd)
{
#pragma unused(fd)
    ESError rv = ES_ERR_OK;
#if 0
    if (fd < 0 || !ESi_Initialized)
    {
        rv = ES_ERR_INVALID;
        goto out;
    }
#endif
    (void)FS_CloseFile(ContentFile);
out:
    return rv;
}


static ESError ES_ImportTitleDone(void)
{
    ESError rv = ES_ERR_OK;
    char srcpath[FS_ENTRY_LONGNAME_MAX];
    char dstpath[FS_ENTRY_LONGNAME_MAX];

    ESTitleId titleId;
    s32 i, numContents;
    const ESContentMeta* content;
    ESContentId cid;
    BOOL    isOK;

#if 0   
    if (!ESi_Initialized)
    {
        rv = ES_ERR_INVALID;
        goto out;
    }
#endif

    ESi_GetTmpPath(srcpath);
    isOK = ESi_ReadFile(srcpath, &TmdBuf, sizeof(TmdBuf));
    if (!isOK)
    {   // /tmp/title.tmdがない場合、エラーとすべきか不明
        goto out;
    }
    
    titleId = MI_BEToH64(TmdBuf.head.titleId);
    ESi_GetTitlePath(dstpath, titleId, TMD_FILE);
    
    if (!ESi_RenameFile(srcpath, dstpath))
    {
        rv = ES_ERR_FILE_OPEN_FAILED;
        goto out;
    }
    
    numContents = MI_BEToH16(TmdBuf.head.numContents);
    for (i = 0; i < numContents; i++)
    {
        char cidStr[32];
        
        content = &TmdBuf.contents[i];
        cid     = MI_BEToH32(content->cid);

        (void)STD_TSPrintf(cidStr, CID_FORMAT, cid);
        
        ESi_GetImportPath(srcpath, titleId, cidStr);
        ESi_GetTitlePath(dstpath, titleId, cidStr);

        if (!ESi_RenameFile(srcpath, dstpath))
        {
            rv = ES_ERR_FILE_OPEN_FAILED;
            goto out;
        }
    }
    
out:
    return rv;
}


/* *********************************** */


#define NAM_PUBLIC_SAVE_FILE_NAME       "public.sav"
#define NAM_PRIVATE_SAVE_FILE_NAME      "private.sav"
#define NAM_SUB_BANNER_FILE_NAME        "banner.sav"
#define NAM_SUPPORT_BANNER_FILE_NAME    "supbnr.sav"
// パス
#define NAM_ARCHIVE_NAND                "nand"
#define NAM_NAND_ROOT                   NAM_ARCHIVE_NAND ":/"

#define NAM_SHARED2_FILE_PATH           NAM_NAND_ROOT "shared2/%04X"
#define NAM_TICKET_FILE_PATH            NAM_NAND_ROOT "ticket/%08x/%08x.tik"
#define NAM_TITLE_ROOT_PATH             NAM_NAND_ROOT "title/%08x/%08x"

#define NAM_CONTENT_DIR_PATH            NAM_TITLE_ROOT_PATH "/content"
#define NAM_DATA_DIR_PATH               NAM_TITLE_ROOT_PATH "/data"

#define NAM_TMD_FILE_PATH               NAM_CONTENT_DIR_PATH "/title.tmd"
#define NAM_CONTENT_FILE_PATH           NAM_CONTENT_DIR_PATH "/%08x.app"


static s32 NAMi_GetTitleSaveFilePath(char* savePublic, char* savePrivate, NAMTitleId titleId)
{
    ESError result;
    char dataDirPath[FS_ENTRY_LONGNAME_MAX];

    SDK_POINTER_OR_NULL_ASSERT(savePublic);
    SDK_POINTER_OR_NULL_ASSERT(savePrivate);

    // ES_GetDataDir は第2引数に必ず 256 byte 書き込むので別バッファを使用する
    result = ES_GetDataDir(titleId, dataDirPath);
    PRINT_RESULT(result);
    if( result == ES_ERR_OK )
    {
        SDK_ASSERT( dataDirPath[STD_GetStringLength(dataDirPath) - 1] != '/' );

        if( savePublic != NULL )
        {
            (void)STD_TSNPrintf(savePublic, NAM_PATH_LEN, "%s/%s", dataDirPath, NAM_PUBLIC_SAVE_FILE_NAME);
        }
        if( savePrivate != NULL )
        {
            (void)STD_TSNPrintf(savePrivate, NAM_PATH_LEN, "%s/%s", dataDirPath, NAM_PRIVATE_SAVE_FILE_NAME);
        }
    }

    return result;
}





static s32 NAMi_GetShared2FilePath(char* path, u16 index)
{
    (void)STD_TSNPrintf(path, NAM_PATH_LEN, NAM_SHARED2_FILE_PATH, index);

    return NAM_OK;
}


static s32 NAMi_GetTitleSubBannerFilePath(char* banner, NAMTitleId titleId)
{
    ESError result;
    char dataDirPath[FS_ENTRY_LONGNAME_MAX];

    SDK_POINTER_ASSERT(banner);

    // ES_GetDataDir は第2引数に必ず 256 byte 書き込むので別バッファを使用する
    result = ES_GetDataDir(titleId, dataDirPath);
    PRINT_RESULT(result);
    if( result == ES_ERR_OK )
    {
        SDK_ASSERT( dataDirPath[STD_GetStringLength(dataDirPath) - 1] != '/' );
        (void)STD_TSNPrintf(banner, NAM_PATH_LEN, "%s/%s", dataDirPath, NAM_SUB_BANNER_FILE_NAME);
    }

    return result;
}







/* ************************************* */



typedef s32 (*NAMiTadFunc)(FSFile* pTadFile, void* arg);


#define TAD_ALIGN			64	// Please load tads to this alignment in memory

typedef struct NAMiTADHeader
{
    u32 hdrSize;        // Size of TADHeader1 96
    u8  tadType[2];
    u16 tadVersion;		// TAD_VERSION_1
    u32 certSize;
    u32 crlSize;
    u32 ticketSize;
    u32 tmdSize;
    u32 contentSize;
    u32 metaSize;

} NAMiTADHeader;

#define NAM_TAD_SIZE_ADJUST_INITLIB          43071
#define NAM_TAD_SIZE_ADJUST_IMPORTTICKET     68624
#define NAM_TAD_SIZE_ADJUST_IMPORTTITLEINIT 127238
#define NAM_TAD_SIZE_ADJUST_IMPORTTITLEDONE  16069
#define NAM_TAD_SIZE_ADJUST_SUM             ( NAM_TAD_SIZE_ADJUST_INITLIB           \
                                            + NAM_TAD_SIZE_ADJUST_IMPORTTICKET      \
                                            + NAM_TAD_SIZE_ADJUST_IMPORTTITLEINIT   \
                                            + NAM_TAD_SIZE_ADJUST_IMPORTTITLEDONE )

#define NAM_ARCHIVE_OTHER_PUB       "otherPub"
#define NAM_ARCHIVE_OTHER_PRV       "otherPrv"
#define NAM_ARCHIVE_SHARE           "share"


extern FSFATFSArchiveWork sArchiveWork ATTRIBUTE_ALIGN(32);


//---- tad 構成情報の要素
typedef struct NAMiTadParams
{
    u32 cert;
    u32 crl;
    u32 ticket;
    u32 tmd;
    u32 content;
    u32 meta;
}
NAMiTadParams;

//---- tad 構成情報
typedef struct NAMiTadInfo
{
    NAMiTadParams sizes;        // 各領域のサイズ
    NAMiTadParams offsets;      // ファイル先頭から各領域へのオフセット
}
NAMiTadInfo;

//---- tad のコンテンツインポート時に使用する情報
typedef struct NAMiTadContentInfo
{
    ESContentId id;         // コンテンツ ID
    u32         index;      // コンテンツ番号
    u32         size;       // コンテンツサイズ
    u32         offset;     // コンテンツ領域先頭から各コンテンツへのオフセット
}
NAMiTadContentInfo;

//---- tad のインポート時に使用する情報
typedef struct NAMiTadImportContext
{
    void*           cert;               // tad から読み込んだ cert
    void*           crl;                // tad から読み込んだ crl
    ESTitleId       titleId;            // Title ID
    u32             publicSaveSize;     // バックアップ可能セーブデータサイズ
    u32             privateSaveSize;    // バックアップ禁止セーブデータサイズ
    BOOL            bCreateBanner;      // バナーファイルを作成する
    u32             numContents;        // コンテンツ数
    NAMiTadContentInfo* pContentInfo;       // コンテンツインポート情報へのポインタ。numContents ぶんのサイズがある
}
NAMiTadImportContext;

static BOOL IsValidTadHeader(NAMiTadInfo* pInfo, const NAMiTADHeader* pHeader, u32 tadFileSize)
{
    u32 sizeCheck;

    //---- ヘッダサイズの確認
    if( NAMi_EndianU32(pHeader->hdrSize) != sizeof(*pHeader) )
    {
        OS_TPrintf("NAM: tad header check failed (%d != %d)", NAMi_EndianU32(pHeader->hdrSize), sizeof(*pHeader) );
        return FALSE;
    }

    //---- サイズ/オフセットの取得
    pInfo->sizes.cert       = NAMi_EndianU32(pHeader->certSize);
    pInfo->sizes.crl        = NAMi_EndianU32(pHeader->crlSize);
    pInfo->sizes.ticket     = NAMi_EndianU32(pHeader->ticketSize);
    pInfo->sizes.tmd        = NAMi_EndianU32(pHeader->tmdSize);
    pInfo->sizes.content    = NAMi_EndianU32(pHeader->contentSize);
    pInfo->sizes.meta       = NAMi_EndianU32(pHeader->metaSize);

    pInfo->offsets.cert     = MATH_ROUNDUP(sizeof(NAMiTADHeader), TAD_ALIGN);
    pInfo->offsets.crl      = pInfo->offsets.cert       + MATH_ROUNDUP(pInfo->sizes.cert,       TAD_ALIGN);
    pInfo->offsets.ticket   = pInfo->offsets.crl        + MATH_ROUNDUP(pInfo->sizes.crl,        TAD_ALIGN);
    pInfo->offsets.tmd      = pInfo->offsets.ticket     + MATH_ROUNDUP(pInfo->sizes.ticket,     TAD_ALIGN);
    pInfo->offsets.content  = pInfo->offsets.tmd        + MATH_ROUNDUP(pInfo->sizes.tmd,        TAD_ALIGN);
    pInfo->offsets.meta     = pInfo->offsets.content    + MATH_ROUNDUP(pInfo->sizes.content,    TAD_ALIGN);
    sizeCheck               = pInfo->offsets.meta       + MATH_ROUNDUP(pInfo->sizes.meta,       TAD_ALIGN);


    //---- サイズ/オフセットの検証
    if( (pInfo->sizes.cert      == 0)
//     || (pInfo->sizes.crl       == 0)                     // crl は 0 でもよい
     || (pInfo->sizes.ticket    == 0)
     || (pInfo->sizes.tmd       == 0)
     || (pInfo->sizes.content   == 0)
//     || (pInfo->sizes.meta      == 0)                     // meta は 0 でもよい
     || (pInfo->offsets.cert    >= pInfo->offsets.crl)
     || (pInfo->offsets.crl     >  pInfo->offsets.ticket)   // crl は 0 も受け付けるので >= でない
     || (pInfo->offsets.ticket  >= pInfo->offsets.tmd)
     || (pInfo->offsets.tmd     >= pInfo->offsets.content)
     || (pInfo->offsets.content >= pInfo->offsets.meta)
     || (pInfo->offsets.meta    > sizeCheck)                // meta は 0 も受け付けるので >= でない
     || (sizeCheck != tadFileSize) )
    {
        OS_TPrintf("NAM: tad header check failed");
        OS_TPrintf("  size.cert:       %8d (0x%08X)", pInfo->sizes.cert,        pInfo->sizes.cert);
        OS_TPrintf("  size.crl:        %8d (0x%08X)", pInfo->sizes.crl,         pInfo->sizes.crl);
        OS_TPrintf("  size.ticket:     %8d (0x%08X)", pInfo->sizes.ticket,      pInfo->sizes.ticket);
        OS_TPrintf("  size.tmd:        %8d (0x%08X)", pInfo->sizes.tmd,         pInfo->sizes.tmd);
        OS_TPrintf("  size.content:    %8d (0x%08X)", pInfo->sizes.content,     pInfo->sizes.content);
        OS_TPrintf("  size.meta:       %8d (0x%08X)", pInfo->sizes.meta,        pInfo->sizes.meta);
        OS_TPrintf("  offsets.cert:    %8d (0x%08X)", pInfo->offsets.cert,      pInfo->offsets.cert);
        OS_TPrintf("  offsets.crl:     %8d (0x%08X)", pInfo->offsets.crl,       pInfo->offsets.crl);
        OS_TPrintf("  offsets.ticket:  %8d (0x%08X)", pInfo->offsets.ticket,    pInfo->offsets.ticket);
        OS_TPrintf("  offsets.tmd:     %8d (0x%08X)", pInfo->offsets.tmd,       pInfo->offsets.tmd);
        OS_TPrintf("  offsets.content: %8d (0x%08X)", pInfo->offsets.content,   pInfo->offsets.content);
        OS_TPrintf("  offsets.meta:    %8d (0x%08X)", pInfo->offsets.meta,      pInfo->offsets.meta);
        OS_TPrintf("  sizeCheck:       %8d (0x%08X)", sizeCheck,                sizeCheck);
        OS_TPrintf("  fileSize:        %8d (0x%08X)", tadFileSize,              tadFileSize);
        return FALSE;
    }

    return TRUE;
}

static s32 LoadCertCrl(NAMiTadImportContext* pContext, FSFile* pTadFile, const NAMiTadInfo* pInfo)
{
    s32 result1;
    s32 result2;
    void* cert  = NULL;
    void* crl   = NULL;

    result1 = NAMi_Load(pTadFile, &cert, pInfo->sizes.cert, pInfo->offsets.cert, FALSE);
    NAMi_Work.currentSize += pInfo->sizes.cert;
    result2 = NAMi_Load(pTadFile, &crl,  pInfo->sizes.crl,  pInfo->offsets.crl, FALSE);
    NAMi_Work.currentSize += pInfo->sizes.crl;

    if( (result1 != NAM_OK) || (result2 != NAM_OK) )
    {
        NAMi_Free(crl);
        NAMi_Free(cert);
        return (result1 != NAM_OK) ? result1: result2;
    }

    pContext->cert = cert;
    pContext->crl  = crl;

    return NAM_OK;
}


static void UnloadCertCrl(NAMiTadImportContext* pContext)
{
    NAMi_Free(pContext->crl);
    NAMi_Free(pContext->cert);

    pContext->cert = NULL;
    pContext->crl  = NULL;
}

static s32 SetupContentInfo(NAMiTadImportContext* pContext, const ESTitleMeta* tmd)
{
    u32 i;
    u32 offset;
    NAMiTadContentInfo* pContentInfo = NULL;
    const u16 numContents = NAMi_EndianU16(tmd->head.numContents);

    //---- コンテンツごとに NAMiTadContentInfo を確保し情報を格納します。

    pContentInfo = NAMi_Alloc(sizeof(NAMiTadContentInfo) * numContents);

    if( pContentInfo == NULL )
    {
        OS_TPrintf("NAM::SetupContentInfo NAM_NO_MEMORY (%d)", sizeof(NAMiTadContentInfo) * numContents);
        NAMi_Free(pContentInfo);
        return NAM_NO_MEMORY;
    }

    offset = 0;
    for( i = 0; i < numContents; ++i )
    {
        const ESContentMeta* pSrc = &tmd->contents[i];
        NAMiTadContentInfo*      pDst = &pContentInfo[i];
        u64 contentSize  = NAMi_EndianU64(pSrc->size);
        u32 contentIndex = NAMi_EndianU16(pSrc->index);

        //---- tmd 中の ESContentMeta の並びと contentIndex は一致しているものとします。
        if( contentIndex != i )
        {
            OS_TPrintf("NAM::SetupContentInfo NAM_INVALID_CONTENT_INDEX: (%d != %d)", i, contentIndex);
            NAMi_Free(pContentInfo);
            return NAM_INVALID_CONTENT_INDEX;
        }
        //---- 32bit を超えるサイズのコンテンツはエラーとします。
        if( contentSize >= (1ull << 32) )
        {
            OS_TPrintf("NAM::SetupContentInfo NAM_TOO_LARGE_CONTENT_SIZE: %016llX %lld", contentSize, contentSize);
            NAMi_Free(pContentInfo);
            return NAM_TOO_LARGE_CONTENT_SIZE;
        }

        pDst->id     = NAMi_EndianU32(pSrc->cid);
        pDst->index  = contentIndex;
        pDst->size   = (u32)contentSize;
        pDst->offset = offset;

        offset += MATH_ROUNDUP(pDst->size, 64);
    }

    {
        NAMiTmdReserved tmdReserved;

        // アライメントを気にしなくて良いように一旦コピーする
        MI_CpuCopy8(tmd->head.reserved, &tmdReserved, sizeof(tmdReserved));

        pContext->titleId           = NAMi_EndianU64(tmd->head.titleId);
        pContext->publicSaveSize    = tmdReserved.publicSaveSize;
        pContext->privateSaveSize   = tmdReserved.privateSaveSize;
        pContext->bCreateBanner     = ((tmdReserved.flags & NAM_TMD_FLAG_NAND_BANNER) != 0);
        pContext->numContents       = numContents;
        pContext->pContentInfo      = pContentInfo;
    }

    return NAM_OK;
}

static void UnloadContentInfo(NAMiTadImportContext* pContext)
{
    NAMi_Free(pContext->pContentInfo);
    pContext->pContentInfo = NULL;
}



static s32 ReadTadHeader(FSFile* pTadFile, NAMiTadInfo* pInfo)
{
    s32 result;
    u32 fileSize;
    NAMiTADHeader* pHeader;
    BOOL isValid;

    fileSize = FS_GetLength(pTadFile);
    if( fileSize < sizeof(*pHeader) )
    {
        OS_TPrintf("NAM::ReadTadHeader NAM_TOO_SMALL_FILE_USER (%d)", fileSize);
        return NAM_TOO_SMALL_FILE_USER;
    }

    // ヘッダを読む
    result = NAMi_Load(pTadFile, (void**)&pHeader, sizeof(*pHeader), 0, FALSE);
    PRINT_RESULT(result);
    if( result != NAM_OK )
    {
        return result;
    }

    isValid = IsValidTadHeader(pInfo, pHeader, fileSize);
    NAMi_Free(pHeader);

    if( ! isValid )
    {
        return NAM_INVALID_TAD_FORMAT;
    }

    return NAM_OK;
}



static s32 ImportTicket(FSFile* pTadFile, const NAMiTadInfo* pInfo, const NAMiTadImportContext* pContext)
{
    s32 result;
    void* ticket = NULL;

    result = NAMi_Load(pTadFile, &ticket, pInfo->sizes.ticket, pInfo->offsets.ticket, FALSE);
    PRINT_RESULT(result);
    NAMi_Work.currentSize += pInfo->sizes.ticket;

    if( result != NAM_OK )
    {
        return result;
    }

    DC_FlushRange( ticket, pInfo->sizes.ticket );
    result = ES_ImportTicket( ticket,
                              pContext->cert, pInfo->sizes.cert, 
                              pContext->crl,  pInfo->sizes.crl,
                              ES_TRANSFER_SERVER );
    PRINT_RESULT(result);
    NAMi_Work.currentSize += NAM_TAD_SIZE_ADJUST_IMPORTTICKET;
    NAMi_Free(ticket);

    return result;
}


static s32 ImportTitleInit(FSFile* pTadFile, const NAMiTadInfo* pInfo, NAMiTadImportContext* pContext)
{
    s32 result;
    void* tmd = NULL;

    result = NAMi_Load(pTadFile, &tmd, pInfo->sizes.tmd, pInfo->offsets.tmd, FALSE);
    PRINT_RESULT(result);
    NAMi_Work.currentSize += pInfo->sizes.tmd;

    if( result != NAM_OK )
    {
        return result;
    }

    //---- コンテンツインポート用の情報をキャッシュしておきます
    result = SetupContentInfo(pContext, (ESTitleMeta*)tmd);
    PRINT_RESULT(result);

    if( result != NAM_OK )
    {
        NAMi_Free(tmd);
        return result;
    }

    DC_FlushRange( tmd, pInfo->sizes.tmd );
    result = ES_ImportTitleInit( tmd,            pInfo->sizes.tmd,
                                 pContext->cert, pInfo->sizes.cert, 
                                 pContext->crl,  pInfo->sizes.crl,
                                 ES_TRANSFER_SERVER,
                                 1 );
    PRINT_RESULT(result);
    NAMi_Work.currentSize += NAM_TAD_SIZE_ADJUST_IMPORTTITLEINIT;
    NAMi_Free(tmd);

    return result;
}


static s32 ImportTitleFinish(NAMiTadImportContext* pContext, s32 result)
{
    UnloadContentInfo(pContext);

    if( result == NAM_OK )
    {
        result = ES_ImportTitleDone();
        PRINT_RESULT(result);
        NAMi_Work.currentSize += NAM_TAD_SIZE_ADJUST_IMPORTTITLEDONE;
    }
    else
    {
        s32 check = ES_ImportTitleCancel();;
#pragma unused(check)
        SDK_WARNING( check == ES_ERR_OK, "ES_ImportTitleCancel failed %d", check );
    }

    return result;
}



static s32 ImportContent(FSFile* pTadFile, const NAMiTadInfo* pInfo, const NAMiTadImportContext* pContext, int idx)
{
    s32 fd;
    const NAMiTadContentInfo* pContentInfo;
    void* readBuffer;
    u32 processedSize;
    BOOL bSuccess;
    s32 result;
    s32 result2;
    u32 offset;

    pContentInfo = &pContext->pContentInfo[idx];

    offset = pInfo->offsets.content + pContentInfo->offset;
    bSuccess = FS_SeekFile(pTadFile, (s32)offset, FS_SEEK_SET);

    if( ! bSuccess )
    {
        OS_TPrintf("NAM::ImportContent NAM_SEEK_FILE_FAILED_USER (%d)", offset);
        return NAM_SEEK_FILE_FAILED_USER;
    }


    readBuffer = NAMi_Alloc(NAM_STREAMING_BUFFER_SIZE);

    if( readBuffer == NULL )
    {
        OS_TPrintf("NAM::ImportContent NAM_NO_MEMORY (%d)", NAM_STREAMING_BUFFER_SIZE);
        return NAM_NO_MEMORY;
    }

    //---- コンテンツインポート前処理
    fd = ES_ImportContentBegin(pContext->titleId, pContentInfo->id);


    if( fd < 0 )
    {
        NAMi_Free(readBuffer);
	OS_TPrintf("%s: %s %5d\n", __FILE__, __FUNCTION__, __LINE__ );
        return fd;
    }


    processedSize = 0;

    //---- コンテンツインポート処理
    while( processedSize < pContentInfo->size )
    {
        u32 nextSize = MATH_MIN(NAM_STREAMING_BUFFER_SIZE, pContentInfo->size - processedSize);
        u32 readSize;

        DC_InvalidateRange(readBuffer, NAM_STREAMING_BUFFER_SIZE);
        readSize = (u32)FS_ReadFile(pTadFile, readBuffer, (s32)nextSize);

        if( readSize != nextSize )
        {
            result = NAM_READ_FILE_FAILED_USER;
            PRINT_RESULT(result);
            break;
        }

        DC_FlushRange( readBuffer, readSize );

        result = ES_ImportContentData(fd, readBuffer, readSize);
        PRINT_RESULT(result);
        NAMi_Work.currentSize += readSize;

        if( result != ES_ERR_OK )
        {
            break;
        }

        processedSize += readSize;
    }

OS_TPrintf("%s: %s %5d\n", __FILE__, __FUNCTION__, __LINE__ );

    //---- コンテンツインポート後処理
    result2 = ES_ImportContentEnd(fd);
    NAMi_Free(readBuffer);

    return (result == ES_ERR_OK) ? result2: result;
}


static s32 CheckFileSize(const char* path, u32 size)
{
    FSFileInfo info;
    FSResult fsResult;

    fsResult = FS_GetFileInfo(path, &info);

    if( fsResult != FS_RESULT_SUCCESS )
    {
        return NAM_GET_FILE_INFO_FAILED;
    }

    return (info.filesize == size) ? NAM_OK: NAM_SIZE_MISMATCH;
}

static s32 CheckFatFile(ESTitleId titleId, const char* archName)
{
    FSResult fsResult;
    s32 result;

    fsResult = FSi_MountSpecialArchive(titleId, archName, &sArchiveWork);

    if( fsResult == FS_RESULT_SUCCESS )
    {
        char tmpArchPath[OS_MOUNT_ARCHIVE_NAME_LEN + 3];    // 3 = ":/\0"
        BOOL bSuccess;
        FSFile dir;

        (void)STD_TSNPrintf(tmpArchPath, sizeof(tmpArchPath), "%s:/", archName);

        FS_InitFile(&dir);
        bSuccess = FS_OpenDirectory(&dir, tmpArchPath, FS_FILEMODE_R);

        if( bSuccess )
        {
            (void)FS_CloseDirectory(&dir);
            result = NAM_OK;
        }
        else
        {
            result = NAM_OPEN_DIRECTORY_FAILED;
        }

        // アンマウント
        (void)FSi_MountSpecialArchive(titleId, NULL, &sArchiveWork);
    }
    else
    {
        result = NAM_MOUNT_DRIVE_FAILED;
        PRINT_RESULT(result);
    }

    return result;
}

static s32 ConvertCheckResult(s32 result, BOOL bCreate)
{
    if( bCreate )
    {
        if( (result == NAM_SIZE_MISMATCH)
         || (result == NAM_GET_FILE_INFO_FAILED) )
        {
            // エラー or サイズが違うなら要 Setup
            return NAM_REQUIRE_SETUP;
        }
        if( result != NAM_OK )
        {
            // 未知のエラー
            OS_TPrintf("ConvertCheckResult: unknown result %d\n", result);
            return result;
        }
    }
    else
    {
        if( result != NAM_GET_FILE_INFO_FAILED )
        {
            // エラー=ファイルが存在しない でなければ要 Setup
            return NAM_REQUIRE_SETUP;
        }
    }

    return NAM_OK;
}

static s32 CheckSaveDataFile(ESTitleId titleId, BOOL bPrivate, u32 size)
{
    char path[NAM_PATH_LEN];
    s32 result;

    if( bPrivate )
    {
        result = NAMi_GetTitleSaveFilePath(NULL, path, titleId);
        PRINT_RESULT(result);
    }
    else
    {
        result = NAMi_GetTitleSaveFilePath(path, NULL, titleId);
        PRINT_RESULT(result);
    }

    if( result != NAM_OK )
    {
        return result;
    }

    result = ConvertCheckResult(CheckFileSize(path, size), (size > 0));
    if( result != NAM_OK )
    {
        return result;
    }

    if( size > 0 )
    {
        result = CheckFatFile(titleId, (bPrivate ? NAM_ARCHIVE_OTHER_PRV: NAM_ARCHIVE_OTHER_PUB));
        if( result != NAM_OK )
        {
            result = NAM_REQUIRE_SETUP;
        }
    }

    return result;
}

static s32 CheckShared2(u16 index, u32 size)
{
    char path[NAM_PATH_LEN];
    s32 result;

    // パス生成
    result = NAMi_GetShared2FilePath(path, index);
    PRINT_RESULT(result);

    if( result != NAM_OK )
    {
        return result;
    }

    result = CheckFileSize(path, size);

    // ファイルが存在しないなら NG
    if( result == NAM_GET_FILE_INFO_FAILED )
    {
        return result;
    }

    // FAT in File かどうかをチェック
    result = CheckFatFile(index, NAM_ARCHIVE_SHARE);

    if( result != NAM_OK )
    {
        return result;
    }

    return result;
}


static s32 CheckSubBannerFile(ESTitleId titleId, BOOL bCreate)
{
    char path[NAM_PATH_LEN];
    const u32 size = (u32)(bCreate ? NAM_SUB_BANNER_FILE_SIZE: 0);
    s32 result;

    result = NAMi_GetTitleSubBannerFilePath(path, titleId);
    PRINT_RESULT(result);

    if( result != NAM_OK )
    {
        return result;
    }

    return ConvertCheckResult(CheckFileSize(path, size), bCreate);
}


static s32 NAMi_CheckTitleDataFile( ESTitleId titleId,
                             u32       targetMask,
                             u32       publicSize,
                             u32       privateSize,
                             BOOL      bCreateBanner )
{
    s32 result;

    // public セーブデータファイルをチェック
    if( (targetMask & NAM_DATA_FILE_PUBLIC) != 0 )
    {
        result = CheckSaveDataFile(titleId, FALSE, publicSize);
        PRINT_RESULT(result);

        if( result != NAM_OK )
        {
            return result;
        }
    }

    // private セーブデータファイルをチェック
    if( (targetMask & NAM_DATA_FILE_PRIVATE) != 0 )
    {
        result = CheckSaveDataFile(titleId, TRUE, privateSize);
        PRINT_RESULT(result);

        if( result != NAM_OK )
        {
            return result;
        }
    }

    // NAND バナーファイルをチェック
    if( (targetMask & NAM_DATA_FILE_BANNER) != 0 )
    {
        result = CheckSubBannerFile(titleId, bCreateBanner);
        PRINT_RESULT(result);

        if( result != NAM_OK )
        {
            return result;
        }
    }
    return NAM_OK;
}


static s32 FormatFatFile(ESTitleId titleId, const char* archName)
{
    FSResult fsResult;
    s32 result;

    fsResult = FSi_MountSpecialArchive(titleId, archName, &sArchiveWork);

    if( fsResult == FS_RESULT_SUCCESS )
    {
        char tmpArchPath[OS_MOUNT_ARCHIVE_NAME_LEN + 3];    // 3 = ":/\0"
        BOOL bSuccess;

        (void)STD_TSNPrintf(tmpArchPath, sizeof(tmpArchPath), "%s:/", archName);

        bSuccess = FATFSi_FormatMedia(tmpArchPath);

        if( bSuccess )
        {
            bSuccess = FATFS_FormatDrive(tmpArchPath);
            result = bSuccess ? NAM_OK: NAM_FORMAT_DRIVE_FAILED;
            PRINT_RESULT(result);
        }
        else
        {
            result = NAM_FORMAT_MEDIA_FAILED;
            PRINT_RESULT(result);
        }

        // アンマウント
        (void)FSi_MountSpecialArchive(titleId, NULL, &sArchiveWork);
    }
    else
    {
        result = NAM_MOUNT_DRIVE_FAILED;
        PRINT_RESULT(result);
    }

    return result;
}


static s32 CreateSizedFile(const char* path, u32 size)
{
    s32 result;

    result = CheckFileSize(path, size);
    PRINT_RESULT(result);

    switch( result )
    {
    case NAM_OK:
        // ファイルサイズが一致するなら処理完了
        return NAM_OK;

    case NAM_SIZE_MISMATCH:
        // ファイルが存在するなら次へ
        break;

    case NAM_GET_FILE_INFO_FAILED:
        {
            // エラーならファイルが存在しないとみなして新規に作成
            BOOL bSuccess;

            bSuccess = FS_CreateFileAuto(path, (FS_PERMIT_R|FS_PERMIT_W));

            if( ! bSuccess )
            {
                return NAM_CREATE_FILE_FAILED_SYSTEM;
            }
        }
        break;

    default:
        // 未知のエラー
        return result;
    }

    // ファイルサイズを変更
    {
        FSFile f;
        BOOL bSuccess;
        FSResult fsResult;

        FS_InitFile(&f);

        bSuccess = FS_OpenFileEx(&f, path, (FS_FILEMODE_R|FS_FILEMODE_W));

        if( ! bSuccess )
        {
            (void)FS_DeleteFile(path);
            return NAM_OPEN_FILE_FAILED_SYSTEM;
        }

        fsResult = FS_SetFileLength(&f, size);
        result = NAMi_CloseFile(&f, TRUE);

        if( fsResult != FS_RESULT_SUCCESS )
        {
            (void)FS_DeleteFile(path);
            return NAM_SET_FILE_LENGTH_FAILED_SYSTEM;
        }
    }

    return result;
}

static s32 FillFileRandom(const char* path)
{
    u64 seed;
    MATHRandContext32 rndctx;
    u32* pBuffer;
    s32 result;
    AESResult aesResult;

    aesResult = AES_Rand(&seed, sizeof(seed));
    if( aesResult != AES_RESULT_SUCCESS )
    {
        return NAM_RAND_FAILED;
    }
    MATH_InitRand32(&rndctx, seed);

    result = NAMi_Alloc2((void**)&pBuffer, NAM_STREAMING_BUFFER_SIZE);
    if( result == NAM_OK )
    {
        BOOL bSuccess;
        FSFile f;

        FS_InitFile(&f);

        bSuccess = FS_OpenFileEx(&f, path, FS_FILEMODE_RWL);
        if( bSuccess )
        {
            s32 resultClose;
            u32 fileSize;
            u32 current;

            fileSize = FS_GetFileLength(&f);

            for( current = 0; current < fileSize; current += NAM_STREAMING_BUFFER_SIZE )
            {
                const u32 nextSize = MATH_MIN(NAM_STREAMING_BUFFER_SIZE, fileSize - current);
                const u32 nextU32  = MATH_DIVUP(nextSize, sizeof(u32));
                u32* p = pBuffer;
                u32 i;
                s32 writtenSize;

                for( i = 0; i < nextU32; ++i )
                {
                    *p++ = MATH_Rand32(&rndctx, 0);
                }

                writtenSize = FS_WriteFile(&f, pBuffer, (s32)nextSize);
                if( writtenSize != (s32)nextSize )
                {
                    result = NAM_WRITE_FILE_FAILED_SYSTEM;
                    break;
                }
            }

            resultClose = NAMi_CloseFile(&f, TRUE);
        }
        else
        {
            result = NAM_OPEN_FILE_FAILED_SYSTEM;
        }

        NAMi_Free(pBuffer);
    }

    return result;
}

static inline s32 DeleteFile(const char* path)
{
    BOOL bSuccess;
    bSuccess = FS_DeleteFile(path);
    if( bSuccess )
    {
        return NAM_OK;
    }
    else
    {
        if( FS_GetArchiveResultCode(path) == FS_RESULT_ALREADY_DONE )
        {
            return NAM_OK;
        }
        else
        {
            return NAM_DELETE_FILE_FAILED;
        }
    }
}


static s32 UpdateSaveDataFileSecure(ESTitleId titleId, BOOL bPrivate, u32 size)
{
    char path[NAM_PATH_LEN];
    s32 result;

    if( bPrivate )
    {
        result = NAMi_GetTitleSaveFilePath(NULL, path, titleId);
        PRINT_RESULT(result);
    }
    else
    {
        result = NAMi_GetTitleSaveFilePath(path, NULL, titleId);
        PRINT_RESULT(result);
    }

    if( result != NAM_OK )
    {
        return result;
    }

    if( size > 0 )
    {
        // ファイルを作成
        result =  CreateSizedFile(path, size);
        PRINT_RESULT(result);

        if( result != NAM_OK )
        {
            return result;
        }

        // ファイルをランダムクリア
        result = FillFileRandom(path);
        PRINT_RESULT(result);

        if( result != NAM_OK )
        {
            return result;
        }

        // ファイルをフォーマット
        result = FormatFatFile(titleId, (bPrivate ? NAM_ARCHIVE_OTHER_PRV: NAM_ARCHIVE_OTHER_PUB));
        PRINT_RESULT(result);

        if( result != NAM_OK )
        {
            (void)FS_DeleteFile(path);
            return result;
        }
    }
    else
    {
        return DeleteFile(path);
    }


    return NAM_OK;
}

static s32 MakeSubBannerFillData(void* pBuffer)
{
    TWLSubBannerFile* pBanner;
    u16 crc;
    s32 result;

    {
        AESResult aesResult;

        aesResult = AES_Rand(pBuffer, NAM_SUB_BANNER_FILE_SIZE);
        if( aesResult != AES_RESULT_SUCCESS )
        {
            return NAM_RAND_FAILED;
        }
    }

    pBanner = (TWLSubBannerFile*)pBuffer;

    result = NAMi_CalcSubBannerCRC(&crc, pBanner);
    if( result != NAM_OK )
    {
        return NAM_OK;
    }

    if( pBanner->h.crc16_anime == 0 )
    {
        pBanner->h.crc16_anime = 0xFFFF;
    }

    pBanner->h.crc16_anime ^= crc;

    return NAM_OK;
}

static s32 CreateSubBannerFile(const char* path)
{
    s32 result;
    FSFile f;
    void* pFillData;

    result = CreateSizedFile(path, NAM_SUB_BANNER_FILE_SIZE);
    PRINT_RESULT(result);

    if( result != NAM_OK )
    {
        return result;
    }

    {
        BOOL bSuccess;
        s32 written;

        pFillData = NAMi_Alloc(NAM_SUB_BANNER_FILE_SIZE);
        if( pFillData == NULL )
        {
            result = NAM_NO_MEMORY;
            PRINT_RESULT(result);
            goto fail_on_alloc;
        }

        result = MakeSubBannerFillData(pFillData);
        if( result != NAM_OK )
        {
            goto fail_on_open;
        }

        FS_InitFile(&f);

        bSuccess = FS_OpenFileEx(&f, path, (FS_FILEMODE_R|FS_FILEMODE_W));
        if( ! bSuccess )
        {
            result = NAM_OPEN_FILE_FAILED_SYSTEM;
            PRINT_RESULT(result);
            goto fail_on_open;
        }

        written = FS_WriteFile(&f, pFillData, NAM_SUB_BANNER_FILE_SIZE);
        if( written != NAM_SUB_BANNER_FILE_SIZE )
        {
            result = NAM_WRITE_FILE_FAILED_SYSTEM;
            PRINT_RESULT(result);
            goto fail_on_write;
        }

        result = NAMi_CloseFile(&f, TRUE);
        if( result != NAM_OK )
        {
            goto fail_on_open;
        }

        NAMi_Free(pFillData);
    }

    return NAM_OK;
//-------------------------
fail_on_write:
    (void)FS_CloseFile(&f);
fail_on_open:
    NAMi_Free(pFillData);
fail_on_alloc:
    (void)FS_DeleteFile(path);

    return result;
}

static s32 UpdateSubBannerFile(ESTitleId titleId, BOOL bCreate)
{
    char bannerPath[NAM_PATH_LEN];
    s32 result;

    result = NAMi_GetTitleSubBannerFilePath(bannerPath, titleId);
    PRINT_RESULT(result);

    if( result != NAM_OK )
    {
        return result;
    }

    if( bCreate )
    {
        return CreateSubBannerFile(bannerPath);
    }
    else
    {
        return DeleteFile(bannerPath);
    }
}



static s32 NAMi_SetupTitleDataFileSecure( ESTitleId titleId,
                                   u32       targetMask,
                                   u32       publicSize,
                                   u32       privateSize,
                                   BOOL      bCreateBanner )
{
    s32 result;

    // public セーブデータファイルを作成
    if( (targetMask & NAM_DATA_FILE_PUBLIC) != 0 )
    {
        result = UpdateSaveDataFileSecure(titleId, FALSE, publicSize);
        PRINT_RESULT(result);

        if( result != NAM_OK )
        {
            return result;
        }
    }

    // private セーブデータファイルを作成
    if( (targetMask & NAM_DATA_FILE_PRIVATE) != 0 )
    {
        result = UpdateSaveDataFileSecure(titleId, TRUE, privateSize);
        PRINT_RESULT(result);

        if( result != NAM_OK )
        {
            return result;
        }
    }

    // NAND バナーファイルを作成
    if( (targetMask & NAM_DATA_FILE_BANNER) != 0 )
    {
        result = UpdateSubBannerFile(titleId, bCreateBanner);
        PRINT_RESULT(result);

        if( result != NAM_OK )
        {
            return result;
        }
    }

    return NAM_OK;
}

static s32 CreateShared2Secure(u16 index, u32 size)
{
    char path[NAM_PATH_LEN];
    s32 result;

    // パス生成
    result = NAMi_GetShared2FilePath(path, index);
    PRINT_RESULT(result);

    if( result != NAM_OK )
    {
        return result;
    }

    // ファイルを作成
    result =  CreateSizedFile(path, size);
    PRINT_RESULT(result);

    if( result != NAM_OK )
    {
        return result;
    }

    // ファイルをランダムクリア
    result = FillFileRandom(path);
    PRINT_RESULT(result);

    if( result != NAM_OK )
    {
        return result;
    }

    // ファイルをフォーマット
    result = FormatFatFile(index, NAM_ARCHIVE_SHARE);
    PRINT_RESULT(result);

    if( result != NAM_OK )
    {
        (void)FS_DeleteFile(path);
        return result;
    }
    return result;
}


static s32 NAMi_SetupShared2Secure(u16 index, u32 size)
{
    s32 result;

    // FS ブロックサイズより小さいなら何もしないで NAM_OK
    if( size < 16 * 1024 )
    {
        return NAM_OK;
    }

    result = CheckShared2(index, size);

    if( result != NAM_OK )
    {
        result = CreateShared2Secure(index, size);
    }

    return result;
}

#define NAM_SHARED2_NUM     6

typedef struct NAMShared2Param
{
    u32 sizes[NAM_SHARED2_NUM];
}
NAMShared2Param;


static inline u32 ExtendShared2Size(u8 x)
{
    return (x == 0) ? 0: (u32)((x + 1) * 16 * 1024);
}


static s32 ReadShared2Param(NAMShared2Param* psp, NAMTitleId titleId)
{
    char path[NAM_PATH_LEN];
    s32 result;

    MI_CpuClear8(psp, sizeof(*psp));

    result = NAMi_GetBootContentPath(path, titleId);
    if( result == NAM_OK )
    {
        FSFile f;
        ROM_Header_Short* pHeader;
        BOOL bSuccess;

        bSuccess = FS_OpenFileEx(&f, path, FS_FILEMODE_R);
        if( bSuccess )
        {
            s32 resultClose;

            result      = NAMi_Load(&f, (void**)&pHeader, sizeof(ROM_Header_Short), 0, TRUE);
            resultClose = NAMi_CloseFile(&f, TRUE);

            if( result == NAM_OK )
            {
                if( pHeader->access_control.shared2_file )
                {
                    psp->sizes[0] = ExtendShared2Size(pHeader->shared2_file0_size);
                    psp->sizes[1] = ExtendShared2Size(pHeader->shared2_file1_size);
                    psp->sizes[2] = ExtendShared2Size(pHeader->shared2_file2_size);
                    psp->sizes[3] = ExtendShared2Size(pHeader->shared2_file3_size);
                    psp->sizes[4] = ExtendShared2Size(pHeader->shared2_file4_size);
                    psp->sizes[5] = ExtendShared2Size(pHeader->shared2_file5_size);
                }

                NAMi_Free(pHeader);
                result = resultClose;
            }
        }
        else
        {
            result = NAM_OPEN_FILE_FAILED_SYSTEM;
        }
    }

    return result;
}


static s32 NAMi_SetupTitleShared2Secure(NAMTitleId titleId)
{
    s32 result = NAM_OK;

    if( NAM_IsExecTitle(titleId) )
    {
        NAMShared2Param sp;

        // SRL から shared2 情報を取得
        result = ReadShared2Param(&sp, titleId);
        if( result == NAM_OK )
        {
            // shared2 ファイルを準備
            u16 i;

            for( i = 0; i < NAM_SHARED2_NUM; ++i )
            {
                if( sp.sizes[i] > 0 )
                {
                    result = NAMi_SetupShared2Secure(i, sp.sizes[i]);

                    if( result != NAM_OK )
                    {
                        break;
                    }
                }
            }
        }
    }

    return result;
}





static s32 ImportTad(FSFile* pTadFile, const NAMiTadInfo* pInfo)
{
    s32 result;
    int idx;
    NAMiTadImportContext tadContext;

    // cert と crl をメモリに読み込み
    result = LoadCertCrl(&tadContext, pTadFile, pInfo);
    PRINT_RESULT(result);

    if( result != NAM_OK )
    {
        return result;
    }

    // eTicket をインポート
    result = ImportTicket(pTadFile, pInfo, &tadContext);
    PRINT_RESULT(result);

    if( result != NAM_OK )
    {
        UnloadCertCrl(&tadContext);
        return result;
    }

    // 本体のインポート準備
    result = ImportTitleInit(pTadFile, pInfo, &tadContext);
    PRINT_RESULT(result);

    if( result == NAM_OK )
    {
        // コンテンツのインポート
        for( idx = 0; idx < tadContext.numContents; ++idx )
        {
            result = ImportContent(pTadFile, pInfo, &tadContext, idx);
            PRINT_RESULT(result);

            if( result != NAM_OK )
            {
                break;
            }
        }
    }

    // インポートの完了
    result = ImportTitleFinish(&tadContext, result);
    PRINT_RESULT(result);
    UnloadCertCrl(&tadContext);

    if( result != NAM_OK )
    {
        return result;
    }

    // セーブデータファイルのチェック
    result = NAMi_CheckTitleDataFile( tadContext.titleId,
                                      NAM_DATA_FILE_ALL,
                                      tadContext.publicSaveSize,
                                      tadContext.privateSaveSize,
                                      tadContext.bCreateBanner );
    PRINT_RESULT(result);

    // Setup が必要なら行う
    if( result == NAM_REQUIRE_SETUP )
    {
        result = NAMi_SetupTitleDataFileSecure( tadContext.titleId,
                                                NAM_DATA_FILE_ALL,
                                                tadContext.publicSaveSize,
                                                tadContext.privateSaveSize,
                                                tadContext.bCreateBanner );
        PRINT_RESULT(result);
    }

    // shared2 ファイルの準備
    if( result == NAM_OK )
    {
        result = NAMi_SetupTitleShared2Secure(tadContext.titleId);
    }

    if( result != NAM_OK )
    {
        // 失敗したのならタイトルを削除
        (void)NAM_DeleteTitle(tadContext.titleId);
        return result;
    }
    return result;
}



static s32 my_NAM_ImportTadWithFile(FSFile* pTadFile)
{
  extern NAMiWork NAMi_Work;
    s32 result;
    NAMiTadInfo tadInfo;

    SDK_POINTER_ASSERT(pTadFile);
    (void)FS_SeekFileToBegin(pTadFile);

    NAMi_Lock();

    NAMi_Work.totalSize   = 0;
    NAMi_Work.currentSize = 0;
    NAMi_Work.bRunning    = TRUE;

    result = ReadTadHeader(pTadFile, &tadInfo);
    PRINT_RESULT(result);
    if( result == NAM_OK )
    {
        NAMi_Work.totalSize = tadInfo.sizes.cert
                            + tadInfo.sizes.crl
                            + tadInfo.sizes.ticket
                            + tadInfo.sizes.tmd
                            + tadInfo.sizes.content
                            + tadInfo.sizes.meta
                            + NAM_TAD_SIZE_ADJUST_SUM;

        NAMi_Work.currentSize += NAM_TAD_SIZE_ADJUST_INITLIB;
        result = ImportTad(pTadFile, &tadInfo);
        PRINT_RESULT(result);
    }

    NAMi_Unlock();

    NAMi_Work.bRunning    = FALSE;

    return result;
}



static s32 FuncImportTad(FSFile* pTadFile, void* arg)
{
#pragma unused(arg)
    return my_NAM_ImportTadWithFile(pTadFile);
}

static s32 TadFramework(const char* path, NAMiTadFunc pTadFunc, void* arg)
{
    FSFile tadFile;
    BOOL bSuccess;
    s32 result;
    s32 resultClose;

    FS_InitFile(&tadFile);

    bSuccess = FS_OpenFile(&tadFile, path);
    if( ! bSuccess )
    {
        return NAM_OPEN_FILE_FAILED_USER;
    }

    result = pTadFunc(&tadFile, arg);
    PRINT_RESULT(result);

    resultClose = NAMi_CloseFile(&tadFile, FALSE);
    PRINT_RESULT(resultClose);

    return (result == NAM_OK) ? resultClose: result;
}

static s32 my_NAM_ImportTad(const char* path)
{
    return TadFramework(path, &FuncImportTad, NULL);
}

#endif
