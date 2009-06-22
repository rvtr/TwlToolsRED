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
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>                      // strcasecmp()
#include    <getopt.h>                      // getopt()
#include    <sys/stat.h>                    // stat()
#include    <stdarg.h>
#include    <twl/types.h>
#include    <twl/os/common/format_rom.h>
#include    "verify_area.h"

/*---------------------------------------------------------------------------*

 constants and variables

 *---------------------------------------------------------------------------*/

// リードバッファサイズ
#define    READ_BUFSIZE                  (10*1024)

#define DEBUGPRINT printf
//#define DEBUGPRINT(...)    ((void)0)

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

 Return:      None.

 *---------------------------------------------------------------------------*/
BOOL VerifyFiles( FILE *fp1, FILE *fp2, const s32 offset, s32 size )
{
    u8   buf1[ READ_BUFSIZE ];
    u8   buf2[ READ_BUFSIZE ];
    s32  len;

    if( !fp1 || !fp2 )
    {
        return FALSE;
    }

    //DEBUGPRINT( "-- [0x%08x,0x%08x]\n", offset, offset+size-1 );

    fseek( fp1, offset, SEEK_SET );
    fseek( fp2, offset, SEEK_SET );
    
    // バッファよりも大きい場合は細切れにリードしてベリファイする
    while( size > 0 )
    {
        len = (size > READ_BUFSIZE)?(READ_BUFSIZE):(size);
        if( fread(buf1, 1, len, fp1) != len )
        {
            printf("\n*** Error: In verify files, failed to read the file. ***\n");
            return FALSE;
        }
        if( fread(buf2, 1, len, fp2) != len )
        {
            printf("\n*** Error: In verify files, failed to read the file. ***\n");
            return FALSE;
        }
        if( memcmp(buf1, buf2, len) != 0 )
        {
            printf("\n*** Error: verify files. ***\n");
            return FALSE;
        }
        size = size - len;
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*

 Name:        InitOutRange

 Description: ベリファイ対象外リストの初期化

 Arguments:   None.

 Return:      リストのポインタ

 *---------------------------------------------------------------------------*/
SOutRange* InitOutRange()
{
    SOutRange *list = calloc( 1, sizeof(SOutRange) );     // リストの先頭はダミー
    if( !list )
    {
        return NULL;
    }
    list->begin = 0xffffffff;
    list->end   = 0xffffffff;           // +1したら0(ファイルの先頭)になる
    strcpy( list->name, "Head" );
    DEBUGPRINT( "Init OutRange\n" );
    return list;
}

/*---------------------------------------------------------------------------*

 Name:        FinalizeOutRange

 Description: ベリファイ対象外リストの終了処理

 Arguments:   [out] リストのポインタ

 Return:      None.

 *---------------------------------------------------------------------------*/
void FinalizeOutRange( SOutRange *list )
{
    // 前から順にメモリ解放
    int cnt = 0;
    SOutRange *p = list;
    while( p )
    {
        SOutRange *free_p = p;
        p = p->next;                // 解放前に次のポインタを取得しておく
        //DEBUGPRINT( "Free OutRange [0x%08x,0x%08x] : %s\n", free_p->begin, free_p->end, free_p->name );
        free( free_p );
        cnt++;
    }
    DEBUGPRINT( "Finalize OutRange %d items(includes Dummy)\n", cnt );
}

/*---------------------------------------------------------------------------*

 Name:        RegisterOutRange

 Description: ベリファイ対象外リストへの登録

 Arguments:   [out] リストのポインタ
              [in]  登録する領域の開始アドレス
              [in]  ..............終了アドレス
              [in]  ..............名前

 Return:      成功したときTRUE.

 *---------------------------------------------------------------------------*/
BOOL RegisterOutRange( SOutRange *list, const u32 begin, const u32 end, const char *name )
{
    SOutRange *prev, *next, *curr;

    // 昇順になるように挿入場所を決める
    prev = list;
    while( prev->next && (begin >= prev->next->begin) )
    {
        prev = prev->next;
    }

    // 新規作成
    //DEBUGPRINT( "Register OutRange [0x%08x,0x%08x] : %s\n", begin, end, name );
    curr = calloc( 1, sizeof(SOutRange) );
    if( !curr )
    {
        return FALSE;
    }
    memset( curr, 0, sizeof(SOutRange) );
    curr->begin = begin;
    curr->end   = end;
    strncpy( curr->name, name, 255 );   // \0の分は残しておく
    curr->name[255] = 0;                // 念のため

    // 挿入
    next = prev->next;
    prev->next = curr;
    curr->next = next;
    return TRUE;
}

/*---------------------------------------------------------------------------*

 Name:        DumpOutRange

 Description: ベリファイ対象外リストの要素を先頭から表示

 Arguments:   [out] リストのポインタ

 Return:      None.

 *---------------------------------------------------------------------------*/
/*
static void DumpOutRange( SOutRange *list )
{
    SOutRange *p = list;
    while( p )
    {
        DEBUGPRINT( "range [0x%08x,0x%08x] : %s\n", p->begin, p->end, p->name );
        p = p->next;
    }
}
*/

/*---------------------------------------------------------------------------*

 Name:        VerifyExceptOutRange

 Description: ベリファイ対象外リストの領域を除いたすべての領域をベリファイする

 Arguments:   [out] リストのポインタ
              [in]  1つめのファイルポインタ
              [in]  2つめのファイルポインタ
              [out] エラー情報

 Return:      None.

 *---------------------------------------------------------------------------*/
BOOL VerifyExceptOutRange( SOutRange *list, FILE *fp1, FILE *fp2 )
{
    u32  filesize;
    SOutRange *p;

    //DumpOutRange( list );

    // ファイルサイズが異なるときおかしい
    fseek( fp1, 0, SEEK_END );
    fseek( fp2, 0, SEEK_END );
    filesize = ftell(fp1);
    if( filesize != ftell(fp2) )
    {
        printf( "\n*** Error: The size of files are different.\n" );
        return FALSE;
    }

    // 探索範囲はリストに含まれない領域 -> 領域の終端から次の領域の先頭までが探索範囲
    p = list;
    while( p->next )
    {
        s32  begin = p->end + 1;
        if( begin < p->next->begin )   // 隙間があるときのみ探索
        {
            s32 size = p->next->begin - begin;
            DEBUGPRINT( "Verify [0x%08x,0x%08x] (between \"%s\" and \"%s\")\n", (unsigned int)begin, (unsigned int)(begin+size-1), p->name, p->next->name );
            if( !VerifyFiles( fp1, fp2, begin, size ) )
            {
                printf( "\n*** Error: Verify error [0x%08x,0x%08x] (between \"%s\" and \"%s\") ***\n",
                        (unsigned int)begin, (unsigned int)(begin+size-1), p->name, p->next->name );
                return FALSE;
            }
        }
        else
        {
            DEBUGPRINT( "No Area exists between %s and %s\n", p->name, p->next->name );
        }
        p = p->next;
    }
    // 最後の領域からファイルの最後までをベリファイ
    {
        s32 begin = p->end + 1;
        s32 size  = filesize - begin;
        DEBUGPRINT( "Verify [0x%08x,0x%08x] (between \"%s\" and the end of file)\n", (unsigned int)begin, (unsigned int)(begin+size-1), p->name );
        if( !VerifyFiles( fp1, fp2, begin, size ) )
        {
            printf( "\n*** Error: Verify error [0x%08x,0x%08x] (between \"%s\" and the end of file) ***\n", 
                    (unsigned int)begin, (unsigned int)(begin+size-1), p->name );
            return FALSE;
        }
    }
    return TRUE;
}

