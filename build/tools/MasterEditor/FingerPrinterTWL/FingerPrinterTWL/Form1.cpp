#include "stdafx.h"

#include "Form1.h"
#include <twl/types.h>
#include <twl/os/common/format_rom.h>
#include <cstring>
#include <cstdio>
#include "fingerprint_util.h"

using namespace FingerPrinterTWL;

// ------------------------------------------------------------------
// �J�n & �I������
// ------------------------------------------------------------------

void Form1::construct()
{
	this->rh = NULL;
	this->rh = new ROM_Header;
	memset( this->rh, 0, sizeof(ROM_Header) );
	this->prevDir = nullptr;

	System::String ^example = "Input Example\r\n\r\n"
							+ "(1)String of ASCII Charactors\r\n"
		                    + "  *THIS IS FINGERPRINT\r\n\r\n"
							+ "(2)Hexadecimal Numbers\r\n"
							+ "  *5A6B78014A235A\r\n"
							+ "  *5a6b78014a235a\r\n"
							+ "  *5A 6B 78 01 4A 23 5A\r\n"
							+ "  (2 charactor per 1 number)";
	this->tboxExample->Text = example;
}

void Form1::destruct()
{
	if( this->rh )
	{
		delete this->rh;
	}
	this->rh = NULL;
}

// ------------------------------------------------------------------
// �t�B���K�[�v�����g
// ------------------------------------------------------------------
void Form1::fingerprintRomHeader()
{
	System::String ^fingerprint = this->tboxFP->Text;
	cli::array<System::Byte> ^bytes;

	// ROM�w�b�_�Ƀt�B���K�[�v�����g������
	if( this->rHex->Checked )
	{
		bytes = TransHexStringToBytes( fingerprint, 32 );
	}
	else
	{
		bytes = TransStringToBytes( fingerprint, 32 );
	}
	pin_ptr<unsigned char> buf = &bytes[0];		// ����̕K�v�Ȃ�
	memcpy( this->rh->s.reserved_C, buf, 32 );
}


// ------------------------------------------------------------------
// �t�@�C�����������ʉ�������
// ------------------------------------------------------------------

void Form1::commonOpenRom( System::String ^srcpath )
{
	memset( this->rh, 0, sizeof(ROM_Header) );
	ReadRomHeader( srcpath, this->rh );	
	this->tboxFile->Text = srcpath;
	this->tboxFile->SelectionStart = srcpath->Length;	// ���肫��Ȃ��Ƃ��ɉE�[��\������
}

void Form1::commonSaveRom( System::String ^dstpath )
{
	System::String ^srcpath = this->tboxFile->Text;
	if( !srcpath )
	{
		throw gcnew Exception("The input ROM file has not read yet.");
	}

	// �t�@�C�����R�s�[����ROM�w�b�_���㏑��
	if( !srcpath->Equals( dstpath ) )
	{
		CopyFile( srcpath, dstpath );
	}
	this->fingerprintRomHeader();
	WriteRomHeader( dstpath, this->rh );
}


// ------------------------------------------------------------------
// �{�^���������ꂽ�Ƃ��̏��� (catch���K�{)
// ------------------------------------------------------------------

void Form1::procOpenRomButton()
{
	try
	{
//		System::String ^rompath = OpenFileUsingDialog( this->prevDir, "rom format (*.srl;*.tad)|*.srl;*.tad|All files (*.*)|*.*" );
		System::String ^rompath = OpenFileUsingDialog( this->prevDir, "rom format (*.srl)|*.srl|All files (*.*)|*.*" );
		if( !rompath )
		{
			return;
		}
		this->commonOpenRom( rompath );
		this->prevDir = System::IO::Path::GetDirectoryName( rompath );
	}
	catch( System::Exception ^ex )
	{
		this->errMsg( ex->Message );
	}
}

void Form1::procSaveRomButton()
{
	try
	{
		System::String ^rompath = SaveFileUsingDialog( this->prevDir, "rom format (*.srl)|*.srl|All files (*.*)|*.*", ".srl" );
		if( !rompath )
		{
			return;
		}
		this->commonSaveRom( rompath );
		this->prevDir = System::IO::Path::GetDirectoryName( rompath );
		this->sucMsg( "Succeed to fingerprint.\n\n{0}", rompath );
	}
	catch( System::Exception ^ex )
	{
		this->errMsg( ex->Message );
	}
}


// ------------------------------------------------------------------
// �G���[���b�Z�[�W
// ------------------------------------------------------------------

void Form1::sucMsg( System::String ^fmt, ... cli::array<System::String^> ^args )
{
	System::String ^msg = System::String::Format( fmt, args );	// ������String�ɓW�J
	MessageBox::Show( msg, "SUCCESS", MessageBoxButtons::OK, MessageBoxIcon::Information );
}

void Form1::errMsg( System::String ^fmt, ... cli::array<System::String^> ^args )
{
	System::String ^msg = System::String::Format( fmt, args );	// ������String�ɓW�J
	MessageBox::Show( msg, "Error", MessageBoxButtons::OK, MessageBoxIcon::Error );
}
