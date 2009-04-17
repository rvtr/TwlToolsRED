// FingerPrinterTWL.cpp : ���C�� �v���W�F�N�g �t�@�C���ł��B

#include "stdafx.h"
#include "Form1.h"

extern void fingerprintConsole( cli::array<System::String^> ^args );

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
