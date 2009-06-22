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

// ���[�h�o�b�t�@�T�C�Y
#define    READ_BUFSIZE                  (10*1024)

#define DEBUGPRINT printf
//#define DEBUGPRINT(...)    ((void)0)

/*---------------------------------------------------------------------------*

 functions

 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*

 Name:        VerifyFiles

 Description: 2��ROM�t�@�C���̔C�ӂ̗̈���x���t�@�C����

 Arguments:   [out] 1�߂̃t�@�C���|�C���^
              [out] 2�߂̃t�@�C���|�C���^
              [in]  �̈�̐擪(�t�@�C���̐擪����̃I�t�Z�b�g)
              [in]  �̈�̃T�C�Y
              [out] ROM�w�b�_�i�[��

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
    
    // �o�b�t�@�����傫���ꍇ�͍א؂�Ƀ��[�h���ăx���t�@�C����
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

 Description: �x���t�@�C�ΏۊO���X�g�̏�����

 Arguments:   None.

 Return:      ���X�g�̃|�C���^

 *---------------------------------------------------------------------------*/
SOutRange* InitOutRange()
{
    SOutRange *list = calloc( 1, sizeof(SOutRange) );     // ���X�g�̐擪�̓_�~�[
    if( !list )
    {
        return NULL;
    }
    list->begin = 0xffffffff;
    list->end   = 0xffffffff;           // +1������0(�t�@�C���̐擪)�ɂȂ�
    strcpy( list->name, "Head" );
    DEBUGPRINT( "Init OutRange\n" );
    return list;
}

/*---------------------------------------------------------------------------*

 Name:        FinalizeOutRange

 Description: �x���t�@�C�ΏۊO���X�g�̏I������

 Arguments:   [out] ���X�g�̃|�C���^

 Return:      None.

 *---------------------------------------------------------------------------*/
void FinalizeOutRange( SOutRange *list )
{
    // �O���珇�Ƀ��������
    int cnt = 0;
    SOutRange *p = list;
    while( p )
    {
        SOutRange *free_p = p;
        p = p->next;                // ����O�Ɏ��̃|�C���^���擾���Ă���
        //DEBUGPRINT( "Free OutRange [0x%08x,0x%08x] : %s\n", free_p->begin, free_p->end, free_p->name );
        free( free_p );
        cnt++;
    }
    DEBUGPRINT( "Finalize OutRange %d items(includes Dummy)\n", cnt );
}

/*---------------------------------------------------------------------------*

 Name:        RegisterOutRange

 Description: �x���t�@�C�ΏۊO���X�g�ւ̓o�^

 Arguments:   [out] ���X�g�̃|�C���^
              [in]  �o�^����̈�̊J�n�A�h���X
              [in]  ..............�I���A�h���X
              [in]  ..............���O

 Return:      ���������Ƃ�TRUE.

 *---------------------------------------------------------------------------*/
BOOL RegisterOutRange( SOutRange *list, const u32 begin, const u32 end, const char *name )
{
    SOutRange *prev, *next, *curr;

    // �����ɂȂ�悤�ɑ}���ꏊ�����߂�
    prev = list;
    while( prev->next && (begin >= prev->next->begin) )
    {
        prev = prev->next;
    }

    // �V�K�쐬
    //DEBUGPRINT( "Register OutRange [0x%08x,0x%08x] : %s\n", begin, end, name );
    curr = calloc( 1, sizeof(SOutRange) );
    if( !curr )
    {
        return FALSE;
    }
    memset( curr, 0, sizeof(SOutRange) );
    curr->begin = begin;
    curr->end   = end;
    strncpy( curr->name, name, 255 );   // \0�̕��͎c���Ă���
    curr->name[255] = 0;                // �O�̂���

    // �}��
    next = prev->next;
    prev->next = curr;
    curr->next = next;
    return TRUE;
}

/*---------------------------------------------------------------------------*

 Name:        DumpOutRange

 Description: �x���t�@�C�ΏۊO���X�g�̗v�f��擪����\��

 Arguments:   [out] ���X�g�̃|�C���^

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

 Description: �x���t�@�C�ΏۊO���X�g�̗̈�����������ׂĂ̗̈���x���t�@�C����

 Arguments:   [out] ���X�g�̃|�C���^
              [in]  1�߂̃t�@�C���|�C���^
              [in]  2�߂̃t�@�C���|�C���^
              [out] �G���[���

 Return:      None.

 *---------------------------------------------------------------------------*/
BOOL VerifyExceptOutRange( SOutRange *list, FILE *fp1, FILE *fp2 )
{
    u32  filesize;
    SOutRange *p;

    //DumpOutRange( list );

    // �t�@�C���T�C�Y���قȂ�Ƃ���������
    fseek( fp1, 0, SEEK_END );
    fseek( fp2, 0, SEEK_END );
    filesize = ftell(fp1);
    if( filesize != ftell(fp2) )
    {
        printf( "\n*** Error: The size of files are different.\n" );
        return FALSE;
    }

    // �T���͈͂̓��X�g�Ɋ܂܂�Ȃ��̈� -> �̈�̏I�[���玟�̗̈�̐擪�܂ł��T���͈�
    p = list;
    while( p->next )
    {
        s32  begin = p->end + 1;
        if( begin < p->next->begin )   // ���Ԃ�����Ƃ��̂ݒT��
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
    // �Ō�̗̈悩��t�@�C���̍Ō�܂ł��x���t�@�C
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

