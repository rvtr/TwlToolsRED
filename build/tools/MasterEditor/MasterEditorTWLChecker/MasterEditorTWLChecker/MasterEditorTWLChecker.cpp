// MasterEditorTWLChecker.cpp : メイン プロジェクト ファイルです。

#include "stdafx.h"
#include "check.h"

using namespace System;

int main(array<System::String ^> ^args)
{
	try
	{
		setDebugPrint( true );

		//FilenameItem ^fItem = gcnew FilenameItem;
		//fItem->parseFilename( args[0] );
		//SheetItem ^sItem = gcnew SheetItem;
		//sItem->readSheet( args[0] );
		//checkSheet( fItem, sItem );

		FilenameItem ^fItem = gcnew FilenameItem;
		fItem->parseFilename( args[0] );
		checkRom( fItem, args[0] );
	}
	catch( System::Exception ^ex )
	{
		Console::WriteLine( "NG - " + ex->Message);
		return -1;
	}
	Console::WriteLine( "OK" );
    return 0;
}
