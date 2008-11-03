#ifndef	__HATAMOTO_LIB__
#define	__HATAMOTO_LIB__

#ifdef __cplusplus
extern "C" {
#endif

void PrintDeviceInfo(void);
void SetupShopTitleId(void);
void SetupUserInfo(void);
BOOL SetupVerData(void);
void SetupNSSL(void);
BOOL SetupNHTTP(void);
BOOL SetupEC(void);
void DeleteECDirectory(void);
void SetupTitlesDataFile(const NAMTitleId* pTitleIds, u32 numTitleIds);
FSFile *hatamotolib_log_start(char *log_file_name );
void hatamotolib_log_end(void);


#ifdef __cplusplus
}
#endif

#endif  // __HATAMOTO_LIB__
