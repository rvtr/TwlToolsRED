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

#include <twl/os/common/ownerInfoEx.h>


#if 0
typedef struct LCFGTWLTPCalibData
{
    LCFGNTRTPCalibData  data;                   // TP�L�����u���[�V�����f�[�^
    u8              rsv[ 8 ];
} LCFGTWLTPCalibData;   // 20 bytes


// �^�b�`�p�l���L�����u���[�V�����f�[�^
typedef struct LCFGNTRTPCalibData
{
    u16             raw_x1;                     // ��P�L�����u���[�V�����_��TP�擾�lX
    u16             raw_y1;                     //         �V                TP�擾�lY
    u8              dx1;                        //         �V                LCD���W X
    u8              dy1;                        //         �V                LCD���W Y
    u16             raw_x2;                     // ��Q�L�����u���[�V�����_��TP�擾�lX
    u16             raw_y2;                     //         �V                TP�擾�lY
    u8              dx2;                        //         �V                LCD���W X
    u8              dy2;                        //         �V                LCD���W Y
} LCFGNTRTPCalibData;   // 12byte
#endif


#define  OS_ADDR_TWL_SETTINGSDATA   ( (OSTWLSettingsData *)HW_PARAM_TWL_SETTINGS_DATA )

void WLAN_FORCE_ON(void)
{
  OSTWLSettingsData *p = (OSTWLSettingsData*)OS_ADDR_TWL_SETTINGSDATA;
  OS_TPrintf("w flag %d\n",p->flags.isAvailableWireless);
  if( p->flags.isAvailableWireless != 0x1 ) {
    p->flags.isAvailableWireless = 0;
  }
}


BOOL IsThisDevelopmentConsole(void)
{
  /* 0 ~ 3   : bonding option information */
  /*
    0 �ʎY�@ 
    1 �f�o�b�K�[
    2 �J�����@
    3 ??
  */
  u16 scfg_bonding_option = 0xffff;

  scfg_bonding_option = SCFG_ReadBondingOption();

  if( scfg_bonding_option == 0 ) {
    /* �ʎY�@ */
    return FALSE;
  }
  /* �J���@ */
  return TRUE;
}


/*
BOOL OS_IsAvailableWireless(void)
{
    BOOL result;
    OSTWLSettingsData *p;
    if( OS_IsRunOnTwl() )
    {
        p = (OSTWLSettingsData*)OS_ADDR_TWL_SETTINGSDATA;
        result = (p->flags.isAvailableWireless == 0x1);
    }
    else
    {
        result = TRUE;
    }
    return (result);
}
*/

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


void  MiyaReadTpCalData(void)
{
  LCFGTWLSettingsData cfg_data;
  LCFGTWLTPCalibData tp_cal_data;
  if( FALSE ==  ReadTWLSettings( &cfg_data ) ) {
    mprintf("Failed read TWLSettings 3.\n" );
  }
  
  /* �Ƃ肠�����ʂłs�o�L�����u���[�V�����f�[�^�����u���Ƃ� */
  LCFG_TSD_GetTPCalibration( &tp_cal_data );
  
  OS_TPrintf("tp. raw_x1 = %d\n",tp_cal_data.data.raw_x1);
  OS_TPrintf("tp.raw_y1 = %d\n",tp_cal_data.data.raw_y1);
  OS_TPrintf("tp.dx1 = %d\n",tp_cal_data.data.dx1);
  OS_TPrintf("tp.dy1 = %d\n",tp_cal_data.data.dy1);
  OS_TPrintf("tp.raw_x2 = %d\n",tp_cal_data.data.raw_x2);
  OS_TPrintf("tp.raw_y2 = %d\n",tp_cal_data.data.raw_y2);
  OS_TPrintf("tp.dx2 = %d\n",tp_cal_data.data.dx2);
  OS_TPrintf("tp.dy2 = %d\n",tp_cal_data.data.dy2);
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
  BOOL tp_over_write_flag = TRUE;

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

  /* �Ƃ肠�����ʂłs�o�L�����u���[�V�����f�[�^�����u���Ƃ� */
  LCFG_TSD_GetTPCalibration( &tp_cal_data );
  if( (tp_cal_data.data.raw_x1 == 0 )
      && (tp_cal_data.data.raw_y1 == 0 )
      && (tp_cal_data.data.dx1 == 0 )
      && (tp_cal_data.data.dy1 == 0 )
      && (tp_cal_data.data.raw_x2 == 0 )
      && (tp_cal_data.data.raw_y2 == 0 )
      && (tp_cal_data.data.dx2 == 0 )
      && (tp_cal_data.data.dy2 == 0 ) ) {
    tp_over_write_flag = FALSE;
  }


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

  if( tp_over_write_flag != FALSE ) {
    /* �������u���Ƃ����s�o�L�����u���[�V�����f�[�^���㏑�� */
    STD_CopyMemory( (void *)&cfg_data.tp, (void *)&tp_cal_data ,sizeof(LCFGTWLTPCalibData) );
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


