// split_tad_console.cpp : メイン プロジェクト ファイルです。

#include "stdafx.h"
#include "twl/types.h"
#include <cstring>
#include <cstdio>
#include "split_tad.h"

using namespace System;

int main(array<System::String ^> ^args)
{
	splitTad( args[0] );

	return 0;
}

