#ifndef _NUC_H_
#define _NUC_H_

#ifdef	__cplusplus
extern "C" {
#endif

void nuc_main(void);

// Network Update Client API
#include <twl/types.h>

/*---------------------------------------------------------------------------*
    定数
 *---------------------------------------------------------------------------*/
#define NUC_MAX_TITLE_UPDATE_COUNT 128

/*---------------------------------------------------------------------------*
    型
 *---------------------------------------------------------------------------*/
typedef OSTitleId NUCTitleId;
typedef void* (*NUCAlloc)(u32 size, int align);
typedef void  (*NUCFree)(void* ptr);

typedef enum{
    NUC_STATUS_NOT_DONE, // 処理実行中
    NUC_STATUS_COMPLETED,// 処理完了
    NUC_STATUS_ERROR     // 処理エラー
}   NucStatus;

typedef enum{
    NUC_ERROR_NO_SPACE,  // TWL本体保存メモリの空き容量が不足しています。
    NUC_ERROR_CONNECT,   // サーバーに接続できません。
    NUC_ERROR_INTERNET,  // インターネットのエラーにより、TWL本体の更新ができません。
    NUC_ERROR_UPDATE     // エラーが発生しましたため、TWL本体の更新ができません。
}   NucError;

/*---------------------------------------------------------------------------*
    関数
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
  Name:         NUC_LoadCert

  Description:  クライアント証明書をWRAMにロードします。
                NA_LoadVersionDataArchive()をあらかじめ呼んでください。
                終了後はNA_UnloadVersionDataArchive()を呼んでください

  Arguments:    なし
     
  Returns:      成功したらTRUE
 *---------------------------------------------------------------------------*/
BOOL NUC_LoadCert(void);

/*---------------------------------------------------------------------------*
  Name:         NUC_Init

  Description:  NUC ライブラリの初期化を行います。

  Arguments:    allocFunc:  メモリ確保関数へのポインタ。
                freeFunc:   メモリ解放関数へのポインタ。

  Returns:      成功したらTRUE
 *---------------------------------------------------------------------------*/
BOOL NUC_Init(NUCAlloc allocFunc, NUCFree freeFunc);

/*---------------------------------------------------------------------------*
  Name:         NUC_CheckAsync

  Description:  アップデートするtitleIDの取得

  Arguments:    titleIds:     更新のあったtitleIdを取得するバッファ
                nTitleIds:    更新のあったtitleIdの数を取得するポインタ

  Returns:      成功したらTRUE
 *---------------------------------------------------------------------------*/
BOOL NUC_CheckAsync(NUCTitleId *titleIds, u32 *nTitleIds);

/*---------------------------------------------------------------------------*
  Name:         NUC_DownloadAsync

  Description:  該当titleIDの更新のダウンロード
                各パラメータがNULLの場合は全てダウンロードします。
                ※注意：この関数を呼んだ後にはFS関数を呼ぶことはできなくなります。
     
  Arguments:    titleIds:   ダウンロードするtitleIDの入ったポインタ
                nTitleIds:  ダウンロードするtitleIDの個数

  Returns:      成功したらTRUE
 *---------------------------------------------------------------------------*/
BOOL NUC_DownloadAsync(NUCTitleId *titleIds, u32 nTitleIds);

/*---------------------------------------------------------------------------*
  Name:         NUC_GetProgress

  Description:  下記処理の進行情報を取得します。
                ・更新情報の取得
                ・更新されたタイトルのダウンロード

  Arguments:    pCurrentSize:   処理済量を格納するバッファへのポインタ。
                pTotalSize:     全体量を格納するバッファへのポインタ。
                pStatus:        現在の進行状況

  Returns:      なし
 *---------------------------------------------------------------------------*/
void NUC_GetProgress(u64* pCurrentSize, u64* pTotalSize, NucStatus* pStatus);

/*---------------------------------------------------------------------------*
  Name:         NUC_Cleanup

  Description:  NAM、NHTTP、NSSL、NUP、ESの終了処理をします。
                終了後に各システムアプリのセーブデータを作成、修復します。

  Arguments:    titleIds:     更新のあったtitleIdの配列
                nTitleIds:    更新のあったtitleIdの数

  Returns:      成功したらTRUE
 *---------------------------------------------------------------------------*/
BOOL NUC_Cleanup(const NUCTitleId *titleIds, u32 nTitleIds);

/*---------------------------------------------------------------------------*
  Name:         NUC_GetLastError

  Description:  最後に発生したエラーを取得します。
                ここで返されたエラーコードをユーザに通知します。

  Arguments:    なし。

  Returns:      エラーコード
 *---------------------------------------------------------------------------*/
int NUC_GetLastError(void);

/*---------------------------------------------------------------------------*
  Name:         NUC_GetErrorType

  Description:  エラーコードから、エラータイプを取得します。
                ユーザーにはエラータイプを判別して
                表示するメッセージを設定してください。

  Arguments:    error_code:  エラーコード

  Returns:      エラーのタイプ
 *---------------------------------------------------------------------------*/
NucError NUC_GetErrorType(int error_code);

/*---------------------------------------------------------------------------*
  Name:         NUC_IsResetNeeded

  Description:  ハードウェアリセットが必要かどうかを返します。

  Arguments:    なし。

  Returns:      エラーコード
 *---------------------------------------------------------------------------*/
BOOL NUC_IsResetNeeded(void);

#ifdef	__cplusplus
}          /* extern "C" */
#endif

#endif  /* _NUC_H_ */
