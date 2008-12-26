// SheetCheckerTWL.cpp : メイン プロジェクト ファイルです。

#include "stdafx.h"
#include "SheetCheckerTWL.h"
#include "crc_whole.h"
#include <twl/types.h>
#include <twl/os/common/format_rom.h>
#include <cstdio>
#include "Form1.h"

using namespace SheetCheckerTWL;

// ------------------------------------------------------------------
// 宣言
// ------------------------------------------------------------------

System::Int32 parseOption( array<System::String ^> ^args, SheetCheckerContext ^context );
int consoleRun( array<System::String ^> ^args, int argc, SheetCheckerContext ^context );
int consoleOptionRun( array<System::String ^> ^args, int argc, SheetCheckerContext ^context );
int printResult( SheetCheckerContext ^context, ROM_Header *rh, SheetItem ^item, 
				System::String ^srlfile, System::String ^sheetfile, System::UInt16 srlcrc, SheetCheckerError error );

// ------------------------------------------------------------------
// main
// ------------------------------------------------------------------

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	SheetCheckerContext ^context = gcnew SheetCheckerContext;

	// getopt
	int argc = parseOption( args, context );

	if( argc > 0 )
	{
		int ret;
		if( context->bResult || context->bSubmitVersion || context->bTadVersion || 
			context->bSubmitVersion || context->bUnnecessaryRating )
		{
			ret = consoleOptionRun( args, argc, context );
		}
		else
		{
			ret = consoleRun( args, argc, context );
		}
		return ret;
	}

	// コントロールが作成される前に、Windows XP ビジュアル効果を有効にします
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// メイン ウィンドウを作成して、実行します
	Application::Run(gcnew Form1());
	return 0;
}

// ------------------------------------------------------------------
// コンソール処理 (ノーマル)
// ------------------------------------------------------------------

int consoleRun( array<System::String ^> ^args, int argc, SheetCheckerContext ^context )
{
	SheetCheckerError error = SheetCheckerError::NOERROR;
	ROM_Header rh;
	memset( (void*)&rh, 0, sizeof(ROM_Header) );
	SheetItem ^item = gcnew SheetItem;

	// 引数処理
	if( argc != 2 )
	{
		error = SheetCheckerError::ERROR_ARG;
		printResult( context, &rh, item, nullptr, nullptr, 0, error );
		return ((int)error);
	}
	System::String ^romfile   = args[0];
	System::String ^sheetfile = args[1];

	// ROMヘッダの読み込み
	if( !readRomHeader( romfile, &rh ) )
	{
		error = SheetCheckerError::ERROR_READ_SRL;
		printResult( context, &rh, item, romfile, sheetfile, 0, error );
		return ((int)error);
	}
	System::UInt16 crc;
	getWholeCRCInFile( romfile, &crc );

	// 提出確認書の読み込み
	if( !readSheet( sheetfile, item ) )
	{
		error = SheetCheckerError::ERROR_READ_SHEET;
		printResult( context, &rh, item, romfile, sheetfile, crc, error );
		return ((int)error);
	}

	// 一致判定
	error = checkSheet( &rh, crc, item );
	printResult( context, &rh, item, romfile, sheetfile, crc, error );	// 結果を表形式で表示

	return ((int)error);
}

// ------------------------------------------------------------------
// コンソール処理 (オプション)
// ------------------------------------------------------------------

int consoleOptionRun( array<System::String ^> ^args, const int argc, SheetCheckerContext ^context )
{
	SheetCheckerError  error = SheetCheckerError::NOERROR;
	System::String ^romfile;
	System::String ^sheetfile;

	// 引数解析
	switch( argc )
	{
		case 1:
			sheetfile = args[0];
			if( context->bResult )		// このオプションのときだけは引数1つを許さない
			{
				error = SheetCheckerError::ERROR_ARG;
				printf( "%d", (int)error );
				return ((int)error);
			}
		break;

		case 2:
			romfile   = args[0];
			sheetfile = args[1];
		break;

		default:
			error = SheetCheckerError::ERROR_ARG;
			printf( "%d", (int)error );
			return ((int)error);
		break;
	}

	// 提出確認書の読み込み
	SheetItem ^item = gcnew SheetItem;
	if( !readSheet( sheetfile, item ) )
	{
		error = SheetCheckerError::ERROR_READ_SHEET;
		printf( "%d", (int)error );
		return ((int)error);
	}

	// 引数が1つのときはSRLとの比較をしない
	if( argc == 2 )
	{
		ROM_Header rh;
		memset( (void*)&rh, 0, sizeof(ROM_Header) );

		// ROMヘッダの読み込み
		if( !readRomHeader( romfile, &rh ) )
		{
			error = SheetCheckerError::ERROR_READ_SHEET;
			printf( "%d", (int)error );
			return ((int)error);
		}
		System::UInt16 crc;
		getWholeCRCInFile( romfile, &crc );

		// 比較
		error = checkSheet( &rh, crc, item );
	}//if( argc == 2 )
	
	// 結果出力はオプションによって異なる(エラーのときには共通)
	if( error != SheetCheckerError::NOERROR )
	{
		if( context->bResult )
		{
			printf( "0" );		// このパスは成功のときのみ
		}
		else if( context->bTadVersion )
		{
			System::UInt16 tadver = item->RomVersion;
			tadver = (tadver << 8) | item->SubmitVersion;
			printf( "%d", tadver );
		}
		else if( context->bSubmitVersion )
		{
			printf( "%d", item->SubmitVersion );
		}
		else if( context->bUnnecessaryRating )
		{
			printf( "%d", (item->IsUnnecessaryRating)?1:0 );
		}
	}
	else
	{
		printf( "%d", (int)error );
	}
	return ((int)error);
}

// ------------------------------------------------------------------
// 一致判定
// ------------------------------------------------------------------

SheetCheckerError checkSheet( ROM_Header *rh, System::UInt16 crc, SheetItem ^item )
{
	SheetCheckerError error;
	if( memcmp( rh->s.game_code, item->GameCode, 4 ) != 0 )
	{
		error = SheetCheckerError::ERROR_VERIFY_GAME_CODE;
	}
	else if( rh->s.rom_version != item->RomVersion )
	{
		error = SheetCheckerError::ERROR_VERIFY_ROM_VERSION;
	}
	else if( crc != item->FileCRC )
	{
		error = SheetCheckerError::ERROR_VERIFY_CRC;
	}
	else
	{
		error = SheetCheckerError::NOERROR;
	}
	return error;
}

// ------------------------------------------------------------------
// 結果表示
// ------------------------------------------------------------------

int printResult( SheetCheckerContext ^context, ROM_Header *rh, SheetItem ^item, 
				System::String ^srlfile, System::String ^sheetfile, System::UInt16 srlcrc, SheetCheckerError error )
{
	System::UInt16 tadver = item->RomVersion;
	tadver = (tadver << 8) | item->SubmitVersion;

	// 通常の表示
	Console::WriteLine( "" );
	Console::WriteLine( "SRL:   " + srlfile );
	Console::WriteLine( "Sheet: " + sheetfile );
	Console::WriteLine( "" );

	printf( "                 SRL       Sheet\n" ); 
	printf( "---------------------------------------\n" ); 
	printf( "InitialCode:     %c%c%c%c      %c%c%c%c\n", 
		rh->s.game_code[0],  rh->s.game_code[1],  rh->s.game_code[2],  rh->s.game_code[3], 
		item->GameCode[0], item->GameCode[1], item->GameCode[2], item->GameCode[3] );
	printf( "RemasterVersion: %02X        %02X\n", rh->s.rom_version, item->RomVersion );
	printf( "File CRC:        %04X      %04X\n", srlcrc, item->FileCRC );
	printf( "---------------------------------------\n" ); 
	printf( "Rating Display:            %s\n", (item->IsUnnecessaryRating)?"Unnecessary":"Necessary" );
	printf( "---------------------------------------\n" ); 
	printf( "SubmitVersion:   -         %d (%02X)\n", item->SubmitVersion, item->SubmitVersion );
	printf( "TAD Version:               %d (%04X)\n", tadver, tadver );
	printf( "---------------------------------------\n" ); 
	printf( "Result:          " );
	if( error == SheetCheckerError::NOERROR )
	{
		printf( "OK\n" );
	}
	else
	{
		printf( "NG (%d)\n", (int)error );
	}
	return 0;
}


