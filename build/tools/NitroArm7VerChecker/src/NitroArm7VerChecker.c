/*---------------------------------------------------------------------------*
  Project:  NitroArm7VerChecker
  File:     CooperationA.c

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

// ----- �r���h���[�h�̐؂�ւ� -----
// �ʏ�ŁE����ł̐؂�ւ��͂����̒�`�ōs��
#define NORMAL_CHECK_TOOL

// �f�o�b�O�r���h�؂�ւ�(�L���ɂ���ꍇ�͂ǂ��炩�Е���)
//#define DEBUG_SHOW_SDK_INFO


#include <twl.h>
#include <nitro/os/ARM9/argument.h>
#include <twl/os/common/format_rom.h>
#include <twl/os/common/banner.h>
#include "misc_simple.h"
#include "NitroArm7VerChecker.h"
#include "revision.h"
#include <../build/libraries/mb/common/include/mb_fileinfo.h>
#include <string.h>

#ifdef NORMAL_CHECK_TOOL
#include "SdkInfoData_normal.h"
#else
#include "SdkInfoData_special.h"
#endif

// define data------------------------------------------
#define SHOW_SDK_INFO_NUM       20
#define SHOW_SDK_INFO_SIZE      50

#ifdef DEBUG_SHOW_SDK_INFO
#define KEY_REPEAT_START        10  // �L�[���s�[�g�J�n�܂ł̃t���[����
#define KEY_REPEAT_SPAN         1   // �L�[���s�[�g�̊Ԋu�t���[����
#define DUMP_SIZE               0x80
#endif

#define THREAD_PRIO 17
#define STACK_SIZE  0x2000

typedef enum DetectError
{
	DETECT_ERROR_NONE = 0,
	DETECT_ERROR_ARM7FLX_SIZE_OVER = 1,
	DETECT_ERROR_ARM7FLX_SIZE_ZERO = 2,
	DETECT_ERROR_NO_HIT = 3,
	DETECT_ERROR_MAX
}
DetectError;

typedef enum RunningMode
{
	MODE_NOCARD = 0,
	MODE_PROC = 1,
    MODE_RESULT = 2,
	MODE_MAX
}
RunningMode;

#ifdef DEBUG_SHOW_SDK_INFO
typedef struct KeyInfo
{
    u16 cnt;    // �����H���͒l
    u16 trg;    // �����g���K����
    u16 up;     // �����g���K����
    u16 rep;    // �����ێ����s�[�g����
} KeyInfo;
#endif

// extern data------------------------------------------

// function's prototype declaration---------------------

static void MenuScene( void );
static void SplitToken(void);
static void CheckCard( void *arg );
#ifdef DEBUG_SHOW_SDK_INFO
void ReadKey(KeyInfo* pKey);
#endif

// global variable -------------------------------------

// static variable -------------------------------------
static char s_mode;
static BOOL s_secret = FALSE;
static BOOL s_show_error = FALSE;
static u8 s_error = 0;
static u8 s_digest[MATH_SHA1_DIGEST_SIZE];
static u32 s_hit = 0;
static char s_sdk_name[SHOW_SDK_INFO_NUM][SHOW_SDK_INFO_SIZE];
static int s_same_sdk_num;

static OSThread s_thread;
static u64 s_stack[ STACK_SIZE / sizeof(u64) ];

#ifdef DEBUG_SHOW_SDK_INFO
static KeyInfo  gKey;
static u32 gIndex;
static void *data_buf;
#endif

// const data  -----------------------------------------

static const ROM_Header_Short *sp_header = (ROM_Header_Short *)HW_CARD_ROM_HEADER;

/*---------------------------------------------------------------------------*
  Name:         HOTSW_IsCardExist

  Description:  SCFG_MC1��CDET�t���O�����āA�J�[�h�̑��ݔ�����s��
 *---------------------------------------------------------------------------*/
static BOOL HOTSW_IsCardExist(void)
{
    if( !(reg_MI_MC1 & REG_MI_MC_SL1_CDET_MASK) ){
        return TRUE;
    }
    else{
        return FALSE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         GetMcSlotMode

  Description:  �X���b�g�̌��݂̃��[�h��Ԃ�
 *---------------------------------------------------------------------------*/
static u32 GetMcSlotMode(void)
{
    return reg_MI_MC1 & REG_MI_MC_SL1_MODE_MASK;
}

/*---------------------------------------------------------------------------*
  Name:         HOTSW_IsCardAccessible

  Description:  SCFG_MC1��CDET�t���O��M(���[�h)�����āA�J�[�h�X���b�g�ɃA�N�Z�X�ł����Ԃ����肷��
 *---------------------------------------------------------------------------*/
static BOOL HOTSW_IsCardAccessible(void)
{
    if( HOTSW_IsCardExist() && (GetMcSlotMode() == SLOT_STATUS_MODE_10)){
        return TRUE;
    }
    else{
        return FALSE;
    }
}


//======================================================
// ARM7�R���|�[�l���g�Ɏg���Ă���SDK�o�[�W�����̃`�F�b�N�v���O����
//======================================================
/*---------------------------------------------------------------------------*
  Name:         DrawMainScene

  Description:  �`��܂Ƃ�
 *---------------------------------------------------------------------------*/
#define CHANGE_INTERVAL       3
#define PROC_MARK_BASE_X     27
#define PROC_MARK_BASE_Y     20
static void DrawMainScene( void )
{
    static u8 count = 0;
    
	myDp_Printf( 0, 7, TXT_COLOR_BLACK, SUB_SCREEN, "Component SDK Version Identifier");

#ifdef DEBUG_SHOW_SDK_INFO
    myDp_Printf( 0, 1, TXT_COLOR_BLUE,  MAIN_SCREEN, "s_same_sdk_num : %d", s_same_sdk_num);
    myDp_Printf( 0, 2, TXT_COLOR_BLUE,  MAIN_SCREEN, "s_hit : %d", s_hit);
    {
        int i;
        for( i = 0; i < s_same_sdk_num; i++ )
        {
            if( s_sdk_name[i][0] == '2' || s_sdk_name[i][0] == '3' || s_sdk_name[i][0] == '4' || s_sdk_name[i][0] == '5' )
            {
                myDp_Printf( 1, 5+i, TXT_COLOR_BLUE,  MAIN_SCREEN, "SDK %s", s_sdk_name[i]);
            }
            else
            {
                myDp_Printf( 5, 5+i, TXT_COLOR_BLACK,  MAIN_SCREEN, "[%s", s_sdk_name[i]);
            }
        }
        if( s_show_error )
        {
            myDp_Printf( 1, 23, TXT_COLOR_RED,  MAIN_SCREEN, "Error : Can Not Show SDK Info");
        }
    }
#endif

    switch( s_mode )
    {
        // �J�[�h�Ȃ����
      case MODE_NOCARD:
		// �J�[�h�������[�h
        myDp_Printf( 1,11, TXT_COLOR_BLACK, SUB_SCREEN, "Please insert DS game card.");
        break;

        // ���������
      case MODE_PROC:
        count++;

        if( count >= 0 && count < CHANGE_INTERVAL )
        {
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y  , TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "+++");
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+1, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "++");
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+2, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "+");
        }
        else if( count >= CHANGE_INTERVAL && count < CHANGE_INTERVAL*2 )
        {
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y  , TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "+++");
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+1, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "+++");
        }
        else if( count >= CHANGE_INTERVAL*2 && count < CHANGE_INTERVAL*3 )
        {
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y  , TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "+++");
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+1, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, " ++");
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+2, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "  +");
        }
        else if( count >= CHANGE_INTERVAL*3 && count < CHANGE_INTERVAL*4 )
        {
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y  , TXT_COLOR_LIGHTBLUE, SUB_SCREEN, " ++");
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+1, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, " ++");
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+2, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, " ++");
        }
        else if( count >= CHANGE_INTERVAL*4 && count < CHANGE_INTERVAL*5 )
        {
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y  , TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "  +");
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+1, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, " ++");
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+2, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "+++");
        }
        else if( count >= CHANGE_INTERVAL*5 && count < CHANGE_INTERVAL*6 )
        {
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+1, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "+++");
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+2, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "+++");
        }
        else if( count >= CHANGE_INTERVAL*6 && count < CHANGE_INTERVAL*7 )
        {
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y  , TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "+");
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+1, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "++");
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+2, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "+++");
        }
        else if( count >= CHANGE_INTERVAL*7 && count < CHANGE_INTERVAL*8)
        {
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y  , TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "++");
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+1, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "++");
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+2, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "++");
        }
        if( count == CHANGE_INTERVAL*8 - 1 )
        {
            count = 0;
        }
        break;
        
        // ���ʕ\�����
      case MODE_RESULT:
        if( s_error != DETECT_ERROR_NONE )
		{
			// �G���[�\��
			myDp_Printf( 1,10, TXT_COLOR_RED, SUB_SCREEN, "Registered ARM7 component");
			myDp_Printf( 1,11, TXT_COLOR_RED, SUB_SCREEN, "was not detected.");
			myDp_Printf( 1,13, TXT_COLOR_RED, SUB_SCREEN, "Error : %d", s_error);
		}
        else
		{
			// �ʏ팋�ʕ\��
			myDp_Printf( 1,10, TXT_COLOR_DARKGREEN, SUB_SCREEN, "Registered ARM7 component");
			myDp_Printf( 1,11, TXT_COLOR_DARKGREEN, SUB_SCREEN, "was detected.");
            
			myDp_Printf( 1, 1, TXT_COLOR_BLACK, MAIN_SCREEN, "GameCode  :");
            myDp_Printf(13, 1, TXT_COLOR_BLUE,  MAIN_SCREEN, "%c%c%c%c", sp_header->game_code[0],sp_header->game_code[1],sp_header->game_code[2],sp_header->game_code[3]);
            myDp_Printf( 1, 3, TXT_COLOR_BLACK, MAIN_SCREEN, "TitleName :");
            myDp_Printf(13, 3, TXT_COLOR_BLUE,  MAIN_SCREEN, "%s", sp_header->title_name);

            {
                int i;
                for( i = 0; i < s_same_sdk_num; i++ )
                {
                    if( s_sdk_name[i][0] == '2' || s_sdk_name[i][0] == '3' || s_sdk_name[i][0] == '4' || s_sdk_name[i][0] == '5' )
                    {
                        myDp_Printf( 1, 5+i, TXT_COLOR_BLUE,  MAIN_SCREEN, "SDK %s", s_sdk_name[i]);
                    }
                    else
                    {
                        myDp_Printf( 5, 5+i, TXT_COLOR_BLACK,  MAIN_SCREEN, "[%s", s_sdk_name[i]);
                    }
                }
                if( s_show_error )
                {
                    myDp_Printf( 1, 23, TXT_COLOR_RED,  MAIN_SCREEN, "Error : Can Not Show SDK Info");
                }
            }
		}
        break;

      default:
        break;
    }

    // �B�� ID �\��
	if ( s_secret )
	{
		myDp_Printf( 1, 20, TXT_COLOR_BLUE, SUB_SCREEN, "ID : %d", ID_NUM);
	}
}


/*---------------------------------------------------------------------------*
  Name:         CheckCard

  Description:  
 *---------------------------------------------------------------------------*/
static void CheckCard(  void *arg  )
{
#pragma unused( arg )
    
	void *sp_arm7flx;
	void *old_lo;
	int m;

    CARD_Enable( TRUE );

	// �ǂݍ��݃o�b�t�@�̓A���[�iLo����Hi�܂ł̊Ԃ�K���Ɏg���ăA���[�iLo���ړ������悤
	old_lo = OS_GetMainArenaLo();
	sp_arm7flx = (void *)( MATH_ROUNDUP32( (int)OS_GetMainArenaLo() ) );
	OS_SetMainArenaLo( OS_GetMainArenaHi() ); // �O�̂��߈�U�[�����Ɋ񂹂Ă���

	CARD_LockRom( 0x03 ); // ID �͓K��
	
	// �w�b�_�̃`�F�b�N�i���Ȃ�j
	// ARM7�̃T�C�Y�`�F�b�N
	if( sp_header->sub_size > BOOTABLE_SIZE_ARM7 )
	{
		// �G���[
		s_error = DETECT_ERROR_ARM7FLX_SIZE_OVER;
		return;
	}else if ( sp_header->sub_size == 0 )
	{
		s_error = DETECT_ERROR_ARM7FLX_SIZE_ZERO;
		return;
	}

	OS_SetMainArenaLo( (void *)((u32)sp_arm7flx + sp_header->sub_size) ); // �A���[�iLo�C��

    // ARM9��Secure��������������ǂݏo��
    if( (sp_header->main_rom_offset + sp_header->main_size) > 0x8000 )
    {
        // Game�̈��ARM9�풓���W���[����
        CARD_ReadRom( MI_DMA_NOT_USE, (void *)0x8000, sp_arm7flx, (sp_header->main_size - (0x8000 - sp_header->main_rom_offset)) );
    }
	CARD_ReadRom( MI_DMA_NOT_USE, (void *)sp_header->sub_rom_offset, sp_arm7flx, sp_header->sub_size );
	CARD_UnlockRom( 0x03 );
    
	// Arm7FLX �̃n�b�V���v�Z
	MATH_CalcSHA1( s_digest, sp_arm7flx, sp_header->sub_size );
    
	// �Y������ SDK ������
	for( s_hit=0;s_hit<SDK_INFO_NUM;s_hit++ )
	{
		for( m=0;m<MATH_SHA1_DIGEST_SIZE;m++ )
		{
			if( s_sdk_info[s_hit].hash[m] != s_digest[m] ) break;
		}
		if( m == MATH_SHA1_DIGEST_SIZE ) break;
	}
    
	if( s_hit == SDK_INFO_NUM )
	{
		s_error = DETECT_ERROR_NO_HIT;
		//s_hit = 0;
		return;
	}
    else
    {
        SplitToken();
    }

	// �����o�b�t�@����Ȃ�
	OS_SetMainArenaLo( old_lo );
    
	s_error = 0;
	return;
}


/*---------------------------------------------------------------------------*
  Name:         SplitToken

  Description:  �\���p��SDK���쐬
 *---------------------------------------------------------------------------*/
static void SplitToken(void)
{
    char *pos;
    Arm7Info temp;

    s_same_sdk_num = 0;
    s_show_error = FALSE;

    // strtok �� ���̔z����󂵂Ă��܂�(token��null�ŏ��������Ă��܂�)�̂ŁAtemp�ō�Ƃ���
    memcpy( &temp, &s_sdk_info[s_hit], sizeof(Arm7Info));
    pos = strtok( temp.name, "/[" );

    while( pos != NULL )
    {
        if( strlen(pos) < SHOW_SDK_INFO_SIZE )
        {
            strcpy( s_sdk_name[s_same_sdk_num], pos);
        }
        else
        {
            s_show_error = TRUE;
            break;
        }
        
        s_same_sdk_num++;

        if( s_same_sdk_num >= SHOW_SDK_INFO_NUM )
        {
            s_show_error = TRUE;
            break;
        }
        
        pos = strtok(NULL, "/[");
    }
}


/*---------------------------------------------------------------------------*
  Name:         NitroArm7VerCheckerInit

  Description:  ������
 *---------------------------------------------------------------------------*/
void NitroArm7VerCheckerInit( void )
{
	GX_DispOff();
	GXS_DispOff();

    GXS_SetVisiblePlane( GX_PLANEMASK_BG0 );
	
	GX_DispOn();
	GXS_DispOn();
	
	FS_Init(3);
    OS_InitTick();

    s_mode = MODE_NOCARD;
    
	// ���̎��_�ŃJ�[�h�����݂��Ă�����J�[�h�`�F�b�N�J�n
	if( HOTSW_IsCardExist() && HOTSW_IsCardAccessible() )
	{
        OS_TPrintf("Card Exist\n");
        OS_CreateThread( &s_thread, CheckCard, NULL, s_stack+STACK_SIZE/sizeof(u64), STACK_SIZE, THREAD_PRIO );
        OS_WakeupThreadDirect( &s_thread );
		s_mode = MODE_PROC;
	}

	// �\��
	DrawMainScene();
}

#define PAD_SECRET ( PAD_BUTTON_START | PAD_BUTTON_X | PAD_BUTTON_Y )


/*---------------------------------------------------------------------------*
  Name:         NitroArm7VerCheckerMain

  Description:  ���C�����[�v
 *---------------------------------------------------------------------------*/
void NitroArm7VerCheckerMain(void)
{
	//--------------------------------------
	//  �L�[���͏���
	//--------------------------------------
	// SLEEP + X + Y + START �ŉB�� ID �\��
	if( PAD_DetectFold() && 
		( gKeyWork.press == PAD_SECRET )
	  )
	{
		s_secret = TRUE;
	}else
	{
		s_secret = FALSE;
	}


#ifdef DEBUG_SHOW_SDK_INFO
    {
        BOOL change = FALSE;
        ReadKey(&gKey);

        if (gKey.rep & PAD_KEY_UP || gKey.trg & PAD_KEY_UP)
        {
            if( s_hit < SDK_INFO_NUM )
            {
                s_hit++;
                change = TRUE;
            }
            else if( s_hit == SDK_INFO_NUM )
            {
                s_hit = 0;
                change = TRUE;
            }
        }
        if (gKey.rep & PAD_KEY_DOWN || gKey.trg & PAD_KEY_DOWN)
        {
            if( s_hit > 0 )
            {
                s_hit--;
                change = TRUE;
            }
            else if( s_hit == 0 )
            {
                s_hit = SDK_INFO_NUM;
                change = TRUE;
            }
        }

        if( change == TRUE )
        {
            SplitToken();
        }
    }
#endif
    
	// �ĕ\��
	DrawMainScene();

    switch( s_mode )
    {
        // �J�[�h�Ȃ����[�h
      case MODE_NOCARD:
        // �J�[�h���h�������̂����o������ċN��
		if( HOTSW_IsCardExist() == TRUE ) {
			OS_DoApplicationJump( OS_GetTitleId(), OS_APP_JUMP_NORMAL );
		}
        break;

        // ���������[�h
      case MODE_PROC:
        if( OS_IsThreadTerminated( &s_thread ) )
        {
            s_mode = MODE_RESULT;
        }
        break;

        // ���ʕ\�����[�h
      case MODE_RESULT:
		// �J�[�h���������̂����o������J�[�h�������[�h��
		if( HOTSW_IsCardExist() == FALSE )
		{
			s_mode = MODE_NOCARD;
		}
        break;

      default:
        break;
    }
}


/*---------------------------------------------------------------------------*
  Name:         ReadKey

  Description:  �L�[���͏����擾���A���͏��\���̂�ҏW����B
                �����g���K�A�����g���K�A�����p�����s�[�g�g���K �����o����B

  Arguments:    pKey  - �ҏW����L�[���͏��\���̂��w�肷��B

  Returns:      None.
 *---------------------------------------------------------------------------*/
#ifdef DEBUG_SHOW_SDK_INFO
void ReadKey(KeyInfo* pKey)
{
    static u16  repeat_count[12];
    int         i;
    u16         r;

    r = PAD_Read();
    pKey->trg = 0x0000;
    pKey->up = 0x0000;
    pKey->rep = 0x0000;

    for (i = 0; i < 12; i++)
    {
        if (r & (0x0001 << i))
        {
            if (!(pKey->cnt & (0x0001 << i)))
            {
                pKey->trg |= (0x0001 << i);     // �����g���K
                repeat_count[i] = 1;
            }
            else
            {
                if (repeat_count[i] > KEY_REPEAT_START)
                {
                    pKey->rep |= (0x0001 << i); // �����p�����s�[�g
                    repeat_count[i] = (u16) (KEY_REPEAT_START - KEY_REPEAT_SPAN);
                }
                else
                {
                    repeat_count[i]++;
                }
            }
        }
        else
        {
            if (pKey->cnt & (0x0001 << i))
            {
                pKey->up |= (0x0001 << i);      // �����g���K
            }
        }
    }

    pKey->cnt = r;  // �����H�L�[����
}
#endif
