// ResourceTransition.cpp : ���C�� �v���W�F�N�g �t�@�C���ł��B

#include "stdafx.h"

using namespace System;

#define ROOT_PATH "..\\"

int main(array<System::String ^> ^args)
{
	// ��ƃ��[�g����������Q�b�g
	System::String ^root;
	if( (args->Length <= 0) || (System::String::IsNullOrEmpty( args[0] )) )
	{
		root = gcnew System::String(ROOT_PATH);		// �Ȃ��ꍇ�̓f�t�H���g
	}
	else
	{
		root = args[0];
		if( !root->EndsWith( "\\" ) )
		{
			root += "\\";
		}
	}
	Console::WriteLine( "Root path: " + root );

	// �V�����t�H���_/jp/���ꃊ�\�[�X.dll �� jp/ �Ɉڂ�(�R�s�[)
	try
	{
		if( System::IO::File::Exists( root + "�V�����t�H���_\\ja\\MasterEditorTWL.resources.dll" ) )
		{
			Console::WriteLine( "File Exists." );
			if( !System::IO::Directory::Exists( root + "ja" ) )
			{
				System::IO::Directory::CreateDirectory( root + "ja" );
				Console::WriteLine( "Create New Directory." );
			}
			System::IO::File::Copy( root + "�V�����t�H���_\\ja\\MasterEditorTWL.resources.dll", 
									root + "ja\\MasterEditorTWL.resources.dll", true );	// �㏑��
			Console::WriteLine( "Copy Resource DLL." );
		}
		else
		{
			Console::WriteLine( "No File Exists." );
		}
	}
	catch( System::Exception ^ex )
	{
		Console::WriteLine( "error: " + ex->Message );
	}

	//Console::ReadKey();

	return 0;
}
