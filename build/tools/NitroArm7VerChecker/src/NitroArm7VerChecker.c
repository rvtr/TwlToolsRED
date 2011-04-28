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

#define THREAD_PRIO 17
#define STACK_SIZE  0x2000

typedef enum DetectError
{
	DETECT_ERROR_NONE = 0,
	DETECT_ERROR_ARM7FLX_SIZE_OVER = 1,
	DETECT_ERROR_ARM7FLX_SIZE_ZERO = 2,
	DETECT_ERROR_NO_HIT = 3,
	DETECT_ERROR_MAX
}
DetectError;

typedef enum RunningMode
{
	MODE_NOCARD = 0,
	MODE_PROC = 1,
    MODE_RESULT = 2,
	MODE_MAX
}
RunningMode;

#ifdef DEBUG_SHOW_SDK_INFO
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
static void CheckCard( void *arg );
#ifdef DEBUG_SHOW_SDK_INFO
void ReadKey(KeyInfo* pKey);
#endif

// global variable -------------------------------------

// static variable -------------------------------------
static char s_mode;
static BOOL s_secret = FALSE;
static BOOL s_show_error = FALSE;
static u8 s_error = 0;
static u8 s_digest[MATH_SHA1_DIGEST_SIZE];
static u32 s_hit = 0;
static char s_sdk_name[SHOW_SDK_INFO_NUM][SHOW_SDK_INFO_SIZE];
static int s_same_sdk_num;

static OSThread s_thread;
static u64 s_stack[ STACK_SIZE / sizeof(u64) ];

#ifdef DEBUG_SHOW_SDK_INFO
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
#define CHANGE_INTERVAL       3
#define PROC_MARK_BASE_X     27
#define PROC_MARK_BASE_Y     20
static void DrawMainScene( void )
{
    static u8 count = 0;
    
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

    switch( s_mode )
    {
        // カードなし状態
      case MODE_NOCARD:
		// カード無しモード
        myDp_Printf( 1,11, TXT_COLOR_BLACK, SUB_SCREEN, "Please insert DS game card.");
        break;

        // 処理中状態
      case MODE_PROC:
        count++;

        if( count >= 0 && count < CHANGE_INTERVAL )
        {
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y  , TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "+++");
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+1, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "++");
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+2, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "+");
        }
        else if( count >= CHANGE_INTERVAL && count < CHANGE_INTERVAL*2 )
        {
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y  , TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "+++");
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+1, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "+++");
        }
        else if( count >= CHANGE_INTERVAL*2 && count < CHANGE_INTERVAL*3 )
        {
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y  , TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "+++");
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+1, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, " ++");
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+2, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "  +");
        }
        else if( count >= CHANGE_INTERVAL*3 && count < CHANGE_INTERVAL*4 )
        {
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y  , TXT_COLOR_LIGHTBLUE, SUB_SCREEN, " ++");
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+1, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, " ++");
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+2, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, " ++");
        }
        else if( count >= CHANGE_INTERVAL*4 && count < CHANGE_INTERVAL*5 )
        {
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y  , TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "  +");
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+1, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, " ++");
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+2, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "+++");
        }
        else if( count >= CHANGE_INTERVAL*5 && count < CHANGE_INTERVAL*6 )
        {
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+1, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "+++");
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+2, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "+++");
        }
        else if( count >= CHANGE_INTERVAL*6 && count < CHANGE_INTERVAL*7 )
        {
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y  , TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "+");
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+1, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "++");
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+2, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "+++");
        }
        else if( count >= CHANGE_INTERVAL*7 && count < CHANGE_INTERVAL*8)
        {
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y  , TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "++");
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+1, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "++");
            myDp_Printf( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+2, TXT_COLOR_LIGHTBLUE, SUB_SCREEN, "++");
        }
        if( count == CHANGE_INTERVAL*8 - 1 )
        {
            count = 0;
        }
        break;
        
        // 結果表示状態
      case MODE_RESULT:
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
		}
        break;

      default:
        break;
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
static void CheckCard(  void *arg  )
{
#pragma unused( arg )
    
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

    // ARM9のSecure除いた部分を空読み出し
    if( (sp_header->main_rom_offset + sp_header->main_size) > 0x8000 )
    {
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

	// もうバッファいらない
	OS_SetMainArenaLo( old_lo );
    
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

    GXS_SetVisiblePlane( GX_PLANEMASK_BG0 );
	
	GX_DispOn();
	GXS_DispOn();
	
	FS_Init(3);
    OS_InitTick();

    s_mode = MODE_NOCARD;
    
	// この時点でカードが存在していたらカードチェック開始
	if( HOTSW_IsCardExist() && HOTSW_IsCardAccessible() )
	{
        OS_TPrintf("Card Exist\n");
        OS_CreateThread( &s_thread, CheckCard, NULL, s_stack+STACK_SIZE/sizeof(u64), STACK_SIZE, THREAD_PRIO );
        OS_WakeupThreadDirect( &s_thread );
		s_mode = MODE_PROC;
	}

	// 表示
	DrawMainScene();
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
    
	// 再表示
	DrawMainScene();

    switch( s_mode )
    {
        // カードなしモード
      case MODE_NOCARD:
        // カードが刺さったのを検出したら再起動
		if( HOTSW_IsCardExist() == TRUE ) {
			OS_DoApplicationJump( OS_GetTitleId(), OS_APP_JUMP_NORMAL );
		}
        break;

        // 処理中モード
      case MODE_PROC:
        if( OS_IsThreadTerminated( &s_thread ) )
        {
            s_mode = MODE_RESULT;
        }
        break;

        // 結果表示モード
      case MODE_RESULT:
		// カードが抜けたのを検出したらカード無しモードへ
		if( HOTSW_IsCardExist() == FALSE )
		{
			s_mode = MODE_NOCARD;
		}
        break;

      default:
        break;
    }
}


/*---------------------------------------------------------------------------*
  Name:         ReadKey

  Description:  キー入力情報を取得し、入力情報構造体を編集する。
                押しトリガ、離しトリガ、押し継続リピートトリガ を検出する。

  Arguments:    pKey  - 編集するキー入力情報構造体を指定する。

  Returns:      None.
 *---------------------------------------------------------------------------*/
#ifdef DEBUG_SHOW_SDK_INFO
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
