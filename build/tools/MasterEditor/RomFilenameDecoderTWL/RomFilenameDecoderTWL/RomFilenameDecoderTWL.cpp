// RomFilenameDecoderTWL.cpp : メイン プロジェクト ファイルです。

#include "stdafx.h"
#include "rfd_context.h"
#include <cstdio>
#include <cstring>
#include <twl/types.h>
#include <twl/os/common/format_rom.h>

using namespace System;

int printResult( RomFilenameDecoderTWL::RCContext ^context, 
				 RomFilenameDecoderTWL::RCFilenameItems ^items );

int main(array<System::String ^> ^args)
{
	// 引数解析
	RomFilenameDecoderTWL::RCContext ^context 
		= gcnew RomFilenameDecoderTWL::RCContext( args );
	// ファイル名の解析
	RomFilenameDecoderTWL::RCFilenameItems ^items 
		= gcnew RomFilenameDecoderTWL::RCFilenameItems( context->Romfile );
	// 比較
	items->compareRomHeader();

	int retval = printResult( context, items );
    return retval;
}

// -------------------------------------------------------
// エラー解析
// -------------------------------------------------------
int printResult( RomFilenameDecoderTWL::RCContext ^context, 
				 RomFilenameDecoderTWL::RCFilenameItems ^items )
{
	System::UInt16 tadver = items->RemasterVersion;
	tadver = (tadver << 8) | ((System::UInt16)items->SubmissionVersion);

	if( context->IsSubmitVersion )
	{
		if( items->Error != RomFilenameDecoderTWL::ECError::NOERROR )
		{
			printf( "%d", (int)items->Error );
		}
		else
		{
			printf( "%d", (int)items->SubmissionVersion );
		}
	}

	else if( context->IsTadVersion )
	{
		if( items->Error != RomFilenameDecoderTWL::ECError::NOERROR )
		{
			printf( "%d", (int)items->Error );
		}
		else
		{
			printf( "%d", (int)tadver );
		}
	}

	else
	{
		System::Console::WriteLine();
		System::Console::WriteLine( "Rom Filename: " + context->Romfile );
		System::Console::WriteLine();

		const char *rhgc = items->Rh->s.game_code;
		char fgc[4];
		RomFilenameDecoderTWL::setStringToChars( fgc, items->InitialCode, 4, 0 );
		printf( "                    RomHeader   Filename\n" );
		printf( "----------------------------------------------\n" );
		printf( "InitialCode:        %c%c%c%c        %c%c%c%c\n", rhgc[0], rhgc[1], rhgc[2], rhgc[3], fgc[0], fgc[1], fgc[2], fgc[3] );
		printf( "RemasterVersion:    %02X          %02X\n", items->Rh->s.rom_version, items->RemasterVersion );
		printf( "SubmissionVersion:  -           %d(%02X)\n", items->SubmissionVersion, items->SubmissionVersion );
		printf( "----------------------------------------------\n" );
		printf( "TAD Version:        -           %d(%04X)\n", (int)tadver, tadver );
		printf( "----------------------------------------------\n" );
		printf( "Result:             " );
		if( items->Error == RomFilenameDecoderTWL::ECError::NOERROR )
		{
			printf( "OK\n" );
		}
		else
		{
			printf( "NG (%d)\n", (int)items->Error );
		}
	}
	return ((int)items->Error);
}