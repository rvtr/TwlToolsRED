/********************************************************************/
/*      myChar.h                                                    */
/*          HelloWolrd                                              */
/*                                                                  */
/*              Copyright (C) 2003-2006 NINTENDO Co.,Ltd.           */
/********************************************************************/
/*
	自作キャラデータ　ヘッダ
*/

#ifndef	__MY_CHAR_H__
#define	__MY_CHAR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro.h>

// define data ------------------------------------------------------

// パレットカラー
#define WHITE				0
#define RED					1
#define	GREEN				2
#define	BLUE				3
#define	YELLOW				4
#define	CYAN				5
#define	PURPLE				6
#define	LIGHTGREEN			7
#define	HIGHLIGHT_Y			8
#define	HIGHLIGHT_C			9

// global variable---------------------------------------------------

// const data--------------------------------------------------------
extern const u16 myPalette[ 13 ][ 16 ];					// パレットデータ
extern const u16 myChar[ 0x20 * 0x140 / sizeof(u16) ];	// キャラクターデータ

// function----------------------------------------------------------


#ifdef __cplusplus
}
#endif

#endif		// __MY_CHAR_H__

