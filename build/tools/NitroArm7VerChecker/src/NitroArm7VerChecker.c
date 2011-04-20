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

// ----- ビルドモードの切り替え -----
// 通常版・特殊版の切り替えはここの定義で行う
#define NORMAL_CHECK_TOOL

// デバッグビルド切り替え(有効にする場合はどちらか片方に)
//#define DEBUG_MEM_DUMP // 現在使用禁止
//#define DEBUG_SHOW_SDK_INFO


#include <twl.h>
#include <nitro/os/ARM9/argument.h>
#include <twl/os/common/format_rom.h>
#include <twl/os/common/banner.h>
#include "misc_simple.h"
#include "NitroArm7VerChecker.h"
#include "revision.h"
#include <../build/libraries/mb/common/include/mb_fileinfo.h>
#include <string.h>

#ifdef NORMAL_CHECK_TOOL
#include "SdkInfoData_normal.h"
#else
#include "SdkInfoData_special.h"
#endif

// define data------------------------------------------
#define SHOW_SDK_INFO_NUM       20
#define SHOW_SDK_INFO_SIZE      50

#ifdef DEBUG_SHOW_SDK_INFO
#define KEY_REPEAT_START        10  // キーリピート開始までのフレーム数
#define KEY_REPEAT_SPAN         1   // キーリピートの間隔フレーム数
#define DUMP_SIZE               0x80
#endif

typedef enum DetectError
{
	DETECT_ERROR_NONE = 0,
	DETECT_ERROR_ARM7FLX_SIZE_OVER = 1,
	DETECT_ERROR_ARM7FLX_SIZE_ZERO = 2,
	DETECT_ERROR_NO_HIT = 3,
	DETECT_ERROR_MAX
}
DetectError;

#ifdef DEBUG_SHOW_SDK_INFO | DEBUG_MEM_DUMP
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
static void SplitToken(void);
#ifdef DEBUG_SHOW_SDK_INFO | DEBUG_MEM_DUMP
void ReadKey(KeyInfo* pKey);
#endif

// global variable -------------------------------------

// static variable -------------------------------------

static char s_mode = 0;
static BOOL s_secret = FALSE;
static BOOL s_show_error = FALSE;
static u8 s_error = 0;
static u8 s_digest[MATH_SHA1_DIGEST_SIZE];
static u32 s_hit = 0;
static char s_sdk_name[SHOW_SDK_INFO_NUM][SHOW_SDK_INFO_SIZE];
static int s_same_sdk_num;
#ifdef DEBUG_SHOW_SDK_INFO | DEBUG_MEM_DUMP
static KeyInfo  gKey;
static u32 gIndex;
static void *data_buf;
#endif

// const data  -----------------------------------------

static const ROM_Header_Short *sp_header = (ROM_Header_Short *)HW_CARD_ROM_HEADER;

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
/*---------------------------------------------------------------------------*
  Name:         DrawMainScene

  Description:  描画まとめ
 *---------------------------------------------------------------------------*/
static void DrawMainScene( void )
{
#ifdef DEBUG_MEM_DUMP
    int i,j,idx;
    u8 *buf = (u8 *)data_buf + gIndex*DUMP_SIZE;

    if( s_mode == 0 )
    {
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
    }
#endif
    
	//int l;
	myDp_Printf( 0, 7, TXT_COLOR_BLACK, SUB_SCREEN, "Component SDK Version Identifier");


#ifdef DEBUG_SHOW_SDK_INFO
    myDp_Printf( 0, 1, TXT_COLOR_BLUE,  MAIN_SCREEN, "s_same_sdk_num : %d", s_same_sdk_num);
    myDp_Printf( 0, 2, TXT_COLOR_BLUE,  MAIN_SCREEN, "s_hit : %d", s_hit);
    {
        int i;
        for( i = 0; i < s_same_sdk_num; i++ )
        {
            if( s_sdk_name[i][0] == '2' || s_sdk_name[i][0] == '3' || s_sdk_name[i][0] == '4' || s_sdk_name[i][0] == '5' )
            {
                myDp_Printf( 1, 5+i, TXT_COLOR_BLUE,  MAIN_SCREEN, "SDK %s", s_sdk_name[i]);
            }
            else
            {
                myDp_Printf( 5, 5+i, TXT_COLOR_BLACK,  MAIN_SCREEN, "[%s", s_sdk_name[i]);
            }
        }
        if( s_show_error )
        {
            myDp_Printf( 1, 23, TXT_COLOR_RED,  MAIN_SCREEN, "Error : Can Not Show SDK Info");
        }
    }
#endif


#ifdef DEBUG_MEM_DUMP
    if( 1 )
#else
	if( s_mode == 0 )
#endif
	{
		// 結果表示モード
		if( s_error != DETECT_ERROR_NONE )
		{
			// エラー表示
			myDp_Printf( 1,10, TXT_COLOR_RED, SUB_SCREEN, "Registered ARM7 component");
			myDp_Printf( 1,11, TXT_COLOR_RED, SUB_SCREEN, "was not detected.");
			myDp_Printf( 1,13, TXT_COLOR_RED, SUB_SCREEN, "Error : %d", s_error);
		}
        else
		{
			// 通常結果表示
			myDp_Printf( 1,10, TXT_COLOR_DARKGREEN, SUB_SCREEN, "Registered ARM7 component");
			myDp_Printf( 1,11, TXT_COLOR_DARKGREEN, SUB_SCREEN, "was detected.");
            
			myDp_Printf( 1, 1, TXT_COLOR_BLACK, MAIN_SCREEN, "GameCode  :");
            myDp_Printf(13, 1, TXT_COLOR_BLUE,  MAIN_SCREEN, "%c%c%c%c", sp_header->game_code[0],sp_header->game_code[1],sp_header->game_code[2],sp_header->game_code[3]);
            myDp_Printf( 1, 3, TXT_COLOR_BLACK, MAIN_SCREEN, "TitleName :");
            myDp_Printf(13, 3, TXT_COLOR_BLUE,  MAIN_SCREEN, "%s", sp_header->title_name);

            
            {
                int i;
                for( i = 0; i < s_same_sdk_num; i++ )
                {
                    if( s_sdk_name[i][0] == '2' || s_sdk_name[i][0] == '3' || s_sdk_name[i][0] == '4' || s_sdk_name[i][0] == '5' )
                    {
                        myDp_Printf( 1, 5+i, TXT_COLOR_BLUE,  MAIN_SCREEN, "SDK %s", s_sdk_name[i]);
                    }
                    else
                    {
                        myDp_Printf( 5, 5+i, TXT_COLOR_BLACK,  MAIN_SCREEN, "[%s", s_sdk_name[i]);
                    }
                }
                if( s_show_error )
                {
                    myDp_Printf( 1, 23, TXT_COLOR_RED,  MAIN_SCREEN, "Error : Can Not Show SDK Info");
                }
            }
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
		myDp_Printf( 1,11, TXT_COLOR_BLACK, SUB_SCREEN, "Please insert DS game card.");
	}
	
	// 隠し ID 表示
	if ( s_secret )
	{
		myDp_Printf( 1, 20, TXT_COLOR_BLUE, SUB_SCREEN, "ID : %d", ID_NUM);
	}
}


/*---------------------------------------------------------------------------*
  Name:         CheckCard

  Description:  
 *---------------------------------------------------------------------------*/
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

#ifdef DEBUG_MEM_DUMP
    data_buf = sp_arm7flx;
#endif
    // ARM9のSecure除いた部分を空読み出し
    if( (sp_header->main_rom_offset + sp_header->main_size) > 0x8000 )
    {
#ifdef DEBUG_MEM_DUMP
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
	for( s_hit=0;s_hit<SDK_INFO_NUM;s_hit++ )
	{
		for( m=0;m<MATH_SHA1_DIGEST_SIZE;m++ )
		{
			if( s_sdk_info[s_hit].hash[m] != s_digest[m] ) break;
		}
		if( m == MATH_SHA1_DIGEST_SIZE ) break;
	}
    
	if( s_hit == SDK_INFO_NUM )
	{
		s_error = DETECT_ERROR_NO_HIT;
		//s_hit = 0;
		return;
	}
    else
    {
        SplitToken();
    }

#ifndef DEBUG_MEM_DUMP
	// もうバッファいらない
	OS_SetMainArenaLo( old_lo );
#endif
    
	s_error = 0;
	return;
}


/*---------------------------------------------------------------------------*
  Name:         SplitToken

  Description:  表示用のSDK情報作成
 *---------------------------------------------------------------------------*/
static void SplitToken(void)
{
    char *pos;
    Arm7Info temp;

    s_same_sdk_num = 0;
    s_show_error = FALSE;

    // strtok が 元の配列を壊してしまう(tokenをnullで書き換えてしまう)ので、tempで作業する
    memcpy( &temp, &s_sdk_info[s_hit], sizeof(Arm7Info));
    pos = strtok( temp.name, "/[" );

    while( pos != NULL )
    {
        if( strlen(pos) < SHOW_SDK_INFO_SIZE )
        {
            strcpy( s_sdk_name[s_same_sdk_num], pos);
        }
        else
        {
            s_show_error = TRUE;
            break;
        }
        
        s_same_sdk_num++;

        if( s_same_sdk_num >= SHOW_SDK_INFO_NUM )
        {
            s_show_error = TRUE;
            break;
        }
        
        pos = strtok(NULL, "/[");
    }
}


/*---------------------------------------------------------------------------*
  Name:         NitroArm7VerCheckerInit

  Description:  初期化
 *---------------------------------------------------------------------------*/
void NitroArm7VerCheckerInit( void )
{
	GX_DispOff();
	GXS_DispOff();
	
	myDp_Printf( 0, 0, TXT_COLOR_BLUE, MAIN_SCREEN, "Component SDK Version Identifier");
	
	GXS_SetVisiblePlane( GX_PLANEMASK_BG0 );
	
	GX_DispOn();
	GXS_DispOn();
	
	FS_Init(3);

    OS_InitTick();

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


/*---------------------------------------------------------------------------*
  Name:         NitroArm7VerCheckerMain

  Description:  メインループ
 *---------------------------------------------------------------------------*/
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

#ifdef DEBUG_SHOW_SDK_INFO
    {
        BOOL change = FALSE;
        ReadKey(&gKey);

        if (gKey.rep & PAD_KEY_UP || gKey.trg & PAD_KEY_UP)
        {
            if( s_hit < SDK_INFO_NUM )
            {
                s_hit++;
                change = TRUE;
            }
            else if( s_hit == SDK_INFO_NUM )
            {
                s_hit = 0;
                change = TRUE;
            }
        }
        if (gKey.rep & PAD_KEY_DOWN || gKey.trg & PAD_KEY_DOWN)
        {
            if( s_hit > 0 )
            {
                s_hit--;
                change = TRUE;
            }
            else if( s_hit == 0 )
            {
                s_hit = SDK_INFO_NUM;
                change = TRUE;
            }
        }

        if( change == TRUE )
        {
            SplitToken();
        }
    }
#endif
    
#ifdef DEBUG_MEM_DUMP
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

    if (gKey.rep & PAD_KEY_LEFT || gKey.trg & PAD_KEY_LEFT)
    {
        if( s_hit - 1 >= 0 )
        {
            s_hit--;
        }
    }
    if (gKey.rep & PAD_KEY_RIGHT || gKey.trg & PAD_KEY_RIGHT)
    {
        if( s_hit + 1 < SDK_INFO_NUM )
        {
            s_hit++;
        }
    }
    
    if (gKey.trg & PAD_BUTTON_B)
    {
        if(s_error < DETECT_ERROR_MAX)
        {
            s_error++;
        }
        else
        {
            s_error=0;
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
#ifdef DEBUG_SHOW_SDK_INFO | DEBUG_MEM_DUMP
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
