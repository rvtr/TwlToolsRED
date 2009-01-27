 /*---------------------------------------------------------------------------*
  Project:  TwlSDK - tests - appjumpTest
  File:     common.h

  Copyright 2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-08-30#$
  $Rev: 8166 $
  $Author: nishimoto_takashi $
 *---------------------------------------------------------------------------*/
#ifndef COMMON_H_
#define COMMON_H_

#ifdef __cplusplus

extern "C" {
#endif

/*===========================================================================*/
#include <twl.h>

/*---------------------------------------------------------------------------*
    �萔��`
 *---------------------------------------------------------------------------*/

#define KEY_REPEAT_START    25  // �L�[���s�[�g�J�n�܂ł̃t���[����
#define KEY_REPEAT_SPAN     10  // �L�[���s�[�g�̊Ԋu�t���[����

/*---------------------------------------------------------------------------*
    �\���� ��`
 *---------------------------------------------------------------------------*/

// �L�[���͏��
typedef struct KeyInfo
{
    u16 cnt;    // �����H���͒l
    u16 trg;    // �����g���K����
    u16 up;     // �����g���K����
    u16 rep;    // �����ێ����s�[�g����
} KeyInfo;

/*---------------------------------------------------------------------------*
   Prototype
 *---------------------------------------------------------------------------*/
void InitCommon(void);

void ReadKey(KeyInfo* pKey);

void VBlankIntr(void);

/*===========================================================================*/
#ifdef __cplusplus

}       /* extern "C" */
#endif

#endif /* COMMON_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
