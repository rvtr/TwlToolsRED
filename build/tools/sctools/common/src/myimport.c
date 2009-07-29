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

typedef struct NAMTitleInfo
{
    NAMTitleId  titleId;
    u16 companyCode;
    u16 version;
    u32 publicSaveSize;
    u32 privateSaveSize;
    u32 blockSize;
}
NAMTitleInfo;

typedef struct NAMTadInfo
{
    NAMTitleInfo titleInfo;

    u32     fileSize;
}
NAMTadInfo;


c:/twlsdk/add-ins/es/es-sdk-20090216/twl/include/estypes.h

typedef u8   ESVersion;            // 8-bit data structure version
typedef u16  ESTitleVersion;       // 16-bit title version
typedef ESTitleId ESSysVersion;    // 64-bit system software version


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
#include        "my_fs_util.h"

#include "my_sign.h"

#include "myimport.h"
#include "ecdl.h"
#include "nam_common.h"
#include "nam_setup.h"
#include "ese_int.h"


/* miyamoto */

#ifdef PRINT_RESULT
#undef PRINT_RESULT
#endif

// #define PRINT_RESULT(x) OS_TPrintf("%s: %4d %5d\n", __FILE__, __LINE__, x )
#define PRINT_RESULT(x) (void)(0)



/*
#define BASE_TICKET_DIR "nand:/ticket"
#define BASE_TITLE_DIR  "nand:/title"
#define BASE_IMPORT_DIR "nand:/import"
#define BASE_TMP_DIR    "nand:/tmp"
*/


/* ************************************* */

typedef s32 (*NAMiTadFunc)(FSFile* pTadFile, void* arg);
typedef s32 (*NAMiSignTadFunc)(MY_SIGN_File* pTadFile, void* arg);


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

BOOL myDeleteTitle(u64 tid, BOOL with_ticket, FSFile *log_fd)
{
  NAMTitleInfo titleInfoTmp;
  s32  nam_result;


  miya_log_fprintf(log_fd,"start %s\n",__FUNCTION__);


  /* 1851879012 -> 0x6E616E64 */
  /*  HNCAだったらはねる。無線ファーム */

  // NANDの情報を取得
  if ( NAM_ReadTitleInfo(&titleInfoTmp, tid) == NAM_OK ) {
    // NANDに既にインストールされているかどうか確認する
    if (tid == titleInfoTmp.titleId) {
      //      miya_log_fprintf(log_fd,"id=0x%08x already installed\n",titleInfoTmp.titleId);
      //      miya_log_fprintf(log_fd," delete title..\n");
      if( with_ticket ) {
	nam_result = NAM_DeleteTitleCompletely( titleInfoTmp.titleId );
      }
      else {
	nam_result = NAM_DeleteTitle( titleInfoTmp.titleId );
      }
      if ( nam_result != NAM_OK ) {
	miya_log_fprintf(log_fd,"Error:NAM_DeleteTitleCompletely RetCode=%x\n", nam_result);
	return FALSE;
      } 
    }
    else {
      /* インストールされていない。 */
      //    miya_log_fprintf(log_fd,"Error:NAM_ReadTitleInfo failed 0x%08x\n",tadInfo.titleInfo.titleId);
      //    return FALSE;
    OS_TPrintf("%s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
      return TRUE;
    }
  }
  else {
    /* インストールされていない。 */   /* NAM_ReadTitleInfo失敗 */
    OS_TPrintf("%s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
      return TRUE;
      //    return FALSE;
  }
  // InstalledSoftBoxCount, FreeSoftBoxCount の値を現在のNANDの状態に合わせて更新します。
  NAMUT_UpdateSoftBoxCount();
  return TRUE;
}


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





static s32 NAMi_Load_sign(MY_SIGN_File* my_pf, void** ppBuffer, u32 size, u32 offset, BOOL isSystem)
{
    *ppBuffer = NULL;

    if( size > 0 )
    {
        void* buffer;
        BOOL bSuccess;
        s32 readLen;

        buffer = NAMi_Alloc(size);

        if( buffer == NULL )
        {
            OS_TWarning("NAM::Load NAM_NO_MEMORY (%d)", size);
            return NAM_NO_MEMORY;
        }

        bSuccess = my_sign_FS_SeekFile(my_pf, (s32)offset, FS_SEEK_SET);

        if( ! bSuccess )
        {
            OS_TWarning("NAM::Load NAM_SEEK_FILE_FAILED (%d)", offset);
            NAMi_Free(buffer);
            return isSystem ? NAM_SEEK_FILE_FAILED_SYSTEM: NAM_SEEK_FILE_FAILED_USER;
        }

        readLen = my_sign_FS_ReadFile(my_pf, buffer, (s32)size);

        if( readLen != (s32)size )
        {
            OS_TWarning("NAM::Load NAM_READ_FILE_FAILED (%d != %d)", readLen, size);
            NAMi_Free(buffer);
            return isSystem ? NAM_READ_FILE_FAILED_SYSTEM: NAM_READ_FILE_FAILED_USER;
        }

        *ppBuffer = buffer;
    }

    return NAM_OK;
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

static s32 my_sign_LoadCertCrl(NAMiTadImportContext* pContext, MY_SIGN_File* pTadFile, const NAMiTadInfo* pInfo)
{
    s32 result1;
    s32 result2;
    void* cert  = NULL;
    void* crl   = NULL;

    result1 = NAMi_Load_sign(pTadFile, &cert, pInfo->sizes.cert, pInfo->offsets.cert, FALSE);
    NAMi_Work.currentSize += pInfo->sizes.cert;
    result2 = NAMi_Load_sign(pTadFile, &crl,  pInfo->sizes.crl,  pInfo->offsets.crl, FALSE);
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


static s32 myReadTadHeader(FSFile* pTadFile, NAMiTadInfo* pInfo)
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



static s32 my_sign_ReadTadHeader(MY_SIGN_File* pTadFile, NAMiTadInfo* pInfo)
{
    s32 result;
    u32 fileSize;
    NAMiTADHeader* pHeader;
    BOOL isValid;

    fileSize = my_sign_FS_GetLength(pTadFile);
    if( fileSize < sizeof(*pHeader) )
    {
        OS_TPrintf("NAM::ReadTadHeader NAM_TOO_SMALL_FILE_USER (%d)", fileSize);
        return NAM_TOO_SMALL_FILE_USER;
    }

    // ヘッダを読む
    result = NAMi_Load_sign(pTadFile, (void**)&pHeader, sizeof(*pHeader), 0, FALSE);

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



static s32 my_ImportTicket(FSFile* pTadFile, const NAMiTadInfo* pInfo, const NAMiTadImportContext* pContext)
{
    s32 result;
    void* ticket = NULL;

    result = NAMi_Load(pTadFile, &ticket, pInfo->sizes.ticket, pInfo->offsets.ticket, FALSE);
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

    NAMi_Work.currentSize += NAM_TAD_SIZE_ADJUST_IMPORTTICKET;
    NAMi_Free(ticket);

    return result;
}

static s32 my_sign_ImportTicket(MY_SIGN_File* pTadFile, const NAMiTadInfo* pInfo, const NAMiTadImportContext* pContext)
{
    s32 result;
    void* ticket = NULL;

    result = NAMi_Load_sign(pTadFile, &ticket, pInfo->sizes.ticket, pInfo->offsets.ticket, FALSE);
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

    NAMi_Work.currentSize += NAM_TAD_SIZE_ADJUST_IMPORTTICKET;
    NAMi_Free(ticket);

    return result;
}



/* **************************** */
static s32 ImportTadTicketOnly(FSFile* pTadFile, const NAMiTadInfo* pInfo)
{
    s32 result;
    //    int idx;
    NAMiTadImportContext tadContext;

    // cert と crl をメモリに読み込み
    result = LoadCertCrl(&tadContext, pTadFile, pInfo);

    if( result != NAM_OK )
    {
        return result;
    }

    // eTicket をインポート
    result = my_ImportTicket(pTadFile, pInfo, &tadContext);

    if( result != NAM_OK )
    {
        UnloadCertCrl(&tadContext);
        return result;
    }


    UnloadCertCrl(&tadContext);

    if( result != NAM_OK )
    {
        return result;
    }

    // セーブデータファイルのチェック
    // Setup が必要なら行う
    // shared2 ファイルの準備

    return result;
}

static s32 my_sign_ImportTadTicketOnly(MY_SIGN_File* pTadFile, const NAMiTadInfo* pInfo)
{
    s32 result;
    //    int idx;
    NAMiTadImportContext tadContext;

    // cert と crl をメモリに読み込み
    result = my_sign_LoadCertCrl(&tadContext, pTadFile, pInfo);

    if( result != NAM_OK )
    {
        return result;
    }

    // eTicket をインポート
    result = my_sign_ImportTicket(pTadFile, pInfo, &tadContext);

    if( result != NAM_OK )
    {
        UnloadCertCrl(&tadContext);
        return result;
    }


    UnloadCertCrl(&tadContext);

    if( result != NAM_OK )
    {
        return result;
    }

    // セーブデータファイルのチェック
    // Setup が必要なら行う
    // shared2 ファイルの準備

    return result;
}




static s32 my_NAM_ImportTadWithFileTicketOnly(FSFile* pTadFile)
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

    result = myReadTadHeader(pTadFile, &tadInfo);

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
        result = ImportTadTicketOnly(pTadFile, &tadInfo);
    }

    NAMi_Unlock();

    NAMi_Work.bRunning    = FALSE;

    return result;
}




static s32 my_sign_NAM_ImportTadWithFileTicketOnly(MY_SIGN_File* pTadFile)
{
  extern NAMiWork NAMi_Work;
  s32 result;
  NAMiTadInfo tadInfo;

  SDK_POINTER_ASSERT(pTadFile);
  (void)my_sign_FS_SeekFileToBegin(pTadFile);

  NAMi_Lock();
  
  NAMi_Work.totalSize   = 0;
  NAMi_Work.currentSize = 0;
  NAMi_Work.bRunning    = TRUE;
  
  result = my_sign_ReadTadHeader(pTadFile, &tadInfo);

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
      result = my_sign_ImportTadTicketOnly(pTadFile, &tadInfo);
    }
  
  NAMi_Unlock();
  
  NAMi_Work.bRunning    = FALSE;
  
  return result;
}






static s32 FuncImportTadTicketOnly(FSFile* pTadFile, void* arg)
{
#pragma unused(arg)
    return my_NAM_ImportTadWithFileTicketOnly(pTadFile);
}


static s32 FuncImportTadTicketOnly_sign(MY_SIGN_File* pTadFile, void* arg)
{
#pragma unused(arg)
    return my_sign_NAM_ImportTadWithFileTicketOnly(pTadFile);
}


static s32 NAMi_sign_CloseFile(MY_SIGN_File* pFile, BOOL isSystem)
{
    if( my_sign_FS_CloseFile(pFile) )
    {
        return NAM_OK;
    }
    else
    {
        return isSystem ? NAM_CLOSE_FILE_FAILED_SYSTEM: NAM_CLOSE_FILE_FAILED_USER;
    }
}


static s32 TadFrameworkTicketOnly(const char* path, NAMiTadFunc pTadFunc, void* arg)
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

    resultClose = NAMi_CloseFile(&tadFile, FALSE);

    return (result == NAM_OK) ? resultClose: result;
}

static s32 TadFrameworkTicketOnly_sign(const char* path, NAMiSignTadFunc pTadFunc, void* arg)
{
    MY_SIGN_File tadFile;
    BOOL bSuccess;
    s32 result;
    s32 resultClose;

    my_sign_FS_InitFile(&tadFile);

    bSuccess = my_sign_FS_OpenFile(&tadFile, (char *)path);
    if( ! bSuccess )
    {
        return NAM_OPEN_FILE_FAILED_USER;
    }

    result = pTadFunc(&tadFile, arg);

    resultClose = NAMi_sign_CloseFile(&tadFile, FALSE);

    return (result == NAM_OK) ? resultClose: result;
}

BOOL my_NAM_ImportTadTicketOnly(const char* path)
{
#if 1
  s32 result;


  result = TadFrameworkTicketOnly(path, &FuncImportTadTicketOnly, NULL);

  if(result != NAM_OK) {
    return result;
  }
  return TRUE;
#else


    FSFile tadFile;
    BOOL bSuccess;
    s32 result;
    s32 resultClose;

    FS_InitFile(&tadFile);

    bSuccess = FS_OpenFile(&tadFile, path);
    if( !bSuccess )
    {
      OS_TPrintf("open file error:%s %s %d\n", __FILE__,__FUNCTION__,__LINE__);    
      return FALSE;
    }

    result = FuncImportTadTicketOnly(&tadFile, NULL);

    resultClose = NAMi_CloseFile(&tadFile, FALSE);

    if( result != NAM_OK) {
      return FALSE;
    }
    return TRUE;


#endif
}

BOOL my_NAM_ImportTadTicketOnly_sign(const char* path)
{
  s32 result;


  result = TadFrameworkTicketOnly_sign(path, &FuncImportTadTicketOnly_sign, NULL);

  if(result != NAM_OK) {
    return result;
  }
  return TRUE;
}






#define DEFINE_CALC_CONTENTS_BLOCKS(name,type,endianU16,endianU64)                      \
    static u32                                                                          \
    CalcContentsBlocksFrom##name(const type* pTmd)                                      \
    {                                                                                   \
        u32 fsBlocks = 0;                                                               \
        u32 tmdSize;                                                                    \
                                                                                        \
        {                                                                               \
            const u16 numContents = endianU16(pTmd->head.numContents);                  \
            int i;                                                                      \
                                                                                        \
            for( i = 0; i < numContents; ++i )                                          \
            {                                                                           \
                const u32 contentSize = (u32)endianU64(pTmd->contents[i].size);         \
                fsBlocks += MATH_DIVUP(contentSize, NAMi_Work.nandClusterSize);         \
            }                                                                           \
                                                                                        \
            tmdSize = sizeof(ESTitleMeta)                                               \
                    - sizeof(((ESTitleMeta*)0)->contents)                               \
                    + sizeof(ESContentMeta) * numContents;                              \
        }                                                                               \
                                                                                        \
        fsBlocks += MATH_DIVUP(tmdSize, NAMi_Work.nandClusterSize);                     \
                                                                                        \
        return fsBlocks;                                                                \
    }

SDK_COMPILER_ASSERT( sizeof(((ESTitleMeta*)0)->contents) == sizeof(ESContentMeta) * ES_MAX_CONTENT );

DEFINE_CALC_CONTENTS_BLOCKS(Tmd, ESTitleMeta, NAMi_EndianU16, NAMi_EndianU64)


static u32
CalcTitleBlocksWithoutContents(const void* pReserved)
{
    NAMiTmdReserved tmdReserved;
    u32 fsBlocks = 0;
    BOOL hasSubBanner;

    MI_CpuCopy8(pReserved, &tmdReserved, sizeof(tmdReserved));

    hasSubBanner = (tmdReserved.flags & NAM_TMD_FLAG_NAND_BANNER);

    fsBlocks += MATH_DIVUP(tmdReserved.publicSaveSize,  NAMi_Work.nandClusterSize);
    fsBlocks += MATH_DIVUP(tmdReserved.privateSaveSize, NAMi_Work.nandClusterSize);

    if( hasSubBanner )
    {
        fsBlocks += MATH_DIVUP(NAM_SUB_BANNER_FILE_SIZE, NAMi_Work.nandClusterSize);
    }

    return fsBlocks;
}


static u32
my_NAMi_CalcTitleBlocksFromTmd(const ESTitleMeta* pTmd)
{
    u32 fsBlocks = CalcTitleBlocksWithoutContents(&pTmd->head.reserved)
                 + CalcContentsBlocksFromTmd(pTmd);
    return fsBlocks;
}




static s32 my_NAM_ReadTadInfoWithFile(NAMTadInfo* pInfo, FSFile* pTadFile)
{
    s32 result;
    NAMiTadInfo tadInfo;
    ESTitleMeta* pTmd;


    SDK_POINTER_ASSERT(pInfo);
    SDK_POINTER_ASSERT(pTadFile);
    (void)FS_SeekFileToBegin(pTadFile);

    result = myReadTadHeader(pTadFile, &tadInfo);

    if( result != NAM_OK )
    {
OS_TPrintf("ERROR:%s %s %d\n", __FILE__,__FUNCTION__,__LINE__);    
        return result;
    }

    // TMD を読む
    result = NAMi_Load( pTadFile, 
                        (void**)&pTmd,
                        tadInfo.sizes.tmd,
                        tadInfo.offsets.tmd,
                        FALSE );
    if( result != NAM_OK )
    {
OS_TPrintf("ERROR:%s %s %d\n", __FILE__,__FUNCTION__,__LINE__);    

        return result;
    }

    NAMi_CopyTmdReservedInfo(&pInfo->titleInfo, &pTmd->head.reserved);
    pInfo->titleInfo.titleId            = NAMi_EndianU64(pTmd->head.titleId);
    pInfo->titleInfo.companyCode        = NAMi_EndianU16(pTmd->head.groupId);
    pInfo->titleInfo.version            = NAMi_EndianU16(pTmd->head.titleVersion);
    pInfo->titleInfo.blockSize          = my_NAMi_CalcTitleBlocksFromTmd(pTmd);
    pInfo->fileSize                     = FS_GetLength(pTadFile);

    NAMi_Free(pTmd);

    return NAM_OK;
}

static s32 my_sign_NAM_ReadTadInfoWithFile(NAMTadInfo* pInfo, MY_SIGN_File* pTadFile)
{
    s32 result;
    NAMiTadInfo tadInfo;
    ESTitleMeta* pTmd;


    SDK_POINTER_ASSERT(pInfo);
    SDK_POINTER_ASSERT(pTadFile);
    (void)my_sign_FS_SeekFileToBegin(pTadFile);

    result = my_sign_ReadTadHeader(pTadFile, &tadInfo);

    if( result != NAM_OK )
    {
OS_TPrintf("ERROR:%s %s %d\n", __FILE__,__FUNCTION__,__LINE__);    
        return result;
    }

    // TMD を読む
    result = NAMi_Load_sign( pTadFile, 
                        (void**)&pTmd,
                        tadInfo.sizes.tmd,
                        tadInfo.offsets.tmd,
                        FALSE );
    if( result != NAM_OK )
    {
OS_TPrintf("ERROR:%s %s %d\n", __FILE__,__FUNCTION__,__LINE__);    

        return result;
    }

    NAMi_CopyTmdReservedInfo(&pInfo->titleInfo, &pTmd->head.reserved);
    pInfo->titleInfo.titleId            = NAMi_EndianU64(pTmd->head.titleId);
    pInfo->titleInfo.companyCode        = NAMi_EndianU16(pTmd->head.groupId);
    pInfo->titleInfo.version            = NAMi_EndianU16(pTmd->head.titleVersion);
    pInfo->titleInfo.blockSize          = my_NAMi_CalcTitleBlocksFromTmd(pTmd);
    pInfo->fileSize                     = my_sign_FS_GetLength(pTadFile);

    NAMi_Free(pTmd);



    return NAM_OK;
}




static s32 FuncReadTadInfo(FSFile* pTadFile, void* arg)
{
    return my_NAM_ReadTadInfoWithFile((NAMTadInfo*)arg, pTadFile);
}

static s32 FuncSignReadTadInfo(MY_SIGN_File* pTadFile, void* arg)
{
    return my_sign_NAM_ReadTadInfoWithFile((NAMTadInfo*)arg, pTadFile);
}

static s32 myTadFramework(const char* path, NAMiTadFunc pTadFunc, void* arg)
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


    resultClose = NAMi_CloseFile(&tadFile, FALSE);

    return (result == NAM_OK) ? resultClose: result;
}


static s32 my_sign_TadFramework(const char* path, NAMiSignTadFunc pTadFunc, void* arg)
{
    MY_SIGN_File tadFile;
    BOOL bSuccess;
    s32 result;
    s32 resultClose;

    my_sign_FS_InitFile(&tadFile);


    bSuccess = my_sign_FS_OpenFile(&tadFile, (char *)path);
    if( ! bSuccess )
    {

        return NAM_OPEN_FILE_FAILED_USER;
    }

    result = pTadFunc(&tadFile, arg);


    resultClose = NAMi_sign_CloseFile(&tadFile, FALSE);

    return (result == NAM_OK) ? resultClose: result;
}


static BOOL my_NAM_ReadTadInfo(NAMTadInfo* pInfo, const char* path)
{

  if( NAM_OK == myTadFramework(path, &FuncReadTadInfo, pInfo )) {

    return TRUE;
  }

  return FALSE;
}


static BOOL my_sign_NAM_ReadTadInfo(NAMTadInfo* pInfo, const char* path)
{

  if( NAM_OK == my_sign_TadFramework(path, &FuncSignReadTadInfo, pInfo )) {

    return TRUE;
  }

  return FALSE;
}

/*
### mwccarm.exe Compiler:
#    1104:     result = NAMi_CheckTitleDataFile( tadContext.titleId,
#   Error:              ^^^^^^^^^^^^^^^^^^^^^^^
#   function has no prototype
### mwccarm.exe Compiler:
#    1114:         result = NAMi_SetupTitleDataFileSecure( tadContext.titleId,
#   Error:                  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#   function has no prototype
### mwccarm.exe Compiler:
#    1125:         result = NAMi_SetupTitleShared2Secure(tadContext.titleId);
*/





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
        OS_TWarning("NAM::SetupContentInfo NAM_NO_MEMORY (%d)", sizeof(NAMiTadContentInfo) * numContents);
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
            OS_TWarning("NAM::SetupContentInfo NAM_INVALID_CONTENT_INDEX: (%d != %d)", i, contentIndex);
            NAMi_Free(pContentInfo);
            return NAM_INVALID_CONTENT_INDEX;
        }
        //---- 32bit を超えるサイズのコンテンツはエラーとします。
        if( contentSize >= (1ull << 32) )
        {
            OS_TWarning("NAM::SetupContentInfo NAM_TOO_LARGE_CONTENT_SIZE: %016llX %lld", contentSize, contentSize);
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



/*---------------------------------------------------------------------------*
  Name:         UnloadContentInfo

  Description:  SetupContentInfo で行った前処理の解放を行います。

  Arguments:    pContext:   対象の ImportContext。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static void UnloadContentInfo(NAMiTadImportContext* pContext)
{
    NAMi_Free(pContext->pContentInfo);
    pContext->pContentInfo = NULL;
}


static s32 my_ImportTitleInit(FSFile* pTadFile, const NAMiTadInfo* pInfo, NAMiTadImportContext* pContext)
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


static s32 my_sign_ImportTitleInit(MY_SIGN_File* pTadFile, const NAMiTadInfo* pInfo, NAMiTadImportContext* pContext)
{
    s32 result;
    void* tmd = NULL;

    result = NAMi_Load_sign(pTadFile, &tmd, pInfo->sizes.tmd, pInfo->offsets.tmd, FALSE);
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



/*---------------------------------------------------------------------------*
  Name:         ImportTitleFinish

  Description:  tad からの NAND アプリインポート後処理を行います。

  Arguments:    pContext:   ImportContext。
                result:     コンテンツインポート処理の結果。

  Returns:      正常に処理が行われたなら NAM_OK を返します。
 *---------------------------------------------------------------------------*/
static s32 my_ImportTitleFinish(NAMiTadImportContext* pContext, s32 result)
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



/*---------------------------------------------------------------------------*
  Name:         ImportContent

  Description:  tad から NAND アプリコンテンツをインポートします。

  Arguments:    pTadFile:   tad ファイル。
                pInfo:      tad 構成情報。
                pContext:   ImportContext。
                idx:        インポートするコンテンツ番号。

  Returns:      正常に処理が行われたなら NAM_OK を返します。
 *---------------------------------------------------------------------------*/
static s32 my_ImportContent(FSFile* pTadFile, const NAMiTadInfo* pInfo, const NAMiTadImportContext* pContext, int idx)
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
        OS_TWarning("NAM::ImportContent NAM_SEEK_FILE_FAILED_USER (%d)", offset);
        return NAM_SEEK_FILE_FAILED_USER;
    }

    readBuffer = NAMi_Alloc(NAM_STREAMING_BUFFER_SIZE);

    if( readBuffer == NULL )
    {
        OS_TWarning("NAM::ImportContent NAM_NO_MEMORY (%d)", NAM_STREAMING_BUFFER_SIZE);
        return NAM_NO_MEMORY;
    }

    //---- コンテンツインポート前処理
    fd = ES_ImportContentBegin(pContext->titleId, pContentInfo->id);

    if( fd < 0 )
    {
        NAMi_Free(readBuffer);
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

    //---- コンテンツインポート後処理
    result2 = ES_ImportContentEnd(fd);
    NAMi_Free(readBuffer);

    return (result == ES_ERR_OK) ? result2: result;
}


static s32 my_sign_ImportContent(MY_SIGN_File* pTadFile, const NAMiTadInfo* pInfo, const NAMiTadImportContext* pContext, int idx)
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
    bSuccess = my_sign_FS_SeekFile(pTadFile, (s32)offset, FS_SEEK_SET);

    if( ! bSuccess )
    {
        OS_TWarning("NAM::ImportContent NAM_SEEK_FILE_FAILED_USER (%d)", offset);
        return NAM_SEEK_FILE_FAILED_USER;
    }

    readBuffer = NAMi_Alloc(NAM_STREAMING_BUFFER_SIZE);

    if( readBuffer == NULL )
    {
        OS_TWarning("NAM::ImportContent NAM_NO_MEMORY (%d)", NAM_STREAMING_BUFFER_SIZE);
        return NAM_NO_MEMORY;
    }

    //---- コンテンツインポート前処理
    fd = ES_ImportContentBegin(pContext->titleId, pContentInfo->id);

    if( fd < 0 )
    {
        NAMi_Free(readBuffer);
        return fd;
    }

    processedSize = 0;

    //---- コンテンツインポート処理
    while( processedSize < pContentInfo->size )
    {
        u32 nextSize = MATH_MIN(NAM_STREAMING_BUFFER_SIZE, pContentInfo->size - processedSize);
        u32 readSize;

        DC_InvalidateRange(readBuffer, NAM_STREAMING_BUFFER_SIZE);
        readSize = (u32)my_sign_FS_ReadFile(pTadFile, readBuffer, (s32)nextSize);

        if( readSize != nextSize )
        {
            result = NAM_READ_FILE_FAILED_USER;
            PRINT_RESULT(result);
            break;
        }

        DC_FlushRange( readBuffer, readSize );

        result = ES_ImportContentData(fd, readBuffer, readSize);
	//        PRINT_RESULT(result);
        NAMi_Work.currentSize += readSize;

        if( result != ES_ERR_OK )
        {
            break;
        }

        processedSize += readSize;
    }

    //---- コンテンツインポート後処理
    result2 = ES_ImportContentEnd(fd);
    NAMi_Free(readBuffer);

    return (result == ES_ERR_OK) ? result2: result;
}


static s32 my_ImportTad(FSFile* pTadFile, const NAMiTadInfo* pInfo)
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
    result = my_ImportTicket(pTadFile, pInfo, &tadContext);
    PRINT_RESULT(result);

    if( result != NAM_OK )
    {
        UnloadCertCrl(&tadContext);
        return result;
    }

    // 本体のインポート準備
    result = my_ImportTitleInit(pTadFile, pInfo, &tadContext);
    PRINT_RESULT(result);

    if( result == NAM_OK )
    {
        // コンテンツのインポート
        for( idx = 0; idx < tadContext.numContents; ++idx )
        {
            result = my_ImportContent(pTadFile, pInfo, &tadContext, idx);
            PRINT_RESULT(result);

            if( result != NAM_OK )
            {
                break;
            }
        }
    }

    // インポートの完了
    result = my_ImportTitleFinish(&tadContext, result);
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

static s32 my_sign_ImportTad(MY_SIGN_File* pTadFile, const NAMiTadInfo* pInfo)
{
    s32 result;
    int idx;
    NAMiTadImportContext tadContext;

    // cert と crl をメモリに読み込み
    result = my_sign_LoadCertCrl(&tadContext, pTadFile, pInfo);
    PRINT_RESULT(result);

    if( result != NAM_OK )
    {
        return result;
    }

    // eTicket をインポート
    result = my_sign_ImportTicket(pTadFile, pInfo, &tadContext);
    PRINT_RESULT(result);

    if( result != NAM_OK )
    {
        UnloadCertCrl(&tadContext);
        return result;
    }

    // 本体のインポート準備
    result = my_sign_ImportTitleInit(pTadFile, pInfo, &tadContext);
    PRINT_RESULT(result);

    if( result == NAM_OK )
    {
        // コンテンツのインポート
        for( idx = 0; idx < tadContext.numContents; ++idx )
        {
            result = my_sign_ImportContent(pTadFile, pInfo, &tadContext, idx);
            PRINT_RESULT(result);

            if( result != NAM_OK )
            {
                break;
            }
        }
    }

    // インポートの完了
    result = my_ImportTitleFinish(&tadContext, result);
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
    s32 result;
    NAMiTadInfo tadInfo;

    SDK_POINTER_ASSERT(pTadFile);
    (void)FS_SeekFileToBegin(pTadFile);

    NAMi_Lock();

    NAMi_Work.totalSize   = 0;
    NAMi_Work.currentSize = 0;
    NAMi_Work.bRunning    = TRUE;

    result = myReadTadHeader(pTadFile, &tadInfo);
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
        result = my_ImportTad(pTadFile, &tadInfo);
        PRINT_RESULT(result);
    }

    NAMi_Unlock();

    NAMi_Work.bRunning    = FALSE;

    return result;
}

static s32 my_sign_NAM_ImportTadWithFile(MY_SIGN_File* pTadFile)
{
    s32 result;
    NAMiTadInfo tadInfo;

    SDK_POINTER_ASSERT(pTadFile);
    (void)my_sign_FS_SeekFileToBegin(pTadFile);

    NAMi_Lock();

    NAMi_Work.totalSize   = 0;
    NAMi_Work.currentSize = 0;
    NAMi_Work.bRunning    = TRUE;

    result = my_sign_ReadTadHeader(pTadFile, &tadInfo);
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
        result = my_sign_ImportTad(pTadFile, &tadInfo);
        PRINT_RESULT(result);
    }

    NAMi_Unlock();

    NAMi_Work.bRunning    = FALSE;

    return result;
}


static s32 my_FuncImportTad(FSFile* pTadFile, void* arg)
{
#pragma unused(arg)
    return my_NAM_ImportTadWithFile(pTadFile);
}

static s32 my_sign_FuncImportTad(MY_SIGN_File* pTadFile, void* arg)
{
#pragma unused(arg)
    return my_sign_NAM_ImportTadWithFile(pTadFile);
}

static s32 my_NAM_ImportTad(const char* path)
{
    return myTadFramework(path, &my_FuncImportTad, NULL);
}


static s32 my_sign_NAM_ImportTad(const char* path)
{
    return my_sign_TadFramework(path, &my_sign_FuncImportTad, NULL);
}





/*---------------------------------------------------------------------------*
  Name:         ImportTad

  Description:  .tad ファイルインポート

  Arguments:    no

  Returns:      None.
 *---------------------------------------------------------------------------*/
BOOL myImportTad(char* full_path, int org_version, FSFile *log_fd)
{
  NAMTadInfo tadInfo;
  NAMTitleInfo titleInfoTmp;
  //  char full_path[FS_ENTRY_LONGNAME_MAX+6];
  BOOL ret = FALSE;
  s32  nam_result;
  BOOL overwrite = FALSE;

  // フルパスを作成
  //  STD_StrCpy(full_path, file_name);
  //  OS_TPrintf("import from ROM(%s)\n", full_path);

  miya_log_fprintf(log_fd,"start myImportTad %s\n",full_path);


  // tadファイルの情報取得
  if ( my_NAM_ReadTadInfo(&tadInfo, full_path) != TRUE ) {
    miya_log_fprintf(log_fd,"Error:NAM_ReadTadInfo failed %s\n",full_path);
    return FALSE;
  }

  miya_log_fprintf(log_fd,"tadfile.ver=%d org.ver=%d\n", tadInfo.titleInfo.version,org_version);
  if(  org_version > tadInfo.titleInfo.version ) {
    miya_log_fprintf(log_fd,"Error:org.ver=%d tadfile.ver=%d %s\n",full_path);
    return FALSE;
  }


  /* 1851879012 -> 0x6E616E64 */
  /*  HNCAだったらはねる。無線ファーム */

  // NANDの情報を取得
  if ( NAM_ReadTitleInfo(&titleInfoTmp, tadInfo.titleInfo.titleId) == NAM_OK ) {
    // NANDに既にインストールされているかどうか確認する
    if (tadInfo.titleInfo.titleId == titleInfoTmp.titleId) {
      miya_log_fprintf(log_fd,"id=0x%08x already installed\n",titleInfoTmp.titleId);
      miya_log_fprintf(log_fd," (%s)\n",full_path);

      miya_log_fprintf(log_fd," delete title..\n");
      if( NAM_OK != NAM_DeleteTitleCompletely( titleInfoTmp.titleId ) ) {
	miya_log_fprintf(log_fd," Error: NAM_DeleteTitle id = 0x%08x\n", titleInfoTmp.titleId);
	return FALSE;
      }
    }
  }
  else {
    /* インストールされていない。 */
    //    miya_log_fprintf(log_fd,"Error:NAM_ReadTitleInfo failed 0x%08x\n",tadInfo.titleInfo.titleId);
    //    return FALSE;
  }
    
  // NOT_LAUNCH_FLAG または DATA_ONLY_FLAG が立っていないタイトルの場合
  // freeSoftBoxCountに空きがなければインポートしない
  if (!(tadInfo.titleInfo.titleId & (TITLE_ID_NOT_LAUNCH_FLAG_MASK | TITLE_ID_DATA_ONLY_FLAG_MASK))) {
    // 上書きインポートの場合はfreeSoftBoxCountはチェックしない
    //    miya_log_fprintf(log_fd,"%s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
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
      //      miya_log_fprintf(log_fd,"%s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
    }
    
    // ESの仕様で古い e-ticket があると新しい e-ticket を使ったインポートができない
    // 暫定対応として該当タイトルを完全削除してからインポートする
    nam_result = NAM_DeleteTitleCompletely(tadInfo.titleInfo.titleId);
    if ( nam_result != NAM_OK ) {
      miya_log_fprintf(log_fd,"Error:NAM_DeleteTitleCompletely RetCode=%x\n", nam_result);
      return FALSE;
    }
  }
  miya_log_fprintf(log_fd,"Import %s Start.\n", full_path );
  
  //  nam_result = NAM_ImportTad( full_path );
  nam_result = my_NAM_ImportTad( full_path );
  
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
  // InstalledSoftBoxCount, FreeSoftBoxCount の値を現在のNANDの状態に合わせて更新します。
  NAMUT_UpdateSoftBoxCount();

  return ret;
}



BOOL myImportTad_sign(char* full_path, int org_version, FSFile *log_fd)
{
  NAMTadInfo tadInfo;
  NAMTitleInfo titleInfoTmp;
  //  char full_path[FS_ENTRY_LONGNAME_MAX+6];
  BOOL ret = FALSE;
  s32  nam_result;
  BOOL overwrite = FALSE;

  // フルパスを作成
  //  STD_StrCpy(full_path, file_name);
  // OS_TPrintf("import from SD Card(%s)\n", full_path);

  miya_log_fprintf(log_fd,"start my_sign_ImportTad %s\n",full_path);


  // tadファイルの情報取得
  if ( my_sign_NAM_ReadTadInfo(&tadInfo, full_path) != TRUE ) {
    miya_log_fprintf(log_fd,"Error:NAM_ReadTadInfo failed %s\n",full_path);
    return FALSE;
  }

  miya_log_fprintf(log_fd,"tadfile.ver=%d org.ver=%d\n", tadInfo.titleInfo.version,org_version);
  if(  org_version > tadInfo.titleInfo.version ) {
    miya_log_fprintf(log_fd,"Error:org.ver=%d tadfile.ver=%d %s\n",full_path);
    return FALSE;
  }

  /* 1851879012 -> 0x6E616E64 */
  /*  HNCAだったらはねる。無線ファーム */

  // NANDの情報を取得
  if ( NAM_ReadTitleInfo(&titleInfoTmp, tadInfo.titleInfo.titleId) == NAM_OK ) {
    // NANDに既にインストールされているかどうか確認する
    if (tadInfo.titleInfo.titleId == titleInfoTmp.titleId) {
      miya_log_fprintf(log_fd,"id=0x%08x already installed\n",titleInfoTmp.titleId);
      miya_log_fprintf(log_fd," (%s)\n",full_path);

      miya_log_fprintf(log_fd," delete title..\n");
      if( NAM_OK != NAM_DeleteTitleCompletely( titleInfoTmp.titleId ) ) {
	miya_log_fprintf(log_fd," Error: NAM_DeleteTitle id = 0x%08x\n", titleInfoTmp.titleId);
	return FALSE;
      }
    }
  }
  else {
    /* インストールされていない。 */
    //    miya_log_fprintf(log_fd,"Error:NAM_ReadTitleInfo failed 0x%08x\n",tadInfo.titleInfo.titleId);
    //    return FALSE;
  }
    
  // NOT_LAUNCH_FLAG または DATA_ONLY_FLAG が立っていないタイトルの場合
  // freeSoftBoxCountに空きがなければインポートしない
  if (!(tadInfo.titleInfo.titleId & (TITLE_ID_NOT_LAUNCH_FLAG_MASK | TITLE_ID_DATA_ONLY_FLAG_MASK))) {
    // 上書きインポートの場合はfreeSoftBoxCountはチェックしない
    //    miya_log_fprintf(log_fd,"%s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
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
      //      miya_log_fprintf(log_fd,"%s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
    }
    
    // ESの仕様で古い e-ticket があると新しい e-ticket を使ったインポートができない
    // 暫定対応として該当タイトルを完全削除してからインポートする
    nam_result = NAM_DeleteTitleCompletely(tadInfo.titleInfo.titleId);
    if ( nam_result != NAM_OK ) {
      miya_log_fprintf(log_fd,"Error:NAM_DeleteTitleCompletely RetCode=%x\n", nam_result);
      return FALSE;
    }
  }
  miya_log_fprintf(log_fd,"Import %s Start.\n", full_path );
  
  //  nam_result = NAM_ImportTad( full_path );
  nam_result = my_sign_NAM_ImportTad( full_path );
  
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
  // InstalledSoftBoxCount, FreeSoftBoxCount の値を現在のNANDの状態に合わせて更新します。
  NAMUT_UpdateSoftBoxCount();

  return ret;
}






