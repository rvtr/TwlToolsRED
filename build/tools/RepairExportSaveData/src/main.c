/*---------------------------------------------------------------------------*
  Project:  TwlSDK - tests - appjumpTest - Nand-2
  File:     main.c

  Copyright 2008 Nintendo.  All rights reserved.

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
#include "RepairExportSaveData.h"
#include "common.h"
#include "screen.h"

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         TwlMain

  Description:  メイン関数
 *---------------------------------------------------------------------------*/
void TwlMain(void)
{
	InitCommon();
    InitScreen();

    GX_DispOn();
    GXS_DispOn();

    ClearScreen();
    RepairSaveDataInit();

    while(TRUE)
    {
		// 処理
		RepairSaveDataMain();
        
        // Ｖブランク待ち
        OS_WaitVBlankIntr();
        
        // 画面クリア
        ClearScreen();
    }

	// Ｖブランク待ち 最後に画面を更新してから終了
    OS_WaitVBlankIntr();
    OS_Terminate();
}


void VBlankIntr(void)
{
    // テキスト表示を更新
    UpdateScreen();

    // IRQ チェックフラグ47をセット
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}


/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
