/*---------------------------------------------------------------------------*
  Project:  TwlIPL
  File:     DS_Chat.h

  Copyright 2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-03-31#$
  $Rev: 1019 $
  $Author: kamikawa $
 *---------------------------------------------------------------------------*/

#ifndef	__HWI_LIB__
#define	__HWI_LIB__

#ifdef __cplusplus
extern "C" {
#endif


#include <twl.h>

#include <twl/lcfg/common/api.h>
#include <twl/lcfg/common/TWLSettings.h>

#include "TWLHWInfo_api.h"
#include "TWLSettings_api.h"


// define data----------------------------------------------------------

BOOL MiyaBackupHWNormalInfo(const char *path);
BOOL MiyaRestoreHWNormalInfo(const char *path);
BOOL MiyaBackupTWLSettings(const char *path);
BOOL MiyaRestoreTWLSettings(const char *path);
BOOL MiyaReadHWNormalInfo( LCFGTWLHWNormalInfo *Info );
BOOL MiyaReadHWSecureInfo( LCFGTWLHWSecureInfo *Info );
BOOL MiyaReadHWNormalInfo_From_SD(const char *path,   LCFGTWLHWNormalInfo *info);
void WLAN_FORCE_ON(void);
void MiyaReadTpCalData(void);

#if 0
typedef struct LCFGTWLHWNormalInfo
{
    u8              rtcAdjust;                                  // RTC�����l
	u8				rsv[ 3 ];
	u8				movableUniqueID[ LCFG_TWL_HWINFO_MOVABLE_UNIQUE_ID_LEN ]; // �ڍs�\�ȃ��j�[�NID
} LCFGTWLHWNormalInfo;  // 20byte



typedef struct LCFGTWLHWSecureInfo
{
    u32             validLanguageBitmap;                            // �{�̂ŗL���Ȍ���R�[�h���r�b�g��ŕ\��
	struct {
		u8			forceDisableWireless :1;
		u8			:7;
	}flags;
	u8				pad[ 3 ];
    u8              region;                                         // ���[�W����
    u8              serialNo[ LCFG_TWL_HWINFO_SERIALNO_LEN_MAX ];   // �V���A��No.�i�I�[�t��ASCII������j
//-------------------------------------------------------------
//  [TODO:]�����`���[�ł����܂ł�24bytes��SystemShared�ɃR�s�[�B
//-------------------------------------------------------------
    u8              launcherTitleID_Lo[ 4 ];                        // NAND�t�@�[�������[�W�����ɂ���ĈقȂ�\���̂��郉���`���[��TitleID�����ʂ��邽�߂Ɏg�p
} LCFGTWLHWSecureInfo;  // 28bytes


static inline void LCFG_TSD_GetTPCalibration( LCFGTWLTPCalibData *pDst )
{
    MI_CpuCopy8( &LCFGi_GetTSD()->tp, pDst, sizeof(LCFGTWLTPCalibData) );
}

	// �^�b�`�p�l���L�����u���[�V�����f�[�^�̃Z�b�g�B
static inline void LCFG_TSD_SetTPCalibration( const LCFGTWLTPCalibData *pTPCalib )
{
    MI_CpuCopy16( pTPCalib, &LCFGi_GetTSD()->tp, sizeof(LCFGTWLTPCalibData) );
//	MI_CpuCopy16( pTPCalib, &LCFGi_GetTSD_OS()->tp, sizeof(LCFGTWLTPCalibData) );
}

typedef struct LCFGTWLSettingsData
{
	union {
	    struct {
    	    u32     isFinishedInitialSetting : 1;       // ����ݒ�I���H
	        u32     isFinishedInitialSetting_Launcher : 1;	// �����`���[�̏���ݒ�I���H
			u32		isSetLanguage : 1;					// ����R�[�h�ݒ�ς݁H
	        u32     isAvailableWireless : 1;            // �������W���[����RF���j�b�g�̗L�����^������
			u32     isBrokenTWLSettings : 1;			// �t���b�V�����V�[�P���X���H
	        u32     rsv : 19;							// �\��
			u32		isAgreeEULAFlagList : 8;			// EULA���Ӄt���O���X�g(�ŉ��ʃr�b�g�����g�p���Ă��Ȃ��j
	        // WiFi�ݒ�͕ʃf�[�^�Ȃ̂ŁA�����ɐݒ�ς݃t���O�͗p�ӂ��Ȃ��B
		};
		u32			raw;
	} flags;
	u8						rsv[ 1 ];				// �\��
    u8                      country;                // ���R�[�h
    u8                      language;               // ����(NTR�Ƃ̈Ⴂ�́A�f�[�^�T�C�Y8bit)
    u8                      rtcLastSetYear;         // RTC�̑O��ݒ�N
    s64                     rtcOffset;              // RTC�ݒ莞�̃I�t�Z�b�g�l�i���[�U�[��RTC�ݒ��ύX����x�ɂ��̒l�ɉ����đ������܂��B�j
													//   16bytes �����܂ł̃p�����[�^�T�C�Y
	u8						agreeEulaVersion[ EULA_VERSION_LIST_NUM ];	//    8bytes ���ӂ���EULA�̃o�[�W����
	u8						pad[2];
    LCFGTWLAlarm            alarm;                  //    6bytes �A���[��
	LCFGTWLLauncherStatus	launcherStatus;			//   16bytes �����`���[�X�e�[�^�X���
    LCFGTWLTPCalibData      tp;                     //   20bytes �^�b�`�p�l���L�����u���[�V�����f�[�^
    LCFGTWLOwnerInfo        owner;                  //   80bytes �I�[�i�[���
    LCFGTWLParentalControl  parental;				//  148bytes �y�A�����^���R���g���[�����
} LCFGTWLSettingsData;  //  296bytes


#define LCFG_TWL_FLAG_SET_LANGUAGE				( 1 << LCFG_TWL_FLAG_SET_LANGUAGE_SHIFT )
#define LCFG_TWL_FLAG_SET_DATE_TIME				( 1 << LCFG_TWL_FLAG_SET_DATE_TIME_SHIFT )
#define LCFG_TWL_FLAG_SET_COUNTRY				( 1 << LCFG_TWL_FLAG_SET_COUNTRY_SHIFT )
#define LCFG_TWL_FLAG_SET_FACE_PHOTO			( 1 << LCFG_TWL_FLAG_SET_FACE_PHOTO_SHIFT )
#define LCFG_TWL_FLAG_SET_NICKNAME				( 1 << LCFG_TWL_FLAG_SET_NICKNAME_SHIFT )
#define LCFG_TWL_FLAG_SET_COMMENT				( 1 << LCFG_TWL_FLAG_SET_COMMENT_SHIFT )
#define LCFG_TWL_FLAG_SET_BIRTHDAY				( 1 << LCFG_TWL_FLAG_SET_BIRTHDAY_SHIFT )
#define LCFG_TWL_FLAG_SET_USER_COLOR			( 1 << LCFG_TWL_FLAG_SET_USER_COLOR_SHIFT )
#define LCFG_TWL_FLAG_SET_TP

#endif


#ifdef __cplusplus
}
#endif

#endif  // __HWI_LIB__
