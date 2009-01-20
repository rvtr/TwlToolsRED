/*---------------------------------------------------------------------------*
  Project:  NITRO-System - build - demos - g2d - Text - textdemolib
  File:     loader.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: loader.h,v $
  Revision 1.1  2005/05/20 05:52:38  hatamoto_minoru
  NitroSystem Ç÷ÇÃìoò^ÅB

  Revision 1.3  2005/03/18 09:19:46  hatamoto_minoru
  APIïœçXÇ…ëŒâûÅB

  Revision 1.2  2005/02/08 04:06:59  hatamoto_minoru
  ÉtÉ@ÉCÉãÉwÉbÉ_ÇÃèCê≥ÅB

  Revision 1.1  2005/01/21 01:03:09  hatamoto_minoru
  TextDraw Ç÷ÇÃìoò^ÅB

  Revision 1.2  2004/11/30 09:16:33  hatamoto_minoru
  LoadNFTR() ÇÃí«â¡ÅB

  Revision 1.1  2004/09/10 11:21:25  hatamoto_minoru
  èâî≈ÇÃìoò^ÅB

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef LOAD_H_
#define LOAD_H_

#include <nnsys/fnd.h>
#include <nnsys/g2d.h>
#include <nnsys/g2d/g2d_Font.h>
#include <nnsys/g2d/load/g2d_NFT_load.h>

#ifdef __cplusplus
extern "C" {
#endif


void* LoadNCER( NNSG2dCellDataBank** ppCellBank, const char* pFname, NNSFndAllocator* pAllocator );
void* LoadNANR( NNSG2dAnimBankData** ppAnimBank, const char* pFname, NNSFndAllocator* pAllocator );
void* LoadNMCR( NNSG2dMultiCellDataBank** ppMCBank, const char* pFname, NNSFndAllocator* pAllocator );
void* LoadNMAR( NNSG2dMultiCellAnimBankData** ppMCABank, const char* pFname, NNSFndAllocator* pAllocator );
void* LoadNCGR( NNSG2dCharacterData** ppCharData, const char* pFname, NNSFndAllocator* pAllocator );
void* LoadNCLR( NNSG2dPaletteData** ppPltData, const char* pFname, NNSFndAllocator* pAllocator );
void* LoadNENR( NNSG2dEntityDataBank** ppEntityBank, const char* pFname, NNSFndAllocator* pAllocator );
void* LoadNCBR( NNSG2dCharacterData** ppCharData, const char* pFname, NNSFndAllocator* pAllocator );
void* LoadNSCR( NNSG2dScreenData** ppScrData, const char* pFname, NNSFndAllocator* pAllocator );
void* LoadNCGRforBG( NNSG2dCharacterData** ppCharData, const char* pFname, NNSFndAllocator* pAllocator );
void* LoadNFTR( NNSG2dFont* pFont, const char* pFname, NNSFndAllocator* pAllocator );



#ifdef __cplusplus
}/* extern "C" */
#endif

#endif // LOAD_H_

