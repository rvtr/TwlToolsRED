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

// �}�X�^�����O�ŕύX�����̈� (�S�̈�x���t�@�C�Ńx���t�@�C�ΏۊO�ƂȂ�)
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

 Description: 2��ROM�t�@�C���̔C�ӂ̗̈���x���t�@�C����

 Arguments:   [out] 1�߂̃t�@�C���|�C���^
              [out] 2�߂̃t�@�C���|�C���^
              [in]  �̈�̐擪(�t�@�C���̐擪����̃I�t�Z�b�g)
              [in]  �̈�̃T�C�Y
              [out] ROM�w�b�_�i�[��

 Return:      ����ȂƂ�TRUE

 *---------------------------------------------------------------------------*/
BOOL VerifyFiles( FILE *fp1, FILE *fp2, const s32 offset, s32 size );

/*---------------------------------------------------------------------------*

 Name:        InitOutRange

 Description: �x���t�@�C�ΏۊO���X�g�̏�����

 Arguments:   None.

 Return:      ���X�g�̃|�C���^

 *---------------------------------------------------------------------------*/
SOutRange* InitOutRange();

/*---------------------------------------------------------------------------*

 Name:        FinalizeOutRange

 Description: �x���t�@�C�ΏۊO���X�g�̏I������

 Arguments:   [out] ���X�g�̃|�C���^

 Return:      None.

 *---------------------------------------------------------------------------*/
void FinalizeOutRange( SOutRange *list );

/*---------------------------------------------------------------------------*

 Name:        RegisterOutRange

 Description: �x���t�@�C�ΏۊO���X�g�ւ̓o�^

 Arguments:   [out] ���X�g�̃|�C���^
              [in]  �o�^����̈�̊J�n�A�h���X
              [in]  ..............�I���A�h���X
              [in]  ..............���O

 Return:      ���������Ƃ�TRUE.

 *---------------------------------------------------------------------------*/
BOOL RegisterOutRange( SOutRange *list, const u32 begin, const u32 end, const char *name );

/*---------------------------------------------------------------------------*

 Name:        VerifyExceptOutRange

 Description: �x���t�@�C�ΏۊO���X�g�̗̈�����������ׂĂ̗̈���x���t�@�C����

 Arguments:   [out] ���X�g�̃|�C���^
              [in]  1�߂̃t�@�C���|�C���^
              [in]  2�߂̃t�@�C���|�C���^
              [out] �G���[���

 Return:      �����̂Ƃ�TRUE

 *---------------------------------------------------------------------------*/
BOOL VerifyExceptOutRange( SOutRange *list, FILE *fp1, FILE *fp2 );


#endif
