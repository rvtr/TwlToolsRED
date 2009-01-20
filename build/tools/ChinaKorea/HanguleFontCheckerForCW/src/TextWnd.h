/*===============================================================*//**
 	@file		TextWnd.h
 	
 	@brief		nitro-system���g���Ă�BG�t�H���g�\���@�\��
 				�㉺��ʂŊȒP�Ɏg����悤�ɂ�������
 				nnsys_demo_lib/txt.h��LCD���W�X�^�ݒ�Ɉˑ�
 				
 	@author		Kazumasa Hirata

*//*================================================================*/
#ifndef TEXTWND_H_ 

#define TEXTWND_H_

//****************************************************************************
// Include
//****************************************************************************
	//�C�V�����C�u����
//	#ifndef NNS_G2D_UNICODE					
	#define NNS_G2D_UNICODE			// ������`��֐�������������Unicode�ɂ���
//	#endif
	#include <nitro.h>
	#include <nnsys/g2d/g2d_TextCanvas.h>
	#include <nnsys/g2d/g2d_CharCanvas.h>
	
	//�C�V���f���p���C�u�����i�ꕔ���ρj
	#include "nnsys_demo_lib/cmn.h"
	#include "nnsys_demo_lib/txt.h"
	#include "nnsys_demo_lib/loader.h"
	
//****************************************************************************
// Declare
//****************************************************************************
	//��{�ƂȂ�e�L�X�g�\���N���X
	class BaseFontWnd {
	protected:
		NNSG2dCharCanvas		gCanvas;		// CharCanvas
		NNSG2dTextCanvas		gTextCanvas;	// TextCanvas
		static const int TEXT_HSPACE = 1;		// ������`�掞�̕����� (�s�N�Z���P��)
		static const int TEXT_VSPACE = 1;		// ������`�掞�̍s��	(�s�N�Z���P��)
		static const int TEXT_COLOR	 =	TXT_COLOR_WHITE;
		static const int BACK_COLOR	 =	TXT_COLOR_NULL;
	public:
		//�C���^�[�t�F�C�X
		
		/*	���z�֐��e�[�u�������������Ȃ��̂ŏ����Ă���
		BaseFontWnd();
		virtural ~BaseFontWnd() = 0;
		virtual void Init() = 0;
		virtual void Draw() = 0;
		*/
		
		//�L�����o�X�̃N���A
		void Clear(int color=BACK_COLOR);
		
		//����������
		void Fill(int x, int y, int w, int h, int color=TEXT_COLOR );
		
		//�����o��
		void PrintGlyphIndex( const NNSG2dFont *pFont,  int x,int y, u16 index, int color=TEXT_COLOR );
		
		//�����o��
		void PrintChar( const NNSG2dFont *pFont,  int x,int y, u16 code, int color=TEXT_COLOR );
		
		//������o��
		void PrintUnicodeString(const NNSG2dFont *pFont, int x,int y, const NNSG2dChar* const str, int color=TEXT_COLOR );
		void PrintString(const NNSG2dFont *pFont, int x,int y, const char* const str, int color=TEXT_COLOR );
		
		//�P�O�i���o��
		void PrintDec(const NNSG2dFont *pFont, int x,int y, s32 number, int color=TEXT_COLOR );
		
	
		//�P�U�i���o��
		void PrintHex(const NNSG2dFont *pFont, int x,int y, u32 number, int color=TEXT_COLOR );
	};
	
	//�Q�c�O���t�B�b�N�G���W���`���g�p����e�L�X�g�\���N���X
	class MainWnd : public BaseFontWnd {
		static const int CANVAS_LEFT	=	0;		// �����`���̈ʒuX (�L�����N�^�P��)
		static const int CANVAS_TOP		=	0;		// �����`���̈ʒuY (�L�����N�^�P��)
		static const int CANVAS_WIDTH	=	32;		// �����`���̕�	 (�L�����N�^�P��)
		static const int CANVAS_HEIGHT	=	24;		// �����`���̍���	 (�L�����N�^�P��)
		GXCharFmt16   gOffBuffer[CANVAS_HEIGHT][CANVAS_WIDTH];
	public:
		void Init( const NNSG2dFont *pFont );
		void Draw();
	};

	//�Q�c�O���t�B�b�N�G���W���a	���g�p����e�L�X�g�\���N���X
	class SubWnd : public BaseFontWnd {
		static const int CANVAS_LEFT	=	0;		// �����`���̈ʒuX (�L�����N�^�P��)
		static const int CANVAS_TOP		=	0;		// �����`���̈ʒuY (�L�����N�^�P��)
		static const int CANVAS_WIDTH	=	32;		// �����`���̕�	 (�L�����N�^�P��)
		static const int CANVAS_HEIGHT	=	24;		// �����`���̍���	 (�L�����N�^�P��)
		GXCharFmt16   gOffBuffer[CANVAS_HEIGHT][CANVAS_WIDTH];
	public:
		void Init( const NNSG2dFont *pFont );
		void Draw();
	};

#endif	// TEXTWND_H_ 
