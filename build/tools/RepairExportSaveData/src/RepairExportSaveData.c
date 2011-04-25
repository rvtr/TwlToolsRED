/*---------------------------------------------------------------------------*
  Project:  RepairExportSaveData
  File:     RepairExportSaveData.c

  Copyright 2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date::            $
  $Rev$
  $Author$
 *---------------------------------------------------------------------------*/
#include <twl.h>
#include <twl/nam.h>
#include "RepairExportSaveData.h"
#include "common.h"
#include "screen.h"

// define data------------------------------------------
typedef enum DetectError
{
	DETECT_ERROR_NONE = 0,
	DETECT_ERROR_DIR_OPEN = 1,
	DETECT_ERROR_RESCUE_DATA = 2,
	DETECT_ERROR_VERIFY_DATA = 3,
	DETECT_ERROR_MAX
}
DetectError;

typedef enum RunningMode
{
	MODE_WRITABLE = 0,
	MODE_NOSD = 1,
	MODE_PROC = 2,
    MODE_RESULT = 3,
	MODE_MAX
}
RunningMode;

typedef enum SDStat
{
	SDSTAT_INSERTED = 0,
	SDSTAT_REMOVED = 1,
	SDSTAT_MAX
}
SDStat;

// extern data------------------------------------------

// function's prototype declaration---------------------
static void MenuScene( void );
DetectError ResucuSDSaveData( void );
static void* AllocForNAM(u32 size);
static void FreeForNAM(void* ptr);

// global variable -------------------------------------

// static variable -------------------------------------
static DetectError s_result;

static char s_mode = 0;
static s32 s_SDCheck = 0;
static BOOL s_protected = FALSE;
static BOOL s_protect_checked = FALSE;
static FSEventHook s_hook;

static s32 s_debug = 0;

static KeyInfo  gKey;

// const data  -----------------------------------------
static const char *s_result_message[ DETECT_ERROR_MAX ] =
{
	"Success",
	"Directory Access Error",
	"Rescue Save Data Error",
	"Verify Save Data Error"
};


//******************************************************************************
//      SD�����Ď��R�[���o�b�N
//******************************************************************************
static void subCallBack( void* userdata, FSEvent event, void* argument )
{
#pragma unused( userdata, argument )

    if( event == FS_EVENT_MEDIA_INSERTED )
    {
        s_SDCheck = SDSTAT_INSERTED;  //�}��
        s_protect_checked = FALSE;
    }
    else if( event == FS_EVENT_MEDIA_REMOVED )
    {
        s_SDCheck = SDSTAT_REMOVED;  //����
    }
}

static BOOL IsSDProtected( void )
{
	FSPathInfo info;
	if( TRUE == FS_GetPathInfo("sdmc:/", &info) )
	{
		if( info.attributes & FS_ATTRIBUTE_IS_PROTECTED )
		{
			return TRUE;
		}
	}
	return FALSE;
}

static FSResult IsSDExist( void )
{
	FSPathInfo info;
	if( TRUE == FS_GetPathInfo("sdmc:/", &info) )
	{
		return FS_RESULT_SUCCESS;
	}else
	{
		return FS_GetArchiveResultCode("sdmc:/");
	}
}

static BOOL IsCardRemoved( void )
{
	// SD �J�[�h�����������A�����Ȃ�v���e�N�g������������A
	// SD �J�[�h��������Ȃ����[�h��
	if( ( s_SDCheck == SDSTAT_REMOVED ) || s_protected )
	{
		s_mode = MODE_NOSD;
		return TRUE;
	}
	return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         DrawMainScene

  Description:  �`��܂Ƃ�

  �����F
  0xf0, // ��			0xf1, // ��				0xf2, // ��
  0xf3, // ��			0xf4, // ��				0xf5, // �s���N
  0xf6, // ���F			0xf7, // �����񂾐�		0xf8, // �����񂾗�
  0xf9, // �����񂾐� 	0xfa, // �����񂾉��F	0xfb, // ��
  0xfc, // ��������		0xfd, // �D�F			0xfe, // �Z���D�F
 *---------------------------------------------------------------------------*/
static void DrawMainScene( void )
{
    PutMainScreen( 2, 2, 0xff, "+--------------------------+");
    PutMainScreen( 2, 3, 0xff, "+                          +");
	PutMainScreen( 2, 4, 0xff, "+ Repair Exported SaveData +");
    PutMainScreen( 2, 5, 0xff, "+                          +");
    PutMainScreen( 2, 6, 0xff, "+--------------------------+");

	// ���[�h���Ƃ̕`�揈��
	switch( s_mode )
	{
		case MODE_WRITABLE:
			// ���C�g���[�h
			PutMainScreen( 3, 12, 0xf6, "Press A Button to start.");
			break;
		case MODE_NOSD:
			// SD �J�[�h��������Ȃ����[�h
			if( ( s_SDCheck == SDSTAT_INSERTED ) && s_protected)
			{
				PutMainScreen( 1, 12, 0xf1, "SD card is locked.");
				PutMainScreen( 1, 14, 0xf1, "Please insert unlocked SD card.");
			}else
			{
				PutMainScreen( 3, 12, 0xf1, "Please insert SD card.");
			}
			break;
		case MODE_PROC:
            PutMainScreen( 3, 12, 0xf4, "Now Accessing SD card");
            PutMainScreen( 3, 14, 0xf4, "Please Do not Touch SD card");
			break;
        case MODE_RESULT:
                if( s_result == DETECT_ERROR_NONE )
                {
                    PutMainScreen( 5, 12, 0xf2, "%s", s_result_message[s_result]);
                }
                else
                {
                    PutMainScreen( 6, 12, 0xf1, "%s", s_result_message[s_result]);
                    PutMainScreen( 7, 14, 0xf1, "Error Code : %d", s_debug);
                }
            break;
		default:
			break;
	}
}


/*---------------------------------------------------------------------------*
  Name:         CheckCard

  Description:  
 *---------------------------------------------------------------------------*/
static void CheckCard( void )
{
	return;
}


/*---------------------------------------------------------------------------*
  Name:         RepairSaveDataInit

  Description:  ������
 *---------------------------------------------------------------------------*/
void RepairSaveDataInit( void )
{
    NAM_Init(AllocForNAM, FreeForNAM);
    
	FS_Init(3);

    OS_InitTick();

    {
		FSResult fsr = IsSDExist();
		s_protected = IsSDProtected();
		
		if( fsr == FS_RESULT_SUCCESS )
		{
			s_SDCheck = SDSTAT_INSERTED;
		}else
		{
			s_SDCheck = SDSTAT_REMOVED;
		}
		
		if( ( fsr == FS_RESULT_SUCCESS ) && !s_protected )
		{
			// �������ݏ������[
			s_mode = MODE_WRITABLE;
		}else
		{
			// SD �J�[�h���������v���e�N�g����Ă�
			s_mode = MODE_NOSD;
		}
		
		// �������� SD �J�[�h�}�������͂��߂�
		FS_RegisterEventHook("sdmc", &s_hook, subCallBack, NULL);
    }
    
	// �\��
	DrawMainScene();
}


/*---------------------------------------------------------------------------*
  Name:         RepairSaveDataMain

  Description:  ���C�����[�v
 *---------------------------------------------------------------------------*/
void RepairSaveDataMain(void)
{
    OSIntrMode intrmode;
    
	// �ĕ\��
	DrawMainScene();
    
	// SD �J�[�h���h���������񂾂��v���e�N�g�`�F�b�N����
	// �Ăюh����܂Ń`�F�b�N���Ȃ�

	// ���荞�݋֎~
	intrmode = OS_DisableInterrupts();
	if( !s_protect_checked )
	{
		s_protect_checked = TRUE;
		// �����Ŋ��荞�݋֎~�������Ȃ��Ƃ������Ă��Ȃ��Ȃ�
		OS_RestoreInterrupts( intrmode );
		s_protected = IsSDProtected();
	}else
	{
		// ������ł����荞�݋֎~����
		OS_RestoreInterrupts( intrmode );
	}

    ReadKey(&gKey);
    
	// ���[�h���Ƃ̃��C�������i�L�[�����A���[�h�J�ڂȂǁj
	switch( s_mode )
	{
        // ���C�g���[�h
		case MODE_WRITABLE:
			// �J�[�h�ASD�J�[�h�������o�Ƌ��ʃ��[�h�`�F���W
			if ( IsCardRemoved() == TRUE )
			{
				break;
			}
			
			// �L�[���͂�����Γǂݍ��݃X���b�h�N�����ă��C�e�B���O���[�h��
			if( gKey.trg == PAD_BUTTON_A )
			{
				//OS_CreateThread( &readwrite_thread, SaveCardToSD, NULL, stack+STACK_SIZE/sizeof(u64), STACK_SIZE, THREAD_PRIO );
				//OS_WakeupThreadDirect( &readwrite_thread );
				s_mode = MODE_PROC;
				break;
			}
			
			break;
        
        // SD �J�[�h��������Ȃ����[�h
		case MODE_NOSD:
			// SD �J�[�h���h�������烉�C�g���[�h��
			// �������v���e�N�g���������Ă���J�ڂ��Ȃ�
			if( s_SDCheck == SDSTAT_INSERTED )
			{
				if( !s_protected )
				{
					s_mode = MODE_WRITABLE;
					break;
				}
			}
			
			break;

        // �������[�h
		case MODE_PROC:
            s_result = ResucuSDSaveData();

            // SD�J�[�h�������o
			if ( IsCardRemoved() == TRUE )
			{
				// �Ƃ肠�����ǂݍ��݃X���b�h�I����҂��Ă���I���
				// �������Ȃ��Ɠǂݍ��ݗp�X���b�h�������Ă��܂��\��
				//while( !OS_IsThreadTerminated( &readwrite_thread ) ){}
				break;
			}
			
			// �ǂݍ��݃X���b�h���I�������烉�C�g���[�h��
			/*if( OS_IsThreadTerminated( &readwrite_thread ) )
			{
				s_mode = MODE_WRITABLE;
				s_read_count = 0; // �ǂݍ��݃X���b�h�I����Ă���łȂ��Ƃ�������邢
				break;
			}*/

        /*
            if( gKey.trg == PAD_BUTTON_A )
            {
                s_mode = MODE_RESULT;
            }
          */
            s_mode = MODE_RESULT;
			break;
        
        // ���ʃ��[�h
		case MODE_RESULT:
            break;
		default:
			break;
	}
}


/*---------------------------------------------------------------------------*
  Name:         ResucuSDSaveData

  Description:  SD�J�[�h�ɑޔ����ꂽ�A�v���̃Z�[�u�f�[�^�̏�����t���ւ���
 *---------------------------------------------------------------------------*/
DetectError ResucuSDSaveData( void )
{
    FSFile file;
    FSDirectoryEntryInfo dir;
    DetectError retval = DETECT_ERROR_NONE;
    
    FS_InitFile(&file);
    
    if( FS_OpenDirectory(&file, "sdmc:/private/ds/title", FS_PERMIT_R ) )
    {
        OS_TPrintf("Open Directory Fail...\n");
    }
    
    while( FS_ReadDirectory(&file, &dir) )
    {
        char* pExtension;
        char path_buf[FS_ENTRY_LONGNAME_MAX];

        // �g���q�� .bin �̂��̂����������s��
        pExtension = STD_SearchCharReverse( dir.longname, '.');
        if( pExtension )
        {
            if(!STD_CompareString( pExtension, ".bin"))
            {
                STD_TSNPrintf( path_buf, FS_ENTRY_LONGNAME_MAX, "sdmc:/private/ds/title/%s", dir.longname );
                
                OS_TPrintf("%s\n", path_buf);

                {
                    FSFile tempFile;
                    BOOL result;
                    
                    FS_InitFile(&tempFile);
                    result = FS_OpenFileEx( &tempFile, path_buf, FS_FILEMODE_R);

                    if( result )
                    {
                        OS_TPrintf("opne success\n");
                    }
                    FS_CloseFile(&tempFile);
                }

                s_debug = NAM_RescueBkp( path_buf );

                // �Z�[�u�f�[�^�̋~�Ϗ��u
                if(s_debug == NAM_OK)
                {
                    // ����������������x���t�@�C
                    if(NAM_VerifyBkpStrict( path_buf ) != NAM_OK)
                    {
                        retval = DETECT_ERROR_VERIFY_DATA;
                        break;
                    }
                }
                else
                {
                    retval = DETECT_ERROR_RESCUE_DATA;
                    break;
                }
            }
        }
    }
    
    FS_CloseDirectory(&file);

    return retval;
}


static void* AllocForNAM(u32 size)
{
	void* ptr;
	ptr = OS_AllocFromMain(size);
	
	if (ptr == NULL)
	{
		OS_Panic("alloc failed.");
	}
	
	return ptr;
	
}

static void FreeForNAM(void* ptr)
{
	OS_FreeToMain(ptr);
}

