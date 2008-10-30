/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - demos - netconnect
  File:     sitedefs.h

  Copyright 2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-07-14#$
  $Rev: 788 $
  $Author: adachi_hiroaki $
 *---------------------------------------------------------------------------*/
#ifndef NITROWIFI_SITEDEFS_H_
#define NITROWIFI_SITEDEFS_H_

#ifdef __cplusplus
extern "C" {
#endif


#define SITEDEFS_DEFAULTCLASS               "WiFi.LAN.1.AP.1"
#define SITEDEFS_DEFAULTCLASS_FOR_TEST      "WiFi.LAN.1.AP.1"
void    SiteDefs_Init(void);
BOOL ENV_SetBinary(const char *name, void *ptr);
BOOL ENV_SetBinary2(const char *name, void *ptr, u32 length);
BOOL ENV_SetString(const char *name, char *str);
BOOL ENV_SetU8(const char *name, u8 val);



#ifdef __cplusplus
}
#endif

#endif // NITROWIFI_SITEDEFS_H_
