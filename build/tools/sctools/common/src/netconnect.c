/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - demos - netconnect
  File:     netconnect.c

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-08-06#$
  $Rev: 880 $
  $Author: adachi_hiroaki $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include <nitroWiFi.h>

#include "sitedefs.h"
#include "wcm_control.h"
#include "netconnect.h"
#include "mywlan.h"

#include        "text.h"
#include        "mprintf.h"
#include        "logprintf.h"


/*---------------------------------------------------------------------------*
   初期化
 *---------------------------------------------------------------------------*/
static void     Heap_Setup(void);
#ifdef SDK_TWL
static void*    myAlloc_SOCL(u32 size);
static void     myFree_SOCL(void* ptr);
#endif // SDK_TWL

static void     ncStartWiFi(void);
static void     ncFinishWiFi(void);
static void     ncStartInet(void);
static void     ncFinishInet(void);

static WcmControlApInfo apInfo;
static s32  previousAddr = 0;
static u8   g_deviceId    = WCM_DEVICEID_DEFAULT;
static BOOL g_started   = FALSE;

/*---------------------------------------------------------------------------*
  Name        : NcGlobalInit
  Description : 
  Arguments   : なし。
  Returns     : 
 *---------------------------------------------------------------------------*/
void NcGlobalInit()
{
    OS_Init();
    OS_InitTick();
    OS_InitAlarm();
    RTC_Init();
    Heap_Setup();
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();
}

#ifndef SDK_WIFI_INET
static void ncStartWiFi(void)
{
    int result;
    if (previousAddr != 0)
    {
        /*
           DHCPサーバに前回取得したアドレスを要求するように設定する。
           アドレスプールを使い切らないようにするために必要。
         */
        SOCL_SetRequestedIP(SOC_NtoHl(previousAddr));
    }

#ifdef SDK_TWL
    if (OS_IsRunOnTwl())
    {
        static SOCLConfig  socl_config;
        MI_CpuClear8(&socl_config, sizeof(socl_config));
        
        socl_config.alloc = myAlloc_SOCL;
        socl_config.free = myFree_SOCL;
        socl_config.use_dhcp = TRUE;
        socl_config.cmd_packet_max = SOCL_CMDPACKET_MAX;
        socl_config.lan_buffer_size = SOCL_LAN_BUFFER_SIZE_DEFAULT * 8;
        socl_config.mtu = 1400;
        socl_config.rwin = 65535;
        
        OS_TPrintf("SOCL_Startup....\n");
        result = SOCL_Startup(&socl_config);
    }
    else
#endif // SDK_TWL
    {
        SOCConfig   soc_config;
        soc_config.alloc = NcAlloc;
        soc_config.free = NcFree;
        soc_config.flag = SOC_FLAG_DHCP;
        soc_config.mtu = 0;
        soc_config.rwin = 0;

        OS_TPrintf("SOC_Startup....\n");
        result = SOC_Startup(&soc_config);
    }
    if (result < 0)
    {
        OS_Panic("SOC_Startup failed (%d)", result);
    }
    g_started = TRUE;
    OS_TPrintf("DHCP....\n");

    while (SOC_GetHostID() == 0)
    {
        OS_Sleep(100);
    }

    OS_TPrintf("IP addr = %3d.%3d.%3d.%3d\n", CPS_CV_IPv4(CPSMyIp));
    OS_TPrintf("NetMask = %3d.%3d.%3d.%3d\n", CPS_CV_IPv4(CPSNetMask));
    OS_TPrintf("GW addr = %3d.%3d.%3d.%3d\n", CPS_CV_IPv4(CPSGatewayIp));
    OS_TPrintf("DNS[0]  = %3d.%3d.%3d.%3d\n", CPS_CV_IPv4(CPSDnsIp[0]));
    OS_TPrintf("DNS[1]  = %3d.%3d.%3d.%3d\n", CPS_CV_IPv4(CPSDnsIp[1]));
}

static void ncFinishWiFi(void)
{
    previousAddr = SOC_GetHostID();
    if (g_started)
    {
        (void)SOC_Cleanup();
        g_started = FALSE;
    }
}

#else // SDK_WIFI_INET

static void ncStartInet(void)
{
    int result;
    SOConfig    soConfig =
    {
        SO_VENDOR_NINTENDO,     // vendor
        SO_VERSION,             // version

        NcAlloc,                // alloc
        NcFree,                 // free

        SO_FLAG_DHCP,           // flag
        SO_HtoNl(SO_INADDR_ANY),// addr
        SO_HtoNl(SO_INADDR_ANY),// netmask
        SO_HtoNl(SO_INADDR_ANY),// router
        SO_HtoNl(SO_INADDR_ANY),// dns1
        SO_HtoNl(SO_INADDR_ANY),// dns1
        4096,                   // timeWaitBuffer
        4096,                   // reassemblyBuffer
        0,                      // maximum transmission unit size

        // TCP
        16384,                  // default TCP receive window size (default 2 x MSS)
        0,                      // default TCP total retransmit timeout value (default 100 sec)

        // PPP      PPP関連機能は未実装
        NULL,
        NULL,

        // PPPoE    PPP関連機能は未実装
        NULL,

        // DHCP
        "NINTENDO-DS",          // DHCP host name
        4,                      // TCP total retransmit times (default 4)

        // UDP
        0,                      // default UDP send buffer size (default 1472)
        0                       // defualt UDP receive buffer size (default 4416)
    };

#ifdef SDK_TWL
    if (OS_IsRunOnTwl())
    {
        soConfig.rwin = 65535;
    }
#endif // SDK_TWL

    OS_TPrintf("SO_Startup....\n");
    result = SO_Startup(&soConfig);
    if (result < 0)
    {
        OS_Panic("SO_Startup failed (%d)", result);
    }
    g_started = TRUE;
    
    OS_TPrintf("DHCP....\n");

    while (SO_GetHostID() == 0 && IP_GetConfigError(NULL) == 0)
    {
        OS_Sleep(100);
    }

    if (SO_GetHostID() != 0)
    {
        int retry = 0;
		u8 ip[ IP_ALEN ];

		IP_GetAddr( NULL, ip );
		OS_Printf("IP addr = %3d.%3d.%3d.%3d\n", ip[0], ip[1], ip[2], ip[3] );
		IP_GetNetmask ( NULL, ip );
		OS_Printf("NetMask = %3d.%3d.%3d.%3d\n", ip[0], ip[1], ip[2], ip[3] );
		IP_GetGateway ( NULL, ip );
		OS_Printf("GW addr = %3d.%3d.%3d.%3d\n", ip[0], ip[1], ip[2], ip[3] );
    }
    else
    {
        OS_TPrintf("NO DHCP SERVER or NO LINK....\n");

        switch (IP_GetConfigError(NULL))
        {
        case IP_ERR_DHCP_TIMEOUT:
            OS_Panic("IP_ERR_DHCP_TIMEOUT\n");
            break;

        case IP_ERR_LINK_DOWN:
            OS_Panic("IP_ERR_LINK_DOWN\n");
            break;

        default:
            OS_Panic("Default???\n");
            break;
        }
    }

}

static void ncFinishInet(void)
{
    previousAddr = SO_GetHostID();
    if (g_started)
    {
        (void)SO_Cleanup();
        g_started = FALSE;
    }
}

#endif // SDK_WIFI_INET

/*---------------------------------------------------------------------------*
  Name        : NcStart
  Description : 
  Arguments   : apClass     - SitDefsに定義されているアクセスポイントのクラス名
  Returns     : 
 *---------------------------------------------------------------------------*/
void NcStart(const char* apClass)
{
  int counter = 0;
  s32 wcm_phase;

  SiteDefs_Init();

    if( FALSE == ENV_SetBinary("WiFi.LAN.1.AP.1.WEP.KEY", (void *)GetWlanKEYSTR()) ) {
      OS_TPrintf("Error %s %d\n", __FUNCTION__,__LINE__);
    }

    if( FALSE == ENV_SetString("WiFi.LAN.1.AP.1.ESSID", GetWlanSSID()) ) {
      OS_TPrintf("Error %s %d\n", __FUNCTION__,__LINE__);
    }

    if( FALSE == ENV_SetU8("WiFi.LAN.1.AP.1.WEP.MODE", (u8)GetWlanMode() ) ) {
      OS_TPrintf("Error %s %d\n", __FUNCTION__,__LINE__);
    }

    if (!InitWcmApInfo(&apInfo, apClass))
    {
        OS_Panic("Invalid AP Class....");
    }

    while (1) {
      wcm_phase = WCM_GetPhase();
      if( wcm_phase == WCM_PHASE_NULL) {
	break;
      }
      // OS_TPrintf("%s %d phase = %d\n", __FUNCTION__,__LINE__,wcm_phase);
      OS_Sleep(100);
    }

    InitWcmControlByApInfoEx(&apInfo, g_deviceId);

    OS_TPrintf("LINK UP....\n");
    mprintf("-LINK UP");
    while ( 1 ) {
      wcm_phase = WCM_GetPhase();
      if( wcm_phase == WCM_PHASE_DCF ) {
	break;
      }
#if 0
#define WCM_PHASE_NULL              0               // 初期化前
#define WCM_PHASE_WAIT              1               // 初期化直後の状態( 要求待ち )
#define WCM_PHASE_WAIT_TO_IDLE      2               // 初期化直後の状態 から 無線機能の起動シーケンス中
#define WCM_PHASE_IDLE              3               // 無線機能アイドル状態
#define WCM_PHASE_IDLE_TO_WAIT      4               // アイドル状態 から 無線機能の停止シーケンス中
#define WCM_PHASE_IDLE_TO_SEARCH    5               // アイドル状態 から AP 自動探索状態への移行シーケンス中
#define WCM_PHASE_SEARCH            6               // AP 自動探索状態
#define WCM_PHASE_SEARCH_TO_IDLE    7               // AP 自動探索状態 から アイドル状態への移行シーケンス中
#define WCM_PHASE_IDLE_TO_DCF       8               // アイドル状態 から AP への無線接続シーケンス中
#define WCM_PHASE_DCF               9               // AP と無線接続された DCF 通信可能状態
#define WCM_PHASE_DCF_TO_IDLE       10              // DCF 通信状態 から 無線接続を切断するシーケンス中
#define WCM_PHASE_FATAL_ERROR       11              // 復旧不可能なエラーが発生し、全ての処理が受け付けられない状態
#define WCM_PHASE_IRREGULAR         12              // 状態遷移シーケンスの途中で問題が発生した状態
#define WCM_PHASE_TERMINATING       13              // WCM ライブラリの強制停止シーケンス中
#endif

      switch( counter ) {
      case 0:
	mprintf("\r-LINK UP.     ");
	break;
      case 1:
	mprintf("\r-LINK UP..    ");
	break;
      case 2:
	mprintf("\r-LINK UP...   ");
	break;
      case 3:
	mprintf("\r-LINK UP....  ");
	break;
      case 5:
	mprintf("\r-LINK UP..... ");
	break;
      case 6:
	mprintf("\r-LINK UP......");
	counter = -1;
	break;
      }
      OS_Sleep(100);
      counter++;
    }

    OS_TPrintf("connected\n");
    mprintf(" connected\n");

#ifndef SDK_WIFI_INET
    ncStartWiFi();
#else // SDK_WIFI_INET
    ncStartInet();
#endif // SDK_WIFI_INET

}

void NcFinish()
{
#ifndef SDK_WIFI_INET
    ncFinishWiFi();
#else // SDK_WIFI_INET
    ncFinishInet();
#endif // SDK_WIFI_INET
}

void NcSetDevice(u8 deviceId)
{
    g_deviceId = deviceId;
}

SDK_WEAK_SYMBOL void* NcAlloc(u32 name, s32 size)
{
#pragma unused(name)
    OSIntrMode  enable = OS_DisableInterrupts();
    void*       ptr = OS_Alloc((u32) size);
    (void)OS_RestoreInterrupts(enable);
    return ptr;
}

SDK_WEAK_SYMBOL void NcFree(u32 name, void* ptr, s32 size)
{
#pragma unused(name, size)
    OSIntrMode  enable = OS_DisableInterrupts();
    OS_Free(ptr);
    (void)OS_RestoreInterrupts(enable);
    return;
}

/*---------------------------------------------------------------------------*
  ヒープ関連
 *---------------------------------------------------------------------------*/

// MainRAM アリーナに対して メモリ割り当てシステム初期化
static void Heap_Setup(void)
{
    void*           nstart;
    OSHeapHandle    handle;

    nstart = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 1);
    OS_SetMainArenaLo(nstart);
    handle = OS_CreateHeap(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi());
    (void)OS_SetCurrentHeap(OS_ARENA_MAIN, handle);
}


#ifdef SDK_TWL
static void* myAlloc_SOCL(u32 size)
{
    s32     msize = (s32) (size + sizeof(s32));
    s32*    m = NcAlloc(0U, msize);

    // 呼び出しに必要なパラメータを領域の先頭に隠しておく
    if (m)
    {
        m[0] = (s32) msize;
        m++;
    }

    return (void*)m;
}

static void myFree_SOCL(void* ptr)
{
    s32*    m = (s32*)ptr;

    // 呼び出しに必要なパラメータを領域の先頭から取り出す
    if (m)
    {
        m--;
        NcFree(0U, (void*)m, m[0]);
    }
}
#endif // SDK_TWL
