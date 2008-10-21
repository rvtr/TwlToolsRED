#ifndef	__HATAMOTO_LIB__
#define	__HATAMOTO_LIB__

#ifdef __cplusplus
extern "C" {
#endif

void PrintDeviceInfo(void);
void SetupShopTitleId(void);
void SetupUserInfo(void);
void SetupVerData(void);
void SetupNSSL(void);
BOOL SetupNHTTP(void);
BOOL SetupEC(void);
void DeleteECDirectory(void);
void SetupTitlesDataFile(const NAMTitleId* pTitleIds, u32 numTitleIds);

BOOL hatamotolib_main(u64 *title_id_buf, u32 num_title);

#ifdef __cplusplus
}
#endif

#endif  // __HATAMOTO_LIB__
