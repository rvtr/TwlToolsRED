#ifndef	_MY_WLAN_H_
#define	_MY_WLAN_H_


#ifdef __cplusplus
extern "C" {
#endif


BOOL LoadWlanConfigFile(char *path);

char *GetWlanSSID(void);
int GetWlanKEYBIN(u8 *buf);
char *GetWlanKEYSTR(void);
int GetWlanMode(void);

#ifdef __cplusplus
}
#endif


#endif /* _MY_WLAN_H_ */
