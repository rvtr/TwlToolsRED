#ifndef	_MY_FILE_NAME_H_
#define	_MY_FILE_NAME_H_


#define MY_FILE_NAME_UNIQUE_ID ("twl_unique.dat")
#define MY_FILE_NAME_WIFI_PARAM  ("twl_wifi.bin")
#define MY_FILE_NAME_USER_SETTINGS ("twl_user.dat")

#define MY_DIR_NAME_APP_SHARED ("shared2")
#define MY_FILE_NAME_APP_SHARED_LOG ("shared2.txt")
#define MY_FILE_NAME_APP_SHARED_LIST  ("shared2.lst")
#define MY_FILE_NAME_APP_SHARED_RESTORE_LOG ("shared2_rst.txt")


#define MY_DIR_NAME_PHOTO ("photo")
#define MY_FILE_NAME_PHOTO_LOG ("photo.txt")
#define MY_FILE_NAME_PHOTO_LIST ("photo.lst")
#define MY_FILE_NAME_PHOTO_RESTORE_LOG ("photo_rst.txt")

#define MY_DIR_NAME_TITLE ("title")
#define MY_FILE_NAME_TITLE_LOG ("title.txt")
#define MY_FILE_NAME_TITLE_LIST ("title.lst")
#define MY_FILE_NAME_TITLE_RESTORE_LOG ("title_rst.txt")

#define MY_DIR_NAME_DOWNLOAD_TITLE_ID ("title")
#define MY_FILE_NAME_DOWNLOAD_TITLE_ID_DATA ("title_id.dat")
#define MY_FILE_NAME_DOWNLOAD_TITLE_ID_LOG ("title_id.txt" )
#define MY_FILE_NAME_DOWNLOAD_TITLE_ID_RESTORE_LOG ("title_id_rst.txt" )

#define MY_FILE_NAME_ORG_DATA ("personal.dat")
#define MY_FILE_NAME_ORG_LOG ("personal.txt")
#define MY_FILE_NAME_ORG_RESTORE_LOG ("personal_rst.txt")


#ifdef __cplusplus
extern "C" {
#endif

void MyFile_SetPathBase(const char *str);
void MyFile_AddPathBase(const char *str);

char *MyFile_GetUniqueIDFileName(void);
char *MyFile_GetWifiParamFileName(void);
char *MyFile_GetUserSettingsFileName(void);

char *MyFile_GetAppSharedSaveDirName(void);
char *MyFile_GetAppSharedLogFileName(void);
char *MyFile_GetAppSharedListFileName(void);
char *MyFile_GetAppSharedRestoreLogFileName(void);

char *MyFile_GetPhotoListFileName(void);
char *MyFile_GetPhotoSaveDirName(void);
char *MyFile_GetPhotoLogFileName(void);
char *MyFile_GetPhotoRestoreLogFileName(void);

char *MyFile_GetAppDataSaveDirName(void);
char *MyFile_GetAppDataLogFileName(void);
char *MyFile_GetAppDataListFileName(void);
char *MyFile_GetAppDataRestoreLogFileName(void);

char *MyFile_GetDownloadTitleIDLogFileName(void);
char *MyFile_GetDownloadTitleIDFileName(void);
char *MyFile_GetDownloadTitleIDRestoreLogFileName(void);

char *MyFile_GetGlobalInformationFileName(void);
char *MyFile_GetGlobalInformationRestoreFileName(void);
char *MyFile_GetGlobalInformationLogFileName(void);


#ifdef __cplusplus
}
#endif


#endif /* _MY_FILE_NAME_H_ */
