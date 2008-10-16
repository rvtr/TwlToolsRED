/*---------------------------------------------------------------------------*
  Project:  TwlIPL
  File:     HWInfoWriterLib.c

  Copyright 2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-05-09#$
  $Rev: 1309 $
  $Author: yosiokat $
 *---------------------------------------------------------------------------*/

#include <twl.h>
// #include <acsign.h>
#include "hwi.h"

#include        "font.h"
#include        "text.h"
#include        "mprintf.h"


/*
// TWL�{�̐ݒ�f�[�^���[�h�i�֐�����NTR�{�̐ݒ�f�[�^�̃��[�h���s���B�j
extern BOOL LCFG_ReadTWLSettings( u8 (*pTempBuffer)[ LCFG_READ_TEMP ] );

// TWL�{�̐ݒ�f�[�^���C�g�i�֐�����NTR�{�̐ݒ�f�[�^�ւ̃��C�g���s���B�j
extern BOOL LCFG_WriteTWLSettings( u8 (*pTempBuffer)[ LCFG_WRITE_TEMP ] );

*/

// TWL�ݒ�f�[�^�̃��[�h
static BOOL ReadTWLSettings( LCFGTWLSettingsData *cfg_data )
{
  BOOL isReadTSD = FALSE;
  u8 *pBuffer;
  if( cfg_data == NULL ) {
    return FALSE;
  }
  pBuffer = OS_Alloc( LCFG_TEMP_BUFFER_SIZE * 2 );
  if( pBuffer ) {
    isReadTSD = LCFG_ReadTWLSettings( (u8 (*)[ LCFG_TEMP_BUFFER_SIZE * 2 ] )pBuffer );
    if( isReadTSD == TRUE ) {
      /* �����o�b�t�@�[����_�C���N�g�ɃR�s�[���� */
      STD_CopyMemory( (void *)cfg_data, (void *)LCFGi_GetTSD(), sizeof(LCFGTWLSettingsData) );
      OS_TPrintf( "TSD read succeeded.\n" );
    }else {
      OS_TPrintf( "TSD read failed.\n" );
    }
    OS_Free( pBuffer );
  }
  return isReadTSD;
}  


static BOOL WriteTWLSettings( LCFGTWLSettingsData *cfg_data )
{
  BOOL isWriteTSD = FALSE;
  u8 *pBuffer;
  if( cfg_data == NULL ) {
    return FALSE;
  }
  pBuffer = OS_Alloc( LCFG_TEMP_BUFFER_SIZE );
  if( pBuffer ) {
    /* �����o�b�t�@�[�Ƀ_�C���N�g�ɃR�s�[���� */
    STD_CopyMemory( (void *)LCFGi_GetTSD(), (void *)cfg_data, sizeof(LCFGTWLSettingsData) );
    isWriteTSD = LCFG_WriteTWLSettings( (u8 (*)[ LCFG_TEMP_BUFFER_SIZE ] )pBuffer );
    if( isWriteTSD == FALSE ) {
      OS_TPrintf( "TSD write failed.\n" );
      mprintf( "TSD write failed.\n" );
    }
    OS_Free( pBuffer );
  }
  return isWriteTSD;
}

BOOL MiyaBackupTWLSettings(const char *path)
{
  FSFile f;
  BOOL bSuccess;
  FSResult fsResult;
  s32 writtenSize;
  LCFGTWLSettingsData cfg_data;
  LCFGReadResult retval;

  retval = LCFGi_THW_ReadSecureInfo();
  if( retval != LCFG_TSF_READ_RESULT_SUCCEEDED ) {
    OS_TPrintf( "HW Normal Info read failed.\n" );
    mprintf( "HW Normal Info read failed.\n" );
    return FALSE;
  }
  
  if( FALSE == ReadTWLSettings( &cfg_data ) ) {
    mprintf("Failed read cfg file 1.\n" );
    return FALSE;
  }

  FS_InitFile(&f);

  /* delete file ? */

  FS_CreateFileAuto(path, (FS_PERMIT_R|FS_PERMIT_W));
  bSuccess = FS_OpenFileEx(&f, path, (FS_FILEMODE_R|FS_FILEMODE_W));
  if (bSuccess == FALSE) {
    fsResult = FS_GetArchiveResultCode(path);
    mprintf("Failed open file 1 - HWNormal Info.:%d\n", fsResult );
    return FALSE;
  }

  writtenSize = FS_WriteFile(&f, (void *)&cfg_data, (s32)sizeof(LCFGTWLSettingsData) );
  if( writtenSize != sizeof(LCFGTWLSettingsData) ) {
    fsResult = FS_GetArchiveResultCode(path);
    mprintf("Failed write file 1 - HWNormal Info.:%d\n", fsResult );
    return FALSE;
  }

  bSuccess = FS_CloseFile(&f);
  if (bSuccess == FALSE) {
    fsResult = FS_GetArchiveResultCode(path);
    mprintf("Failed close file 1 - HWNormal Info.:%d\n", fsResult );
    return FALSE;
  }

  return TRUE;
}

BOOL MiyaRestoreTWLSettings(const char *path)
{
  FSFile f;
  BOOL bSuccess;
  FSResult fsResult;
  s32 readSize;
  LCFGTWLSettingsData cfg_data;
  LCFGReadResult retval;

  retval = LCFGi_THW_ReadSecureInfo();
  if( retval != LCFG_TSF_READ_RESULT_SUCCEEDED ) {
    OS_TPrintf( "HW Normal Info read failed.\n" );
    mprintf( "HW Normal Info read failed.\n" );
    return FALSE;
  }

  if( FALSE ==  ReadTWLSettings( &cfg_data ) ) {
    mprintf("Failed read cfg file 2.\n" );
  }


  FS_InitFile(&f);
  bSuccess = FS_OpenFileEx(&f, path, FS_FILEMODE_R);
  if (bSuccess == FALSE) {
    fsResult = FS_GetArchiveResultCode(path);
    mprintf("Failed open file 2 - HWNormal Info.:%d\n", fsResult );
    return FALSE;
  }
  readSize = FS_ReadFile(&f, (void *)&cfg_data, (s32)sizeof(LCFGTWLSettingsData) );
  if( readSize != sizeof(LCFGTWLSettingsData) ) {
    fsResult = FS_GetArchiveResultCode(path);
    mprintf("Failed read file 2 - HWNormal Info.:%d\n", fsResult );
    return FALSE;
  }

  bSuccess = FS_CloseFile(&f);
  if (bSuccess == FALSE) {
    fsResult = FS_GetArchiveResultCode(path);
    mprintf("Failed close file 2 - HWNormal Info.:%d\n", fsResult );
    return FALSE;
  }





  /* ���ۂɏ����o�� */
  if( FALSE == WriteTWLSettings( &cfg_data ) ) {
    return FALSE;
  }


  return TRUE;
}


BOOL MiyaReadHWSecureInfo( LCFGTWLHWSecureInfo *Info )
{
  LCFGReadResult retval;

  if( Info == NULL ) {
    return FALSE;
  }
  
  retval = LCFGi_THW_ReadSecureInfo();
  if( retval != LCFG_TSF_READ_RESULT_SUCCEEDED ) {
    OS_TPrintf( "HW Normal Info read failed.\n" );
    mprintf( "HW Normal Info read failed.\n" );
    return FALSE;
  }

  /* 
     c:/twlsdk/include/twl/lcfg/common/TWLHWInfo.h
     #define LCFGi_GetHWN()      ( &s_hwInfoN )
  */
  
  STD_CopyMemory( (void *)Info, (void *)LCFGi_GetHWS() , sizeof(LCFGTWLHWSecureInfo) );

  OS_TPrintf( "HW Secure Info read succeeded.\n" );
  return TRUE;
}


BOOL MiyaReadHWNormalInfo( LCFGTWLHWNormalInfo *Info )
{
  LCFGReadResult retval;

  if( Info == NULL ) {
    return FALSE;
  }
  
  retval = LCFGi_THW_ReadNormalInfo();
  if( retval != LCFG_TSF_READ_RESULT_SUCCEEDED ) {
    OS_TPrintf( "HW Normal Info read failed.\n" );
    mprintf( "HW Normal Info read failed.\n" );
    return FALSE;
  }

  /* 
     c:/twlsdk/include/twl/lcfg/common/TWLHWInfo.h
     #define LCFGi_GetHWN()      ( &s_hwInfoN )
  */
  
  STD_CopyMemory( (void *)Info, (void *)LCFGi_GetHWN() , sizeof(LCFGTWLHWNormalInfo) );

  OS_TPrintf( "HW Normal Info read succeeded.\n" );
  return TRUE;
}



/*---------------------------------------------------------------------------*
  Name:         HWI_WriteHWNormalInfoFile

  Description:  HW�m�[�}��Info�t�@�C���̃��C�g

  Arguments:

  Returns:      None.
 *---------------------------------------------------------------------------*/
static BOOL MiyaWriteHWNormalInfoFile( LCFGTWLHWNormalInfo *Info )
{
  /* 
     
     typedef struct LCFGTWLHWNormalInfo {
     u8  rtcAdjust; // RTC�����l
     u8  rsv[ 3 ];
     u8  movableUniqueID[ LCFG_TWL_HWINFO_MOVABLE_UNIQUE_ID_LEN ]; // �ڍs�\�ȃ��j�[�NID
     } LCFGTWLHWNormalInfo;  // 20byte
  */
  
  if (!LCFGi_THW_WriteNormalInfoDirect( Info )) {
    OS_TPrintf( "HW Normal Info Write failed.\n" );
    mprintf( "HW Normal Info Write failed.\n" );
    return FALSE;
  }
  return TRUE;
}




BOOL MiyaBackupHWNormalInfo(const char *path)
{
  FSFile f;
  BOOL bSuccess;
  FSResult fsResult;
  s32 writtenSize;

  LCFGTWLHWNormalInfo info;

  if( FALSE == MiyaReadHWNormalInfo( &info ) ) {
    return FALSE;
  }

  FS_InitFile(&f);

  FS_CreateFileAuto(path, (FS_PERMIT_R|FS_PERMIT_W));
  bSuccess = FS_OpenFileEx(&f, path, (FS_FILEMODE_R|FS_FILEMODE_W));
  if (bSuccess == FALSE) {
    fsResult = FS_GetArchiveResultCode(path);
    mprintf("Failed open file 1 - HWNormal Info.:%d\n", fsResult );
    return FALSE;
  }


  writtenSize = FS_WriteFile(&f, (void *)&info, (s32)sizeof(LCFGTWLHWNormalInfo) );
  if( writtenSize != sizeof(LCFGTWLHWNormalInfo) ) {
    fsResult = FS_GetArchiveResultCode(path);
    mprintf("Failed write file 1 - HWNormal Info.:%d\n", fsResult );
    return FALSE;
  }

  bSuccess = FS_CloseFile(&f);
  if (bSuccess == FALSE) {
    fsResult = FS_GetArchiveResultCode(path);
    mprintf("Failed close file 1 - HWNormal Info.:%d\n", fsResult );
    return FALSE;
  }

  return TRUE;
}

BOOL MiyaReadHWNormalInfo_From_SD(const char *path,   LCFGTWLHWNormalInfo *info)
{
  FSFile f;
  BOOL bSuccess;
  FSResult fsResult;
  s32 readSize;

  if( info == NULL ) {
    return FALSE;
  }

  FS_InitFile(&f);
  bSuccess = FS_OpenFileEx(&f, path, FS_FILEMODE_R);
  if (bSuccess == FALSE) {
    fsResult = FS_GetArchiveResultCode(path);
    mprintf("Failed open file 2 - HWNormal Info.:%d\n", fsResult );
    return FALSE;
  }
  readSize = FS_ReadFile(&f, (void *)info, (s32)sizeof(LCFGTWLHWNormalInfo) );
  if( readSize != sizeof(LCFGTWLHWNormalInfo) ) {
    fsResult = FS_GetArchiveResultCode(path);
    mprintf("Failed read file 2 - HWNormal Info.:%d\n", fsResult );
    return FALSE;
  }

  bSuccess = FS_CloseFile(&f);
  if (bSuccess == FALSE) {
    fsResult = FS_GetArchiveResultCode(path);
    mprintf("Failed close file 2 - HWNormal Info.:%d\n", fsResult );
    return FALSE;
  }

  return TRUE;
}


BOOL MiyaRestoreHWNormalInfo(const char *path)
{
  FSFile f;
  BOOL bSuccess;
  FSResult fsResult;
  s32 readSize;

  LCFGTWLHWNormalInfo info;
  LCFGTWLHWNormalInfo info_temp;

  FS_InitFile(&f);
  bSuccess = FS_OpenFileEx(&f, path, FS_FILEMODE_R);
  if (bSuccess == FALSE) {
    fsResult = FS_GetArchiveResultCode(path);
    mprintf("Failed open file 2 - HWNormal Info.:%d\n", fsResult );
    return FALSE;
  }
  readSize = FS_ReadFile(&f, (void *)&info, (s32)sizeof(LCFGTWLHWNormalInfo) );
  if( readSize != sizeof(LCFGTWLHWNormalInfo) ) {
    fsResult = FS_GetArchiveResultCode(path);
    mprintf("Failed read file 2 - HWNormal Info.:%d\n", fsResult );
    return FALSE;
  }

  bSuccess = FS_CloseFile(&f);
  if (bSuccess == FALSE) {
    fsResult = FS_GetArchiveResultCode(path);
    mprintf("Failed close file 2 - HWNormal Info.:%d\n", fsResult );
    return FALSE;
  }


  /* RTC�̕␳�l�����͂��̂܂܎g�� */
  if( FALSE == MiyaReadHWNormalInfo( &info_temp ) ) {
    return FALSE;
  }
  info.rtcAdjust = info_temp.rtcAdjust;

  /* ���ۂɏ����o�� */
  if( FALSE == MiyaWriteHWNormalInfoFile( &info ) ) {
    return FALSE;
  }

  return TRUE;
}



