/*---------------------------------------------------------------------------*
  Project:  MainMemoryDumper
  File:     main.c

  Copyright 2011 Nintendo.  All rights reserved.

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
#include "mmdumper.h"

// extern data-----------------------------------------------------------------

// define data-----------------------------------------------------------------

// function's prototype-------------------------------------------------------
static void INTR_VBlank( void );

// global variable-------------------------------------------------------------

// static variable-------------------------------------------------------------

// const data------------------------------------------------------------------


// ============================================================================
// function's description
// ============================================================================
void TwlMain(void)
{
    BOOL loop = TRUE;

    myInit();
    mmdumperInit();

    while(loop)
    {

        myPreMain();

        loop = mmdumperMain();

        myProMain();

        OS_WaitIrq(1, OS_IE_V_BLANK);                               // Vブランク割り込み待ち
    }
    while (1)
    {
        OS_WaitIrq(1, OS_IE_V_BLANK);                               // Vブランク割り込み待ち
    }
}

// ============================================================================
// 割り込み処理
// ============================================================================

// Vブランク割り込み
static void INTR_VBlank(void)
{
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);                              // Vブランク割込チェックのセット
}

