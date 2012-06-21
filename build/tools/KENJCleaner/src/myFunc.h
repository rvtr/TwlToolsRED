/********************************************************************/
/*      myChar.h                                                    */
/*          HelloWorld                                              */
/*                                                                  */
/*              Copyright (C) 2003-2006 NINTENDO Co.,Ltd.           */
/********************************************************************/
/*
	����֐��@�w�b�_
*/

#ifndef	__MY_FUNC_H__
#define	__MY_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro.h>

// define data ------------------------------------------------------
#define SCREEN_X_CHAR			32			// �X�N���[�����L�����N�^��
#define SCREEN_Y_CHAR			24			// �X�N���[���c�L�����N�^��

#define BG0_CHAR_BASE			0x4000
#define BG0_SCREEN_BASE			0x0000

#define PrintStringM(...)		PrintString( 1, __VA_ARGS__ )
#define PrintStringS(...)		PrintString( 0, __VA_ARGS__ )
#define ClearRectangleM(...)    ClearRectangle( bg0BakM, __VA_ARGS__ )
#define ClearRectangleS(...)    ClearRectangle( bg0BakS, __VA_ARGS__ )
#define FillRectangleM(...)     FillRectangle( bg0BakM, __VA_ARGS__ )
#define FillRectangleS(...)     FillRectangle( bg0BakS, __VA_ARGS__ )

// global variable---------------------------------------------------
extern u16 bg0BakM[ SCREEN_X_CHAR * SCREEN_Y_CHAR ];					// ���C��BG0  �o�b�N�A�b�v
extern u16 bg0BakS[ SCREEN_X_CHAR * SCREEN_Y_CHAR ];					// �T�u�@BG0  �o�b�N�A�b�v

// const data--------------------------------------------------------

// function----------------------------------------------------------
void InitDispMain( void );
void InitDispSub ( void );
void PrintString( BOOL sel_lcd, s32 x, s32 y, u16 color, const char *fmt, ... ); // 64�����܂ł̏����t�����������ʂɏo��
void ClearRectangle( u16 *pScreen, u16 pos_x, u16 pos_y, u8 width, u8 height );  // ��`�X�N���[���N���A
void FillRectangle( u16 *pScreen, u16 pos_x, u16 pos_y, u8 width, u8 height, u16 code ); // ��`�X�N���[���t�B��


#ifdef __cplusplus
}
#endif

#endif	// __MY_FUNC_H__

