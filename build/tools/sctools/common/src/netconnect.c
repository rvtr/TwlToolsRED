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

static int     ncStartWiFi(void);
static void     ncFinishWiFi(void);

static WcmControlApInfo apInfo;
static s32  previousAddr = 0;
static u8   g_deviceId    = WCM_DEVICEID_DEFAULT;
static BOOL g_started   = FALSE;


static int nc_error_code = 0;


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

#if 0
typedef struct
{
    BOOL    use_dhcp;           // if TRUE, use dhcp
    struct
    {
        SOCLInAddr  my_ip;
        SOCLInAddr  net_mask;
        SOCLInAddr  gateway_ip;
        SOCLInAddr  dns_ip[2];
    } host_ip;
    void*  (*alloc) (u32);
    void (*free) (void*);

    u32     cmd_packet_max;     // コマンドパケットの最大数
    u32     lan_buffer_size;    // 0 なら *alloc で自力確保
    void*   lan_buffer;         // 0 なら default(16384)設定

    // CPS スレッドの優先度
    // 0 なら SOCL_CPS_SOCKET_THREAD_PRIORITY
    s32     cps_thread_prio;

    // 通信バッファサイズ
    s32     mtu;                //default 1500
    s32     rwin;
} SOCLConfig;

#endif

static int ncStartWiFi(void)
{
    int result;
    int timeout_counter;

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




	socl_config.use_dhcp = GetDhcpMODE();
	socl_config.host_ip.gateway_ip = GetGateway();
	socl_config.host_ip.net_mask = GetNetmask();
	socl_config.host_ip.my_ip =  GetIPAddr();
	socl_config.host_ip.dns_ip[0] = GetDNS1();
	socl_config.host_ip.dns_ip[1] = GetDNS2();
	
        
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
        OS_TPrintf("SOC_Startup failed (%d)", result);
        mprintf("SOC_Startup failed (%d)", result);
	return -2;
    }
    g_started = TRUE;
    OS_TPrintf("DHCP....\n");

    timeout_counter = 0;

    while (SOC_GetHostID() == 0)
    {
      s32 err_code = SOC_GetConfigError(NULL);
      if( err_code == SOC_IP_ERR_DHCP_TIMEOUT ) {
	mprintf("%s -dhcp timeout\n",__FUNCTION__);
	return NC_ERROR_TIMEOUT;
      }
      else if(err_code == SOC_IP_ERR_LINK_DOWN ) {
	mprintf("%s -link down\n",__FUNCTION__);
	return NC_ERROR_LINKDOWN;
      }
      else {
	OS_Sleep(100);
	timeout_counter++;
	if( timeout_counter > (60 * 1000 / 100) ) {
	  mprintf("%s -timeout\n",__FUNCTION__);
	  return NC_ERROR_TIMEOUT;
	}
      }
    }

    OS_TPrintf("IP addr = %3d.%3d.%3d.%3d\n", CPS_CV_IPv4(CPSMyIp));
    OS_TPrintf("NetMask = %3d.%3d.%3d.%3d\n", CPS_CV_IPv4(CPSNetMask));
    OS_TPrintf("GW addr = %3d.%3d.%3d.%3d\n", CPS_CV_IPv4(CPSGatewayIp));
    OS_TPrintf("DNS[0]  = %3d.%3d.%3d.%3d\n", CPS_CV_IPv4(CPSDnsIp[0]));
    OS_TPrintf("DNS[1]  = %3d.%3d.%3d.%3d\n", CPS_CV_IPv4(CPSDnsIp[1]));
    return 0;
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



/*---------------------------------------------------------------------------*
  Name        : NcStart
  Description : 
  Arguments   : apClass     - SitDefsに定義されているアクセスポイントのクラス名
  Returns     : 
 *---------------------------------------------------------------------------*/
int NcStart(const char* apClass)
{
  int counter = 0;
  s32 wcm_phase;
  int len;
  u8 key_bin_buf[MAX_KEY_BIN_BUF];
  int timeout_counter;
  int sec = 0;

  SiteDefs_Init();

  if( TRUE == GetKeyModeStr() ) {
    if( FALSE == ENV_SetBinary("WiFi.LAN.1.AP.1.WEP.KEY", (void *)GetWlanKEYSTR()) ) {
      OS_TPrintf("Error %s %d\n", __FUNCTION__,__LINE__);
    }
  }
  else {
    STD_MemSet((void *)key_bin_buf, 0, MAX_KEY_BIN_BUF );
    len = GetWlanKEYBIN(key_bin_buf);
    if( len ) {
      if( FALSE == ENV_SetBinary2("WiFi.LAN.1.AP.1.WEP.KEY", (void *)key_bin_buf, (u32)len) ) {
	OS_TPrintf("Error %s %d\n", __FUNCTION__,__LINE__);
      }
    }
  }

  if( FALSE == ENV_SetString("WiFi.LAN.1.AP.1.ESSID", GetWlanSSID()) ) {
    OS_TPrintf("Error %s %d\n", __FUNCTION__,__LINE__);
  }
 
  if( FALSE == ENV_SetU8("WiFi.LAN.1.AP.1.WEP.MODE", (u8)GetWlanMode() ) ) {
    OS_TPrintf("Error %s %d\n", __FUNCTION__,__LINE__);
  }
 
  if (!InitWcmApInfo(&apInfo, apClass)) {
    mprintf("Invalid AP Class....");
    OS_TPrintf("Invalid AP Class....");
    return  NC_ERROR_INVALID_AP_CLASS;
  }

  timeout_counter = 0;
  while (1) {
    wcm_phase = WCM_GetPhase();
    if( wcm_phase == WCM_PHASE_NULL) {
      break;
    }
    timeout_counter++;
    if( timeout_counter > (60 * 1000 / 100) ) {
      mprintf("%s -timeout\n",__FUNCTION__);
      return NC_ERROR_TIMEOUT;
    }
    OS_Sleep(100);
   
  }

  InitWcmControlByApInfoEx(&apInfo, g_deviceId);
 
  OS_TPrintf("LINK UP....\n");
  mprintf("-LINK UP");

  timeout_counter = 0;


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

#define TIMEOUT_SEC 60
    switch( counter ) {
    case 0:
      mprintf("\r-LINK UP.      %02d/%02d",sec,TIMEOUT_SEC);
      if( timeout_counter > (TIMEOUT_SEC * 1000 / 200) ) {
	goto error_ret;
      }
      break;
    case 1:
      mprintf("\r-LINK UP..     %02d/%02d",sec,TIMEOUT_SEC);
      if( timeout_counter > (TIMEOUT_SEC * 1000 / 200) ) {
	goto error_ret;
      }
      break;
    case 2:
      mprintf("\r-LINK UP...    %02d/%02d",sec,TIMEOUT_SEC);
      if( timeout_counter > (TIMEOUT_SEC * 1000 / 200) ) {
	goto error_ret;
      }
      break;
    case 3:
      mprintf("\r-LINK UP....   %02d/%02d",sec,TIMEOUT_SEC);
      if( timeout_counter > (TIMEOUT_SEC * 1000 / 200) ) {
	goto error_ret;
      }
      break;
    case 5:
      mprintf("\r-LINK UP.....  %02d/%02d",sec,TIMEOUT_SEC);
      if( timeout_counter > (TIMEOUT_SEC * 1000 / 200) ) {
	goto error_ret;
      }
      break;
    case 6:
      mprintf("\r-LINK UP...... %02d/%02d",sec,TIMEOUT_SEC);
      counter = -1;
      if( timeout_counter > (TIMEOUT_SEC * 1000 / 200) ) {
	goto error_ret;
      }
      break;
    }

    timeout_counter++;

    sec = timeout_counter*200/1000;

    OS_Sleep(200);
    counter++;
  }

  m_set_palette(tc[0], M_TEXT_COLOR_GREEN );
  mprintf("         OK.\n");
  m_set_palette(tc[0], M_TEXT_COLOR_WHITE );

 
  return ncStartWiFi();

 error_ret:
  m_set_palette(tc[0], M_TEXT_COLOR_RED );
  mprintf("         NG.\n");
  m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  return NC_ERROR_TIMEOUT;

}

void NcFinish()
{

    ncFinishWiFi();
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
