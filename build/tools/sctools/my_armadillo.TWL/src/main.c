/*---------------------------------------------------------------------------*
  Project:  TwlSDK - components - armadillo.TWL
  File:     main.c

  Copyright 2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-16#$
  $Rev: 8481 $
  $Author: sato_masaki $
 *---------------------------------------------------------------------------*/

#include    <nitro/types.h>
#include    <twl/init/crt0.h>
#include    <twl/memorymap_sp.h>
#include    <twl/os.h>
#include    <twl/os/common/codecmode.h>
#include    <nitro/pad.h>
#include    <twl/aes.h>
#include    <twl/sea.h>
#include    <twl/fatfs.h>
#include    <twl/nwm.h>
#include    <twl/mcu.h>
#include    <twl/cdc.h>
#include    <nitro/snd.h>
#include    <twl/snd/ARM7/sndex_api.h>
#include    <twl/rtc.h>
#include    <nitro/wvr.h>
#include    <twl/spi.h>
#include    "nvram_sp.h"

/*---------------------------------------------------------------------------*
    �萔��`
 *---------------------------------------------------------------------------*/
/* �e�X���b�h�D��x */
#define THREAD_PRIO_MCU         1
#define THREAD_PRIO_SPI         2
#define THREAD_PRIO_SND         6
#define THREAD_PRIO_NWM_EVENT   7
#define THREAD_PRIO_NWM_SDIO    8
#define THREAD_PRIO_FATFS       8
#define THREAD_PRIO_NWM_COMMAND 9
#define THREAD_PRIO_NWM_WPA     10
#define THREAD_PRIO_AES         12
#define THREAD_PRIO_SEA         12
#define THREAD_PRIO_RTC         12
#define THREAD_PRIO_SNDEX       14
#define THREAD_PRIO_FS          15
/* OS_THREAD_LAUNCHER_PRIORITY 16 */

/* ROM ���o�^�G���A�̊g������R�[�h */
#define ROMHEADER_FOR_CHINA_BIT     0x80
#define ROMHEADER_FOR_KOREA_BIT     0x40

/* �g�p DMA �ԍ� */
#define DMA_NO_FATFS        FATFS_DMA_4     // = 0
#define DMA_NO_NWM          NWMSP_DMA_7

/*---------------------------------------------------------------------------*
    �����֐���`
 *---------------------------------------------------------------------------*/
static void         PrintDebugInfo(void);
static OSHeapHandle InitializeAllocateSystem(void);
static void         InitializeFatfs(void);
static void         InitializeNwm(OSHeapHandle hh);
static void         DummyThread(void* arg);

static void         ReadUserInfo(void);
#ifdef  NVRAM_CONFIG_DATA_EX_VERSION
static BOOL         IsValidConfigEx(void);
static u16          GetRomValidLanguage(void);
static s32          CheckCorrectNCDEx(NVRAMConfigEx* ncdsp);
#else
static s32          CheckCorrectNCD(NVRAMConfig* ncdsp);
#endif
static void         VBlankIntr(void);



#define MIYA_MCU_FREE_REG_NO   1

#if 0
#define MIYA_MCU_FREE_REG_CODE 0x55
static u8 miya_mcu_free_register = 0x66;
#endif

static void miya_mcu_free_reg_send_pxi_data(u32 data)
{
  while (PXI_SendWordByFifo(PXI_FIFO_TAG_USER_0, data, FALSE) != PXI_FIFO_SUCCESS) {
    // do nothing
  }
}

#define MIYA_MCU_COMMAND_GET_FREE_REG   1
#define MIYA_MCU_COMMAND_GET_VOLUME     2
#define MIYA_MCU_COMMAND_GET_BRIGHTNESS 3
#define MIYA_MCU_COMMAND_SET_VOLUME     4
#define MIYA_MCU_COMMAND_SET_BRIGHTNESS 5
#define MIYA_MCU_COMMAND_SET_FREE_REG   6

static void miya_mcu_free_reg_pxi_callback(PXIFifoTag tag, u32 data, BOOL err)
{
#pragma unused(tag)
#pragma unused(err)
  
  switch( (data & 0xf) ) {
  case MIYA_MCU_COMMAND_GET_FREE_REG:
    miya_mcu_free_reg_send_pxi_data((u32)MCU_GetFreeRegister((u8)MIYA_MCU_FREE_REG_NO ));
    break;
  case MIYA_MCU_COMMAND_SET_FREE_REG:
    miya_mcu_free_reg_send_pxi_data( (u32)MCU_SetFreeRegister((u8)MIYA_MCU_FREE_REG_NO , (u8)((data >> 4) & 0xff)) );
    break;

  case MIYA_MCU_COMMAND_GET_VOLUME:
    miya_mcu_free_reg_send_pxi_data( (u32)MCU_GetVolume() );
    break;
    
  case MIYA_MCU_COMMAND_GET_BRIGHTNESS:
    miya_mcu_free_reg_send_pxi_data( (u32)MCU_GetBackLightBrightness() );
    break;
    
  case MIYA_MCU_COMMAND_SET_VOLUME:
    miya_mcu_free_reg_send_pxi_data( (u32)MCU_SetVolume( (u8)((data >> 4) & 0xff)) );
    break;
    
  case MIYA_MCU_COMMAND_SET_BRIGHTNESS:
    miya_mcu_free_reg_send_pxi_data( (u32)MCU_SetBackLightBrightness(  (u8)((data >> 4) & 0xff) ));
    break;

  default:
    miya_mcu_free_reg_send_pxi_data( (u32)0xffffffff );
    break;
  }    


}

/*---------------------------------------------------------------------------*
  Name:         TwlSpMain
  Description:  �N���x�N�^�B
  Arguments:    None.
  Returns:      None.
 *---------------------------------------------------------------------------*/
void
TwlSpMain(void)
{
    OSHeapHandle    heapHandle;

    /* OS ������ */
    OS_Init();
    PrintDebugInfo();

    /* NVRAM ���烆�[�U�[���ǂݏo�� */
    ReadUserInfo();

    /* �q�[�v�̈�ݒ� */
    heapHandle  =   InitializeAllocateSystem();

    /* �{�^�����̓T�[�`������ */
    (void)PAD_InitXYButton();

    /* �����݋��� */
    (void)OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)GX_VBlankIntr(TRUE);
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();

    /* �e���C�u���������� */
    AES_Init(THREAD_PRIO_AES);                  // AES
    SEA_Init(THREAD_PRIO_SEA);                  // SEA
//  FS_Init(FS_DMA_NOT_USE);                    // FS for CARD
//  FS_CreateReadServerThread(THREAD_PRIO_FS);  // FS for CARD
    InitializeFatfs();                          // FAT-FS
    InitializeNwm(heapHandle);                  // TWL ����
    MCU_InitIrq(THREAD_PRIO_MCU);               // �}�C�R��
	CDC_InitLib();								// CODEC
//  if (OSi_IsCodecTwlMode() == TRUE)
//	{
//  	CAMERA_Init();							// �J����
//	}
    SND_Init(THREAD_PRIO_SND);                  // �T�E���h
    SNDEX_Init(THREAD_PRIO_SNDEX);              // �T�E���h�g��
    RTC_Init(THREAD_PRIO_RTC);                  // RTC
    WVR_Begin(heapHandle);                      // NITRO ����
    SPI_Init(THREAD_PRIO_SPI);


    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_USER_0, miya_mcu_free_reg_pxi_callback);


    while (TRUE)
    {
        OS_Halt();
        /* �\�t�g�E�F�A���Z�b�g�v���͊Ď����Ȃ� */
        /* AGB �J�[�g���b�W�̑}���`�F�b�N�͍s��Ȃ� */
        /* DS �J�[�h�̑}���`�F�b�N�͍s��Ȃ� */

	/*
	  DS �J�[�h�̑}���`�F�b�N�͍s���B
	  ��������Ȃ��ƃo�b�N�A�b�v�A���X�g�A�͊������Ȃ��悤�ɂ����̂ŁB
	*/
        CARD_CheckPullOut_Polling();

    

    }
}

/*---------------------------------------------------------------------------*
  Name:         PrintDebugInfo
  Description:  ARM7 �R���|�[�l���g�̏����f�o�b�O�o�͂���B
  Arguments:    None.
  Returns:      None.
 *---------------------------------------------------------------------------*/
static void
PrintDebugInfo(void)
{
    OS_TPrintf("ARM7: This component is running on TWL.\n");
    OS_TPrintf("ARM7: This component is \"armadillo.TWL\".\n");
}

/*---------------------------------------------------------------------------*
  Name:         InitializeAllocateSystem
  Description:  �����������ăV�X�e��������������B
  Arguments:    None.
  Returns:      OSHeapHandle - WRAM �A���[�i��Ɋm�ۂ��ꂽ�q�[�v�̃n���h����Ԃ��B
 *---------------------------------------------------------------------------*/
static OSHeapHandle
InitializeAllocateSystem(void)
{
    OSHeapHandle    hh;
    void*           subLo   =   (void*)OS_GetWramSubArenaLo();
    void*           subHi   =   (void*)OS_GetWramSubArenaHi();
    void*           privLo  =   (void*)OS_GetWramSubPrivArenaLo();
    void*           privHi  =   (void*)OS_GetWramSubPrivArenaHi();

    if (((u32)privLo == HW_PRV_WRAM) && ((u32)subHi == HW_PRV_WRAM) && ((u32)subLo < HW_PRV_WRAM))
    {
        /* SUB �A���[�i�� SUBPRIV �A���[�i�ɋz�� */
        OS_SetWramSubArenaHi(subLo);
        OS_SetWramSubPrivArenaLo(subLo);
        privLo  =   subLo;
    }

    /* �A���[�i�� 0 �N���A */
    MI_CpuClear8(privLo, (u32)privHi - (u32)privLo);

    /* �q�[�v�쐬������ */
    privLo  =   OS_InitAlloc(OS_ARENA_WRAM_SUBPRIV, privLo, privHi, 1);
    hh  =   OS_CreateHeap(OS_ARENA_WRAM_SUBPRIV, privLo, privHi);
    if (hh < 0)
    {
        OS_Panic("ARM7: Failed to create heap.\n");
    }

    /* �J�����g�q�[�v�ɐݒ� */
    (void)OS_SetCurrentHeap(OS_ARENA_WRAM_SUBPRIV, hh);

    /* �q�[�v�T�C�Y�̊m�F */
    {
        u32     heapSize;
    
        heapSize    =   (u32)OS_CheckHeap(OS_ARENA_WRAM_SUBPRIV, hh);
    
        if (heapSize <= 0)
        {
            OS_Panic("ARM7: Failed to create heap.\n");
        }
        OS_TPrintf("ARM7: heap size [%d]\n", heapSize);
    }

    return hh;
}

/*---------------------------------------------------------------------------*
  Name:         InitializeFatfs
  Description:  FATFS���C�u����������������BFATFS�������֐����ŃX���b�h�x�~
                ����ׁA�x�~�����삷��_�~�[�̃X���b�h�𗧂Ă�B
  Arguments:    None.
  Returns:      None.
 *---------------------------------------------------------------------------*/
static void
InitializeFatfs(void)
{
    /* FATFS ������ */
    /* [TODO] DMA �� NOT_USE �̂܂܂ŗǂ��H */
    if(!FATFS_Init(DMA_NO_FATFS, FATFS_DMA_NOT_USE, THREAD_PRIO_FATFS))
    {
        /* do nothing */
    }
}

/*---------------------------------------------------------------------------*
  Name:         InitializeNwm
  Description:  NWM���C�u����������������B
  Arguments:    hh  -   ���p�\�ȃq�[�v�̃n���h�����w��B
  Returns:      None.
 *---------------------------------------------------------------------------*/
static void
InitializeNwm(OSHeapHandle hh)
{
    NwmspInit   nwmInit;

    /* [TODO] �m�ۂ��Ă���q�[�v�̈悪�V�����ꎮ���K�v�Ƃ��Ă��郁�����ʈȏォ�̃`�F�b�N���K�v */

    nwmInit.cmdPrio         =   THREAD_PRIO_NWM_COMMAND;
    nwmInit.evtPrio         =   THREAD_PRIO_NWM_EVENT;
    nwmInit.sdioPrio        =   THREAD_PRIO_NWM_SDIO;

    nwmInit.dmaNo           =   DMA_NO_NWM;
    nwmInit.drvHeap.id      =   OS_ARENA_WRAM_SUBPRIV;
    nwmInit.drvHeap.handle  =   hh;

    nwmInit.wpaPrio         =   THREAD_PRIO_NWM_WPA;
    nwmInit.wpaHeap.id      =   OS_ARENA_WRAM_SUBPRIV;
    nwmInit.wpaHeap.handle  =   hh;

    NWMSP_Init(&nwmInit);
}

/*---------------------------------------------------------------------------*
  Name:         DummyThread
  Description:  FATFS���C�u�����ACDC���C�u����������������ۂɗ��Ă�_�~�[��
                �X���b�h�B
  Arguments:    arg -   �g�p���Ȃ��B
  Returns:      None.
 *---------------------------------------------------------------------------*/
static void
DummyThread(void* arg)
{
#pragma unused(arg)
    while (TRUE)
    {
    }
}

#ifdef  WM_PRECALC_ALLOWEDCHANNEL
extern u16 WMSP_GetAllowedChannel(u16 bitField);
#endif
/*---------------------------------------------------------------------------*
  Name:         ReadUserInfo

  Description:  NVRAM���烆�[�U�[����ǂݏo���A���L�̈�ɓW�J����B
                �~���[�����O����Ă���o�b�t�@���������Ă���ꍇ�́A
                ���L�̈�̃��[�U�[���i�[�ꏊ���N���A����B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ReadUserInfo(void)
{
    s32     offset;
#ifdef  NVRAM_CONFIG_DATA_EX_VERSION
    NVRAMConfigEx temp[2];
#else
    NVRAMConfig temp[2];
#endif
    s32     check;
    u8     *p = OS_GetSystemWork()->nvramUserInfo;

    // �I�t�Z�b�g�ǂݏo��
#ifdef  NVRAM_CONFIG_CONST_ADDRESS
    offset = NVRAM_CONFIG_DATA_ADDRESS_DUMMY;
#else
    NVRAM_ReadDataBytes(NVRAM_CONFIG_DATA_OFFSET_ADDRESS, NVRAM_CONFIG_DATA_OFFSET_SIZE,
                        (u8 *)(&offset));
    offset <<= NVRAM_CONFIG_DATA_OFFSET_SHIFT;
#endif

#ifdef  NVRAM_CONFIG_DATA_EX_VERSION
    // �~���[���ꂽ�Q�̃f�[�^��ǂݏo��
    NVRAM_ReadDataBytes((u32)offset, sizeof(NVRAMConfigEx), (u8 *)(&temp[0]));
    NVRAM_ReadDataBytes((u32)(offset + SPI_NVRAM_PAGE_SIZE), sizeof(NVRAMConfigEx),
                        (u8 *)(&temp[1]));
    // �Q�̓��ǂ�����g�������f
    check = CheckCorrectNCDEx(temp);
#else
    // �~���[���ꂽ�Q�̃f�[�^��ǂݏo��
    NVRAM_ReadDataBytes((u32)offset, sizeof(NVRAMConfig), (u8 *)(&temp[0]));
    NVRAM_ReadDataBytes((u32)(offset + SPI_NVRAM_PAGE_SIZE), sizeof(NVRAMConfig), (u8 *)(&temp[1]));
    // �Q�̓��ǂ�����g�������f
    check = CheckCorrectNCD(temp);
#endif

    if (check >= 3)
    {
        // �A�v���P�[�V�����̋N����}��
        MI_CpuFill32(p, 0xffffffff, sizeof(NVRAMConfig));
    }
    else if (check)
    {
        s32     i;

        // �j�b�N�l�[����␳
        if (temp[check - 1].ncd.owner.nickname.length < NVRAM_CONFIG_NICKNAME_LENGTH)
        {
            for (i = NVRAM_CONFIG_NICKNAME_LENGTH;
                 i > temp[check - 1].ncd.owner.nickname.length; i--)
            {
                temp[check - 1].ncd.owner.nickname.str[i - 1] = 0x0000;
            }
        }
        // �R�����g��␳
        if (temp[check - 1].ncd.owner.comment.length < NVRAM_CONFIG_COMMENT_LENGTH)
        {
            for (i = NVRAM_CONFIG_COMMENT_LENGTH; i > temp[check - 1].ncd.owner.comment.length;
                 i--)
            {
                temp[check - 1].ncd.owner.comment.str[i - 1] = 0x0000;
            }
        }
        // ���L�̈�ɃX�g�A
        MI_CpuCopy32(&temp[check - 1], p, sizeof(NVRAMConfig));
    }
    else
    {
        // ���L�̈���N���A
        MI_CpuClear32(p, sizeof(NVRAMConfig));
    }

    // ����MAC�A�h���X�����[�U�[���̌��ɓW�J
    {
        u8      wMac[6];

        // NVRAM����MAC�A�h���X��ǂݏo��
        NVRAM_ReadDataBytes(NVRAM_CONFIG_MACADDRESS_ADDRESS, 6, wMac);
        // �W�J��A�h���X���v�Z
        p = (u8 *)((u32)p + ((sizeof(NVRAMConfig) + 3) & ~0x00000003));
        // ���L�̈�ɓW�J
        MI_CpuCopy8(wMac, p, 6);
    }

#ifdef  WM_PRECALC_ALLOWEDCHANNEL
    // �g�p�\�`�����l������g�p���`�����l�����v�Z
    {
        u16     enableChannel;
        u16     allowedChannel;

        // �g�p�\�`�����l����ǂݏo��
        NVRAM_ReadDataBytes(NVRAM_CONFIG_ENABLECHANNEL_ADDRESS, 2, (u8 *)(&enableChannel));
        // �g�p���`�����l�����v�Z
        allowedChannel = WMSP_GetAllowedChannel((u16)(enableChannel >> 1));
        // �W�J��A�h���X���v�Z(MAC�A�h���X�̌���2�o�C�g)
        p = (u8 *)((u32)p + 6);
        // ���L�̈�ɓW�J
        *((u16 *)p) = allowedChannel;
    }
#endif
}

#ifdef  NVRAM_CONFIG_DATA_EX_VERSION
/*---------------------------------------------------------------------------*
  Name:         IsValidConfigEx

  Description:  ���[�U�[��񂪊g���R���t�B�O�ɑΉ����Ă��邩�ǂ����𒲍�����B

  Arguments:    None.

  Returns:      BOOL    - �g�����[�U�[��񂪗L���ȏꍇ��TRUE��Ԃ��B
                          �����ł���ꍇ��FALSE��Ԃ��B
 *---------------------------------------------------------------------------*/
static BOOL IsValidConfigEx(void)
{
    u8      ipl2_type;

    NVRAM_ReadDataBytes(NVRAM_CONFIG_IPL2_TYPE_ADDRESS, NVRAM_CONFIG_IPL2_TYPE_SIZE, &ipl2_type);
    if (ipl2_type == NVRAM_CONFIG_IPL2_TYPE_NORMAL)
    {
        return FALSE;
    }
    if (ipl2_type & NVRAM_CONFIG_IPL2_TYPE_EX_MASK)
    {
        return TRUE;
    }
    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         GetRomValidLanguage

  Description:  ROM���o�^�G���A�̏�񂩂�A�g������R�[�h�̑Ή�����r�b�g�}�b�v
                �Ɋւ�����𒊏o����B

  Arguments:    None.

  Returns:      u16     - DS�J�[�h�A�������� �}���`�u�[�g�o�C�i�����Ή����Ă���
                          ����R�[�h�̃r�b�g�}�b�v��Ԃ��BDS�J�[�h���g������
                          �R�[�h�ɑΉ����Ă��Ȃ��ꍇ�� 0 ��Ԃ��B
 *---------------------------------------------------------------------------*/
static u16 GetRomValidLanguage(void)
{
    u16     ret = 0x0000;
    u8      langBit = OS_GetSystemWork()->rom_header[0x1d];

    // ROM���o�^�G���A�̊g������R�[�h���m�F
    if (langBit == ROMHEADER_FOR_CHINA_BIT)
    {
        // for CHINA
        ret |= (0x0001 << NVRAM_CONFIG_LANG_CHINESE);
    }
    else if (langBit == ROMHEADER_FOR_KOREA_BIT)
    {
        // for KOREA
        ret |= (0x0001 << NVRAM_CONFIG_LANG_HANGUL);
    }
    return ret;
}

/*---------------------------------------------------------------------------*
  Name:         CheckCorrectNCDEx

  Description:  �~���[�����O����Ă��郆�[�U�[���̂ǂ�����g���ׂ������肷��B

  Arguments:    nvdsp   - ��r����R���t�B�O�f�[�^�Q�̔z��B

  Returns:      s32     - 0: �����s�K�؁B
                          1: �z��[ 0 ]���K�؁B
                          2: �z��[ 1 ]���K�؁B
                          3: �A�v���̋N����}�����ׂ��B
 *---------------------------------------------------------------------------*/
static s32 CheckCorrectNCDEx(NVRAMConfigEx * ncdsp)
{
    u16     i;
    u16     calc_crc;
    s32     crc_flag = 0;
    u16     saveCount;

    // IPL���g������R�[�h�ɑΉ����Ă��邩
    if (IsValidConfigEx())
    {
        // IPL���g������R�[�h�ɑΉ����Ă���ꍇ
        u16     rom_valid_language = GetRomValidLanguage();

        for (i = 0; i < 2; i++)
        {
            calc_crc = SVC_GetCRC16(0xffff, (void *)(&ncdsp[i].ncd), sizeof(NVRAMConfigData));
            if ((ncdsp[i].crc16 == calc_crc) && (ncdsp[i].saveCount < NVRAM_CONFIG_SAVE_COUNT_MAX))
            {
                // CRC �������� saveCount �l�� 0x80 �����̃f�[�^�𐳓��Ɣ��f
                calc_crc =
                    SVC_GetCRC16(0xffff, (void *)(&ncdsp[i].ncd_ex), sizeof(NVRAMConfigDataEx));
                if ((ncdsp[i].crc16_ex == calc_crc)
                    && ((0x0001 << ncdsp[i].ncd_ex.language) &
                        (ncdsp[i].ncd_ex.valid_language_bitmap)))
                {
                    // �g���f�[�^�p CRC ���������A�ݒ茾��R�[�h���Ή�����R�[�h�Ɋ܂܂��ꍇ�ɐ����Ɣ��f
                    if (rom_valid_language & ncdsp[i].ncd_ex.valid_language_bitmap)
                    {
                        // �g������R�[�h�Œʏ팾��R�[�h���㏑��
                        ncdsp[i].ncd.option.language = ncdsp[i].ncd_ex.language;
                    }
                    if (rom_valid_language & (0x0001 << NVRAM_CONFIG_LANG_CHINESE) & ~ncdsp[i].
                        ncd_ex.valid_language_bitmap)
                    {
                        // ROM ���o�^�G���A��"������"�g������R�[�h���ݒ肳��Ă��邪�A
                        // IPL2�̑Ή�����R�[�h��"������"�g������R�[�h���܂܂�Ȃ��ꍇ�͋N����}��
                        return 3;
                    }
                    crc_flag |= (1 << i);
                }
            }
        }
    }
    else
    {
        // IPL���g������R�[�h�ɑΉ����Ă��Ȃ��ꍇ
        u16     rom_valid_language = GetRomValidLanguage();

        if (rom_valid_language & (0x0001 << NVRAM_CONFIG_LANG_CHINESE))
        {
            // ROM ���o�^�G���A��"������"�g������R�[�h���ݒ肳��Ă���ꍇ�͋N����}��
            return 3;
        }
        for (i = 0; i < 2; i++)
        {
            calc_crc = SVC_GetCRC16(0xffff, (void *)(&ncdsp[i].ncd), sizeof(NVRAMConfigData));
            if ((ncdsp[i].crc16 == calc_crc) && (ncdsp[i].saveCount < NVRAM_CONFIG_SAVE_COUNT_MAX))
            {
                // CRC �������� saveCount �l�� 0x80 �����̃f�[�^�𐳓��Ɣ��f
                crc_flag |= (1 << i);
            }
        }
    }

    // �����ȃf�[�^�̂����ǂ̃f�[�^���L�����𔻒肷��B
    switch (crc_flag)
    {
    case 1:
    case 2:
        // �Е���CRC��������
        return crc_flag;

    case 3:
        // �����Ƃ�CRC����������΂ǂ��炪�ŐV�̃f�[�^�����f����B
        saveCount = (u8)((ncdsp[0].saveCount + 1) & NVRAM_CONFIG_SAVE_COUNT_MASK);
        if (saveCount == ncdsp[1].saveCount)
        {
            return 2;
        }
        return 1;
    }

    // �����Ƃ�CRC���s��
    return 0;
}

#else
/*---------------------------------------------------------------------------*
  Name:         CheckCorrectNCD

  Description:  �~���[�����O����Ă��郆�[�U�[���̂ǂ�����g���ׂ������肷��B

  Arguments:    nvdsp   - ��r����R���t�B�O�f�[�^�Q�̔z��B

  Returns:      s32     - 0: �����s�K�؁B
                          1: �z��[ 0 ]���K�؁B
                          2: �z��[ 1 ]���K�؁B
 *---------------------------------------------------------------------------*/
static s32 CheckCorrectNCD(NVRAMConfig *ncdsp)
{
    u16     i;
    u16     calc_crc;
    s32     crc_flag = 0;
    u16     saveCount;

    // �e�~���[�f�[�^��CRC & saveCount�������`�F�b�N
    for (i = 0; i < 2; i++)
    {
        calc_crc = SVC_GetCRC16(0xffff, (void *)(&ncdsp[i].ncd), sizeof(NVRAMConfigData));

        if ((ncdsp[i].crc16 == calc_crc) && (ncdsp[i].saveCount < NVRAM_CONFIG_SAVE_COUNT_MAX))
        {
            // CRC���������AsaveCount�l��0x80�����̃f�[�^�𐳓��Ɣ��f�B
            crc_flag |= (1 << i);
        }
    }

    // �����ȃf�[�^�̂����ǂ̃f�[�^���L�����𔻒肷��B
    switch (crc_flag)
    {
    case 1:
    case 2:
        // �Е���CRC��������
        return crc_flag;

    case 3:
        // �����Ƃ�CRC����������΂ǂ��炪�ŐV�̃f�[�^�����f����B
        saveCount = (u8)((ncdsp[0].saveCount + 1) & NVRAM_CONFIG_SAVE_COUNT_MASK);
        if (saveCount == ncdsp[1].saveCount)
        {
            return 2;
        }
        return 1;
    }

    // �����Ƃ�CRC���s��
    return 0;
}
#endif

/*---------------------------------------------------------------------------*
  Name:         VBlankIntr
  Description:  V �u�����N���荞�݃x�N�^�B
  Arguments:    None.
  Returns:      None.
 *---------------------------------------------------------------------------*/
extern BOOL PMi_Initialized;
extern void PM_SelfBlinkProc(void);

static void
VBlankIntr(void)
{
    if (PMi_Initialized)
    {
        PM_SelfBlinkProc();
    }
}
