/*===============================================================*//**
 	@file		FontView.cpp
 	
 	@brief		NITRO-SYSTEM のデモプログラム「UTF16Text」を
 				ベースにすべてのフォントを表示するビューアです
 				
 	@author		Kazumasa Hirata

*//*================================================================*/

//****************************************************************************
// Include
//****************************************************************************
	#include "FontView.h"

	//任天堂ライブラリ
	#include <nitro.h>

	//任天堂デモ用ライブラリ（一部改変）
	#include "nnsys_demo_lib/txt.h"

	//Floorライブラリ（キー情報取得）
	#include "lob_pad_lib/Pad.hpp"

	//アプリケーションヘルパ（テキスト処理）
	#include "TextWnd.h"
	
	//文字列テーブルデータファイルヘッダ
	#include "TextTable.h"

//****************************************************************************
// Define
//****************************************************************************
	//画面状態
	enum Status{
		STATUS_SELECT,		//
		STATUS_FONT,		//
		STATUS_TEXT,		//
		STATUS_MAX			//
	};

	//１フレームの転送タスク数
	#define TRANS_TASK_NUM  	2

//****************************************************************************
// 外部変数
//****************************************************************************
	//グループごとの文字コードテーブル
/*	extern const unsigned short DS[];		
	extern const unsigned short ASCII[];		
	extern const unsigned short JIS_X_0201_Half_width_KATAKANA[];		
	extern const unsigned short Code_Page_1252[];		
	extern const unsigned short ISO_8859_1_Latin_1[];		
	extern const unsigned short JIS_X_0208_Not_Hanja[];		*/
	extern const unsigned short KS_X_1001_Hangul[];	
	extern const unsigned short KS_X_1001_Jamo[];	
	extern const unsigned short additional_Hangul[];
    extern const unsigned short wii_additional_Hangul[];		// トゥルータイプフォント(2360 + 8844)

	//文字コードテーブルの要素数定数
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

	//文字コードテーブルの名前
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
// フォントカテゴリー	構造体
//****************************************************************************
	//定義
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

	//文字コードテーブルと要素数を関連付けたフォントカテゴリ構造体
	struct FontCategory{
	//privateにするとグローバル定義できないため外す。気持ちはprivate
		const u16    * pArray;
		const u16    * pCount;
		const LPCSTR * pTitle;
	//簡単な使い方を提供するインターフェイス
	public:	
		u16 GetCount() const			{	return *pCount;			}
		const u16 * GetArray() const	{	return pArray;			}
		LPCSTR GetTitle() const			{	return *pTitle;			}
		u16 GetValue( int index ) const	{	return pArray[index];	}
	};

	//フォントカテゴリテーブル
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
        { KS_X_1001_Hangul,        			&Count_of_KS_X_1001_Hangul 				,	&Title_of_wii_TrueTypeFont_Hangul			}, // 新規追加 (Wiiトゥルータイプフォント)
        { wii_additional_Hangul,         	&Count_of_wii_additional_Hangul			,	&Title_of_wii_additional_Hangul 			}, // 新規追加 (Wii拡張トゥルータイプフォント)
	};

//****************************************************************************
// 画面状態ごとの管理構造体		選択状態
//****************************************************************************
	struct Status_Select{
	private:
		//選択肢　定義値
		static const int  MENU_LEFT			= 64;					//座標
		static const int  MENU_TOP			= 64;					//	”
		static const int  MENU_HEIGHT		= 20;					//縦方向オフセット
		static const int  MENU_NUM			= 2;					//個数
		//カーソル　定義値	
		static const int  CURSOR_LEFT		= (MENU_LEFT-20);		//座標
		static const int  CURSOR_TOP		= MENU_TOP;				//
		static const u16  CURSOR_CODE		= 0xe019;				//表示する文字のコード
		
		u8	   m_CursorIndex;
	public:
		Status Run();
	};

//****************************************************************************
// 画面状態ごとの管理構造体		フォント表示状態
//****************************************************************************
	struct Status_Font{
	private:
		//表示
		enum DISPTYPE{
			DISPTYPE_8x2,
			DISPTYPE_10x2,
			DISPTYPE_MAX
		};
		
		//グループウインドウ定義値
		static const int  GROUP_HEIGHT 			= 2	;	// １グループの中の１列の数
		static const int  GROUP_ID_X			= 180;
		static const int  GROUP_ID_Y			= 13;
		static const int  GROUP_FONT_LEFT		= 32;
		static const int  GROUP_FONT_TOP		= 62;
		static const int  GROUP_FONT_WIDTH		= 20;
		static const int  GROUP_FONT_HEIGHT		= 40;

		//フォントID定義値
		static const int  FONT_ID_X				= 110;
		static const int  FONT_ID_Y				= 14;
		
		//セレクトウインドウ定義値
		static const int  SELECT_LEFT			= 10;
		static const int  SELECT_TOP			= 165;
		static const int  SELECT_WIDTH			= 24;
		
		//カーソル定義値
		static const int  CURSOR_TOP			= (SELECT_TOP-20);
		static const u16  CURSOR_CODE			= 0xe01C;
			
		//フォント名 定義値
		static const int  FONTTITLE_X			= 48;
		static const int  FONTTITLE_Y			= 168;
		static const int  FONTTYPE_MAX			= 3;

		//ターゲットウインドウ定義値
		static const int  TARGET_WIDTH			= 3;
		static const int  TARGET_HEIGHT			= 2;
		static const int  TARGET_FONT_TOP		= 76;
		static const int  TARGET_FONT_WIDTH		= 24;
		static const int  TARGET_FONT_HEIGHT	= 32;
		static const int  TARGET_FONT_LEFT		= (256 - (TARGET_FONT_WIDTH * FONTTYPE_MAX) ) / 2; // 72 から変更しました。

		
		u8				m_FontType;			// フォントのサイズ変更
		DISPTYPE 		m_DispType;			// 表示フォントの種類のインデックス
		FONTCATEGORY 	m_CurCategory;		// 
		u16     		m_CurIndex;			// 選択中のフォントのインデックス
		u16     		m_MaxIndex;			
		u16     		m_GroupNum;			// フォントグループNo (画面右上のグループのインデックス番号)
		u16     		m_GroupWidth;		// 上画面の１ライン辺りの表示フォント数
		u8	   			m_GroupCursor;		// 
		bool    		m_fDispUnderLine;	// 下画面、フォント下のアンダーラインの有無

		void ChangeCategory( FONTCATEGORY NewCategory, DISPTYPE	DispType );
		
		u16  GetFontID()  const 	{	return m_FontCategoryTbl[m_CurCategory].GetValue(m_CurIndex);	}
		
		u16  GetGroupID() const 	{	return static_cast<u16>( m_CurIndex / m_GroupNum );				}

	public:
		void 		Run();
		int  		CursorPos() const		{	return m_GroupCursor;	}
	};

//****************************************************************************
// 画面状態ごとの管理構造体		テキスト表示状態
//****************************************************************************
	struct Status_Text{
	private:
		//言語種別定義
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
		
		//言語種別表示位置
		static const int TITLE_LEFT  		= 0;
		static const int TITLE_TOP			= 0;
		
		//「ＳｅｌｅｃｔＬａｎｇｕａｇｅ」メッセージ表示位置
		static const int MES_LEFT  			= 0;
		static const int MES_TOP			= 64;
		
		//言語選択欄　定義値
		static const int LANG_LEFT  		= 16;
		static const int LANG_TOP   		= 104;
		static const int LANG_WIDTH 		= 80;
		static const int LANG_CURSOR_LEFT 	= LANG_LEFT;
		static const int LANG_CURSOR_TOP 	= LANG_TOP -20;
		static const int LANG_CURSOR_WIDTH 	= LANG_WIDTH;
		static const u16 LANG_CURSOR_CODE	= 0xe01C;
		
		//テキストＮｏ　定義値
		static const int TEXT_NO_LEFT  		= 0;
		static const int TEXT_NO_TOP		= 0;
		
		//テキスト　定義値
		static const int TEXT_LEFT  		= 0;
		static const int TEXT_TOP			= 32;
		
		//フォントサイズ　定義値
		static const int SIZE_LEFT  		= 0;
		static const int SIZE_TOP			= 192 - 20;
		
		int m_CurIndex;				//現在表示インデックス
		int m_CurTextType;			//言語種別
		int m_CurFontSize;			//フォントサイズ
	public:
		void Run();
	};


//****************************************************************************
// モジュール変数
//****************************************************************************
	//画面状態
	static  Status 					m_Status;

	// フォント
	static  NNSG2dFont				m_Font;
	static  NNSG2dFont				m_Font_9x9;
//	static  NNSG2dFont				m_Font_10x10; // いらない
	static  NNSG2dFont				m_Font_11x11;
//	static  NNSG2dFont				m_Font_11x12; // いらない
//	static  NNSG2dFont				m_Font_14x14; // いらない
	#define m_Font_15x15			m_Font

    static  NNSG2dFont				m_Font_11x13;
    static  NNSG2dFont				m_Font_13x16;
    static  NNSG2dFont				m_Font_17x21;
                                                                      
	//共有物
	static  MainWnd 				m_MainWnd;
	static  SubWnd  				m_SubWnd;
	static  NNSGfdVramTransferTask  m_TrasTask[TRANS_TASK_NUM];
	static  LOB::RepeatReader* 		m_pKeyReader;			//グローバルコンストラクタが動かないので
															//ポインタにしてインスタンス割り当てはメインで。
	//画面状態ごとの管理構造体
	static Status_Select 			m_Status_Select;
	static Status_Font   			m_Status_Font;
	static Status_Text   			m_Status_Text;

//****************************************************************************
// プロトタイプ
//****************************************************************************
	//全体処理
	static int  Init();
	static void Loop();
	static void HBlankIntr();
	static void VBlankIntr();

	//フォント表示状態のルーチン群
	static int  GetCharWidth(const NNSG2dCharWidths* pWidths);

//****************************************************************************
// コード
//****************************************************************************

	/*------------------------------------------------*//**
	 *	@brief		メイン
	 *	
	 *	
	 *//*------------------------------------------------*/
	void FontView()
	{
		//キーリーダーの初期化
		LOB::RepeatReader pad;			//コンストラクタを走らせて
		m_pKeyReader = &pad;			//ポインタセット
		
		// Initilizing App.
		if(!Init()){
			return ;
		}
		
		// Main loop
		Loop();
	}

	/*------------------------------------------------*//**
	 *	@brief		初期化
	 *	@return 	成否
	 *//*------------------------------------------------*/
	static int Init()
	{
		// SDK とデモライブラリを初期化
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
		
		//背景画像読み込み
		TXT_SetupBackground();
		
		// フォントを読み込みます
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

      // 新規追加 -----------------------------------------
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
      
		//テキストウインドウ初期化
		m_MainWnd.Init(&m_Font);
		m_SubWnd.Init(&m_Font);
		
		// カラーパレットを設定
		GX_LoadBGPltt(TXTColorPalette,  0, sizeof(TXTColorPalette));
		GXS_LoadBGPltt(TXTColorPalette, 0, sizeof(TXTColorPalette));
		
		// start display
		CMN_WaitVBlankIntr();
		GX_DispOn();
		GXS_DispOn();
		
		// オフスクリーンバッファの転送にVRAM転送マネージャを使います
	    NNS_GfdInitVramTransferManager(m_TrasTask, TRANS_TASK_NUM);
		
		//アプリケーション初期化
		m_Status = STATUS_SELECT;
		return 1;
	}

	/*------------------------------------------------*//**
	 *	@brief	メインループ
	 *	
	 *		
	 *//*------------------------------------------------*/
	static void Loop()
	{
		while( TRUE ){
			//現在状態に合わせた分岐を行う。
			switch( m_Status ){
			case STATUS_SELECT:
				//選択結果を次の画面とする
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

	  Description:  Vブランク割り込みを処理します。

	  Arguments:    なし。

	  Returns:      なし。
	 *---------------------------------------------------------------------------*/
	static void VBlankIntr()
	{
	    OS_SetIrqCheckFlag( OS_IE_V_BLANK );                   // checking VBlank interrupt
	}

	/*---------------------------------------------------------------------------*
	  Name:         HBlankIntr

	  Description:  Hブランク割り込みを処理します。

	  Arguments:    なし。

	  Returns:      なし。
	 *---------------------------------------------------------------------------*/
	static void HBlankIntr()
	{
		//フォント表示画面では、上画面ＢＧのカーソルを
		//水平帰線期間割り込みを用いて表示オフセット制御する。
		if( m_Status == STATUS_FONT ){
			
			//VCOUNT値の取得
			const int count = GX_GetVCount();
			
			//表示期間外になったら
			if( count == 192 ){
				//スクリーンオフセットのリセット
				G2_SetBG1Offset( 0, 0 );
			}else{
				//カーソルが上のとき
				if( m_Status_Font.CursorPos() == 0 ){
					//カーソルリセット
	//				if( count == 40 ){
	//					G2_SetBG1Offset( 0, 0 );
	//				}
				} else {
				//カーソルが下のとき
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
	 *	@brief		フォント幅算出
	 *	@return		フォント幅
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
// 画面状態ごとの管理構造体		フォント表示状態
//****************************************************************************
	
	/*------------------------------------------------*//**
	 *	@brief		カテゴリを変更する
	 *	@param		新規カテゴリ
	 *	@param		上段行タイプ
	 *//*------------------------------------------------*/
	void Status_Font::ChangeCategory( FONTCATEGORY NewCategory, DISPTYPE	DispType )
	{
		//上段タイプ初期化
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
		
		//カテゴリ初期化
		m_CurCategory = NewCategory;
		m_CurIndex    = 0;
		m_MaxIndex    = static_cast<u16>(m_FontCategoryTbl[m_CurCategory].GetCount()-1);
		
		OS_Printf( "CurCategory = %d\n", m_CurCategory );
		OS_Printf( "MaxIndex = %d\n",    m_MaxIndex );
	}
	
	/*------------------------------------------------*//**
	 *	@brief		メインループ
	 *//*------------------------------------------------*/
	void Status_Font::Run()
	{
		//初期化 
	 	TXT_EnableBackground();
		ChangeCategory( CATEGORY_KS_X_1001_Hangul, DISPTYPE_8x2 );
		m_FontType = 0;
		m_fDispUnderLine = true;
		
		//ループ処理
		for( u32 Frame=0; ; Frame++ )
		{
			//計算処理　
			int fModify = FALSE;
			{
				//ゲームパッド情報取得
				u16 Rept = m_pKeyReader->Update();
				u16 Trig = m_pKeyReader->Trig();
				u16 Cont = m_pKeyReader->Cont();
				
				//ソフトリセットチェック
				if( Cont == (PAD_BUTTON_R|PAD_BUTTON_L|PAD_BUTTON_A|PAD_BUTTON_B|PAD_BUTTON_X|PAD_BUTTON_Y) ){
					OS_ResetSystem( 0 ); 
				}
				
				//ＬＲによるグループＩＤ変更
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
				
				//Ｙボタンにより上画面のフォント種類変更 
				if( Trig & PAD_BUTTON_Y ){
					fModify = TRUE;

					m_FontType ++;
					if( m_FontType >= FONTTYPE_MAX ){
						m_FontType = 0;
					}
				}
				
				//左右とＡＢでフォント移動
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
				
				//下ディスプレイの文字幅表示フラグ
				if( Trig & PAD_BUTTON_X ){
					fModify = TRUE;
					if( m_fDispUnderLine ){
						m_fDispUnderLine = false;
					}else{
						m_fDispUnderLine = true;
					}
				}
				
				//フォントカテゴリの変更
				if( Trig & PAD_BUTTON_START ){
					fModify = TRUE;
					ChangeCategory( static_cast<FONTCATEGORY>((m_CurCategory+1) % CATEGORY_MAX), m_DispType );
				}
			
				//表示タイプの変更
				if( Trig & PAD_BUTTON_SELECT ){
					fModify = TRUE;
					ChangeCategory( m_CurCategory, static_cast<DISPTYPE>((m_DispType+1) % DISPTYPE_MAX) );
				}
			}
			
			m_GroupCursor = static_cast<u8>( m_CurIndex % m_GroupNum / m_GroupWidth );
			
			//初期フレーム以降は変更があった場合のみ画面更新
			if( Frame==0 || fModify )
			{
				//テキスト領域のクリア
				m_MainWnd.Clear();
				m_SubWnd.Clear();
				
				//GroupIDの表示
				m_MainWnd.PrintDec( &m_Font_15x15, GROUP_ID_X, GROUP_ID_Y, GetGroupID()+1 );
				
				//GroupIDの示すグループ２０文字を表示 (上画面表示)
				for( int y = 0; y<GROUP_HEIGHT; y++ ){
					for( int x = 0; x<m_GroupWidth; x++ ){
						//フォント切り替え
						static const NNSG2dFont* const fonts[FONTTYPE_MAX]={
							&m_Font_9x9,
						//	&m_Font_10x10,
							&m_Font_11x11,
						//	&m_Font_11x12,
						//	&m_Font_14x14,
							&m_Font_15x15,
						};
						
						//先頭文字からのオフセットを求め、描画する
						int Index = x + y*m_GroupWidth + ( GetGroupID() * m_GroupNum );
						
						//表示範囲外に達したら抜ける
						if( Index > m_MaxIndex ){
							break;
						}
						
						//描画パラメータ
						int xPos = GROUP_FONT_LEFT + x*GROUP_FONT_WIDTH;
						int yPos = GROUP_FONT_TOP  + y*GROUP_FONT_HEIGHT;
						u16 c    = m_FontCategoryTbl[m_CurCategory].GetValue(Index);
						
						//文字出力
						m_MainWnd.PrintChar( fonts[m_FontType], xPos, yPos, c );

						//文字幅表示
						if( m_GroupCursor == y ){
				            NNSG2dGlyph g;
				            NNS_G2dFontGetGlyph(&g, fonts[m_FontType], c );
							m_MainWnd.Fill( xPos, yPos + NNS_G2dFontGetHeight(fonts[m_FontType]), GetCharWidth(g.pWidths), 1 );
						}
					}
				}
				
				//フォントカテゴリの表示
				m_MainWnd.PrintString( &m_Font_9x9, FONTTITLE_X, FONTTITLE_Y, m_FontCategoryTbl[m_CurCategory].GetTitle() );
				
				//FontIDの表示
				m_SubWnd.PrintHex( &m_Font_15x15, FONT_ID_X, FONT_ID_Y, GetFontID() );

				//ターゲットウインドウの表示 (下画面の表示)
				for( int y = 0; y<TARGET_HEIGHT; y++ ){
					for( int x = 0; x<TARGET_WIDTH; x++ ){
                      static NNSG2dFont* fonts[TARGET_WIDTH];
                      
						//フォント切り替え
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

                        //文字色切り替え
						static int colors[TARGET_HEIGHT]={
							TXT_COLOR_WHITE,
							TXT_COLOR_BLACK,
						};
						
						//表示位置の決定
						int xPos = TARGET_FONT_LEFT + x*TARGET_FONT_WIDTH;
						int yPos = TARGET_FONT_TOP  + y*TARGET_FONT_HEIGHT - NNS_G2dFontGetHeight(fonts[x]);
						u16 c    = m_FontCategoryTbl[m_CurCategory].GetValue(m_CurIndex);
						
						//文字幅塗る
						m_SubWnd.Fill( xPos, yPos , 
							NNS_G2dFontGetCharWidth(fonts[x], c ), 
							NNS_G2dFontGetHeight(fonts[x]), colors[ (y+1) & 0x01] );
						
						//文字表示
						m_SubWnd.PrintChar( fonts[x], xPos, yPos, c, colors[y] );
						
						//横幅の表示
						if( m_fDispUnderLine ){
				            NNSG2dGlyph g;
				            NNS_G2dFontGetGlyph(&g, fonts[x], c );
							m_SubWnd.Fill( xPos, yPos + NNS_G2dFontGetHeight(fonts[x]), GetCharWidth(g.pWidths), 1, colors[y] );
						}
					}
				}

				//セレクトウインドウの表示
				for( int i=0; i<m_GroupWidth; i++ ){
					int Index = (m_CurIndex / m_GroupWidth * m_GroupWidth)  + i;
					//表示範囲外に達したら抜ける
					if( Index > m_MaxIndex ){
						break;
					}
					u16 c    = m_FontCategoryTbl[m_CurCategory].GetValue(Index);
					m_SubWnd.PrintChar( &m_Font, SELECT_LEFT + i*SELECT_WIDTH, SELECT_TOP, c );
				}
				
				//セレクトウインドウ カーソルの表示
				m_SubWnd.PrintChar( &m_Font, SELECT_LEFT + SELECT_WIDTH * (m_CurIndex % m_GroupWidth), CURSOR_TOP, CURSOR_CODE );
			}
				
			//VBLANK割り込みを待つ
			CMN_WaitVBlankIntr();
			
			//転送タスク登録
			m_MainWnd.Draw();
			m_SubWnd.Draw();
	       
			//転送実行
			NNS_GfdDoVramTransfer();
		}
	}

//****************************************************************************
// 画面状態ごとの管理構造体		選択状態
//****************************************************************************
	Status Status_Select::Run()
	{
		//初期化 
	 	TXT_DisableBackground();
		int fContinue = TRUE;
		m_CursorIndex = 0;
		
		//ループ処理
		for( u32 Frame=0; fContinue ; Frame++ )
		{
			//計算処理　
			int fModify = FALSE;
			{
				//ゲームパッド情報取得
				u16 Rept = m_pKeyReader->Update();
				u16 Trig = m_pKeyReader->Trig();
				
				//カーソル移動
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
				//決定　
				if( Trig & PAD_BUTTON_A ){
					fContinue = FALSE;
				}
			}
			
			//初期フレーム以降は変更があった場合のみ画面更新
			if( Frame==0 || fModify )
			{
				//テキスト領域のクリア
				m_MainWnd.Clear();
				m_SubWnd.Clear();
				
				//メニュー表示
				for( int i=0; i<MENU_NUM; i++ ){
					static const char * const MenuStr[MENU_NUM] = {
						"Font Check Mode",
						"Text Check Mode",
					};
					m_MainWnd.PrintString( &m_Font, MENU_LEFT, MENU_TOP + MENU_HEIGHT*i, MenuStr[i] );
				}
				
				//カーソルの表示
				m_MainWnd.PrintChar( &m_Font, CURSOR_LEFT, CURSOR_TOP+MENU_HEIGHT*m_CursorIndex, CURSOR_CODE );
			}
				
			//VBLANK割り込みを待つ
			CMN_WaitVBlankIntr();
			
			//転送タスク登録
			m_MainWnd.Draw();
			m_SubWnd.Draw();
	       
			//転送実行
			NNS_GfdDoVramTransfer();
		}
		
		//結果となる次の状態を返す
		if( m_CursorIndex == 0 ){
			return STATUS_FONT;
		}else{
			return STATUS_TEXT;
		}
	}

//****************************************************************************
// 画面状態ごとの管理構造体		フォント表示状態
//****************************************************************************
	
	void Status_Text::Run()
	{
		//初期化 
	 	TXT_DisableBackground();
		
		m_CurTextType = TYPE_JAPANESE;
		m_CurIndex    = 0;
		m_CurFontSize = SIZE_15x15;
		
		//ループ処理
		for( u32 Frame=0;  ; Frame++ )
		{
			//計算処理　
			int fModify = FALSE;
			{
				//ゲームパッド情報取得
				u16 Rept = m_pKeyReader->Update();
				u16 Trig = m_pKeyReader->Trig();
				u16 Cont = m_pKeyReader->Cont();
				
				//ソフトリセットチェック
				if( Cont == (PAD_BUTTON_R|PAD_BUTTON_L|PAD_BUTTON_A|PAD_BUTTON_B|PAD_BUTTON_X|PAD_BUTTON_Y) ){
					OS_ResetSystem( 0 ); 
				}
				
				//上下で表示テキストを選択する
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
				
				//左右で種別切り替え
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
				
				//フォントサイズ切り替え
				if( Trig & PAD_BUTTON_Y ){
					fModify = TRUE;
					m_CurFontSize ++;
					if( m_CurFontSize == SIZE_MAX ){
						m_CurFontSize = 0;
					}
				}
			}
			
			//初期フレーム以降は変更があった場合のみ画面更新
			if( Frame==0 || fModify )
			{
				//テキスト領域のクリア
				m_MainWnd.Clear();
				m_SubWnd.Clear();
				
				m_MainWnd.PrintString( &m_Font, TITLE_LEFT, TITLE_TOP, "TEXT CHECK MODE" );			
				//現在言語表示
				m_MainWnd.PrintString( &m_Font, MES_LEFT, MES_TOP, "Select Language" );			
				for( int i=0; i<TYPE_MAX; i++ ){
					m_MainWnd.PrintUnicodeString( &m_Font, LANG_LEFT+ i*LANG_WIDTH, LANG_TOP, g_TextTypes[i] );			
				}
				//現在言語カーソル表示
				m_MainWnd.PrintChar( &m_Font, LANG_CURSOR_LEFT+ m_CurTextType*LANG_CURSOR_WIDTH, LANG_CURSOR_TOP, LANG_CURSOR_CODE );			
				
				//フォントサイズ情報表示
				static const char * const SizeMes[] = {
					" 9x 9",
					"10x10",
					"11x11",
					"11x12",
					"14x14",
					"15x15",
				};
				m_SubWnd.PrintString( &m_Font, SIZE_LEFT, SIZE_TOP, SizeMes[m_CurFontSize] );
				
				//現在インデックス表示
				m_SubWnd.PrintDec( &m_Font, TEXT_NO_LEFT, TEXT_NO_TOP, m_CurIndex );			

				//対象テキスト表示
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
				
			//VBLANK割り込みを待つ
			CMN_WaitVBlankIntr();
			
			//転送タスク登録
			m_MainWnd.Draw();
			m_SubWnd.Draw();
	       
			//転送実行
			NNS_GfdDoVramTransfer();
		}
	}
