#include "stdafx.h"

#include "Form1.h"
#include "FormCheck.h"
#include "FormAbout.h"
#include <twl/types.h>
#include <twl/os/common/format_rom.h>
#include <cstring>
#include <cstdio>
#include "MasterEditorTWL/split_tad.h"
#include "MasterEditorTWL/split_tad_util.h"
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
		                    + "  *ABCD\r\n"
							+ "  => register \"41424344\" (\'T\' \'E\' \'S\' \'T\')\r\n\r\n"
							+ "(2)Hexadecimal Numbers\r\n"
							+ "  *5A6B78014A235A\r\n"
							+ "  *5a6b78014a235a\r\n"
							+ "  *5A 6B 78 01 4A 23 5A\r\n"
							+ "  => register \"5A6B78014A235A\"";
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
	SignRomHeader( this->rh );	// ����
}

System::Boolean Form1::isFingerprint()
{
	int i;
	for( i=0; i < 32; i++ )
	{
		if( this->rh->s.reserved_C[i] != 0 )
		{
			return true;
		}
	}
	return false;
}

// ------------------------------------------------------------------
// �t�@�C�����������ʉ�������
// ------------------------------------------------------------------

void Form1::commonOpenRom( System::String ^srcpath )
{
	System::Boolean bTad = System::IO::Path::GetExtension(srcpath)->ToUpper()->Equals(".TAD");

	if( bTad )
	{
		System::Console::WriteLine( "*** Transform TAD => SRL ***" );
		System::String ^tmp = ".\\tmp" + System::DateTime::Now.ToString("yyyyMMddHHmmss") + ".srl";
		if( splitTad(srcpath, tmp) < 0 )
		{
			throw gcnew Exception("Fail to transform TAD => SRL.");
		}
		this->srlbin = ReadBin(tmp);
		System::IO::File::Delete(tmp);	// ���ԃt�@�C�����폜����
	}
	else
	{
		this->srlbin = ReadBin(srcpath);
	}
	memset( this->rh, 0, sizeof(ROM_Header) );
	ExtractRomHeader( this->srlbin, this->rh );

	// ���łɃt�B���K�[�v�����g����Ă���ꍇ�ɂ͒��ӏ�����\��
	if( this->isFingerprint() )
	{
		FormCheck ^form = gcnew FormCheck;
		form->displayFingerprint( this->rh->s.reserved_C );
		form->Show();
	}
}

void Form1::commonSaveRom( System::String ^dstpath )
{
	System::String ^bindir = System::IO::Path::GetDirectoryName( System::Reflection::Assembly::GetEntryAssembly()->Location );
	if( !bindir->EndsWith("\\") )
	{
		bindir = bindir + "\\";
	}
	System::String ^maketad_path = bindir + "maketad.exe";

	System::String ^srcpath = this->tboxFile->Text;
	if( !srcpath )
	{
		throw gcnew Exception("The input ROM file has not read yet.");
	}

	if( this->rTad->Checked && !(this->rh->s.titleID_Hi && TITLE_ID_MEDIA_MASK) )
	{
		throw gcnew Exception("Cannot make TAD file for software intended for Game Card.");	
	}

	this->fingerprintRomHeader();
	OverwriteRomHeader( this->srlbin, this->rh );

	if( this->rTad->Checked )
	{
		System::Console::WriteLine( "*** Transform SRL => TAD ***" );
		System::String ^tmp = ".\\tmp" + System::DateTime::Now.ToString("yyyyMMddHHmmss") + ".srl";
		WriteBin( tmp, this->srlbin );
		makeTad( maketad_path, tmp, dstpath );
	}
	else
	{
		WriteBin( dstpath, this->srlbin );
	}
}

// ------------------------------------------------------------------
// �{�^���������ꂽ�Ƃ��̏��� (catch���K�{)
// ------------------------------------------------------------------

void Form1::procOpenRomButton( System::String ^path )
{
	try
	{
		System::String ^rompath = nullptr;
		if( !path )
		{
			rompath = OpenFileUsingDialog( this->prevDir, "rom format (*.srl;*.tad)|*.srl;*.tad|All files (*.*)|*.*" );
		}
		else
		{
			rompath = System::String::Copy(path);
		}
		if( !rompath )
		{
			return;
		}
		this->commonOpenRom( rompath );
		this->tboxFile->Text = rompath;
		this->tboxFile->SelectionStart = rompath->Length;	// ���肫��Ȃ��Ƃ��ɉE�[��\������
		this->rSrl->Checked = true;
		this->rTad->Checked = false;
		if( !path )
		{
			this->prevDir = System::IO::Path::GetDirectoryName( rompath );	// ���炩���߃p�X���w�肳�ꂽ�Ƃ��ɂ͍X�V���Ȃ�
		}
	}
	catch( System::Exception ^ex )
	{
		this->tboxFile->Text = "";		// �N���A
		this->errMsg( ex->Message );
	}
}

void Form1::procSaveRomButton()
{
	try
	{
		System::String ^format = nullptr;
		System::String ^ext = nullptr;
		if( this->rTad->Checked )
		{
			format = "rom format (*.tad)|*.tad|All files (*.*)|*.*";
			ext    = ".tad";
		}
		else
		{
			format = "rom format (*.srl)|*.srl|All files (*.*)|*.*";
			ext    = ".srl";
		}

		System::String ^rompath = SaveFileUsingDialog( this->prevDir, format, ext );
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

void Form1::procAboutButton()
{
	try
	{
		FormAbout ^form = gcnew FormAbout;
		form->Show();
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
