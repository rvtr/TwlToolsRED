#pragma once

// ROM�f�[�^(SRL)�N���X�֘A�̒萔�錾

#include <apptype.h>
#include "common.h"
#include <twl/types.h>
#include <twl/os/common/format_rom.h>
#include <twl/os/common/ownerInfoEx.h>
#include "utility.h"

namespace MasterEditorTWL
{
	// -------------------------------------------------------------------
	// Type : enum class
	// Name : ECSrlResult
	//
	// Description : RCSrl�N���X�̑���ł̃G���[��錾
	// -------------------------------------------------------------------
	enum class ECSrlResult
	{
		NOERROR   = 0,
		// �G���[���肵�Ȃ��Ă��������킩��Ƃ��̕Ԃ�l
		// (�G���[��������\���̂���ӏ���1�� etc.)
		ERROR,
		// �t�@�C������ł̃G���[
		ERROR_FILE_OPEN,
		ERROR_FILE_READ,
		ERROR_FILE_WRITE,
		// �����ł̃G���[
		ERROR_SIGN_ENCRYPT,	// �����ł��Ȃ�
		ERROR_SIGN_DECRYPT,	// ���������ł��Ȃ�
		ERROR_SIGN_VERIFY,	// ��������������̃_�C�W�F�X�g������Ȃ�
		// CRC�Z�o�ł̃G���[
		ERROR_SIGN_CRC,
		// SDK�o�[�W�����擾�ł̃G���[
		ERROR_SDK,
		// �v���b�g�t�H�[����NTR��p or �s���o�C�i��
		ERROR_PLATFORM,
	};

	// -------------------------------------------------------------------
	// Type : enum class
	// Name : ECAppType
	//
	// Description : RCSrl�N���X�̃A�v����ʂ�錾
	// -------------------------------------------------------------------
	enum class ECAppType
	{
		USER   = 0,
		SYSTEM,
		SECURE,
		LAUNCHER,
		ILLEGAL,	// �s��
	}; //ECAppType

} // end of namespace MasterEditorTWL