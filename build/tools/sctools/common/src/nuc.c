#include <twl.h>
#include <twl/sea.h>
#include <twl/na.h>
#include <TwlWiFi/nuc.h>

#include "netconnect.h"
#include "sitedefs.h"
#include "nuc.h"
#include "nuc_error_msg.h"

// Network Updateのフェーズを表す
typedef enum{

    PHASE_INIT,                 // 初期状態
    PHASE_READY,                // 開始
    PHASE_CONNECTING_NETWORK,   // ネットワーク接続中
    PHASE_TEST_READY,           // 接続テスト開始
    PHASE_TEST_PROCESS,         // 接続テスト中
    PHASE_TEST_GETTING_WII_ID,  // Wii ID取得処理
    PHASE_TEST_FINISHED,        // 接続テスト完了
    PHASE_TEST_CLEANUP,         // 接続テスト後処理
    PHASE_NUP_BREAK,            // <キー入力待ち>アップデート前確認待ち
    PHASE_NUP_READY,            // ネットワークアップデート開始
    PHASE_NUP_CHECK,            // ネットワークアップデート更新情報取得中
    PHASE_NUP_DOWNLOAD,         // ネットワークアップデート ダウンロード開始
    PHASE_NUP_PROCESS,          // ネットワークアップデート中
    PHASE_NUP_FINISHED,         // ネットワークアップデート完了
    PHASE_NUP_CLEANUP,          // ネットワークアップデート後処理
    PHASE_NUP_SKIPPED,          // ネットワークアップデートがスキップされた
    PHASE_CLEANING_UP,          // 後処理
    PHASE_FINISHED,             // 完了
    PHASE_ERROR_OCCURRED         // エラー発生
} NucPhaseState;


// ネットワーク接続状態
typedef enum{
    NET_CONNECT_NONE,
    NET_CONNECT_OK,
    NET_CONNECT_ERROR
} NetConnectState;

static volatile NetConnectState NetConnect = NET_CONNECT_NONE;

// state管理
static struct
{
    NucPhaseState state;
    u32 count;
} TestState;

static inline void ChangeState(NucPhaseState state)
{
    TestState.state = state;
    TestState.count = 0;
}

// titleID 取得用
static NUCTitleId TitleIds[NUC_MAX_TITLE_UPDATE_COUNT];
static u32 TitleIdNum;

static u8 WorkForNA[NA_VERSION_DATA_WORK_SIZE];

static BOOL AllocFailTest = FALSE;

static void *alloc(u32 size, int align)
{
    u32 *ptr = NULL, *realPtr = NULL;
    u32 realSize;
    OSIntrMode  old;

    old = OS_DisableInterrupts();

    /* realSize is size plus alignment and header */
    if (align < 4)
    {
        align = 4;
    }
    realSize = size + align + 4;

    realPtr = (u32 *) OS_Alloc(realSize);
    ptr = (u32 *)((((u32) realPtr) + 4 + align - 1) & ~(align - 1));

    *((u32 *)(((u32)ptr) - 4)) = (u32) realPtr;
    (void)OS_RestoreInterrupts( old );

  end:
    SDK_ASSERT(((u32)ptr & (align - 1)) == 0);
    return (void *) ptr;
}

static void free(void *p)
{
    u32 realPtr = *((u32 *)(((u32)p) - 4));
    OS_Free((void *) realPtr);
}

/*---------------------------------------------------------------------------*
  Name:         InitNupLib
  Description:  NUCライブラリを開始します。
 *---------------------------------------------------------------------------*/
static BOOL InitNupLib()
{
  BOOL ret;

  ret = NUC_Init(alloc, free);

  if (ret == FALSE) {
    OS_TPrintf("NUC_Init() failed, error code=%d\n", NUC_GetLastError());
  }
  return ret;
}

/*---------------------------------------------------------------------------*
  Name:         StartNupCheck
  Description:  ダウンロードリスト一覧の取得を開始します。
 *---------------------------------------------------------------------------*/
static BOOL StartNupCheck(void)
{
    BOOL ret;
    TitleIdNum = sizeof(TitleIds) / sizeof(TitleIds[0]);

    ret = NUC_CheckAsync(TitleIds, &TitleIdNum);
    if (ret == FALSE)
    {
        OS_TPrintf("NUC_CheckAsync() failed, error code=%d\n", NUC_GetLastError());
    }

    return ret;
}


/*---------------------------------------------------------------------------*
  Name:         ProgressNupCheck
  Description:  ダウンロードリスト一覧の取得状況を確認します
 *---------------------------------------------------------------------------*/
static NucStatus ProgressNupCheck(void)
{
    u64 CurrentSize, TotalSize;
    NucStatus status;

    NUC_GetProgress(&CurrentSize, &TotalSize, &status);
    if (status == NUC_STATUS_ERROR)
    {
        OS_TPrintf("NUC_GetProgress() failed in checking, error code=%d\n", NUC_GetLastError());
    }
#if 0
    if (TestState.count++ % STRING_ANIM_CNT == 0)
    {
        u32 num = (TestState.count / STRING_ANIM_CNT) % 3;
        const char* msg[] = {
            "Now checking list.   ", 
            "Now checking list..  ",
            "Now checking list... "};
        
        PrintString(TEXT_X, TEXT_Y, COLOR_WHITE, msg[num]);
    }
#endif    
    return status;
}

/*---------------------------------------------------------------------------*
  Name:         StartNupDownload
  Description:  ダウンロードを開始します。
 *---------------------------------------------------------------------------*/
static BOOL StartNupDownload(void)
{
  /* こいつが呼ばれたらFSが切り離されてしまうのでリブートが必要 */
    BOOL ret = NUC_DownloadAsync(TitleIds, TitleIdNum);

    if (ret == FALSE)
    {
        OS_TPrintf("NUP_DownloadAsync() failed, error code=%d\n", NUC_GetLastError());
    }

    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         ProgressNupDownload
  Description:  ダウンロードの進行状況を表示します。
 *---------------------------------------------------------------------------*/
static NucStatus ProgressNupDownload(void)
{
    u64 CurrentSize, TotalSize;
    NucStatus status;

    NUC_GetProgress(&CurrentSize, &TotalSize, &status);
    if (status == NUC_STATUS_ERROR)
    {   // エラー発生
        OS_TPrintf("NUC_GetProgress() failed in download, error code=%d\n", NUC_GetLastError());
    }
    else { 
#if 0
      // ダウンロード状況を描画
      int dw = (int)((CurrentSize * BAR_W)/TotalSize);
      FillRect(BAR_X, BAR_Y, dw, BAR_H, GX_RGBA(31, 16, 16, 1));
#endif
    }

#if 0
    if (TestState.count++ % STRING_ANIM_CNT == 0)
    {
        u32 num = (TestState.count / STRING_ANIM_CNT) % 3;
        const char* msg[] = {
            "Now downloading.     ", 
            "Now downloading..    ",
            "Now downloading...   "};
        PrintString(TEXT_X, TEXT_Y, COLOR_WHITE, msg[num]);
    }
#endif    
    return status;
}

/*---------------------------------------------------------------------------*
  Name:         CleanNupLib
  Description:  NUCライブラリを終了します。
 *---------------------------------------------------------------------------*/
static BOOL CleanNupLib()
{
    BOOL ret = NUC_Cleanup(TitleIds, TitleIdNum);
    if (ret == FALSE)
    {
        OS_TPrintf("NUP_CleanUp() failed, error code=%d\n", NUC_GetLastError());
    }
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         ProgressNetConnect
  Description:  ネットワーク接続を待ちます。
 *---------------------------------------------------------------------------*/
static void ProgressNetConnect(void)
{
#if 0
    if (TestState.count++ % STRING_ANIM_CNT == 0)
    {
        u32 num = (TestState.count / STRING_ANIM_CNT) % 3;
        const char* msg[] = {
            "Connecting network.  ", 
            "Connecting network.. ",
            "Connecting network..."};
        PrintString(TEXT_X, TEXT_Y, COLOR_WHITE, msg[num]);
    }
#endif
}



static void ShowErrorMsg(int error_code)
{
#if 0
    PrintString(TEXT_X, TEXT_Y,       COLOR_WHITE,      "Error Occurred        ");
#endif
    if (error_code > 0)
    {
#if 0
        PrintString(TEXT_X, TEXT_Y + 10,  COLOR_WHITE, "Error Code:%d", error_code);
        PrintString(TEXT_X, TEXT_Y + 12, COLOR_WHITE, "%s", GetPublicMsg(error_code));
        PrintString(TEXT_X, TEXT_Y + 15, COLOR_WHITE, "%s", GetPrivateMsg(error_code));
#endif
        OS_TPrintf( "Error Code:%d\n", error_code);
        OS_TPrintf( "%s\n", GetPublicMsg(error_code));
        OS_TPrintf( "%s\n", GetPrivateMsg(error_code));
    }
    else
    {
      OS_TPrintf( "%s\n", "Network Error occurred.\nTry again later.");
#if 0
        // ネットワークエラー時の表示メッセージ(暫定)
        PrintString(TEXT_X, TEXT_Y + 10, COLOR_WHITE, "%s", "Network Error occurred.\nTry again later.");
#endif
    }
}

// ネットワーク接続関連
#define	STACK_SIZE	(1024*4)
static OSThread NetThread;
static u64 NetStack[STACK_SIZE / sizeof(u64)];
#define	  THREAD1_PRIO 24
static volatile BOOL NetHTTPEnd = FALSE;


static void NetConnectProc(void *arg)
{
#pragma unused(arg)
  BOOL ret;
  NucStatus status;
  int error_code = 0;

    
    /* Start networking */
    NcStart(SITEDEFS_DEFAULTCLASS);
    
    NetConnect = NET_CONNECT_OK;
    //OS_TPrintf("%s %d\n",__FUNCTION__,__LINE__);
    // 終了の呼び出しを待つ
//    OS_SleepThread(NULL);
    
  while(1) {
    OS_Sleep( 16 ); /* OS_WaitVBlankIntrの代わり */

    switch ( TestState.state ) {
    case PHASE_CONNECTING_NETWORK:
      ProgressNetConnect();
      if (NetConnect == NET_CONNECT_OK)
	{
	  ChangeState(PHASE_NUP_BREAK);
	}
      else if (NetConnect == NET_CONNECT_ERROR)
	{   // ネットワーク接続エラー
	  error_code = -1;
	  ChangeState(PHASE_ERROR_OCCURRED);
	}
      break;
    case PHASE_NUP_BREAK: // AボタンでNUPライブラリを初期化します。
      ret = InitNupLib();
      if (ret == FALSE)
	{   // エラー発生
	  ChangeState(PHASE_ERROR_OCCURRED);
	  error_code = NUC_GetLastError();
	}
      else
	{
	  ChangeState(PHASE_NUP_READY);
	}
      break;

    case PHASE_NUP_READY: // 更新情報の取得を開始します。
      ret = StartNupCheck();
      if (ret == FALSE)
	{   // エラー発生
	  error_code = NUC_GetLastError();
	  ChangeState(PHASE_NUP_CLEANUP);
	}
      else
	{
	  ChangeState(PHASE_NUP_CHECK);
	}
      break;

    case PHASE_NUP_CHECK: // 更新情報の取得状況を表示します。
      status = ProgressNupCheck();
      if (status == NUC_STATUS_ERROR)
	{   // エラー発生
	  ChangeState(PHASE_NUP_CLEANUP);
	  error_code = NUC_GetLastError();
	}
      else if (status == NUC_STATUS_COMPLETED)
	{    // 更新リスト 取得終了
	  if (TitleIdNum > 0 )
	    {   // ダウンロードへ
	      int i;
	      for (i = 0; i < TitleIdNum; i++)
		{
		  OS_TPrintf("DL list:%3d:0x%llx", i, TitleIds[i]);
		}
	      ChangeState(PHASE_NUP_DOWNLOAD);
	    }
	  else
	    {   // 更新すべきものがない
	      OS_TPrintf("No title to update\n");
	      ChangeState(PHASE_NUP_CLEANUP);
	    }
	}
      break;

    case PHASE_NUP_DOWNLOAD: // ダウンロードを開始します。
      ret = StartNupDownload();
      if (ret == FALSE)
	{   // エラー発生
	  ChangeState(PHASE_NUP_CLEANUP);
	  error_code = NUC_GetLastError();
	}
      else
	{
	  ChangeState(PHASE_NUP_PROCESS);
	}
      break;

    case PHASE_NUP_PROCESS: // ダウンロードの進行状況を表示します。
      status = ProgressNupDownload();
      if (status == NUC_STATUS_ERROR)
	{   // エラー発生
	  ChangeState(PHASE_NUP_CLEANUP);
	  error_code = NUC_GetLastError();
	}
      else if (status == NUC_STATUS_COMPLETED)
	{   // ダウンロード完了
	  ChangeState(PHASE_NUP_CLEANUP);
	}
      break;

    case PHASE_NUP_CLEANUP:  // NUPライブラリのクリーンアップ
      ret = CleanNupLib();
      if (ret == FALSE && error_code == 0)
	{   // 他でエラーが起こっていない場合のみ上書きします。
	  error_code = NUC_GetLastError();
	}
      // ネットの切断と後始末
      OS_WakeupThreadDirect(&NetThread);
      ChangeState(PHASE_CLEANING_UP);
      break;
            
    case PHASE_CLEANING_UP:  // ネットの切断終了を待ちます
      if (NetConnect == NET_CONNECT_NONE)
	{   // 切断終了
	  if (error_code == 0)
	    {    // 正常終了
	      ChangeState(PHASE_FINISHED);
	    }
	  else
	    {
	      ChangeState(PHASE_ERROR_OCCURRED);
	    }
	}
      break;
            
    case PHASE_FINISHED:   // ネットワークアップデートが正常終了しました。
      OS_TPrintf("Network Update Completed\n");
      if (TitleIdNum > 0)
	{
	  OS_TPrintf("%d file is updated\n", TitleIdNum);
	}
      else
	{
	  OS_TPrintf("Nothing is updated\n");
	}

      if (TitleIdNum > 0)
	{    // 再起動する
	  OS_TPrintf("再起動します\n");
	}
      else
	{   // メッセージを表示して戻る
	  OS_TPrintf("アップデートするものがなかったので戻ります\n");
	}
      goto end;

      break;
            
    case PHASE_ERROR_OCCURRED:  // エラーが発生しています。

      ShowErrorMsg(error_code);
      goto end;
#if 0
      if (Trg == PAD_BUTTON_A)
	{
	  OS_TPrintf("エラーが発生したので戻ります\n");
	  goto end;
	}
#endif
      break;

    default:
OS_TPrintf("%s %d\n",__FUNCTION__,__LINE__);

      break;
    }
  }
  
  end:
    NcFinish();
    NetConnect = NET_CONNECT_NONE;

    OS_TPrintf("Network Connection ended\n");
}


/* ********************** */
void nuc_main(void)
{

  ChangeState(PHASE_INIT);
  // NcGlobalInit(); in netconnect.c

  /* クライアント証明書の初期化 */
  SEA_Init();
  // あらかじめWRAMにロードしておきます
  if (!NA_LoadVersionDataArchive(WorkForNA, NA_VERSION_DATA_WORK_SIZE)) {
    OS_TPrintf("NA load error\n");
    goto end;
  }
  
  if (!NUC_LoadCert()) {
    // WRAMにロード
    OS_TPrintf("Client cert load error\n");
    goto end;
  }
  
  (void)NA_UnloadVersionDataArchive();


  /* 接続設定スレッドの作成 */
  OS_CreateThread(&NetThread, NetConnectProc,
		  NULL, NetStack + STACK_SIZE / sizeof(u64),
		  STACK_SIZE, THREAD1_PRIO);
  OS_WakeupThreadDirect(&NetThread);
  ChangeState(PHASE_CONNECTING_NETWORK);

 end:
  return;
}
