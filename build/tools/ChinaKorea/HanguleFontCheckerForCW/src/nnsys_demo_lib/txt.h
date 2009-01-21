/*---------------------------------------------------------------------------*
  Project:  NITRO-System - build - demos - g2d - Text - textdemolib
  File:     txt.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: txt.h,v $
  Revision 1.3  2005/05/30 10:17:05  hatamoto_minoru
  ISO8859からCP1252へ。

  Revision 1.2  2005/05/23 06:17:12  hatamoto_minoru
  ISO-8859-1 エンコーディングのフォントを追加。

  Revision 1.1  2005/05/20 05:52:38  hatamoto_minoru
  NitroSystem への登録。

  Revision 1.9  2005/03/28 02:15:09  hatamoto_minoru
  TXT_LoadFile の追加。

  Revision 1.8  2005/03/18 09:19:46  hatamoto_minoru
  API変更に対応。

  Revision 1.7  2005/03/10 08:18:55  hatamoto_minoru
  API の変更に対応。
  コメントの追加。

  Revision 1.6  2005/02/08 09:52:51  hatamoto_minoru
  ユーザカラー名をプログラミングマニュアルのものに修正。

  Revision 1.5  2005/02/08 04:06:59  hatamoto_minoru
  ファイルヘッダの修正。

  Revision 1.4  2005/02/04 00:15:13  hatamoto_minoru
  フォントデータの差し替え。

  Revision 1.3  2005/02/01 11:53:15  hatamoto_minoru
  共通カラーパレット定義を追加。
  コメントを追加。

  Revision 1.2  2005/01/25 01:42:45  hatamoto_minoru
  TXT_SetCharCanvasOBJAttrs() 追加。

  Revision 1.1  2005/01/21 01:03:09  hatamoto_minoru
  TextDraw への登録。

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef TEXT_H_
#define TEXT_H_

#define NNS_G2D_UNICODE			// 文字列描画関数が扱う文字をUnicodeにする
#include <nnsys/g2d/g2d_Font.h>

#ifdef __cplusplus
extern "C" {
#endif

// DrawText での左上寄せ
#define TXT_DRAWTEXT_FLAG_DEFAULT   (NNS_G2D_VERTICALORIGIN_TOP | NNS_G2D_HORIZONTALORIGIN_LEFT | NNS_G2D_HORIZONTALALIGN_LEFT)

// デモ用フォントリソース名
#define TXT_FONTRESOURCE_NAME               "/data/fonts.NFTR"
#define TXT_SJIS_FONTRESOURCE_NAME          "/data/fonts.NFTR"
#define TXT_UTF8_FONTRESOURCE_NAME          "/data/fontu8.NFTR"
#define TXT_UTF16_FONTRESOURCE_NAME         "/data/fontu16.NFTR"
#define TXT_CP1252_FONTRESOURCE_NAME        "/data/font1252.NFTR"



// TXTColorPalette の色名 16色パレットへのロードを想定
enum
{
    // パレット0 TXT_CPALETTE_MAIN
    TXT_COLOR_NULL=0,
    
    TXT_COLOR_WHITE,
    TXT_COLOR_WHITE_1,
    TXT_COLOR_WHITE_2,
    TXT_COLOR_WHITE_3,

    TXT_COLOR_BLACK,
    TXT_COLOR_BLACK_1,
    TXT_COLOR_BLACK_2,
    TXT_COLOR_BLACK_3,

    // パレット1 TXT_CPALETTE_USERCOLOR
    TXT_UCOLOR_NULL=0,
    TXT_UCOLOR_GRAY,
    TXT_UCOLOR_BROWN,
    TXT_UCOLOR_RED,
    TXT_UCOLOR_PINK,
    TXT_UCOLOR_ORANGE,
    TXT_UCOLOR_YELLOW,
    TXT_UCOLOR_LIMEGREEN,
    TXT_UCOLOR_DARKGREEN,
    TXT_UCOLOR_SEAGREEN,
    TXT_UCOLOR_TURQUOISE,
    TXT_UCOLOR_BLUE,
    TXT_UCOLOR_DARKBLUE,
    TXT_UCOLOR_PURPLE,
    TXT_UCOLOR_VIOLET,
    TXT_UCOLOR_MAGENTA
};

// TXTColorPalette のパレット名 16色パレットへのロードを想定
enum
{
    TXT_CPALETTE_MAIN,
    TXT_CPALETTE_USERCOLOR
};

// 共通カラーパレットデータ
extern GXRgb TXTColorPalette[32];



//****************************************************************************
//
//****************************************************************************

/*---------------------------------------------------------------------------*
  Name:         TXT_Init

  Description:  サンプル共通の初期化。

  Arguments:    なし。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void TXT_Init( OSIrqFunction hBlankFunc, OSIrqFunction vBlankFunc );



/*---------------------------------------------------------------------------*
  Name:         TXT_SetupBackground

  Description:  メイン画面BG0に背景画像をロード＆表示します。

  Arguments:    なし。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void TXT_SetupBackground( void );



/*---------------------------------------------------------------------------*
  Name:         TXT_Alloc

  Description:  メモリを確保します。

  Arguments:    size:   確保するメモリのサイズ

  Returns:      確保したメモリ領域へのポインタ。
 *---------------------------------------------------------------------------*/
void* TXT_Alloc(u32 size);



/*---------------------------------------------------------------------------*
  Name:         TXT_Free

  Description:  TXT_Alloc() で確保したメモリを解放します。

  Arguments:    ptr:    開放するメモリ領域へのポインタ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void TXT_Free(void* ptr);



/*---------------------------------------------------------------------------*
  Name:         TXT_SetCharCanvasOBJAttrs

  Description:  連続するOAMのNNS_G2dArrangeOBJ* で設定されないパラメータを
                まとめて設定します。

  Arguments:    なし。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void TXT_SetCharCanvasOBJAttrs(
    GXOamAttr * oam,
    int num,
    int priority,
    GXOamMode mode,
    BOOL mosaic,
    GXOamEffect effect,
    int cParam,
    int rsParam
);



/*---------------------------------------------------------------------------*
  Name:         TXT_LoadFont

  Description:  フォントをファイルからメモリにロードします。

  Arguments:    pFname: フォントリソースのパス。

  Returns:      ロードしたフォントへのポインタ。
 *---------------------------------------------------------------------------*/
void TXT_LoadFont( NNSG2dFont* pFont, const char* pFname );



/*---------------------------------------------------------------------------*
  Name:         TXT_LoadFile

  Description:  ファイルをメモリにロードします。

  Arguments:    ppFile: ファイルをロードしたメモリアドレスを受け取る
                        バッファへのポインタ。
                fpath:  ロードするファイルのパス

  Returns:      ロードしたファイルのファイルサイズを返します。
                0 の場合はファイルロードに失敗した事を表します。
                この場合 *ppFile の値は無効です。
 *---------------------------------------------------------------------------*/
u32 TXT_LoadFile(void** ppFile, const char* fpath);

void TXT_EnableBackground( void );
void TXT_DisableBackground( void );

#ifdef __cplusplus
}/* extern "C" */
#endif

#endif // TEXT_H_

