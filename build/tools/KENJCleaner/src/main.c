/********************************************************************/
/*      main.c                                                      */
/*          SaveDataCleaner                                         */
/*                                                                  */
/*              Copyright (C) 2003-2006 NINTENDO Co.,Ltd.           */
/********************************************************************/
/*
	メイン
*/

#include <twl.h>
#include <twl/aes.h>
#include <twl/na.h>
#include <twl/na/ARM9/init.h>
#include <twl/sea.h>
#include <twl/nam.h>
#include "myChar.h"
#include "myFunc.h"


// define data-----------------------------------------------------------------
#define MY_DEBUG						0

#define ROUND_UP(value, alignment) \
    (((u32)(value) + (alignment-1)) & ~(alignment-1))

#define STREAMING_BUFFER_SIZE			(128 * 1024)
#define TITLEID_HI_USER_NAND			0x0003000400000000LL
#define TARGET_INITIALCODE_SPEC_FILE	"SaveDataCleaner.spec"

#define V_NUM							5
#define TARGET_TITLE_LIST_NUM			2

// extern data-----------------------------------------------------------------

// function's prototype--------------------------------------------------------
static void INTR_VBlank( void );
static void InitHeap( void );
void ConstructUserTitleID( OSTitleId *pTitleID, const u8 *pInitialCode );
BOOL SearchTitle( OSTitleId titleID, BOOL *pIsPrivSave, BOOL *pIsPubSave );
BOOL CleanupSaveDataDrive( const char *pDriveName , OSTitleId titleID );
BOOL CreateFileWithLength( char *path, u32 length );
BOOL DeleteDirectoryRecursively( const char *path );
BOOL FillFileRandom( const char* path );
BOOL DumpFile( const char* pPath );
BOOL ReadTargetFileFromSD( const char *pFilename, char **ppDst, u32 *pFileSize );

// global variables------------------------------------------------------------
typedef struct TargetTitleList {
	u8 initialCode[ 5 ];
	u8 saveDataClearFlag;
	u8 titleDeleteFlag;
}TargetTitleList;

TargetTitleList targetTitleList[ TARGET_TITLE_LIST_NUM ] = {
	{ { 'K', 'E', 'N', 'J', 0x00 }, 1, 1, },
	{ { 'K', 'C', 'V', 'J', 0x00 }, 0, 0, },
};

// static variables------------------------------------------------------------
static u8 sStreamBuffer[ STREAMING_BUFFER_SIZE ] ATTRIBUTE_ALIGN(32);
static FSFATFSArchiveWork sOtherTitleWork ATTRIBUTE_ALIGN(32);

// const data------------------------------------------------------------------


// ============================================================================
// function's description
// ============================================================================
void TwlMain(void)
{
	BOOL isFound    = FALSE;
	BOOL isPrivSave = FALSE;
	BOOL isPubSave  = FALSE;
	BOOL isFailed   = FALSE;
	BOOL isAutoExe  = TRUE;
	int i;
	u16 pad_old = 0;
	u16 pad = 0;
	u16 trg = 0;
	OSTitleId titleID;
	
	(void)OS_EnableIrq();
	(void)OS_EnableInterrupts();
	
	OS_Init();
    SEA_Init();
	FS_Init( 3 );
//    AES_Init();
	
	InitHeap();
	InitDispMain();
	InitDispSub ();
	
	NAM_Init( OS_AllocFromMain, OS_FreeToMain );
	
	// Vブランク割り込み許可----------------------------
	(void)OS_SetIrqFunction( OS_IE_V_BLANK, INTR_VBlank );
	(void)OS_EnableIrqMask ( OS_IE_V_BLANK );
	(void)GX_VBlankIntr( TRUE );
	
	//---- 表示開始
	GX_DispOn();
	GXS_DispOn();
	
	PrintStringS(  1, 0, YELLOW, "Title Search & Clearner" );
	
	// SDカード上のSPECファイルから、ターゲットのGameCodeを読み込み
//if USE_SDCARD
#if 0
	{
		u32 size;
		char *pSrc = TARGET_INITIALCODE_SPEC_FILE;
		char *pSDFileBuffer;
		if( !ReadTargetFileFromSD( pSrc, &pSDFileBuffer, &size ) ) {
			PrintStringS(  1,  6, RED, "SD card specfile read failed." );
			PrintStringS(  1,  8, WHITE, "Please set" );
			PrintStringS(  1, 10, WHITE, "  \"%s\"", TARGET_INITIALCODE_SPEC_FILE );
			SVC_WaitVBlankIntr();
			OS_Terminate();
		}
		if( !STD_TSScanf( pSDFileBuffer, "%4s", targetGameCode ) ||
			 STD_StrLen( (const char *)targetGameCode ) != 4 ) {
			PrintStringS(  1,  6, RED, "SD card specfile format failed." );
			PrintStringS(  1,  8, WHITE, "Please set InitialCode." );
			SVC_WaitVBlankIntr();
			OS_Terminate();
		}
		OS_TPrintf( "%s\n", targetGameCode );
		OS_Free( pSDFileBuffer );
	}
#endif
	
	
	// メインループ----------------------------
	for( i = 0; i < TARGET_TITLE_LIST_NUM; i++ ) {
		// タイトルサーチ
		ConstructUserTitleID( &titleID, targetTitleList[i].initialCode );
		isFound = SearchTitle( titleID, &isPrivSave, &isPubSave );
		PrintStringS(  1,  4 + i * V_NUM, WHITE, "InitialCode :%s", targetTitleList[i].initialCode );
		if( isFound ) {
			PrintStringS(  19,  4 + i * V_NUM, GREEN,  "found." );
			FillRectangleM(  0,  (u16)(12 * i), 32, 12 , 0xC020 );	// 赤色フィル
			PrintStringM(  0,  (u16)(12 * i), WHITE,  "%s Result", targetTitleList[i].initialCode );
		}else {
			PrintStringS(  19,  4 + i * V_NUM, YELLOW, "not found." );
			FillRectangleM(  0,  (u16)(12 * i), 32, 12 , 0x8020 );	// 黄色フィル
			PrintStringM(  0,  (u16)(12 * i), WHITE,  "%s Result", targetTitleList[i].initialCode );
		}
		
		SVC_WaitVBlankIntr();
		
		trg = PAD_Read();   // パッドデータ読み取り
		while( isFound ){
			isFailed = FALSE;
			// パッドデータ読み取り
			pad_old = pad;
			pad = PAD_Read();
			trg = (u16)( pad ^ pad_old );
			
			if( ( trg & ( PAD_BUTTON_A | PAD_BUTTON_B ) ) || isAutoExe ) {
				ClearRectangleS(  1,  (u16)( 5 + i * V_NUM ), 31, 1 );
				ClearRectangleS(  1,  (u16)( 5 + i * V_NUM ), 31, 1 );
			}
			
			if( ( trg & PAD_BUTTON_A ) || isAutoExe ) {
				// セーブデータクリア実行
				if( targetTitleList[ i ].saveDataClearFlag ) {
					// Privateセーブデータ
					PrintStringS(  5,  5 + i * V_NUM, WHITE, "PrvSave :" );
					if( isPrivSave ) {
						PrintStringS( 14,  5 + i * V_NUM, YELLOW, "cleanup executing..." );
						if( CleanupSaveDataDrive( "otherPrv", titleID ) ) {
							PrintStringS( 14, 5 + i * V_NUM, GREEN, "cleanup succeeded. " );
						}else {
							PrintStringS( 14, 5 + i * V_NUM, RED,   "cleanup failed.     " );
							isFailed = TRUE;
						}
					}else {
						PrintStringS( 14, 5 + i * V_NUM, YELLOW, "not existed." );
					}
					
					// Publicセーブデータ
					PrintStringS(  5,  6 + i * V_NUM, WHITE, "PubSave :" );
					if( isPubSave ) {
						PrintStringS( 14, 6 + i * V_NUM, YELLOW, "cleanup executing." );
						if( CleanupSaveDataDrive( "otherPub",  titleID ) ) {
							PrintStringS( 14, 6 + i * V_NUM, GREEN, "cleanup succeeded. " );
						}else {
							PrintStringS( 14, 6 + i * V_NUM, RED,   "cleanup failed.     " );
							isFailed = TRUE;
						}
					}else {
						PrintStringS( 14, 6 + i * V_NUM, YELLOW, "not existed." );
					}
				}
				
				// タイトル削除実行
				if( targetTitleList[ i ].titleDeleteFlag ) {
					PrintStringS(  5,  7 + i * V_NUM, WHITE, "Title   :" );
					PrintStringS( 14,  7 + i * V_NUM, YELLOW, "cleanup executing." );
					if( NAM_DeleteTitle( titleID ) == NAM_OK ) {
						PrintStringS( 14,  7 + i * V_NUM, GREEN, "cleanup succeeded. " );
					}else {
						PrintStringS( 14,  7 + i * V_NUM, RED,   "cleanup failed.     " );
						isFailed = TRUE;
					}
					if( isFailed ) {
						FillRectangleM(  0,  (u16)(12 * i), 32, 12 , 0xC020 );	// 赤色フィル
						PrintStringM(  0,  (u16)(12 * i), WHITE,  "%s Result", targetTitleList[i].initialCode );
						//*(u16 *)0x05000006 = myPalette[ RED ][ 1 ];
					}else {
						FillRectangleM(  0,  (u16)(12 * i), 32, 12 , 0xD020 );	// 緑色フィル
						PrintStringM(  0,  (u16)(12 * i), WHITE,  "%s Result", targetTitleList[i].initialCode );
						//*(u16 *)0x05000006 = myPalette[ LIGHTGREEN ][ 1 ];
					}
				}
				break;
			}else if( trg & PAD_BUTTON_B ) {
				// クリアキャンセル。
				PrintStringS(  1,   5 + i * V_NUM, YELLOW, "Cancel Title cleanup." );
				break;
			}
			SVC_WaitVBlankIntr();		// Vブランク割込終了待ち
		}
	}
	
	SVC_WaitVBlankIntr();
	OS_Terminate();
}

// Vブランク割り込み
static void INTR_VBlank(void)
{
	//---- BG-VRAMの更新
	DC_FlushRange( bg0BakM, sizeof(bg0BakM) );
	MI_CpuCopyFast ( bg0BakM, (void*)( HW_BG_VRAM    + BG0_SCREEN_BASE ), sizeof(bg0BakM) );
	DC_FlushRange( bg0BakS, sizeof(bg0BakS) );
	MI_CpuCopyFast ( bg0BakS, (void*)( HW_DB_BG_VRAM + BG0_SCREEN_BASE ), sizeof(bg0BakS) );
	//---- 割り込みチェックフラグ
	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}


// ヒープ初期化
static void InitHeap( void )
{
    void*           tempLo;
    OSHeapHandle    hh;
	
    // メインメモリ上のアリーナにヒープをひとつ作成
    tempLo = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 1);
    OS_SetArenaLo(OS_ARENA_MAIN, tempLo);
    hh = OS_CreateHeap(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi());
    if (hh < 0) {
        // ヒープ作成に失敗した場合は異常終了
        OS_Panic("ARM9: Fail to create heap...\n");
    }
    (void)OS_SetCurrentHeap(OS_ARENA_MAIN, hh);
}


/*---------------------------------------------------------------------------*
    関数定義
 *---------------------------------------------------------------------------*/

// InitialCode から titleID を作成
void ConstructUserTitleID( OSTitleId *pTitleID, const u8 *pInitialCode )
{
	int i;
	u8 *pDst = (u8 *)pTitleID;
	*pTitleID = TITLEID_HI_USER_NAND;
	for( i = 0; i < 4; i++ ) {
		*pDst++ = pInitialCode[ 3 -i ];
	}
}


// 指定タイトルが存在するか確認し、存在するなら、sameMakerFlag を強制セットし、Privateセーブデータ、Publicセーブデータの存在有無を返す。
BOOL SearchTitle( OSTitleId titleID, BOOL *pIsPrivSave, BOOL *pIsPubSave )
{
	int i;
	int index, shift;
	
	OSTitleIDList *pList = (OSTitleIDList *)HW_OS_TITLE_ID_LIST;
	for ( i = 0; i < pList->num; i++ ) {
		if( pList->TitleID[ i ] == titleID ) {
			break;
		}
	}
	if( i == pList->num ) {
		*pIsPrivSave = FALSE;
		*pIsPubSave  = FALSE;
		return FALSE;
	}
	index = i >> 3;
	shift = i % 8;
	pList->sameMakerFlag[ index ] |= 0x01 << shift;
//	pList->privateFlag[ index ]   |= 0x01 << shift;		// ユーザーアプリなら、privateFlagも問題ないはず。
	*pIsPrivSave = ( pList->privateFlag[ index ] & ( 0x01 << shift ) ) ? TRUE : FALSE;
	*pIsPubSave  = ( pList->publicFlag [ index ] & ( 0x01 << shift ) ) ? TRUE : FALSE;
	return TRUE;
}


// 指定されたセーブデータドライブのセーブデータ消去
BOOL CleanupSaveDataDrive( const char *pDriveName , OSTitleId titleID )	// otherPrv, otherPub
{
#define TEMP_FILE		"temp.bin"
	BOOL result   = TRUE;
	FSArchiveResource rsc, rsc2;
	char path[ FS_ENTRY_LONGNAME_MAX ];
	
	if( pDriveName == NULL ) {
		OS_TPrintf( "drive name invalid.\n" );
		return FALSE;
	}
	
	// Privateセーブデータをマウント
	if( FSi_MountSpecialArchive( titleID, pDriveName, &sOtherTitleWork) != FS_RESULT_SUCCESS ) {
		return FALSE;
	}
	
	// Privateセーブデータドライブ上の全データ削除
	STD_TSPrintf( path, "%s:/", pDriveName );
	if( !DeleteDirectoryRecursively( path ) ) {
		OS_TPrintf( "%s delete recursively failed.\n", path );
		result = FALSE;
		goto END;
	}
	
	// ドライブの空き領域を取得
	if( !FS_GetArchiveResource( path, &rsc ) ) {
		OS_TPrintf( "FS_GetArchiveResource(%s) failed : code = %08x\n", path, FS_GetArchiveResultCode( &rsc ) );
		result = FALSE;
		goto END;
	}
	
	// 空き領域が32bitを超えていた場合はエラー
	if( rsc.availableSize > 0x0000000100000000LL ) {
		OS_TPrintf( "%s : availableSize over 32bit.\n", pDriveName );
		result = FALSE;
		goto END;
	}
	
	STD_TSPrintf( path, "%s:/%s", pDriveName, TEMP_FILE );
	
#if MY_DEBUG
	(void)CreateFileWithLength( path, (u32)rsc.availableSize );
	(void)DumpFile( path );
	(void)FS_DeleteFile( path );
#endif
	
	// ドライブの空きサイズMAXで、TEMPファイル生成
	if( !CreateFileWithLength( path, (u32)rsc.availableSize ) ) {
	    OS_TPrintf( "CreateFileWithLength(%s, %08x) failed.\n", path, (u32)rsc.availableSize );
		result = FALSE;
		goto END;
	}
	
	// ファイルランダムフィル
	if( !FillFileRandom( path ) ) {
	    OS_TPrintf( "FillFileRandom(%s) failed.\n", path );
		result = FALSE;
	}
	
	// ファイル削除
	if( !FS_DeleteFile( path ) ) {
	    OS_TPrintf( "FS_DeleteFile(%s) failed.\n", path );
		result = FALSE;
	}
	
	// 再度ドライブの空き領域を取得
	if( !FS_GetArchiveResource( path, &rsc2 ) ) {
		OS_TPrintf( "FS_GetArchiveResource(%s) failed : code = %08x\n", path, FS_GetArchiveResultCode( &rsc2 ) );
		result = FALSE;
	}
	
#if MY_DEBUG
	(void)CreateFileWithLength( path, (u32)rsc.availableSize );
	(void)DumpFile( path );
	(void)FS_DeleteFile( path );
#endif
	
	// 空きサイズが同じかチェック
	OS_TPrintf( "Before available size : %016llx\n", rsc.availableSize );
	OS_TPrintf( "After  available size : %016llx\n", rsc2.availableSize );
	if( rsc.availableSize != rsc2.availableSize ) {
		OS_TPrintf( "available size error.\n" );
		result = FALSE;
	}
	
END:
	// セーブデータのマウント解除
	(void)FSi_MountSpecialArchive(0, NULL, &sOtherTitleWork);
	
	return result;
}


// 指定パス以下のディレクトリを消去
BOOL DeleteDirectoryRecursively( const char *pPath )
{
    FSFile  dir;
    FSDirectoryEntryInfo entryInfo;
    BOOL ret = TRUE;
	char path[ FS_ENTRY_LONGNAME_MAX ];
	
    FS_InitFile( &dir );
	
    // 引数で指定されたディレクトリを開く
    if ( !FS_OpenDirectory( &dir, pPath, FS_FILEMODE_R ) ) {
        OS_TWarning( "Fail! FS_OpenDirectory(%s) in %s\n", pPath, __func__ );
        return FALSE;
    }
	
    // ディレクトリの中身を読む
    while ( FS_ReadDirectory( &dir, &entryInfo ) )
    {
        if ( STD_CompareString( entryInfo.longname, "." )  == 0 ||
             STD_CompareString( entryInfo.longname, ".." ) == 0 ) {
            continue;
        }
		
        STD_CopyLString( path, pPath, FS_ENTRY_LONGNAME_MAX );
        STD_ConcatenateLString( path, "/", FS_ENTRY_LONGNAME_MAX );
        STD_ConcatenateLString( path, entryInfo.longname, FS_ENTRY_LONGNAME_MAX );
		
        // ディレクトリ
        if (entryInfo.attributes & FS_ATTRIBUTE_IS_DIRECTORY) {
            if (!FS_DeleteDirectoryAuto(path)) {
                ret = FALSE;
                OS_TWarning( "Fail! FS_DeleteDirectoryAuto(%s) in %s\n", path, __func__ );
            }
		}else {
		// ファイル
            if ( !FS_DeleteFileAuto( path ) ) {
                ret = FALSE;
                OS_TWarning( "Fail! FS_DeleteFileAuto(%s) in %s\n", path, __func__ );
            }
        }
    }

    // ディレクトリを閉じる
    FS_CloseDirectory( &dir );

    return ret;
}


// 指定ファイルを指定サイズで作成
BOOL CreateFileWithLength( char *path, u32 length )
{
	FSFile file;
	BOOL result = TRUE;
	
	// Privateセーブデータドライブ上にTEMPファイル作成
	if ( !FS_CreateFile( path, FS_PERMIT_R | FS_PERMIT_RW ) )
	{
	    OS_TPrintf( "FS_CreateFile(%s) failed.\n", path );
		return FALSE;
	}
	
	// TEMPファイルオープン
	FS_InitFile(&file);
	if ( !FS_OpenFileEx( &file, path, FS_FILEMODE_W ) )
	{
	    OS_TPrintf( "FS_OpenFile(%s) failed.\n", path );
		result = FALSE;
		goto END;
	}
	
	// ファイルサイズをセット
	if ( FS_SetFileLength( &file, length ) != FS_RESULT_SUCCESS )
	{
	    OS_TPrintf( "FS_SetFileLength(%s) failed.\n", path );
		result = FALSE;
	}
	
END:
	// ファイルクローズ
	(void)FS_CloseFile( &file );
	
	return result;
}


// 指定ファイルをランダムデータでフィル
BOOL FillFileRandom( const char* pPath )
{
    u64 seed;
    MATHRandContext32 rndctx;
    u32 *pBuffer = (u32 *)sStreamBuffer;
    s32 result = TRUE;
    FSFile f;
	
	// 乱数のSEED設定
    if( AES_Rand( &seed, sizeof(seed) ) != AES_RESULT_SUCCESS ) {
        return FALSE;
    }
    MATH_InitRand32( &rndctx, seed );
	
	// ファイルフィル
	FS_InitFile( &f );
	if( !FS_OpenFileEx( &f, pPath, FS_FILEMODE_RWL ) ) {
		return FALSE;
	}else {
        u32 fileSize;
        u32 current;
		
        fileSize = FS_GetFileLength(&f);
		
        for( current = 0; current < fileSize; current += STREAMING_BUFFER_SIZE ) {
            const u32 nextSize = MATH_MIN( STREAMING_BUFFER_SIZE, fileSize - current);
            const u32 nextU32  = MATH_DIVUP( nextSize, sizeof(u32) );
            u32 i;
            s32 writtenSize;
            u32* p = pBuffer;
			
            for( i = 0; i < nextU32; ++i ) {
                *p++ = MATH_Rand32( &rndctx, 0 );
            }
			
            writtenSize = FS_WriteFile( &f, pBuffer, (s32)nextSize );
			
            if( writtenSize != (s32)nextSize ) {
                result = FALSE;
                break;
            }
        }
		
        (void)FS_CloseFile( &f );
    }
	
	return result;
}


// 指定ファイルをストリームバッファに読み捨て
BOOL DumpFile( const char* pPath )
{
    u32 *pBuffer = (u32 *)sStreamBuffer;
    s32 result = TRUE;
    FSFile f;
	
	// ファイルフィル
	FS_InitFile( &f );
	if( !FS_OpenFileEx( &f, pPath, FS_FILEMODE_R ) ) {
		return FALSE;
	}else {
        u32 fileSize;
        u32 current;
		
        fileSize = FS_GetFileLength(&f);
		
        for( current = 0; current < fileSize; current += STREAMING_BUFFER_SIZE ) {
            const u32 nextSize = MATH_MIN( STREAMING_BUFFER_SIZE, fileSize - current);
            const u32 nextU32  = MATH_DIVUP( nextSize, sizeof(u32) );
            s32 readSize;
			
            readSize = FS_ReadFile( &f, pBuffer, (s32)nextSize );
			
            if( readSize != (s32)nextSize ) {
                result = FALSE;
                break;
            }
        }
		
        (void)FS_CloseFile( &f );
    }
	
	return result;
}


// SDカードのルートディレクトリ上の指定ファイルをバッファに読み込み
BOOL ReadTargetFileFromSD( const char *pFilename, char **ppDst, u32 *pFileSize )
{
    FSFile  dir, file;	
    BOOL    open_is_ok;
	BOOL    read_is_ok;
	u32  allocSize;
	BOOL result = TRUE;
	char fullPath[FS_ENTRY_LONGNAME_MAX+6];
	
	// バッファのクリア
	MI_CpuClear8( fullPath, sizeof(fullPath) );
	
    // SDカードのルートディレクトリを検索
	FS_InitFile(&dir);
    if ( !FS_OpenDirectory(&dir, "sdmc:/", FS_FILEMODE_R) )
    {
		OS_TPrintf("Error FS_OpenDirectory(sdmc:/)");
    }
    else
    {
        FSDirectoryEntryInfo   info[1];
		// ルートディレクトリ上でターゲットファイルを探す
        while (FS_ReadDirectory(&dir, info))
        {
            if ((info->attributes & (FS_ATTRIBUTE_DOS_DIRECTORY | FS_ATTRIBUTE_IS_DIRECTORY)) == 0 )
            {
				if (!STD_CompareNString(info->longname, pFilename, STD_GetStringLength( pFilename ) ) )
				{
					STD_CopyString( fullPath, "sdmc:/" );
					STD_ConcatenateString( fullPath, pFilename );
					break;
				}
            }
        }
        (void)FS_CloseDirectory(&dir);
    }

	// ファイルオープン
    FS_InitFile(&file);
    open_is_ok = FS_OpenFile(&file, fullPath);
	if (!open_is_ok)
	{
    	OS_TPrintf( "FS_OpenFile(\"%s\") ... ERROR!\n", fullPath);
		return FALSE;
	}

	// ファイルリード
	*pFileSize  = FS_GetFileLength(&file) ;
	allocSize = ROUND_UP(*pFileSize, 32) ;
	*ppDst = OS_Alloc( allocSize );
	SDK_NULL_ASSERT(*ppDst);
	DC_InvalidateRange(*ppDst, allocSize);
	read_is_ok = FS_ReadFile( &file, *ppDst, (s32)*pFileSize );
	FS_CloseFile(&file);
	if (!read_is_ok)
	{
	    OS_TPrintf( "FS_ReadFile(\"%s\") ... ERROR!\n", fullPath);
		OS_Free(*ppDst);
		return FALSE;
	}
	
	return TRUE;
}

