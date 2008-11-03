#ifndef	_MY_NUC_H_
#define	_MY_NUC_H_

#ifdef __cplusplus
extern "C" {
#endif

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


BOOL InitNupLib(void);
BOOL StartNupCheck(void);
NucStatus ProgressNupCheck(void);
BOOL StartNupDownload(void);
NucStatus ProgressNupDownload(void);
BOOL CleanNupLib(void);
void ProgressNetConnect(void);
void ShowErrorMsg(int error_code);
BOOL my_numc_proc(void);
FSFile *my_nuc_log_start(char *log_file_name );
void my_nuc_log_end(void);


#ifdef __cplusplus
}
#endif

#endif /* _MY_NUC_H_ */
