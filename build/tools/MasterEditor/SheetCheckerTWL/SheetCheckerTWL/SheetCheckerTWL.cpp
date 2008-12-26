// SheetCheckerTWL.cpp : ���C�� �v���W�F�N�g �t�@�C���ł��B

#include "stdafx.h"
#include "SheetCheckerTWL.h"
#include "crc_whole.h"
#include <twl/types.h>
#include <twl/os/common/format_rom.h>
#include <cstdio>
#include "Form1.h"

using namespace SheetCheckerTWL;

// ------------------------------------------------------------------
// �錾
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

	// �R���g���[�����쐬�����O�ɁAWindows XP �r�W���A�����ʂ�L���ɂ��܂�
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// ���C�� �E�B���h�E���쐬���āA���s���܂�
	Application::Run(gcnew Form1());
	return 0;
}

// ------------------------------------------------------------------
// �R���\�[������ (�m�[�}��)
// ------------------------------------------------------------------

int consoleRun( array<System::String ^> ^args, int argc, SheetCheckerContext ^context )
{
	SheetCheckerError error = SheetCheckerError::NOERROR;
	ROM_Header rh;
	memset( (void*)&rh, 0, sizeof(ROM_Header) );
	SheetItem ^item = gcnew SheetItem;

	// ��������
	if( argc != 2 )
	{
		error = SheetCheckerError::ERROR_ARG;
		printResult( context, &rh, item, nullptr, nullptr, 0, error );
		return ((int)error);
	}
	System::String ^romfile   = args[0];
	System::String ^sheetfile = args[1];

	// ROM�w�b�_�̓ǂݍ���
	if( !readRomHeader( romfile, &rh ) )
	{
		error = SheetCheckerError::ERROR_READ_SRL;
		printResult( context, &rh, item, romfile, sheetfile, 0, error );
		return ((int)error);
	}
	System::UInt16 crc;
	getWholeCRCInFile( romfile, &crc );

	// ��o�m�F���̓ǂݍ���
	if( !readSheet( sheetfile, item ) )
	{
		error = SheetCheckerError::ERROR_READ_SHEET;
		printResult( context, &rh, item, romfile, sheetfile, crc, error );
		return ((int)error);
	}

	// ��v����
	error = checkSheet( &rh, crc, item );
	printResult( context, &rh, item, romfile, sheetfile, crc, error );	// ���ʂ�\�`���ŕ\��

	return ((int)error);
}

// ------------------------------------------------------------------
// �R���\�[������ (�I�v�V����)
// ------------------------------------------------------------------

int consoleOptionRun( array<System::String ^> ^args, const int argc, SheetCheckerContext ^context )
{
	SheetCheckerError  error = SheetCheckerError::NOERROR;
	System::String ^romfile;
	System::String ^sheetfile;

	// �������
	switch( argc )
	{
		case 1:
			sheetfile = args[0];
			if( context->bResult )		// ���̃I�v�V�����̂Ƃ������͈���1�������Ȃ�
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

	// ��o�m�F���̓ǂݍ���
	SheetItem ^item = gcnew SheetItem;
	if( !readSheet( sheetfile, item ) )
	{
		error = SheetCheckerError::ERROR_READ_SHEET;
		printf( "%d", (int)error );
		return ((int)error);
	}

	// ������1�̂Ƃ���SRL�Ƃ̔�r�����Ȃ�
	if( argc == 2 )
	{
		ROM_Header rh;
		memset( (void*)&rh, 0, sizeof(ROM_Header) );

		// ROM�w�b�_�̓ǂݍ���
		if( !readRomHeader( romfile, &rh ) )
		{
			error = SheetCheckerError::ERROR_READ_SHEET;
			printf( "%d", (int)error );
			return ((int)error);
		}
		System::UInt16 crc;
		getWholeCRCInFile( romfile, &crc );

		// ��r
		error = checkSheet( &rh, crc, item );
	}//if( argc == 2 )
	
	// ���ʏo�͂̓I�v�V�����ɂ���ĈقȂ�(�G���[�̂Ƃ��ɂ͋���)
	if( error != SheetCheckerError::NOERROR )
	{
		if( context->bResult )
		{
			printf( "0" );		// ���̃p�X�͐����̂Ƃ��̂�
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
// ��v����
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
// ���ʕ\��
// ------------------------------------------------------------------

int printResult( SheetCheckerContext ^context, ROM_Header *rh, SheetItem ^item, 
				System::String ^srlfile, System::String ^sheetfile, System::UInt16 srlcrc, SheetCheckerError error )
{
	System::UInt16 tadver = item->RomVersion;
	tadver = (tadver << 8) | item->SubmitVersion;

	// �ʏ�̕\��
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


