#include <twl.h>
#include        "myfilename.h"

static  char path_base[256];
static  char path_log[256];
static  char path[256];

void MyFile_SetPathBase(const char *str)
{
  STD_StrCpy( path_base , str );
}

void MyFile_AddPathBase(const char *str)
{
  STD_StrCat( path_base , str );
}

char *MyFile_GetProductLogFileName(void)
{
  STD_StrCpy( path , path_base );
  STD_StrCat( path , MY_FILE_NAME_PRODUCT_LOG );
  return path;
}

char *MyFile_GetSystemMenuLogFileName(void)
{
  STD_StrCpy( path , path_base );
  STD_StrCat( path , MY_FILE_NAME_SHOP_LOG );
  return path;
}

char *MyFile_GetShopLogFileName(void)
{
  STD_StrCpy( path , path_base );
  STD_StrCat( path , MY_FILE_NAME_SYSMENU_LOG );
  return path;
}


char *MyFile_GetUniqueIDFileName(void)
{
  STD_StrCpy( path , path_base );
  STD_StrCat( path , MY_FILE_NAME_UNIQUE_ID );
  return path;
}

char *MyFile_GetWifiParamFileName(void)
{
  STD_StrCpy( path , path_base );
  STD_StrCat( path , MY_FILE_NAME_WIFI_PARAM );
  return path;
}

char *MyFile_GetUserSettingsFileName(void)
{
  STD_StrCpy( path , path_base );
  STD_StrCat( path , MY_FILE_NAME_USER_SETTINGS );
  return path;
}


char *MyFile_GetUserAppTitleListLogFileName(void)
{
  STD_StrCpy( path , path_base );
  STD_StrCat( path , MY_FILE_NAME_USER_APP_TITLE_LIST_LOG );
  return path;
}

char *MyFile_GetNupLogFileName(void)
{
  STD_StrCpy( path , "sdmc:/" );
  STD_StrCat( path , MY_FILE_NAME_NUP_LOG);
  return path;
}

char *MyFile_GetEcDownloadLogFileName(void)
{
  STD_StrCpy( path , path_base );
  STD_StrCat( path , MY_FILE_NAME_EC_DOWNLOAD_LOG );
  return path;
}



char *MyFile_GetAppSharedSaveDirName(void)
{
  STD_StrCpy( path , path_base );
  STD_StrCat( path , MY_DIR_NAME_APP_SHARED );
  return path;
}

char *MyFile_GetAppSharedLogFileName(void)
{
  STD_StrCpy( path_log , path_base );
  STD_StrCat( path_log , MY_FILE_NAME_APP_SHARED_LOG );  
  return path_log;
}

char *MyFile_GetAppSharedRestoreLogFileName(void)
{
  STD_StrCpy( path_log , path_base );
  STD_StrCat( path_log , MY_FILE_NAME_APP_SHARED_RESTORE_LOG );  
  return path_log;
}

char *MyFile_GetAppSharedSaveLogFileName(void)
{
  STD_StrCpy( path_log , path_base );
  STD_StrCat( path_log , MY_FILE_NAME_APP_SHARED_SAVE_LOG );  
  return path_log;
}


char *MyFile_GetAppSharedListFileName(void)
{
  STD_StrCpy( path , path_base );
  STD_StrCat( path , MY_FILE_NAME_APP_SHARED_LIST );
  return path;
}

char *MyFile_GetPhotoSaveDirName(void)
{
  STD_StrCpy( path , path_base );
  STD_StrCat( path , MY_DIR_NAME_PHOTO );
  return path;
}

char *MyFile_GetPhotoLogFileName(void)
{
  STD_StrCpy( path_log , path_base );
  STD_StrCat( path_log , MY_FILE_NAME_PHOTO_LOG );
  return path_log;
}

char *MyFile_GetPhotoRestoreLogFileName(void)
{
  STD_StrCpy( path_log , path_base );
  STD_StrCat( path_log , MY_FILE_NAME_PHOTO_RESTORE_LOG );
  return path_log;
}

char *MyFile_GetPhotoSaveLogFileName(void)
{
  STD_StrCpy( path_log , path_base );
  STD_StrCat( path_log , MY_FILE_NAME_PHOTO_SAVE_LOG );
  return path_log;
}

char *MyFile_GetPhotoListFileName(void)
{
  STD_StrCpy( path , path_base );
  STD_StrCat( path , MY_FILE_NAME_PHOTO_LIST );
  return path;
}

char *MyFile_GetSaveDataSaveDirName(void)
{
  STD_StrCpy( path , path_base );
  STD_StrCat( path , MY_DIR_NAME_SAVE_DATA );
  return path;
}

char *MyFile_GetSaveDataLogFileName(void)
{
  STD_StrCpy( path_log , path_base );
  STD_StrCat( path_log , MY_FILE_NAME_SAVE_DATA_LOG );
  return path_log;
}

char *MyFile_GetSaveDataRestoreLogFileName(void)
{
  STD_StrCpy( path_log , path_base );
  STD_StrCat( path_log , MY_FILE_NAME_SAVE_DATA_RESTORE_LOG );
  return path_log;
}

char *MyFile_GetSaveDataSaveLogFileName(void)
{
  STD_StrCpy( path_log , path_base );
  STD_StrCat( path_log , MY_FILE_NAME_SAVE_DATA_SAVE_LOG );
  return path_log;
}

char *MyFile_GetSaveDataListFileName(void)
{
  STD_StrCpy( path , path_base );
  STD_StrCat( path , MY_FILE_NAME_SAVE_DATA_LIST );
  return path;
}


char *MyFile_GetSaveDataListLogFileName(void)
{
  STD_StrCpy( path , path_base );
  STD_StrCat( path , MY_FILE_NAME_SAVE_DATA_LIST_LOG );
  return path;
}

char *MyFile_GetDownloadTitleIDLogFileName(void)
{
  STD_StrCpy( path_log , path_base );
  STD_StrCat( path_log , MY_FILE_NAME_DOWNLOAD_TITLE_ID_LOG );
  return path_log;
}

char *MyFile_GetDownloadTitleIDRestoreLogFileName(void)
{
  STD_StrCpy( path_log , path_base );
  STD_StrCat( path_log , MY_FILE_NAME_DOWNLOAD_TITLE_ID_RESTORE_LOG );
  return path_log;
}

char *MyFile_GetDownloadTitleIDSaveLogFileName(void)
{
  STD_StrCpy( path_log , path_base );
  STD_StrCat( path_log , MY_FILE_NAME_DOWNLOAD_TITLE_ID_SAVE_LOG );
  return path_log;
}

char *MyFile_GetDownloadTitleIDFileName(void)
{
  STD_StrCpy( path , path_base );
  STD_StrCat( path , MY_FILE_NAME_DOWNLOAD_TITLE_ID_DATA );
  return path;
}

char *MyFile_GetDownloadTitleIDTicketOnlyFileName(void)
{
  STD_StrCpy( path , path_base );
  STD_StrCat( path , MY_FILE_NAME_DOWNLOAD_TITLE_ID_TICKET_DATA );
  return path;
}

char *MyFile_GetDownloadTitleIDTicketOnlyRestoreLogFileName(void)
{
  STD_StrCpy( path_log , path_base );
  STD_StrCat( path_log , MY_FILE_NAME_DOWNLOAD_TITLE_ID_TICKET_RESTORE_LOG );
  return path_log;
}

char *MyFile_GetDownloadTitleIDTicketOnlySaveLogFileName(void)
{
  STD_StrCpy( path_log , path_base );
  STD_StrCat( path_log , MY_FILE_NAME_DOWNLOAD_TITLE_ID_TICKET_SAVE_LOG );
  return path_log;
}


char *MyFile_GetGlobalInformationFileName(void)
{
  STD_StrCpy( path , path_base );
  STD_StrCat( path , MY_FILE_NAME_ORG_DATA );
  return path;
}

char *MyFile_GetGlobalInformationLogFileName(void)
{
  STD_StrCpy( path , path_base );
  STD_StrCat( path , MY_FILE_NAME_ORG_LOG );
  return path;
}

char *MyFile_GetGlobalInformationRestoreFileName(void)
{
  STD_StrCpy( path , path_base );
  STD_StrCat( path , MY_FILE_NAME_ORG_RESTORE_LOG );
  return path;
}

char *MyFile_GetGlobalInformationSaveFileName(void)
{
  STD_StrCpy( path , path_base );
  STD_StrCat( path , MY_FILE_NAME_ORG_SAVE_LOG );
  return path;
}
