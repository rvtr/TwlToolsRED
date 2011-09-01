/*---------------------------------------------------------------------------*
  Project:  TwlSDK - nandApp - demos - launcher_param
  File:     misc_simple.h

  Copyright 2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date::            $
  $Rev$
  $Author$
 *---------------------------------------------------------------------------*/

#ifndef __MMDUMPER_H__
#define __MMDUMPER_H__

#include <twl.h>

#ifdef __cplusplus
extern "C" {
#endif

// define data----------------------------------------------------------
typedef enum MMDumperStage
{
    MMD_STAGE_INIT = 0,
    MMD_STAGE_PARENT,
    MMD_STAGE_CLIENT,
    MMD_STAGE_PRE_END,
    MMD_STAGE_END,

    MMD_STAGE_ERROR,

    MMD_STAGE_NUMS
} MMDumperStage;

#define BEGIN_ADDRESS   0x02000000
#define END_ADDRESS     0x023ffd80  // +1

typedef struct UserGameInfo
{
    u32 wepSeed;
} UserGameInfo;

typedef struct RequestFormat
{
    u8* addr;
    u16 length;
} RequestFormat;

// global variables--------------------------------------------------
// WEP Key 作成用の共通鍵（親子で共通の鍵を使用する）
// アプリケーションごとに固有のものとすること
// ASCII 文字列である必要はなく、任意の長さのバイナリデータでよい
#define SECRET_KEY  "this is a secret key for Main Memory Dumper"
#define SECRET_KEY_LENGTH   sizeof(SECRET_KEY)

// function-------------------------------------------------------------
void mmdumperInit(void);
void mmdumperEnd(void);
BOOL mmdumperMain(void);

void printWMError(const char* strWMError);

u16 cursorUpDown(u16 now, u16 nums);


#ifdef __cplusplus
}
#endif

#endif  // __MISC_H__
