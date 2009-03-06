// MasterEditorTWLChecker.cpp : メイン プロジェクト ファイルです。

#include "stdafx.h"
#include "check.h"

using namespace System;

// ------------------------------------------------------------------
// context
// ------------------------------------------------------------------
ref class RCContext
{
public:
	property System::Boolean isVerbose;
public:
	RCContext()
	{
		this->isVerbose = false;
	}
};
System::Int32 parseOption( array<System::String ^> ^args, RCContext ^context );

// ------------------------------------------------------------------
// main
// ------------------------------------------------------------------

int main(array<System::String ^> ^args)
{
	RCContext ^context = gcnew RCContext();
	int argc = parseOption( args, context );

	try
	{
		if( context->isVerbose )
		{
			setDebugPrint( true );
		}
		if( argc <= 1 )
		{
			throw (gcnew System::Exception("Argc is less than 2."));
		}

		// ミドルウェアリストをはじく
		if( System::IO::Path::GetFileNameWithoutExtension(args[1])->ToUpper()->EndsWith("MIDDLEWARE") )
		{
			Console::WriteLine( "File: " + args[1] );
			Console::WriteLine( "========================================================" );
			Console::WriteLine( "Not Support to Middleware List." );
			return 0;	// 正常終了扱い
		}

		// 拡張子で判定
		if( System::IO::Path::GetExtension(args[1])->ToUpper() == ".XML" )
		{
			// args[0] はダミー
			System::String ^sheet = args[1];
			DebugPrint( "Sheet file : " + sheet );
			DebugPrint( "\n" );

			SheetItem ^sItem = gcnew SheetItem;
			sItem->readSheet( sheet );
			checkSheet( sItem );
		}
		else
		{
			System::String ^original = args[0];
			System::String ^target   = args[1];
			DebugPrint( "Original file : " + original );
			DebugPrint( "Target file   : " + target );
			DebugPrint( "\n" );

			checkRom( original, target );
		}
	}
	catch( System::Exception ^ex )
	{
		Console::WriteLine( "========================================================" );
		Console::WriteLine( "NG - " + ex->Message);
		return -1;
	}
	Console::WriteLine( "END" );
    return 0;
}

// ------------------------------------------------------------------
// getopt
// ------------------------------------------------------------------

// @ret オプションを除いたときのargc
System::Int32 parseOption( array<System::String ^> ^args, RCContext ^context )
{
	System::Collections::Generic::List<System::Int32> ^indexList
		= gcnew System::Collections::Generic::List<System::Int32>;

	int numopt = 0;
	int i;
	for( i=0; i < args->Length; i++ )
	{
		if( args[i]->StartsWith( "-v" ) )
		{
			context->isVerbose = true;
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
