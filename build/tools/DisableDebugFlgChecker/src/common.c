 /*---------------------------------------------------------------------------*
  Project:  TwlSDK - tests - appjumpTest
  File:     common.c

  Copyright 2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-08-25#$
  $Rev: 8074 $
  $Author: nishimoto_takashi $
 *---------------------------------------------------------------------------*/
#include "common.h"

static void InitInterrupts(void);
static void InitHeap(void);

/*---------------------------------------------------------------------------*
    �֐���`
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         InitCommon

  Description:  ��{�I�ȏ������֐��������ŌĂԁB

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void InitCommon(void)
{
    OS_Init();
    OS_InitTick();
    OS_InitAlarm();
    GX_Init();
    GX_DispOff();
    GXS_DispOff();

	CARD_Init(); 		// �V�K�ǉ�
    CARD_Enable(TRUE); 	// �V�K�ǉ�
    
    InitHeap();
    InitInterrupts();
}

/*---------------------------------------------------------------------------*
  Name:         ReadKey

  Description:  �L�[���͏����擾���A���͏��\���̂�ҏW����B
                �����g���K�A�����g���K�A�����p�����s�[�g�g���K �����o����B

  Arguments:    pKey  - �ҏW����L�[���͏��\���̂��w�肷��B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void ReadKey(KeyInfo* pKey)
{
    static u16  repeat_count[12];
    int         i;
    u16         r;

    r = PAD_Read();
    pKey->trg = 0x0000;
    pKey->up = 0x0000;
    pKey->rep = 0x0000;

    for (i = 0; i < 12; i++)
    {
        if (r & (0x0001 << i))
        {
            if (!(pKey->cnt & (0x0001 << i)))
            {
                pKey->trg |= (0x0001 << i);     // �����g���K
                repeat_count[i] = 1;
            }
            else
            {
                if (repeat_count[i] > KEY_REPEAT_START)
                {
                    pKey->rep |= (0x0001 << i); // �����p�����s�[�g
                    repeat_count[i] = (u16) (KEY_REPEAT_START - KEY_REPEAT_SPAN);
                }
                else
                {
                    repeat_count[i]++;
                }
            }
        }
        else
        {
            if (pKey->cnt & (0x0001 << i))
            {
                pKey->up |= (0x0001 << i);      // �����g���K
            }
        }
    }

    pKey->cnt = r;  // �����H�L�[����
}

/*---------------------------------------------------------------------------*
  Name:         InitInterrupts

  Description:  ���荞�ݐݒ������������B
                V �u�����N���荞�݂������A���荞�݃n���h����ݒ肷��B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void InitInterrupts(void)
{
    // V �u�����N���荞�ݐݒ�
    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)GX_VBlankIntr(TRUE);

    // ���荞�݋���
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();
}

/*---------------------------------------------------------------------------*
  Name:         InitHeap

  Description:  ���C����������̃A���[�i�ɂă����������ăV�X�e��������������B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void InitHeap(void)
{
    void*           tempLo;
    OSHeapHandle    hh;

    // ���C����������̃A���[�i�Ƀq�[�v���ЂƂ쐬
    tempLo = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 1);
    OS_SetArenaLo(OS_ARENA_MAIN, tempLo);
    hh = OS_CreateHeap(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi());
    if (hh < 0)
    {
        // �q�[�v�쐬�Ɏ��s�����ꍇ�ُ͈�I��
        OS_Panic("ARM9: Fail to create heap...\n");
    }
    (void)OS_SetCurrentHeap(OS_ARENA_MAIN, hh);
}

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
