/*---------------------------------------------------------------------------*
  Project:  TwlSDK - tools - ecdl
  File:     kpsc.cpp

  Copyright 2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-22#$
  $Rev: 8604 $
  $Author: hatamoto_minoru $
 *---------------------------------------------------------------------------*/

#include "ecdl.h"


void
KPSClient()
{
    s32 progress;

    OS_TPrintf("generate key pair\n");
    progress = EC_GenerateKeyPair();

    OS_TPrintf("%s %d\n",__FUNCTION__,__LINE__);

    WaitEC(progress);
    OS_TPrintf("%s %d\n",__FUNCTION__,__LINE__);

    OS_TPrintf("confirm key pair\n");
    progress = EC_ConfirmKeyPair();
    OS_TPrintf("%s %d\n",__FUNCTION__,__LINE__);

    WaitEC(progress);

    OS_TPrintf("%s %d\n",__FUNCTION__,__LINE__);

}

