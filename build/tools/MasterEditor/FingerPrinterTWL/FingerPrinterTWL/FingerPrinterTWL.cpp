// FingerPrinterTWL.cpp : メイン プロジェクト ファイルです。

#include "stdafx.h"
#include "Form1.h"

extern void fingerprintConsole( cli::array<System::String^> ^args );
void setpath();

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
			setpath();
			break;
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
		setpath();	// コマンドライン起動のときにはPATHを設定する
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

// 環境変数PATHの設定
void setpath()
{
	System::String ^bindir = System::IO::Path::GetDirectoryName( System::Reflection::Assembly::GetEntryAssembly()->Location );
	System::Environment::SetEnvironmentVariable( "PATH", bindir );
}

