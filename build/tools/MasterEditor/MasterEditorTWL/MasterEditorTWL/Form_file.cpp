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
	if( System::IO::Path::GetExtension( infile )->ToUpper()->Equals( ".TAD" ) )	// �g���q�Ŕ���
	{
		result = this->loadTad( infile );
		if( result )
		{
			this->IsLoadTad = true;		// ���������Ƃ��݂̂łȂ��Ǝ���TAD�̓ǂݍ��݂Ɏ��s�����Ƃ���
										// ���[�h�t���O�̏�񂪏���������Ď����Ă��܂�
		}
	}
	else
	{
		result = this->loadSrl( infile );
		if( result )
		{
			this->IsLoadTad = false;
			System::String ^tmpsrl = this->getSplitTadTmpFile();
			if( System::IO::File::Exists( tmpsrl ) )
			{
				System::IO::File::Delete( tmpsrl );		// SRL�̓ǂݍ��݂�����������ȑO��TAD�ǂݍ��݂Ŏg�p�����ꎞ�t�@�C���͕s�v
			}
		}
	}
	return result;
}

// ----------------------------------------------
// �t�@�C���̏����o�� (TAD/SRL�����o�������b�v)
// ----------------------------------------------
ECFormResult Form1::saveRom( System::String ^outfile )
{
	ECFormResult result = ECFormResult::NOERROR;
	if( this->IsLoadTad )
	{
		// �ꎞ�t�@�C����SRL�������o���Ă���̂ł��̈ꎞ�t�@�C������o�̓t�@�C�����쐬
		System::String ^tmpsrl = this->getSplitTadTmpFile();
		result = this->saveSrl( tmpsrl, outfile );
		if( result != ECFormResult::NOERROR )
		{
			System::IO::File::Delete( tmpsrl );
			this->IsLoadTad = false;			// �o�͂���SRL�����̃\�[�X�ƂȂ�
		}
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
	System::String ^srlfile = this->getSplitTadTmpFile();
	if( splitTad( tadfile, srlfile ) != 0 )		// �㏑���ŕۑ�
	{
		this->errMsg( "TAD�t�@�C���̓ǂݍ��݂Ɏ��s���܂����B", "Reading TAD file failed." );
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
ECFormResult Form1::saveSrl( System::String ^infile, System::String ^outfile )
{
	if( !System::IO::File::Exists( infile ) )
	{
		return ECFormResult::ERROR_FILE_EXIST;
	}

	// �R�s�[�����t�@�C����ROM�w�b�_���㏑��
	ECFormResult result = this->saveSrlCore( infile, outfile );
	if( result != ECFormResult::NOERROR )
	{
		return result;
	}

	// �ă��[�h
	this->loadSrl( outfile );
	return ECFormResult::NOERROR;
} // saveSrl()

ECFormResult Form1::saveSrlCore( System::String ^infile, System::String ^outfile )
{
	// ROM�����t�H�[������擾����SRL�o�C�i���ɔ��f������
	this->setSrlProperties();

	// �t�@�C�����R�s�[
	if( !(outfile->Equals( infile )) )
	{
		//try
		//{
		//	System::IO::File::Copy( infile, outfile, true );
		//}
		//catch( System::Exception ^ex )
		//{
		//	(void)ex;
		//	return ECFormResult::ERROR_FILE_COPY;
		//}
		ECFormResult formRes = this->copyFile( infile, outfile );
		if( formRes != ECFormResult::NOERROR )
		{
			return ECFormResult::ERROR_FILE_COPY;
		}
	}

	// �R�s�[�����t�@�C����ROM�w�b�_���㏑��
	ECSrlResult srlRes = this->hSrl->writeToFile( outfile );
	if( srlRes != ECSrlResult::NOERROR )
	{
		ECFormResult formRes = ECFormResult::NOERROR; 
		switch( srlRes )
		{
			case ECSrlResult::ERROR_FILE_OPEN:  formRes = ECFormResult::ERROR_FILE_OPEN;  break;
			case ECSrlResult::ERROR_FILE_WRITE: formRes = ECFormResult::ERROR_FILE_WRITE; break;
			case ECSrlResult::ERROR_FILE_READ:  formRes = ECFormResult::ERROR_FILE_READ;  break;
			case ECSrlResult::ERROR_SIGN_ENCRYPT:
			case ECSrlResult::ERROR_SIGN_DECRYPT:
			case ECSrlResult::ERROR_SIGN_VERIFY:
				formRes = ECFormResult::ERROR_FILE_SIGN;
			break;
			default:
				formRes = ECFormResult::ERROR;
			break;
		}
		return formRes;
	}
	return ECFormResult::NOERROR;
}

// ----------------------------------------------
// �t�@�C���̃R�s�[
// ----------------------------------------------
#define METWL_COPY_FILE_BUFSIZE   (1*1024*1024)
ECFormResult Form1::copyFile( System::String ^infile, System::String ^outfile )
{
	const char *pchInfile = 
		(const char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( infile ).ToPointer();
	const char *pchOutfile = 
		(const char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( outfile ).ToPointer();

	FILE       *ifp = NULL;
	if( fopen_s( &ifp, pchInfile, "rb" ) != NULL )
	{
		this->errMsg(
			"�t�@�C���R�s�[�ɂ����ē��̓t�@�C���̃I�[�v���Ɏ��s���܂����B",
			"In Copying file, the input file can't be opened." );
		return (ECFormResult::ERROR_FILE_OPEN);
	}
	FILE       *ofp = NULL;
	if( fopen_s( &ofp, pchOutfile, "wb" ) != NULL )	// �����t�@�C�����폜���ĐV�K�Ƀ��C�g�E�o�C�i��
	{
		this->errMsg(
			"�t�@�C���R�s�[�ɂ����ďo�̓t�@�C���̃I�[�v���Ɏ��s���܂����B",
			"In Copying file, the output file can't be opened." );
		fclose(ifp);
		return (ECFormResult::ERROR_FILE_OPEN);
	}

	// ���̓t�@�C���̃T�C�Y�擾
	fseek( ifp, 0, SEEK_END );
	System::Int32 size = ftell(ifp);

	cli::array<System::Byte> ^mbuf = gcnew cli::array<System::Byte>(METWL_COPY_FILE_BUFSIZE);	// ����̕K�v�Ȃ�
	pin_ptr<unsigned char> buf = &mbuf[0];
	fseek( ifp, 0, SEEK_SET );
	fseek( ofp, 0, SEEK_SET );
	while( size > 0 )
	{
		System::Int32 datasize = (METWL_COPY_FILE_BUFSIZE < size)?METWL_COPY_FILE_BUFSIZE:size;

		if( datasize != fread(buf, 1, datasize, ifp) )
		{
			this->errMsg(
				"�t�@�C���R�s�[�ɂ����ē��̓t�@�C������̃f�[�^���[�h�Ɏ��s���܂����B",
				"In Copying file, contents can't be read from the input file." );
			fclose(ofp);
			fclose(ifp);
			return ECFormResult::ERROR_FILE_READ;
		}
		if( datasize != fwrite(buf, 1, datasize, ofp) )
		{
			this->errMsg(
				"�t�@�C���R�s�[�ɂ����ďo�̓t�@�C������̃f�[�^���C�g�Ɏ��s���܂����B",
				"In Copying file, contents can't be written to the output file." );
			fclose(ofp);
			fclose(ifp);
			return ECFormResult::ERROR_FILE_READ;
		}
		size -= datasize;
	}
	fclose(ofp);
	fclose(ifp);
	return ECFormResult::NOERROR;
}
