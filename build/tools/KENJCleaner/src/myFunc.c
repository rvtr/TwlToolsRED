/********************************************************************/
/*      myFunc.c                                                    */
/*          HelloWorld                                              */
/*                                                                  */
/*              Copyright (C) 2003-2006 NINTENDO Co.,Ltd.           */
/********************************************************************/
/*
	自作ルーチン
*/

#include <nitro.h>
#include "myFunc.h"
#include "myChar.h"

// define data-------------------------------------------------------

// function's prototype----------------------------------------------
static void ClearVRAM( void );
static void PrintStringCore( BOOL sel_lcd, s32 x, s32 y, u16 color, const char *strp );

// extern data-------------------------------------------------------

// global variable---------------------------------------------------
u16	bg0BakM[ SCREEN_X_CHAR * SCREEN_Y_CHAR ] ATTRIBUTE_ALIGN(32);	// メインBG0  バックアップ
u16	bg0BakS[ SCREEN_X_CHAR * SCREEN_Y_CHAR ] ATTRIBUTE_ALIGN(32);	// サブ　BG0  バックアップ

// static variable---------------------------------------------------

// const data--------------------------------------------------------


// ==================================================================
// function's description
// ==================================================================

// VRAMクリア
static void ClearVRAM( void )
{
	static BOOL cleared = FALSE;
	if( !cleared ) {
		GX_SetBankForLCDC( GX_VRAM_LCDC_ALL );							// VRAMクリアのために一時的にLCDCにVRAMを全て割り当てる。
		MI_CpuClearFast( (void*)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE );
		(void)GX_DisableBankForLCDC();
		cleared = TRUE;
	}
}


// 画面初期化
void InitDispMain( void )
{
	//	画面設定-----------------------------------
	GX_DispOff();													// LCDC OFF
	
	// VRAMクリア
	ClearVRAM();
	
	//---- パレットクリア
	MI_CpuClearFast( (void*)HW_PLTT, HW_PLTT_SIZE );
	
	//---- VRAMの割り当て
	GX_SetBankForBG ( GX_VRAM_BG_128_A );
	
	//---- グラフィックス表示モードにする
	GX_SetGraphicsMode( GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D );
	
	//---- BG0の設定
	G2_SetBG0Control(	GX_BG_SCRSIZE_TEXT_256x256,
					 	GX_BG_COLORMODE_16,
					 	GX_BG_SCRBASE_0x0000,
					 	GX_BG_CHARBASE_0x04000,
					 	GX_BG_EXTPLTT_01 );
	G2_SetBG0Priority( 0 );											// BGコントロール セット
	
	//---- BG0の表示のみON
	GX_SetVisiblePlane( GX_PLANEMASK_BG0 );
	
	//---- データロード
	MI_CpuCopy16( myChar, (void *)( HW_BG_VRAM + BG0_CHAR_BASE ), sizeof(myChar) );	//  BGキャラクタ セット
	MI_CpuCopy16( myPalette, (void *)HW_BG_PLTT, sizeof(myPalette) );				//  BGパレット   セット
	
	// スクリーンバッファクリア
	MI_CpuClear32( bg0BakM, sizeof(bg0BakM) );
}


// サブ画面初期化
void InitDispSub( void )
{
	//	画面設定-----------------------------------
	GXS_DispOff();													// LCDC OFF
	
	// VRAMクリア
	ClearVRAM();
	
	//---- パレットクリア
	MI_CpuClearFast( (void*)HW_DB_PLTT, HW_DB_PLTT_SIZE );
	
	//---- VRAMの割り当て
	GX_SetBankForSubBG ( GX_VRAM_SUB_BG_128_C );
	
	//---- グラフィックス表示モードにする
	GXS_SetGraphicsMode( GX_BGMODE_0 );
	
	//---- BG0の設定
	G2S_SetBG0Control(	GX_BG_SCRSIZE_TEXT_256x256,
					 	GX_BG_COLORMODE_16,
					 	GX_BG_SCRBASE_0x0000,
					 	GX_BG_CHARBASE_0x04000,
					 	GX_BG_EXTPLTT_01 );
	G2S_SetBG0Priority( 0 );										// BGコントロール セット
	
	//---- BG0の表示のみON
	GXS_SetVisiblePlane( GX_PLANEMASK_BG0 );
	
	//---- データロード
	MI_CpuCopy16( myChar, (void *)( HW_DB_BG_VRAM + BG0_CHAR_BASE ), sizeof(myChar) );	//  BGキャラクタ セット
	MI_CpuCopy16( myPalette, (void *)HW_DB_BG_PLTT, sizeof(myPalette) );				//  BGパレット   セット
	
	// スクリーンバッファクリア
	MI_CpuClear32( bg0BakS, sizeof(bg0BakS) );
}


// DSスクリーンへの文字列出力
void PrintString( BOOL sel_lcd, s32 x, s32 y, u16 color, const char *fmt, ... )
{
	char buf[ 64 + 2 ];
	
	va_list vlist;
	va_start( vlist, fmt );
	(void)OS_VSNPrintf( buf, sizeof(buf) - 2, fmt, vlist );
	va_end( vlist );
	
	PrintStringCore( sel_lcd, x, y, color, (const char *)buf );
}


// DSスクリーンバッファへの文字列書き出し
static void PrintStringCore( BOOL sel_lcd, s32 x, s32 y, u16 color, const char *strp )
{
	u16 *buffp = ( sel_lcd ) ? (u16 *)bg0BakM : (u16 *)bg0BakS;
	u16 *dstp  = buffp + x + ( y * SCREEN_X_CHAR );
	
	while( *strp ) {
		*dstp++ = (u16)( ( color << GX_SCRFMT_TEXT_COLORPLTT_SHIFT ) | *strp++ );
	}
}


// 矩形スクリーンクリア
void ClearRectangle( u16 *pScreen, u16 pos_x, u16 pos_y, u8 width, u8 height )
{
	u16 i,j;
	
	pScreen += pos_x + ( pos_y << 5 );
	for(i=0;i<height;i++) {
		for( j = 0; j < width; j++ ) {
			*pScreen++ = 0x0020;
		}
		pScreen += 0x20 - width;
	}
}
