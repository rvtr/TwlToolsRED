#pragma once

// ���ރN���X�̐錾

#include <apptype.h>
#include <twl/types.h>
#include <twl/os/common/format_rom.h>
#include <apptype.h>
#include "common.h"
#include "srl.h"

namespace MasterEditorTWL
{
	// -------------------------------------------------------------------
	// Type : enum class
	// Name : ECDeliverableResult
	//
	// Description : RCDeliverable �N���X�̑���ł̃G���[��錾
	// -------------------------------------------------------------------
	enum class ECDeliverableResult
	{
		NOERROR   = 0,
		// �G���[���肵�Ȃ��Ă��������킩��Ƃ��̕Ԃ�l
		// (�G���[��������\���̂���ӏ���1�� etc.)
		ERROR,
		// �t�@�C������ł̃G���[
		ERROR_FILE_OPEN,
		ERROR_FILE_READ,
		ERROR_FILE_WRITE,
	};

	// -------------------------------------------------------------------
	// Type : ref class
	// Name : RCDeliverable
	//
	// Description : ��o���ރN���X
	// 
	// Role : ��o���̓��o��
	// -------------------------------------------------------------------
	ref class RCDeliverable
	{
		// field
	public:
		// ��o���
		property System::String ^hProductName;		// ���i��
		property System::String ^hProductCode1;		// ���i�R�[�h
		property System::String ^hProductCode2;		// ���i�R�[�h
		property System::Int32  ^hReleaseYear;		// �����\���
		property System::Int32  ^hReleaseMonth;
		property System::Int32  ^hReleaseDay;
		property System::Int32  ^hSubmitYear;		// ��o��
		property System::Int32  ^hSubmitMonth;
		property System::Int32  ^hSubmitDay;
		property System::String ^hSubmitWay;		// ��o���@
		property System::String ^hUsage;			// �p�r
		property System::String ^hUsageOther;		// ���̑��̗p�r
		property System::Int32  ^hSubmitVersion;	// ��o�o�[�W����
		property System::String ^hSDK;				// SDK�o�[�W����
		property System::Boolean ^hReleaseForeign;	// �C�O�ł̗\��
		property System::String  ^hProductNameForeign;
		property System::String  ^hProductCode1Foreign;
		property System::String  ^hProductCode2Foreign;

		// ��Џ��

		// �S����(1�l��)
		property System::String  ^hCompany1;		// ��Ж�
		property System::String  ^hPerson1;			// ���O
		property System::String  ^hFurigana1;		// �ӂ肪��
		property System::String  ^hTel1;			// �d�b�ԍ�
		property System::String  ^hFax1;			// FAX�ԍ�
		property System::String  ^hMail1;			// ���A�h
		property System::String  ^hNTSC1;			// NTSC User ID
		// �S����(2�l��)
		property System::String  ^hCompany2;
		property System::String  ^hPerson2;
		property System::String  ^hFurigana2;
		property System::String  ^hTel2;
		property System::String  ^hFax2;
		property System::String  ^hMail2;
		property System::String  ^hNTSC2;

		// ���l
		property System::String  ^hCaption;

		// ROM�w�b�_�s�L�ڂ�ROM�o�C�i��(SRL)�ŗL���
		property System::String  ^hBackupMemory;		// �o�b�N�A�b�v�������̎��

		// SRL���̈ꕔ�𕶎���Ŏ����Ă���
		property System::String  ^hRegion;
		property System::String  ^hCERO;
		property System::String  ^hESRB;
		property System::String  ^hUSK;
		property System::String  ^hPEGI;
		property System::String  ^hPEGI_PRT;
		property System::String  ^hPEGI_BBFC;
		property System::String  ^hOFLC;
		property System::String  ^hAppTypeOther;		// ���L����

		// SRL�ɂ͓o�^����Ȃ�ROM�d�l
		property System::Boolean  ^hIsUGC;
		property System::Boolean  ^hIsPhotoEx;

		// constructor and destructor
	public:

		// method
	public:

		//
		// ���ޏo��
		//
		// @arg [out] �o�̓t�@�C����
		// @arg [in]  ROM�o�C�i��(SRL)�ŗL���
		// @arg [in]  �t�@�C���S�̂�CRC
		// @arg [in]  SRL�̃t�@�C����(���ނɋL�q���邽�߂Ɏg�p)
		// @arg [in]  �p��t���O
		//
		ECDeliverableResult writeSpreadsheet( 
			System::String ^hFilename, RCSrl ^hSrl, System::UInt16 ^hCRC, System::String ^hSrlFilename, System::Boolean english );

	}; // end of ref class RCDeliverable

} // end of namespace MasterEditorTWL
