/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - demos - netconnect
  File:     netconnect.h

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-01-11#$
  $Rev: 212 $
  $Author: adachi_hiroaki $
 *---------------------------------------------------------------------------*/

#ifndef NITROWIFI_DEMOS_NETCONNECT_H_
#define NITROWIFI_DEMOS_NETCONNECT_H_


#define NC_ERROR_TIMEOUT 1
#define NC_ERROR_INVALID_AP_CLASS 2
#define NC_ERROR_LINKDOWN 3

#ifdef __cplusplus

extern "C"
{
#endif

void    NcGlobalInit(void);
int    NcStart(const char* apClass);
void    NcFinish(void);
void    NcSetDevice(u8 deviceId);
void*   NcAlloc(u32 name, s32 size);
void    NcFree(u32 name, void* ptr, s32 size);

#ifdef __cplusplus

} /* extern "C" */
#endif

#endif /* NITROWIFI_DEMOS_NETCOONECT_H_ */
