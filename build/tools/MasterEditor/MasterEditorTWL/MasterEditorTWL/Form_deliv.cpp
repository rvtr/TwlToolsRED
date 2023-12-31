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
    this->hDeliv->hSubmitWay = gcnew System::String( "" );

	// 用途
	cli::array<System::Windows::Forms::RadioButton ^> ^rbuts = gcnew cli::array<System::Windows::Forms::RadioButton ^>
	{
		this->rPurposeCardProduction,
		this->rPurposeCardTouchTryDS,
		this->rPurposeCardDistribution,
		this->rPurposeCardKiosk,
		this->rPurposeDSiWare,
		this->rPurposeOther,
	};
	for each( System::Windows::Forms::RadioButton ^r in rbuts )
	{
		if( r->Checked )
		{
			this->hDeliv->hUsage = r->Text;
		}
	}
	if( this->rPurposeOther->Checked )
	{
		this->hDeliv->hUsageDetail = this->tboxPurposeOther->Text;
	}
	else
	{
		this->hDeliv->hUsageDetail = "";
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

	// リージョン
	if( this->combRegion->DropDownStyle == System::Windows::Forms::ComboBoxStyle::DropDown )
	{
		// 中韓のときはリストで選択不可のテキストがコンボボックスに書かれているのでそれを登録
		this->hDeliv->hRegion = this->combRegion->Text;
	}
	else
	{
		// WorldWide のときはリストで選択可能な文字列を登録
		if( this->combRegion->SelectedIndex < 0 )
		{
			this->hDeliv->hRegion = this->isJapanese()?METWL_STRING_UNDEFINED_REGION_J:METWL_STRING_UNDEFINED_REGION_E;	// 起こり得ない
		}
		else
		{
			// リストで選択されているテキストを登録
			this->hDeliv->hRegion = dynamic_cast<System::String^>(this->combRegion->SelectedItem);
		}
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

	if( !this->tboxPrivateSaveDataPurpose->Enabled )
	{
		if( this->isJapanese() )
		{
			this->hDeliv->hPrivateSaveDataPurpose = "なし";
		}
		else
		{
			this->hDeliv->hPrivateSaveDataPurpose = "None";
		}
	}
	else
	{
		this->hDeliv->hPrivateSaveDataPurpose = this->tboxPrivateSaveDataPurpose->Text;
	}
}

// ----------------------------------------------
// 提出確認書にレーティング(1団体)のフォームを反映
// ----------------------------------------------
System::String^ Form1::setDeliverableRatingOgnProperties( System::Windows::Forms::ComboBox ^box )
{
	System::String ^str;

	// 中国リージョン特別対応
	if( (this->combRegion->DropDownStyle == System::Windows::Forms::ComboBoxStyle::DropDown) // コンボボックスが中韓リージョン用になっているとき
		&& this->hSrl->IsRegionChina )	// SRLが読み込まれていることは保証される
	{
		if( this->cboxIsUnnecessaryRating->Checked )
		{
			// レーティング表示不要が選択されているときには団体が中国リージョンに含まれていなくても「レーティング表示不要」となる
			str = this->isJapanese()?METWL_STRING_UNNECESSARY_RATING_J:METWL_STRING_UNNECESSARY_RATING_E;
		}
		else
		{
			// 中国リージョンに含まれていなくても「全年齢」
			str = this->isJapanese()?METWL_STRING_CHINA_RATING_FREE_J:METWL_STRING_CHINA_RATING_FREE_E;
		}
		return str;
	}

	// リージョンに含まれる団体のみレーティングが設定される それ以外は「不可」となる
	if( this->cboxIsUnnecessaryRating->Checked )	// レーティング表示不要が選択されているとき
	{
		if( box->FlatStyle == System::Windows::Forms::FlatStyle::Standard )	// リージョンに含まれているときの判定
		{
			// リージョンに含まれている団体のみ「レーティング表示不要」とする
			str = this->isJapanese()?METWL_STRING_UNNECESSARY_RATING_J:METWL_STRING_UNNECESSARY_RATING_E;
		}
		else
		{
			// リージョンに含まれていない(コンボボックスが表示されていない)ときには「不可」とする
			str = this->isJapanese()?METWL_STRING_UNDEFINED_RATING_J:METWL_STRING_UNDEFINED_RATING_E;
		}
	}
	else
	{
		if( box->SelectedIndex < 0 )
		{
			// リージョンに含まれていない(コンボボックスが表示されていない)ときには「不可」とする
			str = this->isJapanese()?METWL_STRING_UNDEFINED_RATING_J:METWL_STRING_UNDEFINED_RATING_E;
		}
		else
		{
			// リージョンに含まれている団体にはリストで選択されているテキストを代入
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
	if( this->rPurposeOther->Checked )
	{
		this->checkTextForm( this->tboxPurposeOther->Text, "LabelUsage" );
	}

	//「用途」の項目はグループボックスの機能を使っておらず
	// もしかしたらチェック漏れが発生するかもしれないのでチェックが入っているかを調べておく
	cli::array<System::Windows::Forms::RadioButton^> ^rbuts = gcnew cli::array<System::Windows::Forms::RadioButton ^>
	{
		this->rPurposeCardProduction,
		this->rPurposeCardTouchTryDS,
		this->rPurposeCardDistribution,
		this->rPurposeCardKiosk,
		this->rPurposeDSiWare,
		this->rPurposeOther,
	};
	this->checkRadioButton( rbuts, "LabelUsage" );

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

	if( this->tboxPrivateSaveDataPurpose->Enabled )
	{
		this->checkTextForm( this->tboxPrivateSaveDataPurpose->Text, "PrivateSaveDataPurposeTitle", "PrivateSaveDataPurposeMsg" );
	}

	// ひととおりエラー登録をした後で
	// 書類上のエラー(SRLバイナリには影響しない)が存在するかチェック
	return this->isValidOnlyDeliverable();
}

// end of file