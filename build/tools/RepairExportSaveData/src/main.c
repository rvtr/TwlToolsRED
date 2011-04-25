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
//#include <twl/fatfs.h>
//#include <twl/nam.h>
//#include <twl/aes.h>
//#include <twl/os/common/banner.h>

#include "RepairExportSaveData.h"
#include "common.h"
#include "screen.h"

/*---------------------------------------------------------------------------*
    変数 定義
 *---------------------------------------------------------------------------*/
/*
typedef void* (*NAMUTAlloc)(u32 size);
typedef void  (*NAMUTFree)(void* ptr);

static NAMUTAlloc spAllocFunc;
static NAMUTFree  spFreeFunc;

// キー入力
static KeyInfo  gKey;
*/
/*---------------------------------------------------------------------------*
   Prototype
 *---------------------------------------------------------------------------*/
/*
static void DrawScene(void);
static void* AllocForNAM(u32 size);
static void FreeForNAM(void* ptr);

void* MyNAMUT_Alloc(u32 size);
void MyNAMUT_Free(void* buffer);
*/
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

//    spAllocFunc = AllocForNAM;
//    spFreeFunc = FreeForNAM;

//    FS_Init( FS_DMA_NOT_USE );

//    NAM_Init(AllocForNAM, FreeForNAM);

//	PutMainScreen( 7, 12, 0xf6, "--- Now Loading ---");
//	PutSubScreen(  7, 12, 0xf6, "--- Now Loading ---");

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


/*---------------------------------------------------------------------------*
  Name:         DrawScene

  Description:  画面描画関数

  メモ：
    0xf0, // 黒				0xf1, // 赤				0xf2, // 緑
  	0xf3, // 青				0xf4, // 黄				0xf5, // ピンク
	0xf6, // 水色			0xf7, // くすんだ赤		0xf8, // くすんだ緑
	0xf9, // くすんだ青 	0xfa, // くすんだ黄色	0xfb, // 紫
    0xfc, // うすい青		0xfd, // 灰色			0xfe, // 濃い灰色
 *---------------------------------------------------------------------------*/
/*
#define COMMON_COLOR			((u8)0xff)
#define PERSONALIZED_COLOR		((u8)0xfc)

#define GAME_CODE_BASE_X		1

static void DrawScene(void)
{
    PutMainScreen( 10, 12, 0xf1, "--- Error ---");
    PutSubScreen(  10, 12, 0xf1, "--- Error ---");
}


void* MyNAMUT_Alloc(u32 size)
{
    const u32 allocSize = MATH_ROUNDUP32(size);
    SDK_ASSERTMSG( spAllocFunc != NULL, "NAMUT_Init should be called previously.\n");
    return spAllocFunc(allocSize);
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


void MyNAMUT_Free(void* buffer)
{
    SDK_ASSERTMSG( spFreeFunc != NULL, "NAMUT_Init should be called previously.\n");
    if (buffer)
    {
        spFreeFunc(buffer);
    }
}


static void FreeForNAM(void* ptr)
{
	OS_FreeToMain(ptr);
}
*/

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
