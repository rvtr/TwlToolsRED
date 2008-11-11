/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - build - demos.TWL - nuc
  File:     error_msg.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 1024 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#include <twl.h>
//#include <TwlWiFi/nuc.h>
#include "nuc.h"
#include "nuc_error_msg.h"

/*---------------------------------------------------------------------------*
    内部使用の定数定義
 *---------------------------------------------------------------------------*/
static const char* PrivateNupError[] =
{
    "[NUP] Unknown Error",
    "[NUP] Another NUP operation is in progress",
    "[NUP] Unexpected internal error in the NUP library",
    "[NUP] Invalid input parameters",
    "[NUP] Country code is undefined",
    "[NUP] Unable to get version data",
    "[NUP] Unable to create data file",
    "[NUP] Out of memory"
};

static const char* PrivateCheckError[] =
{
   "[CHK] Unknown Error",
   "[CHK] No operation started since NUP_Init()",
   "[CHK] The check operation is in progress",
   "[CHK] Unexpected internal error in the NUP library",
   "[CHK] Exceeded memory restriction",
   "[CHK] Required system titles not on system",
   "[CHK] Failed to parse SOAP response",
   "[CHK] Server returned error in SOAP response",
   "[CHK] Version of a title is newer on the system than on the server",
   "[CHK] Server returned a HTTP status error when downloading content, ticket, or TMD"
};

static const char* PrivateDownloadError[] =
{
   "[DL] Unknown Error",
   "[DL] No operation started since NUP_Init()",
   "[DL] The download operation is in progress",
   "[DL] Unexpected internal error in the NUP library",
   "[DL] Exceeded memory restriction",
   "[DL] Required system titles not on system",
   "[DL] Failed to parse SOAP response",
   "[DL] Server returned error in SOAP response",
   "[DL] Version of a title is newer on the system than on the server",
   "[DL] Server returned a HTTP status error when downloading content, ticket, or TMD",
   "[DL] One or more specified titles not found on the update list",
   "[DL] Invalid TMD downloaded",
   "[DL] Invalid ticket downloaded",
   "[DL] Failed to import ticket",
   "[DL] Failed to initialize title import",
   "[DL] Failed to complete title import",
   "[DL] Failed to begin content import",
   "[DL] Failed to import content data",
   "[DL] Failed to end content import",
   "[DL] Insufficient space on the file system to download title",
   "[DL] Insufficient space on the file system to create backup data"
};

static const char* PrivateHTTPError[] =
{
    "[HTTP] Unknown Error",
    "[HTTP] Failed to resolve DNS",
    "[HTTP] Failed to connect to server",
    "[HTTP] Failed to resolve DNS in proxy server",
    "[HTTP] Failed to connect to proxy server",
    "[HTTP] Invalid handle error",
    "[HTTP] Failed to alloc",
    "[HTTP] Socket Error",
    "[HTTP] Receive buffer is full",
    "[HTTP] Failed to parse HTTP header",
    "[HTTP] Request is canceled by the caller",
    "[HTTP] Failed to create HTTP thread",
    "[HTTP] Error in sending, recieving or closing socket",
    "[HTTP] Invalid parameter is set in NHTTPCreateConnection",
    "[HTTP] request is in progress",
    "[HTTP] Failed to set Root CA",
    "[HTTP] Failed to set client cert"
};

static const char* PrivateSSLError[] =
{
    "[SSL] Unknown Error",
    "[SSL] SSL protocol failure",
    "[SSL] Read attempted but without waiting for blocking from the caller",
    "[SSL] Read attempted but without waiting for blocking from the caller",
    "[SSL] System call error",
    "[SSL] Read/write system returned 0",
    "[SSL] BIO not yet connected",
    "[SSL] SSL Descriptor is out of range",
    "[SSL] the server name and common name do not match",
    "[SSL] verification failed for the root CA",
    "[SSL] the certificate chain is incorrect",
    "[SSL] the time limit for the certificate has expired",
    "[SSL] Cert size is too large to get",
    "[SSL] Invalid root CA",
    "[SSL] Invalid client cert",
    "[SSL] the certificate has been denied",
    "[SSL] NSSL library was not initialized"
};

static const char* PrivateFSError[] =
{
    "[FS] Unknown Error",
    "[FS] Failed to correctly process results",
    "[FS] Current command is busy",
    "[FS] Command was canceled",
    "[FS] Archive is not supported by the specified command",
    "[FS] Some arguments are invalid to process the specified command",
    "[FS] Some internal resource is not enough to process the specified command",
    "[FS] Situation which the specified command will make is already made",
    "[FS] Specified command has failed by the problem of access permission.",
    "[FS] Detected file system corruption",
    "[FS] Failed in the results where a detectable error occurred during processing."
};

static const char* PrivateESError[] =
{
    "[ES] Unknown Error",
    "[ES] Incorrect cert type",
    "[ES] Failed to generate key",
    "[ES] Failed to verify signature",
    "[ES] Cannot open file",
    "[ES] Incorrect public key type",
    "[ES] Cert issurer mismatch",
    "[ES] Failed to encrypt",
    "[ES] Failed to open file",
    "[ES] Failed to read file",
    "[ES] Failed to write file",
    "[ES] TMD contents number error",
    "[ES] Incorrect signature length",
    "[ES] Incorrect cert length",
    "[ES] Device error",
    "[ES] File descripter is maximum",
    "[ES] Invalid operation",
    "[ES] FS connection failure",
    "[ES] Unsupported transfer source",
    "[ES] Device ID mismatch",
    "[ES] Incorrect content size",
    "[ES] Hash mismatch",
    "[ES] Incorrent content count",
    "[ES] Out of memory",
    "[ES] No TMD file exists",
    "[ES] No right",
    "[ES] Issuer not found",
    "[ES] No ticket exists",
    "[ES] Incorrect ticket",
    "[ES] Not enough space",
    "[ES] Incorrect boot version",
    "[ES] Unknown error",
    "[ES] Expired error",
    "[ES] Unused error",
    "[ES] Incorrect title version",
    "[ES] OS Ticket does not exist",
    "[ES] OS Content does not exist",
    "[ES] Not empty",
    "[ES] Disc NAND have no TMD"
};

static const char* PublicError[] =
{
    /* エラーが発生しましたため、
       TWL本体の更新ができません。*/
    "Error occurred.\n  Unable to update TWL system.",
    /* TWL本体保存メモリの空き容量が
       不足しています。
       TWL本体の更新はできませんでした。*/
    "Insufficient memory in TWL\n  Unable to update TWL system.",
    /* サーバーに接続できません。
       しばらく待ってやり直してください。*/
    "Unable to connect to server\n  Try again later.",
    /* インターネットのエラーにより、
       TWL本体の更新ができません。
       しばらく待ってやり直してください。*/
    "Network Error occurred.\n  Try again later."
};

/*---------------------------------------------------------------------------*
    内部使用のワーク領域型定義
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    グローバル変数
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    内部使用の関数定義（ローカル）
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    外部公開の関数定義
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         GetPrivateMsg

  Description:  開発者向けのエラーのメッセージを取得します。

  Arguments:    error_code

  Returns:      文字列へのポインタ
 *---------------------------------------------------------------------------*/
const char* GetPrivateMsg(int error_code)
{
    if (34000 <= error_code && error_code <= 34007)
    {   // NUPの一般的なエラー
        return PrivateNupError[error_code - 34000];
    }
    else if (34100 <= error_code && error_code <= 34109)
    {   // 更新情報取得中のエラー
        return PrivateCheckError[error_code - 34100];
    }
    else if (34200 <= error_code && error_code <= 34220)
    {   // ダウンロード中のエラー
        return PrivateDownloadError[error_code - 34200];
    }
    else if (34300 <= error_code && error_code <= 34316)
    {   // NHTTPのエラー
        return PrivateHTTPError[error_code - 34300];
    }
    else if (34400 <= error_code && error_code <= 34417)
    {   // NSSLのエラー
        return PrivateSSLError[error_code - 34400];
    }
    else if (34500 <= error_code && error_code <= 34510)
    {   // FSのエラー
        return PrivateFSError[error_code - 34500];
    }
    else if (34600 <= error_code && error_code <= 34639)
    {   // ESのエラー
        return PrivateESError[error_code - 34600];
    }
    return NULL;
}

/*---------------------------------------------------------------------------*
  Name:         GetPublicMsg

  Description:  ユーザー向けのエラーのメッセージを取得します。

  Arguments:    error_code

  Returns:      文字列へのポインタ
 *---------------------------------------------------------------------------*/
const char* GetPublicMsg(int error_code)
{
    NucError type = NUC_GetErrorType(error_code);
    switch(type)
    {
      case NUC_ERROR_NO_SPACE:
        return PublicError[1];
      case NUC_ERROR_CONNECT:
        return PublicError[2];
      case NUC_ERROR_INTERNET:
        return PublicError[3];
      case NUC_ERROR_UPDATE:
      default:
        return PublicError[0];
    }
}

