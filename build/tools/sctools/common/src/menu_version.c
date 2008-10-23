#include <twl.h>
#include <twl/na.h>
#include "menu_version.h"


#define TWL_SYSMENU_VER_STR_LEN			28	// システムメニューバージョン文字列MAX bytes
#define FS_VERSION_FILE					"verdata:/version.bin"
#define FS_TIMESTAMP_FILE				"verdata:/time_stamp.bin"


#if 0
	typedef struct SystemMenuVersion {
	  u16		major;
	  u16		minor;
	  u16		str[ TWL_SYSMENU_VER_STR_LEN / sizeof(u16) ];
	} SystemMenuVersion;
	
	SystemMenuVersion s_version;
	u32 s_timestamp;
	char str_ver[ TWL_SYSMENU_VER_STR_LEN / sizeof(u16) ];
	int len = sizeof(str_ver);
	//	STD_MemSet((void *)s_version, 0, sizeof(TWL_SYSMENU_VER_STR_LEN);
	STD_MemSet((void *)str_ver, 0, TWL_SYSMENU_VER_STR_LEN);
#endif

typedef struct SystemMenuVersion {
  u16		major;
  u16		minor;
  u16		str[ TWL_SYSMENU_VER_STR_LEN / sizeof(u16) ];
} SystemMenuVersion;

static u8 WorkForNA[NA_VERSION_DATA_WORK_SIZE];

BOOL Read_SystemMenuVersion(u16 *major, u16 *minor, u32 *ts)
{

  FSFile file;
  BOOL retval = TRUE;
  SystemMenuVersion s_version;
  u32 s_timestamp;
  char str_ver[ TWL_SYSMENU_VER_STR_LEN / sizeof(u16) ];
  int file_len;
  int len = sizeof(str_ver);

  if( major == NULL || minor == NULL || ts == NULL ) {
    return FALSE;
  }
  //	STD_MemSet((void *)s_version, 0, sizeof(TWL_SYSMENU_VER_STR_LEN);
  STD_MemSet((void *)str_ver, 0, TWL_SYSMENU_VER_STR_LEN);

  
  if (!NA_LoadVersionDataArchive(WorkForNA, NA_VERSION_DATA_WORK_SIZE)) {
    OS_TPrintf("NA load error\n");
    return FALSE;
  }

  
  FS_InitFile(&file);
  
  if (!FS_OpenFileEx(&file, FS_VERSION_FILE, FS_FILEMODE_R)) {
    retval = FALSE;
  }
  
  file_len = FS_ReadFile(&file, &s_version, (s32)sizeof(s_version));
  if (file_len < 0) {
    retval = FALSE;
    *major = 0;
    *minor = 0;
  }
  else {
    *major = s_version.major;
    *minor = s_version.minor;
  }
  FS_CloseFile(&file);


  if( STD_ConvertStringUnicodeToSjis( str_ver, &len, s_version.str, NULL, NULL ) != STD_RESULT_SUCCESS ) {
    retval = FALSE;
  }
  
  
  if (!FS_OpenFileEx(&file, FS_TIMESTAMP_FILE, FS_FILEMODE_R)) {
    retval = FALSE;
  }
  
  file_len = FS_ReadFile(&file, &s_timestamp, (s32)sizeof(s_timestamp));
  if (file_len < 0) {
    retval = FALSE;
    *ts = 0;
  }
  else {
    *ts = s_timestamp;
  }
  
  FS_CloseFile(&file);
  
  (void)NA_UnloadVersionDataArchive();

  return retval;
}

