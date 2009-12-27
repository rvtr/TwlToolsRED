
#include <twl.h>
#include <nitro/nvram/nvram.h>
#include <nitroWiFi/ncfg.h>

#include        "font.h"
#include        "text.h"
#include        "mprintf.h"
#include        "mynvram.h"
#include        "my_fs_util.h"


#define NVRAM_PERSONAL_DATA_OFFSET 0x20

/* Wifi�ݒ肾���Ȃ�0x0A00 */
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


static u8 my_nor_buf[NVRAM_PERSONAL_DATA_SIZE] ATTRIBUTE_ALIGN(32);
static u8 work_content[NCFG_CHECKCONFIGEX_WORK_SIZE];


BOOL nvram_get(u8 buf[0xa00])
{
  BOOL ret_flag = TRUE;
  u32 offset;

  if( TRUE !=  my_nvram_read( NVRAM_PERSONAL_DATA_OFFSET , sizeof(u16), (void* )&offset) ) {
    OS_TPrintf( "nvram error: %s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
  }

  if( offset == 0 ) {
    OS_TPrintf( "nvram error: offset = 0x%02x\n", offset);
    return FALSE;
  }

  offset *= 8;
  offset -= 0xA00;

  if( TRUE !=  my_nvram_read( offset , NVRAM_PERSONAL_DATA_SIZE, (void* )buf) ) {
    OS_TPrintf( "nvram error: %s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
    ret_flag = FALSE;
  }
  return ret_flag;
}

BOOL nvram_set(u8 buf[0xa00])
{
  BOOL ret_flag = TRUE;
  u32 offset;

  if( TRUE !=  my_nvram_read( NVRAM_PERSONAL_DATA_OFFSET , sizeof(u16), (void* )&offset) ) {
    OS_TPrintf( "nvram error: %s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
    ret_flag = FALSE;
  }
  else {
    OS_TPrintf( "nvram success: offset = 0x%02x\n", offset);
  }
 
  if( offset == 0 ) {
    OS_TPrintf( "nvram error: offset = 0x%02x\n", offset);
    return FALSE;
  }

  /* offset�̃`�F�b�N�́H */
  offset *= 8;
  offset -= 0xA00;
  if( TRUE !=  my_nvram_write( offset , /* size */ NVRAM_PERSONAL_DATA_SIZE, (void* )buf) ) {
    ret_flag = FALSE;
    OS_TPrintf( "nvram write error: %s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
  }
  return ret_flag;
}


BOOL nvram_backup(char *path)
{
  BOOL bSuccess;
  BOOL ret_flag = TRUE;
  FSFile nor_fd;
  u32 offset;
  int len;

  char *nor_file_path = path;

  void *work = (void *)work_content;

  MI_CpuClear8(work, NCFG_CHECKCONFIGEX_WORK_SIZE);
  if( NCFG_RESULT_INIT_OK != NCFG_CheckConfigEx( work ) ) {
    return FALSE;
  }


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

  if( offset == 0 ) {
    OS_TPrintf( "nvram error: offset = 0x%02x\n", offset);
    (void)FS_CloseFile(&nor_fd);
    return FALSE;
  }

  offset *= 8;
  offset -= 0xA00;

  if( TRUE !=  my_nvram_read( offset , NVRAM_PERSONAL_DATA_SIZE, (void* )my_nor_buf) ) {
    OS_TPrintf( "nvram error: %s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
    ret_flag = FALSE;
  }
  else {
    len = my_fs_crypto_write(&nor_fd, my_nor_buf, NVRAM_PERSONAL_DATA_SIZE);
    if (len != NVRAM_PERSONAL_DATA_SIZE) {
      OS_TPrintf("FS_WriteFile() failed.");
      ret_flag = FALSE;
    }
  }

  OS_TPrintf("\n");

  FS_FlushFile(&nor_fd);

  bSuccess = FS_CloseFile(&nor_fd);

  //  OS_TPrintf( "nvram read completed.\n");
  return ret_flag;
}


typedef struct  tagDWCWiFiInfo {
  u64  attestedUserId;       // ���[�U ID (�F�؍ς�)
  u64  notAttestedId;        // ���[�U ID (�F�ؑO)
  u16  pass;                 // �p�X���[�h
  u16  randomHistory;        // ��������
} DWCWiFiInfo;



BOOL nvram_restore(char *path)
{
  BOOL bSuccess;
  BOOL ret_flag = TRUE;
  FSFile nor_fd;
  u32 offset;
  int len;
  //  char nor_file_path[FS_FILE_NAME_MAX];
  char *nor_file_path = path;

  u64 id1;
  u64 id2;

  FS_InitFile(&nor_fd);
  //  STD_TSNPrintf(nor_file_path, sizeof(nor_file_path), path );
  bSuccess = FS_OpenFileEx(&nor_fd, nor_file_path, FS_FILEMODE_R);
  if( ! bSuccess ) {
    OS_TPrintf("error %s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
    return FALSE;
  }

  /* offset�A�h���X�̎擾 */
  if( TRUE !=  my_nvram_read( NVRAM_PERSONAL_DATA_OFFSET , sizeof(u16), (void* )&offset) ) {
    OS_TPrintf( "nvram error: %s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
    ret_flag = FALSE;
  }
  else {
    OS_TPrintf( "nvram success: offset = 0x%02x\n", offset);
  }
 
  if( offset == 0 ) {
    OS_TPrintf( "nvram error: offset = 0x%02x\n", offset);
    return FALSE;
  }

  /* offset�̃`�F�b�N�́H */
  offset *= 8;
  offset -= 0xA00;
  
  len = my_fs_crypto_read(&nor_fd, my_nor_buf, NVRAM_PERSONAL_DATA_SIZE);
  if (len != NVRAM_PERSONAL_DATA_SIZE) {
    ret_flag = FALSE;
    OS_TPrintf("FS_ReadFile() failed.");
  }


  
  // DWCWiFiInfo *buf;
  //  u8 Wifi[14];

  // > �ƂȂ�܂��BDWC���C�u�����ɂ��C�����g��Ȃ��ꍇ��
  // > �E0x0f0��0x1f0�ɂ���ID���m�F���āA������ID�������A����0�łȂ��ꍇ�ɃR�s�[����
  //  MI_CpuCopy8(&p_ncfgc->slot[0].wifi[0], &id1, 6);
  MI_CpuCopy8(&my_nor_buf[0x600+ 0xf0], &id1, 6);
  id1 &= 0x07FFFFFFFFFF;

  MI_CpuCopy8(&my_nor_buf[0x600+ 0x1f0], &id2, 6);
  id2 &= 0x07FFFFFFFFFF;

  if( (id1 == id2) && (id1 != 0) ) {
    if( TRUE !=  my_nvram_write( offset , /* size */ NVRAM_PERSONAL_DATA_SIZE, (void* )my_nor_buf) ) {
      ret_flag = FALSE;
      OS_TPrintf( "nvram write error: %s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
    }
  }
  else {
    OS_TPrintf( "nvram write id1 id2 - 0 %s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
    ret_flag = 3;
  }

  /*
    #define NVRAM_PERSONAL_DATA_OFFSET 0x20
    
    Wifi�ݒ肾���Ȃ�0x0A00
    #define NVRAM_PERSONAL_DATA_SIZE 0x0A00
    #define NVRAM_INTERNAL_BUF_SIZE 0x100
    
    
    �ENOR��0x20����2byte��ǂݏo���A8�{����i��������0x1fe00���炢�ɂȂ�j
    �E��������-0x400�����l�i��������0x1fa00�j��Wi-Fi���[�U�[���̐擪�ƂȂ�
    
    
    �����āA�{ID���L�邩�A�������̊m�F��
    Wi-Fi���[�U�[����0xf0����14byte��Wi-Fi�R�l�N�V����ID���ɂȂ�܂��B
    ����14byte��Wi-Fi�R�l�N�V����ID���̌��43bit���F�؍ς݃��[�U�[ID���i�[�����
    �ꏊ�ƂȂ�܂��̂ŁA�����̒l�����ׂāu0�v�̏ꍇ�́A�{ID�������ƌ��Ȃ������ł��܂��B
    # �ڍׂ͓Y�t�̎��������m�F���������B
    # ������DS�̂��̂ł����AWi-Fi�R�l�N�V����ID���̕����͋��ʂƂȂ�܂��B
    
    �����āA����̃c�[���ŏC�������肢���������e�Ƃ��܂��Ă�
    �E��L��Wi-Fi�R�l�N�V����ID���ɖ{ID�����݂��Ȃ��ꍇ�́A
    �@Wi-Fi���[�U�[���S�Ă��ڍs���Ȃ�
    �ɂȂ�܂��B
    
    �ڍs���Ȃ��̈��
    �EWi-Fi���[�U�[���̐擪�i��������0x1fa00�j����X��-0x600�����Ƃ���i��������0x1f400�j
    �@����0x1000byte
    �@# -0x600�ɂ�TWL�Ŋg�����ꂽ�̈悪����܂��B
    �ƁA�Ȃ�܂��B
  */




  OS_TPrintf("\n");
  bSuccess = FS_CloseFile(&nor_fd);

  OS_TPrintf( "nvram write completed.\n");

  return ret_flag;
}

