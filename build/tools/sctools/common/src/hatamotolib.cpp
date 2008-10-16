#include "ecdl.h"
#include <twl/sea.h>
#include <twl/lcfg.h>
#include <twl/na.h>
#include <twl/nam.h>

#ifdef USE_DWC
#include <dwc.h>
#include <ac/dwc_ac.h>
#endif

#include <NitroWiFi/nhttp.h>
#include "nssl.h"


#include "netconnect.h"
#include "sitedefs.h"
#include "wcm_control.h"

#include "hatamotolib.h"

static void *Alloc(size_t size)
{
  OSIntrMode old = OS_DisableInterrupts();
  void* p = OS_Alloc(size);
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

#ifdef USE_DWC
static void*   AllocForDWC(DWCAllocType name, u32 size, int align)
{ 
  SDK_ASSERT(align <= 32);(void)name;(void)align; return Alloc(static_cast<u32>(size));
}

static void    FreeForDWC(DWCAllocType name, void* ptr, u32 size)
{ 
  (void)name;(void)size; Free(ptr);
}
#endif

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
  
  for( u32 i = 0; i < size; ++i )
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


#ifdef USE_DWC
static const char*
GetDWCApInfoTypeString(DWCApInfoType type)
{
  static const StringMap STRING_MAP[] =
    {
#include "string_map_dwc_apinfo_type.inc"
      { 0, NULL }
    };
  
  return FindString(STRING_MAP, type);
}
#endif


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
  for( u32 i = 0; i < numTitleIds; ++i )
    {
      NAMTitleId tid = pTitleIds[i];
      NAM_SetupTitleDataFile(tid);
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


#ifdef USE_DWC
static u8 sDwcWork[ DWC_INIT_WORK_SIZE ]    ATTRIBUTE_ALIGN(32);
static DWCInetControl sDwcInetCtrl;

static void PollConnection_DWC()
{
  int errCode;
  int counter = 0;
  while( ! DWC_CheckInet() )
    {
      DWC_ProcessInet();
      OS_Sleep(16);
      OS_TPrintf("*** %s %d %d\n",__FUNCTION__, __LINE__, counter);
      counter++;
    }

  OS_TPrintf("*** %s %d\n",__FUNCTION__, __LINE__);
  
  switch ( DWC_GetInetStatus() )
    {
    case DWC_CONNECTINET_STATE_NOT_INITIALIZED:
      OS_TPrintf("   DWC_CONNECTINET_STATE_NOT_INITIALIZED\n" );
      break;
    case DWC_CONNECTINET_STATE_IDLE:
      OS_TPrintf("   DWC_CONNECTINET_STATE_IDLE           \n" );
      break;
    case DWC_CONNECTINET_STATE_OPERATING:
      OS_TPrintf("   DWC_CONNECTINET_STATE_OPERATING      \n" );
      break;
    case DWC_CONNECTINET_STATE_OPERATED:
      OS_TPrintf("   DWC_CONNECTINET_STATE_OPERATED       \n" );
      break;
    case DWC_CONNECTINET_STATE_CONNECTED:
      OS_TPrintf("   DWC_CONNECTINET_STATE_CONNECTED      \n" );
      break;
    case DWC_CONNECTINET_STATE_DISCONNECTING:
      OS_TPrintf("   DWC_CONNECTINET_STATE_DISCONNECTING  \n" );
      break;
    case DWC_CONNECTINET_STATE_DISCONNECTED:
      OS_TPrintf("   DWC_CONNECTINET_STATE_DISCONNECTED %d\n");
      break;
    case DWC_CONNECTINET_STATE_ERROR:
      DWC_GetLastError(&errCode);
      OS_Panic("   DWC_CONNECTINET_STATE_ERROR        %d\n", errCode );
      break;
    case DWC_CONNECTINET_STATE_FATAL_ERROR:
      DWC_GetLastError(&errCode);
      OS_Panic("   DWC_CONNECTINET_STATE_FATAL_ERROR  %d\n", errCode );
      break;
    default:
      DWC_GetLastError(&errCode);
      OS_Panic("   DWC_CONNECTINET_STATE_UNKNOWN_ERROR  %d\n", errCode );
    }
}

static bool ConnectionResult_DWC()
{
  DWCApInfo apinfo;
  
  if ( DWC_GetApInfo( &apinfo ) == TRUE )
    {
      OS_TPrintf("   AP type: %s\n", GetDWCApInfoTypeString(apinfo.aptype));
      OS_TPrintf("   ESSID  : %s\n", &apinfo.essid);
      return true;
    }
  else
    {
      DWCError     error;
      int          errorCode;
      DWCErrorType errorType;
      // 接続失敗のエラーコード表示
      error = DWC_GetLastErrorEx( &errorCode, &errorType );
      OS_TPrintf("   error point : %d\n", error );
      OS_TPrintf("   error no    : %d\n", -errorCode );
      OS_TPrintf("   error type  : %d\n", errorType );
      
      return false;
    }
}



void NetworkAutoConnect_DWC(void)
{

  DWC_SetReportLevel(DWC_REPORTFLAG_ALL);
  
  int result = DWC_Init(sDwcWork);

  if ( result == DWC_INIT_RESULT_DESTROY_OTHER_SETTING )
    {
      OS_TPrintf( "Wi-Fi setting might be broken.\n" );
    }
  
  DWC_SetMemFunc( &AllocForDWC, &FreeForDWC );

  DWC_InitInet( &sDwcInetCtrl );

  DWC_SetDisableEulaCheck();

  DWC_ConnectInetAsync();


  PollConnection_DWC();

  if( ! ConnectionResult_DWC() )
    {
      OS_Panic("auto connect failed");
    }
}

void NetworkShutdown_DWC(void)
{
  DWC_CleanupInet();
}
#endif

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

void SetupNHTTP(void)
{
  int rv;
  
  rv = NHTTPStartup(AllocForNHTTP, FreeForNHTTP, 18);
  NHTTP_SetBuiltinRootCAAsDefault(NSSL_ROOTCA_NINTENDO_2);
  
  if (rv != NHTTP_ERROR_NONE)
    {
      OS_Panic("Failed to start NHTTP, rv=%d\n", rv);
    }
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

static void
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
      OS_Panic("Cannot open %s\n", path);
    }

  certSize = FS_GetFileLength(&f);
  pCert = OS_Alloc(certSize);
  if ( pCert == NULL )
    {
      OS_Panic("Cannot allocate work memroy\n");
    }

  readSize = FS_ReadFile(&f, pCert, static_cast<s32>(certSize));
  if( readSize != certSize )
    {
      OS_Panic("fail to read file\n");
    }

  FS_CloseFile(&f);

  result = NA_DecodeVersionData(pCert, certSize, pCert, certSize);
  if( result <= 0 )
    {
      OS_Panic("fail to decode version info %d\n", result);
    }

  *ppCert = pCert;
  *pSize  = certSize;
}


void SetupEC(void)
{
  static u32 logLevel, clientCertSize, clientKeySize;
  static char deviceCode[17];
  
  ECError rv = EC_ERROR_OK;
  void* pClientCert;
  void* pClientKey;
  
  // Initialize the EC library

  logLevel = EC_LOG_FINE;
  //  logLevel = EC_LOG_NONE;

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
  SDK_ASSERTMSG(rv == EC_ERROR_OK, "Failed to initialize EC, rv=%d\n", rv);
  
  rv = EC_SetWebSvcUrls( "https://ecs.t.shop.nintendowifi.net/ecs/services/ECommerceSOAP",
			 "https://ias.t.shop.nintendowifi.net/ias/services/IdentityAuthenticationSOAP",
			 "https://cas.t.shop.nintendowifi.net/cas/services/CatalogingSOAP" );
  SDK_ASSERTMSG(rv == EC_ERROR_OK, "Failed to EC_SetWebSvcUrls, rv=%d\n", rv);
  
  rv = EC_SetContentUrls( "http://ccs.t.shop.nintendowifi.net/ccs/download",
			  "http://ccs.t.shop.nintendowifi.net/ccs/download" );
  SDK_ASSERTMSG(rv == EC_ERROR_OK, "Failed to EC_SetContentUrls, rv=%d\n", rv);
}


void WaitEC(ECOpId opId)
{
  ECError result;
  ECProgress progress;
  ECProgress progress_prev;
  
  if( opId < 0 )
    {
      OS_TPanic("error %d %s\n", opId, GetECErrorString(opId));
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
	  OS_TPanic("Failed to EC_GetProgress, result=%d %s\n", result, GetECErrorString(result));
        }

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
      if( progress.phase == EC_PHASE_Done )
        {
	  break;
        }

      OS_Sleep(300);
    }
}



void hatamotolib_main(void)
{
    // 不要：デバイス情報の表示
    PrintDeviceInfo();

    OS_TPrintf("--------------------------------\n");

    // setup
    {
        // 必須：タイトル ID の偽装
        SetupShopTitleId();

        // ？：ユーザ設定がされていないと接続できないので適当に設定
        SetupUserInfo();

        // 必須：バージョンデータのマウント
        SetupVerData();

        // 必須：ネットワークへの接続
        OS_TPrintf("connecting to AP....\n");
#ifdef USE_DWC
        NetworkAutoConnect_DWC();
#endif
        OS_TPrintf("connected\n");

        // 必須：HTTP と SSL の初期化
        OS_TPrintf("start NHTTP\n");
        SetupNSSL();
        SetupNHTTP();

        // 必須：EC の初期化
        OS_TPrintf("start EC\n");
        SetupEC();
    }

    // body
    {
        // ダウンロードすべきタイトルの指定
        NAMTitleId tids[] =
        {
            0x00030004346b6141ull,
//            0x0003000434616141ull,
//            0x0003000434616241ull,
//            0x000300043461644aull,
//            0x0003000434616741ull,
//            0x0003000434616a41ull,
//            0x0003000434617441ull,
//            0x0003000434626141ull,
//            0x0003000434626341ull,
//            0x0003000434626641ull,
//            0x0003000434626841ull,
//            0x0003000434626941ull,
//            0x0003000434636341ull,
//            0x00030004346b6141ull,
//            0x00030004346b6241ull,
//            0x00030004346b6341ull,
        };

        // 必須：デバイス証明書の発行
        KPSClient();

        // 必須：指定タイトルをダウンロード
        ECDownload(tids, sizeof(tids)/sizeof(*tids));

        // 不要：セーブデータ領域を作成
        SetupTitlesDataFile(tids, sizeof(tids)/sizeof(*tids));
    }

    // cleanup
    {
        ECError rv = EC_ERROR_OK;

        // EC の終了処理
        rv = EC_Shutdown();
        SDK_WARNING(rv == EC_ERROR_OK, "Failed to shutdown EC, rv=%d\n", rv);

        // ネットワークからの切断
#ifdef USE_DWC
        NetworkShutdown_DWC();
#endif
    }

    // EC が自分の Title ID のディレクトリを作成してしまうため、削除する
    DeleteECDirectory();

}

