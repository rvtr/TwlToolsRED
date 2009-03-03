// ----------------------------------------------
// 初期処理 / 終了処理
// ----------------------------------------------

#include "stdafx.h"
#include <apptype.h>
#include "common.h"
#include "srl.h"
#include "deliverable.h"
#include "crc_whole.h"
#include "utility.h"
#include "lang.h"
#include "FormError.h"
#include "Form1.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace MasterEditorTWL;


// ----------------------------------------------
// コンストラクタから呼ばれる初期処理
// ----------------------------------------------
void Form1::construct(void)
{
	// フィールド初期化
	this->hSrl   = gcnew RCSrl( this->getMrcMessageFileJ(), this->getMrcMessageFileE() );
	this->hDeliv = gcnew RCDeliverable;
	this->IsLoadTad = false;
	this->hErrorList = gcnew System::Collections::Generic::List<RCMrcError^>();
	this->hErrorList->Clear();
	this->hWarnList = gcnew System::Collections::Generic::List<RCMrcError^>();
	this->hWarnList->Clear();
	this->IsCheckedUGC     = false;
	this->IsCheckedPhotoEx = false;
	this->IsSpreadSheet = true;
	this->IsReadOnly    = false;
	this->SizeGboxExFlags   = this->gboxExFlags->Size;
	this->hMsg = gcnew RCMessageBank( this->getGuiMessageFileJ(), this->getGuiMessageFileE() );

	// バージョン情報を表示
	//this->labAssemblyVersion->Text = System::Windows::Forms::Application::ProductVersion;
	System::Reflection::Assembly ^ass = System::Reflection::Assembly::GetEntryAssembly();
	this->labAssemblyVersion->Text = "ver." + this->getVersion();

	// TAD読み込みの際に作成される一時ファイルと同名ファイルがあった場合には削除してよいか確認
	//System::Diagnostics::Debug::WriteLine( this->getSplitTadTmpFile() );
	if( System::IO::File::Exists( this->getSplitTadTmpFile() ) )
	{
		this->sucMsg( "E_Start_TmpExist" );
		System::IO::File::Delete( this->getSplitTadTmpFile() );
	}

	// 日付フォームの初期設定を現在の時刻にする
	this->dateRelease->Value = System::DateTime::Now;
	this->dateSubmit->Value  = System::DateTime::Now;

	// システム版のときにはリージョンに全リージョンの項目を追加
#if defined(METWL_VER_APPTYPE_SYSTEM) || defined(METWL_VER_APPTYPE_SECURE) || defined(METWL_VER_APPTYPE_LAUNCHER)
	this->combRegion->Items->Add( gcnew System::String( L"全リージョン" ) );
#endif

	// アプリ種別をつける
	System::String ^appstr = nullptr;
#ifdef METWL_VER_APPTYPE_LAUNCHER
	appstr += "Launcher/";
#endif
#ifdef METWL_VER_APPTYPE_SECURE
	appstr += "Secure/";
#endif
#ifdef METWL_VER_APPTYPE_SYSTEM
	appstr += "System/";
#endif
	if( appstr != nullptr)
	{
		this->Text += " [ Supported App: " + appstr + "User ]";
	}

	// 複数行表示の改行を挿入
	this->tboxGuideRomEditInfo->Text  = this->tboxGuideRomEditInfo->Text->Replace( "<newline>", "\r\n" );
	this->tboxGuideNandSizeInfo->Text = this->tboxGuideNandSizeInfo->Text->Replace( "<newline>", "\r\n" );
	this->tboxGuideErrorInfo->Text    = this->tboxGuideErrorInfo->Text->Replace( "<newline>", "\r\n" );

	// 言語バージョンにあわせてメニューにチェックを入れる
	if( System::Threading::Thread::CurrentThread->CurrentUICulture->Name->StartsWith( "ja" ) )
	{
		this->stripItemJapanese->Checked = true;
		this->stripItemEnglish->Checked  = false;
	}
	else
	{
		this->stripItemJapanese->Checked = false;
		this->stripItemEnglish->Checked  = true;
	}

	// 非表示項目
	this->changeVisibleForms( false );

	// 設定ファイルの読み込み
	this->loadInit();
	this->loadAppendInit();	// 追加設定ファイル

	// 引数処理
	this->handleArgs();
	//System::Diagnostics::Debug::WriteLine( "constructed" );
}


// ----------------------------------------------
// デストラクタから呼ばれる終了処理
// ----------------------------------------------
void Form1::destruct(void)
{
	// TAD読み出しの際に作成される一時SRLファイルを削除(書き出しをせずに終了したときに起こりうる)
	System::String ^srlfile = this->getSplitTadTmpFile();
	if( System::IO::File::Exists( srlfile ) )
	{
		System::IO::File::Delete( srlfile );	// すでに存在する場合は削除
	}
}


// ----------------------------------------------
// コマンドライン引数処理
// ----------------------------------------------
void Form1::handleArgs(void)
{
	cli::array<System::String^> ^args = System::Environment::GetCommandLineArgs();
	if( args->Length < 2 )
	{
		return;
	}

	// 引数でsrlが指定されているときは読み込む
	System::String ^romfile = args[1];
	if( !this->loadRom( romfile ) )
	{
		return;
	}
	System::String ^rompath;
	if( System::IO::Path::IsPathRooted( romfile ) )		// 絶対パスのときはそのまま表示
	{
		rompath = romfile;
	}
	else
	{
		// 表示のために絶対パスに変換
		System::String ^currdir = System::Environment::CurrentDirectory;
		if( !currdir->EndsWith( "\\" ) )
		{
			currdir = currdir + "\\";
		}
		rompath = romfile->Replace( '/', '\\' );
		rompath = currdir + rompath;
	}
	this->tboxFile->Text = rompath;
	this->clearOtherForms();
}
