/*---------------------------------------------------------------------------*
  Project:  TwlSDK - tools - ecdl
  File:     main.c

  Copyright 2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-24#$
  $Rev: 8618 $
  $Author: hatamoto_minoru $
 *---------------------------------------------------------------------------*/

#include "ecdl.h"

#ifdef SDK_DEBUG
#define ECDL_LOG(msg)   OS_TPrintf("----\nECDL-LOG: %s\n----\n", msg);
#endif
#ifdef SDK_RELEASE
#define ECDL_LOG(msg)   OS_TPrintf("ECDL-LOG: %s\n", msg);
#endif

namespace
{
    char CheckRegistration()
    {
        s32 progress;
        ECError ecError;
        ECDeviceInfo di;

        ECDL_LOG("check registeration");
        progress = EC_CheckRegistration();
        WaitEC(progress);

        ecError = EC_GetDeviceInfo(&di);
        SDK_ASSERT( ecError == EC_ERROR_OK );

#ifdef SDK_DEBUG
#define ECDL_DI_FMT "%-30s"
        OS_TPrintf(ECDL_DI_FMT " %d\n", "isKeyPairConfirmed", di.isKeyPairConfirmed);
        OS_TPrintf(ECDL_DI_FMT " %d\n", "deviceId", di.deviceId);
        OS_TPrintf(ECDL_DI_FMT " %s\n", "serial", di.serial);
        OS_TPrintf(ECDL_DI_FMT " %s\n", "originalSerial", di.originalSerial);
        OS_TPrintf(ECDL_DI_FMT " %s\n", "accountId", di.accountId);
        OS_TPrintf(ECDL_DI_FMT " %s\n", "registrationStatus", di.registrationStatus);
        OS_TPrintf(ECDL_DI_FMT " %s\n", "extAccountId", di.extAccountId);
        OS_TPrintf(ECDL_DI_FMT " %s\n", "country", di.country);
        OS_TPrintf(ECDL_DI_FMT " %s\n", "accountCountry", di.accountCountry);
        OS_TPrintf(ECDL_DI_FMT " %s\n", "region", di.region);
        OS_TPrintf(ECDL_DI_FMT " %s\n", "language", di.language);
        OS_TPrintf(ECDL_DI_FMT " %d\n", "blockSize", di.blockSize);
        OS_TPrintf(ECDL_DI_FMT " %d\n", "usedBlocks", di.usedBlocks);
        OS_TPrintf(ECDL_DI_FMT " %d\n", "totalBlocks", di.totalBlocks);
        OS_TPrintf(ECDL_DI_FMT " %d\n", "netContentRestrictions", di.netContentRestrictions);
        OS_TPrintf(ECDL_DI_FMT " %d\n", "userAge", di.userAge);
        OS_TPrintf(ECDL_DI_FMT " %d\n", "parentalControlFlags", di.parentalControlFlags);
        OS_TPrintf(ECDL_DI_FMT " %d\n", "parentalControlOgn", di.parentalControlOgn);
        OS_TPrintf(ECDL_DI_FMT " %d\n", "isParentalControlEnabled", di.isParentalControlEnabled);
        OS_TPrintf(ECDL_DI_FMT " %d\n", "isNeedTicketSync", di.isNeedTicketSync);
        OS_TPrintf(ECDL_DI_FMT " %d\n", "lastTicketSyncTime", di.lastTicketSyncTime);
        OS_TPrintf(ECDL_DI_FMT " %d\n", "wirelessMACAddr", di.wirelessMACAddr);
        OS_TPrintf(ECDL_DI_FMT " %d\n", "bluetoothMACAddr", di.bluetoothMACAddr);
        OS_TPrintf(ECDL_DI_FMT " %d\n", "titleId", di.titleId);
        OS_TPrintf(ECDL_DI_FMT " %d\n", "freeChannelAppCount", di.freeChannelAppCount);
        OS_TPrintf(ECDL_DI_FMT " %d\n", "usedUserInodes", di.usedUserInodes);
        OS_TPrintf(ECDL_DI_FMT " %d\n", "maxUserInodes", di.maxUserInodes);
        OS_TPrintf(ECDL_DI_FMT " %s\n", "deviceCode", di.deviceCode);
        OS_TPrintf(ECDL_DI_FMT " %s\n", "accountDeviceCode", di.accountDeviceCode);
        OS_TPrintf(ECDL_DI_FMT " %d\n", "isNeedTicketSyncImportAll", di.isNeedTicketSyncImportAll);
#endif

        return di.registrationStatus[0];
    }

    void GetChallenge(char* challenge)
    {
        s32 progress;
        ECError ecError;

        ECDL_LOG("get challenge");
        progress = EC_SendChallengeReq();
        WaitEC(progress);

        ecError = EC_GetChallengeResp(challenge);
        SDK_ASSERT( ecError == EC_ERROR_OK );
    }

    void SyncRegistration(const char* challenge)
    {
        s32 progress;

        ECDL_LOG("sync registration");
        progress = EC_SyncRegistration(challenge);
        WaitEC(progress);
    }

    void Register(const char* challenge)
    {
        s32 progress;

        ECDL_LOG("register");
        progress = EC_Register(challenge, NULL, NULL);
        WaitEC(progress);
    }

    void Transfer(const char* challenge)
    {
        s32 progress;

        ECDL_LOG("transfer");
        progress = EC_Transfer(challenge);
        WaitEC(progress);
    }
    
    void SyncTickets()
    {
        s32 progress;

        ECDL_LOG("sync tickets");
        progress = EC_SyncTickets(EC_SYNC_TYPE_IMPORT_ALL);
        WaitEC(progress);
    }

    void DownloadTitles(const NAMTitleId* pTitleIds, u32 numTitleIds)
    {
        s32 progress;

        ECDL_LOG("download");
        for( u32 i = 0; i < numTitleIds; ++i )
        {
            NAMTitleId tid = pTitleIds[i];

            OS_TPrintf("download %08X %08X\n", (u32)(tid >> 32), (u32)tid);
            progress = EC_DownloadTitle(tid, EC_DT_UPDATE_REQUIRED_CONTENTS);
            WaitEC(progress);
        }
    }
}


void
ECDownload(const NAMTitleId* pTitleIds, u32 numTitleIds)
{
    char challenge[EC_CHALLENGE_BUF_SIZE];
    char status;

    status = CheckRegistration();
    // U  unregistered
    // R  registered
    // P  pending
    // T  transfered
    SDK_ASSERTMSG(status != 'U', "acount not transfered yet.");
    SDK_ASSERTMSG(status != 'R', "already registered. please delete acount.");
    SDK_ASSERTMSG( (status == 'P') || (status == 'T'), "invalid registration status '%c'", status );

    GetChallenge(challenge);
    Transfer(challenge);

    GetChallenge(challenge);
    SyncRegistration(challenge);

    SyncTickets();
    DownloadTitles(pTitleIds, numTitleIds);
}

