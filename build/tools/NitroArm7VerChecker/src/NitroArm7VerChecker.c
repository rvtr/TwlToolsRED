/*---------------------------------------------------------------------------*
  Project:  NitroArm7VerChecker
  File:     CooperationA.c

  Copyright 2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date::            $
  $Rev$
  $Author$
 *---------------------------------------------------------------------------*/

#include <twl.h>
#include <nitro/os/ARM9/argument.h>
#include <twl/os/common/format_rom.h>
#include <twl/os/common/banner.h>
#include "misc_simple.h"
#include "NitroArm7VerChecker.h"
#include "revision.h"
#include    <../build/libraries/mb/common/include/mb_fileinfo.h>

// define data------------------------------------------

//#define MENU_ELEMENT_NUM			5						// メニューの項目数

//#define DEBUG_MODE

#define ARM7_INFO_NUM			7

#ifdef DEBUG_MODE
#define KEY_REPEAT_START        10  // キーリピート開始までのフレーム数
#define KEY_REPEAT_SPAN         1   // キーリピートの間隔フレーム数
#define DUMP_SIZE               0x80
#endif

typedef struct Arm7Info
{
    char name[32];
    unsigned char hash[MATH_SHA1_DIGEST_SIZE];
}
Arm7Info;

typedef enum DetectError
{
	DETECT_ERROR_NONE = 0,
	DETECT_ERROR_ARM7FLX_SIZE_OVER = 1,
	DETECT_ERROR_ARM7FLX_SIZE_ZERO = 2,
	DETECT_ERROR_NO_HIT = 3,
	DETECT_ERROR_MAX
}
DetectError;

#ifdef DEBUG_MODE
typedef struct KeyInfo
{
    u16 cnt;    // 未加工入力値
    u16 trg;    // 押しトリガ入力
    u16 up;     // 離しトリガ入力
    u16 rep;    // 押し維持リピート入力
} KeyInfo;
#endif

// extern data------------------------------------------

// function's prototype declaration---------------------

static void MenuScene( void );
#ifdef DEBUG_MODE
void ReadKey(KeyInfo* pKey);
#endif

// global variable -------------------------------------

// static variable -------------------------------------

static char s_mode = 0;
static BOOL s_secret = FALSE;
static u8 s_error = 0;
static u8 s_digest[MATH_SHA1_DIGEST_SIZE];
static u8 s_hit = 0;
#ifdef DEBUG_MODE
static KeyInfo  gKey;
static u32 gIndex;
static void *data_buf;
#endif

// const data  -----------------------------------------

static const ROM_Header_Short *sp_header = (ROM_Header_Short *)HW_CARD_ROM_HEADER;
static const Arm7Info s_info[ ARM7_INFO_NUM ] =
{
    {
	"NitroSDK 2.2 RELEASE plus 3",
	{ 0xB4, 0x6C, 0xD3, 0x35, 0x5D, 0xB1, 0x6E, 0xC9, 0xEC, 0x5F,
	  0xC4, 0x82, 0x23, 0x23, 0xA1, 0x90, 0xD9, 0x8D, 0xBA, 0xC4 }
    },
    {
	"NitroSDK 4.0 RELEASE plus 1",
	{ 0x77, 0xA5, 0xC0, 0x89, 0x83, 0x66, 0xC1, 0x0D, 0x0A, 0x3B,
	  0x31, 0xA0, 0x63, 0xE6, 0xF5, 0x4F, 0xED, 0xC4, 0xC7, 0xAE }
    },
    {
	"NitroSDK 2.0 RC4 plus 1",
	{ 0x83, 0x1E, 0x93, 0x52, 0x58, 0x9A, 0xF5, 0x11, 0x62, 0x06,
	  0x63, 0x7F, 0x79, 0x57, 0xDD, 0xB2, 0x24, 0x3B, 0x95, 0x33 }
    },
    {
	"NitroSDK 4.2 RELEASE plus 1",
    { 0x82, 0x7d, 0xa0, 0x82, 0xd0, 0x56, 0xb3, 0x5a, 0x57, 0x19,
      0xca, 0xea, 0x1b, 0xd4, 0xa2, 0xda, 0x3e, 0xdc, 0xab, 0xbc }
    },
    {
	"NitroSDK 3.1 RELEASE",
    { 0x1e, 0x62, 0x0f, 0x41, 0xe9, 0x83, 0x96, 0xbf, 0x21, 0x4c,
      0x45, 0x40, 0x87, 0x32, 0x94, 0x12, 0x72, 0x1b, 0xf2, 0xcf,}
    },
    {
	"NitroSDK 3.1 RELEASE",
    { 0x46, 0x10, 0x64, 0xdf, 0xad, 0xcc, 0xfe, 0x3a, 0x39, 0xa1,
      0x49, 0xb8, 0x71, 0x9a, 0x11, 0x94, 0x9a, 0x7d, 0x9d, 0x86,}
    },
    {
	"NitroSDK 2.2 RELEASE plus 2",
    { 0xb8, 0xa6, 0xd1, 0x67, 0xc3, 0x67, 0xa6, 0xf7, 0x70, 0xa3,
      0xc6, 0x4a, 0xc0, 0x6d, 0x8f, 0x2e, 0xc9, 0x1b, 0xc9, 0xf3,}
    },
};


/*
static const char *s_pStrMenu[ MENU_ELEMENT_NUM ] = 
{
	"AAA",
	"BBB",
	"CCC",
	"DDD",
	"EEE",
};

static MenuPos s_menuPos[] = {
	{ TRUE,  3,   6 },
	{ TRUE,  3,   8 },
	{ TRUE,  3,  10 },
	{ TRUE, 3,  12 },
	{ TRUE, 3,  14 },
};

static const MenuParam s_menuParam = {
	MENU_ELEMENT_NUM,
	TXT_COLOR_BLACK,
	TXT_COLOR_GREEN,
	TXT_COLOR_RED,
	&s_menuPos[ 0 ],
	(const char **)&s_pStrMenu,
};
*/

// 活線挿抜からのパクり

#define SLOT_STATUS_MODE_10                                0x08
#define REG_MC1_OFFSET                                     0x4010
#define REG_MC1_ADDR                                       (HW_REG_BASE + REG_MC1_OFFSET)
#define reg_MI_MC1                                         (*( REGType32v *) REG_MC1_ADDR)

/*---------------------------------------------------------------------------*
  Name:         HOTSW_IsCardExist

  Description:  SCFG_MC1のCDETフラグを見て、カードの存在判定を行う
 *---------------------------------------------------------------------------*/
static BOOL HOTSW_IsCardExist(void)
{
    if( !(reg_MI_MC1 & REG_MI_MC_SL1_CDET_MASK) ){
        return TRUE;
    }
    else{
        return FALSE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         GetMcSlotMode

  Description:  スロットの現在のモードを返す
 *---------------------------------------------------------------------------*/
static u32 GetMcSlotMode(void)
{
    return reg_MI_MC1 & REG_MI_MC_SL1_MODE_MASK;
}

/*---------------------------------------------------------------------------*
  Name:         HOTSW_IsCardAccessible

  Description:  SCFG_MC1のCDETフラグとM(モード)を見て、カードスロットにアクセスできる状態か判定する
 *---------------------------------------------------------------------------*/
static BOOL HOTSW_IsCardAccessible(void)
{
    if( HOTSW_IsCardExist() && (GetMcSlotMode() == SLOT_STATUS_MODE_10)){
        return TRUE;
    }
    else{
        return FALSE;
    }
}



//======================================================
// ARM7コンポーネントに使われているSDKバージョンのチェックプログラム
//======================================================

// 描画まとめ
static void DrawMainScene( void )
{
#ifdef DEBUG_MODE
    int i,j,idx;
    u8 *buf = (u8 *)data_buf + gIndex*DUMP_SIZE;
    
    myDp_Printf( 2, 1, TXT_COLOR_BLUE, SUB_SCREEN, "sub_rom_offset : 0x%08x", sp_header->sub_rom_offset);
    myDp_Printf( 2, 2, TXT_COLOR_BLUE, SUB_SCREEN, "banner_offset  : 0x%08x", sp_header->banner_offset);
    myDp_Printf( 2, 3, TXT_COLOR_BLUE, SUB_SCREEN, "dump adr index : 0x%08x", DUMP_SIZE * gIndex );
    
    idx = 0;
    for ( i=0; i<0x10; i++ )
    {
        for ( j=0; j<8; j++ )
        {
            myDp_Printf(       2, 7 + i*1, TXT_COLOR_BLUE, SUB_SCREEN, "%02x #", i*0x8);
            myDp_Printf( 7 + j*3, 7 + i*1, TXT_COLOR_BLUE, SUB_SCREEN, "%02x", buf[idx]);
            idx++;
        }
    }
#endif
    
	//int l;
	myDp_Printf( 0, 0, TXT_COLOR_BLUE, MAIN_SCREEN, "Component SDK Version Identifier");
    
	if( s_mode == 0 )
	{
		// 結果表示モード
		if( s_error != DETECT_ERROR_NONE )
		{
			// エラー表示
			myDp_Printf( 1, 3, TXT_COLOR_RED, MAIN_SCREEN, "Registered ARM7 component");
			myDp_Printf( 1, 4, TXT_COLOR_RED, MAIN_SCREEN, "was not detected.");
			myDp_Printf( 1, 6, TXT_COLOR_RED, MAIN_SCREEN, "Error : %d", s_error);
		}else
		{
			// 通常結果表示
			myDp_Printf( 1, 3, TXT_COLOR_BLACK, MAIN_SCREEN, "Registered ARM7 component");
			myDp_Printf( 1, 4, TXT_COLOR_BLACK, MAIN_SCREEN, "was detected.");
			myDp_Printf( 1, 6, TXT_COLOR_BLACK, MAIN_SCREEN, "Same as");
			myDp_Printf( 1, 7, TXT_COLOR_BLUE, MAIN_SCREEN, "%s", s_info[s_hit].name);
			myDp_Printf( 1, 9, TXT_COLOR_BLUE, MAIN_SCREEN, "GameCode        : %c%c%c%c", sp_header->game_code[0],sp_header->game_code[1],sp_header->game_code[2],sp_header->game_code[3]);
			/*
			myDp_Printf( 1, 10, TXT_COLOR_BLUE, MAIN_SCREEN, "Arm7FLXDigest   :");
			for(l=0;l<MATH_SHA1_DIGEST_SIZE;l++)
			{
				myDp_Printf( 2+3*(l%10), 11+l/10, TXT_COLOR_BLUE, MAIN_SCREEN, "%02x ", s_digest[l]);
			}
			*/
		}
	}else
	{
		// カード無しモード
		myDp_Printf( 1, 3, TXT_COLOR_BLACK, MAIN_SCREEN, "Please insert DS game card.");
	}
	
	// 隠し ID 表示
	if ( s_secret )
	{
		myDp_Printf( 1, 20, TXT_COLOR_BLUE, MAIN_SCREEN, "ID : %d", ID_NUM);
	}
}

static void CheckCard( void )
{
	void *sp_arm7flx;
	void *old_lo;
	int m;

    CARD_Enable( TRUE );

	// 読み込みバッファはアリーナLoからHiまでの間を適当に使ってアリーナLoを移動させよう
	old_lo = OS_GetMainArenaLo();
	sp_arm7flx = (void *)( MATH_ROUNDUP32( (int)OS_GetMainArenaLo() ) );
	OS_SetMainArenaLo( OS_GetMainArenaHi() ); // 念のため一旦端っこに寄せておく

	CARD_LockRom( 0x03 ); // ID は適当
	
	// ヘッダのチェック（やるなら）
	// ARM7のサイズチェック
	if( sp_header->sub_size > BOOTABLE_SIZE_ARM7 )
	{
		// エラー
		s_error = DETECT_ERROR_ARM7FLX_SIZE_OVER;
		return;
	}else if ( sp_header->sub_size == 0 )
	{
		s_error = DETECT_ERROR_ARM7FLX_SIZE_ZERO;
		return;
	}

	OS_SetMainArenaLo( (void *)((u32)sp_arm7flx + sp_header->sub_size) ); // アリーナLo修正

#ifdef DEBUG_MODE
    data_buf = sp_arm7flx;
#endif

    // ARM9のSecure除いた部分を空読み出し
    if( (sp_header->main_rom_offset + sp_header->main_size) > 0x8000 )
    {
#ifdef DEBUG_MODE
        u32 auth_offset   = sp_header->rom_valid_size ? sp_header->rom_valid_size : 0x01000000;
        u32 page_offset   = auth_offset & 0xFFFFFE00;
        
        // バナーデータ分  ※ DEBUG_MODE有効時、ここの部分が実行時エラーになるかもしれないので、コメントアウトしておく。
//        CARD_ReadRom( MI_DMA_NOT_USE, (void *)sp_header->banner_offset, sp_arm7flx, sizeof(TWLBannerFile) );

        // 認証コード分  ※ DEBUG_MODE有効時、ここの部分が実行時エラーになるかもしれないので、コメントアウトしておく。
//        CARD_ReadRom( MI_DMA_NOT_USE, (void *)page_offset, sp_arm7flx, MB_AUTHCODE_SIZE );
#endif

        // Game領域のARM9常駐モジュール分
        CARD_ReadRom( MI_DMA_NOT_USE, (void *)0x8000, sp_arm7flx, (sp_header->main_size - (0x8000 - sp_header->main_rom_offset)) );
    }
	CARD_ReadRom( MI_DMA_NOT_USE, (void *)sp_header->sub_rom_offset, sp_arm7flx, sp_header->sub_size );

	CARD_UnlockRom( 0x03 );
	
	// Arm7FLX のハッシュ計算
	MATH_CalcSHA1( s_digest, sp_arm7flx, sp_header->sub_size );

	// 該当する SDK を検索
	for( s_hit=0;s_hit<ARM7_INFO_NUM;s_hit++ )
	{
		for( m=0;m<MATH_SHA1_DIGEST_SIZE;m++ )
		{
			if( s_info[s_hit].hash[m] != s_digest[m] ) break;
		}
		if( m == MATH_SHA1_DIGEST_SIZE ) break;
	}
	if( s_hit == ARM7_INFO_NUM )
	{
		s_error = DETECT_ERROR_NO_HIT;
		//s_hit = 0;
		return;
	}

#ifndef DEBUG_MODE
	// もうバッファいらない
	OS_SetMainArenaLo( old_lo );
#endif
    
	s_error = 0;
	return;
}


// 初期化
void NitroArm7VerCheckerInit( void )
{
	GX_DispOff();
	GXS_DispOff();
	
	myDp_Printf( 0, 0, TXT_COLOR_BLUE, MAIN_SCREEN, "Component SDK Version Identifier");
	
	GXS_SetVisiblePlane( GX_PLANEMASK_BG0 );
	
	GX_DispOn();
	GXS_DispOn();
	
	FS_Init(3);

	// この時点でカードが存在していたらカードチェック開始
	if( HOTSW_IsCardExist() && HOTSW_IsCardAccessible() )
	{
		s_mode = 0;
		CheckCard();
	}else
	{
		s_mode = 1;
	}
	
	// 表示
	DrawMainScene();

    OS_Printf("*** TWLBannerFile size : 0x%08x\n", sizeof(TWLBannerFile) );
}

#define PAD_SECRET ( PAD_BUTTON_START | PAD_BUTTON_X | PAD_BUTTON_Y )

// メインループ
void NitroArm7VerCheckerMain(void)
{
	//--------------------------------------
	//  キー入力処理
	//--------------------------------------

	// SLEEP + X + Y + START で隠し ID 表示
	if( PAD_DetectFold() && 
		( gKeyWork.press == PAD_SECRET )
	  )
	{
		s_secret = TRUE;
	}else
	{
		s_secret = FALSE;
	}

	//myDp_Printf( 1, 16, TXT_COLOR_BLUE, MAIN_SCREEN, "slotmode : %d", HOTSW_IsCardAccessible());
	//myDp_Printf( 1, 17, TXT_COLOR_BLUE, MAIN_SCREEN, "exist      : %d", HOTSW_IsCardExist());

#ifdef DEBUG_MODE
    ReadKey(&gKey);

    if (gKey.rep & PAD_KEY_DOWN || gKey.trg & PAD_KEY_DOWN)
    {
        if( (gIndex + 1) * DUMP_SIZE < sp_header->sub_size )
        {
            gIndex++;
        }
    }
    if (gKey.rep & PAD_KEY_UP || gKey.trg & PAD_KEY_UP)
    {
        if( gIndex >= 1 )
        {
            gIndex--;
        }
    }
    if (gKey.rep & PAD_BUTTON_R || gKey.trg & PAD_BUTTON_R)
    {
        if( (gIndex + 0x100) * DUMP_SIZE < sp_header->sub_size )
        {
            gIndex+=0x100;
        }
    }
    if (gKey.rep & PAD_BUTTON_L || gKey.trg & PAD_BUTTON_L)
    {
        if( gIndex >= 0x100 )
        {
            gIndex-=0x100;
        }
    }
#endif
    
	// 再表示
	DrawMainScene();
	
	if( s_mode == 0)
	{
		// 結果表示モード
		// カードが抜けたのを検出したらカード無しモードへ
		if( HOTSW_IsCardExist() == FALSE )
		{
			s_mode = 1;
		}
	}else
	{
		// カード無しモード
		// カードが刺さったのを検出したら再起動
		if( HOTSW_IsCardExist() == TRUE ) {
			OS_DoApplicationJump( OS_GetTitleId(), OS_APP_JUMP_NORMAL );
		}
	}
	
}


/*---------------------------------------------------------------------------*
  Name:         ReadKey

  Description:  キー入力情報を取得し、入力情報構造体を編集する。
                押しトリガ、離しトリガ、押し継続リピートトリガ を検出する。

  Arguments:    pKey  - 編集するキー入力情報構造体を指定する。

  Returns:      None.
 *---------------------------------------------------------------------------*/
#ifdef DEBUG_MODE
void ReadKey(KeyInfo* pKey)
{
    static u16  repeat_count[12];
    int         i;
    u16         r;

    r = PAD_Read();
    pKey->trg = 0x0000;
    pKey->up = 0x0000;
    pKey->rep = 0x0000;

    for (i = 0; i < 12; i++)
    {
        if (r & (0x0001 << i))
        {
            if (!(pKey->cnt & (0x0001 << i)))
            {
                pKey->trg |= (0x0001 << i);     // 押しトリガ
                repeat_count[i] = 1;
            }
            else
            {
                if (repeat_count[i] > KEY_REPEAT_START)
                {
                    pKey->rep |= (0x0001 << i); // 押し継続リピート
                    repeat_count[i] = (u16) (KEY_REPEAT_START - KEY_REPEAT_SPAN);
                }
                else
                {
                    repeat_count[i]++;
                }
            }
        }
        else
        {
            if (pKey->cnt & (0x0001 << i))
            {
                pKey->up |= (0x0001 << i);      // 離しトリガ
            }
        }
    }

    pKey->cnt = r;  // 未加工キー入力
}
#endif
