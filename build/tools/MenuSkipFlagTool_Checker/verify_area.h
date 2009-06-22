/*---------------------------------------------------------------------------*
  Project:  TwlFirmware - tools - mastering
  File:     mastering.c

  Copyright 2003 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: #$
  $Rev$
  $Author$
 *---------------------------------------------------------------------------*/
#ifndef MENUSKIP_VERIFY_AREA_H
#define MENUSKIP_VERIFY_AREA_H

#include    <stdio.h>
#include    <twl/types.h>
#include    <twl/os/common/format_rom.h>

/*---------------------------------------------------------------------------*

 types

 *---------------------------------------------------------------------------*/

// マスタリングで変更される領域 (全領域ベリファイでベリファイ対象外となる)
typedef struct _SOutRange
{
    u32  begin;
    u32  end;
    char name[256];
    struct _SOutRange *next;
}
SOutRange;

/*---------------------------------------------------------------------------*

 functions

 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*

 Name:        VerifyFiles

 Description: 2つのROMファイルの任意の領域をベリファイする

 Arguments:   [out] 1つめのファイルポインタ
              [out] 2つめのファイルポインタ
              [in]  領域の先頭(ファイルの先頭からのオフセット)
              [in]  領域のサイズ
              [out] ROMヘッダ格納先

 Return:      正常なときTRUE

 *---------------------------------------------------------------------------*/
BOOL VerifyFiles( FILE *fp1, FILE *fp2, const s32 offset, s32 size );

/*---------------------------------------------------------------------------*

 Name:        InitOutRange

 Description: ベリファイ対象外リストの初期化

 Arguments:   None.

 Return:      リストのポインタ

 *---------------------------------------------------------------------------*/
SOutRange* InitOutRange();

/*---------------------------------------------------------------------------*

 Name:        FinalizeOutRange

 Description: ベリファイ対象外リストの終了処理

 Arguments:   [out] リストのポインタ

 Return:      None.

 *---------------------------------------------------------------------------*/
void FinalizeOutRange( SOutRange *list );

/*---------------------------------------------------------------------------*

 Name:        RegisterOutRange

 Description: ベリファイ対象外リストへの登録

 Arguments:   [out] リストのポインタ
              [in]  登録する領域の開始アドレス
              [in]  ..............終了アドレス
              [in]  ..............名前

 Return:      成功したときTRUE.

 *---------------------------------------------------------------------------*/
BOOL RegisterOutRange( SOutRange *list, const u32 begin, const u32 end, const char *name );

/*---------------------------------------------------------------------------*

 Name:        VerifyExceptOutRange

 Description: ベリファイ対象外リストの領域を除いたすべての領域をベリファイする

 Arguments:   [out] リストのポインタ
              [in]  1つめのファイルポインタ
              [in]  2つめのファイルポインタ
              [out] エラー情報

 Return:      成功のときTRUE

 *---------------------------------------------------------------------------*/
BOOL VerifyExceptOutRange( SOutRange *list, FILE *fp1, FILE *fp2 );


#endif
