/*===============================================================*//**
 	@file		TextWnd.cpp
 	
 	@brief		nitro-system���g���Ă�BG�t�H���g�\���@�\��
 				�㉺��ʂŊȒP�Ɏg����悤�ɂ�������
 				nnsys_demo_lib/txt.h��LCD���W�X�^�ݒ�Ɉˑ�
 				
 	@author		Kazumasa Hirata

*//*================================================================*/

//****************************************************************************
// Include
//****************************************************************************
	#include "TextWnd.h"
	
//****************************************************************************
// Define
//****************************************************************************
	// �g�p����L�����N�^��̊J�n�ԍ�
	#define CHARACTER_OFFSET	1				
	
//****************************************************************************
// �R�[�h
//****************************************************************************
	//�L�����o�X�̃N���A
	void BaseFontWnd::Clear(int color)
	{
		NNS_G2dCharCanvasClear(&gCanvas, color);
	}
	
	//����������
	void BaseFontWnd::Fill( int x, int y, int w, int h, int color )
	{
		if( w > 0 && h > 0 ){
			NNS_G2dCharCanvasClearArea(&gCanvas, color, x, y, w, h );
		}
	}
	
	//�����o��
	void BaseFontWnd::PrintGlyphIndex(const NNSG2dFont *pFont, int x,int y, u16 index, int color )
	{
		NNSG2dGlyph g;
		NNS_G2dFontGetGlyphFromIndex( &g, pFont, index );
		NNS_G2dCharCanvasDrawGlyph( &gCanvas, pFont, x, y, color, &g );
	}
	
	//�����o��
	void BaseFontWnd::PrintChar(const NNSG2dFont *pFont, int x,int y, u16 code, int color )
	{
		NNS_G2dCharCanvasDrawChar( &gCanvas, pFont, x, y, color, code );
	}
	
	//������o��
	void BaseFontWnd::PrintUnicodeString(const NNSG2dFont *pFont, int x,int y, const NNSG2dChar* const str, int color )
	{
		//�t�H���g�����ɖ߂����߂Ɍ��ݐݒ肳��Ă���t�H���g���擾����
		const NNSG2dFont* pBak = NNS_G2dTextCanvasGetFont(	&gTextCanvas );
		
		//�t�H���g�ύX
		NNS_G2dTextCanvasSetFont( &gTextCanvas, pFont );
		
		//������o��
		NNS_G2dTextCanvasDrawText(
			&gTextCanvas, 
			x, y, 
			color, 
			TXT_DRAWTEXT_FLAG_DEFAULT, 
			str );
		
		//�t�H���g�����ɖ߂�
		NNS_G2dTextCanvasSetFont( &gTextCanvas, pBak );
	}
	
	void BaseFontWnd::PrintString(const NNSG2dFont *pFont, int x,int y, const char* const str, int color )
	{
		NNSG2dChar * p;
		
		//������̒��������߂�
		int len = STD_GetStringLength( str );
	
		//�������m�ہi�k�������p�ɂP�]���j
		p = reinterpret_cast<NNSG2dChar *>(TXT_Alloc( sizeof(NNSG2dChar)*len + 1 ));
		
		if( p ){
			//�m�ۂ����������ɃR�s�[
			for( int i=0; i<len; i++ ){
				p[i] = str[i];
			}
			//�I�[�Ƀk��������
			p[len] = 0x0000;
			
			//��������o��
			PrintUnicodeString( pFont, x, y, p, color );
			
			//�������J��
			TXT_Free( p );
		}
	}
	
	//�P�O�i���o��
	void BaseFontWnd::PrintDec(const NNSG2dFont *pFont,int x,int y, s32 number, int color )
	{
		//Ascii�ŕ����񐶐�
		static char 		buf0[32];
		OS_SPrintf( buf0, "%d", number );
		//Unicode�ɕϊ�
		static NNSG2dChar 	buf[32];
		int i;
		for( i=0; buf0[i] != 0; i++ ){
			buf[i] = buf0[i];
		}
		buf[i] = 0;
		//�o��
		PrintUnicodeString( pFont,x, y, buf, color );
	}
	
	//�P�U�i���o��
	void BaseFontWnd::PrintHex(const NNSG2dFont *pFont,int x, int y, u32 number, int color )
	{
		static char 		buf0[32];
		OS_SPrintf( buf0, "%04x", number );	//�Ƃ肠��������S�������v��Ȃ�
		
		static NNSG2dChar 	buf[32];
		int i;
		for( i=0; buf0[i] != 0; i++ ){
			buf[i] = buf0[i];
		}
		buf[i] = 0;
		
		PrintUnicodeString( pFont, x, y, buf,color );
	}

	void MainWnd::Init( const NNSG2dFont *pFont )
	{
		// Main BG 0 ��ݒ�
		G2_SetBG0Control(
			GX_BG_SCRSIZE_TEXT_256x256,		// �X�N���[���T�C�Y 256x256
			GX_BG_COLORMODE_16,				// �J���[���[�h		16�F
			GX_BG_SCRBASE_0x0000,			// �X�N���[���x�[�X
			GX_BG_CHARBASE_0x00000,			// �L�����N�^�x�[�X
			GX_BG_EXTPLTT_01				// �g���p���b�g�X���b�g
		);
		
		// Main BG0 ������
		CMN_SetPlaneVisible( GX_PLANEMASK_BG0 );
		
		// CharCanvas �̏�����
		NNS_G2dCharCanvasInitForBG(
			&gCanvas,
			gOffBuffer,
			CANVAS_WIDTH,
			CANVAS_HEIGHT,
			NNS_G2D_CHARA_COLORMODE_16
		);

		// TextCanvas�̏�����
		NNS_G2dTextCanvasInit(
			&gTextCanvas,
			&gCanvas,
			pFont,
			TEXT_HSPACE,
			TEXT_VSPACE
		);

		// �X�N���[����ݒ�
		NNS_G2dMapScrToCharText(
			G2_GetBG0ScrPtr(),
			CANVAS_WIDTH,
			CANVAS_HEIGHT,
			CANVAS_LEFT,
			CANVAS_TOP,
			NNS_G2D_TEXT_BG_WIDTH_256,
			CHARACTER_OFFSET,
			TXT_CPALETTE_MAIN
		);
	}
	
	void MainWnd::Draw()
	{
        // V�u�����N���Ԓ��̓]���^�X�N��o�^���܂��B
        (void)NNS_GfdRegisterNewVramTransferTask(
            NNS_GFD_DST_2D_BG0_CHAR_MAIN,
            sizeof(GXCharFmt16) * CHARACTER_OFFSET,
            gOffBuffer,
            sizeof(gOffBuffer)
        );
	}

	void SubWnd::Init( const NNSG2dFont *pFont )
	{
		//SUB BG0 �Z�b�g�A�b�v
	    G2S_SetBG0Control(
			GX_BG_SCRSIZE_TEXT_256x256,		// �X�N���[���T�C�Y 256x256
			GX_BG_COLORMODE_16,				// �J���[���[�h		16�F
			GX_BG_SCRBASE_0x0000,			// �X�N���[���x�[�X
			GX_BG_CHARBASE_0x04000,			// �L�����N�^�x�[�X
			GX_BG_EXTPLTT_01				// �g���p���b�g�X���b�g
	    );
		
		//����
		CMN_SetPlaneVisibleSub( GX_PLANEMASK_BG0 );
		
		// CharCanvas�̏�����
		NNS_G2dCharCanvasInitForBG(
			&gCanvas,
			gOffBuffer,
			CANVAS_WIDTH,
			CANVAS_HEIGHT,
			NNS_G2D_CHARA_COLORMODE_16
		);
	   
		// TextCanvas�̏�����
		NNS_G2dTextCanvasInit(
			&gTextCanvas,
			&gCanvas,
			pFont,
			TEXT_HSPACE,
			TEXT_VSPACE
		);

		// �X�N���[����ݒ�
		NNS_G2dMapScrToCharText(
			G2S_GetBG0ScrPtr(),
			CANVAS_WIDTH,
			CANVAS_HEIGHT,
			CANVAS_LEFT,
			CANVAS_TOP,
			NNS_G2D_TEXT_BG_WIDTH_256,
			CHARACTER_OFFSET,
			TXT_CPALETTE_MAIN
		);
	}
	void SubWnd::Draw()
	{
        // V�u�����N���Ԓ��̓]���^�X�N��o�^���܂��B
        (void)NNS_GfdRegisterNewVramTransferTask(
            NNS_GFD_DST_2D_BG0_CHAR_SUB,
            sizeof(GXCharFmt16) * CHARACTER_OFFSET,
            gOffBuffer,
            sizeof(gOffBuffer)
        );
	}

