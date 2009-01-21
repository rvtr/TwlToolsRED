/*---------------------------------------------------------------------------*
  Project:  NITRO-System - build - demos - g2d - Text - textdemolib
  File:     txt.c

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: txt.c,v $
  Revision 1.1  2005/05/20 04:34:35  hatamoto_minoru
  NitroSystem への登録。

  Revision 1.6  2005/03/28 02:15:09  hatamoto_minoru
  TXT_LoadFile の追加。

  Revision 1.5  2005/03/18 09:19:46  hatamoto_minoru
  API変更に対応。

  Revision 1.4  2005/02/08 04:06:59  hatamoto_minoru
  ファイルヘッダの修正。

  Revision 1.3  2005/02/01 11:53:15  hatamoto_minoru
  共通カラーパレット定義を追加。
  コメントを追加。

  Revision 1.2  2005/01/25 01:42:45  hatamoto_minoru
  TXT_SetCharCanvasOBJAttrs() 追加。

  Revision 1.1  2005/01/21 01:03:09  hatamoto_minoru
  TextDraw への登録。

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include "txt.h"
#include <nnsys.h>
#include "cmn.h"
#include "loader.h"

static NNSFndAllocator sAllocator;

// デモ共通のカラーパレット
GXRgb TXTColorPalette[32] =
{
    GX_RGB( 0,  0,  0), // NULL
    GX_RGB(31, 31, 31), GX_RGB(25, 25, 25), GX_RGB(22, 22, 22), GX_RGB(20, 20, 20),
    GX_RGB( 0,  0,  0), GX_RGB( 6,  6,  6), GX_RGB( 8,  8,  8), GX_RGB(10, 10, 10),
    GX_RGB( 0,  0,  0), GX_RGB( 0,  0,  0), GX_RGB( 0,  0,  0), GX_RGB( 0,  0,  0),
    GX_RGB( 0,  0,  0), GX_RGB( 0,  0,  0), GX_RGB( 0,  0,  0),
    
    GX_RGB( 0,  0,  0), GX_RGB(12, 16, 19), GX_RGB(23,  9,  0), GX_RGB(31,  0,  3),
    GX_RGB(31, 17, 31), GX_RGB(31, 18,  0), GX_RGB(30, 28,  0), GX_RGB(21, 31,  0),
    GX_RGB( 0, 20,  7), GX_RGB( 9, 27, 17), GX_RGB( 6, 23, 30), GX_RGB( 0, 11, 30),
    GX_RGB( 0,  0, 18), GX_RGB(17,  0, 26), GX_RGB(26,  0, 29), GX_RGB(31,  0, 18),
};

//****************************************************************************
//
//****************************************************************************

/*---------------------------------------------------------------------------*
  Name:         TXT_Init

  Arguments:    なし。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void TXT_Init( OSIrqFunction hBlankFunc, OSIrqFunction vBlankFunc )
{
    // Common initialization.
    OS_Init();
    FX_Init();
    GX_Init();

    GX_DispOff();
    GXS_DispOff();

    CMN_InitInterrupt();
    CMN_BeginHVBlankIntr( hBlankFunc, vBlankFunc );
    
	CMN_ClearVram();
    CMN_InitAllocator( &sAllocator );
    CMN_InitFileSystem( &sAllocator );
}

/*---------------------------------------------------------------------------*
  Name:         TXT_Alloc

  Description:  メモリを確保します。

  Arguments:    size:   確保するメモリのサイズ

  Returns:      確保したメモリ領域へのポインタ。
 *---------------------------------------------------------------------------*/
void* TXT_Alloc(u32 size)
{
    return NNS_FndAllocFromAllocator( &sAllocator, size );
}

/*---------------------------------------------------------------------------*
  Name:         TXT_Free

  Description:  TXT_Alloc() で確保したメモリを解放します。

  Arguments:    ptr:    開放するメモリ領域へのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void TXT_Free(void* ptr)
{
    NNS_FndFreeToAllocator( &sAllocator, ptr );
}

/*---------------------------------------------------------------------------*
  Name:         TXT_LoadFont

  Description:  フォントをファイルからメモリにロードします。

  Arguments:    pFname: フォントリソースのパス。

  Returns:      ロードしたフォントへのポインタ。
 *---------------------------------------------------------------------------*/
void TXT_LoadFont( NNSG2dFont* pFont, const char* pFname )
{
    void* pBuf;

    pBuf = LoadNFTR( pFont, pFname, &sAllocator );
    if( pBuf == NULL ) {
        OS_Panic("Fail to load font file(%s).", pFname);
    }

    return;
}

/*---------------------------------------------------------------------------*
  Name:         TXT_LoadFile

  Description:  ファイルをメモリにロードします。

  Arguments:    ppFile: ファイルをロードしたメモリアドレスを受け取る
                        バッファへのポインタ。
                fpath:  ロードするファイルのパス

  Returns:      ロードしたファイルのファイルサイズを返します。
                0 の場合はファイルロードに失敗した事を表します。
                この場合 *ppFile の値は無効です。
 *---------------------------------------------------------------------------*/
u32 TXT_LoadFile(void** ppFile, const char* fpath)
{
    return CMN_LoadFile(ppFile, fpath, &sAllocator);
}

/*---------------------------------------------------------------------------*
  Name:         TXT_SetupBackground

  Description:  メイン画面BG0に背景画像をロード＆表示します。

  Arguments:    なし。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void TXT_SetupBackground( void )
{
    void*                   pPltFile;
    void*                   pChrFile;
    void*                   pScnFile;
    NNSG2dPaletteData*      pPltData;
    NNSG2dCharacterData*    pChrData;
    NNSG2dScreenData*       pScnData;

    pPltFile = LoadNCLR( &pPltData, "/data/font_up3.NCLR", &sAllocator );
    SDK_NULL_ASSERT( pPltFile );
    pChrFile = LoadNCGR( &pChrData, "/data/font_up3.NCGR", &sAllocator );
    SDK_NULL_ASSERT( pChrFile );
	pScnFile = LoadNSCR( &pScnData, "/data/font_up3.NSCR", &sAllocator );
    SDK_NULL_ASSERT( pScnFile );

    GX_SetVisiblePlane(GX_PLANEMASK_BG1);
    G2_SetBG1Priority(3);
    NNS_G2dBGSetup(
        NNS_G2D_BGSELECT_MAIN1,
        pScnData,
        pChrData,
        pPltData,
        GX_BG_SCRBASE_0x7800,
        GX_BG_CHARBASE_0x38000
    );
    NNS_FndFreeToAllocator( &sAllocator, pPltFile );
    NNS_FndFreeToAllocator( &sAllocator, pChrFile );
    NNS_FndFreeToAllocator( &sAllocator, pScnFile );

    pPltFile = LoadNCLR( &pPltData, "/data/font_dw.NCLR", &sAllocator );
    SDK_NULL_ASSERT( pPltFile );
    pChrFile = LoadNCGR( &pChrData, "/data/font_dw.NCGR", &sAllocator );
    SDK_NULL_ASSERT( pChrFile );
	pScnFile = LoadNSCR( &pScnData, "/data/font_dw.NSCR", &sAllocator );
    SDK_NULL_ASSERT( pScnFile );
	
	GXS_SetVisiblePlane(GX_PLANEMASK_BG1);
    G2S_SetBG1Priority(3);
    NNS_G2dBGSetup(
        NNS_G2D_BGSELECT_SUB1,
        pScnData,
        pChrData,
        pPltData,
        GX_BG_SCRBASE_0x0800,
        GX_BG_CHARBASE_0x0c000
    );

    NNS_FndFreeToAllocator( &sAllocator, pPltFile );
    NNS_FndFreeToAllocator( &sAllocator, pChrFile );
    NNS_FndFreeToAllocator( &sAllocator, pScnFile );
}

void TXT_EnableBackground( void )
{
	GX_SetVisiblePlane(  GX_GetVisiblePlane()  | GX_PLANEMASK_BG1 );
	GXS_SetVisiblePlane( GXS_GetVisiblePlane() | GX_PLANEMASK_BG1 );
}

void TXT_DisableBackground( void )
{
	GX_SetVisiblePlane(  GX_GetVisiblePlane()  & (~GX_PLANEMASK_BG1) );
	GXS_SetVisiblePlane( GXS_GetVisiblePlane() & (~GX_PLANEMASK_BG1) );
}
