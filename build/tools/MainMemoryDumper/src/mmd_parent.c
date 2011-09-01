/*---------------------------------------------------------------------------*
  Project:  MainMemoryDumper
  File:     main.c

  Copyright 2011 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date::            $
  $Rev$
  $Author$
 *---------------------------------------------------------------------------*/

#include <twl.h>
#include <twl/os/common/format_rom.h>
#include "wh.h"
#include "misc_simple.h"
#include "mmd_parent.h"

// extern data-----------------------------------------------------------------

// define data-----------------------------------------------------------------

// function's prototype-------------------------------------------------------
static u16 ParentWEPKeyGenerator(u16 *wepkey, const WMParentParam *parentParam);
static void ParentReceiveCallback(u16 aid, u16 *data, u16 length);
static void ParentSendCallback(u16 *data, u16 length);
static void whSessionUpdateCallback(void* arg);

// global variable-------------------------------------------------------------

// static variable-------------------------------------------------------------
static MMDPStage stage;
static u16 tgid;
static UserGameInfo userGameInfo;
static RequestFormat requestForPrint;

// const data------------------------------------------------------------------


// ============================================================================
// function's description
// ============================================================================
void printParent(void)
{
    static int count = 0;
    if ( WH_GetSystemState() == WH_SYSSTATE_ERROR )
    {
        myDp_Printf(1, 3, TXT_COLOR_BLACK, MAIN_SCREEN, "Stopped because of an error.");
    }
    else if ( !requestForPrint.addr )
    {
        myDp_Printf(1, 3, TXT_COLOR_BLACK, MAIN_SCREEN, "Waiting to connect");
        switch ( count++ & 0x30 )
        {
        case 0x00: break;
        case 0x10: myDp_Printf(19, 3, TXT_COLOR_BLACK, MAIN_SCREEN, ".");   break;
        case 0x20: myDp_Printf(19, 3, TXT_COLOR_BLACK, MAIN_SCREEN, "..");  break;
        case 0x30: myDp_Printf(19, 3, TXT_COLOR_BLACK, MAIN_SCREEN, "..."); break;
        }
    }
    else if ( stage == MMDP_STAGE_END )
    {
        myDp_Printf(1, 3, TXT_COLOR_BLACK, MAIN_SCREEN, "Done.");
    }
    else
    {
        myDp_Printf(1, 3, TXT_COLOR_BLACK, MAIN_SCREEN, "Sending");
        myDp_Printf(3, 4, TXT_COLOR_RED, MAIN_SCREEN, "%08X-%08X", requestForPrint.addr, requestForPrint.addr + requestForPrint.length - 1);
        switch ( count++ & 0x30 )
        {
        case 0x00: break;
        case 0x10: myDp_Printf(8, 3, TXT_COLOR_BLACK, MAIN_SCREEN, ".");    break;
        case 0x20: myDp_Printf(8, 3, TXT_COLOR_BLACK, MAIN_SCREEN, "..");   break;
        case 0x30: myDp_Printf(8, 3, TXT_COLOR_BLACK, MAIN_SCREEN, "...");  break;
        }
    }
}

void actionParent(void)
{
    switch (stage)
    {
    case MMDP_STAGE_INIT:
        RTC_Init();
        requestForPrint.addr = 0;
        requestForPrint.length = 0;
        WH_SetSessionUpdateCallback( whSessionUpdateCallback );
        if ( !WH_Initialize() )
        {
            printWMError("WH_Initialize() was failed");
            return;
        }
        else
        {
            stage = MMDP_STAGE_WAIT_INIT;
        }
        break;
    case MMDP_STAGE_WAIT_INIT:
        if ( WH_GetSystemState() != WH_SYSSTATE_BUSY )
        {
            stage = MMDP_STAGE_MEASURE_CHANNEL;
        }
        break;
    case MMDP_STAGE_MEASURE_CHANNEL:
        WH_SetReceiver(ParentReceiveCallback);
        WH_SetParentWEPKeyGenerator(ParentWEPKeyGenerator);
        {   // WEP
            MATHRandContext32 sRand;
            u64     randSeed = 0;
            RTCDate date;
            RTCTime time;
            if (RTC_GetDateTime(&date, &time) == RTC_RESULT_SUCCESS)
            {
                randSeed =
                    (((((((u64)date.year * 16ULL + date.month) * 32ULL) + date.day) * 32ULL +
                       time.hour) * 64ULL + time.minute) * 64ULL + time.second);
            }
            MATH_InitRand32(&sRand, randSeed);
            userGameInfo.wepSeed = MATH_Rand32(&sRand, 0);
            WH_SetUserGameInfo((u16 *)&userGameInfo, 4);
        }
        if ( !WH_StartMeasureChannel() )
        {
            printWMError("WH_StartMeasureChannel() was failed");
        }
        else
        {
            stage = MMDP_STAGE_WAIT_MEASURE_CHANNEL;
        }
        break;
    case MMDP_STAGE_WAIT_MEASURE_CHANNEL:
        if ( WH_GetSystemState() == WH_SYSSTATE_MEASURECHANNEL )
        {
            stage = MMDP_STAGE_START_PARENT;
        }
        break;
    case MMDP_STAGE_START_PARENT:
        if ( !WH_ParentConnect(WH_CONNECTMODE_MP_PARENT, ++tgid, WH_GetMeasureChannel()) )
        {
            printWMError("WH_ParentConnect() was failed");
        }
        else
        {
            stage = MMDP_STAGE_WAIT_START_PARENT;
        }
        break;
    case MMDP_STAGE_WAIT_START_PARENT:
        if ( WH_GetSystemState() == WH_SYSSTATE_CONNECTED )
        {
            stage = MMDP_STAGE_START_MP;
        }
        break;
    case MMDP_STAGE_START_MP:
        if ( WH_GetSystemState() == WH_SYSSTATE_ERROR )
        {
            printWMError("Some error occurred.");
        }
        break;
    case MMDP_STAGE_END:
        break;
    }
}

static u16 ParentWEPKeyGenerator(u16 *wepkey, const WMParentParam *parentParam)
{
    u16     data[20 / sizeof(u16)];
    u16     tmpWep[20 / sizeof(u16)];
    u8      macAddress[WM_SIZE_MACADDR];
    UserGameInfo* userGameInfo = (UserGameInfo*)parentParam->userGameInfo;

    OS_GetMacAddress(macAddress);
    MI_CpuClear16(data, sizeof(data));

    // 前もって WH_ParentConnect 関数の直前に WH_SetUserGameInfo 関数で
    // UserGameInfo に設定しておいた Seed 値を WEP Key に反映させる
    MI_CpuCopy16(&userGameInfo->wepSeed, data, 4);
    // 同じアルゴリズムを異なるアプリケーションで使いまわせるように
    // GGID も WEP Key に反映させる
    MI_CpuCopy16(&parentParam->ggid, data + 2, sizeof(u32));
    *(u16 *)(data + 4) = parentParam->tgid;
    // 親機が異なれば WEP Key も異なるように MAC Address も WEP Key に反映させる
    MI_CpuCopy8(macAddress, (u8 *)(data + 5), WM_SIZE_MACADDR);

    // HMAC-SHA-1 で鍵付ハッシュ値を取り、128bit に切り詰めて WEP Key とする。
    // HMAC で使用する鍵はアプリケーション固有のものとすること。
    // (HMAC に関しては RFC2104 参照)
    MATH_CalcHMACSHA1(tmpWep, data, sizeof(data), SECRET_KEY, SECRET_KEY_LENGTH);
    MI_CpuCopy8(tmpWep, wepkey, 16);
    OS_TPrintf("wepkey: %04x%04x %04x%04x %04x%04x %04x%04x\n", wepkey[0], wepkey[1],
               wepkey[2], wepkey[3], wepkey[4], wepkey[5], wepkey[6], wepkey[7]);

    return WM_WEPMODE_128BIT;
}

static void ParentReceiveCallback(u16 aid, u16 *data, u16 length)
{
    OS_TPrintf("ParentReceiveCallback: %d, 0x%x, %d\n", aid, data, length);
    if ( aid != 1 )
    {
        printWMError("Unsupported AID");
    }
    if ( data == NULL )
    {
        OS_TPrintf("Received NO data.\n");
        return;
    }
    if ( length != sizeof(RequestFormat) )
    {
        OS_TPrintf("Received invalid length.\n");
        return;
    }
    ParentSendCallback(data, length);
}

static void ParentSendCallback(u16 *data, u16 length)
{
    RequestFormat* request = (RequestFormat*)data;
    (void)length;
    OS_TPrintf("ParentSendCallback: 0x%x, %d (addr=%x, length=%d)\n", data, length, request->addr, request->length);

    // 先頭が範囲外ならエラー
    if ( (u32)request->addr < BEGIN_ADDRESS )
    {
        printWMError("Unsupported data address from child.");
    }
    else
    {
        // 終端が範囲外なら丸めて送信
        u16 len = (u32)request->addr + request->length < END_ADDRESS ? request->length : (u16)(END_ADDRESS - (u32)request->addr);
        requestForPrint.addr = request->addr;
        requestForPrint.length = len;
        WH_SendData(request->addr, len, NULL);
        OS_TPrintf("WH_SendData: addr=%x, length=%d\n", request->addr, len);
    }
}

static void whSessionUpdateCallback(void* arg)
{
    switch ( ((WMCallback*)arg)->apiid )
    {
    case WM_APIID_START_MP:
        {
            WMStartMPCallback *cb = (WMStartMPCallback *)arg;
            if ( cb->state == WM_STATECODE_MP_START )
            {
                OS_TPrintf("whSessionUpdateCallback: WM_APIID_START_MP: WM_STATECODE_MP_START: %d\n", cb->errcode);
            }
        }
        break;
    case WM_APIID_START_PARENT:
        {
            WMStartParentCallback* cb = (WMStartParentCallback*)arg;
            switch ( cb->state )
            {
            case WM_STATECODE_CONNECTED:
                OS_TPrintf("whSessionUpdateCallback: WM_APIID_START_PARENT: WM_STATE_CODE_CONNECTED: %d\n", cb->aid);
                break;
            case WM_STATECODE_DISCONNECTED:
                OS_TPrintf("whSessionUpdateCallback: WM_APIID_START_PARENT: WM_STATE_CODE_DISCONNECTED: %d\n", cb->aid);
                if ( (u32)requestForPrint.addr + requestForPrint.length >= END_ADDRESS )
                {
                    mmdumperEnd();
                    stage = MMDP_STAGE_END;
                }
                break;
            }
        }
        break;
    case WM_APIID_SET_MP_DATA:
        {
            WMPortSendCallback* cb = (WMPortSendCallback*)arg;
            if ( cb->errcode )
            {
                OS_TPrintf("whSessionUpdateCallback: WM_APIID_SET_MP_DATA: %d\n", cb->errcode);
            }
            else
            {
                OS_TPrintf("whSessionUpdateCallback: WM_APIID_SET_MP_DATA: %d %d %d \n", cb->destBitmap, cb->restBitmap, cb->sentBitmap);
            }
        }
        break;
    default:
        OS_TPrintf("whSessionUpdateCallback: apiid=%d\n", ((WMCallback*)arg)->apiid);
        break;
    }
}
