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
// フォームのチェック
// ----------------------------------------------

// テキスト入力がされているかチェック
System::Boolean Form1::checkTextForm( System::String ^formtext, System::String ^labelJ, System::String ^labelE, System::Boolean affectRom )
{
	System::String ^msgJ = gcnew System::String( "入力されていません。" );
	System::String ^msgE = gcnew System::String( "No item is set. Please retry to input." );

	System::String ^tmp = formtext->Replace( " ", "" );		// スペースのみの文字列もエラー
	if( (formtext == nullptr) || formtext->Equals("") || tmp->Equals("") )
	{
		this->hErrorList->Add( gcnew RCMrcError( labelJ, METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE, msgJ, labelE, msgE, true, affectRom ) );
		return false;
	}
	return true;
}
// 数値入力が正常かどうかチェック
System::Boolean Form1::checkNumRange( 
	System::Int32 val, System::Int32 min, System::Int32 max, System::String ^labelJ, System::String ^labelE, System::Boolean affectRom )
{
	System::String ^msgJ = gcnew System::String( "値の範囲が不正です。やり直してください。" );
	System::String ^msgE = gcnew System::String( "Invalidate range of value. Please retry." );

	if( (val < min) || (max < val) )
	{
		this->hErrorList->Add( gcnew RCMrcError( labelJ, METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE, msgJ, labelE, msgE, true, affectRom ) );
		return false;
	}
	return true;
}
System::Boolean Form1::checkNumRange( System::String ^strval, System::Int32 min, System::Int32 max, 
							          System::String ^labelJ, System::String ^labelE, System::Boolean affectRom )
{
	try
	{
		System::Int32  i = System::Int32::Parse(strval);
		return (this->checkNumRange( i, min, max, labelJ, labelE, affectRom ));
	}
	catch ( System::FormatException ^ex )
	{
		(void)ex;
		return (this->checkNumRange( max+1, min, max, labelJ, labelE, affectRom ));		// 必ず失敗するように max+1 を検査
	}
}
// コンボボックスをチェック
System::Boolean Form1::checkBoxIndex( System::Windows::Forms::ComboBox ^box, System::String ^labelJ, System::String ^labelE, System::Boolean affectRom )
{
	System::String ^msgJ = gcnew System::String( "選択されていません。" );
	System::String ^msgE = gcnew System::String( "One item is not selected." );
	
	if( box->SelectedIndex < 0 )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			labelJ, METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE, msgJ, labelE, msgE, true, affectRom ) );
	}
	return true;
}


// --------------------------------------------------------
// エラー情報の登録
// --------------------------------------------------------

// 読み込み時エラーの登録
void Form1::setGridError( void )
{
	this->gridError->Rows->Clear();
	if( this->hSrl->hErrorList != nullptr )
	{
		for each( RCMrcError ^err in this->hSrl->hErrorList )
		{
			this->gridError->Rows->Add( err->getAll(this->isJapanese()) );
			this->colorGridError( err );
		}
	}
}

void Form1::setGridWarn( void )
{
	this->gridWarn->Rows->Clear();
	if( this->hSrl->hWarnList != nullptr )
	{
		for each( RCMrcError ^err in this->hSrl->hWarnList )
		{
			this->gridWarn->Rows->Add( err->getAll(this->isJapanese()) );
			this->colorGridWarn( err );
		}
	}
}

// 読み込み時に検出した修正可能エラーに現在の入力を反映
void Form1::overloadGridError( void )
{
	this->gridError->Rows->Clear();
	if( this->hSrl->hErrorList != nullptr )
	{
		for each( RCMrcError ^err in this->hSrl->hErrorList )
		{
			if( !err->IsEnableModify )	// 修正可能な情報は表示しない
			{
				this->gridError->Rows->Add( err->getAll(this->isJapanese()) );
				this->colorGridError( err );
			}
		}
	}
	if( this->hErrorList != nullptr )
	{
		for each( RCMrcError ^err in this->hErrorList )
		{
			this->gridError->Rows->Add( err->getAll(this->isJapanese()) );
			this->colorGridError( err );
		}
	}
}
void Form1::overloadGridWarn( void )
{
	this->gridWarn->Rows->Clear();
	if( this->hSrl->hWarnList != nullptr )
	{
		for each( RCMrcError ^err in this->hSrl->hWarnList )
		{
			if( !err->IsEnableModify )
			{
				this->gridWarn->Rows->Add( err->getAll(this->isJapanese()) );
				this->colorGridWarn( err );
			}
		}
	}
	if( this->hWarnList != nullptr )
	{
		for each( RCMrcError ^err in this->hWarnList )
		{
			this->gridWarn->Rows->Add( err->getAll(this->isJapanese()) );
			this->colorGridWarn( err );
		}
	}
}

// セルの色を変える
void Form1::colorGridError( RCMrcError ^err )
{
	if( err->IsAffectRom && !err->IsEnableModify )		// SRLに関係ありで修正不可
	{
		System::Int32 last = this->gridError->Rows->Count - 2;	// 追加直後の行
		this->gridError->Rows[ last ]->DefaultCellStyle->ForeColor = System::Drawing::Color::Red;
	}
	else if( err->IsAffectRom && err->IsEnableModify )	// SRLに関係ありで修正可
	{
		System::Int32 last = this->gridError->Rows->Count - 2;
		this->gridError->Rows[ last ]->DefaultCellStyle->ForeColor = System::Drawing::Color::Blue;
	}
}
void Form1::colorGridWarn( RCMrcError ^err )
{
	if( err->IsAffectRom && !err->IsEnableModify )
	{
		System::Int32 last = this->gridWarn->Rows->Count - 2;
		this->gridWarn->Rows[ last ]->DefaultCellStyle->ForeColor = System::Drawing::Color::Red;
	}
	else if( err->IsAffectRom && err->IsEnableModify )
	{
		System::Int32 last = this->gridWarn->Rows->Count - 2;
		this->gridWarn->Rows[ last ]->DefaultCellStyle->ForeColor = System::Drawing::Color::Blue;
	}
}

// まとめて更新
void Form1::updateGrid(void)
{
	if( this->rErrorReading->Checked == true )
	{
		this->setGridError();
		this->setGridWarn();
	}
	else
	{
		if( !System::String::IsNullOrEmpty(this->tboxFile->Text) )
		{
			this->hErrorList->Clear();
			this->hWarnList->Clear();
			this->checkSrlForms();
			this->checkDeliverableForms();
			this->overloadGridError();
			this->overloadGridWarn();
		}
	}
}

// ----------------------------------------------
// エラーカウント
// ----------------------------------------------

// SRLには関係しない書類上のエラーをチェック
System::Boolean Form1::isValidOnlyDeliverable(void)
{
	System::Int32 count = 0;

	// SRLクラスのエラーリストはすべてSRLに関係するのでチェックしない
	// -> 入力エラーのみのチェックでよい
	for each( RCMrcError ^err in this->hErrorList )
	{
		if( !err->IsAffectRom )
			count++;
	}
	return (count == 0);
}

// SRLのバイナリに影響する項目にエラーがあるかチェック
System::Boolean Form1::isValidAffectRom(void)
{
	System::Int32 count = 0;

	// SRLクラスの修正不可エラーをカウント
	// (修正可エラーは入力によって修正されてるかもしれないのでチェックしない)
	for each( RCMrcError ^err in this->hSrl->hErrorList )
	{
		if( !err->IsEnableModify )	// すべてSRLバイナリに影響する
			count++;
	}

	// SRLバイナリに影響するエラーの中で
	// 修正可エラーがフォーム入力によって修正されているかカウント
	// (エラーリストが更新されていることが前提)
	for each( RCMrcError ^err in this->hErrorList )
	{
		if( err->IsAffectRom )		// 修正不可エラーは存在しない
			count++;
	}
	return (count == 0);
}

// SRLのバイナリに影響する項目の中で修正可能なエラーだけをチェック
System::Boolean Form1::isValidAffectRomModified(void)
{
	System::Int32 count = 0;
	for each( RCMrcError ^err in this->hErrorList )
	{
		if( err->IsAffectRom )		// 修正不可エラーは存在しない
			count++;
	}
	return (count == 0);
}

// ----------------------------------------------
// ファイル操作ユーティリティ
// ----------------------------------------------

// ファイルをダイアログで取得
// @arg [in] 拡張子フィルタ
// @ret 取得したファイル名 エラーのとき nullptr
System::String^ Form1::openFileDlg( System::String ^filter )
{
	System::Windows::Forms::OpenFileDialog ^dlg = gcnew (OpenFileDialog);
	if( System::String::IsNullOrEmpty( this->prevDir ) || !System::IO::Directory::Exists( this->prevDir ) )
	{
		dlg->InitialDirectory = System::Environment::GetFolderPath( System::Environment::SpecialFolder::Desktop );
	}
	else
	{
		dlg->InitialDirectory = this->prevDir;	// 前に選んだディレクトリをデフォルトにする
	}
	dlg->Filter      = filter;
	dlg->FilterIndex = 1;
	dlg->RestoreDirectory = true;

	if( dlg->ShowDialog() != System::Windows::Forms::DialogResult::OK )
	{
		return nullptr;
	}
	this->prevDir = System::IO::Path::GetDirectoryName( dlg->FileName );	// デフォルトディレクトリの更新
	return System::String::Copy(dlg->FileName);
}

// セーブするファイルをダイアログで取得
// @arg [in] 拡張子フィルタ
// @arg [in] ファイルの拡張子が不正なときに追加するときの正しい拡張子
// @ret 取得したファイル名 エラーのとき nullptr
System::String^ Form1::saveFileDlg( System::String ^filter, System::String ^extension )
{
	System::String ^retfile;
	System::Windows::Forms::SaveFileDialog ^dlg = gcnew (SaveFileDialog);

	if( System::String::IsNullOrEmpty( this->prevDir ) || !System::IO::Directory::Exists( this->prevDir ) )
	{
		dlg->InitialDirectory = System::Environment::GetFolderPath( System::Environment::SpecialFolder::Desktop );
	}
	else
	{
		dlg->InitialDirectory = this->prevDir;
	}
	dlg->Filter      = filter;
	dlg->FilterIndex = 1;
	dlg->RestoreDirectory = true;

	if( dlg->ShowDialog() != System::Windows::Forms::DialogResult::OK )
	{
		return nullptr;
	}
	this->prevDir = System::IO::Path::GetDirectoryName( dlg->FileName );
	retfile = dlg->FileName;
	if( !System::String::IsNullOrEmpty(extension) && !(dlg->FileName->EndsWith( extension )) )
	{
		retfile += extension;
	}
	return retfile;
}

// セーブするディレクトリをダイアログで取得
// @ret 取得したディレクトリ名(\\で終わるように調整される) エラーのときnullptr
System::String^ Form1::saveDirDlg( System::String ^msgJ, System::String ^msgE )
{
	System::String ^dir;
	System::Windows::Forms::FolderBrowserDialog ^dlg = gcnew (System::Windows::Forms::FolderBrowserDialog);

	// デフォルトのフォルダを前に選択したフォルダにする
	dlg->RootFolder = System::Environment::SpecialFolder::Desktop;
	if( System::String::IsNullOrEmpty( this->prevDir ) || !System::IO::Directory::Exists( this->prevDir ) )
	{
		dlg->SelectedPath = System::Environment::GetFolderPath( System::Environment::SpecialFolder::Desktop );
	}
	else
	{
		dlg->SelectedPath = this->prevDir;
	}
	dlg->ShowNewFolderButton = true;

	// ダイアログの上部に出るメッセージ
	if( this->isEnglish() && msgE )
	{
		dlg->Description = msgE;
	}
	else if( msgJ )
	{
		dlg->Description = msgJ;
	}

	if( dlg->ShowDialog() != System::Windows::Forms::DialogResult::OK )
	{
		return nullptr;
	}

	if( !dlg->SelectedPath->EndsWith("\\") )
	{
		dir = dlg->SelectedPath + "\\";
	}
	else
	{
		dir = System::String::Copy(dlg->SelectedPath);
	}
	this->prevDir = dir;	// 選択したフォルダを次のフォルダにする
	return dir;
}

// ファイルが存在するかを調べて上書き確認をする
bool Form1::isOverwriteFile( System::String ^path )
{
	if( System::IO::File::Exists( path ) )
	{
		System::String ^msg;
		if( this->isJapanese() )
		{
			msg = gcnew System::String( path + "はすでに存在します。上書きしますか?" );
		}
		else
		{
			msg = gcnew System::String( path + "already exists. Overwrite it?" );
		}

		if( MessageBox::Show( msg, "Information", MessageBoxButtons::YesNo, MessageBoxIcon::None ) 
			== System::Windows::Forms::DialogResult::No )
		{
			return false;
		}
	}
	return true;	// ファイルが存在しない場合 || 上書きOKの場合 ファイルを作成してもよい
}


// end of file