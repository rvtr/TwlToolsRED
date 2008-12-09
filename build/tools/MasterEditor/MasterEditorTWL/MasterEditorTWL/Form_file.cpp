// ----------------------------------------------
// ROM�t�@�C����R/W
// ----------------------------------------------

#include "stdafx.h"
#include <apptype.h>
#include "common.h"
#include "srl.h"
#include "deliverable.h"
#include "crc_whole.h"
#include "utility.h"
#include "lang.h"
#include "split_tad.h"
#include "FormError.h"
#include "Form1.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace MasterEditorTWL;


// ----------------------------------------------
// �t�@�C���̓ǂݍ��� (TAD/SRL�ǂݍ��݂����b�v)
// ----------------------------------------------
System::Boolean Form1::loadRom( System::String ^infile )
{
	System::Boolean result = false;
	this->IsLoadTad = false;
	if( System::IO::Path::GetExtension( infile )->ToUpper()->Equals( ".TAD" ) )	// �g���q�Ŕ���
	{
		this->IsLoadTad = true;
		result = this->loadTad( infile );
	}
	else
	{
		result = this->loadSrl( infile );
	}
	return result;
}

// ----------------------------------------------
// �t�@�C���̏����o�� (TAD/SRL�����o�������b�v)
// ----------------------------------------------
System::Boolean Form1::saveRom( System::String ^outfile )
{
	System::Boolean result = false;
	if( this->IsLoadTad )
	{
		// �ꎞ�t�@�C����SRL�������o���Ă���̂ł��̈ꎞ�t�@�C������o�̓t�@�C�����쐬
		System::String ^tmpsrl = this->getSplitTadTmpFilename();
		result = this->saveSrl( tmpsrl, outfile );
		System::IO::File::Delete( tmpsrl );
	}
	else
	{
		result = this->saveSrl( this->tboxFile->Text, outfile );
	}
	return result;
}

// ----------------------------------------------
// tad�̓ǂݍ���
// ----------------------------------------------
System::Boolean Form1::loadTad( System::String ^tadfile )
{
	// tad�t�@�C����ϊ�����SRL���ꎞ�t�@�C���ɕۑ�
	System::String ^srlfile = this->getSplitTadTmpFilename();
	if( System::IO::File::Exists( srlfile ) )
	{
		System::IO::File::Delete( srlfile );	// ���łɑ��݂���ꍇ�͍폜(�A���ɓǂݍ��񂾏ꍇ�ɋN���肤��)
	}
	if( splitTad( tadfile, srlfile ) != 0 )
	{
		this->errMsg( "TAD�t�@�C���̓ǂݏo���Ɏ��s���܂����B", "Reading TAD file failed." );
		System::IO::File::Delete( srlfile );
		return false;
	}
	System::Boolean result = this->loadSrl( srlfile );	// �ꎞ�ۑ�����SRL��ǂݍ���
	return result;
}

// ----------------------------------------------
// SRL�̓ǂݍ���
// ----------------------------------------------
System::Boolean Form1::loadSrl( System::String ^srlfile )
{
	ECSrlResult result = this->hSrl->readFromFile( srlfile );
	if( result != ECSrlResult::NOERROR )
	{
		switch( result )
		{
			case ECSrlResult::ERROR_PLATFORM:
				this->errMsg( "�{�c�[����TWL�p�ł��BNTR��pROM�Ȃǂ�TWL��Ή�ROM��ǂݍ��ނ��Ƃ͂ł��܂���B",
							  "This tool can only read TWL ROM. This can't read an other data e.g. NTR limited ROM." );
			break;

			case ECSrlResult::ERROR_SIGN_DECRYPT:
			case ECSrlResult::ERROR_SIGN_VERIFY:
				this->errMsg( "�s����ROM�f�[�^�ł��BTWL�Ή�/��pROM�łȂ���ROM�f�[�^�������񂳂�Ă���\��������܂��B",
							  "Illegal ROM data. It is not for TWL ROM, or is altered illegally." );
			break;

			default:
				this->errMsg( "ROM�f�[�^�t�@�C���̓ǂݍ��݂Ɏ��s���܂����B\n�ēxROM�f�[�^��ǂݍ��܂��Ă��������B", 
					          "Reading the ROM data file failed. \nPlease read a ROM data file again, with \"Open a ROM data file\"" );
			break;
		}
		return false;
	}

	// GUI��ROM�����i�[
	this->setSrlForms();
	if( this->IsReadOnly )
	{
		this->readOnly();	// ���[�h�I�����[���[�h�̂Ƃ��t�H�[����Enable�ɂ���
	}

	// SRL�ɓo�^����Ȃ�ROM�d�l�̃t�H�[�������߂�{�^���œǂݍ��ݎ��̏�Ԃɖ߂�������
	// �ǂݍ��ݎ��̏�Ԃ�SRL�ɕۑ����Ă����Ȃ��̂őޔ����Ă���
	this->saveOtherForms();

	// �S�̂�CRC���Z�o
	u16  crc;
	if( !getWholeCRCInFile( srlfile, &crc ) )
	{
		this->errMsg( "ROM�f�[�^��CRC�v�Z�Ɏ��s���܂����BROM�f�[�^�̓ǂݍ��݂̓L�����Z������܂����B",
			          "Calculating CRC of the ROM data failed. Therefore reading ROM data is canceled." );
		return false;
	}
	System::UInt16 ^hcrc = gcnew System::UInt16( crc );
	this->tboxWholeCRC->Clear();
	this->tboxWholeCRC->AppendText( "0x" );
	this->tboxWholeCRC->AppendText( hcrc->ToString("X") );

	// �ǂݍ��ݎ��G���[��o�^����
	this->rErrorReading->Checked = true;
	this->setGridError();
	this->setGridWarn();

	return true;
} // loadSrl()

// ----------------------------------------------
// SRL�̕ۑ�
// ----------------------------------------------
System::Boolean Form1::saveSrl( System::String ^infile, System::String ^outfile )
{
	if( !System::IO::File::Exists( infile ) )
	{
		return false;
	}

	// �R�s�[�����t�@�C����ROM�w�b�_���㏑��
	if( !this->saveSrlCore( infile, outfile ) )
	{
		return false;
	}

	// �ă��[�h
	this->loadSrl( outfile );
	return true;
} // saveSrl()

System::Boolean Form1::saveSrlCore( System::String ^infile, System::String ^outfile )
{
	// ROM�����t�H�[������擾����SRL�o�C�i���ɔ��f������
	this->setSrlProperties();

	// �t�@�C�����R�s�[
	if( !(outfile->Equals( infile )) )
	{
		System::IO::File::Copy( infile, outfile, true );
	}

	// �R�s�[�����t�@�C����ROM�w�b�_���㏑��
	if( this->hSrl->writeToFile( outfile ) != ECSrlResult::NOERROR )
	{
		return false;
	}
	return true;
}
