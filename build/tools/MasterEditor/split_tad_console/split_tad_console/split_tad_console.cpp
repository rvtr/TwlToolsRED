// split_tad_console.cpp : メイン プロジェクト ファイルです。

#include "stdafx.h"
#include "twl/types.h"
#include <cstring>
#include <cstdio>
#include "split_tad.h"

using namespace System;

int main(array<System::String ^> ^args)
{
	if( args->Length < 1 )
	{
		printf( "argument error\n" );
		return -1;
	}
	System::String ^tad = args[0];
	System::String ^srl = (args->Length >=2)?args[1]:"out.srl";

	int result = splitTad( tad, srl );
	if( result < 0 )
	{
		printf( "\nerror\n" );
	}
	else
	{
		printf( "\nsucceeded\n" );
	}

	return result;
}

