#ifndef	_MY_WLAN_H_
#define	_MY_WLAN_H_


#ifdef __cplusplus
extern "C" {
#endif

#define MAX_KEY_BIN_BUF 80


BOOL LoadWlanConfigFile(char *path);

char *GetWlanSSID(void);
int GetWlanKEYBIN(u8 *buf);
char *GetWlanKEYSTR(void);
int GetWlanMode(void);

u32 GetGateway(void);
u32 GetNetmask(void);
u32 GetIPAddr(void);
BOOL GetDhcpMODE(void);
u32 GetDNS1(void);
u32 GetDNS2(void);

BOOL GetKeyModeStr(void);

#ifdef __cplusplus
}
#endif


#endif /* _MY_WLAN_H_ */
