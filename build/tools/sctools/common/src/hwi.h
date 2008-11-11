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
    u8              rtcAdjust;                                  // RTC調整値
	u8				rsv[ 3 ];
	u8				movableUniqueID[ LCFG_TWL_HWINFO_MOVABLE_UNIQUE_ID_LEN ]; // 移行可能なユニークID
} LCFGTWLHWNormalInfo;  // 20byte



typedef struct LCFGTWLHWSecureInfo
{
    u32             validLanguageBitmap;                            // 本体で有効な言語コードをビット列で表現
	struct {
		u8			forceDisableWireless :1;
		u8			:7;
	}flags;
	u8				pad[ 3 ];
    u8              region;                                         // リージョン
    u8              serialNo[ LCFG_TWL_HWINFO_SERIALNO_LEN_MAX ];   // シリアルNo.（終端付きASCII文字列）
//-------------------------------------------------------------
//  [TODO:]ランチャーでここまでの24bytesをSystemSharedにコピー。
//-------------------------------------------------------------
    u8              launcherTitleID_Lo[ 4 ];                        // NANDファームがリージョンによって異なる可能性のあるランチャーのTitleIDを識別するために使用
} LCFGTWLHWSecureInfo;  // 28bytes


static inline void LCFG_TSD_GetTPCalibration( LCFGTWLTPCalibData *pDst )
{
    MI_CpuCopy8( &LCFGi_GetTSD()->tp, pDst, sizeof(LCFGTWLTPCalibData) );
}

	// タッチパネルキャリブレーションデータのセット。
static inline void LCFG_TSD_SetTPCalibration( const LCFGTWLTPCalibData *pTPCalib )
{
    MI_CpuCopy16( pTPCalib, &LCFGi_GetTSD()->tp, sizeof(LCFGTWLTPCalibData) );
//	MI_CpuCopy16( pTPCalib, &LCFGi_GetTSD_OS()->tp, sizeof(LCFGTWLTPCalibData) );
}

typedef struct LCFGTWLSettingsData
{
	union {
	    struct {
    	    u32     isFinishedInitialSetting : 1;       // 初回設定終了？
	        u32     isFinishedInitialSetting_Launcher : 1;	// ランチャーの初回設定終了？
			u32		isSetLanguage : 1;					// 言語コード設定済み？
	        u32     isAvailableWireless : 1;            // 無線モジュールのRFユニットの有効化／無効化
			u32     isBrokenTWLSettings : 1;			// フラッシュ壊れシーケンス中？
	        u32     rsv : 19;							// 予約
			u32		isAgreeEULAFlagList : 8;			// EULA同意フラグリスト(最下位ビットしか使用していない）
	        // WiFi設定は別データなので、ここに設定済みフラグは用意しない。
		};
		u32			raw;
	} flags;
	u8						rsv[ 1 ];				// 予約
    u8                      country;                // 国コード
    u8                      language;               // 言語(NTRとの違いは、データサイズ8bit)
    u8                      rtcLastSetYear;         // RTCの前回設定年
    s64                     rtcOffset;              // RTC設定時のオフセット値（ユーザーがRTC設定を変更する度にその値に応じて増減します。）
													//   16bytes ここまでのパラメータサイズ
	u8						agreeEulaVersion[ EULA_VERSION_LIST_NUM ];	//    8bytes 同意したEULAのバージョン
	u8						pad[2];
    LCFGTWLAlarm            alarm;                  //    6bytes アラーム
	LCFGTWLLauncherStatus	launcherStatus;			//   16bytes ランチャーステータス情報
    LCFGTWLTPCalibData      tp;                     //   20bytes タッチパネルキャリブレーションデータ
    LCFGTWLOwnerInfo        owner;                  //   80bytes オーナー情報
    LCFGTWLParentalControl  parental;				//  148bytes ペアレンタルコントロール情報
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
