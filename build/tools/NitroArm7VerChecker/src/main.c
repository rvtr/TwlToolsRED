/*---------------------------------------------------------------------------*
  Project:  NitroArm7VerChecker
  File:     main.c

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
#include "misc_simple.h"
#include "NitroArm7VerChecker.h"

// extern data-----------------------------------------------------------------

// define data-----------------------------------------------------------------

// function's prototype-------------------------------------------------------
static void INTR_VBlank( void );

// global variable-------------------------------------------------------------

// static variable-------------------------------------------------------------
BOOL g_isValidTSD;

// const data------------------------------------------------------------------


// ============================================================================
// function's description
// ============================================================================
void TwlMain(void)
{
	myInit();

	NitroArm7VerCheckerInit();
	// ���C�����[�v----------------------------
	while(1){
		
		myPreMain();
	    
		NitroArm7VerCheckerMain();
		
		myProMain();
		
		OS_WaitIrq(1, OS_IE_V_BLANK);								// V�u�����N���荞�ݑ҂�
	}
}


// ============================================================================
// ���荞�ݏ���
// ============================================================================

// V�u�����N���荞��
static void INTR_VBlank(void)
{
	OS_SetIrqCheckFlag(OS_IE_V_BLANK);								// V�u�����N�����`�F�b�N�̃Z�b�g
}

