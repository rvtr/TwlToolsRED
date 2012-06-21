/********************************************************************/
/*      myChar.h                                                    */
/*          HelloWorld                                              */
/*                                                                  */
/*              Copyright (C) 2003-2006 NINTENDO Co.,Ltd.           */
/********************************************************************/
/*
	自作関数　ヘッダ
*/

#ifndef	__MY_FUNC_H__
#define	__MY_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro.h>

// define data ------------------------------------------------------
#define SCREEN_X_CHAR			32			// スクリーン横キャラクタ数
#define SCREEN_Y_CHAR			24			// スクリーン縦キャラクタ数

#define BG0_CHAR_BASE			0x4000
#define BG0_SCREEN_BASE			0x0000

#define PrintStringM(...)		PrintString( 1, __VA_ARGS__ )
#define PrintStringS(...)		PrintString( 0, __VA_ARGS__ )
#define ClearRectangleM(...)    ClearRectangle( bg0BakM, __VA_ARGS__ )
#define ClearRectangleS(...)    ClearRectangle( bg0BakS, __VA_ARGS__ )
#define FillRectangleM(...)     FillRectangle( bg0BakM, __VA_ARGS__ )
#define FillRectangleS(...)     FillRectangle( bg0BakS, __VA_ARGS__ )

// global variable---------------------------------------------------
extern u16 bg0BakM[ SCREEN_X_CHAR * SCREEN_Y_CHAR ];					// メインBG0  バックアップ
extern u16 bg0BakS[ SCREEN_X_CHAR * SCREEN_Y_CHAR ];					// サブ　BG0  バックアップ

// const data--------------------------------------------------------

// function----------------------------------------------------------
void InitDispMain( void );
void InitDispSub ( void );
void PrintString( BOOL sel_lcd, s32 x, s32 y, u16 color, const char *fmt, ... ); // 64文字までの書式付き文字列を画面に出力
void ClearRectangle( u16 *pScreen, u16 pos_x, u16 pos_y, u8 width, u8 height );  // 矩形スクリーンクリア
void FillRectangle( u16 *pScreen, u16 pos_x, u16 pos_y, u8 width, u8 height, u16 code ); // 矩形スクリーンフィル


#ifdef __cplusplus
}
#endif

#endif	// __MY_FUNC_H__

