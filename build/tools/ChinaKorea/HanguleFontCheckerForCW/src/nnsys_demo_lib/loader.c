/*---------------------------------------------------------------------------*
  Project:  NITRO-System - build - demos - g2d - Text - textdemolib
  File:     loader.c

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: loader.c,v $
  Revision 1.1  2005/05/20 04:34:35  hatamoto_minoru
  NitroSystem への登録。

  Revision 1.5  2005/03/18 09:19:46  hatamoto_minoru
  API変更に対応。

  Revision 1.4  2005/03/10 08:18:55  hatamoto_minoru
  API の変更に対応。
  コメントの追加。

  Revision 1.3  2005/02/18 09:37:55  hatamoto_minoru
  sign/unsign警告に対処。

  Revision 1.2  2005/02/08 04:06:59  hatamoto_minoru
  ファイルヘッダの修正。

  Revision 1.1  2005/01/21 01:03:09  hatamoto_minoru
  TextDraw への登録。

  Revision 1.2  2004/11/30 09:16:33  hatamoto_minoru
  LoadNFTR() の追加。

  Revision 1.1  2004/09/10 11:21:25  hatamoto_minoru
  初版の登録。

  $NoKeywords: $
 *---------------------------------------------------------------------------*/

#include "loader.h"
#include <nitro.h>
#include <nnsys/g2d/g2d_Font.h>

#define LoadXXXX(ppData, pFname, pAlloc, unpack, print) \
    void*   pFile = NULL;                               \
                                                        \
    SDK_NULL_ASSERT( ppData );                          \
    SDK_NULL_ASSERT( pFname );                          \
                                                        \
    pFile = LoadFileToMainMem_( pFname, pAlloc );       \
    if( pFile != NULL )                                 \
    {                                                   \
        if( unpack( pFile, ppData ) )                   \
        {                                               \
            print( *ppData );                           \
            return pFile;                               \
        }                                               \
                                                        \
        NNS_FndFreeToAllocator( pAlloc, pFile );        \
    }                                                   \
                                                        \
    return NULL;                                        \


static void* LoadFileToMainMem_( const char* pFname, NNSFndAllocator* pAllocator )
{
    FSFile  file;
    void*   pFile = NULL;
    BOOL    bSuccess;

    SDK_NULL_ASSERT( pFname );

    FS_InitFile( &file );
    bSuccess = FS_OpenFile( &file, pFname );

    if( bSuccess )
    {
        const u32 szFile    = FS_GetLength( &file );


        pFile = NNS_FndAllocFromAllocator( pAllocator, szFile );
        SDK_NULL_ASSERT( pFile );

        if( szFile != (u32)FS_ReadFile( &file, pFile, (s32)szFile ) )
        {
            NNS_FndFreeToAllocator( pAllocator, pFile );
            pFile = NULL;
        }

        bSuccess = FS_CloseFile( &file );
        SDK_ASSERT( bSuccess );
    }else{
        OS_Warning(" Can't find the file : %s ", pFname );
    }

    return pFile;
}


void* LoadNCER( NNSG2dCellDataBank** ppData, const char* pFname, NNSFndAllocator* pAllocator )
{
    LoadXXXX(ppData, pFname, pAllocator, NNS_G2dGetUnpackedCellBank, NNS_G2dPrintCellBank );
}

void* LoadNANR( NNSG2dAnimBankData** ppData, const char* pFname, NNSFndAllocator* pAllocator )
{
    LoadXXXX(ppData, pFname, pAllocator, NNS_G2dGetUnpackedAnimBank, NNS_G2dPrintAnimBank);
}

void* LoadNMCR( NNSG2dMultiCellDataBank** ppData, const char* pFname, NNSFndAllocator* pAllocator )
{
    LoadXXXX(ppData, pFname, pAllocator, NNS_G2dGetUnpackedMultiCellBank, NNS_G2dPrintMultiCellBank );
}

void* LoadNMAR( NNSG2dMultiCellAnimBankData** ppData, const char* pFname, NNSFndAllocator* pAllocator )
{
    LoadXXXX(ppData, pFname, pAllocator, NNS_G2dGetUnpackedMCAnimBank, NNS_G2dPrintAnimBank);
}

void* LoadNCGR( NNSG2dCharacterData** ppData, const char* pFname, NNSFndAllocator* pAllocator )
{
    LoadXXXX(ppData, pFname, pAllocator, NNS_G2dGetUnpackedCharacterData, NNS_G2dPrintCharacterData  );
}

void* LoadNCLR( NNSG2dPaletteData** ppData, const char* pFname, NNSFndAllocator* pAllocator )
{
    LoadXXXX(ppData, pFname, pAllocator, NNS_G2dGetUnpackedPaletteData, NNS_G2dPrintPaletteData );
}

void* LoadNENR( NNSG2dEntityDataBank** ppData, const char* pFname, NNSFndAllocator* pAllocator )
{
    LoadXXXX(ppData, pFname, pAllocator, NNS_G2dGetUnpackedEntityBank, NNS_G2dPrintEntityDataBank );
}

void* LoadNCBR( NNSG2dCharacterData** ppData, const char* pFname, NNSFndAllocator* pAllocator )
{
    LoadXXXX(ppData, pFname, pAllocator, NNS_G2dGetUnpackedCharacterData, NNS_G2dPrintCharacterData  );
}

void* LoadNSCR( NNSG2dScreenData** ppScrData, const char* pFname, NNSFndAllocator* pAllocator )
{
    LoadXXXX(ppScrData, pFname, pAllocator, NNS_G2dGetUnpackedScreenData, NNS_G2dPrintScreenData);
}

void* LoadNCGRforBG( NNSG2dCharacterData** ppCharData, const char* pFname, NNSFndAllocator* pAllocator )
{
    LoadXXXX(ppCharData, pFname, pAllocator, NNS_G2dGetUnpackedBGCharacterData, NNS_G2dPrintCharacterData);
}

void* LoadNFTR( NNSG2dFont* pFont, const char* pFname, NNSFndAllocator* pAllocator )
{
    void*   pFile = NULL;

    SDK_NULL_ASSERT( pFont );
    SDK_NULL_ASSERT( pFname );

    pFile = LoadFileToMainMem_( pFname, pAllocator );
    if( pFile != NULL )
    {
        NNS_G2dFontInitAuto(pFont, pFile);
        NNS_G2dPrintFont(pFont);
        return pFile;
    }

    return NULL;
}
