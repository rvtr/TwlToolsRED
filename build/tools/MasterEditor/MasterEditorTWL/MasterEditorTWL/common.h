#pragma once

#define   METWL_WHETHER_PLATFORM_CHECK		// �v���b�g�t�H�[����TWL�Ή��ȊO���ǂ������`�F�b�N����
#define   METWL_WHETHER_SIGN_DECRYPT		// �������O���ă_�C�W�F�X�g���x���t�@�C����
#define   METWL_WHETHER_MRC					// MRC�@�\���܂߂邩
#define   METWL_LIGHT_CHECK					// ��o�������ɂ����邩

// �萔
#define   METWL_ERRLIST_NORANGE			0			// �G���[�̃A�h���X�͈͂��w�肵�Ȃ����Ƃ�\�����ʂȒl
#define   METWL_ROMSIZE_MIN				256			// �ŏ��̃f�o�C�X�e�� 256Mbit
#define   METWL_ROMSIZE_MAX				4096		// �ő��..           4Gbit
#define   METWL_ROMSIZE_MIN_NAND		1
#define   METWL_ROMSIZE_MAX_NAND		256
#define   METWL_ALLSIZE_MAX_NAND		(32*1024*1024)	// Public/Private�Z�[�u�f�[�^�̃T�C�Y���܂߂�NAND�A�v���̗e�ʐ���(�P��Byte)
#define   METWL_MASK_REGION_JAPAN       0x00000001
#define   METWL_MASK_REGION_AMERICA     0x00000002
#define   METWL_MASK_REGION_EUROPE      0x00000004
#define   METWL_MASK_REGION_AUSTRALIA   0x00000008
#define   METWL_MASK_REGION_CHINA       0x00000010
#define   METWL_MASK_REGION_KOREA       0x00000020
#define   METWL_MASK_REGION_ALL         0xffffffff
#define   METWL_NUMOF_SHARED2FILES		6
#define   METWL_SEG3_CRC                0x0254
