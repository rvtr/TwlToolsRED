/*---------------------------------------------------------------------------*
  Project:  RepairExportSaveData
  File:     RepairExportSaveData.c

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
#include <twl/nam.h>
#include "RepairExportSaveData.h"
#include "common.h"
#include "screen.h"

// define data------------------------------------------
#define THREAD_PRIO 17
#define STACK_SIZE  0x2000

typedef enum RepairResult
{
	REPAIR_RESULT_SUCCESS = 0,
	REPAIR_RESULT_DIR_OPEN = 1,
	REPAIR_RESULT_RESCUE_DATA = 2,
	REPAIR_RESULT_VERIFY_DATA = 3,
	REPAIR_RESULT_MAX
}
RepairResult;

typedef enum RunningMode
{
	MODE_WRITABLE = 0,
	MODE_NOSD = 1,
	MODE_PROC = 2,
    MODE_RESULT = 3,
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
static void ResucuSDSaveData( void *arg );
static void* AllocForNAM(u32 size);
static void FreeForNAM(void* ptr);

// global variable -------------------------------------

// static variable -------------------------------------
static RepairResult s_result;

static char s_mode = 0;
static s32 s_SDCheck = 0;
static BOOL s_protected = FALSE;
static BOOL s_protect_checked = FALSE;
static FSEventHook s_hook;

static s32 s_error_code = 0;
static KeyInfo  gKey;

static OSThread s_thread;
static u64 s_stack[ STACK_SIZE / sizeof(u64) ];

static u8 s_font_color = 0xff;

// const data  -----------------------------------------
static const char *s_result_message[ REPAIR_RESULT_MAX ] =
{
	"Success",
	"Directory Access Error",
	"Rescue SaveData Error",
	"Verify SaveData Error"
};


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

static BOOL IsCardRemoved( void )
{
	// SD カードが抜けたか、いきなりプロテクトがかかったら、
	// SD カード見当たらないモードへ
	if( ( s_SDCheck == SDSTAT_REMOVED ) || s_protected )
	{
		s_mode = MODE_NOSD;
		return TRUE;
	}
	return FALSE;
}


/*---------------------------------------------------------------------------*
  Name:         DrawMainScene

  Description:  描画まとめ

  メモ：
  0xf0, // 黒			0xf1, // 赤				0xf2, // 緑
  0xf3, // 青			0xf4, // 黄				0xf5, // ピンク
  0xf6, // 水色			0xf7, // くすんだ赤		0xf8, // くすんだ緑
  0xf9, // くすんだ青 	0xfa, // くすんだ黄色	0xfb, // 紫
  0xfc, // うすい青		0xfd, // 灰色			0xfe, // 濃い灰色
 *---------------------------------------------------------------------------*/
#define CHANGE_INTERVAL       5
#define PROC_MARK_BASE_X     27
#define PROC_MARK_BASE_Y     20
static void DrawMainScene( void )
{
    static u8 count = 0;
    
    PutMainScreen( 2, 2, s_font_color, "+--------------------------+");
    PutMainScreen( 2, 3, s_font_color, "+                          +");
	PutMainScreen( 2, 4, s_font_color, "+ Repair Exported SaveData +");
    PutMainScreen( 2, 5, s_font_color, "+                          +");
    PutMainScreen( 2, 6, s_font_color, "+--------------------------+");

	// モードごとの描画処理
	switch( s_mode )
	{
		case MODE_WRITABLE:
			// ライト可モード
			PutMainScreen( 3, 12, 0xf6, "Press A Button to start.");
			break;
		case MODE_NOSD:
			// SD カード見当たらないモード
			if( ( s_SDCheck == SDSTAT_INSERTED ) && s_protected)
			{
				PutMainScreen( 1, 12, 0xf1, "SD card is locked.");
				PutMainScreen( 1, 14, 0xf1, "Please insert unlocked SD card.");
			}else
			{
				PutMainScreen( 3, 12, 0xf1, "Please insert SD card.");
			}
			break;
		case MODE_PROC:
            count++;

            if( count >= 0 && count < CHANGE_INTERVAL )
            {
                PutMainScreen( PROC_MARK_BASE_X, PROC_MARK_BASE_Y  , 0xfc, "+++");
                PutMainScreen( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+1, 0xfc, "++");
                PutMainScreen( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+2, 0xfc, "+");
            }
            else if( count >= CHANGE_INTERVAL && count < CHANGE_INTERVAL*2 )
            {
                PutMainScreen( PROC_MARK_BASE_X, PROC_MARK_BASE_Y  , 0xfc, "+++");
                PutMainScreen( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+1, 0xfc, "+++");
            }
            else if( count >= CHANGE_INTERVAL*2 && count < CHANGE_INTERVAL*3 )
            {
                PutMainScreen( PROC_MARK_BASE_X, PROC_MARK_BASE_Y  , 0xfc, "+++");
                PutMainScreen( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+1, 0xfc, " ++");
                PutMainScreen( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+2, 0xfc, "  +");
            }
            else if( count >= CHANGE_INTERVAL*3 && count < CHANGE_INTERVAL*4 )
            {
                PutMainScreen( PROC_MARK_BASE_X, PROC_MARK_BASE_Y  , 0xfc, " ++");
                PutMainScreen( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+1, 0xfc, " ++");
                PutMainScreen( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+2, 0xfc, " ++");
            }
            else if( count >= CHANGE_INTERVAL*4 && count < CHANGE_INTERVAL*5 )
            {
                PutMainScreen( PROC_MARK_BASE_X, PROC_MARK_BASE_Y  , 0xfc, "  +");
                PutMainScreen( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+1, 0xfc, " ++");
                PutMainScreen( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+2, 0xfc, "+++");
            }
            else if( count >= CHANGE_INTERVAL*5 && count < CHANGE_INTERVAL*6 )
            {
                PutMainScreen( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+1, 0xfc, "+++");
                PutMainScreen( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+2, 0xfc, "+++");
            }
            else if( count >= CHANGE_INTERVAL*6 && count < CHANGE_INTERVAL*7 )
            {
                PutMainScreen( PROC_MARK_BASE_X, PROC_MARK_BASE_Y  , 0xfc, "+");
                PutMainScreen( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+1, 0xfc, "++");
                PutMainScreen( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+2, 0xfc, "+++");
            }
            else if( count >= CHANGE_INTERVAL*7 && count < CHANGE_INTERVAL*8)
            {
                PutMainScreen( PROC_MARK_BASE_X, PROC_MARK_BASE_Y  , 0xfc, "++");
                PutMainScreen( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+1, 0xfc, "++");
                PutMainScreen( PROC_MARK_BASE_X, PROC_MARK_BASE_Y+2, 0xfc, "++");
            }
            if( count == CHANGE_INTERVAL*8 - 1 )
            {
                count = 0;
            }
            PutMainScreen( 3, 12, 0xf4, "Now Accessing SD card");
            PutMainScreen( 3, 14, 0xf4, "Please Do not Touch SD card");
			break;
        case MODE_RESULT:
                if( s_result == REPAIR_RESULT_SUCCESS )
                {
                    PutMainScreen( 4, 12, s_font_color, "%s!", s_result_message[s_result]);
                    PutMainScreen( 4, 14, s_font_color, "Repair SaveData Completed");
                    PutMainScreen( 4, 16, s_font_color, "Please Turn Off Power");
                }
                else
                {
                    PutMainScreen( 4, 12, s_font_color, "%s", s_result_message[s_result]);
                    PutMainScreen( 4, 14, s_font_color, "Error Code : %d", s_error_code);
                }
            break;
		default:
			break;
	}
}


/*---------------------------------------------------------------------------*
  Name:         RepairSaveDataInit

  Description:  初期化
 *---------------------------------------------------------------------------*/
void RepairSaveDataInit( void )
{
    NAM_Init(AllocForNAM, FreeForNAM);
    
	FS_Init(3);

    OS_InitTick();

    {
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
    }
    
	// 表示
	DrawMainScene();
}


/*---------------------------------------------------------------------------*
  Name:         RepairSaveDataMain

  Description:  メインループ
 *---------------------------------------------------------------------------*/
void RepairSaveDataMain(void)
{
    OSIntrMode intrmode;
    
	// 再表示
	DrawMainScene();
    
	// SD カードが刺さったら一回だけプロテクトチェックする
	// 再び刺さるまでチェックしない
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

    ReadKey(&gKey);
    
	// モードごとのメイン処理（キー処理、モード遷移など）
	switch( s_mode )
	{
        // ライト可モード
		case MODE_WRITABLE:
			// カード、SDカード抜け検出と共通モードチェンジ
			if ( IsCardRemoved() == TRUE )
			{
				break;
			}
			
			// キー入力があれば読み込みスレッド起動してライティングモードへ
			if( gKey.trg == PAD_BUTTON_A )
			{
				OS_CreateThread( &s_thread, ResucuSDSaveData, NULL, s_stack+STACK_SIZE/sizeof(u64), STACK_SIZE, THREAD_PRIO );
				OS_WakeupThreadDirect( &s_thread );
				s_mode = MODE_PROC;
				break;
			}
			
			break;
        
        // SD カード見当たらないモード
		case MODE_NOSD:
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

        // 処理モード
		case MODE_PROC:
            // SDカード抜け検出
			if ( IsCardRemoved() == TRUE )
			{
				// とりあえず読み込みスレッド終了を待ってから終わる
				// そうしないと読み込み用スレッド並立してしまう可能性
				while( !OS_IsThreadTerminated( &s_thread ) ){}
				break;
			}
			
			// 読み込みスレッドが終了したらライト可モードへ
			if( OS_IsThreadTerminated( &s_thread ) )
			{
				s_mode = MODE_RESULT;
                s_font_color = (s_result == REPAIR_RESULT_SUCCESS) ? (u8)0xf2 : (u8)0xf1;
				break;
			}

            break;
        
        // 結果モード
		case MODE_RESULT:
            break;
		default:
			break;
	}
}


/*---------------------------------------------------------------------------*
  Name:         ResucuSDSaveData

  Description:  SDカードに退避されたアプリのセーブデータの署名を付け替える
 *---------------------------------------------------------------------------*/
static void ResucuSDSaveData(  void *arg  )
{
#pragma unused( arg )
    FSFile file;
    FSDirectoryEntryInfo dir;
    RepairResult result = REPAIR_RESULT_SUCCESS;
    
    FS_InitFile(&file);
    
    if( FS_OpenDirectory(&file, "sdmc:/private/ds/title", FS_PERMIT_R ) )
    {
        OS_TPrintf("Open Directory Fail...\n");
    }
    
    while( FS_ReadDirectory(&file, &dir) )
    {
        char* pExtension;
        char path_buf[FS_ENTRY_LONGNAME_MAX];

        // 拡張子が .bin のものだけ処理を行う
        pExtension = STD_SearchCharReverse( dir.longname, '.');
        if( pExtension )
        {
            if(!STD_CompareString( pExtension, ".bin"))
            {
                STD_TSNPrintf( path_buf, FS_ENTRY_LONGNAME_MAX, "sdmc:/private/ds/title/%s", dir.longname );
                
                OS_TPrintf("%s\n", path_buf);

                {
                    FSFile tempFile;
                    BOOL result;
                    
                    FS_InitFile(&tempFile);
                    result = FS_OpenFileEx( &tempFile, path_buf, FS_FILEMODE_R);

                    if( result )
                    {
                        OS_TPrintf("opne success\n");
                    }
                    FS_CloseFile(&tempFile);
                }

                s_error_code = NAM_RescueBkp( path_buf );

                // セーブデータの救済処置
                if(s_error_code == NAM_OK)
                {
                    // 処理が成功したらベリファイ
                    if(NAM_VerifyBkpStrict( path_buf ) != NAM_OK)
                    {
                        result = REPAIR_RESULT_VERIFY_DATA;
                        break;
                    }
                }
                else
                {
                    result = REPAIR_RESULT_RESCUE_DATA;
                    break;
                }
            }
        }
    }
    
    FS_CloseDirectory(&file);

    s_result = result;
}


/*---------------------------------------------------------------------------*
  Name:         AllocForNAM

  Description:  
 *---------------------------------------------------------------------------*/
static void* AllocForNAM(u32 size)
{
	void* ptr;
	ptr = OS_AllocFromMain(size);
	
	if (ptr == NULL)
	{
		OS_Panic("alloc failed.");
	}
	
	return ptr;
	
}


/*---------------------------------------------------------------------------*
  Name:         FreeForNAM

  Description:  
 *---------------------------------------------------------------------------*/
static void FreeForNAM(void* ptr)
{
	OS_FreeToMain(ptr);
}

