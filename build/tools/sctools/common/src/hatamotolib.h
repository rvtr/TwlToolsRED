#ifndef	__HATAMOTO_LIB__
#define	__HATAMOTO_LIB__

#ifdef __cplusplus
extern "C" {
#endif

void PrintDeviceInfo(void);
void SetupShopTitleId(void);
void SetupUserInfo(void);
void SetupVerData(void);
void NetworkAutoConnect_DWC(void);
void NetworkShutdown_DWC(void);
void SetupNSSL(void);
BOOL SetupNHTTP(void);
void SetupEC(void);
BOOL WaitEC(ECOpId opId);
void DeleteECDirectory(void);
void SetupTitlesDataFile(const NAMTitleId* pTitleIds, u32 numTitleIds);

BOOL hatamotolib_main(void);


#ifdef __cplusplus
}
#endif

#endif  // __HATAMOTO_LIB__
