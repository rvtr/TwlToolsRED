// MasterEditorTWLChecker.cpp : メイン プロジェクト ファイルです。

#include "stdafx.h"
#include "check.h"

using namespace System;

int main(array<System::String ^> ^args)
{
	FilenameItem ^fItem = gcnew FilenameItem;
	fItem->parseFilename( args[0] );

	SheetItem ^sItem = gcnew SheetItem;
	sItem->readSheet( args[0] );

	System::String^ errmsg = checkSheet( fItem, sItem );
	if( errmsg != nullptr )
	{
		Console::WriteLine( "NG " + errmsg );
	}
	else
	{
		Console::WriteLine( "OK" );
	}

	Console::WriteLine(L"Hello World");
    return 0;
}
