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
int printResult( SheetCheckerContext ^context, ROM_Header *rh, SheetItem ^item, 
				 System::String ^srlfile, System::String ^sheetfile, System::UInt16 srlcrc );
int consoleRun( array<System::String ^> ^args, int argc, SheetCheckerContext ^hContext );

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	SheetCheckerContext ^hContext = gcnew SheetCheckerContext;

	// getopt
	int argc = parseOption( args, hContext );

	if( argc > 0 )
	{
		int ret = consoleRun( args, argc, hContext );
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
// コンソール処理
// ------------------------------------------------------------------

int consoleRun( array<System::String ^> ^args, int argc, SheetCheckerContext ^hContext )
{
	ROM_Header rh;
	memset( (void*)&rh, 0, sizeof(ROM_Header) );
	SheetItem ^hItem = gcnew SheetItem;

	// 引数処理
	if( argc != 2 )
	{
		hContext->ErrorCode = SheetCheckerError::ERROR_ARG;
		printResult( hContext, &rh, hItem, nullptr, nullptr, 0 );
		return -1;
	}
	System::String ^hSrlFile   = args[0];
	System::String ^hSheetFile = args[1];

	// ROMヘッダの読み込み
	if( !readRomHeader( hSrlFile, &rh ) )
	{
		hContext->ErrorCode = SheetCheckerError::ERROR_READ_SRL;
		printResult( hContext, &rh, hItem, hSrlFile, hSheetFile, 0 );
		return -1;
	}
	System::UInt16 crc;
	getWholeCRCInFile( hSrlFile, &crc );

	// 提出確認書の読み込み
	if( !readSheet( hSheetFile, hItem ) )
	{
		hContext->ErrorCode = SheetCheckerError::ERROR_READ_SHEET;
		printResult( hContext, &rh, hItem, hSrlFile, hSheetFile, crc );
		return -1;
	}

	// 一致判定
	hContext->ErrorCode = checkSheet( &rh, crc, hItem );

	// 結果表示
	int ret = printResult( hContext, &rh, hItem, hSrlFile, hSheetFile, crc );
	return ret;
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
				 System::String ^srlfile, System::String ^sheetfile, System::UInt16 srlcrc )
{
	System::UInt16 tadver = item->RomVersion;
	tadver = (tadver << 8) | item->SubmitVersion;

	// 通常の表示
	if( !context->bSubmitVersion && !context->bResult && !context->bTadVersion )
	{
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
		printf( "SubmitVersion:   -         %d (%02X)\n", item->SubmitVersion, item->SubmitVersion );
		printf( "TAD Version:               %d (%04X)\n", tadver, tadver );
		printf( "---------------------------------------\n" ); 
		printf( "Result:          " );
		if( context->ErrorCode == SheetCheckerError::NOERROR )
		{
			printf( "OK\n" );
		}
		else
		{
			printf( "NG (%d)\n", context->ErrorCode );
		}
	}

	// オプションのときの表示
	if( context->bSubmitVersion )
	{
		if( context->ErrorCode == SheetCheckerError::NOERROR )
		{
			printf( "%d", item->SubmitVersion );
		}
		else
		{
			printf( "%d", context->ErrorCode );	// エラーのときはエラーコード
		}
	}
	if( context->bTadVersion )
	{
		if( context->ErrorCode == SheetCheckerError::NOERROR )
		{
			printf( "%d", tadver );
		}
		else
		{
			printf( "%d", context->ErrorCode );
		}
	}
	if( context->bResult )
	{
		printf( "%d", context->ErrorCode );
	}

	if( context->ErrorCode != SheetCheckerError::NOERROR )
	{
		return -1;
	}
	return 0;
}


// ------------------------------------------------------------------
// ROMヘッダの読み込み
// ------------------------------------------------------------------

System::Boolean readRomHeader( System::String ^srlfile, ROM_Header *rh )
{
	FILE       *fp = NULL;
	const char *pchFilename = 
		(const char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( srlfile ).ToPointer();

	// ファイルを開いてROMヘッダのみ読み出す
	if( fopen_s( &fp, pchFilename, "rb" ) != NULL )
	{
		return false;
	}
	(void)fseek( fp, 0, SEEK_SET );		// ROMヘッダはsrlの先頭から

	// 1バイトをsizeof(~)だけリード (逆だと返り値がsizeof(~)にならないので注意)
	if( fread( (void*)rh, 1, sizeof(ROM_Header), fp ) != sizeof(ROM_Header) )
	{
		fclose( fp );
		return false;
	}
	fclose( fp );
	return true;
}

// ------------------------------------------------------------------
// 提出確認書の読み込み
// ------------------------------------------------------------------

System::Boolean readSheet( System::String ^sheetfile, SheetItem ^item )
{
	// XSLによってXML変換
	System::String ^tmpfile = ".\\temp" + System::DateTime::Now.ToString("yyyyMMddHHmmss") + ".xml";
	System::Xml::Xsl::XslCompiledTransform ^xslt = gcnew System::Xml::Xsl::XslCompiledTransform;
	System::String ^xslpath = System::IO::Path::GetDirectoryName( System::Reflection::Assembly::GetEntryAssembly()->Location )
		                      + "\\extract_sheet.xsl";
	try
	{
		//Console::WriteLine( "xslpath: " + xslpath );
		xslt->Load( xslpath );
		xslt->Transform( sheetfile, tmpfile );
	}
	catch( System::Exception ^ex )
	{
		(void)ex;
		//Console::WriteLine( "XSLT Error" );
		return false;
	}
	// 変換したXMLを読み込み
	System::Xml::XmlDocument ^doc = gcnew System::Xml::XmlDocument;
	try
	{
		doc->Load( tmpfile );
	}
	catch( System::Exception ^ex )
	{
		(void)ex;
		//Console::WriteLine( "Load error" );
		return false;
	}

	// XMLからデータを抽出
	System::Xml::XmlElement  ^root = doc->DocumentElement;
	System::String ^text;
	try
	{
		item->Media = getXPathText( root, "/Sheet/Media" );

		text = getXPathText( root, "/Sheet/GameCode" );
		char code[4];
		int  i;
		for(i=0; i<4; i++ )
		{
			code[i] = (char)text[i];
		}
		item->GameCode = code;	// 代入したらコピーするように property を定義している

		text = getXPathText( root, "/Sheet/RomVersion" );
		if( text->Contains( "(" ) )	// 事前版のときには"(事前版)"が入る
		{
			text = text->Remove( text->IndexOf("(") );
		}
		text = text->Trim();
		item->RomVersion = System::Byte::Parse( text, System::Globalization::NumberStyles::AllowHexSpecifier );

		text = getXPathText( root, "/Sheet/CRC" );
		if( text->Contains( "0x" ) )
		{
			text = text->Substring( text->IndexOf("x")+1 );
		}
		item->FileCRC = System::UInt16::Parse( text, System::Globalization::NumberStyles::AllowHexSpecifier );

		text = getXPathText( root, "/Sheet/SubmitVersion" );
		char c = (char)text[0];
		if( ('G' <= c) && (c <= 'Z') )		// Fより上はG..Zで表現されていく(可能性あり)
		{
			item->SubmitVersion = c - 'G' + 16;
		}
		else if( ('g' <= c) && (c <= 'z') )
		{
			item->SubmitVersion = c - 'g' + 16;
		}
		else
		{
			item->SubmitVersion = System::Byte::Parse( text, System::Globalization::NumberStyles::AllowHexSpecifier );
		}
	}
	catch( System::Exception ^ex )
	{
		(void)ex;
		return false;
	}

	// 中間ファイルを削除
	if( System::IO::File::Exists( tmpfile ) )
	{
		System::IO::File::Delete( tmpfile );
	}
	return true;
}

// ------------------------------------------------------------------
// getopt
// ------------------------------------------------------------------

// @ret オプションを除いたときのargc
System::Int32 parseOption( array<System::String ^> ^args, SheetCheckerContext ^context )
{
	System::Collections::Generic::List<System::Int32> ^indexList
		= gcnew System::Collections::Generic::List<System::Int32>;

	int numopt = 0;
	int i;
	for( i=0; i < args->Length; i++ )
	{
		if( args[i]->StartsWith( "-s" ) )
		{
			context->bSubmitVersion = true;
			numopt++;
		}
		else if( args[i]->StartsWith( "-r" ) )
		{
			context->bResult = true;
			numopt++;
		}
		else if( args[i]->StartsWith( "-t" ) )
		{
			context->bTadVersion = true;
			numopt++;
		}
		else if( !args[i]->StartsWith( "-" ) )	// オプションでない引数のindexを記録
		{
			indexList->Add(i);
		}
	}
	i=0;
	for each( System::Int32 index in indexList )	// オプションでない引数を前につめていく
	{
		args[i] = args[index];
		i++;
	}
	return (args->Length - numopt);
}

// ------------------------------------------------------------------
// XMLタグ検索
// ------------------------------------------------------------------

// @ret テキストが存在するときそのテキストを返す。存在しないときnullptr。
System::String^ getXPathText( System::Xml::XmlElement ^root, System::String ^xpath )
{
	System::Xml::XmlNode ^tmp = root->SelectSingleNode( xpath );
	if( tmp && tmp->FirstChild && tmp->FirstChild->Value )
	{
		return tmp->FirstChild->Value;
	}
	return nullptr;
}
