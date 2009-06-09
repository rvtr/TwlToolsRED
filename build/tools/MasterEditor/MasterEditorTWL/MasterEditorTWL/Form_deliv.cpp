// ----------------------------------------------
// マスタ書類情報(SRL影響なし)をフォームから取得
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
// [sheet <= Form]
// 提出確認書にフォームを反映
// ----------------------------------------------
void Form1::setDeliverableProperties(void)
{
	// プログラムのバージョン
	this->hDeliv->hMasterEditorVersion = this->getVersion();

	// 提出情報
	this->hDeliv->hProductName     = this->tboxProductName->Text;
	this->hDeliv->hProductCode1    = this->tboxProductCode1->Text;
	this->hDeliv->hProductCode2    = this->tboxProductCode2->Text;
	this->hDeliv->IsReleaseForeign = this->cboxReleaseForeign->Checked;
	if( this->cboxReleaseForeign->Checked == true )
	{
		this->hDeliv->hProductNameForeign  = this->tboxProductNameForeign->Text;
		this->hDeliv->hProductCode1Foreign = this->tboxProductCode1Foreign->Text;
		this->hDeliv->hProductCode2Foreign = gcnew System::String("");
		this->hDeliv->hProductCode2Foreign = this->tboxProductCode2Foreign1->Text;
		if( !System::String::IsNullOrEmpty( this->tboxProductCode2Foreign2->Text ) )
		{
			this->hDeliv->hProductCode2Foreign += ("/" + this->tboxProductCode2Foreign2->Text);
		}
		if( !System::String::IsNullOrEmpty( this->tboxProductCode2Foreign3->Text ) )
		{
			this->hDeliv->hProductCode2Foreign += ("/" + this->tboxProductCode2Foreign3->Text);
		}
	}
	else
	{
		this->hDeliv->hProductNameForeign  = nullptr;
		this->hDeliv->hProductCode1Foreign = nullptr;
		this->hDeliv->hProductCode2Foreign = nullptr;
	}
	this->hDeliv->ReleaseYear   = this->dateRelease->Value.Year;
	this->hDeliv->ReleaseMonth  = this->dateRelease->Value.Month;
	this->hDeliv->ReleaseDay    = this->dateRelease->Value.Day;
	this->hDeliv->SubmitYear    = this->dateSubmit->Value.Year;
	this->hDeliv->SubmitMonth   = this->dateSubmit->Value.Month;
	this->hDeliv->SubmitDay     = this->dateSubmit->Value.Day;
	this->hDeliv->SubmitVersion = System::Decimal::ToInt32( this->numSubmitVersion->Value );
	// SDK
	this->hDeliv->hSDK = nullptr;
	if( this->hSrl->hSDKList )
	{
		for each( RCSDKVersion ^sdk in this->hSrl->hSDKList )	// 書類には ARM9 static のバージョン情報を記入する
		{
			if( sdk->IsStatic )
				this->hDeliv->hSDK = sdk->Version;
		}
	}
	if( this->hDeliv->hSDK == nullptr )
	{
		this->hDeliv->hSDK = gcnew System::String( "" );
	}
	// 提出方法
	if( this->rSubmitInternet->Checked == true )
	{
		this->hDeliv->hSubmitWay = this->rSubmitInternet->Text;
	}
	else if( this->rSubmitPost->Checked == true )
	{
		this->hDeliv->hSubmitWay = this->rSubmitPost->Text;
	}
	else
	{
		this->hDeliv->hSubmitWay = this->rSubmitHand->Text;
	}

	// 用途
	if( this->rUsageSale->Checked == true )
	{
		this->hDeliv->hUsage = this->rUsageSale->Text;
	}
	else if( this->rUsageSample->Checked == true )
	{
		this->hDeliv->hUsage = this->rUsageSample->Text;
	}
	else if( this->rUsageDst->Checked == true )
	{
		this->hDeliv->hUsage = this->rUsageDst->Text;
	}
	else if( this->rUsageOther->Checked == true )
	{
		this->hDeliv->hUsage = this->tboxUsageOther->Text;
	}

	// 会社情報
	this->hDeliv->hCompany1    = this->tboxCompany1->Text + " " + this->tboxDepart1->Text;
	this->hDeliv->hPerson1     = this->tboxPerson1->Text;
	if( this->isJapanese() == true )
	{
		this->hDeliv->hFurigana1 = this->tboxFurigana1->Text;
	}
	else
	{
		this->hDeliv->hFurigana1 = nullptr;
	}
	this->hDeliv->hTel1        = this->tboxTel1->Text;
	this->hDeliv->hFax1        = this->tboxFax1->Text;
	this->hDeliv->hMail1       = this->tboxMail1->Text;
	if( this->isJapanese() == true )
	{
		this->hDeliv->hNTSC1 = this->tboxNTSC1->Text;
	}
	else
	{
		this->hDeliv->hNTSC1 = nullptr;
	}
	if( this->cboxIsInputPerson2->Checked == true )
	{
		this->hDeliv->hCompany2    = this->tboxCompany2->Text + " " + this->tboxDepart2->Text;
		this->hDeliv->hPerson2     = this->tboxPerson2->Text;
		if( this->isJapanese() == true )
		{
			this->hDeliv->hFurigana2 = this->tboxFurigana2->Text;
		}
		else
		{
			this->hDeliv->hFurigana2 = nullptr;
		}
		this->hDeliv->hTel2        = this->tboxTel2->Text;
		this->hDeliv->hFax2        = this->tboxFax2->Text;
		this->hDeliv->hMail2       = this->tboxMail2->Text;
		if( this->isJapanese() == true )
		{
			this->hDeliv->hNTSC2 = this->tboxNTSC2->Text;
		}
		else
		{
			this->hDeliv->hNTSC2 = nullptr;
		}
	}
	else
	{
		this->hDeliv->hCompany2    = nullptr;
		this->hDeliv->hPerson2     = nullptr;
		this->hDeliv->hFurigana2   = nullptr;
		this->hDeliv->hTel2        = nullptr;
		this->hDeliv->hFax2        = nullptr;
		this->hDeliv->hMail2       = nullptr;
		this->hDeliv->hNTSC2       = nullptr;
	}

	// 備考
	System::String ^tmp = this->tboxCaption->Text->Replace( " ", "" );
	if( this->tboxCaption->Text->Equals("") || tmp->Equals("") )	// スペースのみの文字列は含めない
	{
		this->hDeliv->hCaption = nullptr;
	}
	else
	{
		this->hDeliv->hCaption = this->tboxCaption->Text;
	}

	// 特殊なアプリ種別
	this->hDeliv->hAppTypeOther = this->tboxAppTypeOther->Text;
	if( this->hDeliv->hAppTypeOther != nullptr )
	{
		this->hDeliv->hAppTypeOther->Replace("\r\n","");
	}

	// バックアップメモリはROMヘッダには記述されないので提出確認書にのみ記載
	if( this->combBackup->SelectedIndex != (this->combBackup->Items->Count - 1) )
	{
		this->hDeliv->hBackupMemory = this->combBackup->SelectedItem->ToString();
	}
	else
	{
		this->hDeliv->hBackupMemory = this->tboxBackupOther->Text;
	}

	//// DSi Wareの販売カテゴリ
	//if( this->combDLCategory->SelectedIndex != (this->combDLCategory->Items->Count - 1) )
	//{
	//	if( this->combDLCategory->SelectedIndex > 0 )
	//	{
	//		this->hDeliv->hDLCategory = this->combDLCategory->SelectedItem->ToString();
	//	}
	//	else
	//	{
	//		this->hDeliv->hDLCategory = gcnew System::String("");
	//	}
	//}
	//else
	//{
	//	this->hDeliv->hDLCategory = this->tboxDLCategoryOther->Text;
	//}

	// SRL情報を文字列で登録
	if( this->combRegion->SelectedIndex < 0 )
	{
		if( this->isJapanese() == true )
			this->hDeliv->hRegion = gcnew System::String("不明");
		else
			this->hDeliv->hRegion = gcnew System::String("Undefined");
	}
	else
	{
		this->hDeliv->hRegion = dynamic_cast<System::String^>(this->combRegion->SelectedItem);
	}
	this->hDeliv->hCERO = this->setDeliverableRatingOgnProperties( this->combCERO );
	this->hDeliv->hESRB = this->setDeliverableRatingOgnProperties( this->combESRB );
	this->hDeliv->hUSK  = this->setDeliverableRatingOgnProperties( this->combUSK );
	this->hDeliv->hPEGI = this->setDeliverableRatingOgnProperties( this->combPEGI );
	this->hDeliv->hPEGI_PRT  = this->setDeliverableRatingOgnProperties( this->combPEGI_PRT );
	this->hDeliv->hPEGI_BBFC = this->setDeliverableRatingOgnProperties( this->combPEGI_BBFC );
	this->hDeliv->hOFLC = this->setDeliverableRatingOgnProperties( this->combOFLC );
	this->hDeliv->hGRB  = this->setDeliverableRatingOgnProperties( this->combGRB );

	// SRLには登録されないROM仕様
	this->hDeliv->IsUGC     = this->cboxIsUGC->Checked;
	this->hDeliv->IsPhotoEx = this->cboxIsPhotoEx->Checked;
}

// ----------------------------------------------
// 提出確認書にレーティング(1団体)のフォームを反映
// ----------------------------------------------
System::String^ Form1::setDeliverableRatingOgnProperties( System::Windows::Forms::ComboBox ^box )
{
	System::String ^str;
	if( this->cboxIsUnnecessaryRating->Checked )
	{
		if( box->FlatStyle == System::Windows::Forms::FlatStyle::Standard )
		{
			str = System::String::Copy( box->Text );	// テキスト入力可になっているので取得できるはず
		}
		else	// リージョンに含まれていない(コンボボックスが表示されていない)ときには不可とする
		{
			if( this->isJapanese() == true )
				str = gcnew System::String("不可");
			else
				str = gcnew System::String("Undefined");
		}
	}
	else
	{
		if( box->SelectedIndex < 0 )
		{
			if( this->isJapanese() == true )
				str = gcnew System::String("不可");
			else
				str = gcnew System::String("Undefined");
		}
		else
		{
			str = dynamic_cast<System::String^>(box->SelectedItem);
		}
	}
	return str;
}

// ----------------------------------------------
// マスタ書類情報(SRL影響なし)のフォームチェック
// ----------------------------------------------
System::Boolean Form1::checkDeliverableForms(void)
{
	// 不正な場合はダイアログで注意してreturn

	// 提出情報
	this->checkTextForm( this->tboxProductName->Text, "LabelProductName" );	// SRL作成には問題のないエラー
	this->checkTextForm( this->tboxProductCode1->Text, "LabelProductCode" );
	this->checkTextForm( this->tboxProductCode2->Text, "LabelProductCode" );
	if( this->cboxReleaseForeign->Checked == true )
	{
		this->checkTextForm( this->tboxProductNameForeign->Text, "LabelProductNameForeign" );
		this->checkTextForm( this->tboxProductCode1Foreign->Text, "LabelProductCodeForeign" );
		this->checkTextForm( this->tboxProductCode2Foreign1->Text,"LabelProductCodeForeign" );
	}
	if( this->rUsageOther->Checked == true )
	{
		this->checkTextForm( this->tboxUsageOther->Text, "LabelUsage" );
	}

	// 会社情報
	this->checkTextForm( this->tboxPerson1->Text, "LabelPerson1" );
	this->checkTextForm( this->tboxCompany1->Text, "LabelCompany1" );
	this->checkTextForm( this->tboxDepart1->Text, "LabelDepart2" );
	if( this->isJapanese() == true )
	{
		this->checkTextForm( this->tboxFurigana1->Text, "LabelFurigana1" );
	}
	this->checkTextForm( this->tboxTel1->Text, "LabelTel1" );
	this->checkTextForm( this->tboxMail1->Text, "LabelMail1" );

	if( this->cboxIsInputPerson2->Checked == true )
	{
		this->checkTextForm( this->tboxPerson2->Text, "LabelPerson2" );
		this->checkTextForm( this->tboxCompany2->Text, "LabelCompany2" );
		this->checkTextForm( this->tboxDepart2->Text, "LabelDepart2" );
		if( this->isJapanese() == true )
		{
			this->checkTextForm( this->tboxFurigana2->Text, "LabelFurigana2" );
		}
		this->checkTextForm( this->tboxTel2->Text, "LabelTel2" );
		this->checkTextForm( this->tboxMail2->Text, "LabelMail2" );
	}

	// 一部のROM情報(SRLバイナリに反映されない情報)をここでチェックする
	this->checkComboBoxIndex( this->combBackup, "LabelBackup", false );
	if( this->combBackup->SelectedIndex == (this->combBackup->Items->Count - 1) )
	{
		this->checkTextForm( this->tboxBackupOther->Text, "LabelBackup" );
	}

	// ひととおりエラー登録をした後で
	// 書類上のエラー(SRLバイナリには影響しない)が存在するかチェック
	return this->isValidOnlyDeliverable();
}

// end of file