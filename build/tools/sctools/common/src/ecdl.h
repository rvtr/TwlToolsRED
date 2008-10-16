/*---------------------------------------------------------------------------*
  Project:  TwlSDK - tools - ecdl
  File:     ecdl.h

  Copyright 2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-19#$
  $Rev: 8593 $
  $Author: hatamoto_minoru $
 *---------------------------------------------------------------------------*/
#ifndef ECDL_H_
#define ECDL_H_

#define EC_NO_JS

#include <twl.h>
#include <twl/nam.h>
#include <nitro/fs.h>
#include "es.h"
#include "ecx.h"

#define PRINT_LINE  OS_TPrintf("%4d\n", __LINE__)

#ifdef __cplusplus
extern "C" {
#endif

void ECDownload(const NAMTitleId* pTitleIds, u32 numTitleIds);
void KPSClient();
void WaitEC(ECOpId opId);


#ifdef __cplusplus
}
#endif

#endif  // ECDL_H_
