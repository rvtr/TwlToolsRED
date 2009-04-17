// FingerPrinterTWL.cpp : メイン プロジェクト ファイルです。

#include "stdafx.h"
#include "Form1.h"

extern void fingerprintConsole( cli::array<System::String^> ^args );

using namespace FingerPrinterTWL;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// GUIモードかチェック
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
		// コントロールが作成される前に、Windows XP ビジュアル効果を有効にします
		Application::EnableVisualStyles();
		Application::SetCompatibleTextRenderingDefault(false); 

		// メイン ウィンドウを作成して、実行します
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
