/*===============================================================*//**
 	@file		FontView.cpp
 	
 	@brief		NITRO-SYSTEM �̃f���v���O�����uUTF16Text�v��
 				�x�[�X�ɂ��ׂẴt�H���g��\������r���[�A�ł�
 				
 	@author		Kazumasa Hirata

*//*================================================================*/

//****************************************************************************
// Include
//****************************************************************************
	#include "FontView.h"

	//�C�V�����C�u����
	#include <nitro.h>

	//�C�V���f���p���C�u�����i�ꕔ���ρj
	#include "nnsys_demo_lib/txt.h"

	//Floor���C�u�����i�L�[���擾�j
	#include "lob_pad_lib/Pad.hpp"

	//�A�v���P�[�V�����w���p�i�e�L�X�g�����j
	#include "TextWnd.h"
	
	//������e�[�u���f�[�^�t�@�C���w�b�_
	#include "TextTable.h"

//****************************************************************************
// Define
//****************************************************************************
	//��ʏ��
	enum Status{
		STATUS_SELECT,		//
		STATUS_FONT,		//
		STATUS_TEXT,		//
		STATUS_MAX			//
	};

	//�P�t���[���̓]���^�X�N��
	#define TRANS_TASK_NUM  	2

//****************************************************************************
// �O���ϐ�
//****************************************************************************
	//�O���[�v���Ƃ̕����R�[�h�e�[�u��
/*	extern const unsigned short DS[];		
	extern const unsigned short ASCII[];		
	extern const unsigned short JIS_X_0201_Half_width_KATAKANA[];		
	extern const unsigned short Code_Page_1252[];		
	extern const unsigned short ISO_8859_1_Latin_1[];		
	extern const unsigned short JIS_X_0208_Not_Hanja[];		*/
	extern const unsigned short KS_X_1001_Hangul[];	
	extern const unsigned short KS_X_1001_Jamo[];	
	extern const unsigned short additional_Hangul[];
    extern const unsigned short wii_additional_Hangul[];		// �g�D���[�^�C�v�t�H���g(2360 + 8844)

	//�����R�[�h�e�[�u���̗v�f���萔
/*	extern const unsigned short Count_of_DS ;
	extern const unsigned short Count_of_ASCII ;
	extern const unsigned short Count_of_JIS_X_0201_Half_width_KATAKANA ;
	extern const unsigned short Count_of_Code_Page_1252 ;
	extern const unsigned short Count_of_ISO_8859_1_Latin_1 ;
	extern const unsigned short Count_of_JIS_X_0208_Not_Hanja ;*/
	extern const unsigned short Count_of_KS_X_1001_Hangul;
	extern const unsigned short Count_of_KS_X_1001_Jamo;
	extern const unsigned short Count_of_additional_Hangul;
    extern const unsigned short Count_of_wii_additional_Hangul;	// 8844

	//�����R�[�h�e�[�u���̖��O
	typedef const char * LPCSTR;
/*	extern const LPCSTR Title_of_DS ;
	extern const LPCSTR Title_of_ASCII ;
	extern const LPCSTR Title_of_JIS_X_0201_Half_width_KATAKANA ;
	extern const LPCSTR Title_of_Code_Page_1252 ;
	extern const LPCSTR Title_of_ISO_8859_1_Latin_1 ;
	extern const LPCSTR Title_of_JIS_X_0208_Not_Hanja ;*/
	extern const LPCSTR Title_of_KS_X_1001_Hangul ;
	extern const LPCSTR Title_of_KS_X_1001_Jamo ;
	extern const LPCSTR Title_of_additional_Hangul ;
    extern const LPCSTR Title_of_wii_TrueTypeFont_Hangul;		// "wii True Type Font Hangul"
    extern const LPCSTR Title_of_wii_additional_Hangul ; 		// "wii additional Hangul"

//****************************************************************************
// �t�H���g�J�e�S���[	�\����
//****************************************************************************
	//��`
	enum FONTCATEGORY{
/*		CATEGORY_DS,
		CATEGORY_ASCII,
		CATEGORY_JIS_X_0201_Half_width_KATAKANA,
		CATEGORY_Code_Page_1252,
		CATEGORY_ISO_8859_1_Latin_1,
		CATEGORY_JIS_X_0208_Not_Hanja,*/
		CATEGORY_KS_X_1001_Hangul,
		CATEGORY_KS_X_1001_Jamo,
		CATEGORY_additional_Hangul,
        CATEGORY_wii_TrueTypeFont_Hangul,
		CATEGORY_wii_additional_Hangul,

		CATEGORY_MAX
	};

	//�����R�[�h�e�[�u���Ɨv�f�����֘A�t�����t�H���g�J�e�S���\����
	struct FontCategory{
	//private�ɂ���ƃO���[�o����`�ł��Ȃ����ߊO���B�C������private
		const u16    * pArray;
		const u16    * pCount;
		const LPCSTR * pTitle;
	//�ȒP�Ȏg������񋟂���C���^�[�t�F�C�X
	public:	
		u16 GetCount() const			{	return *pCount;			}
		const u16 * GetArray() const	{	return pArray;			}
		LPCSTR GetTitle() const			{	return *pTitle;			}
		u16 GetValue( int index ) const	{	return pArray[index];	}
	};

	//�t�H���g�J�e�S���e�[�u��
	static const FontCategory m_FontCategoryTbl[] = {
/*		{ DS,								&Count_of_DS 							,	&Title_of_DS 								},
		{ ASCII,                    		&Count_of_ASCII 						,	&Title_of_ASCII 							},	
		{ JIS_X_0201_Half_width_KATAKANA,   &Count_of_JIS_X_0201_Half_width_KATAKANA,	&Title_of_JIS_X_0201_Half_width_KATAKANA 	},
		{ Code_Page_1252,           		&Count_of_Code_Page_1252                ,   &Title_of_Code_Page_1252                    },
		{ ISO_8859_1_Latin_1,				&Count_of_ISO_8859_1_Latin_1 			,	&Title_of_ISO_8859_1_Latin_1 				},
		{ JIS_X_0208_Not_Hanja,     		&Count_of_JIS_X_0208_Not_Hanja       	,   &Title_of_JIS_X_0208_Not_Hanja              },*/
		{ KS_X_1001_Hangul,         		&Count_of_KS_X_1001_Hangul 				,	&Title_of_KS_X_1001_Hangul 					},
		{ KS_X_1001_Jamo,       	  		&Count_of_KS_X_1001_Jamo 				,	&Title_of_KS_X_1001_Jamo 					},
		{ additional_Hangul,         		&Count_of_additional_Hangul				,	&Title_of_additional_Hangul					},
        { KS_X_1001_Hangul,        			&Count_of_KS_X_1001_Hangul 				,	&Title_of_wii_TrueTypeFont_Hangul			}, // �V�K�ǉ� (Wii�g�D���[�^�C�v�t�H���g)
        { wii_additional_Hangul,         	&Count_of_wii_additional_Hangul			,	&Title_of_wii_additional_Hangul 			}, // �V�K�ǉ� (Wii�g���g�D���[�^�C�v�t�H���g)
	};

//****************************************************************************
// ��ʏ�Ԃ��Ƃ̊Ǘ��\����		�I�����
//****************************************************************************
	struct Status_Select{
	private:
		//�I�����@��`�l
		static const int  MENU_LEFT			= 64;					//���W
		static const int  MENU_TOP			= 64;					//	�h
		static const int  MENU_HEIGHT		= 20;					//�c�����I�t�Z�b�g
		static const int  MENU_NUM			= 2;					//��
		//�J�[�\���@��`�l	
		static const int  CURSOR_LEFT		= (MENU_LEFT-20);		//���W
		static const int  CURSOR_TOP		= MENU_TOP;				//
		static const u16  CURSOR_CODE		= 0xe019;				//�\�����镶���̃R�[�h
		
		u8	   m_CursorIndex;
	public:
		Status Run();
	};

//****************************************************************************
// ��ʏ�Ԃ��Ƃ̊Ǘ��\����		�t�H���g�\�����
//****************************************************************************
	struct Status_Font{
	private:
		//�\��
		enum DISPTYPE{
			DISPTYPE_8x2,
			DISPTYPE_10x2,
			DISPTYPE_MAX
		};
		
		//�O���[�v�E�C���h�E��`�l
		static const int  GROUP_HEIGHT 			= 2	;	// �P�O���[�v�̒��̂P��̐�
		static const int  GROUP_ID_X			= 180;
		static const int  GROUP_ID_Y			= 13;
		static const int  GROUP_FONT_LEFT		= 32;
		static const int  GROUP_FONT_TOP		= 62;
		static const int  GROUP_FONT_WIDTH		= 20;
		static const int  GROUP_FONT_HEIGHT		= 40;

		//�t�H���gID��`�l
		static const int  FONT_ID_X				= 110;
		static const int  FONT_ID_Y				= 14;
		
		//�Z���N�g�E�C���h�E��`�l
		static const int  SELECT_LEFT			= 10;
		static const int  SELECT_TOP			= 165;
		static const int  SELECT_WIDTH			= 24;
		
		//�J�[�\����`�l
		static const int  CURSOR_TOP			= (SELECT_TOP-20);
		static const u16  CURSOR_CODE			= 0xe01C;
			
		//�t�H���g�� ��`�l
		static const int  FONTTITLE_X			= 48;
		static const int  FONTTITLE_Y			= 168;
		static const int  FONTTYPE_MAX			= 3;

		//�^�[�Q�b�g�E�C���h�E��`�l
		static const int  TARGET_WIDTH			= 3;
		static const int  TARGET_HEIGHT			= 2;
		static const int  TARGET_FONT_TOP		= 76;
		static const int  TARGET_FONT_WIDTH		= 24;
		static const int  TARGET_FONT_HEIGHT	= 32;
		static const int  TARGET_FONT_LEFT		= (256 - (TARGET_FONT_WIDTH * FONTTYPE_MAX) ) / 2; // 72 ����ύX���܂����B

		
		u8				m_FontType;			// �t�H���g�̃T�C�Y�ύX
		DISPTYPE 		m_DispType;			// �\���t�H���g�̎�ނ̃C���f�b�N�X
		FONTCATEGORY 	m_CurCategory;		// 
		u16     		m_CurIndex;			// �I�𒆂̃t�H���g�̃C���f�b�N�X
		u16     		m_MaxIndex;			
		u16     		m_GroupNum;			// �t�H���g�O���[�vNo (��ʉE��̃O���[�v�̃C���f�b�N�X�ԍ�)
		u16     		m_GroupWidth;		// ���ʂ̂P���C���ӂ�̕\���t�H���g��
		u8	   			m_GroupCursor;		// 
		bool    		m_fDispUnderLine;	// ����ʁA�t�H���g���̃A���_�[���C���̗L��

		void ChangeCategory( FONTCATEGORY NewCategory, DISPTYPE	DispType );
		
		u16  GetFontID()  const 	{	return m_FontCategoryTbl[m_CurCategory].GetValue(m_CurIndex);	}
		
		u16  GetGroupID() const 	{	return static_cast<u16>( m_CurIndex / m_GroupNum );				}

	public:
		void 		Run();
		int  		CursorPos() const		{	return m_GroupCursor;	}
	};

//****************************************************************************
// ��ʏ�Ԃ��Ƃ̊Ǘ��\����		�e�L�X�g�\�����
//****************************************************************************
	struct Status_Text{
	private:
		//�����ʒ�`
		enum TextType {
			TYPE_HANGEUL,
			TYPE_JAPANESE,
			TYPE_ENGISH,
			
			TYPE_MAX
		};
		enum FontSize{
			SIZE_9x9,
			SIZE_10x10,
			SIZE_11x11,
			SIZE_11x12,
			SIZE_14x14,
			SIZE_15x15,
			
			SIZE_MAX
		};
		
		static const int  TARGET_WIDTH		= 3;
		
		//�����ʕ\���ʒu
		static const int TITLE_LEFT  		= 0;
		static const int TITLE_TOP			= 0;
		
		//�u�r�����������k���������������v���b�Z�[�W�\���ʒu
		static const int MES_LEFT  			= 0;
		static const int MES_TOP			= 64;
		
		//����I�𗓁@��`�l
		static const int LANG_LEFT  		= 16;
		static const int LANG_TOP   		= 104;
		static const int LANG_WIDTH 		= 80;
		static const int LANG_CURSOR_LEFT 	= LANG_LEFT;
		static const int LANG_CURSOR_TOP 	= LANG_TOP -20;
		static const int LANG_CURSOR_WIDTH 	= LANG_WIDTH;
		static const u16 LANG_CURSOR_CODE	= 0xe01C;
		
		//�e�L�X�g�m���@��`�l
		static const int TEXT_NO_LEFT  		= 0;
		static const int TEXT_NO_TOP		= 0;
		
		//�e�L�X�g�@��`�l
		static const int TEXT_LEFT  		= 0;
		static const int TEXT_TOP			= 32;
		
		//�t�H���g�T�C�Y�@��`�l
		static const int SIZE_LEFT  		= 0;
		static const int SIZE_TOP			= 192 - 20;
		
		int m_CurIndex;				//���ݕ\���C���f�b�N�X
		int m_CurTextType;			//������
		int m_CurFontSize;			//�t�H���g�T�C�Y
	public:
		void Run();
	};


//****************************************************************************
// ���W���[���ϐ�
//****************************************************************************
	//��ʏ��
	static  Status 					m_Status;

	// �t�H���g
	static  NNSG2dFont				m_Font;
	static  NNSG2dFont				m_Font_9x9;
//	static  NNSG2dFont				m_Font_10x10; // ����Ȃ�
	static  NNSG2dFont				m_Font_11x11;
//	static  NNSG2dFont				m_Font_11x12; // ����Ȃ�
//	static  NNSG2dFont				m_Font_14x14; // ����Ȃ�
	#define m_Font_15x15			m_Font

    static  NNSG2dFont				m_Font_11x13;
    static  NNSG2dFont				m_Font_13x16;
    static  NNSG2dFont				m_Font_17x21;
                                                                      
	//���L��
	static  MainWnd 				m_MainWnd;
	static  SubWnd  				m_SubWnd;
	static  NNSGfdVramTransferTask  m_TrasTask[TRANS_TASK_NUM];
	static  LOB::RepeatReader* 		m_pKeyReader;			//�O���[�o���R���X�g���N�^�������Ȃ��̂�
															//�|�C���^�ɂ��ăC���X�^���X���蓖�Ă̓��C���ŁB
	//��ʏ�Ԃ��Ƃ̊Ǘ��\����
	static Status_Select 			m_Status_Select;
	static Status_Font   			m_Status_Font;
	static Status_Text   			m_Status_Text;

//****************************************************************************
// �v���g�^�C�v
//****************************************************************************
	//�S�̏���
	static int  Init();
	static void Loop();
	static void HBlankIntr();
	static void VBlankIntr();

	//�t�H���g�\����Ԃ̃��[�`���Q
	static int  GetCharWidth(const NNSG2dCharWidths* pWidths);

//****************************************************************************
// �R�[�h
//****************************************************************************

	/*------------------------------------------------*//**
	 *	@brief		���C��
	 *	
	 *	
	 *//*------------------------------------------------*/
	void FontView()
	{
		//�L�[���[�_�[�̏�����
		LOB::RepeatReader pad;			//�R���X�g���N�^�𑖂点��
		m_pKeyReader = &pad;			//�|�C���^�Z�b�g
		
		// Initilizing App.
		if(!Init()){
			return ;
		}
		
		// Main loop
		Loop();
	}

	/*------------------------------------------------*//**
	 *	@brief		������
	 *	@return 	����
	 *//*------------------------------------------------*/
	static int Init()
	{
		// SDK �ƃf�����C�u������������
		OS_Init();
		TXT_Init( HBlankIntr, VBlankIntr );
		
	    //Main Display
		GX_SetBankForBG(GX_VRAM_BG_256_AB);
	    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D);
		GX_SetBGScrOffset(GX_BGSCROFFSET_0x30000);
	    GX_SetBGCharOffset(GX_BGCHAROFFSET_0x00000);
		
		//Sub Display
	    GX_SetBankForSubBG(GX_VRAM_SUB_BG_128_C);
	    GXS_SetGraphicsMode(GX_BGMODE_0);
		
		//�w�i�摜�ǂݍ���
		TXT_SetupBackground();
		
		// �t�H���g��ǂݍ��݂܂�
		{
			void* pFontFile;
			u32 size = TXT_LoadFile( &pFontFile, "data/HANGUL_9x9.NFTR" );
			NNS_G2D_ASSERT( size > 0 );
			NNS_G2dFontInitUTF16(&m_Font_9x9, pFontFile);
			NNS_G2dPrintFont(&m_Font_9x9);
		}
/*		{
			void* pFontFile;
			u32 size = TXT_LoadFile( &pFontFile, "data/HANGUL_10x10.NFTR" );
			NNS_G2D_ASSERT( size > 0 );
			NNS_G2dFontInitUTF16(&m_Font_10x10, pFontFile);
			NNS_G2dPrintFont(&m_Font_10x10);
		}*/
		{
			void* pFontFile;
			u32 size = TXT_LoadFile( &pFontFile, "data/HANGUL_11x11.NFTR" );
			NNS_G2D_ASSERT( size > 0 );
			NNS_G2dFontInitUTF16(&m_Font_11x11, pFontFile);
			NNS_G2dPrintFont(&m_Font_11x11);
		}
/*		{
			void* pFontFile;
			u32 size = TXT_LoadFile( &pFontFile, "data/HANGUL_11x12.NFTR" );
			NNS_G2D_ASSERT( size > 0 );
			NNS_G2dFontInitUTF16(&m_Font_11x12, pFontFile);
			NNS_G2dPrintFont(&m_Font_11x12);
		}*/
/*		{
			void* pFontFile;
			u32 size = TXT_LoadFile( &pFontFile, "data/HANGUL_14x14.NFTR" );
			NNS_G2D_ASSERT( size > 0 );
			NNS_G2dFontInitUTF16(&m_Font_14x14, pFontFile);
			NNS_G2dPrintFont(&m_Font_14x14);
		}*/
		{
			void* pFontFile;
			u32 size = TXT_LoadFile( &pFontFile, "data/HANGUL_15x15.NFTR" );
			NNS_G2D_ASSERT( size > 0 );
			NNS_G2dFontInitUTF16(&m_Font_15x15, pFontFile);
			NNS_G2dPrintFont(&m_Font_15x15);
		}

      // �V�K�ǉ� -----------------------------------------
		{
			void* pFontFile;
			u32 size = TXT_LoadFile( &pFontFile, "data/shared_s.NFTR" );
			NNS_G2D_ASSERT( size > 0 );
			NNS_G2dFontInitUTF16(&m_Font_11x13, pFontFile);
			NNS_G2dPrintFont(&m_Font_11x13);
		}
		{
			void* pFontFile;
			u32 size = TXT_LoadFile( &pFontFile, "data/shared_m.NFTR" );
			NNS_G2D_ASSERT( size > 0 );
			NNS_G2dFontInitUTF16(&m_Font_13x16, pFontFile);
			NNS_G2dPrintFont(&m_Font_13x16);
		}
		{
			void* pFontFile;
			u32 size = TXT_LoadFile( &pFontFile, "data/shared_l.NFTR" );
			NNS_G2D_ASSERT( size > 0 );
			NNS_G2dFontInitUTF16(&m_Font_17x21, pFontFile);
			NNS_G2dPrintFont(&m_Font_17x21);
		}
      
		//�e�L�X�g�E�C���h�E������
		m_MainWnd.Init(&m_Font);
		m_SubWnd.Init(&m_Font);
		
		// �J���[�p���b�g��ݒ�
		GX_LoadBGPltt(TXTColorPalette,  0, sizeof(TXTColorPalette));
		GXS_LoadBGPltt(TXTColorPalette, 0, sizeof(TXTColorPalette));
		
		// start display
		CMN_WaitVBlankIntr();
		GX_DispOn();
		GXS_DispOn();
		
		// �I�t�X�N���[���o�b�t�@�̓]����VRAM�]���}�l�[�W�����g���܂�
	    NNS_GfdInitVramTransferManager(m_TrasTask, TRANS_TASK_NUM);
		
		//�A�v���P�[�V����������
		m_Status = STATUS_SELECT;
		return 1;
	}

	/*------------------------------------------------*//**
	 *	@brief	���C�����[�v
	 *	
	 *		
	 *//*------------------------------------------------*/
	static void Loop()
	{
		while( TRUE ){
			//���ݏ�Ԃɍ��킹��������s���B
			switch( m_Status ){
			case STATUS_SELECT:
				//�I�����ʂ����̉�ʂƂ���
				m_Status = m_Status_Select.Run();
				break;
			case STATUS_FONT:
				m_Status_Font.Run();
				break;
			case STATUS_TEXT:
				m_Status_Text.Run();
				break;
			}
		}
	}

	/*---------------------------------------------------------------------------*
	  Name:         VBlankIntr

	  Description:  V�u�����N���荞�݂��������܂��B

	  Arguments:    �Ȃ��B

	  Returns:      �Ȃ��B
	 *---------------------------------------------------------------------------*/
	static void VBlankIntr()
	{
	    OS_SetIrqCheckFlag( OS_IE_V_BLANK );                   // checking VBlank interrupt
	}

	/*---------------------------------------------------------------------------*
	  Name:         HBlankIntr

	  Description:  H�u�����N���荞�݂��������܂��B

	  Arguments:    �Ȃ��B

	  Returns:      �Ȃ��B
	 *---------------------------------------------------------------------------*/
	static void HBlankIntr()
	{
		//�t�H���g�\����ʂł́A���ʂa�f�̃J�[�\����
		//�����A�����Ԋ��荞�݂�p���ĕ\���I�t�Z�b�g���䂷��B
		if( m_Status == STATUS_FONT ){
			
			//VCOUNT�l�̎擾
			const int count = GX_GetVCount();
			
			//�\�����ԊO�ɂȂ�����
			if( count == 192 ){
				//�X�N���[���I�t�Z�b�g�̃��Z�b�g
				G2_SetBG1Offset( 0, 0 );
			}else{
				//�J�[�\������̂Ƃ�
				if( m_Status_Font.CursorPos() == 0 ){
					//�J�[�\�����Z�b�g
	//				if( count == 40 ){
	//					G2_SetBG1Offset( 0, 0 );
	//				}
				} else {
				//�J�[�\�������̂Ƃ�
					if( count == 40 ){
						G2_SetBG1Offset( 0, -96 );
					}else if( count == 80 ){
						G2_SetBG1Offset( 0, -40 );
					}
				}
			}
		}
		
	    OS_SetIrqCheckFlag( OS_IE_H_BLANK );     // checking HBlank interrupt
	}

	/*------------------------------------------------*//**
	 *	@brief		�t�H���g���Z�o
	 *	@return		�t�H���g��
	 *//*------------------------------------------------*/
	static int GetCharWidth(const NNSG2dCharWidths* pWidths)
	{
//	#ifdef NNS_G2D_FONT_USE_OLD_RESOURCE
//	    if( m_Font.isOldVer ){
//	        return pWidths->left + pWidths->glyphWidth;
//	    }
//	    else
//	#endif
	    return pWidths->charWidth;
	}

//****************************************************************************
// ��ʏ�Ԃ��Ƃ̊Ǘ��\����		�t�H���g�\�����
//****************************************************************************
	
	/*------------------------------------------------*//**
	 *	@brief		�J�e�S����ύX����
	 *	@param		�V�K�J�e�S��
	 *	@param		��i�s�^�C�v
	 *//*------------------------------------------------*/
	void Status_Font::ChangeCategory( FONTCATEGORY NewCategory, DISPTYPE	DispType )
	{
		//��i�^�C�v������
		m_DispType 	  = DispType;
		switch( m_DispType ){
		case DISPTYPE_8x2:	
			m_GroupNum   = 16;
			m_GroupWidth = 8;
			break;
		case DISPTYPE_10x2:	
			m_GroupNum   = 20;
			m_GroupWidth = 10;
			break;
		};
		
		//�J�e�S��������
		m_CurCategory = NewCategory;
		m_CurIndex    = 0;
		m_MaxIndex    = static_cast<u16>(m_FontCategoryTbl[m_CurCategory].GetCount()-1);
		
		OS_Printf( "CurCategory = %d\n", m_CurCategory );
		OS_Printf( "MaxIndex = %d\n",    m_MaxIndex );
	}
	
	/*------------------------------------------------*//**
	 *	@brief		���C�����[�v
	 *//*------------------------------------------------*/
	void Status_Font::Run()
	{
		//������ 
	 	TXT_EnableBackground();
		ChangeCategory( CATEGORY_KS_X_1001_Hangul, DISPTYPE_8x2 );
		m_FontType = 0;
		m_fDispUnderLine = true;
		
		//���[�v����
		for( u32 Frame=0; ; Frame++ )
		{
			//�v�Z�����@
			int fModify = FALSE;
			{
				//�Q�[���p�b�h���擾
				u16 Rept = m_pKeyReader->Update();
				u16 Trig = m_pKeyReader->Trig();
				u16 Cont = m_pKeyReader->Cont();
				
				//�\�t�g���Z�b�g�`�F�b�N
				if( Cont == (PAD_BUTTON_R|PAD_BUTTON_L|PAD_BUTTON_A|PAD_BUTTON_B|PAD_BUTTON_X|PAD_BUTTON_Y) ){
					OS_ResetSystem( 0 ); 
				}
				
				//�k�q�ɂ��O���[�v�h�c�ύX
				if( Rept & PAD_BUTTON_R ){
					fModify = TRUE;
					
					m_CurIndex += m_GroupNum;
					if( m_CurIndex > m_MaxIndex ){
						m_CurIndex = 0;
					}
				}
				if( Rept & PAD_BUTTON_L ){
					fModify = TRUE;
					
					if( m_CurIndex < m_GroupNum ){
						m_CurIndex = m_MaxIndex ;
					}else{
						m_CurIndex -= m_GroupNum;
					}
				}
				
				//�x�{�^���ɂ����ʂ̃t�H���g��ޕύX 
				if( Trig & PAD_BUTTON_Y ){
					fModify = TRUE;

					m_FontType ++;
					if( m_FontType >= FONTTYPE_MAX ){
						m_FontType = 0;
					}
				}
				
				//���E�Ƃ`�a�Ńt�H���g�ړ�
				if( Rept & (PAD_KEY_RIGHT | PAD_BUTTON_A)  ){
					fModify = TRUE;
					
					m_CurIndex ++;
					if( m_CurIndex > m_MaxIndex ){
						m_CurIndex = 0;
					}
				}
				if( Rept & (PAD_KEY_LEFT | PAD_BUTTON_B ) ){
					fModify = TRUE;
					
					if( m_CurIndex == 0 ){
						m_CurIndex = m_MaxIndex ;
					}else{
						m_CurIndex --;
					}
				}
				
				//���f�B�X�v���C�̕������\���t���O
				if( Trig & PAD_BUTTON_X ){
					fModify = TRUE;
					if( m_fDispUnderLine ){
						m_fDispUnderLine = false;
					}else{
						m_fDispUnderLine = true;
					}
				}
				
				//�t�H���g�J�e�S���̕ύX
				if( Trig & PAD_BUTTON_START ){
					fModify = TRUE;
					ChangeCategory( static_cast<FONTCATEGORY>((m_CurCategory+1) % CATEGORY_MAX), m_DispType );
				}
			
				//�\���^�C�v�̕ύX
				if( Trig & PAD_BUTTON_SELECT ){
					fModify = TRUE;
					ChangeCategory( m_CurCategory, static_cast<DISPTYPE>((m_DispType+1) % DISPTYPE_MAX) );
				}
			}
			
			m_GroupCursor = static_cast<u8>( m_CurIndex % m_GroupNum / m_GroupWidth );
			
			//�����t���[���ȍ~�͕ύX���������ꍇ�̂݉�ʍX�V
			if( Frame==0 || fModify )
			{
				//�e�L�X�g�̈�̃N���A
				m_MainWnd.Clear();
				m_SubWnd.Clear();
				
				//GroupID�̕\��
				m_MainWnd.PrintDec( &m_Font_15x15, GROUP_ID_X, GROUP_ID_Y, GetGroupID()+1 );
				
				//GroupID�̎����O���[�v�Q�O������\�� (���ʕ\��)
				for( int y = 0; y<GROUP_HEIGHT; y++ ){
					for( int x = 0; x<m_GroupWidth; x++ ){
						//�t�H���g�؂�ւ�
						static const NNSG2dFont* const fonts[FONTTYPE_MAX]={
							&m_Font_9x9,
						//	&m_Font_10x10,
							&m_Font_11x11,
						//	&m_Font_11x12,
						//	&m_Font_14x14,
							&m_Font_15x15,
						};
						
						//�擪��������̃I�t�Z�b�g�����߁A�`�悷��
						int Index = x + y*m_GroupWidth + ( GetGroupID() * m_GroupNum );
						
						//�\���͈͊O�ɒB�����甲����
						if( Index > m_MaxIndex ){
							break;
						}
						
						//�`��p�����[�^
						int xPos = GROUP_FONT_LEFT + x*GROUP_FONT_WIDTH;
						int yPos = GROUP_FONT_TOP  + y*GROUP_FONT_HEIGHT;
						u16 c    = m_FontCategoryTbl[m_CurCategory].GetValue(Index);
						
						//�����o��
						m_MainWnd.PrintChar( fonts[m_FontType], xPos, yPos, c );

						//�������\��
						if( m_GroupCursor == y ){
				            NNSG2dGlyph g;
				            NNS_G2dFontGetGlyph(&g, fonts[m_FontType], c );
							m_MainWnd.Fill( xPos, yPos + NNS_G2dFontGetHeight(fonts[m_FontType]), GetCharWidth(g.pWidths), 1 );
						}
					}
				}
				
				//�t�H���g�J�e�S���̕\��
				m_MainWnd.PrintString( &m_Font_9x9, FONTTITLE_X, FONTTITLE_Y, m_FontCategoryTbl[m_CurCategory].GetTitle() );
				
				//FontID�̕\��
				m_SubWnd.PrintHex( &m_Font_15x15, FONT_ID_X, FONT_ID_Y, GetFontID() );

				//�^�[�Q�b�g�E�C���h�E�̕\�� (����ʂ̕\��)
				for( int y = 0; y<TARGET_HEIGHT; y++ ){
					for( int x = 0; x<TARGET_WIDTH; x++ ){
                      static NNSG2dFont* fonts[TARGET_WIDTH];
                      
						//�t�H���g�؂�ւ�
                      if(m_CurCategory == 3 || m_CurCategory == 4){
                        fonts[0] = &m_Font_17x21;
						fonts[1] = &m_Font_13x16;
						fonts[2] = &m_Font_11x13;
                      }
                      else{
                        fonts[0] = &m_Font_15x15;
						fonts[1] = &m_Font_11x11;
						fonts[2] = &m_Font_9x9;
                      }

                        //�����F�؂�ւ�
						static int colors[TARGET_HEIGHT]={
							TXT_COLOR_WHITE,
							TXT_COLOR_BLACK,
						};
						
						//�\���ʒu�̌���
						int xPos = TARGET_FONT_LEFT + x*TARGET_FONT_WIDTH;
						int yPos = TARGET_FONT_TOP  + y*TARGET_FONT_HEIGHT - NNS_G2dFontGetHeight(fonts[x]);
						u16 c    = m_FontCategoryTbl[m_CurCategory].GetValue(m_CurIndex);
						
						//�������h��
						m_SubWnd.Fill( xPos, yPos , 
							NNS_G2dFontGetCharWidth(fonts[x], c ), 
							NNS_G2dFontGetHeight(fonts[x]), colors[ (y+1) & 0x01] );
						
						//�����\��
						m_SubWnd.PrintChar( fonts[x], xPos, yPos, c, colors[y] );
						
						//�����̕\��
						if( m_fDispUnderLine ){
				            NNSG2dGlyph g;
				            NNS_G2dFontGetGlyph(&g, fonts[x], c );
							m_SubWnd.Fill( xPos, yPos + NNS_G2dFontGetHeight(fonts[x]), GetCharWidth(g.pWidths), 1, colors[y] );
						}
					}
				}

				//�Z���N�g�E�C���h�E�̕\��
				for( int i=0; i<m_GroupWidth; i++ ){
					int Index = (m_CurIndex / m_GroupWidth * m_GroupWidth)  + i;
					//�\���͈͊O�ɒB�����甲����
					if( Index > m_MaxIndex ){
						break;
					}
					u16 c    = m_FontCategoryTbl[m_CurCategory].GetValue(Index);
					m_SubWnd.PrintChar( &m_Font, SELECT_LEFT + i*SELECT_WIDTH, SELECT_TOP, c );
				}
				
				//�Z���N�g�E�C���h�E �J�[�\���̕\��
				m_SubWnd.PrintChar( &m_Font, SELECT_LEFT + SELECT_WIDTH * (m_CurIndex % m_GroupWidth), CURSOR_TOP, CURSOR_CODE );
			}
				
			//VBLANK���荞�݂�҂�
			CMN_WaitVBlankIntr();
			
			//�]���^�X�N�o�^
			m_MainWnd.Draw();
			m_SubWnd.Draw();
	       
			//�]�����s
			NNS_GfdDoVramTransfer();
		}
	}

//****************************************************************************
// ��ʏ�Ԃ��Ƃ̊Ǘ��\����		�I�����
//****************************************************************************
	Status Status_Select::Run()
	{
		//������ 
	 	TXT_DisableBackground();
		int fContinue = TRUE;
		m_CursorIndex = 0;
		
		//���[�v����
		for( u32 Frame=0; fContinue ; Frame++ )
		{
			//�v�Z�����@
			int fModify = FALSE;
			{
				//�Q�[���p�b�h���擾
				u16 Rept = m_pKeyReader->Update();
				u16 Trig = m_pKeyReader->Trig();
				
				//�J�[�\���ړ�
				if( Trig & PAD_KEY_UP ){
					fModify = TRUE;
					if( m_CursorIndex == 0 ){
						m_CursorIndex = MENU_NUM;
					}
					m_CursorIndex --;
				}
				if( Trig & PAD_KEY_DOWN ){
					fModify = TRUE;
					m_CursorIndex ++;
					if( m_CursorIndex == MENU_NUM ){
						m_CursorIndex = 0;
					}
				}
				//����@
				if( Trig & PAD_BUTTON_A ){
					fContinue = FALSE;
				}
			}
			
			//�����t���[���ȍ~�͕ύX���������ꍇ�̂݉�ʍX�V
			if( Frame==0 || fModify )
			{
				//�e�L�X�g�̈�̃N���A
				m_MainWnd.Clear();
				m_SubWnd.Clear();
				
				//���j���[�\��
				for( int i=0; i<MENU_NUM; i++ ){
					static const char * const MenuStr[MENU_NUM] = {
						"Font Check Mode",
						"Text Check Mode",
					};
					m_MainWnd.PrintString( &m_Font, MENU_LEFT, MENU_TOP + MENU_HEIGHT*i, MenuStr[i] );
				}
				
				//�J�[�\���̕\��
				m_MainWnd.PrintChar( &m_Font, CURSOR_LEFT, CURSOR_TOP+MENU_HEIGHT*m_CursorIndex, CURSOR_CODE );
			}
				
			//VBLANK���荞�݂�҂�
			CMN_WaitVBlankIntr();
			
			//�]���^�X�N�o�^
			m_MainWnd.Draw();
			m_SubWnd.Draw();
	       
			//�]�����s
			NNS_GfdDoVramTransfer();
		}
		
		//���ʂƂȂ鎟�̏�Ԃ�Ԃ�
		if( m_CursorIndex == 0 ){
			return STATUS_FONT;
		}else{
			return STATUS_TEXT;
		}
	}

//****************************************************************************
// ��ʏ�Ԃ��Ƃ̊Ǘ��\����		�t�H���g�\�����
//****************************************************************************
	
	void Status_Text::Run()
	{
		//������ 
	 	TXT_DisableBackground();
		
		m_CurTextType = TYPE_JAPANESE;
		m_CurIndex    = 0;
		m_CurFontSize = SIZE_15x15;
		
		//���[�v����
		for( u32 Frame=0;  ; Frame++ )
		{
			//�v�Z�����@
			int fModify = FALSE;
			{
				//�Q�[���p�b�h���擾
				u16 Rept = m_pKeyReader->Update();
				u16 Trig = m_pKeyReader->Trig();
				u16 Cont = m_pKeyReader->Cont();
				
				//�\�t�g���Z�b�g�`�F�b�N
				if( Cont == (PAD_BUTTON_R|PAD_BUTTON_L|PAD_BUTTON_A|PAD_BUTTON_B|PAD_BUTTON_X|PAD_BUTTON_Y) ){
					OS_ResetSystem( 0 ); 
				}
				
				//�㉺�ŕ\���e�L�X�g��I������
				if( Rept & PAD_KEY_UP ){
					fModify = TRUE;
					if( m_CurIndex == 0 ){
						m_CurIndex = g_TextTable_Num;
					}
					m_CurIndex --;
				}
				if( Rept & PAD_KEY_DOWN ){
					fModify = TRUE;
					m_CurIndex ++;
					if( m_CurIndex == g_TextTable_Num ){
						m_CurIndex = 0;
					}
				}
				
				//���E�Ŏ�ʐ؂�ւ�
				if( Rept & PAD_KEY_LEFT ){
					fModify = TRUE;
					if( m_CurTextType == 0 ){
						m_CurTextType = TYPE_MAX;
					}
					m_CurTextType --;
				}
				if( Rept & PAD_KEY_RIGHT ){
					fModify = TRUE;
					m_CurTextType ++;
					if( m_CurTextType == TYPE_MAX ){
						m_CurTextType = 0;
					}
				}
				
				//�t�H���g�T�C�Y�؂�ւ�
				if( Trig & PAD_BUTTON_Y ){
					fModify = TRUE;
					m_CurFontSize ++;
					if( m_CurFontSize == SIZE_MAX ){
						m_CurFontSize = 0;
					}
				}
			}
			
			//�����t���[���ȍ~�͕ύX���������ꍇ�̂݉�ʍX�V
			if( Frame==0 || fModify )
			{
				//�e�L�X�g�̈�̃N���A
				m_MainWnd.Clear();
				m_SubWnd.Clear();
				
				m_MainWnd.PrintString( &m_Font, TITLE_LEFT, TITLE_TOP, "TEXT CHECK MODE" );			
				//���݌���\��
				m_MainWnd.PrintString( &m_Font, MES_LEFT, MES_TOP, "Select Language" );			
				for( int i=0; i<TYPE_MAX; i++ ){
					m_MainWnd.PrintUnicodeString( &m_Font, LANG_LEFT+ i*LANG_WIDTH, LANG_TOP, g_TextTypes[i] );			
				}
				//���݌���J�[�\���\��
				m_MainWnd.PrintChar( &m_Font, LANG_CURSOR_LEFT+ m_CurTextType*LANG_CURSOR_WIDTH, LANG_CURSOR_TOP, LANG_CURSOR_CODE );			
				
				//�t�H���g�T�C�Y���\��
				static const char * const SizeMes[] = {
					" 9x 9",
					"10x10",
					"11x11",
					"11x12",
					"14x14",
					"15x15",
				};
				m_SubWnd.PrintString( &m_Font, SIZE_LEFT, SIZE_TOP, SizeMes[m_CurFontSize] );
				
				//���݃C���f�b�N�X�\��
				m_SubWnd.PrintDec( &m_Font, TEXT_NO_LEFT, TEXT_NO_TOP, m_CurIndex );			

				//�Ώۃe�L�X�g�\��
				static const NNSG2dFont* const fonts[SIZE_MAX]={
					&m_Font_9x9,
				//	&m_Font_10x10,
					&m_Font_11x11,
				//	&m_Font_11x12,
				//	&m_Font_14x14,
					&m_Font_15x15,
				};
				m_SubWnd.PrintUnicodeString( fonts[m_CurFontSize], TEXT_LEFT, TEXT_TOP, g_TextTable[m_CurIndex][m_CurTextType] );
			}
				
			//VBLANK���荞�݂�҂�
			CMN_WaitVBlankIntr();
			
			//�]���^�X�N�o�^
			m_MainWnd.Draw();
			m_SubWnd.Draw();
	       
			//�]�����s
			NNS_GfdDoVramTransfer();
		}
	}
