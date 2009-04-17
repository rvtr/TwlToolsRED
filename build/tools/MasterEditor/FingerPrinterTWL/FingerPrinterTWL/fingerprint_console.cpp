#include "stdafx.h"
#include "fingerprint_util.h"
#include "MasterEditorTWL/split_tad.h"

using namespace System;

//
// usage
//
void usage()
{
	System::String ^msg =
		"Usage:\n" +
		"FingerprintTWL.exe input_rom output_rom fingerprint [options]\n\n" +
		"options:\n" +
		"-a : register fingerprint as ASCII charactors.(Default)\n" +
		"-b : regsiter fingerprint as hexadicimal binaries.\n" +
		"-s : output the ROM as SRL format.(Default)\n" +
		"-t : output the ROM as TAD format.\n" +
		"-f : Force to overwrite the fingerprint to the already fingerprinted ROM.\n"
		"-c : Fingerprint check mode. Typing \"output_rom\" and \"fingerprint\" is unnecessary.\n" +
		"-g : GUI mode. Other options are ignored.\n" +
		"-h : display this usage.\n";
	System::Console::WriteLine( "{0}", msg );
}

//
// コンソール処理
//
void fingerprintConsole( cli::array<System::String^> ^args )
{
	System::String ^optarg;
	char opt;
	bool isAscii = true;
	bool isTad   = false;
	bool isForce = false;
	bool isCheck = false;

	while( (opt = getopt(args, "abcstfgh", optarg)) >= 0 )
	{
		switch( opt )
		{
		case 'a':
			isAscii = true;
			break;

		case 'b':
			isAscii = false;
			break;

		case 't':
			isTad = true;
			break;

		case 's':
			isTad = false;
			break;

		case 'f':
			isForce = false;
			break;

		case 'g':
			return;
			break;

		case 'c':
			isCheck = true;
			break;

		case 'h':
			usage();
			return;
			break;

		default:
			throw gcnew System::Exception( "Arguments error." );
			break;
		}
	}

	if( !isCheck && (args->Length < 3) )
	{
		throw gcnew System::Exception( "Arguments error." );
	}
	if( isCheck && (args->Length < 1) )
	{
		throw gcnew System::Exception( "Arguments error." );
	}

	// 入出力ファイルのパス
	System::String ^srcpath = args[0];
	System::Console::WriteLine( "Input file:  {0}", srcpath );
	System::String ^dstpath;
	System::String ^fingerprint;
	if( !isCheck )
	{
		dstpath     = args[1];
		fingerprint = args[2];
	}
	System::Console::WriteLine( "Output file: {0}", dstpath );
	System::Console::WriteLine();

	// SRLの読み込み
	cli::array<System::Byte> ^srlbin;
	if( System::IO::Path::GetExtension(srcpath)->ToUpper()->Equals(".TAD") )
	{
		// TADの場合 split_tad
		System::Console::WriteLine( "*** Transform TAD => SRL ***" );
		System::String ^tmp = ".\\tmp" + System::DateTime::Now.ToString("yyyyMMddHHmmss") + ".srl";
		if( splitTad(srcpath, tmp) < 0 )
		{
			throw gcnew System::Exception("Failed to transform TAD => SRL.");
		}
		srlbin = ReadBin(tmp);
		System::IO::File::Delete(tmp);	// 中間ファイルを削除する
		System::Console::WriteLine();
	}
	else
	{
		srlbin = ReadBin(srcpath);
	}
	ROM_Header rh;
	ExtractRomHeader( srlbin, &rh );
	AuthenticateRomHeader( &rh );

	// fingerprint が埋め込まれていたら表示して上書き確認をする
	bool isFingerprint = false;
	int i;
	for( i=0; i < 32; i++ )
	{
		if( rh.s.reserved_C[i] != 0 )
		{
			isFingerprint = true;
			break;
		}
	}
	if( isFingerprint )
	{
		cli::array<System::Byte> ^bytes = gcnew cli::array<System::Byte>(32);
		int i;
		for( i=0; i < 32; i++ )
		{
			bytes[i] = rh.s.reserved_C[i];
		}

		System::Console::WriteLine( "*** Check the fingerprint ***" );
		System::Console::WriteLine();
		System::Console::WriteLine( "ASCII Charactors:" );
		System::Console::WriteLine( "{0}", TransBytesToString(bytes) );
		System::Console::WriteLine();
		System::Console::WriteLine( "Hexadecimal Numbers:" );
		System::Console::WriteLine( "{0}", TransBytesToHexString(bytes) );
		System::Console::WriteLine();
		System::Console::WriteLine( "The fingerprint has already been registered in the ROM." );
		System::Console::WriteLine();

		if( isCheck )	// チェックモードのときはここで終了
		{
			return;
		}

		if( !isForce )
		{
			System::Console::Write( "Overwrite it? (y/n) > " );
			System::ConsoleKeyInfo ^key = System::Console::ReadKey();
			System::Console::WriteLine();
			if( (key->KeyChar == 'y') || (key->KeyChar == 'Y') )
			{
				System::Console::WriteLine( "Overwrite the fingerprint." );
				System::Console::WriteLine();
			}
			else
			{
				throw gcnew System::Exception( "Canceled." );
			}
		}
	}
	else if( isCheck )
	{
		System::Console::WriteLine( "*** Check the fingerprint ***" );
		System::Console::WriteLine();
		System::Console::WriteLine( "The fingerprint has not been registered in the ROM yet." );
		return;
	}

	// fingerprint の埋め込み
	if( fingerprint )
	{
		System::Console::WriteLine( "*** Register the fingerprint ***" );
		System::Console::WriteLine();
		cli::array<System::Byte> ^bytes;
		if( isAscii )
		{
			System::Console::WriteLine( "ASCII Charactors:" );
			System::Console::WriteLine( "{0}", fingerprint );
			System::Console::WriteLine();
			bytes = TransStringToBytes( fingerprint, 32 );
		}
		else
		{
			bytes = TransHexStringToBytes( fingerprint, 32 );
		}
		pin_ptr<unsigned char> buf = &bytes[0];		// 解放の必要なし
		memcpy( rh.s.reserved_C, buf, 32 );
		SignRomHeader( &rh );	// 署名
		OverwriteRomHeader( srlbin, &rh );
		System::Console::WriteLine( "Hexadecimal Numbers:" );
		System::Console::WriteLine( "{0}", TransBytesToHexString(bytes) );
		System::Console::WriteLine();
	}

	// maketad
	if( isTad )
	{
		System::String ^bindir = System::IO::Path::GetDirectoryName( System::Reflection::Assembly::GetEntryAssembly()->Location );
		if( !bindir->EndsWith("\\") )
		{
			bindir = bindir + "\\";
		}
		System::String ^maketad_path = bindir + "maketad.exe";

		System::Console::WriteLine( "*** Transform SRL => TAD ***" );
		System::String ^tmp = ".\\tmp" + System::DateTime::Now.ToString("yyyyMMddHHmmss") + ".srl";
		WriteBin( tmp, srlbin );
		makeTad( maketad_path, tmp, dstpath );
		System::IO::File::Delete(tmp);
	}
	else
	{
		WriteBin( dstpath, srlbin );
	}
}