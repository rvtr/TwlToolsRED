/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - demos - netconnect
  File:     wcm_control.h

  Copyright 2005,2006 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2007-12-06#$
  $Rev: 156 $
  $Author: okajima_manabu $
 *---------------------------------------------------------------------------*/
#ifndef SHARED_WCM_CONTROL_H_
#define SHARED_WCM_CONTROL_H_

#include "ap_info.h"

#ifdef __cplusplus

extern "C" {
#endif

/*===========================================================================*/

/*---------------------------------------------------------------------------*
  Name:         InitWcmControl

  Description:  �����ϐ������������AWCM �R���g���[���X���b�h���N������B
                ���ɃX���b�h���N�����Ă���ꍇ�͉����s��Ȃ��B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    InitWcmControl(void);
void    InitWcmControlEx(u8 deviceId);
void    InitWcmControlByApInfo(const WcmControlApInfo* ptr);
void    InitWcmControlByApInfoEx(const WcmControlApInfo* ptr, u8 deviceId);

/*---------------------------------------------------------------------------*
  Name:         InitWcmApInfo

  Description:  �A�N�Z�X�|�C���g�ڑ��p�����[�^�����ݒ�l����擾����
                ����������

  Arguments:    apinfo   �ڑ��p�����[�^
                apclass  �A�N�Z�X�|�C���g�̃N���X��

  Returns:      TRUE   ����
                FALSE  ���s
 *---------------------------------------------------------------------------*/
BOOL    InitWcmApInfo(WcmControlApInfo* apinfo, const char* apclass);

/*---------------------------------------------------------------------------*
  Name:         TerminateWcmControl

  Description:  WCM �R���g���[���X���b�h���I������B
                �X���b�h���N������Ă��Ȃ��ꍇ�͉����s��Ȃ��B
    NOTICE:     ���֐��̓X���b�h���I������ׂ̃g���K�ƂȂ邾���Ȃ̂ŁA�X���b�h
                ���̂��I������܂ő҂����ɖ߂�_�ɒ��ӁB
                �X���b�h���̂̏I����҂������ꍇ�́AOS_SendMessage �Ăяo����
                ������ OS_JoinThread ���Ăяo���K�v������B
                �Ȃ��AWCM_GetPhase �֐��� WCM_PHASE_NULL ��Ԃ��悤�ɂȂ��
                �X���b�h�͏I�����Ă���B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    TerminateWcmControl(void);

/*---------------------------------------------------------------------------*
  Name:         SetWcmThreadPriority

  Description:  WCM ���C�u�����̏�ԊǗ����s���X���b�h�̗D��x��ύX����B

  Arguments:    priority    -   0 �` 31 �̃X���b�h�D��x���w�肷��B

  Returns:      u8  -   �ύX����O�ɐݒ肳��Ă����D��x��Ԃ��B
                        �Ȃ�炩�̗��R�ŗD��x�̕ύX�Ɏ��s�����ꍇ�� 0xff ��Ԃ��B
 *---------------------------------------------------------------------------*/
u8      SetWcmThreadPriority(u8 priority);

/*---------------------------------------------------------------------------*
  Name:         GetWcmSearchAroundCount

  Description:  �S�`�����l�������T�X�L������������Ԃ��B
                StartUp�����0�ɂȂ�B

  Arguments:    None.

  Returns:      ����X�L�����������B
 *---------------------------------------------------------------------------*/
u32     GetWcmSearchAroundCount(void);

/*---------------------------------------------------------------------------*
  Name:         SetWcmManualConnectMode

  Description:  AP�̃��X�g����蓮�őI�����Đڑ�����悤�ݒ�.
                �f�t�H���g�ł� FALSE (�����ڑ�)�ƂȂ��Ă���.
                InitWcmControl() ����ɌĂяo���K�v������.

  Arguments:    enable -   �蓮�Őڑ�����ꍇ�� TRUE�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    SetWcmManualConnect(BOOL enable);

/*---------------------------------------------------------------------------*
  Name:         ConnectAP

  Description:  �w�肳�ꂽBssDesc��AP�֐ڑ�����

  Arguments:    bd     -   �ڑ�����AP�������\���̂ւ̃|�C���^�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    ConnectAP(const WCMBssDesc* pbd);

/*---------------------------------------------------------------------------*
  Name:         DisconnectAP

  Description:  AP �Ƃ̐ڑ��̐ؒf�����݂�

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    DisconnectAP(void);

/*---------------------------------------------------------------------------*
  Name:         SetWcmScanType

  Description:  AP ��T������ۂ̃X�L����������ݒ肷��B
                �f�t�H���g�ł̓A�N�e�B�u�X�L���������ƂȂ��Ă���B
                InitWcmControl() ����ɌĂяo���K�v������B

  Arguments:    type    -   WCM_OPTION_SCANTYPE_* �̒萔���w�肷��B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    SetWcmScanType(u32 type);

/*---------------------------------------------------------------------------*
  Name:         SetWcmPowerMode

  Description:  AP �ƒʐM����ۂ̓d�͊Ǘ��������w�肷��B
                �f�t�H���g�ł͏펞�A�N�e�B�u�����ƂȂ��Ă���B
                InitWcmControl() ����ɌĂяo���K�v������B

  Arguments:    mode    -   WCM_OPTION_POWER_* �̒萔���w�肷��B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    SetWcmPowerMode(u32 mode);

/*===========================================================================*/
#ifdef __cplusplus

}       /* extern "C" */
#endif

#endif /* SHARED_WCM_CONTROL_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
