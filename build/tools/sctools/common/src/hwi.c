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
#include        "my_fs_util.h"


/*
// TWL本体設定データリード（関数内でNTR本体設定データのリードも行う。）
extern BOOL LCFG_ReadTWLSettings( u8 (*pTempBuffer)[ LCFG_READ_TEMP ] );

// TWL本体設定データライト（関数内でNTR本体設定データへのライトも行う。）
extern BOOL LCFG_WriteTWLSettings( u8 (*pTempBuffer)[ LCFG_WRITE_TEMP ] );

*/

// TWL設定データのリード
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
      /* 内部バッファーからダイレクトにコピーする */
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
    /* 内部バッファーにダイレクトにコピーする */
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

#if 0
  writtenSize = FS_WriteFile(&f, (void *)&cfg_data, (s32)sizeof(LCFGTWLSettingsData) );
#else
  writtenSize = my_fs_crypto_write(&f, (void *)&cfg_data, (s32)sizeof(LCFGTWLSettingsData) );
#endif

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
  LCFGTWLTPCalibData tp_cal_data;

  retval = LCFGi_THW_ReadSecureInfo();
  if( retval != LCFG_TSF_READ_RESULT_SUCCEEDED ) {
    OS_TPrintf( "HW Normal Info read failed.\n" );
    mprintf( "HW Normal Info read failed.\n" );
    return FALSE;
  }

  if( FALSE ==  ReadTWLSettings( &cfg_data ) ) {
    mprintf("Failed read TWLSettings 2.\n" );
    return FALSE;
  }

  /* とりあえず別でＴＰキャリブレーションデータだけ置いとく */
  LCFG_TSD_GetTPCalibration( &tp_cal_data );


  FS_InitFile(&f);
  bSuccess = FS_OpenFileEx(&f, path, FS_FILEMODE_R);
  if (bSuccess == FALSE) {
    fsResult = FS_GetArchiveResultCode(path);
    mprintf("Failed open file 2 - HWNormal Info.:%d\n", fsResult );
    return FALSE;
  }

  readSize = my_fs_crypto_read(&f, (void *)&cfg_data, (s32)sizeof(LCFGTWLSettingsData) );


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

  /* さっき置いといたＴＰキャリブレーションデータを上書き */
  STD_CopyMemory( (void *)&cfg_data.tp, (void *)&tp_cal_data ,sizeof(LCFGTWLTPCalibData) );

  /* 実際に書き出し */
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
    OS_TPrintf( "HW Secure Info read failed.\n" );
    mprintf( "HW Secure Info read failed.\n" );
    return FALSE;
  }

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


