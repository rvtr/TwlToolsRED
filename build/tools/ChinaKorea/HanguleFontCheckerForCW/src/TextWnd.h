/*===============================================================*//**
 	@file		TextWnd.h
 	
 	@brief		nitro-systemを使ってのBGフォント表示機能を
 				上下画面で簡単に使えるようにしたもの
 				nnsys_demo_lib/txt.hのLCDレジスタ設定に依存
 				
 	@author		Kazumasa Hirata

*//*================================================================*/
#ifndef TEXTWND_H_ 

#define TEXTWND_H_

//****************************************************************************
// Include
//****************************************************************************
	//任天堂ライブラリ
//	#ifndef NNS_G2D_UNICODE					
	#define NNS_G2D_UNICODE			// 文字列描画関数が扱う文字をUnicodeにする
//	#endif
	#include <nitro.h>
	#include <nnsys/g2d/g2d_TextCanvas.h>
	#include <nnsys/g2d/g2d_CharCanvas.h>
	
	//任天堂デモ用ライブラリ（一部改変）
	#include "nnsys_demo_lib/cmn.h"
	#include "nnsys_demo_lib/txt.h"
	#include "nnsys_demo_lib/loader.h"
	
//****************************************************************************
// Declare
//****************************************************************************
	//基本となるテキスト表示クラス
	class BaseFontWnd {
	protected:
		NNSG2dCharCanvas		gCanvas;		// CharCanvas
		NNSG2dTextCanvas		gTextCanvas;	// TextCanvas
		static const int TEXT_HSPACE = 1;		// 文字列描画時の文字間 (ピクセル単位)
		static const int TEXT_VSPACE = 1;		// 文字列描画時の行間	(ピクセル単位)
		static const int TEXT_COLOR	 =	TXT_COLOR_WHITE;
		static const int BACK_COLOR	 =	TXT_COLOR_NULL;
	public:
		//インターフェイス
		
		/*	仮想関数テーブルがもったいないので消しておく
		BaseFontWnd();
		virtural ~BaseFontWnd() = 0;
		virtual void Init() = 0;
		virtual void Draw() = 0;
		*/
		
		//キャンバスのクリア
		void Clear(int color=BACK_COLOR);
		
		//しかくけい
		void Fill(int x, int y, int w, int h, int color=TEXT_COLOR );
		
		//文字出力
		void PrintGlyphIndex( const NNSG2dFont *pFont,  int x,int y, u16 index, int color=TEXT_COLOR );
		
		//文字出力
		void PrintChar( const NNSG2dFont *pFont,  int x,int y, u16 code, int color=TEXT_COLOR );
		
		//文字列出力
		void PrintUnicodeString(const NNSG2dFont *pFont, int x,int y, const NNSG2dChar* const str, int color=TEXT_COLOR );
		void PrintString(const NNSG2dFont *pFont, int x,int y, const char* const str, int color=TEXT_COLOR );
		
		//１０進数出力
		void PrintDec(const NNSG2dFont *pFont, int x,int y, s32 number, int color=TEXT_COLOR );
		
	
		//１６進数出力
		void PrintHex(const NNSG2dFont *pFont, int x,int y, u32 number, int color=TEXT_COLOR );
	};
	
	//２ＤグラフィックエンジンＡを使用するテキスト表示クラス
	class MainWnd : public BaseFontWnd {
		static const int CANVAS_LEFT	=	0;		// 文字描画域の位置X (キャラクタ単位)
		static const int CANVAS_TOP		=	0;		// 文字描画域の位置Y (キャラクタ単位)
		static const int CANVAS_WIDTH	=	32;		// 文字描画域の幅	 (キャラクタ単位)
		static const int CANVAS_HEIGHT	=	24;		// 文字描画域の高さ	 (キャラクタ単位)
		GXCharFmt16   gOffBuffer[CANVAS_HEIGHT][CANVAS_WIDTH];
	public:
		void Init( const NNSG2dFont *pFont );
		void Draw();
	};

	//２ＤグラフィックエンジンＢ	を使用するテキスト表示クラス
	class SubWnd : public BaseFontWnd {
		static const int CANVAS_LEFT	=	0;		// 文字描画域の位置X (キャラクタ単位)
		static const int CANVAS_TOP		=	0;		// 文字描画域の位置Y (キャラクタ単位)
		static const int CANVAS_WIDTH	=	32;		// 文字描画域の幅	 (キャラクタ単位)
		static const int CANVAS_HEIGHT	=	24;		// 文字描画域の高さ	 (キャラクタ単位)
		GXCharFmt16   gOffBuffer[CANVAS_HEIGHT][CANVAS_WIDTH];
	public:
		void Init( const NNSG2dFont *pFont );
		void Draw();
	};

#endif	// TEXTWND_H_ 
