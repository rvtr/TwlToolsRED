/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - demos - netconnect
  File:     sitedefs.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2007-10-30#$
  $Rev: 74 $
  $Author: adachi_hiroaki $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nitroWiFi.h>
#include "sitedefs.h"

/* 
   c:/twlsdk/include/nitro/env/env_system.h
*/

BOOL ENV_SetBinary2(const char *name, void *ptr, u32 length)
{
  ENVResource *resSetPtr;
  ENVResource *p = ENVi_Search(name, &resSetPtr);
  u16 len = 1;

  if (!p) {
    return FALSE;
    }
    
  if (p->type != ENV_RESTYPE_BINARY) {
    return FALSE;
  }

  len = (u16)(p->len - ((p->type == ENV_RESTYPE_STRING) ? 1 : 0));
  OS_TPrintf("env bin len = %d\n", len);


  if (p->type & ENV_RESTYPE_OFFSET_MASK)
    {
      //      return (void *)((u32)resSetPtr + (u32)(p->ptr));
      //      (char *)((u32)resSetPtr + (u32)(p->ptr)) = val;
      STD_CopyMemory( (void *)((u32)resSetPtr + (u32)(p->ptr)) , ptr, length );
    }
  else
    {
      //      return p->ptr;
      // (char *)p->ptr = val;
      //      STD_CopyMemory( (void *)(p->ptr) , ptr , (u32)len );
      STD_CopyMemory( (void *)(p->ptr) , ptr , length );
    }
  return TRUE;
}

BOOL ENV_SetBinary(const char *name, void *ptr)
{
  ENVResource *resSetPtr;
  ENVResource *p = ENVi_Search(name, &resSetPtr);
  u16 len = 1;

  if (!p) {
    return FALSE;
    }
    
  if (p->type != ENV_RESTYPE_BINARY) {
    return FALSE;
  }

  len = (u16)(p->len - ((p->type == ENV_RESTYPE_STRING) ? 1 : 0));
  OS_TPrintf("env bin len = %d\n", len);


  if (p->type & ENV_RESTYPE_OFFSET_MASK)
    {
      //      return (void *)((u32)resSetPtr + (u32)(p->ptr));
      //      (char *)((u32)resSetPtr + (u32)(p->ptr)) = val;
      STD_CopyMemory( (void *)((u32)resSetPtr + (u32)(p->ptr)) , ptr, (u32)STD_StrLen(ptr) );
    }
  else
    {
      //      return p->ptr;
      // (char *)p->ptr = val;
      //      STD_CopyMemory( (void *)(p->ptr) , ptr , (u32)len );
      STD_CopyMemory( (void *)(p->ptr) , ptr , (u32)STD_StrLen(ptr) );
    }
  return TRUE;
}


BOOL ENV_SetString(const char *name, char *str)
{
  ENVResource *resSetPtr;
  ENVResource *p = ENVi_Search(name, &resSetPtr);
  u16 len = 1;

  if (!p) {
    return FALSE;
    }
    
  if (p->type != ENV_RESTYPE_STRING) {
    return FALSE;
  }

  len = (u16)(p->len - ((p->type == ENV_RESTYPE_STRING) ? 1 : 0));
  OS_TPrintf("env str len = %d\n", len);


  if (p->type & ENV_RESTYPE_OFFSET_MASK)
    {
      //      return (void *)((u32)resSetPtr + (u32)(p->ptr));
      //      (char *)((u32)resSetPtr + (u32)(p->ptr)) = val;
      STD_CopyMemory( (void *)((u32)resSetPtr + (u32)(p->ptr)) , (void *)str, (u32)STD_StrLen(str) );
    }
  else
    {
      //      return p->ptr;
      // (char *)p->ptr = val;
      STD_CopyMemory( (void *)(p->ptr) , (void *)str , (u32)STD_StrLen(str) );
    }
  return TRUE;
}



BOOL ENV_SetU8(const char *name, u8 val)
{
  ENVResource *resSetPtr;
  ENVResource *p = ENVi_Search(name, &resSetPtr);
  u16 len;

  if (!p) {
    return FALSE;
  }

  len = (u16)(p->len - ((p->type == ENV_RESTYPE_STRING) ? 1 : 0));
  OS_TPrintf("env u8 len = %d\n", len);


  //      return p->ptr;
  p->ptr = (u8 *)val;
  return TRUE;
}




#if 0
void SitedefsSetWepMode()
{
  ENVResource *ap_wepmode = ENV_Search("WiFi.LAN.1.AP.1.WEP.MODE"); /* U8 */
  "WiFi.LAN.1.AP.1.WEP.KEY" // BINARY -> ENV_GetBinary(const char *name, void **retPtr)
    // BOOL ENV_GetBinaryAndSize(const char *name, void **retPtr, int *size)

    "WiFi.LAN.1.AP.1.ESSID" // STRING
    static inline BOOL ENV_GetString(const char *name, char **retPtr)
    static inline BOOL ENV_GetStringAndLength(const char *name, char **retPtr, int *len)
}
#endif

/* *INDENT-OFF* */
static ENVResource myResources[] =
{
    //-----------------------------
    // LAN
    //-----------------------------

    // LAN:Access Point 1
    // Demos in TwlWiFi use this settings by default.
    { "WiFi.LAN.1.AP.1.ISVALID",    ENV_BOOL  ( TRUE )                       },
    { "WiFi.LAN.1.AP.1.DESC",       ENV_STRING( "Access Point for Demos" )   },
    { "WiFi.LAN.1.AP.1.ROUTERMODE", ENV_BOOL  ( TRUE )                       },
    { "WiFi.LAN.1.AP.1.ESSID",      ENV_STRING( "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
						"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" )  }, /* 最大３２バイト */
    { "WiFi.LAN.1.AP.1.BSSID",      ENV_BINARY( "\xff\xff\xff\xff\xff\xff" ) },
    { "WiFi.LAN.1.AP.1.AUTHMODE",   ENV_U32   ( WCM_OPTION_AUTH_OPENSYSTEM ) },
    { "WiFi.LAN.1.AP.1.WEP.MODE",   ENV_U8    ( WCM_WEPMODE_NONE )           },
    { "WiFi.LAN.1.AP.1.WEP.KEYID",  ENV_U8    ( 0 )                          },
    { "WiFi.LAN.1.AP.1.WEP.KEY", /* 最大８０バイト */
        ENV_BINARY("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00") },
#if 0
    // HOST with fixed address on the LAN.1.AP.1
    { "WiFi.LAN.1.AP.1.HOST.1.ISVALID",     ENV_BOOL  ( TRUE )              },
    { "WiFi.LAN.1.AP.1.HOST.1.NAME",        ENV_STRING( "192.168.2.106"  )  },
    { "WiFi.LAN.1.AP.1.HOST.1.NETMASK",     ENV_STRING( "255.255.255.0"  )  },
    { "WiFi.LAN.1.AP.1.HOST.1.GATEWAY",     ENV_STRING( "192.168.2.1"    )  },
    { "WiFi.LAN.1.AP.1.HOST.1.DNS1",        ENV_STRING( "192.168.2.1"    )  },
    { "WiFi.LAN.1.AP.1.HOST.1.DNS2",        ENV_STRING( "0.0.0.0"        )  },
#else
    // HOST with fixed address on the LAN.1.AP.1
    { "WiFi.LAN.1.AP.1.HOST.1.ISVALID",     ENV_BOOL  ( TRUE )              },
    { "WiFi.LAN.1.AP.1.HOST.1.NAME",        ENV_STRING( "10.101.11.91"   )  },
    { "WiFi.LAN.1.AP.1.HOST.1.NETMASK",     ENV_STRING( "255.255.255.0"  )  },
    { "WiFi.LAN.1.AP.1.HOST.1.GATEWAY",     ENV_STRING( "10.101.11.230"  )  },
    { "WiFi.LAN.1.AP.1.HOST.1.DNS1",        ENV_STRING( "10.100.11.1"    )  },
    { "WiFi.LAN.1.AP.1.HOST.1.DNS2",        ENV_STRING( "10.100.11.5"    )  },

#endif
    // LAN:HTTP Server
    { "WiFi.LAN.1.HTTP.1.ISVALID",   ENV_BOOL  ( TRUE )                     },
    { "WiFi.LAN.1.HTTP.1.DESC",      ENV_STRING( "linux server" )           },
    { "WiFi.LAN.1.HTTP.1.NAME",      ENV_STRING( "192.168.2.2" )            },
    { "WiFi.LAN.1.HTTP.1.PORT.1",    ENV_U16   ( 80 )                       },

    // LAN:ECHO Server
    { "WiFi.LAN.1.ECHO.1.ISVALID",   ENV_BOOL  ( TRUE )                     },
    { "WiFi.LAN.1.ECHO.1.DESC",      ENV_STRING( "linux server" )           },
    { "WiFi.LAN.1.ECHO.1.NAME",      ENV_STRING( "192.168.2.2" )            },
    { "WiFi.LAN.1.ECHO.1.PORT.1",    ENV_U16   ( 7 )                        },

    // LAN:CHARGEN Server
    { "WiFi.LAN.1.CHARGEN.1.ISVALID",ENV_BOOL  ( TRUE )                     },
    { "WiFi.LAN.1.CHARGEN.1.DESC",   ENV_STRING( "linux server" )           },
    { "WiFi.LAN.1.CHARGEN.1.NAME",   ENV_STRING( "192.168.2.2" )            },
    { "WiFi.LAN.1.CHARGEN.1.PORT.1", ENV_U16   ( 19 )                       },

    // LAN:Ping Taret
    { "WiFi.LAN.1.PING.1.ISVALID",   ENV_BOOL  ( TRUE )                     },
    { "WiFi.LAN.1.PING.1.DESC",      ENV_STRING( "default gateway" )        },
    { "WiFi.LAN.1.PING.1.NAME",      ENV_STRING( "192.168.2.1" )            },

    // LAN:FTP Server
    { "WiFi.LAN.1.FTP.1.ISVALID",    ENV_BOOL  ( TRUE )                     },
    { "WiFi.LAN.1.FTP.1.DESC",       ENV_STRING( "linux server" )           },
    { "WiFi.LAN.1.FTP.1.NAME",       ENV_STRING( "192.168.2.2" )            },
    { "WiFi.LAN.1.FTP.1.PORT.1",     ENV_U16   ( 21 )                       },

    //---- end mark
    ENV_RESOURCE_END,
};

ENVResource* resourceArray[]=
{
   myResources, NULL
};
/* *INDENT-ON* */

/*---------------------------------------------------------------------------*
  Name:         SiteDefs_Init

  Description:  main

  Arguments:    None

  Returns:      None
 *---------------------------------------------------------------------------*/
void SiteDefs_Init(void)
{
    //---- declaration of using ENV system
    //SDK と WIFI の両方に ENV ライブラリが入っているため、以下のように切り替える
    //SDKがENVの旧versionを持っているなら、それに合わせて ENV_Init は昔版で。
#if (SDK_VERSION_DATE <= 20050918) && (SDK_VERSION_DATE >= 20050908)
    ENV_Init(myResources);
#else
    //SDKにENVの旧versionは含まれていないので、それに合わせて ENV_Init は改良版で。
    ENV_Init();
#endif
}

/*====== End of sitedefs.c ======*/
