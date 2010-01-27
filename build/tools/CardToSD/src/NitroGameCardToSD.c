/*---------------------------------------------------------------------------*
  Project:  NitroGameCardToSD
  File:     NitroGameCardToSD.c

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
#include "misc_simple.h"
#include "NitroGameCardToSD.h"

// define data------------------------------------------

//#define MENU_ELEMENT_NUM			5						// メニューの項目数

#define ARM7_INFO_NUM			3

#define ROMSIZE_TO_BYTE( x )		( 1 << ( x + 17 ) )

typedef struct Arm7Info
{
    char name[32];
    unsigned char hash[MATH_SHA1_DIGEST_SIZE];
}
Arm7Info;

typedef enum DetectError
{
	DETECT_ERROR_NONE = 0,
	DETECT_ERROR_FILE_CREATE = 1,
	DETECT_ERROR_FILE_OPEN = 2,
	DETECT_ERROR_FILE_WRITE = 3,
	DETECT_ERROR_GAMECARD_REMOVED = 4,
	DETECT_ERROR_MAX
}
DetectError;

typedef enum RunningMode
{
	MODE_WRITABLE = 0,
	MODE_NOSD = 1,
	MODE_WRITING = 2,
	MODE_WAITING_GAMECARD = 3,
	MODE_MAX
}
RunningMode;

typedef enum SDStat
{
	SDSTAT_INSERTED = 0,
	SDSTAT_REMOVED = 1,
	SDSTAT_MAX
}
SDStat;

// extern data------------------------------------------

// function's prototype declaration---------------------

static void MenuScene( void );

// global variable -------------------------------------

// static variable -------------------------------------

static char s_mode = 0;
static BOOL s_secret = FALSE;
static u8 s_error = DETECT_ERROR_NONE;
static u8 s_digest[MATH_SHA1_DIGEST_SIZE];
static u8 s_hit = 0;
static s32 s_SDCheck = 0;
static BOOL s_protected = FALSE;
static BOOL s_protect_checked = FALSE;
static FSEventHook s_hook;
static u32 s_read_count = 0;
static BOOL s_complete = FALSE;


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
	"NitroSDK 2.0 RC4 plus 3",
	{ 0x83, 0x1E, 0x93, 0x52, 0x58, 0x9A, 0xF5, 0x11, 0x62, 0x06,
	  0x63, 0x7F, 0x79, 0x57, 0xDD, 0xB2, 0x24, 0x3B, 0x95, 0x33 }
    },
};

static const char *s_error_message[ DETECT_ERROR_MAX ] =
{
	"NOTHING",
	"ERROR_FILE_CREATE",
	"ERROR_FILE_OPEN",
	"ERROR_FILE_WRITE",
	"DETECT_ERROR_GAMECARD_REMOVED"
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

//******************************************************************************
//      SD抜け監視コールバック
//******************************************************************************

static void subCallBack( void* userdata, FSEvent event, void* argument )
{
#pragma unused( userdata, argument )

    if( event == FS_EVENT_MEDIA_INSERTED )
    {
        s_SDCheck = SDSTAT_INSERTED;  //挿入
        s_protect_checked = FALSE;
    }
    else if( event == FS_EVENT_MEDIA_REMOVED )
    {
        s_SDCheck = SDSTAT_REMOVED;  //抜け
    }
}

static BOOL IsSDProtected( void )
{
	FSPathInfo info;
	if( TRUE == FS_GetPathInfo("sdmc:/", &info) )
	{
		if( info.attributes & FS_ATTRIBUTE_IS_PROTECTED )
		{
			return TRUE;
		}
	}
	return FALSE;
}

static FSResult IsSDExist( void )
{
	FSPathInfo info;
	if( TRUE == FS_GetPathInfo("sdmc:/", &info) )
	{
		return FS_RESULT_SUCCESS;
	}else
	{
		return FS_GetArchiveResultCode("sdmc:/");
	}
}

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
	// 共通描画処理
	myDp_Printf( 0, 0, TXT_COLOR_BLUE, MAIN_SCREEN, "DS Game Card To SD");
	
	if( s_error != DETECT_ERROR_NONE )
	{
		// エラー表示
		myDp_Printf( 1, 8, TXT_COLOR_RED, MAIN_SCREEN, "Error : %d", s_error);
		myDp_Printf( 1, 9, TXT_COLOR_RED, MAIN_SCREEN, "        %s", s_error_message[ s_error ]);
		myDp_Printf( 1, 10, TXT_COLOR_RED, MAIN_SCREEN, "SDERROR%d", FS_GetArchiveResultCode("sdmc:/") );
	}
	
	// モードごとの描画処理
	switch( s_mode )
	{
		case MODE_WRITABLE:
			// ライト可モード
			myDp_Printf( 1, 3, TXT_COLOR_BLACK, MAIN_SCREEN, "Press A Button to start.");
			myDp_Printf( 1, 4, TXT_COLOR_BLACK, MAIN_SCREEN, "GameCode        : %c%c%c%c", sp_header->game_code[0],sp_header->game_code[1],sp_header->game_code[2],sp_header->game_code[3]);
			if( s_complete )
			{
				myDp_Printf( 1, 6, TXT_COLOR_BLUE, MAIN_SCREEN, "Complete.");
			}
			break;
		case MODE_NOSD:
			// SD カード見当たらないモード
			if( ( s_SDCheck == SDSTAT_INSERTED ) && s_protected)
			{
				myDp_Printf( 1, 3, TXT_COLOR_BLACK, MAIN_SCREEN, "SD card is locked.");
				myDp_Printf( 1, 4, TXT_COLOR_BLACK, MAIN_SCREEN, "Please insert unlocked SD card.");
			}else
			{
				myDp_Printf( 1, 3, TXT_COLOR_BLACK, MAIN_SCREEN, "Please insert SD card.");
			}
			break;
		case MODE_WRITING:
			{
				// ライティングモード
				int l;
				static u8 count = 0;
				static u8 count2 = 0;
				static u8 cc = 0;
				u64 prog = ( 30ULL * s_read_count ) / ROMSIZE_TO_BYTE( sp_header->rom_size ) ;
				myDp_Printf( 1, 3, TXT_COLOR_BLACK, MAIN_SCREEN, "Now writing%c%c%c%c",
							 ( (count%5>0) ? '.' : ' ' ), ( (count%5>1) ? '.' : ' ' ), ( (count%5>2) ? '.' : ' ' ), ( (count%5>3) ? '.' : ' ' ) );
				myDp_Printf( 1, 4, TXT_COLOR_BLACK, MAIN_SCREEN, "GameCode        : %c%c%c%c",
							 sp_header->game_code[0],sp_header->game_code[1],sp_header->game_code[2],sp_header->game_code[3]);
				myDp_Printf( 1, 6, TXT_COLOR_BLACK, MAIN_SCREEN, "------------------------------");
				for( l=0;l<prog;l++ )
				{
					myDp_Printf( 1+l, 6, (u8)( (l + (47 - count2))/3 ), MAIN_SCREEN, ">");
				}
				cc++;
				if(cc == 6)
				{
					cc = 0;
					count++;
					count2++;
					if( count == 5 ) count = 0;
					if( count2 == 48 ) count2 = 0;
				}
			}
			break;
		case MODE_WAITING_GAMECARD:
			// カード待ちモード
			myDp_Printf( 1, 3, TXT_COLOR_BLACK, MAIN_SCREEN, "Please insert DS game card.");
			break;
		default:
			break;
	}
}

#define SIZE_512K (512 * 1024)

static void SaveCardToSD( void *arg )
{
#pragma unused( arg )
	void *p_buf;
	void *old_lo;
	FSFile dest;
    char filename[64];
    RTCDate date;
    RTCTime time;
	
	s32 lock_id = OS_GetLockID();
	
	s_error = DETECT_ERROR_NONE;
	s_complete = FALSE;
	
	CARD_Enable( TRUE );
	CARD_LockRom( (u16)lock_id );
	
	// 読み込みバッファはアリーナLoからHiまでの間を適当に使ってアリーナLoを移動させよう
	old_lo = OS_GetMainArenaLo();
	p_buf = (void *)( MATH_ROUNDUP32( (int)OS_GetMainArenaLo() ) );
	OS_SetMainArenaLo( (void *)((u32)p_buf + SIZE_512K ) ); // とりあえず1MBぐらい確保してみる
	
	// ヘッダのチェックはしないでとりあえず無視して読み込む

	// ROMの読み込み
	// まずBootSegment4kbはすでに読み込まれているはずのヘッダを利用する
	// HW_CARD_ROM_HEADER_SIZE(0x160)しか残ってないと思うので、とりあえずそれだけ。
	// 残りは0埋め。
	// その後読み込めない領域（KeyTable+Secure）が28KB。つまり0x7000ぶん。
	// 実際にカードから読み込むのは0x8000からとなる。
	// 実効サイズから0x8000引いておき、書き込み終了の目安とする
		
	// ファイル作成＆オープン

	RTC_GetDateTime( &date, &time );
	STD_TSNPrintf( filename, 64, "sdmc:/%c%c%c%c_20%02d%02d%02d_%02d%02d%02d.srl.dmp",
					sp_header->game_code[0],sp_header->game_code[1],sp_header->game_code[2],sp_header->game_code[3],
					date.year, date.month, date.day, time.hour, time.minute, time.second );
	
	FS_InitFile( &dest );
	if( !FS_CreateFile( filename, FS_PERMIT_W | FS_PERMIT_R) )
	{
		s_error = DETECT_ERROR_FILE_CREATE;
		return;
	}
	if( !FS_OpenFileEx( &dest, filename, FS_FILEMODE_W ))
	{
		BOOL read_ok = FALSE;
		int retry_count = 0;
		// なんか作成直後にオープンできない事があるのでリトライしてみる
		while( FS_RESULT_NO_ENTRY == FS_GetArchiveResultCode("sdmc:/") )
		{
			if( FS_OpenFileEx( &dest, filename, FS_FILEMODE_W ) )
			{
				read_ok = TRUE;
				break;
			}
			retry_count++;
			if( retry_count > 100 ) break;
		}
		if( !read_ok )
		{
			FS_CloseFile( &dest );
			s_error = DETECT_ERROR_FILE_OPEN;
			return;
		}
	}
	
	s_read_count = 0;
	
	// 読み込んで書き出し
	while( s_read_count < ROMSIZE_TO_BYTE( sp_header->rom_size ) )
	{
		u32 remain = ROMSIZE_TO_BYTE( sp_header->rom_size ) - s_read_count;
		u32 read_size = ( ( SIZE_512K < remain ) ? SIZE_512K : remain );
		
		// 一応カード抜けたかのチェックをしておく
		if( !( HOTSW_IsCardExist() && HOTSW_IsCardAccessible() ) )
		{
			// 読み込める状態ではない
			FS_CloseFile( &dest );
			s_error = DETECT_ERROR_GAMECARD_REMOVED;
			return;
		}
		
		CARD_ReadRom( MI_DMA_NOT_USE, (void *)s_read_count, p_buf, read_size );
		if( s_read_count == 0 )
		{
			// 最初の一回だけ処理しなければならない領域
			MI_CpuClear32( p_buf, 0x8000 );
			MI_CpuCopy32( sp_header, p_buf, HW_CARD_ROM_HEADER_SIZE );
		}
		if ( -1 == FS_WriteFile( &dest, (void *)p_buf, (s32)read_size ) )
		{
			// 書き込み失敗
			FS_CloseFile( &dest );
			s_error = DETECT_ERROR_FILE_WRITE;
			return;
		}
		
		s_read_count += read_size;
	}
	
	// ファイルクローズ
	FS_CloseFile( &dest );
	
	CARD_UnlockRom( (u16)lock_id );
	OS_ReleaseLockID( (u16)lock_id );
	
	// もうバッファいらない
	OS_SetMainArenaLo( old_lo );
	
	s_error = DETECT_ERROR_NONE;
	s_complete = TRUE;
	return;
}


// 初期化
void NitroGameCardToSDInit( void )
{
	GX_DispOff();
	GXS_DispOff();
	
	myDp_Printf( 0, 0, TXT_COLOR_BLUE, MAIN_SCREEN, "DS Game Card To SD");
	
	GXS_SetVisiblePlane( GX_PLANEMASK_BG0 );
	
	GX_DispOn();
	GXS_DispOn();
	
	FS_Init(3);
	RTC_Init();

	// ここでおおまかにモードを切り替える。まずゲームカードを見る
	if( HOTSW_IsCardExist() && HOTSW_IsCardAccessible() )
	{
		// ゲームカードがあったら SD も見る
		FSResult fsr = IsSDExist();
		s_protected = IsSDProtected();
		
		if( fsr == FS_RESULT_SUCCESS )
		{
			s_SDCheck = SDSTAT_INSERTED;
		}else
		{
			s_SDCheck = SDSTAT_REMOVED;
		}
		
		if( ( fsr == FS_RESULT_SUCCESS ) && !s_protected )
		{
			// 書き込み準備万端
			s_mode = MODE_WRITABLE;
		}else
		{
			// SD カードが無いかプロテクトされてる
			s_mode = MODE_NOSD;
		}
		
		// ここから SD カード挿抜を見はじめる
		FS_RegisterEventHook("sdmc", &s_hook, subCallBack, NULL);
		
	}else
	{
		// ゲームカードがない
		s_mode = MODE_WAITING_GAMECARD;
	}
	
	
	// 表示
	DrawMainScene();

}

static BOOL IsGameCardRemovedAndChangeMode( void )
{
	// ゲームカードが抜けたのを検出したらカード待ちモードへ
	if( HOTSW_IsCardExist() == FALSE )
	{
		s_mode = MODE_WAITING_GAMECARD;
		// もうSDカード見る必要なし
		FS_UnregisterEventHook( &s_hook );
		return TRUE;
	}
	return FALSE;
}

static BOOL IsCardRemovedAndChangeMode( void )
{
	// ゲームカード抜け検出とモードチェンジ
	if( IsGameCardRemovedAndChangeMode() == TRUE )
	{
		return TRUE;
	}
	
	// SD カードが抜けたか、いきなりプロテクトがかかったら、
	// SD カード見当たらないモードへ
	if( ( s_SDCheck == SDSTAT_REMOVED ) || s_protected )
	{
		s_mode = MODE_NOSD;
		return TRUE;
	}
	return FALSE;
}

#define THREAD_PRIO 17
#define STACK_SIZE  0x2000
static OSThread readwrite_thread;
static u64 stack[ STACK_SIZE / sizeof(u64) ];

// メインループ
void NitroGameCardToSDMain(void)
{
	OSIntrMode intrmode;
	// 再表示
	DrawMainScene();
	
	
	// SD カードが刺さったら一回だけプロテクトチェックする
	// 再び刺さるまでチェックしない
	
	// 割り込み禁止
	intrmode = OS_DisableInterrupts();
	if( !s_protect_checked )
	{
		s_protect_checked = TRUE;
		// ここで割り込み禁止解除しないとかえってこなくなる
		OS_RestoreInterrupts( intrmode );
		s_protected = IsSDProtected();
	}else
	{
		// こちらでも割り込み禁止解除
		OS_RestoreInterrupts( intrmode );
	}
	
	// モードごとのメイン処理（キー処理、モード遷移など）
	switch( s_mode )
	{
		case MODE_WRITABLE:
			// ライト可モード
			
			// カード、SDカード抜け検出と共通モードチェンジ
			if ( IsCardRemovedAndChangeMode() == TRUE )
			{
				break;
			}
			
			// キー入力があれば読み込みスレッド起動してライティングモードへ
			if( gKeyWork.trigger == PAD_BUTTON_A )
			{
				OS_CreateThread( &readwrite_thread, SaveCardToSD, NULL, stack+STACK_SIZE/sizeof(u64), STACK_SIZE, THREAD_PRIO );
				OS_WakeupThreadDirect( &readwrite_thread );
				s_mode = MODE_WRITING;
				break;
			}
			
			break;
		case MODE_NOSD:
			// SD カード見当たらないモード
			
			// ゲームカード抜け検出とモードチェンジ
			if ( IsGameCardRemovedAndChangeMode() == TRUE )
			{
				break;
			}
			
			// SD カードが刺さったらライト可モードへ
			// ただしプロテクトがかかってたら遷移しない
			if( s_SDCheck == SDSTAT_INSERTED )
			{
				if( !s_protected )
				{
					s_mode = MODE_WRITABLE;
					break;
				}
			}
			
			break;
		case MODE_WRITING:
			// ライティングモード
			
			// カード、SDカード抜け検出と共通モードチェンジ
			if ( IsCardRemovedAndChangeMode() == TRUE )
			{
				// とりあえず読み込みスレッド終了を待ってから終わる
				// そうしないと読み込み用スレッド並立してしまう可能性
				while( !OS_IsThreadTerminated( &readwrite_thread ) ){}
				break;
			}
			
			// 読み込みスレッドが終了したらライト可モードへ
			if( OS_IsThreadTerminated( &readwrite_thread ) )
			{
				s_mode = MODE_WRITABLE;
				s_read_count = 0; // 読み込みスレッド終わってからでないとかっこわるい
				break;
			}
			
			break;
		case MODE_WAITING_GAMECARD:
			// カード待ちモード
			
			// カードが刺さったのを検出したら再起動
			if( HOTSW_IsCardExist() == TRUE ) {
				OS_DoApplicationJump( OS_GetTitleId(), OS_APP_JUMP_NORMAL );
			}
			break;
		default:
			break;
	}
	
}

