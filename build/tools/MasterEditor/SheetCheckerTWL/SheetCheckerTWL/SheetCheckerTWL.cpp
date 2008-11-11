// SheetCheckerTWL.cpp : メイン プロジェクト ファイルです。

#include "stdafx.h"
#include "SheetCheckerTWL.h"
#include "crc_whole.h"
#include <twl/types.h>
#include <twl/os/common/format_rom.h>
#include <cstdio>

using namespace System;

// ------------------------------------------------------------------
// 宣言
// ------------------------------------------------------------------

System::Int32 parseOption( array<System::String ^> ^args, SheetCheckerContext ^context );
System::Boolean readRomHeader( System::String ^srlfile, ROM_Header *rh );
System::Boolean readSheet( System::String ^sheetfile, SheetItem ^item );
System::String^ getXPathText( System::Xml::XmlElement ^root, System::String ^xpath );


// ------------------------------------------------------------------
// 本体
// ------------------------------------------------------------------

int main(array<System::String ^> ^args)
{
	SheetCheckerContext ^hContext = gcnew SheetCheckerContext;

	// getopt
	int argc = parseOption( args, hContext );

	// 引数処理
	if( argc != 2 )
	{
		hContext->ErrorCode = SheetCheckerError::ERROR_ARG;
		Console::WriteLine( "error arguments" );
		return 0;
	}
	System::String ^hSrlFile   = args[0];
	System::String ^hSheetFile = args[1];

	// ROMヘッダの読み込み
	ROM_Header rh;
	memset( (void*)&rh, 0, sizeof(ROM_Header) );
	if( !readRomHeader( hSrlFile, &rh ) )
	{
		hContext->ErrorCode = SheetCheckerError::ERROR_READ_SRL;
	}
	System::UInt16 crc;
	getWholeCRCInFile( hSrlFile, &crc );

	// 提出確認書の読み込み
	SheetItem ^hItem = gcnew SheetItem;
	if( !readSheet( hSheetFile, hItem ) )
	{
		hContext->ErrorCode = SheetCheckerError::ERROR_READ_SHEET;
	}

	// 一致判定
	if( hContext->ErrorCode == SheetCheckerError::NOERROR )
	{
		if( memcmp( rh.s.game_code, hItem->GameCode, 4 ) != 0 )
		{
			hContext->ErrorCode = SheetCheckerError::ERROR_VERIFY_GAME_CODE;
		}
		else if( rh.s.rom_version != hItem->RomVersion )
		{
			hContext->ErrorCode = SheetCheckerError::ERROR_VERIFY_ROM_VERSION;
		}
		else if( crc != hItem->FileCRC )
		{
			hContext->ErrorCode = SheetCheckerError::ERROR_VERIFY_CRC;
		}
	}

	// 通常の表示
	if( !hContext->bSubmitVersion && !hContext->bResult )
	{

		Console::WriteLine( "" );
		Console::WriteLine( "SRL:   " + hSrlFile );
		Console::WriteLine( "Sheet: " + hSheetFile );
		Console::WriteLine( "" );

		printf( "                 SRL       Sheet\n" ); 
		printf( "------------------------------------\n" ); 
		printf( "InitialCode:     %c%c%c%c      %c%c%c%c\n", 
			rh.s.game_code[0],  rh.s.game_code[1],  rh.s.game_code[2],  rh.s.game_code[3], 
			hItem->GameCode[0], hItem->GameCode[1], hItem->GameCode[2], hItem->GameCode[3] );
		printf( "RemasterVersion: %02X        %02X\n", rh.s.rom_version, hItem->RomVersion );
		printf( "File CRC:        %04X      %04X\n", crc, hItem->FileCRC );
		printf( "------------------------------------\n" ); 
		printf( "SubmitVersion:   -         %d (%02X)\n", hItem->SubmitVersion, hItem->SubmitVersion );
		printf( "------------------------------------\n" ); 
		printf( "Result:          " );
		if( hContext->ErrorCode == SheetCheckerError::NOERROR )
		{
			printf( "OK\n" );
		}
		else
		{
			printf( "NG (%d)\n", hContext->ErrorCode );
		}
	}

	// オプションのときの表示
	if( hContext->bSubmitVersion )
	{
		if( hContext->ErrorCode == SheetCheckerError::NOERROR )
		{
			printf( "%d\n", hItem->SubmitVersion );
		}
		else
		{
			printf( "%d\n", hContext->ErrorCode );	// エラーのときはエラーコード
		}
	}
	if( hContext->bResult )
	{
		printf( "%d\n", hContext->ErrorCode );
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

// 返り値: オプションを除いたときのargc
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

