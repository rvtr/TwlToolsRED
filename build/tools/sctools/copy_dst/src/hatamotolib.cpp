#include "ecdl.h"
#include <twl/sea.h>
#include <twl/lcfg.h>
#include <twl/na.h>
#include <twl/nam.h>


#include <NitroWiFi/nhttp.h>
#include "nssl.h"


#include "netconnect.h"
#include "sitedefs.h"
#include "wcm_control.h"

#include "hatamotolib.h"

#include        "text.h"
#include        "mprintf.h"
#include        "logprintf.h"



#ifdef SDK_DEBUG
#define ECDL_LOG(msg)   OS_TPrintf("----\nECDL-LOG: %s\n----\n", msg);
#endif
#ifdef SDK_RELEASE
#define ECDL_LOG(msg)   OS_TPrintf("ECDL-LOG: %s\n", msg);
#endif
#ifdef SDK_FINALROM
#define ECDL_LOG(msg)   OS_TPrintf("ECDL-LOG: %s\n", msg);
#endif



static void *Alloc(size_t size)
{
  OSIntrMode old = OS_DisableInterrupts();
  void* p = OS_Alloc(size);
  if( p == NULL ) {
    OS_TPrintf("Alloc error %s %d\n",__FUNCTION__,__LINE__);
  }
  OS_RestoreInterrupts(old);
  return p;
}

static void Free(void* ptr)
{
  if( ptr != NULL )
    {
      OSIntrMode old = OS_DisableInterrupts();
      OS_Free(ptr);
      OS_RestoreInterrupts(old);
    }
}


static void*   AllocForNHTTP(u32 size, int align) { SDK_ASSERT(align <= 32);(void)align; return Alloc(size); }
static void*   AllocForEC   (u32 size, int align) { SDK_ASSERT(align <= 32);(void)align; return Alloc(size); }

static void*   AllocForNSSL (u32 size)            { return Alloc(size); }
static void*   AllocForNAM  (u32 size)            { return Alloc(size); }
static void    FreeForNHTTP (void* p)             { Free(p); }
static void    FreeForEC    (void* p)             { Free(p); }
static void    FreeForNSSL  (void* p)             { Free(p); }
static void    FreeForNAM   (void* p)             { Free(p); }
static void* ReallocForNSSL(void* p, u32 size)
{
  if( p != NULL )
    {
      void* newp = Alloc(size);
      MI_CpuCopy8(p, newp, size);
      Free(p);
      return newp;
    }
  else
    {
      return Alloc(size);
    }
}


static void PrintBytes(const void* pv, u32 size)
{
  const u8* p = reinterpret_cast<const u8*>(pv);
  
  for( u32 i = 0; i < size; i++ )
    {
      OS_TPrintf("%02X", p[i]);
    }
}


struct StringMap
{
  int value;
  const char* string;
};

static  const char*
FindString(const StringMap* pMap, int value)
{
  while( pMap->string != NULL )
    {
      if( pMap->value == value )
	{
	  return pMap->string;
	}
      
      pMap++;
    }
  
  return "unknwon value";
}


static const char* GetECErrorString(ECError err)
{
  static const StringMap STRING_MAP[] =
    {
#include "string_map_ec_error.inc"
      { 0, NULL }
    };
  
  return FindString(STRING_MAP, err);
}

static const char*
GetECOperationString(ECOperation op)
{
  static const StringMap STRING_MAP[] =
    {
#include "string_map_ec_op.inc"
      { 0, NULL }
    };
  
  return FindString(STRING_MAP, op);
}

static const char*
GetECOpPhaseString(ECOpPhase phase)
{
  static const StringMap STRING_MAP[] =
    {
#include "string_map_ec_phase.inc"
      { 0, NULL }
    };
  
  return FindString(STRING_MAP, phase);
}



static const char*
GetOSTWLRegionString(u8 x)
{
  static const StringMap STRING_MAP[] =
    {
      { OS_TWL_REGION_JAPAN,      "japan" },
      { OS_TWL_REGION_AMERICA,    "america" },
      { OS_TWL_REGION_EUROPE,     "europe" },
      { OS_TWL_REGION_AUSTRALIA,  "australia" },
      { OS_TWL_REGION_CHINA,      "china" },
      { OS_TWL_REGION_KOREA,      "korea" },
      { 0, NULL }
    };
  
  return FindString(STRING_MAP, x);
}


void SetupUserInfo(void)
{
  BOOL bSuccess;
  BOOL bModified = FALSE;
  void* pWork;
  
  // LCFG_ReadTWLSettings を行う前に LCFG_ReadHWSecureInfo が必要
  {
    bSuccess = LCFG_ReadHWSecureInfo();
    SDK_ASSERT(bSuccess);
  }
  
  // 設定の読み込み
  {
    pWork = Alloc(LCFG_READ_TEMP);
    SDK_POINTER_ASSERT(pWork);
    
    bSuccess = LCFG_ReadTWLSettings(reinterpret_cast<u8 (*)[LCFG_READ_TEMP]>(pWork));
    SDK_ASSERT(bSuccess);
    
    Free(pWork);
  }
  
  {
    // ニックネームが空なら適当に設定
    if( *LCFG_TSD_GetNicknamePtr() == L'\0' )
      {
	LCFG_TSD_SetNickname(reinterpret_cast<const u16*>(L"ecdl"));
	bModified = TRUE;
      }
    
    // 国が選択されていないなら適当に設定
    if( LCFG_TSD_GetCountry() == LCFG_TWL_COUNTRY_UNDEFINED )
      {
	LCFG_TSD_SetCountry(LCFG_TWL_COUNTRY_JAPAN);
	bModified = TRUE;
      }
  }
  
  // 設定が変更されているなら書き出す
  if( bModified )
    {
      pWork = Alloc(LCFG_WRITE_TEMP);
      SDK_POINTER_ASSERT(pWork);
      
      bSuccess = LCFG_WriteTWLSettings(reinterpret_cast<u8 (*)[LCFG_WRITE_TEMP]>(pWork));
      SDK_ASSERT(bSuccess);
      
      Free(pWork);
    }
}

void SetupVerData(void)
{
  BOOL bSuccess;
  
  void* pWork = Alloc(NA_VERSION_DATA_WORK_SIZE);
  SDK_POINTER_ASSERT(pWork);
  
  bSuccess = NA_LoadVersionDataArchive(pWork, NA_VERSION_DATA_WORK_SIZE);

  SDK_ASSERT(bSuccess);
}



void PrintDeviceInfo(void)
{
  ESId deviceId;
  u8 region;
  u32 launcherInitialCode;
  const u8* pSerial;
  const u8* pMovable;
  u32 movableLen1;
  u32 movableLen2;
  u64 fuseId;
  char bmsDeviceId[32];

  LCFG_ReadHWNormalInfo();
  LCFG_ReadHWSecureInfo();

  // region
  region = LCFG_THW_GetRegion();

  // launcher initial code
  LCFG_THW_GetLauncherTitleID_Lo(reinterpret_cast<u8*>(&launcherInitialCode));

  // ES Device ID
  ES_GetDeviceId(&deviceId);
  snprintf(bmsDeviceId, sizeof(bmsDeviceId), "%lld", ((0x3ull << 32) | deviceId));

  // serial
  pSerial = LCFG_THW_GetSerialNoPtr();

  // movable id
  pMovable = LCFG_THW_GetMovableUniqueIDPtr();
  movableLen1 = LCFG_TWL_HWINFO_MOVABLE_UNIQUE_ID_LEN / 2;
  movableLen2 = LCFG_TWL_HWINFO_MOVABLE_UNIQUE_ID_LEN - movableLen1;

  // fuse id
  fuseId = SCFG_ReadFuseData();

  OS_TPrintf("Region:    %-10s %08X\n", GetOSTWLRegionString(region), launcherInitialCode);
  OS_TPrintf("DeviceID:  %08X %u\n", deviceId, deviceId);
  OS_TPrintf("           %s\n", bmsDeviceId);
  OS_TPrintf("Serial:    %s\n", pSerial);
  OS_TPrintf("MovableID: "); PrintBytes(pMovable, movableLen1); OS_TPrintf("\n");
  OS_TPrintf("           "); PrintBytes(pMovable + movableLen1, movableLen2); OS_TPrintf("\n");
  OS_TPrintf("FuseID:    %08X%08X\n", static_cast<u32>(fuseId >> 32), static_cast<u32>(fuseId));
}

void SetupTitlesDataFile(const NAMTitleId* pTitleIds, u32 numTitleIds)
{
  NAMTitleId tid;

  //  OS_TPrintf("%s %d num=%d\n",__FUNCTION__,__LINE__,numTitleIds);

  for( u32 i = 0; i < numTitleIds; i++ )
    {
      tid = pTitleIds[i];
      //      OS_TPrintf("%s %d 0x%016X\n",__FUNCTION__,__LINE__,(u64)tid);
      NAM_SetupTitleDataFile(tid);
      //OS_TPrintf("%s %d\n",__FUNCTION__,__LINE__);
    }
}

void DeleteECDirectory(void)
{
  char buf[64];
  OSTitleId tid = OS_GetTitleId();
  
  STD_TSNPrintf(buf, sizeof(buf), "nand:/title/%08x/%08x",
		(u32)(tid >> 32), (u32)(tid >> 0) );
  
  FS_DeleteDirectoryAuto(buf);
}

void SetupShopTitleId(void)
{
  *(u64 *)(HW_TWL_ROM_HEADER_BUF + 0x230) = 0x00030015484E4641ull;
}



void SetupNSSL(void)
{
  NSSLConfig conf;
  
  conf.maxId          = 8;
  conf.alloc          = AllocForNSSL;
  conf.free           = FreeForNSSL;
  conf.realloc        = ReallocForNSSL;
  conf.fixedHeapSize  = 0;
  NSSL_Init(&conf);
}

BOOL SetupNHTTP(void)
{
  int rv;
  
  rv = NHTTPStartup(AllocForNHTTP, FreeForNHTTP, 18);
  NHTTP_SetBuiltinRootCAAsDefault(NSSL_ROOTCA_NINTENDO_2);
  
  if (rv != NHTTP_ERROR_NONE)
    {
      OS_TPrintf("Failed to start NHTTP, rv=%d\n", rv);
      return FALSE;
    }
  return TRUE;
}



static void
GetDeviceCode(char *deviceCode, u32 bufSize)
{
  u64 eFuseData;
  u32 i;
  u8 digit;

  /* Fake the device code, seeded from the eFuse data */
  SCFG_Init();
  eFuseData = SCFG_ReadFuseData();

  for (i = 0; i < (bufSize - 1); i++) {
    digit = (u8) (eFuseData % 10);
    deviceCode[i] = (char) (digit + 48);
    eFuseData /= 10;
  }

  deviceCode[bufSize - 1] = '\0';
}

static void
Dummy_WWW_AddJSPlugin()
{
}

static BOOL
LoadCert(void** ppCert, u32* pSize, const char* name)
{
  FSFile f;
  BOOL bSuccess;
  s32 readSize;
  s32 result;
  char path[64];

  void* pCert;
  u32 certSize;

  FS_InitFile(&f);

  STD_TSNPrintf(path, sizeof(path), "verdata:/%s", name);
        
  bSuccess = FS_OpenFile(&f, path);
  if( ! bSuccess )
    {
      OS_TPrintf("Cannot open %s\n", path);
      return FALSE;
    }

  certSize = FS_GetFileLength(&f);
  pCert = OS_Alloc(certSize);
  if ( pCert == NULL )
    {
      OS_TPrintf("Cannot allocate work memroy\n");
      return FALSE;
    }

  readSize = FS_ReadFile(&f, pCert, static_cast<s32>(certSize));
  if( readSize != certSize )
    {
      OS_TPrintf("fail to read file\n");
      return FALSE;
    }

  FS_CloseFile(&f);

  result = NA_DecodeVersionData(pCert, certSize, pCert, certSize);
  if( result <= 0 )
    {
      OS_TPrintf("fail to decode version info %d\n", result);
      return FALSE;
    }

  *ppCert = pCert;
  *pSize  = certSize;
  return TRUE;
}


BOOL SetupEC(void)
{
  static u32 logLevel, clientCertSize, clientKeySize;
  static char deviceCode[17];
  
  ECError rv = EC_ERROR_OK;
  void* pClientCert;
  void* pClientKey;
  
  // Initialize the EC library

  /* log level definitions */

  //#define EC_LOG_NONE       0
  //#define EC_LOG_ERR        1
  //#define EC_LOG_WARN       2
  //#define EC_LOG_INFO       3
  //#define EC_LOG_FINE       4
  //#define EC_LOG_FINER      5
  //#define EC_LOG_FINEST     6

  //  logLevel = EC_LOG_FINEST;
  logLevel = EC_LOG_FINE;
  // logLevel = EC_LOG_NONE;

  LoadCert(&pClientCert, &clientCertSize, ".twl-nup-cert.der");
  LoadCert(&pClientKey,  &clientKeySize,  ".twl-nup-prvkey.der");
  GetDeviceCode(deviceCode, sizeof(deviceCode));
  
  ECNameValue initArgs[] =
    {
      { EC_ALLOC,                     &AllocForEC              },
      { EC_FREE,                      &FreeForEC               },
      { EC_LOG_LEVEL,                 &logLevel                },
      { EC_CLIENT_CERT,               pClientCert              },
      { EC_CLIENT_CERT_SIZE,          &clientCertSize          },
      { EC_CLIENT_KEY,                pClientKey               },
      { EC_CLIENT_KEY_SIZE,           &clientKeySize           },
      { EC_DEVICE_CODE,               deviceCode               },
      { EC_ADD_JS_PLUGIN_CALLBACK,    &Dummy_WWW_AddJSPlugin   },
    };
  const u32 nInitArgs = sizeof(initArgs) / sizeof(initArgs[0]);
  
  rv = EC_Init(initArgs, nInitArgs);
  // SDK_ASSERTMSG(rv == EC_ERROR_OK, "Failed to initialize EC, rv=%d\n", rv);
  if( rv != EC_ERROR_OK ) {
    OS_TPrintf("Failed to initialize EC, rv=%d\n", rv);
    return FALSE;
  } 

  rv = EC_SetWebSvcUrls( "https://ecs.t.shop.nintendowifi.net/ecs/services/ECommerceSOAP",
			 "https://ias.t.shop.nintendowifi.net/ias/services/IdentityAuthenticationSOAP",
			 "https://cas.t.shop.nintendowifi.net/cas/services/CatalogingSOAP" );
  // SDK_ASSERTMSG(rv == EC_ERROR_OK, "Failed to EC_SetWebSvcUrls, rv=%d\n", rv);
  if( rv != EC_ERROR_OK ) {
    OS_TPrintf("Failed to EC_SetWebSvcUrls, rv=%d\n", rv);
    return FALSE;
  } 
  
  rv = EC_SetContentUrls( "http://ccs.t.shop.nintendowifi.net/ccs/download",
			  "http://ccs.t.shop.nintendowifi.net/ccs/download" );
  // SDK_ASSERTMSG(rv == EC_ERROR_OK, "Failed to EC_SetContentUrls, rv=%d\n", rv);
  if( rv != EC_ERROR_OK ) {
    OS_TPrintf("Failed to EC_SetContentUrls, rv=%d\n", rv);
    return FALSE;
  } 

  return TRUE;
}


BOOL WaitEC(ECOpId opId)
{
  ECError result;
  ECProgress progress;
  ECProgress progress_prev;
  
  if( opId < 0 )
    {
      OS_TPrintf("error %d %s\n", opId, GetECErrorString(opId));
      return FALSE;
    }
  
  MI_CpuClear(&progress_prev, sizeof(progress_prev));
  
  for(;;)
    {
      result = EC_GetProgress(opId, &progress);
      if( (result != EC_ERROR_OK) && (result != EC_ERROR_NOT_DONE) )
        {
	  if( result == EC_ERROR_NOT_ACTIVE )
            {
	      OS_TPrintf("opId=%d\n", opId);
            }
	  OS_TPrintf("Failed to EC_GetProgress, result=%d %s\n", result, GetECErrorString(result));
	  mprintf("EC_GetProgress failed %d\n %s\n", result, GetECErrorString(result));
	  return FALSE;
        }

#if 0
      //#ifdef SDK_DEBUG
      if( MI_CpuComp8(&progress_prev, &progress, sizeof(progress)) != 0 )
        {
	  OS_TPrintf("---------\n");
	  OS_TPrintf("progress report\n");
	  OS_TPrintf("  status            %5d %s\n", progress.status, GetECErrorString(progress.status));
	  OS_TPrintf("  operation         %5d %s\n", progress.operation, GetECOperationString(progress.operation));
	  OS_TPrintf("  phase             %5d %s\n", progress.phase, GetECOpPhaseString(progress.phase));
	  OS_TPrintf("  isCancelRequested %5d\n", progress.isCancelRequested);
	  OS_TPrintf("  totalSize         %5d\n", progress.totalSize);
	  OS_TPrintf("  downloadedSize    %5d\n", progress.downloadedSize);
	  OS_TPrintf("  errCode           %5d\n", progress.errCode);
	  OS_TPrintf("  errInfo           %s\n", progress.errInfo);
	  progress_prev = progress;
        }
      //#endif
#endif
      if( progress.phase == EC_PHASE_Done )
        {
	  break;
        }

      OS_Sleep(300);
    }
  return TRUE;;
}




static char CheckRegistration()
{
  s32 progress;
  ECError ecError;
  ECDeviceInfo di;

  ECDL_LOG("check registeration");
  progress = EC_CheckRegistration();
  if( FALSE == WaitEC(progress) ) {
    return '\0'; // 微妙・・
  }

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

static    BOOL GetChallenge(char* challenge)
{
  s32 progress;
  ECError ecError;

  ECDL_LOG("get challenge");
  progress = EC_SendChallengeReq();
  if( FALSE == WaitEC(progress) ) {
    return FALSE;
  }

  ecError = EC_GetChallengeResp(challenge);
  SDK_ASSERT( ecError == EC_ERROR_OK );
  return TRUE;
}

static BOOL SyncRegistration(const char* challenge)
{
  s32 progress;

  ECDL_LOG("sync registration");
  progress = EC_SyncRegistration(challenge);
  if( FALSE == WaitEC(progress) ) {
    return FALSE;
  }
  return TRUE;
}

static BOOL Register(const char* challenge)
{
  s32 progress;

  ECDL_LOG("register");
  progress = EC_Register(challenge, NULL, NULL);
  if( FALSE == WaitEC(progress) ) {
    return FALSE;
  }
  return TRUE;
}

static BOOL Transfer(const char* challenge)
{
  s32 progress;

  ECDL_LOG("transfer");
  progress = EC_Transfer(challenge);
  if( FALSE == WaitEC(progress) ) {
    return FALSE;
  }
  return TRUE;
}
    
static BOOL SyncTickets()
{
  s32 progress;

  ECDL_LOG("sync tickets");
  progress = EC_SyncTickets(EC_SYNC_TYPE_IMPORT_ALL);
  if( FALSE == WaitEC(progress) ) {
    return FALSE;
  }
  return TRUE;
}

static BOOL DownloadTitles(const NAMTitleId* pTitleIds, u32 numTitleIds)
{
  s32 progress;

  ECDL_LOG("download");
  for( u32 i = 0; i < numTitleIds; i++ )
    {
      NAMTitleId tid = pTitleIds[i];
      progress = EC_DownloadTitle(tid, EC_DT_UPDATE_REQUIRED_CONTENTS);
      if( FALSE == WaitEC(progress) ) {
	return FALSE;
      }
    }
  return TRUE;
}

BOOL ECDownload(const NAMTitleId* pTitleIds, u32 numTitleIds)
{
  char challenge[EC_CHALLENGE_BUF_SIZE];
  char status;

  //    mprintf("-CheckRegistration..\n");
  status = CheckRegistration();
  // U  unregistered
  // R  registered
  // P  pending
  // T  transfered
#if 0
  SDK_ASSERTMSG(status != 'U', "acount not transfered yet.");
  SDK_ASSERTMSG(status != 'R', "already registered. please delete acount.");
  SDK_ASSERTMSG( (status == 'P') || (status == 'T'), "invalid registration status '%c'", status );
#else
  if( status == 'U') {
    mprintf(" acount not transfered yet.\n");
    return FALSE;
  }
  if( status == 'R') {
    mprintf(" already registered. please delete acount.\n");
    return FALSE;
  }
  if( (status != 'P') && (status != 'T') ) {
    mprintf(" invalid registration status '%c'\n", status );
    return FALSE;
  }
  //    mprintf(" succeeded.");
#endif

  mprintf("-get challenge1              ");
  if( FALSE == GetChallenge(challenge) ) {
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    return FALSE;
  }
  else {
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }

  mprintf("-transfer                    ");
  if( FALSE == Transfer(challenge) ) {
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    return FALSE;
  }
  else {
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }

  mprintf("-get challenge2              ");
  if( FALSE == GetChallenge(challenge) ) {
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    return FALSE;
  }
  else {
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }


  mprintf("-sync registration           ");
  if( FALSE == SyncRegistration(challenge) ) {
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    return FALSE;
  }
  else {
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }

  mprintf("-sync tickets                ");
  if( FALSE == SyncTickets() ) {
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    return FALSE;
  }
  else {
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }

  mprintf("-download titles             ");
  if( FALSE == DownloadTitles(pTitleIds, numTitleIds) ) {
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    return FALSE;
  }
  else {
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }

  return TRUE;
}


BOOL KPSClient()
{
  s32 progress;

  OS_TPrintf("generate key pair\n");
  mprintf("-generate key pair           ");
  progress = EC_GenerateKeyPair();
  if( FALSE == WaitEC(progress) ) {
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    return FALSE;
  }
  else {
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }


  OS_TPrintf("confirm key pair\n");
  mprintf("-confirm key pair            ");
  progress = EC_ConfirmKeyPair();
  if( FALSE ==  WaitEC(progress) ) {
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    return FALSE;
  }
  else {
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }

  return TRUE;
}



