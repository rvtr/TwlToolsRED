// MasterEditorTWLChecker.cpp : ���C�� �v���W�F�N�g �t�@�C���ł��B

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

		// �g���q�Ŕ���
		if( System::IO::Path::GetExtension(args[1])->ToUpper() == ".XML" )
		{
			// args[0] �̓_�~�[
			System::String ^sheet = args[1];
			DebugPrint( "Sheet file : " + sheet );
			Console::Write( System::IO::Path::GetFileName( sheet ) + "\t" );
			DebugPrint( "\n" );

			FilenameItem ^fItem = gcnew FilenameItem;
			fItem->parseFilename( sheet );
			SheetItem ^sItem = gcnew SheetItem;
			sItem->readSheet( sheet );
			checkSheet( fItem, sItem );
		}
		else
		{
			System::String ^original = args[0];
			System::String ^target   = args[1];
			DebugPrint( "Original file : " + original );
			DebugPrint( "Target file   : " + target );
			Console::Write( System::IO::Path::GetFileName( target ) + "\t" );
			DebugPrint( "\n" );

			FilenameItem ^fItem = gcnew FilenameItem;
			fItem->parseFilename( target );
			checkRom( fItem, original, target );
		}
	}
	catch( System::Exception ^ex )
	{
		Console::WriteLine( "NG - " + ex->Message);
		return -1;
	}
	Console::WriteLine( "OK" );
    return 0;
}

// ------------------------------------------------------------------
// getopt
// ------------------------------------------------------------------

// @ret �I�v�V�������������Ƃ���argc
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
		else if( !args[i]->StartsWith( "-" ) )	// �I�v�V�����łȂ�������index���L�^
		{
			indexList->Add(i);
		}
	}
	i=0;
	for each( System::Int32 index in indexList )	// �I�v�V�����łȂ�������O�ɂ߂Ă���
	{
		args[i] = args[index];
		i++;
	}
	return (args->Length - numopt);
}
