
#include <twl.h>
#include <nitro/nvram/nvram.h>

#include        "font.h"
#include        "text.h"
#include        "mprintf.h"
#include        "mynvram.h"
#include        "my_fs_util.h"


#define NVRAM_PERSONAL_DATA_OFFSET 0x20

/* WifiÝ’è‚¾‚¯‚È‚ç0x0A00 */
//#define NVRAM_PERSONAL_DATA_SIZE 0x0C00
#define NVRAM_PERSONAL_DATA_SIZE 0x0A00


#define NVRAM_INTERNAL_BUF_SIZE 0x100
static u8 nvram_buffer[NVRAM_INTERNAL_BUF_SIZE] ATTRIBUTE_ALIGN(32);


static BOOL my_nvram_read( u32 offset, u32 size, void *buf)
{
  u32 internal_size = size;
  u32 internal_offset = offset;
  void *temp_buf = buf;
  u32 temp_size;
  int i;

  while( internal_size ) {
    if( internal_size > NVRAM_INTERNAL_BUF_SIZE ) {
      temp_size = NVRAM_INTERNAL_BUF_SIZE;
    }
    else {
      temp_size = internal_size;
    }

    DC_InvalidateRange(nvram_buffer, NVRAM_INTERNAL_BUF_SIZE);
    if( NVRAM_RESULT_SUCCESS !=  NVRAMi_Read( internal_offset , temp_size, (void* )nvram_buffer) ) {
      mprintf("nvram read error\n");
      OS_TPrintf( "nvram error: %s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
      return FALSE;
    }
    for( i = 0 ; i < temp_size ; i++ ) {
      *(u8 *)temp_buf = nvram_buffer[i];
      ((u8 *)temp_buf)++;
    }
 
    internal_offset += temp_size;
    internal_size -= temp_size;
  }
  //  OS_TPrintf( "nvram success: offset = 0x%02x\n", offset);
  return TRUE;
}

static BOOL my_nvram_write( u32 offset, u32 size, void *buf)
{
  u32 internal_size = size;
  u32 internal_offset = offset;
  void *temp_buf = buf;
  u32 temp_size;
  int i;

  while( internal_size ) {

    if( internal_size > NVRAM_INTERNAL_BUF_SIZE ) {
      temp_size = NVRAM_INTERNAL_BUF_SIZE;
    }
    else {
      temp_size = internal_size;
    }

    for( i = 0 ; i < temp_size ; i++ ) {
      nvram_buffer[i] = *(u8 *)temp_buf;
      ((u8 *)temp_buf)++;
    }

    DC_FlushRange(nvram_buffer, NVRAM_INTERNAL_BUF_SIZE);
    if( NVRAM_RESULT_SUCCESS !=  NVRAMi_Write( internal_offset , temp_size, (void* )nvram_buffer) ) {
      OS_TPrintf( "nvram write error: %s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
      return FALSE;
    }

    internal_offset += temp_size;
    internal_size -= temp_size;
  }
  //  OS_TPrintf( "nvram success: offset = 0x%02x\n", offset);
  return TRUE;
}


/* *INDENT-OFF* */
static const char  * const fs_result_strings[] =
{
    "FS_RESULT_SUCCESS",
    "FS_RESULT_FAILURE",
    "FS_RESULT_BUSY",
    "FS_RESULT_CANCELED",
    "FS_RESULT_UNSUPPORTED",
    "FS_RESULT_ERROR",
    "FS_RESULT_INVALID_PARAMETER",
    "FS_RESULT_NO_MORE_RESOUCE",
    "FS_RESULT_ALREADY_DONE",
    "FS_RESULT_PERMISSION_DENIED",
    "FS_RESULT_MEDIA_FATAL",
    "FS_RESULT_NO_ENTRY",
    "FS_RESULT_MEDIA_NOTHING",
    "FS_RESULT_MEDIA_UNKNOWN",
    "FS_RESULT_BAD_FORMAT"
};








static const size_t fs_result_string_max = sizeof(fs_result_strings) / sizeof(*fs_result_strings);

static void ReportLastErrorPath(const char *path)
{
    FSResult    result = FS_GetArchiveResultCode(path);
    if( (result >= 0) && (result < fs_result_string_max)) {
      OS_TPrintf("FS error:    \"%s\"    %s\n", path, fs_result_strings[result]);
      mprintf("FS error:    \"%s\"    %s\n",  path, fs_result_strings[result]);
    }
    else {
      OS_TPrintf("FS error: unknown\n");
      mprintf("FS error:\n  unknown\n");
    }

}

BOOL nvram_backup(char *path)
{
  BOOL bSuccess;
  FSFile nor_fd;
  u32 offset;
  u32 vol;
  int len;
#define BUF_SIZE 0x100
  u8 nor_buf[BUF_SIZE];
  char *nor_file_path = path;

  FS_InitFile(&nor_fd);

  //  STD_TSNPrintf(nor_file_path, sizeof(nor_file_path), path );
  bSuccess = FS_OpenFileEx(&nor_fd, nor_file_path, (FS_FILEMODE_R|FS_FILEMODE_W));
  if( ! bSuccess )
    {
      if( !FS_CreateFileAuto(nor_file_path, FS_PERMIT_R | FS_PERMIT_W)) {
        ReportLastErrorPath(nor_file_path);
        OS_TPrintf("2 FS_CreateFileAuto(%s) failed.", nor_file_path);
	return FALSE;
      }
      bSuccess = FS_OpenFileEx(&nor_fd, nor_file_path, (FS_FILEMODE_R|FS_FILEMODE_W));
      if( ! bSuccess ) {
	OS_TPrintf("error %s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
	return FALSE;
      }
    }
  if( TRUE !=  my_nvram_read( NVRAM_PERSONAL_DATA_OFFSET , sizeof(u16), (void* )&offset) ) {
    OS_TPrintf( "nvram error: %s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
  }
  else {
    OS_TPrintf( "nvram success: offset = 0x%02x\n", offset);
  }

  if( offset == 0 ) {
    OS_TPrintf( "nvram error: offset = 0x%02x\n", offset);
    return FALSE;
  }

  offset *= 8;
  offset -= 0xA00;

  OS_TPrintf( "nvram %d: offset = 0x%02x\n",__LINE__, offset);

  for( vol = 0 ; vol < NVRAM_PERSONAL_DATA_SIZE ; vol += BUF_SIZE ) {
    if( TRUE !=  my_nvram_read( offset+vol , BUF_SIZE, (void* )nor_buf) ) {
      OS_TPrintf( "nvram error: %s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
    }
    else {
      OS_TPrintf( " --: offset = 0x%02x\n", offset+vol);
#if 0
      len = FS_WriteFile(&nor_fd, nor_buf, BUF_SIZE);
#else
      len = my_fs_crypto_write(&nor_fd, nor_buf, BUF_SIZE);
#endif
      if (len != BUF_SIZE)
	{
	  OS_TPrintf("FS_WriteFile() failed.");
	  break;
	}
    }
  }
  OS_TPrintf("\n");

  FS_FlushFile(&nor_fd);

  bSuccess = FS_CloseFile(&nor_fd);

  //  OS_TPrintf( "nvram read completed.\n");
  return TRUE;
}

BOOL nvram_restore(char *path)
{
  BOOL bSuccess;
  FSFile nor_fd;
  u32 offset;
  u32 vol;
  int len;
  //  char nor_file_path[FS_FILE_NAME_MAX];
  char *nor_file_path = path;

#define BUF_SIZE 0x100
  u8 nor_buf[BUF_SIZE];

  FS_InitFile(&nor_fd);
  //  STD_TSNPrintf(nor_file_path, sizeof(nor_file_path), path );
  bSuccess = FS_OpenFileEx(&nor_fd, nor_file_path, FS_FILEMODE_R);
  if( ! bSuccess ) {
    OS_TPrintf("error %s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
    return FALSE;
  }

  /* offsetƒAƒhƒŒƒX‚ÌŽæ“¾ */
  if( TRUE !=  my_nvram_read( NVRAM_PERSONAL_DATA_OFFSET , sizeof(u16), (void* )&offset) ) {
    OS_TPrintf( "nvram error: %s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
  }
  else {
    OS_TPrintf( "nvram success: offset = 0x%02x\n", offset);
  }
 
  if( offset == 0 ) {
    OS_TPrintf( "nvram error: offset = 0x%02x\n", offset);
    return FALSE;
  }

  offset *= 8;
  offset -= 0xA00;
  
  for( vol = 0 ; vol < NVRAM_PERSONAL_DATA_SIZE ; vol += BUF_SIZE ) {
    OS_TPrintf(".");

#if 0
    len = FS_ReadFile(&nor_fd, nor_buf, BUF_SIZE);
#else
    len = my_fs_crypto_read(&nor_fd, nor_buf, BUF_SIZE);
#endif
    if (len != BUF_SIZE) {
      OS_TPrintf("FS_ReadFile() failed.");
      break;
    }

    if( TRUE !=  my_nvram_write( offset+vol , BUF_SIZE, (void* )nor_buf) ) {
      OS_TPrintf( "nvram write error: %s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
    }
    else {
    }
  }

  OS_TPrintf("\n");
  bSuccess = FS_CloseFile(&nor_fd);

  OS_TPrintf( "nvram write completed.\n");

  return TRUE;
}

