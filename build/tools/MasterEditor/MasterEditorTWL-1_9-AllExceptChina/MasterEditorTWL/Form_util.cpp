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
System::Boolean Form1::checkTextForm( System::String ^formtext, System::String ^tag )
{
	System::String ^tmp = formtext->Replace( " ", "" );		// スペースのみの文字列もエラー
	if( (formtext == nullptr) || formtext->Equals("") || tmp->Equals("") )
	{
		System::String ^labelJ = this->hMsg->getMessage(tag,"J");
		System::String ^labelE = this->hMsg->getMessage(tag,"E");
		System::String ^msgJ = this->hMsg->getMessage("TextBoxInput", "J");
		System::String ^msgE = this->hMsg->getMessage("TextBoxInput", "E");
		this->hErrorList->Add( gcnew RCMrcError( labelJ, 0, 0, msgJ, labelE, msgE, true, false, RCMrcError::PurposeType::Common ) );
		return false;
	}
	return true;
}
System::Boolean Form1::checkTextForm( System::String ^formtext, System::String ^title_tag, System::String ^msg_tag )
{
	System::String ^tmp = formtext->Replace( " ", "" );		// スペースのみの文字列もエラー
	if( (formtext == nullptr) || formtext->Equals("") || tmp->Equals("") )
	{
		System::String ^labelJ = this->hMsg->getMessage(title_tag,"J");
		System::String ^labelE = this->hMsg->getMessage(title_tag,"E");
		System::String ^msgJ = this->hMsg->getMessage(msg_tag, "J");
		System::String ^msgE = this->hMsg->getMessage(msg_tag, "E");
		this->hErrorList->Add( gcnew RCMrcError( labelJ, 0, 0, msgJ, labelE, msgE, true, false, RCMrcError::PurposeType::Common ) );
		return false;
	}
	return true;
}

// 数値入力が正常かどうかチェック
System::Boolean Form1::checkNumRange( System::Int32 val, System::Int32 min, System::Int32 max, System::String ^tag )
{
	if( (val < min) || (max < val) )
	{
		System::String ^labelJ = this->hMsg->getMessage(tag,"J");
		System::String ^labelE = this->hMsg->getMessage(tag,"E");
		System::String ^msgJ = this->hMsg->getMessage("NumRangeInput", "J");
		System::String ^msgE = this->hMsg->getMessage("NumRangeInput", "E");
		this->hErrorList->Add( gcnew RCMrcError( labelJ, 0, 0, msgJ, labelE, msgE, true, false, RCMrcError::PurposeType::Common ) );
		return false;
	}
	return true;
}
System::Boolean Form1::checkNumRange( System::String ^strval, System::Int32 min, System::Int32 max, System::String ^tag )
{
	try
	{
		System::Int32  i = System::Int32::Parse(strval);
		return (this->checkNumRange( i, min, max, tag ));
	}
	catch ( System::FormatException ^ex )
	{
		(void)ex;
		return (this->checkNumRange( max+1, min, max, tag ));		// 必ず失敗するように max+1 を検査
	}
}
// コンボボックスをチェック
System::Boolean Form1::checkComboBoxIndex( System::Windows::Forms::ComboBox ^box, System::String ^tag, System::Boolean isAffectRom )
{
	if( box->SelectedIndex < 0 )
	{
		System::String ^labelJ = this->hMsg->getMessage(tag,"J");
		System::String ^labelE = this->hMsg->getMessage(tag,"E");
		System::String ^msgJ = this->hMsg->getMessage("ComboBoxInput", "J");
		System::String ^msgE = this->hMsg->getMessage("ComboBoxInput", "E");
		this->hErrorList->Add( gcnew RCMrcError( 
			labelJ, 0, 0, msgJ, labelE, msgE, true, isAffectRom, RCMrcError::PurposeType::Common ) );
		return false;
	}
	return true;
}

// ラジオボタンをチェック
System::Boolean Form1::checkRadioButton( cli::array<System::Windows::Forms::RadioButton^> ^rbuts, System::String ^tag )
{
	bool checked = false;
	for each( System::Windows::Forms::RadioButton ^r in rbuts )
	{
		if( r->Checked )
		{
			checked = true;
		}
	}
	if( !checked )
	{
		System::String ^labelJ = this->hMsg->getMessage(tag,"J");
		System::String ^labelE = this->hMsg->getMessage(tag,"E");
		System::String ^msgJ = this->hMsg->getMessage("RadioButtonInput", "J");
		System::String ^msgE = this->hMsg->getMessage("RadioButtonInput", "E");
		this->hErrorList->Add( gcnew RCMrcError( labelJ, 0, 0, msgJ, labelE, msgE, true, false, RCMrcError::PurposeType::Common ) );
		return false;
	}
	return true;
}

// --------------------------------------------------------
// エラー情報の登録
// --------------------------------------------------------

// エラー情報の作成
RCMrcError^ Form1::makeErrorMsg( System::Boolean isAffectRom, System::String ^labeltag, System::String ^msgtag, ... cli::array<System::String^> ^args )
{
	// 外部ファイルから項目名を取得
	System::String ^nameJ = this->hMsg->getMessage( labeltag, "J" );
	System::String ^nameE = this->hMsg->getMessage( labeltag, "E" );
	// メッセージを取得
	System::String ^fmtJ  = this->hMsg->getMessage( msgtag, "J" );	// メッセージファイルから書式を取得
	System::String ^msgJ = System::String::Format( fmtJ, args );	// 書式をStringに展開
	System::String ^fmtE  = this->hMsg->getMessage( msgtag, "E" );
	System::String ^msgE = System::String::Format( fmtE, args );

	return (gcnew RCMrcError( nameJ, 0, 0, msgJ, nameE, msgE, true, isAffectRom, RCMrcError::PurposeType::Common ));  // 修正可能なエラーとして作成
}

// 用途のラジオボタンの状態によって表示するかどうかを振り分ける
bool Form1::isDisplayOneGridErrorForPurpose( RCMrcError ^err )
{
	bool is_display = false;

	if( ((err->Purpose & RCMrcError::PurposeType::Production) != (RCMrcError::PurposeType)0) &&
		(this->rPurposeCardProduction->Checked ||
		this->rPurposeDSiWare->Checked || 
		this->rPurposeOther->Checked) )
	{
		is_display = true;
	}
	if( ((err->Purpose & RCMrcError::PurposeType::CardDistribution) != (RCMrcError::PurposeType)0) &&
		this->rPurposeCardDistribution->Checked )
	{
		is_display = true;
	}
	if( ((err->Purpose & RCMrcError::PurposeType::CardKiosk) != (RCMrcError::PurposeType)0) &&
		this->rPurposeCardKiosk->Checked )
	{
		is_display = true;
	}
	if( ((err->Purpose & RCMrcError::PurposeType::TouchTryDS) != (RCMrcError::PurposeType)0) &&
		this->rPurposeCardTouchTryDS->Checked)
	{
		is_display = true;
	}
	/*
		Zone と DSDownload Service は用途のラジオボタンが追加されていないので追加されるまで何もしない
	*/
	return is_display;
} //isDisplayOneGridErrorForPurpose()

// 読み込み時エラーの登録
void Form1::setGridError( void )
{
	this->gridError->Rows->Clear();
	if( this->hSrl->hErrorList != nullptr )		// ROM読み込み時に発見されたエラー
	{
		for each( RCMrcError ^err in this->hSrl->hErrorList )
		{
			if( this->isDisplayOneGridErrorForPurpose( err ) )
			{
				this->gridError->Rows->Add( err->getAll(this->isJapanese()) );
				this->colorGridError( err );
			}
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
			if( this->isDisplayOneGridErrorForPurpose( err ) )
			{
				this->gridWarn->Rows->Add( err->getAll(this->isJapanese()) );
				this->colorGridWarn( err );
			}
		}
	}
}

// 読み込み時に検出した修正可能エラーに現在の入力を反映
void Form1::overloadGridError( void )
{
	this->gridError->Rows->Clear();
	if( this->hSrl->hErrorList != nullptr )		// ROM読み込み時に発見されたエラー
	{
		for each( RCMrcError ^err in this->hSrl->hErrorList )
		{
			if( !err->IsEnableModify )	// 修正可能な情報は表示しない
			{
				if( this->isDisplayOneGridErrorForPurpose( err ) )
				{
					this->gridError->Rows->Add( err->getAll(this->isJapanese()) );
					this->colorGridError( err );
				}
			}
		}
	}
	if( this->hErrorList != nullptr )	// 現在のフォーム入力から発見されたエラー
	{
		for each( RCMrcError ^err in this->hErrorList )
		{
			if( this->isDisplayOneGridErrorForPurpose( err ) )
			{
				this->gridError->Rows->Add( err->getAll(this->isJapanese()) );
				this->colorGridError( err );
			}
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
				if( this->isDisplayOneGridErrorForPurpose( err ) )
				{
					this->gridWarn->Rows->Add( err->getAll(this->isJapanese()) );
					this->colorGridWarn( err );
				}
			}
		}
	}
	if( this->hWarnList != nullptr )
	{
		for each( RCMrcError ^err in this->hWarnList )
		{
			if( this->isDisplayOneGridErrorForPurpose( err ) )
			{
				this->gridWarn->Rows->Add( err->getAll(this->isJapanese()) );
				this->colorGridWarn( err );
			}
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
	if( !System::String::IsNullOrEmpty(extension) && !(dlg->FileName->ToLower()->EndsWith( extension->ToLower() )) )
	{
		retfile += extension;
	}
	return retfile;
}

// セーブするディレクトリをダイアログで取得
// @ret 取得したディレクトリ名(\\で終わるように調整される) エラーのときnullptr
System::String^ Form1::saveDirDlg( System::String ^msg )
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
	dlg->Description = msg;		// ダイアログの上部に出るメッセージ

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
		if( MessageBox::Show( this->makeMsg( "OverWrite", path ), "Information", MessageBoxButtons::YesNo, MessageBoxIcon::None ) 
			== System::Windows::Forms::DialogResult::No )
		{
			return false;
		}
	}
	return true;	// ファイルが存在しない場合 || 上書きOKの場合 ファイルを作成してもよい
}


// end of file