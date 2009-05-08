#include <twl.h>
#include <twl/nam.h>
#include <nitro/fs.h>

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

#include        "my_fs_util.h"
#include "hatamotolib.h"


#include        "text.h"
#include        "mprintf.h"
#include        "logprintf.h"
#include        "error_report.h"


static BOOL WaitEC(ECOpId opId);

#ifdef SDK_DEBUG
#define ECDL_LOG(msg)   OS_TPrintf("----\nECDL-LOG: %s\n----\n", msg);
#endif
#ifdef SDK_RELEASE
#define ECDL_LOG(msg)   OS_TPrintf("ECDL-LOG: %s\n", msg);
#endif
#ifdef SDK_FINALROM
#define ECDL_LOG(msg)   OS_TPrintf("ECDL-LOG: %s\n", msg);
#endif


static BOOL log_active = FALSE;
static FSFile *log_fd;
static FSFile log_fd_real;



static void *Alloc(size_t size)
{
  OSIntrMode old = OS_DisableInterrupts();
  void* p = OS_Alloc(size);
  if( p == NULL ) {
    OS_TPrintf("Alloc error %s %d\n",__FUNCTION__,__LINE__);
    mprintf("Alloc error %s %d\n",__FUNCTION__,__LINE__);
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


static void*   AllocForNHTTP(u32 size, int align)
{ 
  if(align <= 32) {
    return Alloc(size);
  }
  return Alloc(size);
}

static void*   AllocForEC(u32 size, int align)
{ 
  if(align <= 32) {
    return Alloc(size);
  }
  mprintf("Failed to %s %d\n", __FUNCTION__,__LINE__);

  return NULL;
}

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
      if( newp == NULL ) {
	Free(p);
	mprintf("Failed to %s %d\n", __FUNCTION__,__LINE__);
	return NULL;
      }
      MI_CpuCopy8(p, newp, size);
      Free(p);
      return newp;
    }
  else
    {
      return Alloc(size);
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


BOOL SetupVerData(void)
{
  BOOL bSuccess;
  void* pWork = Alloc(NA_VERSION_DATA_WORK_SIZE);
  if( pWork == NULL ) {
    miya_log_fprintf(log_fd, "%s Error memory alloc\n", __FUNCTION__);
    return FALSE;
  }
  bSuccess = NA_LoadVersionDataArchive(pWork, NA_VERSION_DATA_WORK_SIZE);
  return bSuccess;
}



//void SetupTitlesDataFile(const NAMTitleId* pTitleIds, u32 numTitleIds)
void SetupTitlesDataFile(const MY_USER_APP_TID* pTitleIds, u32 numTitleIds)
{
  NAMTitleId tid;
  for( u32 i = 0; i < numTitleIds; i++ )
    {
      //      tid = pTitleIds[i];
      tid = pTitleIds[i].tid;
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
  // はたもとくんに確認->ショップのオールリージョンでＯＫ
  //  *(u64 *)(HW_TWL_ROM_HEADER_BUF + 0x230) = 0x00030015　48-4E-46-41-ull;
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
      miya_log_fprintf(log_fd, "Failed to start NHTTP, rv=%d\n", rv);
      mprintf("Failed to start NHTTP, rv=%d\n", rv);
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

static void Dummy_WWW_AddJSPlugin()
{
}

static BOOL LoadCert(void** ppCert, u32* pSize, const char* name)
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
      miya_log_fprintf(log_fd, "%s Cannot open %s\n", __FUNCTION__, path);
      mprintf("%s Cannot open %s\n", __FUNCTION__,path);
      return FALSE;
    }

  certSize = FS_GetFileLength(&f);
  pCert = OS_Alloc(certSize);
  if ( pCert == NULL )
    {
      miya_log_fprintf(log_fd, "%s Cannot allocate work memroy\n",__FUNCTION__);
      mprintf("%s Cannot allocate work memroy \n",__FUNCTION__);
      return FALSE;
    }

  readSize = FS_ReadFile(&f, pCert, static_cast<s32>(certSize));
  if( readSize != certSize )
    {
      miya_log_fprintf(log_fd, "%s fail to read file\n", __FUNCTION__);
      mprintf("%s fail to read file\n",__FUNCTION__);
      return FALSE;
    }

  FS_CloseFile(&f);


  //  LoadCert(&pClientCert, &clientCertSize, ".twl-nup-cert.der");
  //  LoadCert(&pClientKey,  &clientKeySize,  ".twl-nup-prvkey.der");

  // static inline s32  NA_DecodeVersionData(const void* src, u32 srcSize, void* dst, u32 dstSize)
  //    { return OS_IsRunOnTwl() ? NAi_DecodeVersionData(src, srcSize, dst, dstSize): -4; }

  /*
    s32 NAi_DecodeVersionData(const void* src, u32 srcSize, void* dst, u32 dstSize)
    {
    u32 decryptedSize;
    AESResult result;
    
    if ( !SEAi_IsHWInitialized() )
    {
    return -1;
    }
    
    decryptedSize = srcSize - AES_SIGN_HEADER_SIZE;
    
    if ( src == NULL || dst == NULL || decryptedSize > dstSize )
    {
    return -2;
    }
    
    SEA_UseWeakKeyTemporary(SEA_WEAK_KEY_VERSION);
    result = SEA_Decrypt(src, srcSize, dst);
    
    if( result != AES_RESULT_SUCCESS )
    {
    return -3;
    }
    
    return (s32)decryptedSize;
    }
  */

  result = NA_DecodeVersionData(pCert, certSize, pCert, certSize);
  if( result <= 0 )
    {
      miya_log_fprintf(log_fd, "%s fail to decode version info %d\n",__FUNCTION__,result);
      mprintf("%s fail to decode version info %d\n", __FUNCTION__,result);
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
  //  logLevel = EC_LOG_FINE;
  logLevel = EC_LOG_WARN;

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
    miya_log_fprintf(log_fd, "%s Failed to initialize EC, rv=%d\n",__FUNCTION__, rv);
    mprintf("Failed to initialize EC, rv=%d\n", rv);
    return FALSE;
  } 

  rv = EC_SetWebSvcUrls( "https://ecs.t.shop.nintendowifi.net/ecs/services/ECommerceSOAP",
			 "https://ias.t.shop.nintendowifi.net/ias/services/IdentityAuthenticationSOAP",
			 "https://cas.t.shop.nintendowifi.net/cas/services/CatalogingSOAP" );
  // SDK_ASSERTMSG(rv == EC_ERROR_OK, "Failed to EC_SetWebSvcUrls, rv=%d\n", rv);
  if( rv != EC_ERROR_OK ) {
    miya_log_fprintf(log_fd, "%s Failed to EC_SetWebSvcUrls, rv=%d\n", __FUNCTION__, rv);
    mprintf("Failed to EC_SetWebSvcUrls, rv=%d\n", rv);
    return FALSE;
  } 
  
  rv = EC_SetContentUrls( "http://ccs.t.shop.nintendowifi.net/ccs/download",
			  "http://ccs.t.shop.nintendowifi.net/ccs/download" );
  // SDK_ASSERTMSG(rv == EC_ERROR_OK, "Failed to EC_SetContentUrls, rv=%d\n", rv);
  if( rv != EC_ERROR_OK ) {
    miya_log_fprintf(log_fd, "%s Failed to EC_SetContentUrls, rv=%d\n",__FUNCTION__, rv);
    mprintf("Failed to EC_SetContentUrls, rv=%d\n", rv);
    return FALSE;
  } 

  return TRUE;
}

static ECError WaitEC_result;

static BOOL WaitEC(ECOpId opId)
{
  ECError result;
  ECProgress progress;
  ECProgress progress_prev;
  
  if( opId < 0 )
    {
      miya_log_fprintf(log_fd, "%s WaitEC error %d %s\n", __FUNCTION__,opId, GetECErrorString(opId));
      mprintf("WaitEC error %d %s\n", opId, GetECErrorString(opId));
      return FALSE;
    }
  
  MI_CpuClear(&progress_prev, sizeof(progress_prev));
  
  for(;;)
    {
      result = EC_GetProgress(opId, &progress);
      WaitEC_result = result;
      if( (result != EC_ERROR_OK) && (result != EC_ERROR_NOT_DONE) )
        {
	  if( result == EC_ERROR_NOT_ACTIVE )
            {
	      miya_log_fprintf(log_fd, "%s  opId=%d\n", __FUNCTION__, opId);
            }
	  miya_log_fprintf(log_fd, "%s Failed to EC_GetProgress, result=%d %s\n", 
			   __FUNCTION__, result, GetECErrorString(result));
	  mprintf("EC_GetProgress failed %d\n %s\n", result, GetECErrorString(result));

	  if( MI_CpuComp8(&progress_prev, &progress, sizeof(progress)) != 0 ) {
	    miya_log_fprintf(log_fd, "---------\n");
	    miya_log_fprintf(log_fd, "progress report\n");
	    miya_log_fprintf(log_fd, "  status            %5d %s\n", progress.status, 
			     GetECErrorString(progress.status));
	    miya_log_fprintf(log_fd, "  operation         %5d %s\n", progress.operation,
			     GetECOperationString(progress.operation));
	    miya_log_fprintf(log_fd, "  phase             %5d %s\n", progress.phase, 
			     GetECOpPhaseString(progress.phase));
	    miya_log_fprintf(log_fd, "  isCancelRequested %5d\n", progress.isCancelRequested);
	    miya_log_fprintf(log_fd, "  totalSize         %5d\n", progress.totalSize);
	    miya_log_fprintf(log_fd, "  downloadedSize    %5d\n", progress.downloadedSize);
	    miya_log_fprintf(log_fd, "  errCode           %5d\n", progress.errCode);
	    miya_log_fprintf(log_fd, "  errInfo           %s\n", progress.errInfo);
	    progress_prev = progress;
	    /*
	      progress report
	      status            -4015 EC_ERROR_WS_REPORT
	      operation            12 EC_OP_Transfer
	      phase                 2 EC_PHASE_Done
	      isCancelRequested     0
	      totalSize             0
	      downloadedSize      553
	      errCode           -5986
	      errInfo           IAS - TWL device unmatched
	     */ 
	  }
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

      OS_Sleep(100);
    }
  return TRUE;;
}


static BOOL WaitEC_with_NG_print(ECOpId opId)
{
  ECError result;
  ECProgress progress;
  ECProgress progress_prev;
  
  if( opId < 0 )
    {
      miya_log_fprintf(log_fd, "NG.\n");
      miya_log_fprintf(log_fd, "  %s WaitEC error %d\n   %s\n", __FUNCTION__,opId, GetECErrorString(opId));

      m_set_palette(tc[0], M_TEXT_COLOR_RED );
      mprintf("NG.\n");
      m_set_palette(tc[0], M_TEXT_COLOR_YELLOW );
      mprintf("   WaitEC error %d\n   %s\n", opId, GetECErrorString(opId));
      m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
      return FALSE;
    }
  
  MI_CpuClear(&progress_prev, sizeof(progress_prev));
  
  for(;;)
    {
      result = EC_GetProgress(opId, &progress);
      WaitEC_result = result;
      if( (result != EC_ERROR_OK) && (result != EC_ERROR_NOT_DONE) ) {

	miya_log_fprintf(log_fd, "NG.\n");
	m_set_palette(tc[0], M_TEXT_COLOR_RED );
	mprintf("NG.\n");
	m_set_palette(tc[0], M_TEXT_COLOR_YELLOW );

	if( result == EC_ERROR_NOT_ACTIVE ) {
	  miya_log_fprintf(log_fd, "   %s opId=%d\n", __FUNCTION__, opId);
	  mprintf("   %s opId=%d\n", __FUNCTION__, opId);
	}
	miya_log_fprintf(log_fd, "   %s Failed to EC_GetProgress, result=%d %s\n", 
			 __FUNCTION__, result, GetECErrorString(result));
	mprintf("   EC_GetProgress failed %d\n   %s\n", result, GetECErrorString(result));

	m_set_palette(tc[0], M_TEXT_COLOR_WHITE );

	
	if( MI_CpuComp8(&progress_prev, &progress, sizeof(progress)) != 0 ) {
	  miya_log_fprintf(log_fd, "---------\n");
	  miya_log_fprintf(log_fd, "progress report\n");
	  miya_log_fprintf(log_fd, "  status            %5d %s\n", progress.status, 
			   GetECErrorString(progress.status));
	  miya_log_fprintf(log_fd, "  operation         %5d %s\n", progress.operation,
			   GetECOperationString(progress.operation));
	  miya_log_fprintf(log_fd, "  phase             %5d %s\n", progress.phase, 
			   GetECOpPhaseString(progress.phase));
	  miya_log_fprintf(log_fd, "  isCancelRequested %5d\n", progress.isCancelRequested);
	  miya_log_fprintf(log_fd, "  totalSize         %5d\n", progress.totalSize);
	  miya_log_fprintf(log_fd, "  downloadedSize    %5d\n", progress.downloadedSize);
	  miya_log_fprintf(log_fd, "  errCode           %5d\n", progress.errCode);
	  miya_log_fprintf(log_fd, "  errInfo           %s\n", progress.errInfo);
	  progress_prev = progress;
	  /*
	    progress report
	    status            -4015 EC_ERROR_WS_REPORT
	    operation            12 EC_OP_Transfer
	    phase                 2 EC_PHASE_Done
	    isCancelRequested     0
	    totalSize             0
	    downloadedSize      553
	    errCode           -5986
	    errInfo           IAS - TWL device unmatched
	  */ 
	}
	return FALSE;
      }
      
      if( progress.phase == EC_PHASE_Done ) {
	break;
      }
      OS_Sleep(100);
    }
  return TRUE;;
}




static BOOL CheckRegistration( char *status )
{
  s32 progress;
  ECError ecError;
  ECDeviceInfo di;
  BOOL printf_status_flag = FALSE;

  progress = EC_CheckRegistration();
  if( FALSE == WaitEC_with_NG_print(progress) ) {
    return FALSE;
  }

  ecError = EC_GetDeviceInfo(&di);
  if( ecError != EC_ERROR_OK ) {
    miya_log_fprintf(log_fd, "NG.\n");
    miya_log_fprintf(log_fd, " GetDeviceInfo failed\n");

    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_YELLOW );
    mprintf("  GetDeviceInfo failed\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    return FALSE;
  }


  switch( di.registrationStatus[0] ) {
  case '\0':
    printf_status_flag = TRUE;
    break;
  case 'U':
    printf_status_flag = TRUE;
    break;
  case 'R':
    printf_status_flag = TRUE;
    break;
  case 'P':
  case 'T':
    printf_status_flag = TRUE;
    break;
  default:
    break;
  }

  if( printf_status_flag == TRUE ) {
#define ECDL_DI_FMT "%-30s"
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %d\n", "isKeyPairConfirmed", di.isKeyPairConfirmed);
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %dll\n", "deviceId", di.deviceId);
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %s\n", "serial", di.serial);
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %s\n", "originalSerial", di.originalSerial);
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %s\n", "accountId", di.accountId);
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %s\n", "registrationStatus", di.registrationStatus);
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %s\n", "extAccountId", di.extAccountId);
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %s\n", "country", di.country);
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %s\n", "accountCountry", di.accountCountry);
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %s\n", "region", di.region);
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %s\n", "language", di.language);
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %d\n", "blockSize", di.blockSize);
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %d\n", "usedBlocks", di.usedBlocks);
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %d\n", "totalBlocks", di.totalBlocks);
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %d\n", "netContentRestrictions", di.netContentRestrictions);
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %d\n", "userAge", di.userAge);
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %d\n", "parentalControlFlags", di.parentalControlFlags);
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %d\n", "parentalControlOgn", di.parentalControlOgn);
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %d\n", "isParentalControlEnabled", di.isParentalControlEnabled);
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %d\n", "isNeedTicketSync", di.isNeedTicketSync);
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %d\n", "lastTicketSyncTime", di.lastTicketSyncTime);
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %d\n", "wirelessMACAddr", di.wirelessMACAddr);
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %d\n", "bluetoothMACAddr", di.bluetoothMACAddr);
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %d\n", "titleId", di.titleId);
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %d\n", "freeChannelAppCount", di.freeChannelAppCount);
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %d\n", "usedUserInodes", di.usedUserInodes);
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %d\n", "maxUserInodes", di.maxUserInodes);
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %s\n", "deviceCode", di.deviceCode);
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %s\n", "accountDeviceCode", di.accountDeviceCode);
    miya_log_fprintf(log_fd, ECDL_DI_FMT " %d\n", "isNeedTicketSyncImportAll", di.isNeedTicketSyncImportAll);
  }

  *status = di.registrationStatus[0];
  return TRUE;
}

static    BOOL GetChallenge(char* challenge)
{
  s32 progress;
  ECError ecError;

  //ECDL_LOG("get challenge");
  progress = EC_SendChallengeReq();
  if( FALSE == WaitEC_with_NG_print(progress) ) {
    return FALSE;
  }

  ecError = EC_GetChallengeResp(challenge);
  if( ecError != EC_ERROR_OK ) {
    miya_log_fprintf(log_fd, "NG.\n");
    miya_log_fprintf(log_fd, "   EC_GetChallengeResp error.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_YELLOW );
    mprintf("   EC_GetChallengeResp error.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    return FALSE;
  }
  return TRUE;
}

static BOOL SyncRegistration(const char* challenge)
{
  s32 progress;

  // ECDL_LOG("sync registration");
  progress = EC_SyncRegistration(challenge);
  if( FALSE == WaitEC_with_NG_print(progress) ) {
    return FALSE;
  }
  return TRUE;
}

static BOOL Register(const char* challenge)
{
  s32 progress;

  //  ECDL_LOG("register");
  progress = EC_Register(challenge, NULL, NULL);
  if( FALSE == WaitEC(progress) ) {
    return FALSE;
  }
  return TRUE;
}

static BOOL Transfer(const char* challenge)
{
  s32 progress;

  //  ECDL_LOG("transfer");
  progress = EC_Transfer(challenge);
  if( FALSE == WaitEC_with_NG_print(progress) ) {
    return FALSE;
  }
  return TRUE;
}
    
static BOOL SyncTickets()
{
  s32 progress;

  //  ECDL_LOG("sync tickets");
  progress = EC_SyncTickets(EC_SYNC_TYPE_IMPORT_ALL);
  if( FALSE == WaitEC_with_NG_print(progress) ) {
    return FALSE;
  }
  return TRUE;
}

static int a_to_int(char c)
{
  if( ('a' <= c) && (c <= 'f') ) {
    return (int)( c - 'a' + 10 );
  }
  else if( ('A' <= c) && (c <= 'F') ) {
    return (int)( c - 'A' + 10 );
  }
  else if( ('0' <= c) && (c <= '9') ) {
    return (int)( c - '0' );
  }
  return -1;
}



// static BOOL DownloadTitles(const NAMTitleId* pTitleIds, u32 numTitleIds)
static BOOL DownloadTitles(MY_USER_APP_TID* pTitleIds, u32 numTitleIds)
{
  s32 progress;
  NAMTitleId tid;
  BOOL ret_flag = TRUE;
  //  ECDL_LOG("download");
  char game_code_buf[5];

  for( u32 i = 0; i < numTitleIds; i++ ) {
    //      tid = pTitleIds[i];
    if( pTitleIds[i].is_personalized == 1 ) {
      /* 
	 personalized ticketのときは何もしない、というかプリンストールもの 
	 pTitleIds[i].is_personalized = 1 -> common 
	 pTitleIds[i].is_personalized = 2 -> personalized
      */
    }
    else {
      tid = pTitleIds[i].tid;
      progress = EC_DownloadTitle(tid, EC_DT_UPDATE_REQUIRED_CONTENTS);
      //      mprintf("-check registration..        ");
      (void)my_fs_Tid_To_GameCode((u64)tid, game_code_buf);
      game_code_buf[4] = '\0';
      mprintf("  downloading.. [%s]       ", game_code_buf);
#if 0
      if( FALSE == WaitEC(progress) ) {
	
	/* EC_ERROR_NET_CONTENTはずす？エラーにせずにバックアップだけ復活してやるか？ */ 
	/* 
	   if( WaitEC_result == EC_ERROR_NET_CONTENT ) {
	   tid を記録だけしてエラーにしない。
	   } 
	   
	*/
	m_set_palette(tc[0], M_TEXT_COLOR_RED );
	mprintf("NG.\n");
	m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
	miya_log_fprintf(log_fd, " %s download NG.\n",game_code_buf);
	ret_flag = FALSE;
      }
#else
      if( FALSE == WaitEC_with_NG_print(progress) ) {
	/* EC_ERROR_NET_CONTENTはずす？エラーにせずにバックアップだけ復活してやるか？ */ 
	miya_log_fprintf(log_fd, " %s download NG.\n",game_code_buf);
	ret_flag = FALSE;
      }
#endif
      else {
	pTitleIds[i].install_success_flag = TRUE;

	m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
	mprintf("OK.\n");
	m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
	miya_log_fprintf(log_fd, " %s download OK.\n",game_code_buf);
      }
    }
  }
  return ret_flag;
}

//int ECDownload(const NAMTitleId* pTitleIds, u32 numTitleIds)
int ECDownload(MY_USER_APP_TID *pTitleIds, u32 numTitleIds)
{
  char challenge[EC_CHALLENGE_BUF_SIZE];
  char status;
  //  BOOL ret_flag;

  STD_MemSet((void *)challenge,(int)0, EC_CHALLENGE_BUF_SIZE);

  /*********************************/
  mprintf("-check registration..        ");
  miya_log_fprintf(log_fd, "-check registration...");

  if( FALSE == CheckRegistration( &status ) ) {
    return ECDOWNLOAD_FAILURE;
  }

  // U  unregistered
  // R  registered
  // P  pending
  // T  transfered
  if( status == '\0' ) {
    miya_log_fprintf(log_fd, "NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );

    mprintf(" my error.\n");
    miya_log_fprintf(log_fd, " my error.\n");
    return ECDOWNLOAD_FAILURE;
  }
  else if( status == 'U') {
    miya_log_fprintf(log_fd, "NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_YELLOW );
    mprintf("  acount not transfered yet.\n");
    //    mprintf("   OR, acount already cleared\n\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    miya_log_fprintf(log_fd, " acount not transfered yet.\n");
    //    miya_log_fprintf(log_fd, "  OR acount already cleared by user.\n");
    // 「ご利用記録の削除」をした場合、ここでエラーになる。
    // ここでＯＫにする？
    return ECDOWNLOAD_FAILURE;
    //    return ECDOWNLOAD_NO_REGISTER;
  }
  else if( status == 'R') {
    miya_log_fprintf(log_fd, "NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_YELLOW );
    mprintf("  already registered. please delete acount.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    miya_log_fprintf(log_fd, "  already registered. please delete acount.\n");
    return ECDOWNLOAD_FAILURE;
  }
  else if( (status != 'P') && (status != 'T') ) {
    m_set_palette(tc[0], M_TEXT_COLOR_RED );
    mprintf("NG.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_YELLOW );
    mprintf("  invalid registration status '%c'\n", status );
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
    miya_log_fprintf(log_fd, " invalid registration status '%c'\n", status );
    return ECDOWNLOAD_FAILURE;
  }
  else {
    miya_log_fprintf(log_fd, "OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }


  /*********************************/
  miya_log_fprintf(log_fd, "-get challenge1..");
  mprintf("-get challenge1              ");
  if( FALSE == GetChallenge(challenge) ) {
    return ECDOWNLOAD_FAILURE;
  }
  else {
    miya_log_fprintf(log_fd, "OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }

  /*********************************/
  miya_log_fprintf(log_fd, "-transfer.. ");
  mprintf("-transfer                    ");
  if( FALSE == Transfer(challenge) ) {
    return ECDOWNLOAD_FAILURE;
  }
  else {
    miya_log_fprintf(log_fd, "OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }

  /*********************************/
  miya_log_fprintf(log_fd, "-get challenge.. ");
  mprintf("-get challenge2              ");
  if( FALSE == GetChallenge(challenge) ) {
    return ECDOWNLOAD_FAILURE;
  }
  else {
    miya_log_fprintf(log_fd, "OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }

  /*********************************/
  miya_log_fprintf(log_fd, "-sync registration..");
  mprintf("-sync registration           ");
  if( FALSE == SyncRegistration(challenge) ) {
    return ECDOWNLOAD_FAILURE;
  }
  else {
    miya_log_fprintf(log_fd, "OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }

  /*********************************/
  miya_log_fprintf(log_fd, "-sync tickets..");
  mprintf("-sync tickets                ");
  if( FALSE == SyncTickets() ) {
    return ECDOWNLOAD_FAILURE;
  }
  else {
    miya_log_fprintf(log_fd, "OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }

  /*********************************/
  // ここでアプリをダウンロードしている
  miya_log_fprintf(log_fd, "-download titles\n");
  mprintf("-download titles\n");

  if( FALSE == DownloadTitles(pTitleIds, numTitleIds) ) {
    return ECDOWNLOAD_FAILURE;
  }
  
  //#define ECDOWNLOAD_DUMMY       0
  //#define ECDOWNLOAD_SUCCESS     1
  //#define ECDOWNLOAD_NO_REGISTER 2
  //#define ECDOWNLOAD_FAILURE     3

  return ECDOWNLOAD_SUCCESS;
}


BOOL KPSClient()
{
  s32 progress;

  miya_log_fprintf(log_fd, "-generate key pair ..");
  mprintf("-generate key pair           ");
  progress = EC_GenerateKeyPair();
  if( FALSE == WaitEC_with_NG_print(progress) ) {
    return FALSE;
  }
  else {
    miya_log_fprintf(log_fd, "OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }


  miya_log_fprintf(log_fd, "-confirm key pair .. ");
  mprintf("-confirm key pair            ");
  progress = EC_ConfirmKeyPair();
  if( FALSE ==  WaitEC_with_NG_print(progress) ) {
    return FALSE;
  }
  else {
    miya_log_fprintf(log_fd, "OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_GREEN );	/* green  */
    mprintf("OK.\n");
    m_set_palette(tc[0], M_TEXT_COLOR_WHITE );
  }

  return TRUE;
}



FSFile *hatamotolib_log_start(char *log_file_name )
{
  log_fd = &log_fd_real;
  log_active = Log_File_Open( log_fd, log_file_name );
  if( !log_active ) {
    log_fd = NULL;
  }
  miya_log_fprintf(log_fd, "%s START\n", __FUNCTION__);
  return log_fd;
}

void hatamotolib_log_end(void)
{
  miya_log_fprintf(log_fd, "%s END\n\n", __FUNCTION__);
  if( log_active ) {
    Log_File_Close(log_fd);
  }
}

