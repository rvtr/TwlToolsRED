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
#include "mmd_client.h"

#include <twl/ltdmain_begin.h>

// extern data-----------------------------------------------------------------

// define data-----------------------------------------------------------------

typedef enum SDStat
{
    SDSTAT_INSERTED = 0,
    SDSTAT_REMOVED = 1,
    SDSTAT_MAX
}
SDStat;

// function's prototype-------------------------------------------------------
static u16 ChildWEPKeyGenerator(u16 *wepkey, const WMBssDesc *bssDesc);
static void ChildReceiveCallback(u16 aid, u16 *data, u16 length);
static void ChildSendCallback(void);
static void whSessionUpdateCallback(void* arg);

static void subCallBack( void* userdata, FSEvent event, void* argument );

// global variable-------------------------------------------------------------

// static variable-------------------------------------------------------------
static MMDCStage stage;
static RequestFormat request;

static s32 s_SDCheck = 0;
static FSEventHook s_hook;
static FSFile dumpFile;
static u8 buffer[2][16*1024] ATTRIBUTE_ALIGN(32);
static size_t offset[2];
static BOOL flip;
static BOOL allEnd;

// const data------------------------------------------------------------------
static const u8 ANY_PARENT[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
static const u16 ALL_CHANNEL = 0;

// ============================================================================
// function's description
// ============================================================================
void printClient(void)
{
    static int count = 0;
    if ( WH_GetSystemState() == WH_SYSSTATE_ERROR )
    {
        myDp_Printf(1, 3, TXT_COLOR_BLACK, MAIN_SCREEN, "Stopped because of an error.");
    }
    else if ( WH_GetSystemState() != WH_SYSSTATE_CONNECTED )
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
    else if ( stage == MMDC_STAGE_END )
    {
        myDp_Printf(1, 3, TXT_COLOR_BLACK, MAIN_SCREEN, "Done.");
    }
    else
    {
        myDp_Printf(1, 3, TXT_COLOR_BLACK, MAIN_SCREEN, "Receiving");
        myDp_Printf(3, 4, TXT_COLOR_RED, MAIN_SCREEN, "%08X-%08X", request.addr, request.addr + request.length - 1);
        switch ( count++ & 0x30 )
        {
        case 0x00: break;
        case 0x10: myDp_Printf(10, 3, TXT_COLOR_BLACK, MAIN_SCREEN, ".");   break;
        case 0x20: myDp_Printf(10, 3, TXT_COLOR_BLACK, MAIN_SCREEN, "..");  break;
        case 0x30: myDp_Printf(10, 3, TXT_COLOR_BLACK, MAIN_SCREEN, "..."); break;
        }
    }
}

void actionClient(void)
{
    if ( s_SDCheck == SDSTAT_REMOVED )
    {
        printWMError("SD Card is removed.");
    }

    switch (stage)
    {
    case MMDC_STAGE_INIT:
        FS_Init(3);
        FS_InitFatDriver();
        RTC_Init();
        {   // SDカード
            FSPathInfo info;
            FS_RegisterEventHook("sdmc", &s_hook, subCallBack, NULL);
            if ( !FS_GetPathInfo("sdmc:/", &info) )
            {
                FSResult result = FS_GetArchiveResultCode("sdmc:/");
                printWMError("SD Card is not inserted.");
                OS_TPrintf("FSResult: %d\n", result);
                return;
            }
            if ( info.attributes & FS_ATTRIBUTE_IS_PROTECTED )
            {
                printWMError("SD Card is not writable.");
                return;
            }
        }
        request.addr = (u8*)BEGIN_ADDRESS;
        request.length = 128;
        offset[0] = offset[1] = 0;
        flip = FALSE;
        allEnd = FALSE;
        WH_SetSessionUpdateCallback( whSessionUpdateCallback );
        if ( !WH_Initialize() )
        {
            printWMError("WH_Initialize() was failed");
            return;
        }
        else
        {
            stage = MMDC_STAGE_WAIT_INIT;
        }
        break;
    case MMDC_STAGE_WAIT_INIT:
        if ( WH_GetSystemState() != WH_SYSSTATE_BUSY )
        {
            stage = MMDC_STAGE_CHILD_CONNECT_AUTO;
        }
        break;
    case MMDC_STAGE_CHILD_CONNECT_AUTO:
        {   // SDカード
            RTCDate date;
            RTCTime time;
            char temp[64];
            if (RTC_GetDateTime(&date, &time) == RTC_RESULT_SUCCESS)
            {
                STD_TSNPrintf(temp, sizeof(temp)-1, "sdmc:/MMD-%04d%02d%02d-%02d%02d%02d.dat", 2000+date.year, date.month, date.day, time.hour, time.minute, time.second);
            }
            else
            {
                STD_TSNPrintf(temp, sizeof(temp)-1, "sdmc:/MMD-00000000-000000.dat");
            }
            if ( !FS_CreateFile(temp, FS_PERMIT_W) )
            {
                printWMError("Cannot create a dump file.");
                return;
            }
            FS_InitFile(&dumpFile);
            if ( !FS_OpenFileEx(&dumpFile, temp, FS_FILEMODE_W) )
            {
                printWMError("Cannot open the dump file.");
                return;
            }
        }
        WH_SetReceiver(ChildReceiveCallback);
        WH_SetChildWEPKeyGenerator(ChildWEPKeyGenerator);
        if ( !WH_ChildConnectAuto(WH_CONNECTMODE_MP_CHILD, ANY_PARENT, ALL_CHANNEL) )
        {
            printWMError("WH_ChildConnectAuto() was failed");
        }
        else
        {
            stage = MMDC_STAGE_WAIT_CHILD_CONNECT_AUTO;
        }
        break;
    case MMDC_STAGE_WAIT_CHILD_CONNECT_AUTO:
        if ( WH_GetSystemState() == WH_SYSSTATE_CONNECTED )
        {
            stage = MMDC_STAGE_START_MP;
        }
        break;
    case MMDC_STAGE_START_MP:
        if ( WH_GetSystemState() == WH_SYSSTATE_ERROR )
        {
            printWMError("Some error occurred.");
        }
        if ( offset[flip^1] ) // writable data is available
        {
            FS_WriteFile(&dumpFile, buffer[flip^1], (s32)offset[flip^1]);
            offset[flip^1] = 0;
        }
        if ( allEnd )
        {
            if ( offset[flip] )
            {
                FS_WriteFile(&dumpFile, buffer[flip], (s32)offset[flip]);
            }
            FS_CloseFile(&dumpFile);
            mmdumperEnd();
            stage = MMDC_STAGE_END;
        }
        break;
    case MMDC_STAGE_END:
        break;
    }
}

static u16 ChildWEPKeyGenerator(u16 *wepkey, const WMBssDesc *bssDesc)
{
    u16     data[20 / sizeof(u16)];
    u16     tmpWep[20 / sizeof(u16)];
    UserGameInfo* userGameInfo = (UserGameInfo*)bssDesc->gameInfo.userGameInfo;

    MI_CpuClear16(data, sizeof(data));

    // UserGameInfo 内に埋め込まれていた Seed 値を使用して WEP Key を計算
    MI_CpuCopy16(&userGameInfo->wepSeed, data, 4);
    // GGID や TGID や親機の MAC Address  も考慮に入れる
    MI_CpuCopy16(&bssDesc->gameInfo.ggid, data + 2, sizeof(u32));
    *(u16 *)(data + 4) = bssDesc->gameInfo.tgid;
    MI_CpuCopy8(bssDesc->bssid, (u8 *)(data + 5), WM_SIZE_MACADDR);

    // HMAC-SHA-1 で鍵付ハッシュ値を取り、128bit に切り詰めて WEP Key とする。
    // HMAC で使用する鍵はアプリケーション固有のものとすること。
    // (HMAC に関しては RFC2104 参照)
    MATH_CalcHMACSHA1(tmpWep, data, sizeof(data), SECRET_KEY, SECRET_KEY_LENGTH);
    MI_CpuCopy8(tmpWep, wepkey, 16);
    OS_TPrintf("wepkey: %04x%04x %04x%04x %04x%04x %04x%04x\n", wepkey[0], wepkey[1],
               wepkey[2], wepkey[3], wepkey[4], wepkey[5], wepkey[6], wepkey[7]);

    return WM_WEPMODE_128BIT;
}

static void ChildReceiveCallback(u16 aid, u16 *data, u16 length)
{
    OS_TPrintf("ChildReceiveCallback: %d, 0x%x, %d\n", aid, data, length);
    if ( aid != 0 )
    {
        printWMError("Unsupported AID");
    }
    if ( data == NULL )
    {
        OS_TPrintf("Received NO data.\n");
    }
    else
    {
        if ( length != request.length )
        {
            OS_TPrintf("WARNING: Received length is not valid (%d <> %d).\n", length, request.length);
        }
        if ( offset[flip] + length > sizeof(buffer[0]) ) // overflow then using new buffer
        {
            flip ^= 1;
            offset[flip] = 0;
        }
        MI_CpuCopy8( data, &buffer[flip][offset[flip]], length );
        offset[flip] += length;
        request.addr += length;
        if ( (u32)request.addr >= END_ADDRESS )
        {
            allEnd = TRUE;
        }
        else
        {
            if ( (u32)request.addr + request.length > END_ADDRESS )
            {
                request.length = (u16)(END_ADDRESS - (u32)request.addr);
            }
            ChildSendCallback();
        }
    }
}

static void ChildSendCallback(void)
{
    WH_SendData(&request, sizeof(request), NULL);
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
                // first request
                ChildSendCallback();
            }
        }
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

//******************************************************************************
//      SD抜け監視コールバック
//******************************************************************************

static void subCallBack( void* userdata, FSEvent event, void* argument )
{
#pragma unused( userdata, argument )

    if( event == FS_EVENT_MEDIA_INSERTED )
    {
        s_SDCheck = SDSTAT_INSERTED;  //挿入
    }
    else if( event == FS_EVENT_MEDIA_REMOVED )
    {
        s_SDCheck = SDSTAT_REMOVED;  //抜け
    }
}

#include <twl/ltdmain_end.h>
