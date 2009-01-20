/*---------------------------------------------------------------------------*
  Project:  NITRO-System - demos - g2d - Text - SimpleFontView
  File:     main.c

  Copyright 2004-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Revision$
 *---------------------------------------------------------------------------*/

// ============================================================================
//  デモの内容
//      フォントリソース内のグリフイメージを表示します。
//      data ディレクトリ内の fonts.NFTR を削除し、別のフォントを入れてから
//      リビルドすれば、入れ替えたフォントの内容を表示します。
//      data ディレクトリ内の fontd.NFTR は削除しないで下さい。
//
//  操作方法
//      ←→    1文字移動
//      ↑↓    1行分移動
//      B       押している間キーリピート有効
//      X       拡大率変更 4→1→2→3→4→1→…
// ============================================================================

#include "g2d_textdemolib.h"
#include <string.h>
#include <nnsys/gfd.h>

#define SCREEN_WIDTH        256                     // 画面幅
#define SCREEN_HEIGHT       192                     // 画面高さ

#define CHARACTER_WIDTH     8
#define CHARACTER_HEIGHT    8



#define LIMIT_GLYPH_INDEX(gi)   \
    while( gi > giMax )         \
    {                           \
        gi -= giMax + 1;        \
    }                           \
    while( gi < 0 )             \
    {                           \
        gi += giMax + 1;        \
    }

#define LIMIT_GLYPH_INDEX2(gi)  \
    while( gi > giMax )         \
    {                           \
        gi = giMax;             \
    }                           \
    while( gi < 0 )             \
    {                           \
        gi += giMax + 1;        \
    }

//------------------------------------------------------------------------------
// グローバル変数
NNSG2dCharCanvas    gCc;        // グリフリスト/グリフインデックス表示用 CharCanvas
NNSG2dTextCanvas    gTxn;       // グリフリスト表示用 TextCanvas
NNSG2dTextCanvas    gITxn;      // グリフインデックス表示用 TextCanvas

NNSG2dCharCanvas    gDCc;       // グリフイメージ拡大表示用 CharCanvas
NNSG2dTextCanvas    gDTxn;      // グリフイメージ拡大表示用 TextCanvas

NNSG2dFont          gFont;      // 表示対象フォント
NNSG2dFont          gInfoFont;  // 情報表示用フォント

int                 giCenter;   // グリフリスト中央のグリフインデックス
int                 giMax;      // gFont のもつグリフインデックス最大値
int                 giLine;     // グリフリストに表示するグリフ数
int                 giLast;     // 最後にカーソルキーで選択したグリフインデックス

static GXCharBGText256          gBG1OffScreen;      // グリフリスト/グリフインデックス表示用
static GXCharBGAffine256        gBG2OffScreen;      // グリフイメージ拡大表示用
static NNSGfdVramTransferTask   gTransferTask[2];   // オフスクリーン転送用

const static int BG1_CANVAS_WIDTH   = 32;
const static int BG1_CANVAS_HEIGHT  = 24;
const static int BG1_CANVAS_LEFT    =  0;
const static int BG1_CANVAS_TOP     =  0;
const static int BG1_CANVAS_OFFSET  =  1;

const static int BG2_CANVAS_WIDTH   = 16;
const static int BG2_CANVAS_HEIGHT  = 12;
const static int BG2_CANVAS_LEFT    =  0;
const static int BG2_CANVAS_TOP     =  0;
const static int BG2_CANVAS_OFFSET  =  1;

const static int CHAR_SPACE         =  5;
const static int MARGIN             =  5;

const static int REPEAT_THRESHOLD   = 22;

static int scale = 4;
static char fontName[FS_FILE_NAME_MAX + 1];



//****************************************************************************
// wrapper
//****************************************************************************
static inline GetCharWidth(const NNSG2dCharWidths* pWidths)
{
    return pWidths->charWidth;
}


//****************************************************************************
// Initialize etc.
//****************************************************************************

/*---------------------------------------------------------------------------*
  Name:         ClearAreaSafe

  Description:  引数を適切な範囲に収めてから NNS_G2dCharCanvasClearArea を
                呼び出します。

  Arguments:    pCC:    CharCanvas へのポインタ。
                cl:     塗りつぶすのに用いるカラー番号。
                x:      矩形左上の x 座標。
                y:      矩形左上の y 座標。
                w:      矩形幅。
                h:      矩形高さ。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static void ClearAreaSafe(
    const NNSG2dCharCanvas* pCC,
    int cl,
    int x,
    int y,
    int w,
    int h
)
{
    // 引数に指定可能な最大値
    const int x_max = pCC->areaWidth * CHARACTER_WIDTH;
    const int y_max = pCC->areaHeight * CHARACTER_HEIGHT;

    if( x < 0 )
    {
        x = 0;
    }
    if( y < 0 )
    {
        y = 0;
    }
    if( x_max <= x )
    {
        x = x_max - 1;
    }
    if( y_max <= y )
    {
        y = y_max - 1;
    }
    if( x_max < w )
    {
        w = x_max;
    }
    if( y_max < h )
    {
        h = y_max;
    }
    if( x_max < x + w )
    {
        w = x_max - x;
    }
    if( y_max < y + h )
    {
        h = y_max - y;
    }
    if( w < 0 || h < 0 )
    {
        return;
    }

    NNS_G2dCharCanvasClearArea(pCC, cl, x, y, w, h);
}

static int font_count = 0;
static void CountFont(void)
{
    BOOL bSuccess;
    FSFile dir;
    FSDirectoryEntryInfo entry;

    FS_InitFile(&dir);

    bSuccess = FS_OpenDirectory(&dir, "sdmc:", 0);
    SDK_ASSERT( bSuccess );
    // ディレクトリ内を探索
    while( FS_ReadDirectory(&dir, &entry) != FALSE )
    {
        if( entry.longname_length >= 5 )
        {
            // 拡張子が .NFTR であって
            if( strcmp( entry.longname + entry.longname_length - 5, ".NFTR" ) == 0 )
            {
                font_count++;
            }
        }
    }
    // フォントないよ
    if(font_count==0){
		SDK_ASSERTMSG(FALSE, "There are no fonts.");
	}
	
	FS_CloseDirectory(&dir);
}

/*---------------------------------------------------------------------------*
  Name:         LoadFont2

  Description:  フォントをロードします。
                呼ばれるたびに拡張子 .NFTR のファイルを探して見つかったものを
                順にロードします。

  Arguments:    なし。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static int sel_font = 0;
static void* pFontfile = NULL;
static void LoadFont2(void)
{
    BOOL bSuccess;
    FSFile dir;
    FSDirectoryEntryInfo entry;
    int count = 0;

    FS_InitFile(&dir);

    bSuccess = FS_OpenDirectory(&dir, "sdmc:", 0);
    SDK_ASSERT( bSuccess );
    
    // フォントないよ
    if(font_count==0){
		SDK_ASSERTMSG(FALSE, "There are no fonts.");
		FS_CloseDirectory(&dir);
		return;
	}

    // ディレクトリ内を探索
    while( FS_ReadDirectory(&dir, &entry) != FALSE )
    {
        if( entry.longname_length >= 5 )
        {
            // 拡張子が .NFTR であって
            if( strcmp( entry.longname + entry.longname_length - 5, ".NFTR" ) == 0 )
            {
                if(sel_font == count){
                    char fname[6 + FS_FILE_NAME_MAX + 1] = "sdmc:/";

                    (void)strcat(fname, entry.longname);
                    (void)strcpy(fontName, entry.longname);

                    // フォントをロード
                    if(pFontfile!=NULL) TXT_UnloadFont( pFontfile );
                    pFontfile = TXT_LoadFont( &gFont, fname );
                    break;
                }else
                {
					count++;
				}
            }
        }
    }

    SDK_ASSERTMSG(FALSE, "There are no fonts.");
	FS_CloseDirectory(&dir);
}

/*---------------------------------------------------------------------------*
  Name:         LoadFont

  Description:  フォントをロードします。
                拡張子 .NFTR のファイルを探して1番最初に見つかったものを
                ロードします。

  Arguments:    なし。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static void LoadFont(void)
{
    BOOL bSuccess;
    FSFile dir;
    FSDirEntry entry;

    FS_InitFile(&dir);

    bSuccess = FS_FindDir(&dir, "/data");
    SDK_ASSERT( bSuccess );

    // ディレクトリ内を探索
    while( FS_ReadDir(&dir, &entry) != FALSE )
    {
        if( entry.name_len >= 5 )
        {
            // 拡張子が .NFTR であって
            if( strcmp( entry.name + entry.name_len - 5, ".NFTR" ) == 0 )
            {
                // デバッグ表示用のフォントではない
                if( strcmp( entry.name, "fontd.NFTR" ) != 0 )
                {
                    char fname[6 + FS_FILE_NAME_MAX + 1] = "/data/";

                    (void)strcat(fname, entry.name);
                    (void)strcpy(fontName, entry.name);

                    // フォントをロード
                    TXT_LoadFont( &gFont, fname );
                    return;
                }
            }
        }
    }

    SDK_ASSERTMSG(FALSE, "There are no fonts.");
}



/*---------------------------------------------------------------------------*
  Name:         InitScreenCommon

  Description:  各BG面で共通の初期化を行います。

  Arguments:    なし。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static void InitScreenCommon(void)
{
	static void *pDeb_Fontfile = NULL;
    // バックドロップ色 + 情報表示文字色 + 枠色 + 最大フォント階調数 (32階調まで)
    const int max_colors = 32;
    static u16 colorPalette[1 + 1 + max_colors] =
    {
        GX_RGB(0, 0, 31), GX_RGB(9, 27, 17), GX_RGB(27, 2, 2)
    };

    // 情報表示フォントをロード
    if(pDeb_Fontfile!=NULL) TXT_UnloadFont( pDeb_Fontfile );
	pDeb_Fontfile = TXT_LoadFont( &gInfoFont, DEBUG_FONTRESOURCE_NAME );

    // 表示対象フォントをロード
    LoadFont2();

    // ロードしたフォントに応じてカラーパレットを作成
    {
        const int nColors = MATH_IMin((1 << NNS_G2dFontGetBpp(&gFont)), max_colors);
        int i;

        for( i = 0; i < nColors; ++i )
        {
            int level = ((nColors - 1 - i) * (max_colors - 1) / (nColors - 1));

            colorPalette[i+3] = GX_RGB(level, level, level);
        }
    }

    // カラーパレットをロード
    GX_LoadBGPltt(colorPalette, 0, sizeof(colorPalette));
}



/*---------------------------------------------------------------------------*
  Name:         InitEnumScreen

  Description:  文字リスト/グリフインデックス表示用 Canvas 初期化

  Arguments:    なし。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static void InitEnumScreen(void)
{
    const static int margin = 1;

    // BG 1 をテキストBGに設定
    G2_SetBG1Control(
        GX_BG_SCRSIZE_TEXT_256x256,
        GX_BG_COLORMODE_256,
        GX_BG_SCRBASE_0x0000,
        GX_BG_CHARBASE_0x00000,
        GX_BG_EXTPLTT_01
    );

    // BG1 を可視に
    CMN_SetPlaneVisible(GX_PLANEMASK_BG1);

    // CharCanvas初期化
    NNS_G2dCharCanvasInitForBG(
        &gCc,
        &gBG1OffScreen,
        BG1_CANVAS_WIDTH,
        BG1_CANVAS_HEIGHT,
        NNS_G2D_CHARA_COLORMODE_256
    );

    // スクリーン設定
    NNS_G2dMapScrToCharText(
        G2_GetBG1ScrPtr(),
        BG1_CANVAS_WIDTH,
        BG1_CANVAS_HEIGHT,
        BG1_CANVAS_LEFT,
        BG1_CANVAS_TOP,
        NNS_G2D_TEXT_BG_WIDTH_256,
        BG1_CANVAS_OFFSET,
        0
    );

    // CharCanvasクリア
    NNS_G2dCharCanvasClear(&gCc, 0);

    // TextCanvas初期化
    NNS_G2dTextCanvasInit(&gTxn, &gCc, &gFont, margin, margin);
    NNS_G2dTextCanvasInit(&gITxn, &gCc, &gInfoFont, margin, margin);

    // グリフインデックス最大値を取得
    giMax = (int)NNS_G2D_FONT_MAX_GLYPH_INDEX(&gFont) - 1;

    // グリフリスト表示数を計算
    {
        const int cw = NNS_G2dFontGetMaxCharWidth(&gFont);

        giLine = ((SCREEN_WIDTH/2) / (cw + CHAR_SPACE)) * 2 + 1;
    }
}



/*---------------------------------------------------------------------------*
  Name:         UpdateScreenScale

  Description:  グリフイメージ拡大率の変更適用。

  Arguments:    なし。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static void UpdateScreenScale(void)
{
    const int cw = NNS_G2dFontGetMaxCharWidth(&gFont);
    const int ch = NNS_G2dFontGetHeight(&gFont);
    const int ew = cw + MARGIN * 2;
    const int eh = ch + MARGIN * 2;
    const int info_height = MARGIN + ch + MARGIN;
    const int x = (SCREEN_WIDTH - ew * scale + 1) / 2;
    const int y = (SCREEN_HEIGHT - eh * scale + 1 - info_height) / 2;
    MtxFx22 mtx;

    // scale 倍に拡大するアフィン行列
    mtx._00 = FX32_ONE / scale;
    mtx._01 = 0;
    mtx._10 = 0;
    mtx._11 = FX32_ONE / scale;


    // BG2 にアフィン行列を設定
    G2_SetBG2Affine(&mtx, 0, 0, -x, -y);
}



/*---------------------------------------------------------------------------*
  Name:         InitDoubleScaleScreen

  Description:  グリフイメージ拡大表示用 Canvas 初期化

  Arguments:    なし。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static void InitDoubleScaleScreen(void)
{
    const static int margin = 1;

    // BG 2 をアフィンBGに設定
    G2_SetBG2ControlAffine(
        GX_BG_SCRSIZE_AFFINE_256x256,
        GX_BG_AREAOVER_XLU,
        GX_BG_SCRBASE_0x0800,
        GX_BG_CHARBASE_0x10000
    );

    UpdateScreenScale();

    // BG2 を可視に
    CMN_SetPlaneVisible(GX_PLANEMASK_BG2);


    // CharCanvas初期化
    NNS_G2dCharCanvasInitForBG(
        &gDCc,
        &gBG2OffScreen,
        BG2_CANVAS_WIDTH,
        BG2_CANVAS_HEIGHT,
        NNS_G2D_CHARA_COLORMODE_256
    );

    // スクリーン設定
    NNS_G2dMapScrToCharAffine(
        &( ((GXScrAffine32x32*)G2_GetBG2ScrPtr())->scr[BG2_CANVAS_TOP][BG2_CANVAS_LEFT] ),
        BG2_CANVAS_WIDTH,
        BG2_CANVAS_HEIGHT,
        NNS_G2D_AFFINE_BG_WIDTH_256,
        BG2_CANVAS_OFFSET
    );

    // CharCanvasクリア
    NNS_G2dCharCanvasClear(&gDCc, 0);

    // TextCanvas初期化
    NNS_G2dTextCanvasInit(&gDTxn, &gDCc, &gFont, margin, margin);
}



/*---------------------------------------------------------------------------*
  Name:         UpdateDisplay

  Description:  描画処理を行います。

  Arguments:    なし。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static void UpdateDisplay(void)
{
    const u8 barColor       = 1;    // 緑青 GX_RGB(9, 27, 17)
    const u8 borderColor    = 2;    // 橙色 GX_RGB(27, 17, 9)
    const u8 bgColor        = 3;    // 白（自動生成の先頭）
    const u8 txtColor       = 4;    // 自動生成の2番目以降

    const int ch = NNS_G2dFontGetHeight(&gFont);
    const int cw = NNS_G2dFontGetMaxCharWidth(&gFont);
    const int dw = cw + CHAR_SPACE;

    const int l_px = (SCREEN_WIDTH - (giLine * dw - CHAR_SPACE)) / 2;
    const int l_py = SCREEN_HEIGHT - (ch + MARGIN);


    {
        // 拡大表示
        {
            NNSG2dGlyph g;

            NNS_G2dFontGetGlyphFromIndex(&g, &gFont, (u16)(giCenter));

            {
                // グリフが中央にくるように調整
            	const int cWidth = GetCharWidth(g.pWidths);
                const int e_pxc = MARGIN + g.pWidths->left + (cw - cWidth) / 2;
                const int bar_x = e_pxc - g.pWidths->left;
                const int bar_y = MARGIN + ch + 1;

                ClearAreaSafe(&gDCc, bgColor, 0, 0, (cw + MARGIN * 2), (ch + MARGIN * 2) );
                NNS_G2dCharCanvasDrawGlyph(&gDCc, &gFont, e_pxc, MARGIN, txtColor, &g);

                // NNS_G2dCharCanvasClearArea を使って幅線を描く
                ClearAreaSafe(&gDCc, barColor, bar_x, bar_y, cWidth, 1);
            }
        }

        // グリフリスト表示
        {
            int i;

            ClearAreaSafe(&gCc, bgColor, 0, (u16)(l_py - MARGIN), SCREEN_WIDTH, (u16)(ch + MARGIN * 2));

            // giCenter を中心として giLine 個のグリフを表示
            for( i = 0; i < giLine; ++i )
            {
                NNSG2dGlyph g;
                int gi = giCenter + i - giLine / 2;

                LIMIT_GLYPH_INDEX(gi);

                NNS_G2dFontGetGlyphFromIndex(&g, &gFont, (u16)(gi));

                {
                    // 補正x = 行オフセット + 文字位置 + 左スペース + フォント内文字相対オフセット
                    const int pxc = l_px + dw * i + g.pWidths->left + (cw - GetCharWidth(g.pWidths)) / 2;

                    NNS_G2dCharCanvasDrawGlyph(&gCc, &gFont, pxc, l_py, txtColor, &g);
                }
            }

            // NNS_G2dCharCanvasClearArea を使って枠を描く
            {
                int border_x = l_px + dw * (giLine / 2) - 2;
                int border_y = l_py - 2;
                int border_w = cw + 2;
                int border_h = ch + 2;

                ClearAreaSafe(&gCc, borderColor, border_x,                  border_y,                   border_w + 2,   1);
                ClearAreaSafe(&gCc, borderColor, border_x,                  border_y + border_h + 1,    border_w + 2,   1);

                ClearAreaSafe(&gCc, borderColor, border_x,                  border_y + 1,               1,              border_h);
                ClearAreaSafe(&gCc, borderColor, border_x + border_w + 1,   border_y + 1,               1,              border_h);
            }
        }
    }

    // VRAM転送タスクを登録
    {
        (void)NNS_GfdRegisterNewVramTransferTask(
            NNS_GFD_DST_2D_BG1_CHAR_MAIN,
            sizeof(GXCharFmt256) * BG1_CANVAS_OFFSET,
            &gBG1OffScreen,
            sizeof(GXCharFmt256) * BG1_CANVAS_WIDTH * BG1_CANVAS_HEIGHT );

        (void)NNS_GfdRegisterNewVramTransferTask(
            NNS_GFD_DST_2D_BG2_CHAR_MAIN,
            sizeof(GXCharFmt256) * BG2_CANVAS_OFFSET,
            &gBG2OffScreen,
            sizeof(GXCharFmt256) * BG2_CANVAS_WIDTH * BG2_CANVAS_HEIGHT );
    }
}



/*---------------------------------------------------------------------------*
  Name:         PrintSampleInfo

  Description:  サンプルの情報を下画面に表示します。

  Arguments:    なし。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static void PrintSampleInfo(void)
{
    const static char* encodingName[] =
    {
        "UTF-8",
        "UTF-16",
        "Shift_JIS",
        "CP1252",
    };
    NNSG2dGlyph g;
    int right;

    NNS_G2dFontGetGlyphFromIndex(&g, &gFont, (u16)(giCenter));
	right = GetCharWidth(g.pWidths) - (g.pWidths->left + g.pWidths->glyphWidth);

    DTX_PrintLine(
        "Simple Font Viewer Sample\n"
        "operation\n"
        "  left, right  move one character\n"
        "  up, down     move one line\n"
        "  X            change scale\n"
        "  L, R         change font\n"
        "  start        set zero to index"
    );

    DTX_PrintLine("font file name:  %s", fontName);
    DTX_PrintLine("font encoding:   %s", encodingName[NNSi_G2dFontGetEncoding(&gFont)]);
    DTX_PrintLine("font cell size:  %5d x %5d", NNS_G2dFontGetCellWidth(&gFont), NNS_G2dFontGetCellHeight(&gFont));
    DTX_PrintLine("font bpp:        %5d", NNS_G2dFontGetBpp(&gFont));
    DTX_PrintLine("font baseline:   %5d", NNS_G2dFontGetBaselinePos(&gFont));
    DTX_PrintLine("font linefeed:   %5d", NNS_G2dFontGetLineFeed(&gFont));
    DTX_PrintLine("font alter char: %5d", NNS_G2dFontGetAlternateGlyphIndex(&gFont));
    DTX_PrintLine("glyph index/sum: %5d / %5d", giCenter, giMax);
    DTX_PrintLine("glyph widths:     %4d + %3d + %4d = %4d",
        g.pWidths->left, g.pWidths->glyphWidth, right, g.pWidths->charWidth);
    DTX_PrintLine("scale:              x%d", scale);
}



/*---------------------------------------------------------------------------*
  Name:         SampleInit

  Description:  表示用の初期化を行います。

  Arguments:    なし。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static void SampleInit(void)
{
    InitScreenCommon();
    InitEnumScreen();
    InitDoubleScaleScreen();

    // オフスクリーン転送用
    NNS_GfdInitVramTransferManager(gTransferTask, ARY_SIZEOF(gTransferTask));

}



/*---------------------------------------------------------------------------*
  Name:         SampleMain

  Description:  毎フレームの描画処理を行います。

  Arguments:    なし。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
static int iCenter = -1;
static void SampleMain(void)
{

    // 更新されている時だけ描画する
    if( giCenter != iCenter )
    {
        iCenter = giCenter;
        PrintSampleInfo();
        UpdateDisplay();
    }
}

//****************************************************************************
// Main
//****************************************************************************


/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  メイン関数です。

  Arguments:    なし。

  Returns:      なし。
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    // Initilizing App.
    {
        // SDK とデモライブラリを初期化
        OS_Init();
        TXT_Init();

        // 表示初期化
        CountFont();
        SampleInit();
	    // 初期表示位置
	    giCenter = 0;
    }

    // start display
    {
        CMN_WaitVBlankIntr();
        GX_DispOn();
        GXS_DispOn();
    }


    // Main loop
    while( TRUE )
    {
        static u32 repeat_count = 0;

        CMN_ReadGamePad();

        // 拡大率の変更
        if( CMN_IsTrigger(PAD_BUTTON_X) )
        {
            scale ++;
            if( scale > 4 )
            {
                scale = 1;
            }
            UpdateScreenScale();
            PrintSampleInfo();
        }
        
        if( CMN_IsTrigger(PAD_BUTTON_R) )
        {
			sel_font++;
			if( font_count <= sel_font ) sel_font = 0;
			SampleInit();
			giCenter = giLast;
			LIMIT_GLYPH_INDEX2(giCenter);
		    iCenter = -1;
		}
        else if( CMN_IsTrigger(PAD_BUTTON_L) )
        {
			sel_font--;
			if( sel_font < 0 ) sel_font = font_count - 1;
			SampleInit();
			giCenter = giLast;
			LIMIT_GLYPH_INDEX2(giCenter);
		    iCenter = -1;
		}
        if( CMN_IsTrigger(PAD_BUTTON_START) )
        {
			giCenter = 0;
			giLast = 0;
		}

        // 表示グリフの変更
        if( CMN_IsPress(PAD_PLUS_KEY_MASK) )
        {
#define REPEAT(key)     ( (repeat_count >= REPEAT_THRESHOLD) ? CMN_IsPress(key): CMN_IsTrigger(key) )
            if( REPEAT(PAD_KEY_LEFT) )
            {
                giCenter--;
            }
            if( REPEAT(PAD_KEY_RIGHT) )
            {
                giCenter++;
            }
            if( REPEAT(PAD_KEY_UP) )
            {
                giCenter -= giLine;
            }
            if( REPEAT(PAD_KEY_DOWN) )
            {
                giCenter += giLine;
            }

            LIMIT_GLYPH_INDEX(giCenter);
            giLast = giCenter;

            repeat_count++;
        }
        else
        {
            repeat_count = 0;
        }

        // 描画処理
        SampleMain();

        CMN_WaitVBlankIntr();

        // 情報出力の表示
        DTX_Reflect();

        // オフスクリーンバッファを転送
        NNS_GfdDoVramTransfer();
    }
}
