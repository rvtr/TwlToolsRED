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

#ifndef __MMD_PARENT_H__
#define __MMD_PARENT_H__

#include <twl.h>
#include "mmdumper.h"

#ifdef __cplusplus
extern "C" {
#endif

// define data----------------------------------------------------------
typedef enum MMDPStage
{
    MMDP_STAGE_INIT,
    MMDP_STAGE_WAIT_INIT,
    MMDP_STAGE_MEASURE_CHANNEL,
    MMDP_STAGE_WAIT_MEASURE_CHANNEL,
    MMDP_STAGE_START_PARENT,
    MMDP_STAGE_WAIT_START_PARENT,
    MMDP_STAGE_START_MP,
    MMDP_STAGE_END,

    MMDP_STAGE_NUMS
} MMDPStage;

// global variables--------------------------------------------------

// function-------------------------------------------------------------
void printParent(void);
void actionParent(void);

#ifdef __cplusplus
}
#endif

#endif  // __MISC_H__
