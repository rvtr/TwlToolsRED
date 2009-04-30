// ---------------------------------------------------------------------
// リージョンとペアレンタルコントロール設定
// ---------------------------------------------------------------------

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

// ---------------------------------------------------------------------
// リージョン設定は複雑なので別に切り出す
// ---------------------------------------------------------------------

// フォーム入力をSRLに反映させる
void Form1::setRegionSrlPropaties(void)
{
	this->hSrl->IsRegionJapan     = false;
	this->hSrl->IsRegionAmerica   = false;
	this->hSrl->IsRegionEurope    = false;
	this->hSrl->IsRegionAustralia = false;
	switch( this->combRegion->SelectedIndex )
	{
		case 0:
			this->hSrl->IsRegionJapan = true;
		break;

		case 1:
			this->hSrl->IsRegionAmerica = true;
		break;

		case 2:
			this->hSrl->IsRegionEurope = true;
		break;

		case 3:
			this->hSrl->IsRegionAustralia = true;
		break;

		case 4:
			this->hSrl->IsRegionEurope    = true;
			this->hSrl->IsRegionAustralia = true;
		break;

		case 5:
			this->hSrl->IsRegionAmerica   = true;
			this->hSrl->IsRegionAustralia = true;
		break;

		case 6:
			this->hSrl->IsRegionAmerica   = true;
			this->hSrl->IsRegionEurope    = true;
			this->hSrl->IsRegionAustralia = true;
		break;

#if defined(METWL_VER_APPTYPE_SYSTEM) || defined(METWL_VER_APPTYPE_SECURE) || defined(METWL_VER_APPTYPE_LAUNCHER)
		case 7:
			this->hSrl->IsRegionJapan     = true;
			this->hSrl->IsRegionAmerica   = true;
			this->hSrl->IsRegionEurope    = true;
			this->hSrl->IsRegionAustralia = true;
		break;
#endif //defined(METWL_VER_APPTYPE_SYSTEM) || defined(METWL_VER_APPTYPE_SECURE) || defined(METWL_VER_APPTYPE_LAUNCHER)
		default:
		break;
	}
}

// SRL情報をフォームに反映させる
void Form1::setRegionForms(void)
{
	System::Boolean isJapan   = this->hSrl->IsRegionJapan;			// リージョン
	System::Boolean isAmerica = this->hSrl->IsRegionAmerica;
	System::Boolean isEurope  = this->hSrl->IsRegionEurope;
	System::Boolean isAustralia = this->hSrl->IsRegionAustralia;
	System::Int32  index;
	if( isJapan && !isAmerica && !isEurope && !isAustralia )
		index = 0;
	else if( !isJapan && isAmerica && !isEurope && !isAustralia )
		index = 1;
	else if( !isJapan && !isAmerica && isEurope && !isAustralia )
		index = 2;
	else if( !isJapan && !isAmerica && !isEurope && isAustralia )
		index = 3;
	else if( !isJapan && !isAmerica && isEurope && isAustralia )
		index = 4;
	else if( !isJapan && isAmerica && !isEurope && isAustralia )
		index = 5;
	else if( !isJapan && isAmerica && isEurope && isAustralia )
		index = 6;
	else
		index = -1;	// 不正
#if defined(METWL_VER_APPTYPE_SYSTEM) || defined(METWL_VER_APPTYPE_SECURE) || defined(METWL_VER_APPTYPE_LAUNCHER)
	if( isJapan && isAmerica && isEurope && isAustralia )
	index = 7;
#endif
	this->combRegion->SelectedIndex = index;
	this->maskRatingForms();		// ペアレンタルコントロール用フォームの表示/非表示切り替え
}

// ---------------------------------------------------------------------
// ペアレンタルコントロール設定は複雑なので別に切り出す
// ---------------------------------------------------------------------

// フォーム入力をSRLに反映させる
void Form1::setRatingSrlProperties(void)
{
	// 各団体のフォーム入力を反映
	this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_CERO ] = this->combCERO->SelectedIndex;
	this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_ESRB ] = this->combESRB->SelectedIndex;
	this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_USK ]  = this->combUSK->SelectedIndex;
	this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_PEGI_GEN ]  = this->combPEGI->SelectedIndex;
	this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_PEGI_PRT ]  = this->combPEGI_PRT->SelectedIndex;
	this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_PEGI_BBFC ] = this->combPEGI_BBFC->SelectedIndex;
	this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_AGCB ] = this->combOFLC->SelectedIndex;

	// レーティング表示不要かどうかを設定
	this->hSrl->IsUnnecessaryRating = this->cboxIsUnnecessaryRating->Checked;
} //setRatingSrlProperties()

// SRL内のペアレンタルコントロール情報を抜き出してフォームに反映させる
void Form1::setRatingForms(void)
{
	// レーティング表示不要かどうかを判断
	this->cboxIsUnnecessaryRating->Checked = this->hSrl->IsUnnecessaryRating;
	this->changeUnnecessaryRatingForms( true );	// 不要かどうかに応じてフォームを設定

	// 各団体のコンボボックスのインデックスを設定
	this->combCERO->SelectedIndex = this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_CERO ];
	this->combESRB->SelectedIndex = this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_ESRB ];
	this->combUSK->SelectedIndex  = this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_USK ];
	this->combPEGI->SelectedIndex = this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_PEGI_GEN ];
	this->combPEGI_PRT->SelectedIndex  = this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_PEGI_PRT ];
	this->combPEGI_BBFC->SelectedIndex = this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_PEGI_BBFC ];
	this->combOFLC->SelectedIndex = this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_AGCB ];
} //setRatingForms()

// リージョン情報からペアレンタルコントロールの編集可能団体をマスクする
void Form1::maskRatingForms(void)
{
	this->enableRating( this->combCERO, this->labCERO, nullptr );
	this->enableRating( this->combESRB, this->labESRB, nullptr );
	this->enableRating( this->combUSK,  this->labUSK,  nullptr );
	this->enableRating( this->combPEGI, this->labPEGI, nullptr );
	this->enableRating( this->combPEGI_PRT,  this->labPEGI_PRT,  nullptr );
	this->enableRating( this->combPEGI_BBFC, this->labPEGI_BBFC, nullptr );
	this->enableRating( this->combOFLC, this->labOFLC, nullptr );
	switch( this->combRegion->SelectedIndex )
	{
		case 0:
			// 日本
			this->enableRating( this->combCERO, this->labCERO, nullptr );
			this->disableRating( this->combESRB, this->labESRB, nullptr );
			this->disableRating( this->combUSK,  this->labUSK,  nullptr );
			this->disableRating( this->combPEGI, this->labPEGI, nullptr );
			this->disableRating( this->combPEGI_PRT,  this->labPEGI_PRT,  nullptr );
			this->disableRating( this->combPEGI_BBFC, this->labPEGI_BBFC, nullptr );
			this->disableRating( this->combOFLC, this->labOFLC, nullptr );
		break;

		case 1:
			// 北米
			this->disableRating( this->combCERO, this->labCERO, nullptr );
			this->enableRating( this->combESRB,  this->labESRB, nullptr );
			this->disableRating( this->combUSK,  this->labUSK,  nullptr );
			this->disableRating( this->combPEGI, this->labPEGI, nullptr );
			this->disableRating( this->combPEGI_PRT,  this->labPEGI_PRT,  nullptr );
			this->disableRating( this->combPEGI_BBFC, this->labPEGI_BBFC, nullptr );
			this->disableRating( this->combOFLC, this->labOFLC, nullptr );
		break;

		case 2:
			// 欧州
			this->disableRating( this->combCERO, this->labCERO, nullptr );
			this->disableRating( this->combESRB, this->labESRB, nullptr );
			this->enableRating( this->combUSK,   this->labUSK,  nullptr );
			this->enableRating( this->combPEGI,  this->labPEGI, nullptr );
			this->enableRating( this->combPEGI_PRT,  this->labPEGI_PRT,  nullptr );
			this->enableRating( this->combPEGI_BBFC, this->labPEGI_BBFC, nullptr );
			this->disableRating( this->combOFLC, this->labOFLC, nullptr );
		break;

		case 3:
			// 豪州
			this->disableRating( this->combCERO, this->labCERO, nullptr );
			this->disableRating( this->combESRB, this->labESRB, nullptr );
			this->disableRating( this->combUSK,  this->labUSK,  nullptr );
			this->disableRating( this->combPEGI, this->labPEGI, nullptr );
			this->disableRating( this->combPEGI_PRT,  this->labPEGI_PRT,  nullptr );
			this->disableRating( this->combPEGI_BBFC, this->labPEGI_BBFC, nullptr );
			this->enableRating( this->combOFLC,  this->labOFLC, nullptr );
		break;

		case 4:
			// 欧州と豪州
			this->disableRating( this->combCERO, this->labCERO, nullptr );
			this->disableRating( this->combESRB, this->labESRB, nullptr );
			this->enableRating( this->combUSK,   this->labUSK,  nullptr );
			this->enableRating( this->combPEGI,  this->labPEGI, nullptr );
			this->enableRating( this->combPEGI_PRT,  this->labPEGI_PRT,  nullptr );
			this->enableRating( this->combPEGI_BBFC, this->labPEGI_BBFC, nullptr );
			this->enableRating( this->combOFLC,  this->labOFLC, nullptr );
		break;

		case 5:
			// 北米と豪州
			this->disableRating( this->combCERO, this->labCERO, nullptr );
			this->enableRating( this->combESRB,  this->labESRB, nullptr );
			this->disableRating( this->combUSK,  this->labUSK,  nullptr );
			this->disableRating( this->combPEGI, this->labPEGI, nullptr );
			this->disableRating( this->combPEGI_PRT,  this->labPEGI_PRT,  nullptr );
			this->disableRating( this->combPEGI_BBFC, this->labPEGI_BBFC, nullptr );
			this->enableRating( this->combOFLC,  this->labOFLC, nullptr );
		break;

		case 6:
			// 北米と欧州と豪州
			this->disableRating( this->combCERO, this->labCERO, nullptr );
			this->enableRating( this->combESRB,  this->labESRB, nullptr );
			this->enableRating( this->combUSK,   this->labUSK,  nullptr );
			this->enableRating( this->combPEGI,  this->labPEGI, nullptr );
			this->enableRating( this->combPEGI_PRT,  this->labPEGI_PRT,  nullptr );
			this->enableRating( this->combPEGI_BBFC, this->labPEGI_BBFC, nullptr );
			this->enableRating( this->combOFLC,  this->labOFLC, nullptr );
		break;

		// 全リージョンのときは何もdisableにしない
		default:
		break;
	}
} //maskRatingForms()

// 全団体を「レーティング表示不要」の設定/解除をする
void Form1::changeUnnecessaryRatingForms( System::Boolean bInitial )
{
	if( this->cboxIsUnnecessaryRating->Checked )
	{
		this->unnecessaryRating( this->combCERO );		// マスクは別のところでするのでここでは全団体を不要にしてもかまわない
		this->unnecessaryRating( this->combESRB );
		this->unnecessaryRating( this->combUSK );
		this->unnecessaryRating( this->combPEGI );
		this->unnecessaryRating( this->combPEGI_PRT );
		this->unnecessaryRating( this->combPEGI_BBFC );
		this->unnecessaryRating( this->combOFLC );
	}
	else
	{
		this->necessaryRating( this->combCERO, bInitial );
		this->necessaryRating( this->combESRB, bInitial );
		this->necessaryRating( this->combUSK, bInitial );
		this->necessaryRating( this->combPEGI, bInitial );
		this->necessaryRating( this->combPEGI_PRT, bInitial );
		this->necessaryRating( this->combPEGI_BBFC, bInitial );
		this->necessaryRating( this->combOFLC, bInitial );
	}
}

// ペアレンタルコントロール関連のフォーム入力が正しいか書き込み前チェック
void Form1::checkRatingForms( System::Boolean inRegion, System::Windows::Forms::ComboBox ^comb, System::String ^ogn )
{
	// リージョンに含まれていないとき: 0クリアが保証されるのでチェック必要なし
	if( !inRegion )
		return;

	// 設定されていないときエラー
	if( (comb->SelectedIndex < 0) || (comb->SelectedIndex >= comb->Items->Count)  )
	{
		this->hErrorList->Add( this->makeErrorMsg(true, "LabelRating", "RatingInput", ogn) );
	}

	// 審査中のとき警告
	if( comb->SelectedIndex == (comb->Items->Count - 1) )
	{
		this->hWarnList->Add( this->makeErrorMsg(true, "LabelRating", "RatingPendingSelect", ogn) );
	}
} //checkRatingForms()

// ペアレンタルコントロール情報をクリアする
void Form1::clearRating( System::Windows::Forms::ComboBox ^comb )
{
	comb->SelectedIndex = -1;	// 空白にする
}


// ペアレンタルコントロール情報を編集できるようにする
void Form1::enableRating( System::Windows::Forms::ComboBox ^comb, 
							System::Windows::Forms::Label    ^lab1, 
							System::Windows::Forms::Label    ^lab2 )
{
	comb->Enabled   = true;
	comb->Visible   = true;
	lab1->Visible   = true;
	if( lab2 != nullptr )
	{
		lab2->Visible   = true;
	}
	// 本来は不必要な設定
	//「レーティング表示不要」でEnableをfalseにするのでリージョンに含まれているのかどうかを知る術がない
	// (Visibleは親タブが変わると勝手にFalseになってしまう)
	// そのため無関係な設定を変えることでリージョンに含まれていることを表現する
	comb->FlatStyle = System::Windows::Forms::FlatStyle::Standard;
}

// ペアレンタルコントロール情報を編集できなくする
void Form1::disableRating( System::Windows::Forms::ComboBox ^comb, 
							 System::Windows::Forms::Label    ^lab1, 
							 System::Windows::Forms::Label    ^lab2 )
{
	this->clearRating( comb );
	comb->Enabled   = false;
	comb->Visible   = false;
	lab1->Visible   = false;
	if( lab2 != nullptr )
	{
		lab2->Visible   = false;
	}
	comb->FlatStyle = System::Windows::Forms::FlatStyle::Popup;		// Disableを表現
}

// 「レーティング表示不要」と表示して編集できなくする
void Form1::unnecessaryRating( System::Windows::Forms::ComboBox ^comb )
{
	comb->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDown;	// 一時的にテキスト入力可能にする
	comb->SelectedIndex = -1;	// 何も選択されていないとみなす
	System::String ^msg;
	if( this->isJapanese() )
	{
		msg = gcnew System::String( "レーティング表示不要(全年齢)" );
	}
	else
	{
		msg = gcnew System::String( "Unnecessary Rating(All ages)" );
	}
	comb->Text = msg;
	comb->Enabled = false;		// 編集不可能にする
}

// 「レーティング表示不要」表示を消して通常の設定に戻す
void Form1::necessaryRating( System::Windows::Forms::ComboBox ^comb, System::Boolean bInitial )
{
	comb->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
	if( !bInitial )
	{
		comb->SelectedIndex = -1;	// 読み込みの場合にはコンボボックスを初期化しない(せっかく読み込んだ情報を消してしまうため)
	}
	comb->Enabled = true;	// 編集可能にする
}

// end of file