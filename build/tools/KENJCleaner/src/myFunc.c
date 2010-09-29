/********************************************************************/
/*      myFunc.c                                                    */
/*          HelloWorld                                              */
/*                                                                  */
/*              Copyright (C) 2003-2006 NINTENDO Co.,Ltd.           */
/********************************************************************/
/*
	���샋�[�`��
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
u16	bg0BakM[ SCREEN_X_CHAR * SCREEN_Y_CHAR ] ATTRIBUTE_ALIGN(32);	// ���C��BG0  �o�b�N�A�b�v
u16	bg0BakS[ SCREEN_X_CHAR * SCREEN_Y_CHAR ] ATTRIBUTE_ALIGN(32);	// �T�u�@BG0  �o�b�N�A�b�v

// static variable---------------------------------------------------

// const data--------------------------------------------------------


// ==================================================================
// function's description
// ==================================================================

// VRAM�N���A
static void ClearVRAM( void )
{
	static BOOL cleared = FALSE;
	if( !cleared ) {
		GX_SetBankForLCDC( GX_VRAM_LCDC_ALL );							// VRAM�N���A�̂��߂Ɉꎞ�I��LCDC��VRAM��S�Ċ��蓖�Ă�B
		MI_CpuClearFast( (void*)HW_LCDC_VRAM, HW_LCDC_VRAM_SIZE );
		(void)GX_DisableBankForLCDC();
		cleared = TRUE;
	}
}


// ��ʏ�����
void InitDispMain( void )
{
	//	��ʐݒ�-----------------------------------
	GX_DispOff();													// LCDC OFF
	
	// VRAM�N���A
	ClearVRAM();
	
	//---- �p���b�g�N���A
	MI_CpuClearFast( (void*)HW_PLTT, HW_PLTT_SIZE );
	
	//---- VRAM�̊��蓖��
	GX_SetBankForBG ( GX_VRAM_BG_128_A );
	
	//---- �O���t�B�b�N�X�\�����[�h�ɂ���
	GX_SetGraphicsMode( GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D );
	
	//---- BG0�̐ݒ�
	G2_SetBG0Control(	GX_BG_SCRSIZE_TEXT_256x256,
					 	GX_BG_COLORMODE_16,
					 	GX_BG_SCRBASE_0x0000,
					 	GX_BG_CHARBASE_0x04000,
					 	GX_BG_EXTPLTT_01 );
	G2_SetBG0Priority( 0 );											// BG�R���g���[�� �Z�b�g
	
	//---- BG0�̕\���̂�ON
	GX_SetVisiblePlane( GX_PLANEMASK_BG0 );
	
	//---- �f�[�^���[�h
	MI_CpuCopy16( myChar, (void *)( HW_BG_VRAM + BG0_CHAR_BASE ), sizeof(myChar) );	//  BG�L�����N�^ �Z�b�g
	MI_CpuCopy16( myPalette, (void *)HW_BG_PLTT, sizeof(myPalette) );				//  BG�p���b�g   �Z�b�g
	
	// �X�N���[���o�b�t�@�N���A
	MI_CpuClear32( bg0BakM, sizeof(bg0BakM) );
}


// �T�u��ʏ�����
void InitDispSub( void )
{
	//	��ʐݒ�-----------------------------------
	GXS_DispOff();													// LCDC OFF
	
	// VRAM�N���A
	ClearVRAM();
	
	//---- �p���b�g�N���A
	MI_CpuClearFast( (void*)HW_DB_PLTT, HW_DB_PLTT_SIZE );
	
	//---- VRAM�̊��蓖��
	GX_SetBankForSubBG ( GX_VRAM_SUB_BG_128_C );
	
	//---- �O���t�B�b�N�X�\�����[�h�ɂ���
	GXS_SetGraphicsMode( GX_BGMODE_0 );
	
	//---- BG0�̐ݒ�
	G2S_SetBG0Control(	GX_BG_SCRSIZE_TEXT_256x256,
					 	GX_BG_COLORMODE_16,
					 	GX_BG_SCRBASE_0x0000,
					 	GX_BG_CHARBASE_0x04000,
					 	GX_BG_EXTPLTT_01 );
	G2S_SetBG0Priority( 0 );										// BG�R���g���[�� �Z�b�g
	
	//---- BG0�̕\���̂�ON
	GXS_SetVisiblePlane( GX_PLANEMASK_BG0 );
	
	//---- �f�[�^���[�h
	MI_CpuCopy16( myChar, (void *)( HW_DB_BG_VRAM + BG0_CHAR_BASE ), sizeof(myChar) );	//  BG�L�����N�^ �Z�b�g
	MI_CpuCopy16( myPalette, (void *)HW_DB_BG_PLTT, sizeof(myPalette) );				//  BG�p���b�g   �Z�b�g
	
	// �X�N���[���o�b�t�@�N���A
	MI_CpuClear32( bg0BakS, sizeof(bg0BakS) );
}


// DS�X�N���[���ւ̕�����o��
void PrintString( BOOL sel_lcd, s32 x, s32 y, u16 color, const char *fmt, ... )
{
	char buf[ 64 + 2 ];
	
	va_list vlist;
	va_start( vlist, fmt );
	(void)OS_VSNPrintf( buf, sizeof(buf) - 2, fmt, vlist );
	va_end( vlist );
	
	PrintStringCore( sel_lcd, x, y, color, (const char *)buf );
}


// DS�X�N���[���o�b�t�@�ւ̕����񏑂��o��
static void PrintStringCore( BOOL sel_lcd, s32 x, s32 y, u16 color, const char *strp )
{
	u16 *buffp = ( sel_lcd ) ? (u16 *)bg0BakM : (u16 *)bg0BakS;
	u16 *dstp  = buffp + x + ( y * SCREEN_X_CHAR );
	
	while( *strp ) {
		*dstp++ = (u16)( ( color << GX_SCRFMT_TEXT_COLORPLTT_SHIFT ) | *strp++ );
	}
}


// ��`�X�N���[���N���A
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
