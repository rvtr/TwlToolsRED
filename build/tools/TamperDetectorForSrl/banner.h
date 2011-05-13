/*---------------------------------------------------------------------------*
  Project:  TwlSDK - tools - makebanner
  File:     banner.h

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

 *---------------------------------------------------------------------------*/
#ifndef	 TWL_OS_COMMON_BANNER_H_
#define	 TWL_OS_COMMON_BANNER_H_

// define data---------------------------------------------

#define BANNER_IMAGE_SIZE				(32 * 32 / (8/4))
#define BANNER_PLTT_SIZE				(16 * 2)
#define BANNER_LANG_LENGTH				128
#define BANNER_LANG_SIZE				(BANNER_LANG_LENGTH * 2)

#define BANNER_VER_NTR_MIN				1		// NTR�o�i�[ver.MIN.
#define BANNER_VER_NTR_MAX				3		// NTR�o�i�[ver.MAX.(smaller than UCHAR_MAX)
#define BANNER_VER_TWL_MIN				3		// TWL�o�i�[ver.MIN.
#define BANNER_VER_TWL_MAX				3		// TWL�o�i�[ver.MAX.(smaller than UCHAR_MAX)

#define BANNER_CHINESE_SUPPORT_VER		2		// ������T�|�[�gver.
#define BANNER_KOREAN_SUPPORT_VER		3		// �؍���T�|�[�gver.

#define BANNER_LANG_NUM_RSV				8		// ����\��̈搔
#define BANNER_ANIME_PATTERN_NUM		8		// �o�i�[�A�j���p�^�[����
#define BANNER_ANIME_CONTROL_INFO_NUM	64		// �o�i�[�A�j���R���g���[�����


typedef enum {
    BANNER_PRIO_JAPANESE = 0,
    BANNER_PRIO_ENGLISH,
    BANNER_PRIO_FRENCH,
    BANNER_PRIO_GERMAN,
    BANNER_PRIO_ITALIAN,
    BANNER_PRIO_SPANISH,
    BANNER_PRIO_CHINESE,
    BANNER_PRIO_KOREAN,
    BANNER_LANG_NUM,

    BANNER_LANG_NUM_V1 = BANNER_PRIO_CHINESE,
    BANNER_LANG_NUM_V2 = BANNER_PRIO_KOREAN - BANNER_PRIO_CHINESE,
    BANNER_LANG_NUM_V3 = BANNER_LANG_NUM - BANNER_PRIO_KOREAN
}BannerLanguagePriorityIdx;


// �v���b�g�t�H�[���R�[�h
typedef enum {
	BANNER_PLATFORM_NTR = 0,
	BANNER_PLATFORM_TWL = 1,
	
	BANNER_PLATFORM_MAX = 2
}BannerPlatformCode;


// �o�i�[�w�b�_
typedef struct {
    u8      version;
    u8      platform;			// ��LBannerPlatformCode�Ŏw��
    u16     crc16_v1;
    u16     crc16_v2;
    u16     crc16_v3;
    u16     crc16_anime;
    u8      reserved_B[ 22 ];
}BannerHeader;


// �o�i�[ver.1 �{�f�B
typedef struct {
    u8      image[ BANNER_IMAGE_SIZE ];
    u8      pltt[ BANNER_PLTT_SIZE ];
    u16     gameName[ BANNER_LANG_NUM_V1 ][ BANNER_LANG_LENGTH ];
}BannerFileV1;


// �o�i�[ver.2 �{�f�B�ǉ���
typedef struct {
    u16     gameName[ BANNER_LANG_NUM_V2 ][ BANNER_LANG_LENGTH ];
}BannerFileV2;


// �o�i�[ver.3 �{�f�B�ǉ���
typedef struct {
    u16     gameName[ BANNER_LANG_NUM_V3 ][ BANNER_LANG_LENGTH ];
}BannerFileV3;


// �o�i�[gameName ����g���\��̈�
typedef struct {
    u16     gameName[ BANNER_LANG_NUM_RSV ][ BANNER_LANG_LENGTH ];
}BannerFileRsv;


// NTR�o�i�[�t�@�C���\����
typedef struct {
    BannerHeader	h;
    BannerFileV1	v1;
    BannerFileV2	v2;
    BannerFileV3	v3;
}NTRBannerFile;


#endif //TWL_OS_COMMON_BANNER_H_
