/*===============================================================*//**
 	@file		TextWnd.cpp
 	
 	@brief		nitro-systemを使ってのBGフォント表示機能を
 				上下画面で簡単に使えるようにしたもの
 				nnsys_demo_lib/txt.hのLCDレジスタ設定に依存
 				
 	@author		Kazumasa Hirata

*//*================================================================*/

//****************************************************************************
// Include
//****************************************************************************
	#include "TextWnd.h"
	
//****************************************************************************
// Define
//****************************************************************************
	// 使用するキャラクタ列の開始番号
	#define CHARACTER_OFFSET	1				
	
//****************************************************************************
// コード
//****************************************************************************
	//キャンバスのクリア
	void BaseFontWnd::Clear(int color)
	{
		NNS_G2dCharCanvasClear(&gCanvas, color);
	}
	
	//しかくけい
	void BaseFontWnd::Fill( int x, int y, int w, int h, int color )
	{
		if( w > 0 && h > 0 ){
			NNS_G2dCharCanvasClearArea(&gCanvas, color, x, y, w, h );
		}
	}
	
	//文字出力
	void BaseFontWnd::PrintGlyphIndex(const NNSG2dFont *pFont, int x,int y, u16 index, int color )
	{
		NNSG2dGlyph g;
		NNS_G2dFontGetGlyphFromIndex( &g, pFont, index );
		NNS_G2dCharCanvasDrawGlyph( &gCanvas, pFont, x, y, color, &g );
	}
	
	//文字出力
	void BaseFontWnd::PrintChar(const NNSG2dFont *pFont, int x,int y, u16 code, int color )
	{
		NNS_G2dCharCanvasDrawChar( &gCanvas, pFont, x, y, color, code );
	}
	
	//文字列出力
	void BaseFontWnd::PrintUnicodeString(const NNSG2dFont *pFont, int x,int y, const NNSG2dChar* const str, int color )
	{
		//フォントを元に戻すために現在設定されているフォントを取得する
		const NNSG2dFont* pBak = NNS_G2dTextCanvasGetFont(	&gTextCanvas );
		
		//フォント変更
		NNS_G2dTextCanvasSetFont( &gTextCanvas, pFont );
		
		//文字列出力
		NNS_G2dTextCanvasDrawText(
			&gTextCanvas, 
			x, y, 
			color, 
			TXT_DRAWTEXT_FLAG_DEFAULT, 
			str );
		
		//フォントを元に戻す
		NNS_G2dTextCanvasSetFont( &gTextCanvas, pBak );
	}
	
	void BaseFontWnd::PrintString(const NNSG2dFont *pFont, int x,int y, const char* const str, int color )
	{
		NNSG2dChar * p;
		
		//文字列の長さを求める
		int len = STD_GetStringLength( str );
	
		//メモリ確保（ヌル文字用に１余分）
		p = reinterpret_cast<NNSG2dChar *>(TXT_Alloc( sizeof(NNSG2dChar)*len + 1 ));
		
		if( p ){
			//確保したメモリにコピー
			for( int i=0; i<len; i++ ){
				p[i] = str[i];
			}
			//終端にヌルを入れる
			p[len] = 0x0000;
			
			//文字列を出力
			PrintUnicodeString( pFont, x, y, p, color );
			
			//メモリ開放
			TXT_Free( p );
		}
	}
	
	//１０進数出力
	void BaseFontWnd::PrintDec(const NNSG2dFont *pFont,int x,int y, s32 number, int color )
	{
		//Asciiで文字列生成
		static char 		buf0[32];
		OS_SPrintf( buf0, "%d", number );
		//Unicodeに変換
		static NNSG2dChar 	buf[32];
		int i;
		for( i=0; buf0[i] != 0; i++ ){
			buf[i] = buf0[i];
		}
		buf[i] = 0;
		//出力
		PrintUnicodeString( pFont,x, y, buf, color );
	}
	
	//１６進数出力
	void BaseFontWnd::PrintHex(const NNSG2dFont *pFont,int x, int y, u32 number, int color )
	{
		static char 		buf0[32];
		OS_SPrintf( buf0, "%04x", number );	//とりあえず今回４桁しか要らない
		
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
		// Main BG 0 を設定
		G2_SetBG0Control(
			GX_BG_SCRSIZE_TEXT_256x256,		// スクリーンサイズ 256x256
			GX_BG_COLORMODE_16,				// カラーモード		16色
			GX_BG_SCRBASE_0x0000,			// スクリーンベース
			GX_BG_CHARBASE_0x00000,			// キャラクタベース
			GX_BG_EXTPLTT_01				// 拡張パレットスロット
		);
		
		// Main BG0 を可視に
		CMN_SetPlaneVisible( GX_PLANEMASK_BG0 );
		
		// CharCanvas の初期化
		NNS_G2dCharCanvasInitForBG(
			&gCanvas,
			gOffBuffer,
			CANVAS_WIDTH,
			CANVAS_HEIGHT,
			NNS_G2D_CHARA_COLORMODE_16
		);

		// TextCanvasの初期化
		NNS_G2dTextCanvasInit(
			&gTextCanvas,
			&gCanvas,
			pFont,
			TEXT_HSPACE,
			TEXT_VSPACE
		);

		// スクリーンを設定
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
        // Vブランク期間中の転送タスクを登録します。
        (void)NNS_GfdRegisterNewVramTransferTask(
            NNS_GFD_DST_2D_BG0_CHAR_MAIN,
            sizeof(GXCharFmt16) * CHARACTER_OFFSET,
            gOffBuffer,
            sizeof(gOffBuffer)
        );
	}

	void SubWnd::Init( const NNSG2dFont *pFont )
	{
		//SUB BG0 セットアップ
	    G2S_SetBG0Control(
			GX_BG_SCRSIZE_TEXT_256x256,		// スクリーンサイズ 256x256
			GX_BG_COLORMODE_16,				// カラーモード		16色
			GX_BG_SCRBASE_0x0000,			// スクリーンベース
			GX_BG_CHARBASE_0x04000,			// キャラクタベース
			GX_BG_EXTPLTT_01				// 拡張パレットスロット
	    );
		
		//可視に
		CMN_SetPlaneVisibleSub( GX_PLANEMASK_BG0 );
		
		// CharCanvasの初期化
		NNS_G2dCharCanvasInitForBG(
			&gCanvas,
			gOffBuffer,
			CANVAS_WIDTH,
			CANVAS_HEIGHT,
			NNS_G2D_CHARA_COLORMODE_16
		);
	   
		// TextCanvasの初期化
		NNS_G2dTextCanvasInit(
			&gTextCanvas,
			&gCanvas,
			pFont,
			TEXT_HSPACE,
			TEXT_VSPACE
		);

		// スクリーンを設定
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
        // Vブランク期間中の転送タスクを登録します。
        (void)NNS_GfdRegisterNewVramTransferTask(
            NNS_GFD_DST_2D_BG0_CHAR_SUB,
            sizeof(GXCharFmt16) * CHARACTER_OFFSET,
            gOffBuffer,
            sizeof(gOffBuffer)
        );
	}

