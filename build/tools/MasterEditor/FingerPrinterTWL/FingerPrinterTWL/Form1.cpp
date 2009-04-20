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
// 開始 & 終了処理
// ------------------------------------------------------------------

void Form1::construct()
{
	this->rh = NULL;
	this->rh = new ROM_Header;
	memset( this->rh, 0, sizeof(ROM_Header) );
	this->prevDir = nullptr;

	System::String ^example = "Input Example\r\n\r\n"
							+ "(1)String of ASCII Charactors\r\n"
		                    + "  *AB  CD\r\n"
							+ "  => register \"4142204344\" (\'A\' \'B\' \'  \' \'C\' \'D\')\r\n\r\n"
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
// フィンガープリント
// ------------------------------------------------------------------
void Form1::fingerprintRomHeader()
{
	System::String ^fingerprint = this->tboxFP->Text;
	cli::array<System::Byte> ^bytes;

	// ROMヘッダにフィンガープリントを入れる
	if( this->rHex->Checked )
	{
		bytes = TransHexStringToBytes( fingerprint, 32 );
	}
	else
	{
		bytes = TransStringToBytes( fingerprint, 32 );
	}
	pin_ptr<unsigned char> buf = &bytes[0];		// 解放の必要なし
	memcpy( this->rh->s.reserved_C, buf, 32 );
	SignRomHeader( this->rh );	// 署名
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
// ファイル処理を共通化させる
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
			throw gcnew Exception("Failed to transform TAD => SRL.");
		}
		this->srlbin = ReadBin(tmp);
		System::IO::File::Delete(tmp);	// 中間ファイルを削除する
	}
	else
	{
		this->srlbin = ReadBin(srcpath);
	}
	memset( this->rh, 0, sizeof(ROM_Header) );
	ExtractRomHeader( this->srlbin, this->rh );
	AuthenticateRomHeader( this->rh );

	// すでにフィンガープリントされている場合には注意書きを表示
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

	this->fingerprintRomHeader();
	OverwriteRomHeader( this->srlbin, this->rh );

	if( this->rTad->Checked )
	{
		System::Console::WriteLine( "*** Transform SRL => TAD ***" );
		System::String ^tmp = ".\\tmp" + System::DateTime::Now.ToString("yyyyMMddHHmmss") + ".srl";
		WriteBin( tmp, this->srlbin );
		makeTad( maketad_path, tmp, dstpath );
		System::IO::File::Delete(tmp);
	}
	else
	{
		WriteBin( dstpath, this->srlbin );
	}
}

// ------------------------------------------------------------------
// ボタンが押されたときの処理 (catch文必須)
// ------------------------------------------------------------------

void Form1::procOpenRomButton()
{
	try
	{
		System::String ^rompath = OpenFileUsingDialog( this->prevDir, "rom format (*.srl;*.tad)|*.srl;*.tad|All files (*.*)|*.*" );
		if( !rompath )
		{
			return;
		}
		this->commonOpenRom( rompath );
		this->tboxFile->Text = rompath;
		this->tboxFile->SelectionStart = rompath->Length;	// 入りきらないときに右端を表示する
		this->rSrl->Checked = true;
		this->rTad->Checked = false;
		this->prevDir = System::IO::Path::GetDirectoryName( rompath );	// あらかじめパスが指定されたときには更新しない
	}
	catch( System::Exception ^ex )
	{
		this->tboxFile->Text = "";		// クリア
		this->errMsg( ex->Message );
	}
}

void Form1::procSaveRomButton()
{
	try
	{
		if( System::String::IsNullOrEmpty(this->tboxFile->Text) )
		{
			throw gcnew Exception("The input ROM file has not read yet.");
		}
		if( this->rTad->Checked && !(this->rh->s.titleID_Hi & TITLE_ID_HI_MEDIA_MASK) )
		{
			throw gcnew Exception("Cannot make TAD file from the software intended for Game Card.");	
		}
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
		// デフォルトのファイル名を決める
		System::String ^defname = System::IO::Path::GetFileNameWithoutExtension( this->tboxFile->Text )
								  + ".fp" + ext;

		System::String ^rompath = SaveFileUsingDialog( this->prevDir, defname, format, ext );
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

void Form1::procDragDrop( System::String ^rompath )
{
	try
	{
		this->commonOpenRom( rompath );
		this->tboxFile->Text = rompath;
		this->tboxFile->SelectionStart = rompath->Length;
		this->rSrl->Checked = true;
		this->rTad->Checked = false;
		this->prevDir = System::IO::Path::GetDirectoryName( rompath );
	}
	catch( System::Exception ^ex )
	{
		this->tboxFile->Text = "";		// クリア
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
// エラーメッセージ
// ------------------------------------------------------------------

void Form1::sucMsg( System::String ^fmt, ... cli::array<System::String^> ^args )
{
	System::String ^msg = System::String::Format( fmt, args );	// 書式をStringに展開
	MessageBox::Show( msg, "SUCCESS", MessageBoxButtons::OK, MessageBoxIcon::None );
}

void Form1::errMsg( System::String ^fmt, ... cli::array<System::String^> ^args )
{
	System::String ^msg = System::String::Format( fmt, args );	// 書式をStringに展開
	MessageBox::Show( msg, "Error", MessageBoxButtons::OK, MessageBoxIcon::Error );
}
