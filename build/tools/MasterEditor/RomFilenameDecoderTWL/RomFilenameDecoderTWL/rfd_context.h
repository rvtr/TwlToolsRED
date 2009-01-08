#pragma once

#include <twl/types.h>
#include <twl/os/common/format_rom.h>
#include <cstdio>
#include <cstring>
#include "rfd_util.h"

namespace RomFilenameDecoderTWL
{

// -----------------------------------------------------------
// �G���[�R�[�h
// -----------------------------------------------------------
enum class ECError
{
	NOERROR                =  0,
	ERROR_READ_FILE        = -1,
	ERROR_FILE_NAME        = -2,
	// -3..-7 �\��
	ERROR_INITIAL_CODE     = -8,
	ERROR_REMASTER_VERSION = -9,
};

// -----------------------------------------------------------
// ���scontext
// -----------------------------------------------------------
ref class RCContext
{
private:
	// �����̉�͌���
	System::String ^romfile;	// ���̓t�@�C����
	System::Int32   argc;		// �����̐�
	// �I�v�V������\���t���O
	System::Boolean isSubmitVersion;
	System::Boolean isTadVersion;
public:
	// ReadOnly�ɂ���
	property System::String^ Romfile
	{
		System::String^ get(void)
		{ 
			if( !romfile )
			{
				return nullptr;
			}
			return System::String::Copy( this->romfile );
		}
	}
	property System::Int32 Argc
	{
		System::Int32 get(void){ return this->argc; }
	}
	property System::Boolean IsSubmitVersion
	{
		System::Boolean get(void){ return this->isSubmitVersion; }
	}
	property System::Boolean IsTadVersion
	{
		System::Boolean get(void){ return this->isTadVersion; }
	}
private:
	RCContext(){}	// ������
public:
	// �R���X�g���N�^�ł͈�����������ăt���O����͂���
	RCContext( cli::array<System::String ^> ^args )
	{
		this->parseArgs( args );
	}
private:
	// �������
	void parseArgs( cli::array<System::String ^> ^args )
	{
		System::Collections::Generic::List<System::Int32> ^indexList
			= gcnew System::Collections::Generic::List<System::Int32>;

		int numopt = 0;
		int i;
		for( i=0; i < args->Length; i++ )
		{
			if( args[i]->StartsWith( "-s" ) )
			{
				this->isSubmitVersion = true;
				numopt++;
			}
			else if( args[i]->StartsWith( "-t" ) )
			{
				this->isTadVersion = true;
				numopt++;
			}
			else if( !args[i]->StartsWith( "-" ) )	// �I�v�V�����łȂ�������index���L�^
			{
				indexList->Add(i);
			}
		}
		i=0;
		for each( System::Int32 index in indexList )	// �I�v�V�����łȂ�������O�ɂ߂Ă���
		{
			args[i] = args[index];
			i++;
		}
		this->argc    = args->Length - numopt;		// �����̐�����I�v�V�����̐�������
		if( this->argc > 0 )
		{
			this->romfile = System::String::Copy( args[0] );
		}
		else
		{
			this->romfile = nullptr;
		}
	}
}; //ref class RCContext

// -----------------------------------------------------------
// �t�@�C��������擾�ł�����
// -----------------------------------------------------------
ref class RCFilenameItems
{
private:
	// �t�@�C��������擾�ł�����
	System::String^ initialCode;
	System::Byte    remasterVersion;
	System::Byte    submissionVersion;
	// �t�@�C������ǂݍ���ROM�w�b�_
	ROM_Header      *rh;
	// �G���[���
	RomFilenameDecoderTWL::ECError  error;
public:
	property System::String^ InitialCode
	{
		System::String^ get(void){ return System::String::Copy(this->initialCode); }
	}
	property System::Byte RemasterVersion
	{
		System::Byte get(void){ return this->remasterVersion; }
	}
	property System::Byte SubmissionVersion
	{
		System::Byte get(void){ return this->submissionVersion; }
	}
	property const ROM_Header* Rh
	{
		const ROM_Header* get(void){ return this->rh; }
	}
	property RomFilenameDecoderTWL::ECError Error
	{
		RomFilenameDecoderTWL::ECError get(void){ return this->error; }
	}
private:
	RCFilenameItems(){}
public:
	RCFilenameItems( System::String ^filepath )
	{
		this->initialCode = gcnew System::String("");
		this->remasterVersion   = 0;
		this->submissionVersion = 0;
		this->rh = new ROM_Header;
		memset( this->rh, 0, sizeof(ROM_Header) );
		this->error = RomFilenameDecoderTWL::ECError::NOERROR;

		this->parseFilename( filepath );
		if( this->error == RomFilenameDecoderTWL::ECError::NOERROR )
		{
			this->readRomHeader( filepath );
		}
	}
	~RCFilenameItems()
	{
		this->!RCFilenameItems();
	}
	!RCFilenameItems()
	{
		delete this->rh;
	}
private:
	// ROM�w�b�_��ǂݍ���
	void readRomHeader( System::String ^filepath )
	{
		if( !RomFilenameDecoderTWL::readRomHeader( filepath, this->rh ) )
		{
			this->error = RomFilenameDecoderTWL::ECError::ERROR_READ_FILE;
		}
	}
	// �t�@�C��������͂��ă����o�ϐ���ݒ肷��
	void parseFilename( System::String ^filepath )
	{
		if( !filepath )	// �w�肳��Ă��Ȃ��ꍇ
		{
			this->error = RomFilenameDecoderTWL::ECError::ERROR_FILE_NAME;
			return;
		}
		System::String^ prefix = System::IO::Path::GetFileNameWithoutExtension( filepath );	// �g���q�ƃf�B���N�g���p�X������
		if( prefix->Length < 7 )	// T****@#.SRL / T****0#E.SRL �̂����ꂩ
		{
			this->error = RomFilenameDecoderTWL::ECError::ERROR_FILE_NAME;
			return;
		}
		this->initialCode       = prefix->Substring(1,4);
		this->remasterVersion   = this->parseStrToByte( prefix->Substring(5,1) );
		if( prefix->Length >= 8 )
		{
			if( ((char)prefix[7] == 'E') && (this->remasterVersion == 0) )
			{
				this->remasterVersion = 0xE0;
			}
		}
		this->submissionVersion = this->parseStrToByte( prefix->Substring(6,1) );
	} //void parseFilename()
public:
	// 1���̐����𐔒l�ɕϊ�
	System::Byte parseStrToByte( System::String ^str )
	{
		if( str->Length > 1 )
		{
			return 0xff;
		}
		System::Byte val;

		// �ʏ� 0..F ���� ���̏�� G..Z �ŕ\���\��������
		char c = (char)str[0];
		if( ('G' <= c) && (c <= 'Z') )		// F�����G..Z�ŕ\������Ă���(�\������)
		{
			 val = c - 'G' + 16;
		}
		else if( ('g' <= c) && (c <= 'z') )
		{
			val = c - 'g' + 16;
		}
		else
		{
			try
			{
				val = System::Byte::Parse( str, System::Globalization::NumberStyles::AllowHexSpecifier );
			}
			catch(System::Exception ^e )
			{
				(void)e;
				val = 0xff;
			}
		}
		return val;
	} //parseStrToByte()
public:
	// ROM�w�b�_�̏��Ǝ��g�̏����r(�G���[���̓����o�ϐ��ɐݒ�)
	void compareRomHeader()
	{
		// ���łɃG���[������ꍇ�ɂ͉������Ȃ�
		if( this->error != RomFilenameDecoderTWL::ECError::NOERROR )
		{
			return;
		}
		// �C�j�V�����R�[�h�̔�r
		char inicode[4];
		RomFilenameDecoderTWL::setStringToChars( inicode, this->initialCode, 4, 0 );
		if( memcmp( this->rh->s.game_code, inicode, 4 ) != 0 )
		{
			this->error = RomFilenameDecoderTWL::ECError::ERROR_INITIAL_CODE;
			return;
		}
		// ���}�X�^�[�o�[�W�����̔�r
		if( this->rh->s.rom_version != this->remasterVersion )
		{
			this->error = RomFilenameDecoderTWL::ECError::ERROR_REMASTER_VERSION;
			return;
		}
		// ��o�o�[�W������ROM�w�b�_�ɂȂ��̂Ŕ�r�ł��Ȃ�
		this->error = RomFilenameDecoderTWL::ECError::NOERROR;
		return;
	} //void compareRomHeader()
}; //ref class RCFilenameItems

} //namespace RomFilenameDecoderTWL