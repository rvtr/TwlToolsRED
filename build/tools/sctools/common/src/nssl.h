/*---------------------------------------------------------------------------*
  Project:  SSL library
  File:     nssl.h

  Copyright (C) 2007 Nintendo All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.
 *---------------------------------------------------------------------------*/

#ifndef	__NSSL_H__
#define	__NSSL_H__

#ifdef	__cplusplus
extern "C" {
#endif	// __cplusplus
    
/*---------------------------------------------------------------------------*
 * Includes
 *---------------------------------------------------------------------------*/

#include <nitro.h>

/*---------------------------------------------------------------------------*
 * Definitions
 *---------------------------------------------------------------------------*/
#define NSSL_VERIFY_NONE                         0x00
#define NSSL_VERIFY_COMMON_NAME                  0x01
#define NSSL_VERIFY_ROOT_CA                      0x02
#define NSSL_VERIFY_DATE                         0x04
#define NSSL_VERIFY_CHAIN                        0x08
#define NSSL_VERIFY_SUBJECT_ALT_NAME             0x10
#define NSSL_OPTION_SESSION_CACHE                0x100

#define NSSL_ERROR_BASE                          0xfffff000
#define NSSL_ERROR_CODE(code)                    ((s32)(NSSL_ERROR_BASE|(code)))
#define NSSL_ENONE                               0
#define NSSL_EFAILED                             NSSL_ERROR_CODE(1)
#define NSSL_EWANT_READ                          NSSL_ERROR_CODE(2)
#define NSSL_EWANT_WRITE                         NSSL_ERROR_CODE(3)
#define NSSL_ESYSCALL                            NSSL_ERROR_CODE(5)
#define NSSL_EZERO_RETURN                        NSSL_ERROR_CODE(6)
#define NSSL_EWANT_CONNECT                       NSSL_ERROR_CODE(7)
#define NSSL_ESSLID                              NSSL_ERROR_CODE(8)
#define NSSL_EVERIFY_COMMON_NAME                 NSSL_ERROR_CODE(9)
#define NSSL_EVERIFY_ROOT_CA                     NSSL_ERROR_CODE(10)
#define NSSL_EVERIFY_CHAIN                       NSSL_ERROR_CODE(11)
#define NSSL_EVERIFY_DATE                        NSSL_ERROR_CODE(12)
#define NSSL_EGET_SERVER_CERT                    NSSL_ERROR_CODE(13)
#define NSSL_EROOTCAID                           NSSL_ERROR_CODE(14) // not used
#define NSSL_ECLIENTCERTID                       NSSL_ERROR_CODE(15) // not used
#define NSSL_EVERIFY_REVOKED_CERT                NSSL_ERROR_CODE(16)
#define NSSL_ESTATE                              NSSL_ERROR_CODE(17)
#define NSSL_EVERIFY_USER                        NSSL_ERROR_CODE(18)

#define NSSL_ID_MAX                              8

#define NSSL_ROOTCA_MAX                          0
#define NSSL_CLIENTCERT_MAX                      0

#define NSSL_DEFAULT_HEAP_SIZE                   (1024*80)


/*---------------------------------------------------------------------------*
 * Types/Declarations
 *---------------------------------------------------------------------------*/

typedef struct NSSLConfig
{
    u32   maxId;
    void* (*alloc)(u32);
    void  (*free)(void*);
    void* (*realloc)(void*,u32);
    u32   fixedHeapSize;
} NSSLConfig;

typedef int  NSSLId;
typedef u32  NSSLClientCertId; // not used
typedef u32  NSSLRootCAId;

typedef struct NSSLResource
{
    int numDescSsl;
    u32 freeHeapSize;
    u32 usedHeapSize;
    u32 maxHeapSize;
} NSSLResource;

typedef struct NSSLRootCACertEntry
{
    int     rootCACertSize;
    u8*     rootCACert;
} NSSLRootCACertEntry;

/*---------------------------------------------------------------------------*
 * Cert Data
 *---------------------------------------------------------------------------*/
extern NSSLRootCACertEntry NSSLiRootCA_Nintendo_0;
extern NSSLRootCACertEntry NSSLiRootCA_Nintendo_1;
extern NSSLRootCACertEntry NSSLiRootCA_Nintendo_2;

#define NSSL_ROOTCA_NINTENDO_0                   ((NSSLRootCAId)&NSSLiRootCA_Nintendo_0)
#define NSSL_ROOTCA_NINTENDO_1                   ((NSSLRootCAId)&NSSLiRootCA_Nintendo_1)
#define NSSL_ROOTCA_NINTENDO_2                   ((NSSLRootCAId)&NSSLiRootCA_Nintendo_2)

/*---------------------------------------------------------------------------*
 * Function Prototypes
 *---------------------------------------------------------------------------*/
int     NSSL_Init            ( const NSSLConfig* config );
int     NSSL_Finish          ( void );
NSSLId  NSSL_NewClient       ( int socket, u32 option, const char* serverName );
int     NSSL_DoHandshake     ( NSSLId sslId );
int     NSSL_DoHandshakeEx   ( NSSLId sslId, char* serverCertData, size_t serverCertSize );
int     NSSL_Read            ( NSSLId sslId, char* buf, size_t bufSize );
int     NSSL_Peek            ( NSSLId sslId, char* buf, size_t bufSize );
int     NSSL_Write           ( NSSLId sslId, const char* buf, size_t bufSize );
int     NSSL_Shutdown        ( NSSLId sslId );
int     NSSL_SetClientCert        ( NSSLId sslId, const char* clientCertData, size_t clientCertSize, const char* privateKeyData, size_t privateKeySize );
int     NSSL_RemoveClientCert     ( NSSLId sslId );
int     NSSL_SetRootCA            ( NSSLId sslId, const char* rootCAData, size_t rootCASize );
int     NSSL_SetBuiltinRootCA     ( NSSLId sslId, NSSLRootCAId rootCAId );
int     NSSL_SetBuiltinClientCert ( NSSLId i_sslId, NSSLClientCertId i_clientCertId );
int     NSSL_DisableVerifyOptionForDebug( NSSLId i_sslId, u32 i_verifyOption );

int     NSSL_SetUTCTime           ( s64 utcTime );

int     NSSL_GetCipherInfo( NSSLId sslId, char** version, char** name, int* algBits, int* effectiveBits );

void    NSSL_GetResource(NSSLResource* resource);

// private function
NSSLId  NSSLi_New            ( u32 option, const char* serverName );
int     NSSLi_Connect        ( NSSLId sslId, int socket );
BOOL    NSSL_IsAvailable     ( void );
void    NSSL_GatherEntropy   ( void );

#ifdef	__cplusplus
}
#endif	// __cplusplus
#endif	// __NSSL_H__
