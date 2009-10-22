#ifndef	__HATAMOTO_LIB__
#define	__HATAMOTO_LIB__

#ifdef __cplusplus
extern "C" {
#endif


#define ECDOWNLOAD_DUMMY       0
#define ECDOWNLOAD_SUCCESS     1
  // #define ECDOWNLOAD_NO_REGISTER 2
#define ECDOWNLOAD_FAILURE     3

// int ECDownload(const NAMTitleId* pTitleIds, u32 numTitleIds);
int ECDownload(MY_USER_APP_TID * pTitleIds, u32 numTitleIds);
int ECDownload_Auto(MY_USER_APP_TID *pTitleIds, u32 numTitleIds);

BOOL KPSClient();
  // BOOL WaitEC(ECOpId opId);


void PrintDeviceInfo(void);
void SetupShopTitleId(void);
void SetupUserInfo(void);
BOOL SetupVerData(void);
void SetupNSSL(void);
BOOL SetupNHTTP(void);
BOOL SetupEC(void);
void DeleteECDirectory(void);
BOOL SetupEC_pre(void);
// void SetupTitlesDataFile(const NAMTitleId* pTitleIds, u32 numTitleIds);
void SetupTitlesDataFile(const MY_USER_APP_TID * pTitleIds, u32 numTitleIds);

FSFile *hatamotolib_log_start(char *log_file_name );
void hatamotolib_log_end(void);
BOOL Tid_To_GameCode(u64 tid, char *gcode);

#ifdef __cplusplus
}
#endif

#endif  // __HATAMOTO_LIB__
