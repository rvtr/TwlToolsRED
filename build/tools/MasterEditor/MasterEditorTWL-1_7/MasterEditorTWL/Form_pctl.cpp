// ---------------------------------------------------------------------
// リージョンとレーティング設定
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

// =====================================================================
// リージョン
// =====================================================================

// ---------------------------------------------------------------------
// GUIのコンボボックスからROMヘッダのリージョンコードを決定する
// ---------------------------------------------------------------------

void Form1::setRegionSrlPropaties(void)
{
	this->hSrl->IsRegionJapan     = false;
	this->hSrl->IsRegionAmerica   = false;
	this->hSrl->IsRegionEurope    = false;
	this->hSrl->IsRegionAustralia = false;

	// 中国版と韓国版はリージョン変更させない
	if( this->combRegion->DropDownStyle == System::Windows::Forms::ComboBoxStyle::DropDown )	// コンボボックスが中韓設定用になっているとき
	{
		return;
	}

	// ALLリージョンのとき中韓ビットも立っているので中韓ビットを落としておく必要がある
	this->hSrl->IsRegionChina = false;
	this->hSrl->IsRegionKorea = false;

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

		// 中韓は設定不可

#if defined(METWL_VER_APPTYPE_SYSTEM) || defined(METWL_VER_APPTYPE_SECURE) || defined(METWL_VER_APPTYPE_LAUNCHER)
		case 7:
			this->hSrl->IsRegionJapan     = true;
			this->hSrl->IsRegionAmerica   = true;
			this->hSrl->IsRegionEurope    = true;
			this->hSrl->IsRegionAustralia = true;
			this->hSrl->IsRegionKorea     = true;
			this->hSrl->IsRegionChina     = true;
		break;
#endif //defined(METWL_VER_APPTYPE_SYSTEM) || defined(METWL_VER_APPTYPE_SECURE) || defined(METWL_VER_APPTYPE_LAUNCHER)
		default:
		break;
	}
}

// ---------------------------------------------------------------------
// ROMヘッダのリージョンコードからGUIのコンボボックスの値を決定する
// ---------------------------------------------------------------------

void Form1::setRegionForms(void)
{
	System::Boolean isJapan   = this->hSrl->IsRegionJapan;			// リージョン
	System::Boolean isAmerica = this->hSrl->IsRegionAmerica;
	System::Boolean isEurope  = this->hSrl->IsRegionEurope;
	System::Boolean isAustralia = this->hSrl->IsRegionAustralia;
	System::Boolean isKorea   = this->hSrl->IsRegionKorea;
	System::Boolean isChina   = this->hSrl->IsRegionChina;

	// 中韓のときコンボボックスを編集不可にする
	if( !isJapan && !isAmerica && !isEurope && !isAustralia && !isKorea && isChina )		// 中国
	{
		this->combRegion->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDown;	// 自由にテキストを入力できる
		this->combRegion->SelectedIndex = -1;	// 設定の順序に注意: テキストの設定よりも前に入れておかないとテキスト入力が反映されないことがある
		this->combRegion->Text = this->isJapanese()?METWL_STRING_CHINA_REGION_J:METWL_STRING_CHINA_REGION_E;
		this->combRegion->Enabled = false;	// 編集不可
	}
	else if( !isJapan && !isAmerica && !isEurope && !isAustralia && isKorea && !isChina )	// 韓国
	{
		this->combRegion->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDown;
		this->combRegion->SelectedIndex = -1;
		this->combRegion->Text = this->isJapanese()?METWL_STRING_KOREA_REGION_J:METWL_STRING_KOREA_REGION_E;
		this->combRegion->Enabled = false;
	}
	else	// WorldWide
	{
		// 選択可能にする
		this->combRegion->Enabled = true;
		this->combRegion->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;	// リストの中から選択しかできない

		// コンボボックスのデフォルトの選択値を決定
		System::Int32  index = -1;
		if( isJapan && !isAmerica && !isEurope && !isAustralia && !isKorea && !isChina )
			index = 0;
		else if( !isJapan && isAmerica && !isEurope && !isAustralia && !isKorea && !isChina )
			index = 1;
		else if( !isJapan && !isAmerica && isEurope && !isAustralia && !isKorea && !isChina )
			index = 2;
		else if( !isJapan && !isAmerica && !isEurope && isAustralia && !isKorea && !isChina )
			index = 3;
		else if( !isJapan && !isAmerica && isEurope && isAustralia && !isKorea && !isChina )
			index = 4;
		else if( !isJapan && isAmerica && !isEurope && isAustralia && !isKorea && !isChina )
			index = 5;
		else if( !isJapan && isAmerica && isEurope && isAustralia && !isKorea && !isChina )
			index = 6;
#if defined(METWL_VER_APPTYPE_SYSTEM) || defined(METWL_VER_APPTYPE_SECURE) || defined(METWL_VER_APPTYPE_LAUNCHER)
		if( isJapan && isAmerica && isEurope && isAustralia && isKorea && isChina )
			index = 7;
#endif
		this->combRegion->SelectedIndex = index;
	}
	this->maskRatingForms();		// ペアレンタルコントロール用フォームの表示/非表示切り替え
}

// ---------------------------------------------------------------------
// リージョンのフォームに問題がないかをチェックする
// ---------------------------------------------------------------------

void Form1::checkRegionForms(void)
{
	// 中韓のときにはリージョン選択できないのでコンボボックスが選択されているかのチェックは必要なし
	if( this->combRegion->DropDownStyle == System::Windows::Forms::ComboBoxStyle::DropDownList )
	{
		this->checkComboBoxIndex( this->combRegion, "LabelRegion", true );
	}
}


// =====================================================================
// レーティング
// =====================================================================

// ---------------------------------------------------------------------
// ROMヘッダのレーティング情報からGUIのコンボボックスを設定する
// ---------------------------------------------------------------------

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
	this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_GRB]   = this->combGRB->SelectedIndex;

	// レーティング表示不要かどうかを設定
	this->hSrl->IsUnnecessaryRating = this->cboxIsUnnecessaryRating->Checked;
} //setRatingSrlProperties()

// ---------------------------------------------------------------------
// GUIのコンボボックスの選択をROMヘッダに反映させる
// ---------------------------------------------------------------------

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
	this->combGRB->SelectedIndex  = this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_GRB ];
} //setRatingForms()

// ---------------------------------------------------------------------
// GUIで表示するレーティング団体をリージョンによって変える
// ---------------------------------------------------------------------

void Form1::maskRatingForms(void)
{
	this->enableRating( this->combCERO, this->labCERO, nullptr );
	this->enableRating( this->combESRB, this->labESRB, nullptr );
	this->enableRating( this->combUSK,  this->labUSK,  nullptr );
	this->enableRating( this->combPEGI, this->labPEGI, nullptr );
	this->enableRating( this->combPEGI_PRT,  this->labPEGI_PRT,  nullptr );
	this->enableRating( this->combPEGI_BBFC, this->labPEGI_BBFC, nullptr );
	this->enableRating( this->combOFLC, this->labOFLC, nullptr );
	this->enableRating( this->combGRB, this->labGRB, nullptr );

	// 中韓リージョンは設定不可なので特別処理
	this->tboxWarningChinaRating->Visible = false;
	if( (this->combRegion->DropDownStyle == System::Windows::Forms::ComboBoxStyle::DropDown) // コンボボックスが中韓リージョン用になっているとき
		&& this->hSrl->IsRegionChina )	// SRLが読み込まれていることは保証される
	{
		this->tboxWarningChinaRating->Visible = true;	// 中国用の特別メッセージを表示する

		// 中国にレーティング団体なし
		this->disableRating( this->combCERO, this->labCERO, nullptr );
		this->disableRating( this->combESRB,  this->labESRB, nullptr );
		this->disableRating( this->combUSK,   this->labUSK,  nullptr );
		this->disableRating( this->combPEGI,  this->labPEGI, nullptr );
		this->disableRating( this->combPEGI_PRT,  this->labPEGI_PRT,  nullptr );
		this->disableRating( this->combPEGI_BBFC, this->labPEGI_BBFC, nullptr );
		this->disableRating( this->combOFLC,  this->labOFLC, nullptr );
		this->disableRating( this->combGRB, this->labGRB, nullptr );
	}
	else if( (this->combRegion->DropDownStyle == System::Windows::Forms::ComboBoxStyle::DropDown) 
		&& this->hSrl->IsRegionKorea )
	{
		// 韓国はGRBのみ
		this->disableRating( this->combCERO, this->labCERO, nullptr );
		this->disableRating( this->combESRB,  this->labESRB, nullptr );
		this->disableRating( this->combUSK,   this->labUSK,  nullptr );
		this->disableRating( this->combPEGI,  this->labPEGI, nullptr );
		this->disableRating( this->combPEGI_PRT,  this->labPEGI_PRT,  nullptr );
		this->disableRating( this->combPEGI_BBFC, this->labPEGI_BBFC, nullptr );
		this->disableRating( this->combOFLC,  this->labOFLC, nullptr );
		this->enableRating( this->combGRB, this->labGRB, nullptr );
	}
	else
	{
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
				this->disableRating( this->combGRB, this->labGRB, nullptr );
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
				this->disableRating( this->combGRB, this->labGRB, nullptr );
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
				this->disableRating( this->combGRB, this->labGRB, nullptr );
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
				this->disableRating( this->combGRB, this->labGRB, nullptr );
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
				this->disableRating( this->combGRB, this->labGRB, nullptr );
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
				this->disableRating( this->combGRB, this->labGRB, nullptr );
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
				this->disableRating( this->combGRB, this->labGRB, nullptr );
			break;

			// 全リージョンのときは何もdisableにしない
			default:
			break;
		}// switch
	}
} //maskRatingForms()

// ---------------------------------------------------------------------
// GUIのレーティング設定に問題がないかチェックする
// ---------------------------------------------------------------------

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

// ---------------------------------------------------------------------
// 「レーティング表示不要」が選択されたかどうかでコンボボックスの内容を変える
// ROM読み込み時および「レーティング表示不要」チェックボックスに変化があったときに呼び出される
// ---------------------------------------------------------------------

void Form1::changeUnnecessaryRatingForms( System::Boolean bInitial )
{
	if( this->cboxIsUnnecessaryRating->Checked )
	{
		this->unnecessaryRating( this->combCERO );		// 表示/非表示の選択は別のところでするのでここでは全団体を不要にしてもかまわない
		this->unnecessaryRating( this->combESRB );
		this->unnecessaryRating( this->combUSK );
		this->unnecessaryRating( this->combPEGI );
		this->unnecessaryRating( this->combPEGI_PRT );
		this->unnecessaryRating( this->combPEGI_BBFC );
		this->unnecessaryRating( this->combOFLC );
		this->unnecessaryRating( this->combGRB );
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
		this->necessaryRating( this->combGRB, bInitial );
	}
}

// ---------------------------------------------------------------------
// レーティングのコンボボックスをクリアする
// ---------------------------------------------------------------------

void Form1::clearRating( System::Windows::Forms::ComboBox ^comb )
{
	comb->SelectedIndex = -1;	// 空白にする
}

// ---------------------------------------------------------------------
// レーティングのコンボボックスを編集可能な状態にする
// ---------------------------------------------------------------------

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
	// 提出確認書にレーティングを記述する際に「団体がリージョンに含まれているかどうか」の情報が必要となる
	// comb->Enableは編集可能かどうかを表すので
	// リージョンに含まれているかどうかの情報として使えるように思えるが
	//「レーティング表示不要」でcomb->Enabledをfalseにしなければならないので
	// リージョンに含まれているのかをcomb->Enabledを使って判定できない
	// (comb->Visibleが使えそうだが残念ながら別のタブに移動すると勝手にfalseになってしまう)
	// そのため無関係な設定を変えることでリージョンに含まれていることを表現する
	comb->FlatStyle = System::Windows::Forms::FlatStyle::Standard;
}

// ---------------------------------------------------------------------
// レーティングのコンボボックスを編集不可状態にする
// ---------------------------------------------------------------------

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
	// リージョンに含まれないことを表現 (表示が消えるのでこの値が変わっても問題ない)
	comb->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
}

// ---------------------------------------------------------------------
// レーティングのコンボボックスに「レーティング表示不要」と表示し
// 編集不可状態にする
// ---------------------------------------------------------------------

void Form1::unnecessaryRating( System::Windows::Forms::ComboBox ^comb )
{
	comb->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDown;	// 自由なテキストを表示可能にする
	comb->SelectedIndex = -1;	// 何も選択されていないとみなす
	System::String ^msg = this->isJapanese()?METWL_STRING_UNNECESSARY_RATING_J:METWL_STRING_UNNECESSARY_RATING_E;
	comb->Text = msg;
	comb->Enabled = false;		// 編集不可能にする
}

// ---------------------------------------------------------------------
// レーティングのコンボボックスの「レーティング表示不要」の表示をクリアし
// 編集可能状態にする
// ---------------------------------------------------------------------

void Form1::necessaryRating( System::Windows::Forms::ComboBox ^comb, System::Boolean bInitial )
{
	comb->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;	// リストの中からしか選択できないようにする
	if( !bInitial )
	{
		comb->SelectedIndex = -1;	// ROM読み込み時の場合にはコンボボックスを初期化しない(せっかく読み込んだ情報を消してしまうため)
									// GUI上で変更したときには空白に初期化する(再度設定させる)
	}
	comb->Enabled = true;	// 編集可能にする
}

// end of file