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

#ifndef __MMD_CLIENT_H__
#define __MMD_CLIENT_H__

#include <twl.h>
#include "mmdumper.h"

#ifdef __cplusplus
extern "C" {
#endif

// define data----------------------------------------------------------
typedef enum MMDCStage
{
    MMDC_STAGE_INIT,
    MMDC_STAGE_WAIT_INIT,
    MMDC_STAGE_CHILD_CONNECT_AUTO,
    MMDC_STAGE_WAIT_CHILD_CONNECT_AUTO,
    MMDC_STAGE_START_MP,
    MMDC_STAGE_END,

    MMDC_STAGE_NUMS
} MMDCStage;

// global variables--------------------------------------------------

// function-------------------------------------------------------------
void printClient(void);
void actionClient(void);

#ifdef __cplusplus
}
#endif

#endif  // __MISC_H__
