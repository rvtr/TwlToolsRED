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

char *MyFile_GetPhotoListFileName(void)
{
  STD_StrCpy( path , path_base );
  STD_StrCat( path , MY_FILE_NAME_PHOTO_LIST );
  return path;
}

char *MyFile_GetAppDataSaveDirName(void)
{
  STD_StrCpy( path , path_base );
  STD_StrCat( path , MY_DIR_NAME_TITLE );
  return path;
}
char *MyFile_GetAppDataLogFileName(void)
{
  STD_StrCpy( path_log , path_base );
  STD_StrCat( path_log , MY_FILE_NAME_TITLE_LOG );
  return path_log;
}
char *MyFile_GetAppDataListFileName(void)
{
  STD_StrCpy( path , path_base );
  STD_StrCat( path , MY_FILE_NAME_TITLE_LIST );
  return path;
}

char *MyFile_GetDownloadTitleIDLogFileName(void)
{
  STD_StrCpy( path_log , path_base );
  STD_StrCat( path_log , MY_FILE_NAME_DOWNLOAD_TITLE_ID_LOG );
  return path_log;
}
char *MyFile_GetDownloadTitleIDFileName(void)
{
  STD_StrCpy( path , path_base );
  STD_StrCat( path , MY_FILE_NAME_DOWNLOAD_TITLE_ID_DATA );
  return path;
}

char *MyFile_GetGlobalInformationFileName(void)
{
  STD_StrCpy( path , path_base );
  STD_StrCat( path , MY_FILE_NAME_ORG_DATA );
  return path;
}


