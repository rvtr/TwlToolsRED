#ifndef	_MY_FILE_NAME_H_
#define	_MY_FILE_NAME_H_



#define MY_FILE_NAME_UNIQUE_ID ("twl_unique.dat")
#define MY_FILE_NAME_WIFI_PARAM  ("twl_wifi.bin")
#define MY_FILE_NAME_USER_SETTINGS ("twl_user.dat")

#define MY_FILE_NAME_USER_APP_TITLE_LIST_LOG ("user_app_lst.txt")
#define MY_FILE_NAME_NUP_LOG                 ("nup_log.txt")
#define MY_FILE_NAME_EC_DOWNLOAD_LOG         ("ecdown.txt")

#define MY_DIR_NAME_APP_SHARED ("shared2")
#define MY_FILE_NAME_APP_SHARED_LOG ("shared2.txt")
#define MY_FILE_NAME_APP_SHARED_LIST  ("shared2.lst")
#define MY_FILE_NAME_APP_SHARED_RESTORE_LOG ("shared2_rst.txt")
#define MY_FILE_NAME_APP_SHARED_SAVE_LOG ("shared2_sv.txt")


#define MY_DIR_NAME_PHOTO ("photo")
#define MY_FILE_NAME_PHOTO_LOG ("photo.txt")
#define MY_FILE_NAME_PHOTO_LIST ("photo.lst")
#define MY_FILE_NAME_PHOTO_RESTORE_LOG ("photo_rst.txt")
#define MY_FILE_NAME_PHOTO_SAVE_LOG ("photo_sv.txt")

#define MY_DIR_NAME_SAVE_DATA ("save")

#define MY_FILE_NAME_SAVE_DATA_LIST ("save.lst")
#define MY_FILE_NAME_SAVE_DATA_LOG ("save.txt")
#define MY_FILE_NAME_SAVE_DATA_LIST_LOG ("save_lst.txt")
#define MY_FILE_NAME_SAVE_DATA_RESTORE_LOG ("save_rst.txt")
#define MY_FILE_NAME_SAVE_DATA_SAVE_LOG ("save_sv.txt")


#define MY_DIR_NAME_DOWNLOAD_TITLE_ID ("title_id")
#define MY_FILE_NAME_DOWNLOAD_TITLE_ID_DATA ("title_id.dat")
#define MY_FILE_NAME_DOWNLOAD_TITLE_ID_LOG ("title_id.txt" )
#define MY_FILE_NAME_DOWNLOAD_TITLE_ID_RESTORE_LOG ("title_id_rst.txt" )
#define MY_FILE_NAME_DOWNLOAD_TITLE_ID_SAVE_LOG ("title_id_sv.txt" )

#define MY_FILE_NAME_DOWNLOAD_TITLE_ID_TICKET_DATA ("title_tk.dat")
#define MY_FILE_NAME_DOWNLOAD_TITLE_ID_TICKET_LOG ("title_tk.txt" )
#define MY_FILE_NAME_DOWNLOAD_TITLE_ID_TICKET_RESTORE_LOG ("title_tk_rst.txt" )
#define MY_FILE_NAME_DOWNLOAD_TITLE_ID_TICKET_SAVE_LOG ("title_id_sv.txt" )

#define MY_FILE_NAME_ORG_DATA ("personal.dat")
#define MY_FILE_NAME_ORG_LOG ("personal.txt")
#define MY_FILE_NAME_ORG_RESTORE_LOG ("personal_rst.txt")
#define MY_FILE_NAME_ORG_SAVE_LOG ("personal_sv.txt")


#define MY_FILE_NAME_PRODUCT_LOG "product.log"
#define MY_FILE_NAME_SHOP_LOG    "sysmenu.log"
#define MY_FILE_NAME_SYSMENU_LOG "shop.log"


#ifdef __cplusplus
extern "C" {
#endif

void MyFile_SetPathBase(const char *str);
void MyFile_AddPathBase(const char *str);


char *MyFile_GetProductLogFileName(void);
char *MyFile_GetSystemMenuLogFileName(void);
char *MyFile_GetShopLogFileName(void);

char *MyFile_GetUniqueIDFileName(void);
char *MyFile_GetWifiParamFileName(void);
char *MyFile_GetUserSettingsFileName(void);

char *MyFile_GetUserAppTitleListLogFileName(void);
char *MyFile_GetEcDownloadLogFileName(void);
char *MyFile_GetNupLogFileName(void);

char *MyFile_GetAppSharedSaveDirName(void);
char *MyFile_GetAppSharedLogFileName(void);
char *MyFile_GetAppSharedListFileName(void);
char *MyFile_GetAppSharedRestoreLogFileName(void);
char *MyFile_GetAppSharedSaveLogFileName(void);

char *MyFile_GetPhotoListFileName(void);
char *MyFile_GetPhotoSaveDirName(void);
char *MyFile_GetPhotoLogFileName(void);
char *MyFile_GetPhotoRestoreLogFileName(void);
char *MyFile_GetPhotoSaveLogFileName(void);

char *MyFile_GetSaveDataSaveDirName(void);
char *MyFile_GetSaveDataLogFileName(void);
char *MyFile_GetSaveDataListFileName(void);
char *MyFile_GetSaveDataListLogFileName(void);
char *MyFile_GetSaveDataRestoreLogFileName(void);
char *MyFile_GetSaveDataSaveLogFileName(void);

char *MyFile_GetUserTitleIDLogFileName(void);
char *MyFile_GetUserTitleIDFileName(void);
char *MyFile_GetUserTitleIDRestoreLogFileName(void);
char *MyFile_GetUserTitleIDSaveLogFileName(void);

char *MyFile_GetUserTitleIDTicketOnlyFileName(void);
char *MyFile_GetUserTitleIDTicketOnlySaveLogFileName(void);
char *MyFile_GetUserTitleIDTicketOnlyRestoreLogFileName(void);

char *MyFile_GetGlobalInformationFileName(void);
char *MyFile_GetGlobalInformationRestoreFileName(void);
char *MyFile_GetGlobalInformationLogFileName(void);
char *MyFile_GetGlobalInformationSaveFileName(void);

#ifdef __cplusplus
}
#endif


#endif /* _MY_FILE_NAME_H_ */
