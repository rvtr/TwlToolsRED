// FingerPrinterTWL.cpp : ���C�� �v���W�F�N�g �t�@�C���ł��B

#include "stdafx.h"
#include "Form1.h"

extern void fingerprintConsole( cli::array<System::String^> ^args );
void setpath();

using namespace FingerPrinterTWL;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// GUI���[�h���`�F�b�N
	bool guimode = false;
	for each( System::String ^arg in args )
	{
		if( arg->StartsWith("-") && (arg->IndexOf('g') >= 0) )
		{
			guimode = true;
			setpath();
			break;
		}
	}

	if( (args->Length == 0) || guimode )
	{
		// �R���g���[�����쐬�����O�ɁAWindows XP �r�W���A�����ʂ�L���ɂ��܂�
		Application::EnableVisualStyles();
		Application::SetCompatibleTextRenderingDefault(false); 

		// ���C�� �E�B���h�E���쐬���āA���s���܂�
		Application::Run(gcnew Form1());
	}
	else
	{
		setpath();	// �R�}���h���C���N���̂Ƃ��ɂ�PATH��ݒ肷��
		try
		{
			fingerprintConsole( args );
		}
		catch( System::Exception ^ex )
		{
			System::Console::WriteLine(ex->Message);
			return -1;
		}
	}
	return 0;
}

// ���ϐ�PATH�̐ݒ�
void setpath()
{
	System::String ^bindir = System::IO::Path::GetDirectoryName( System::Reflection::Assembly::GetEntryAssembly()->Location );
	System::Environment::SetEnvironmentVariable( "PATH", bindir );
}

