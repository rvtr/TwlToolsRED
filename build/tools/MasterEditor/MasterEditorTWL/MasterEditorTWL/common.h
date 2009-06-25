#pragma once

#define   METWL_WHETHER_PLATFORM_CHECK		// �v���b�g�t�H�[����TWL�Ή��ȊO���ǂ������`�F�b�N����
#define   METWL_WHETHER_SIGN_DECRYPT		// �������O���ă_�C�W�F�X�g���x���t�@�C����
#define   METWL_WHETHER_MRC					// MRC�@�\���܂߂邩
#define   METWL_LIGHT_CHECK					// ��o�������ɂ����邩

// �萔
#define   METWL_ROMSIZE_MIN				256			// �ŏ��̃f�o�C�X�e�� 256Mbit
#define   METWL_ROMSIZE_MAX				4096		// �ő��..           4Gbit
#define   METWL_ROMSIZE_MIN_NAND		1
#define   METWL_ROMSIZE_MAX_NAND		256
#define   METWL_ALLSIZE_MAX_NAND		(32*1024*1024)	// �V�X�e�����NAND�A�v���̗e�ʐ���(�P��Byte)
#define   METWL_ALLSIZE_MAX_NAND_LIC	(16*1024*1024)	// �^�p���(�_����)NAND�A�v���̗e�ʐ���(�P��Byte)
#define   METWL_MASK_REGION_JAPAN       0x00000001
#define   METWL_MASK_REGION_AMERICA     0x00000002
#define   METWL_MASK_REGION_EUROPE      0x00000004
#define   METWL_MASK_REGION_AUSTRALIA   0x00000008
#define   METWL_MASK_REGION_CHINA       0x00000010
#define   METWL_MASK_REGION_KOREA       0x00000020
#define   METWL_MASK_REGION_ALL         0xffffffff
#define   METWL_NUMOF_SHARED2FILES		6
#define   METWL_SEG3_CRC                0x0254
#define   METWL_TAD_TMP_FILENAME        "tmp.srl"		// TAD�ǂݍ��݂̍ۂ� split_tad �ňꎞ�I�ɍ쐬����t�@�C����
#define   METWL_SDK52_RELEASE			((5 << 24)|(2 << 16)|30000)		// SD�A�N�Z�X���̔��肪�K�v��SDK�o�[�W���� (5.2 RELEASE)

// ���ʂ�GUI�\��/��o�m�F���ɋL�q���镶����
#define	  METWL_STRING_UNNECESSARY_RATING_J		("���[�e�B���O�\���s�v(�S�N��)")
#define	  METWL_STRING_UNNECESSARY_RATING_E		("Rating Not Required (All ages)")
#define   METWL_STRING_UNDEFINED_RATING_J		("�s��")
#define   METWL_STRING_UNDEFINED_RATING_E		("Undefined")
#define   METWL_STRING_UNDEFINED_REGION_J		("�s��")		// �N���蓾�Ȃ�
#define   METWL_STRING_UNDEFINED_REGION_E		("Undefined")
#define   METWL_STRING_CHINA_RATING_FREE_J		("�S�N��(�����őΉ�)")
#define   METWL_STRING_CHINA_RATING_FREE_E		("All ages (China version support)")
#define   METWL_STRING_CHINA_REGION_J			("�����̂�")
#define   METWL_STRING_CHINA_REGION_E			("China only")
#define   METWL_STRING_KOREA_REGION_J			("�؍��̂�")
#define   METWL_STRING_KOREA_REGION_E			("Korea only")
