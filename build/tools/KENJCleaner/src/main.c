/********************************************************************/
/*      main.c                                                      */
/*          SaveDataCleaner                                         */
/*                                                                  */
/*              Copyright (C) 2003-2006 NINTENDO Co.,Ltd.           */
/********************************************************************/
/*
	���C��
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
#define MY_DEBUG	0

#define ROUND_UP(value, alignment) \
    (((u32)(value) + (alignment-1)) & ~(alignment-1))

#define STREAMING_BUFFER_SIZE         (128 * 1024)
#define TITLEID_HI_USER_NAND		  0x0003000400000000LL
#define TARGET_INITIALCODE_SPEC_FILE  "SaveDataCleaner.spec"

// extern data-----------------------------------------------------------------

// function's prototype--------------------------------------------------------
static void INTR_VBlank( void );
static void InitHeap( void );
BOOL SearchTitle( OSTitleId titleID, BOOL *pIsPrivSave, BOOL *pIsPubSave );
BOOL CleanupSaveDataDrive( const char *pDriveName , OSTitleId titleID );
BOOL CreateFileWithLength( char *path, u32 length );
BOOL DeleteDirectoryRecursively( const char *path );
BOOL FillFileRandom( const char* path );
BOOL DumpFile( const char* pPath );
BOOL ReadTargetFileFromSD( const char *pFilename, char **ppDst, u32 *pFileSize );

// global variables------------------------------------------------------------
u8 targetGameCode[ 5 ] = { 'K', 'E', 'N', 'J', 0x00 };

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
	
	// V�u�����N���荞�݋���----------------------------
	(void)OS_SetIrqFunction( OS_IE_V_BLANK, INTR_VBlank );
	(void)OS_EnableIrqMask ( OS_IE_V_BLANK );
	(void)GX_VBlankIntr( TRUE );
	
	//---- �\���J�n
	GX_DispOn();
	GXS_DispOn();
	
	PrintStringS(  1, 0, YELLOW, "SaveData Clearner" );
	
	// SD�J�[�h���SPEC�t�@�C������A�^�[�Q�b�g��GameCode��ǂݍ���
#if USE_SDCARD
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
	
	// targetGameCode ���� titleID ���쐬
	{
		int i;
		u8 *pDst = (u8 *)&titleID;
		titleID = TITLEID_HI_USER_NAND;
		for( i = 0; i < 4; i++ ) {
			*pDst++ = targetGameCode[ 3 -i ];
		}
	}
	
	// �Z�[�u�f�[�^�T�[�`
	isFound = SearchTitle( titleID, &isPrivSave, &isPubSave );
	PrintStringS(  1, 6, WHITE, "InitialCode : %s", targetGameCode );
	if( isFound ) {
		PrintStringS(  1,  8, GREEN, "application found.", targetGameCode );
		PrintStringS(  1, 10, WHITE, "Press [A] to Cleanup START." );
		PrintStringS(  1, 11, WHITE, "Press [B] to STOP." );
	}else {
		PrintStringS(  1,  8, YELLOW, "application not found.", targetGameCode );
		*(u16 *)0x05000006 = myPalette[ YELLOW ][ 1 ];
	}
	SVC_WaitVBlankIntr();
	
	// ���C�����[�v----------------------------
	trg = PAD_Read();   // �p�b�h�f�[�^�ǂݎ��
	while( isFound ){
		// �p�b�h�f�[�^�ǂݎ��
		pad_old = pad;
		pad = PAD_Read();
		trg = (u16)( pad ^ pad_old );
		
		if( ( trg & ( PAD_BUTTON_A | PAD_BUTTON_B ) ) || isAutoExe ) {
			ClearRectangleS(  1, 10, 31, 1 );
			ClearRectangleS(  1, 11, 31, 1 );
		}
		if( ( trg & PAD_BUTTON_A ) || isAutoExe ) {
			// �Z�[�u�f�[�^�N���A���s
			// Private�Z�[�u�f�[�^
			PrintStringS(  1, 10, WHITE, "Private Save:" );
			if( isPrivSave ) {
				PrintStringS( 14, 10, YELLOW, "cleanup executing..." );
				if( CleanupSaveDataDrive( "otherPrv", titleID ) ) {
					PrintStringS( 14, 10, GREEN, "cleanup succeedded. " );
				}else {
					PrintStringS( 14, 10, RED,   "cleanup failed.     " );
					isFailed = TRUE;
				}
			}else {
				PrintStringS( 14, 10, YELLOW, "not existed." );
			}
			
			// Public�Z�[�u�f�[�^
			PrintStringS(  1, 11, WHITE, "Public  Save:" );
			if( isPubSave ) {
				PrintStringS( 14, 11, YELLOW, "cleanup executing." );
				if( CleanupSaveDataDrive( "otherPub",  titleID ) ) {
					PrintStringS( 14, 11, GREEN, "cleanup succeedded. " );
				}else {
					PrintStringS( 14, 11, RED,   "cleanup failed.     " );
					isFailed = TRUE;
				}
			}else {
				PrintStringS( 14, 11, YELLOW, "not existed." );
			}
			
			PrintStringS(  1, 12, WHITE, "Application :" );
			PrintStringS( 14, 12, YELLOW, "cleanup executing." );
			if( NAM_DeleteTitle( titleID ) == NAM_OK ) {
				PrintStringS( 14, 12, GREEN, "cleanup succeedded. " );
			}else {
				PrintStringS( 14, 12, RED,   "cleanup failed.     " );
				isFailed = TRUE;
			}
			if( isFailed ) {
				*(u16 *)0x05000006 = myPalette[ RED ][ 1 ];
			}else {
				*(u16 *)0x05000006 = myPalette[ LIGHTGREEN ][ 1 ];
			}
			break;
		}else if( trg & PAD_BUTTON_B ) {
			// �Z�[�u�f�[�^�N���A�L�����Z���B
			PrintStringS(  1, 10, YELLOW, "Cancel SaveData cleanup." );
			break;
		}
		SVC_WaitVBlankIntr();		// V�u�����N�����I���҂�
	}
	
	SVC_WaitVBlankIntr();
	OS_Terminate();
}

// V�u�����N���荞��
static void INTR_VBlank(void)
{
	//---- BG-VRAM�̍X�V
	DC_FlushRange( bg0BakM, sizeof(bg0BakM) );
	MI_CpuCopyFast ( bg0BakM, (void*)( HW_BG_VRAM    + BG0_SCREEN_BASE ), sizeof(bg0BakM) );
	DC_FlushRange( bg0BakS, sizeof(bg0BakS) );
	MI_CpuCopyFast ( bg0BakS, (void*)( HW_DB_BG_VRAM + BG0_SCREEN_BASE ), sizeof(bg0BakS) );
	//---- ���荞�݃`�F�b�N�t���O
	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}


// �q�[�v������
static void InitHeap( void )
{
    void*           tempLo;
    OSHeapHandle    hh;
	
    // ���C����������̃A���[�i�Ƀq�[�v���ЂƂ쐬
    tempLo = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 1);
    OS_SetArenaLo(OS_ARENA_MAIN, tempLo);
    hh = OS_CreateHeap(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi());
    if (hh < 0) {
        // �q�[�v�쐬�Ɏ��s�����ꍇ�ُ͈�I��
        OS_Panic("ARM9: Fail to create heap...\n");
    }
    (void)OS_SetCurrentHeap(OS_ARENA_MAIN, hh);
}


/*---------------------------------------------------------------------------*
    �֐���`
 *---------------------------------------------------------------------------*/

// �w��^�C�g�������݂��邩�m�F���A���݂���Ȃ�AsameMakerFlag �������Z�b�g���APrivate�Z�[�u�f�[�^�APublic�Z�[�u�f�[�^�̑��ݗL����Ԃ��B
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
//	pList->privateFlag[ index ]   |= 0x01 << shift;		// ���[�U�[�A�v���Ȃ�AprivateFlag�����Ȃ��͂��B
	*pIsPrivSave = ( pList->privateFlag[ index ] & ( 0x01 << shift ) ) ? TRUE : FALSE;
	*pIsPubSave  = ( pList->publicFlag [ index ] & ( 0x01 << shift ) ) ? TRUE : FALSE;
	return TRUE;
}


// �w�肳�ꂽ�Z�[�u�f�[�^�h���C�u�̃Z�[�u�f�[�^����
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
	
	// Private�Z�[�u�f�[�^���}�E���g
	if( FSi_MountSpecialArchive( titleID, pDriveName, &sOtherTitleWork) != FS_RESULT_SUCCESS ) {
		return FALSE;
	}
	
	// Private�Z�[�u�f�[�^�h���C�u��̑S�f�[�^�폜
	STD_TSPrintf( path, "%s:/", pDriveName );
	if( !DeleteDirectoryRecursively( path ) ) {
		OS_TPrintf( "%s delete recursively failed.\n", path );
		result = FALSE;
		goto END;
	}
	
	// �h���C�u�̋󂫗̈���擾
	if( !FS_GetArchiveResource( path, &rsc ) ) {
		OS_TPrintf( "FS_GetArchiveResource(%s) failed : code = %08x\n", path, FS_GetArchiveResultCode( &rsc ) );
		result = FALSE;
		goto END;
	}
	
	// �󂫗̈悪32bit�𒴂��Ă����ꍇ�̓G���[
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
	
	// �h���C�u�̋󂫃T�C�YMAX�ŁATEMP�t�@�C������
	if( !CreateFileWithLength( path, (u32)rsc.availableSize ) ) {
	    OS_TPrintf( "CreateFileWithLength(%s, %08x) failed.\n", path, (u32)rsc.availableSize );
		result = FALSE;
		goto END;
	}
	
	// �t�@�C�������_���t�B��
	if( !FillFileRandom( path ) ) {
	    OS_TPrintf( "FillFileRandom(%s) failed.\n", path );
		result = FALSE;
	}
	
	// �t�@�C���폜
	if( !FS_DeleteFile( path ) ) {
	    OS_TPrintf( "FS_DeleteFile(%s) failed.\n", path );
		result = FALSE;
	}
	
	// �ēx�h���C�u�̋󂫗̈���擾
	if( !FS_GetArchiveResource( path, &rsc2 ) ) {
		OS_TPrintf( "FS_GetArchiveResource(%s) failed : code = %08x\n", path, FS_GetArchiveResultCode( &rsc2 ) );
		result = FALSE;
	}
	
#if MY_DEBUG
	(void)CreateFileWithLength( path, (u32)rsc.availableSize );
	(void)DumpFile( path );
	(void)FS_DeleteFile( path );
#endif
	
	// �󂫃T�C�Y���������`�F�b�N
	OS_TPrintf( "Before available size : %016llx\n", rsc.availableSize );
	OS_TPrintf( "After  available size : %016llx\n", rsc2.availableSize );
	if( rsc.availableSize != rsc2.availableSize ) {
		OS_TPrintf( "available size error.\n" );
		result = FALSE;
	}
	
END:
	// �Z�[�u�f�[�^�̃}�E���g����
	(void)FSi_MountSpecialArchive(0, NULL, &sOtherTitleWork);
	
	return result;
}


// �w��p�X�ȉ��̃f�B���N�g��������
BOOL DeleteDirectoryRecursively( const char *pPath )
{
    FSFile  dir;
    FSDirectoryEntryInfo entryInfo;
    BOOL ret = TRUE;
	char path[ FS_ENTRY_LONGNAME_MAX ];
	
    FS_InitFile( &dir );
	
    // �����Ŏw�肳�ꂽ�f�B���N�g�����J��
    if ( !FS_OpenDirectory( &dir, pPath, FS_FILEMODE_R ) ) {
        OS_TWarning( "Fail! FS_OpenDirectory(%s) in %s\n", pPath, __func__ );
        return FALSE;
    }
	
    // �f�B���N�g���̒��g��ǂ�
    while ( FS_ReadDirectory( &dir, &entryInfo ) )
    {
        if ( STD_CompareString( entryInfo.longname, "." )  == 0 ||
             STD_CompareString( entryInfo.longname, ".." ) == 0 ) {
            continue;
        }
		
        STD_CopyLString( path, pPath, FS_ENTRY_LONGNAME_MAX );
        STD_ConcatenateLString( path, "/", FS_ENTRY_LONGNAME_MAX );
        STD_ConcatenateLString( path, entryInfo.longname, FS_ENTRY_LONGNAME_MAX );
		
        // �f�B���N�g��
        if (entryInfo.attributes & FS_ATTRIBUTE_IS_DIRECTORY) {
            if (!FS_DeleteDirectoryAuto(path)) {
                ret = FALSE;
                OS_TWarning( "Fail! FS_DeleteDirectoryAuto(%s) in %s\n", path, __func__ );
            }
		}else {
		// �t�@�C��
            if ( !FS_DeleteFileAuto( path ) ) {
                ret = FALSE;
                OS_TWarning( "Fail! FS_DeleteFileAuto(%s) in %s\n", path, __func__ );
            }
        }
    }

    // �f�B���N�g�������
    FS_CloseDirectory( &dir );

    return ret;
}


// �w��t�@�C�����w��T�C�Y�ō쐬
BOOL CreateFileWithLength( char *path, u32 length )
{
	FSFile file;
	BOOL result = TRUE;
	
	// Private�Z�[�u�f�[�^�h���C�u���TEMP�t�@�C���쐬
	if ( !FS_CreateFile( path, FS_PERMIT_R | FS_PERMIT_RW ) )
	{
	    OS_TPrintf( "FS_CreateFile(%s) failed.\n", path );
		return FALSE;
	}
	
	// TEMP�t�@�C���I�[�v��
	FS_InitFile(&file);
	if ( !FS_OpenFileEx( &file, path, FS_FILEMODE_W ) )
	{
	    OS_TPrintf( "FS_OpenFile(%s) failed.\n", path );
		result = FALSE;
		goto END;
	}
	
	// �t�@�C���T�C�Y���Z�b�g
	if ( FS_SetFileLength( &file, length ) != FS_RESULT_SUCCESS )
	{
	    OS_TPrintf( "FS_SetFileLength(%s) failed.\n", path );
		result = FALSE;
	}
	
END:
	// �t�@�C���N���[�Y
	(void)FS_CloseFile( &file );
	
	return result;
}


// �w��t�@�C���������_���f�[�^�Ńt�B��
BOOL FillFileRandom( const char* pPath )
{
    u64 seed;
    MATHRandContext32 rndctx;
    u32 *pBuffer = (u32 *)sStreamBuffer;
    s32 result = TRUE;
    FSFile f;
	
	// ������SEED�ݒ�
    if( AES_Rand( &seed, sizeof(seed) ) != AES_RESULT_SUCCESS ) {
        return FALSE;
    }
    MATH_InitRand32( &rndctx, seed );
	
	// �t�@�C���t�B��
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


// �w��t�@�C�����X�g���[���o�b�t�@�ɓǂݎ̂�
BOOL DumpFile( const char* pPath )
{
    u32 *pBuffer = (u32 *)sStreamBuffer;
    s32 result = TRUE;
    FSFile f;
	
	// �t�@�C���t�B��
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


// SD�J�[�h�̃��[�g�f�B���N�g����̎w��t�@�C�����o�b�t�@�ɓǂݍ���
BOOL ReadTargetFileFromSD( const char *pFilename, char **ppDst, u32 *pFileSize )
{
    FSFile  dir, file;	
    BOOL    open_is_ok;
	BOOL    read_is_ok;
	u32  allocSize;
	BOOL result = TRUE;
	char fullPath[FS_ENTRY_LONGNAME_MAX+6];
	
	// �o�b�t�@�̃N���A
	MI_CpuClear8( fullPath, sizeof(fullPath) );
	
    // SD�J�[�h�̃��[�g�f�B���N�g��������
	FS_InitFile(&dir);
    if ( !FS_OpenDirectory(&dir, "sdmc:/", FS_FILEMODE_R) )
    {
		OS_TPrintf("Error FS_OpenDirectory(sdmc:/)");
    }
    else
    {
        FSDirectoryEntryInfo   info[1];
		// ���[�g�f�B���N�g����Ń^�[�Q�b�g�t�@�C����T��
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

	// �t�@�C���I�[�v��
    FS_InitFile(&file);
    open_is_ok = FS_OpenFile(&file, fullPath);
	if (!open_is_ok)
	{
    	OS_TPrintf( "FS_OpenFile(\"%s\") ... ERROR!\n", fullPath);
		return FALSE;
	}

	// �t�@�C�����[�h
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

