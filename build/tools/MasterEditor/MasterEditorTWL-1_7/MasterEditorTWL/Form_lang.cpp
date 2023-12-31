// ----------------------------------------------
// 日英両対応
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
// テキストボックスの文字列制限を変える
// (日本語版と英語版で文字列制限を変えたい)
// ----------------------------------------------
void Form1::changeMaxLength( System::Windows::Forms::TextBox ^tbox, System::Int32 maxlen )
{
	if( tbox->Text->Length > maxlen )
		tbox->Text = "";

	tbox->MaxLength = maxlen;
}

// ----------------------------------------------
// フォームのEnableやVisibleを変える
// ----------------------------------------------
void Form1::changeFormInput( System::String ^langname )
{
	// 日本語のみの入力項目
	if( langname->StartsWith("ja") )
	{
		// ふりがなは日本のみ
		this->tboxFurigana1->Enabled = true;
		this->tboxNTSC1->Enabled     = true;
		this->tboxFurigana2->Enabled = true;
		this->tboxNTSC2->Enabled     = true;
		this->labFurigana1->Visible  = true;
		this->labNTSC1Pre->Visible   = true;
		this->labNTSC1Sur->Visible   = true;
		this->labFurigana2->Visible  = true;
		this->labNTSC2Pre->Visible   = true;
		this->labNTSC2Sur->Visible   = true;
		this->labArbit2->Visible     = true;
		this->labArbit4->Visible     = true;

		// 日本ではInternet提出を認めないので表示を消す
		if( this->rSubmitInternet->Checked )
		{
			this->rSubmitInternet->Checked = false;		// Internetが選択されているときに表示を消すとどれも選択されていない状況になるので
			this->rSubmitPost->Checked     = true;		// 必ずどれか1つが選択されているようにしておく
		}
		this->rSubmitInternet->Enabled = false;
		this->rSubmitInternet->Visible = false;
	}
	else
	{
		this->tboxFurigana1->Enabled = false;
		this->tboxNTSC1->Enabled     = false;
		this->tboxFurigana2->Enabled = false;
		this->tboxNTSC2->Enabled     = false;
		this->labFurigana1->Visible  = false;
		this->labNTSC1Pre->Visible   = false;
		this->labNTSC1Sur->Visible   = false;
		this->labFurigana2->Visible  = false;
		this->labNTSC2Pre->Visible   = false;
		this->labNTSC2Sur->Visible   = false;
		this->labArbit2->Visible     = false;
		this->labArbit4->Visible     = false;

		this->rSubmitInternet->Enabled = true;
		this->rSubmitInternet->Visible = true;
	}
}

// ----------------------------------------------
// 日本語版への切り替え
// ----------------------------------------------
void Form1::changeJapanese(void)
{
	this->changeLanguage( "ja" );

}

// ----------------------------------------------
// 英語版への切り替え
// ----------------------------------------------
void  Form1::changeEnglish(void)
{
	this->changeLanguage( "en" );
}

// ----------------------------------------------
// 言語リソース切り替え
// ----------------------------------------------
void MasterEditorTWL::Form1::changeLanguage( System::String ^langname )
{
	//int  index;

	System::Threading::Thread::CurrentThread->CurrentUICulture = gcnew System::Globalization::CultureInfo(langname,true);
	System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));

	resources->ApplyResources(this->tboxFile, L"tboxFile");
	resources->ApplyResources(this->gboxSrl, L"gboxSrl");
	resources->ApplyResources(this->tboxRemasterVer, L"tboxRemasterVer");
	resources->ApplyResources(this->tboxRomSize, L"tboxRomSize");
	resources->ApplyResources(this->tboxPlatform, L"tboxPlatform");
	resources->ApplyResources(this->labPlatform, L"labPlatform");
	resources->ApplyResources(this->tboxRomLatency, L"tboxRomLatency");
	resources->ApplyResources(this->labRomSize, L"labRomSize");
	resources->ApplyResources(this->labRomType, L"labRomType");
	resources->ApplyResources(this->tboxMakerCode, L"tboxMakerCode");
	resources->ApplyResources(this->cboxRemasterVerE, L"cboxRemasterVerE");
	resources->ApplyResources(this->labMakerCode, L"labMakerCode");
	resources->ApplyResources(this->labGameCode, L"labGameCode");
	resources->ApplyResources(this->tboxGameCode, L"tboxGameCode");
	resources->ApplyResources(this->labTitleName, L"labTitleName");
	resources->ApplyResources(this->labRemasterVer, L"labRemasterVer");
	resources->ApplyResources(this->tboxTitleName, L"tboxTitleName");
	resources->ApplyResources(this->labBackup, L"labBackup");

	int indexBackup = this->combBackup->SelectedIndex;
	this->combBackup->Items->Clear();
	this->combBackup->Items->AddRange(gcnew cli::array< System::Object^  >(10) {resources->GetString(L"combBackup.Items"), resources->GetString(L"combBackup.Items1"), 
		resources->GetString(L"combBackup.Items2"), resources->GetString(L"combBackup.Items3"), resources->GetString(L"combBackup.Items4"), 
		resources->GetString(L"combBackup.Items5"), resources->GetString(L"combBackup.Items6"), resources->GetString(L"combBackup.Items7"), 
		resources->GetString(L"combBackup.Items8"), resources->GetString(L"combBackup.Items9")});
	resources->ApplyResources(this->combBackup, L"combBackup");
	this->combBackup->SelectedIndex = indexBackup;

	resources->ApplyResources(this->tboxBackupOther, L"tboxBackupOther");
	resources->ApplyResources(this->gboxCRC, L"gboxCRC");
	resources->ApplyResources(this->labRomCRC, L"labRomCRC");
	resources->ApplyResources(this->labHeaderCRC, L"labHeaderCRC");
	resources->ApplyResources(this->tboxHeaderCRC, L"tboxHeaderCRC");
	resources->ApplyResources(this->tboxWholeCRC, L"tboxWholeCRC");
	resources->ApplyResources(this->labCaption, L"labCaption");
	resources->ApplyResources(this->tboxCaption, L"tboxCaption");
	resources->ApplyResources(this->labGRB, L"labGRB");
	resources->ApplyResources(this->labOFLC, L"labOFLC");
	resources->ApplyResources(this->labPEGI_BBFC, L"labPEGI_BBFC");
	resources->ApplyResources(this->labPEGI_PRT, L"labPEGI_PRT");
	resources->ApplyResources(this->labPEGI, L"labPEGI");
	resources->ApplyResources(this->labUSK, L"labUSK");
	resources->ApplyResources(this->labESRB, L"labESRB");
	resources->ApplyResources(this->labCERO, L"labCERO");

	int indexGRB = this->combGRB->SelectedIndex;
	this->combGRB->Items->Clear();
	this->combGRB->Items->AddRange(gcnew cli::array< System::Object^  >(5) {resources->GetString(L"combGRB.Items"), resources->GetString(L"combGRB.Items1"), 
		resources->GetString(L"combGRB.Items2"), resources->GetString(L"combGRB.Items3"), resources->GetString(L"combGRB.Items4")});

	resources->ApplyResources(this->combOFLC, L"combOFLC");
	int indexOFLC = this->combOFLC->SelectedIndex;
	this->combOFLC->Items->Clear();
	this->combOFLC->Items->AddRange(gcnew cli::array< System::Object^  >(5) {resources->GetString(L"combOFLC.Items"), resources->GetString(L"combOFLC.Items1"), 
		resources->GetString(L"combOFLC.Items2"), resources->GetString(L"combOFLC.Items3"), resources->GetString(L"combOFLC.Items4")});

	resources->ApplyResources(this->combPEGI_BBFC, L"combPEGI_BBFC");
	int indexPEGI_BBFC = this->combPEGI_BBFC->SelectedIndex;
	this->combPEGI_BBFC->Items->Clear();
	this->combPEGI_BBFC->Items->AddRange(gcnew cli::array< System::Object^  >(10) {resources->GetString(L"combPEGI_BBFC.Items"), 
		resources->GetString(L"combPEGI_BBFC.Items1"), resources->GetString(L"combPEGI_BBFC.Items2"), resources->GetString(L"combPEGI_BBFC.Items3"), 
		resources->GetString(L"combPEGI_BBFC.Items4"), resources->GetString(L"combPEGI_BBFC.Items5"), resources->GetString(L"combPEGI_BBFC.Items6"), 
		resources->GetString(L"combPEGI_BBFC.Items7"), resources->GetString(L"combPEGI_BBFC.Items8"), resources->GetString(L"combPEGI_BBFC.Items9")});

	resources->ApplyResources(this->combPEGI_PRT, L"combPEGI_PRT");
	int indexPEGI_PRT = this->combPEGI_PRT->SelectedIndex;
	this->combPEGI_PRT->Items->Clear();
	this->combPEGI_PRT->Items->AddRange(gcnew cli::array< System::Object^  >(7) {resources->GetString(L"combPEGI_PRT.Items"), 
		resources->GetString(L"combPEGI_PRT.Items1"), resources->GetString(L"combPEGI_PRT.Items2"), resources->GetString(L"combPEGI_PRT.Items3"), 
		resources->GetString(L"combPEGI_PRT.Items4"), resources->GetString(L"combPEGI_PRT.Items5"), resources->GetString(L"combPEGI_PRT.Items6")});

	resources->ApplyResources(this->combPEGI, L"combPEGI");
	int indexPEGI = this->combPEGI->SelectedIndex;
	this->combPEGI->Items->Clear();
	this->combPEGI->Items->AddRange(gcnew cli::array< System::Object^  >(7) {resources->GetString(L"combPEGI.Items"), resources->GetString(L"combPEGI.Items1"), 
		resources->GetString(L"combPEGI.Items2"), resources->GetString(L"combPEGI.Items3"), resources->GetString(L"combPEGI.Items4"), 
		resources->GetString(L"combPEGI.Items5"), resources->GetString(L"combPEGI.Items6")});

	resources->ApplyResources(this->combUSK, L"combUSK");
	int indexUSK = this->combUSK->SelectedIndex;
	this->combUSK->Items->Clear();
	this->combUSK->Items->AddRange(gcnew cli::array< System::Object^  >(6) {resources->GetString(L"combUSK.Items"), resources->GetString(L"combUSK.Items1"), 
		resources->GetString(L"combUSK.Items2"), resources->GetString(L"combUSK.Items3"), resources->GetString(L"combUSK.Items4"), resources->GetString(L"combUSK.Items5")});

	resources->ApplyResources(this->combESRB, L"combESRB");
	int indexESRB = this->combESRB->SelectedIndex;
	this->combESRB->Items->Clear();
	this->combESRB->Items->AddRange(gcnew cli::array< System::Object^  >(7) {resources->GetString(L"combESRB.Items"), resources->GetString(L"combESRB.Items1"), 
		resources->GetString(L"combESRB.Items2"), resources->GetString(L"combESRB.Items3"), resources->GetString(L"combESRB.Items4"), 
		resources->GetString(L"combESRB.Items5"), resources->GetString(L"combESRB.Items6")});

	resources->ApplyResources(this->combCERO, L"combCERO");
	int indexCERO = this->combCERO->SelectedIndex;
	this->combCERO->Items->Clear();
	this->combCERO->Items->AddRange(gcnew cli::array< System::Object^  >(6) {resources->GetString(L"combCERO.Items"), resources->GetString(L"combCERO.Items1"), 
		resources->GetString(L"combCERO.Items2"), resources->GetString(L"combCERO.Items3"), resources->GetString(L"combCERO.Items4"), 
		resources->GetString(L"combCERO.Items5")});

	resources->ApplyResources(this->labRegion, L"labRegion");
	resources->ApplyResources(this->cboxIsEULA, L"cboxIsEULA");

	resources->ApplyResources(this->combRegion, L"combRegion");
	int indexRegion = this->combRegion->SelectedIndex;
	this->combRegion->Items->Clear();
	this->combRegion->Items->AddRange(gcnew cli::array< System::Object^  >(7) {resources->GetString(L"combRegion.Items"), resources->GetString(L"combRegion.Items1"), 
		resources->GetString(L"combRegion.Items2"), resources->GetString(L"combRegion.Items3"), resources->GetString(L"combRegion.Items4"), 
		resources->GetString(L"combRegion.Items5"), resources->GetString(L"combRegion.Items6")});
#if defined(METWL_VER_APPTYPE_SYSTEM) || defined(METWL_VER_APPTYPE_SECURE) || defined(METWL_VER_APPTYPE_LAUNCHER)
	if( langname->Equals( "ja" ) )
	{
		this->combRegion->Items->Add( gcnew System::String( L"全リージョン" ) );
	}
	else
	{
		this->combRegion->Items->Add( gcnew System::String( L"All Region" ) );
	}
#endif
	this->combRegion->SelectedIndex = indexRegion;
	// 中韓のときの特別対応
	if( this->combRegion->DropDownStyle == System::Windows::Forms::ComboBoxStyle::DropDown )
	{
		if( this->hSrl->IsRegionChina )		// このパスになるとき必ずSRLは読み込まれている
		{
			this->combRegion->Text = (langname == "ja")?"中国のみ":"China only";
		}
		else
		{
			this->combRegion->Text = (langname == "ja")?"韓国のみ":"Korea only";
		}
	}

	// Region が変わったときのイベントが勝手に呼ばれて index が -1 にされるので Region 変更後に index を設定
	this->combCERO->SelectedIndex = indexCERO;
	this->combESRB->SelectedIndex = indexESRB;
	this->combUSK->SelectedIndex = indexUSK;
	this->combPEGI->SelectedIndex = indexPEGI;
	this->combPEGI_PRT->SelectedIndex = indexPEGI_PRT;
	this->combPEGI_BBFC->SelectedIndex = indexPEGI_BBFC;
	this->combOFLC->SelectedIndex = indexOFLC;
	this->combGRB->SelectedIndex = indexGRB;

	//resources->ApplyResources(this->combDLCategory, L"combDLCategory");
	//index = this->combDLCategory->SelectedIndex;
	//this->combDLCategory->Items->Clear();
	//this->combDLCategory->Items->AddRange(gcnew cli::array< System::Object^  >(6) {resources->GetString(L"combDLCategory.Items"), 
	//	resources->GetString(L"combDLCategory.Items1"), resources->GetString(L"combDLCategory.Items2"), resources->GetString(L"combDLCategory.Items3"), 
	//	resources->GetString(L"combDLCategory.Items4"), resources->GetString(L"combDLCategory.Items5")});
	//this->combDLCategory->SelectedIndex = index;

	resources->ApplyResources(this->cboxIsInputPerson2, L"cboxIsInputPerson2");
	resources->ApplyResources(this->gboxPerson2, L"gboxPerson2");
	resources->ApplyResources(this->labArbit4, L"labArbit4");
	resources->ApplyResources(this->labArbit3, L"labArbit3");
	resources->ApplyResources(this->labNTSC2Sur, L"labNTSC2Sur");
	resources->ApplyResources(this->tboxNTSC2, L"tboxNTSC2");
	resources->ApplyResources(this->labFax2, L"labFax2");
	resources->ApplyResources(this->labNTSC2Pre, L"labNTSC2Pre");
	resources->ApplyResources(this->tboxFax2, L"tboxFax2");
	resources->ApplyResources(this->tboxMail2, L"tboxMail2");
	resources->ApplyResources(this->tboxTel2, L"tboxTel2");
	resources->ApplyResources(this->tboxFurigana2, L"tboxFurigana2");
	resources->ApplyResources(this->tboxPerson2, L"tboxPerson2");
	resources->ApplyResources(this->tboxDepart2, L"tboxDepart2");
	resources->ApplyResources(this->labDepart2, L"labDepart2");
	resources->ApplyResources(this->tboxCompany2, L"tboxCompany2");
	resources->ApplyResources(this->labMail2, L"labMail2");
	resources->ApplyResources(this->labTel2, L"labTel2");
	resources->ApplyResources(this->labFurigana2, L"labFurigana2");
	resources->ApplyResources(this->labPerson2, L"labPerson2");
	resources->ApplyResources(this->labCompany2, L"labCompany2");
	resources->ApplyResources(this->gboxPerson1, L"gboxPerson1");
	resources->ApplyResources(this->labArbit2, L"labArbit2");
	resources->ApplyResources(this->labArbit1, L"labArbit1");
	resources->ApplyResources(this->labNTSC1Sur, L"labNTSC1Sur");
	resources->ApplyResources(this->labFax1, L"labFax1");
	resources->ApplyResources(this->labNTSC1Pre, L"labNTSC1Pre");
	resources->ApplyResources(this->tboxNTSC1, L"tboxNTSC1");
	resources->ApplyResources(this->tboxFax1, L"tboxFax1");
	resources->ApplyResources(this->tboxMail1, L"tboxMail1");
	resources->ApplyResources(this->tboxTel1, L"tboxTel1");
	resources->ApplyResources(this->tboxFurigana1, L"tboxFurigana1");
	resources->ApplyResources(this->tboxPerson1, L"tboxPerson1");
	resources->ApplyResources(this->tboxDepart1, L"tboxDepart1");
	resources->ApplyResources(this->labDepart1, L"labDepart1");
	resources->ApplyResources(this->tboxCompany1, L"tboxCompany1");
	resources->ApplyResources(this->labMail1, L"labMail1");
	resources->ApplyResources(this->labTel1, L"labTel1");
	resources->ApplyResources(this->labFurigana1, L"labFurigana1");
	resources->ApplyResources(this->labPerson1, L"labPerson1");
	resources->ApplyResources(this->labCompany1, L"labCompany1");
	resources->ApplyResources(this->tboxProductCode2, L"tboxProductCode2");
	resources->ApplyResources(this->tboxProductCode1, L"tboxProductCode1");
	resources->ApplyResources(this->tboxProductName, L"tboxProductName");
	resources->ApplyResources(this->labProductCode2, L"labProductCode2");
	resources->ApplyResources(this->labProductCode1, L"labProductCode1");
	resources->ApplyResources(this->dateSubmit, L"dateSubmit");
	resources->ApplyResources(this->dateRelease, L"dateRelease");
	resources->ApplyResources(this->gboxUsage, L"gboxUsage");
	resources->ApplyResources(this->tboxUsageOther, L"tboxUsageOther");
	resources->ApplyResources(this->rUsageOther, L"rUsageOther");
	resources->ApplyResources(this->rUsageDst, L"rUsageDst");
	resources->ApplyResources(this->rUsageSample, L"rUsageSample");
	resources->ApplyResources(this->rUsageSale, L"rUsageSale");
	resources->ApplyResources(this->gboxSubmitWay, L"gboxSubmitWay");
	resources->ApplyResources(this->rSubmitHand, L"rSubmitHand");
	resources->ApplyResources(this->rSubmitPost, L"rSubmitPost");
	resources->ApplyResources(this->labSubmiteDate, L"labSubmiteDate");
	resources->ApplyResources(this->labReleaseDate, L"labReleaseDate");
	resources->ApplyResources(this->labProductCode, L"labProductCode");
	resources->ApplyResources(this->labProductName, L"labProductName");
	resources->ApplyResources(this->labCapSubmitVer, L"labCapSubmitVer");
	resources->ApplyResources(this->numSubmitVersion, L"numSubmitVersion");
	resources->ApplyResources(this->labSubmitVer, L"labSubmitVer");
	resources->ApplyResources(this->labMultiForeign1, L"labMultiForeign1");
	resources->ApplyResources(this->tboxProductCode2Foreign3, L"tboxProductCode2Foreign3");
	resources->ApplyResources(this->tboxProductCode2Foreign2, L"tboxProductCode2Foreign2");
	resources->ApplyResources(this->labProductCode2Foreign, L"labProductCode2Foreign");
	resources->ApplyResources(this->cboxReleaseForeign, L"cboxReleaseForeign");
	resources->ApplyResources(this->labProductNameForeign, L"labProductNameForeign");
	resources->ApplyResources(this->tboxProductNameForeign, L"tboxProductNameForeign");
	resources->ApplyResources(this->labProductCode1Foreign, L"labProductCode1Foreign");
	resources->ApplyResources(this->tboxProductCode1Foreign, L"tboxProductCode1Foreign");
	resources->ApplyResources(this->labProductCodeForeign, L"labProductCodeForeign");
	resources->ApplyResources(this->tboxProductCode2Foreign1, L"tboxProductCode2Foreign1");
	resources->ApplyResources(this->gboxShared2Size, L"gboxShared2Size");
	resources->ApplyResources(this->labShared2Size5, L"labShared2Size5");
	resources->ApplyResources(this->labShared2Size4, L"labShared2Size4");
	resources->ApplyResources(this->labShared2Size3, L"labShared2Size3");
	resources->ApplyResources(this->labShared2Size2, L"labShared2Size2");
	resources->ApplyResources(this->labShared2Size1, L"labShared2Size1");
	resources->ApplyResources(this->labShared2Size0, L"labShared2Size0");
	resources->ApplyResources(this->tboxShared2Size5, L"tboxShared2Size5");
	resources->ApplyResources(this->tboxShared2Size4, L"tboxShared2Size4");
	resources->ApplyResources(this->tboxShared2Size3, L"tboxShared2Size3");
	resources->ApplyResources(this->tboxShared2Size2, L"tboxShared2Size2");
	resources->ApplyResources(this->tboxShared2Size1, L"tboxShared2Size1");
	resources->ApplyResources(this->tboxShared2Size0, L"tboxShared2Size0");
	resources->ApplyResources(this->cboxIsShared2, L"cboxIsShared2");
	resources->ApplyResources(this->labLib, L"labLib");
	resources->ApplyResources(this->gridSDK, L"gridSDK");
	resources->ApplyResources(this->labSDK, L"labSDK");
	resources->ApplyResources(this->gboxTWLExInfo, L"gboxTWLExInfo");
	resources->ApplyResources(this->labByte1, L"labByte1");
	resources->ApplyResources(this->labHex4, L"labHex4");
	resources->ApplyResources(this->labHex3, L"labHex3");
	resources->ApplyResources(this->tboxIsCodec, L"tboxIsCodec");
	resources->ApplyResources(this->labIsCodec, L"labIsCodec");
	resources->ApplyResources(this->labNormalRomOffset, L"labNormalRomOffset");
	resources->ApplyResources(this->tboxNormalRomOffset, L"tboxNormalRomOffset");
	resources->ApplyResources(this->labKeyTableRomOffset, L"labKeyTableRomOffset");
	resources->ApplyResources(this->tboxPrivateSize, L"tboxPrivateSize");
	resources->ApplyResources(this->labPrivateSize, L"labPrivateSize");
	resources->ApplyResources(this->tboxKeyTableRomOffset, L"tboxKeyTableRomOffset");
	resources->ApplyResources(this->labPublicSize, L"labPublicSize");
	resources->ApplyResources(this->tboxPublicSize, L"tboxPublicSize");
	resources->ApplyResources(this->cboxIsSubBanner, L"cboxIsSubBanner");
	resources->ApplyResources(this->cboxIsNormalJump, L"cboxIsNormalJump");
	resources->ApplyResources(this->cboxIsTmpJump, L"cboxIsTmpJump");
	resources->ApplyResources(this->gboxAccess, L"gboxAccess");
	resources->ApplyResources(this->labSDAccessRight, L"labSDAccessRight");
	resources->ApplyResources(this->labPhotoAccess, L"labPhotoAccess");
	resources->ApplyResources(this->labAccessOther, L"labAccessOther");
	resources->ApplyResources(this->tboxAccessOther, L"tboxAccessOther");
	resources->ApplyResources(this->tboxIsGameCardOn, L"tboxIsGameCardOn");
	resources->ApplyResources(this->labIsGameCardOn, L"labIsGameCardOn");
	resources->ApplyResources(this->labIsGameCardOn2, L"labIsGameCardOn2");
	resources->ApplyResources(this->cboxIsNAND, L"cboxIsNAND");
	resources->ApplyResources(this->cboxIsSD, L"cboxIsSD");
	resources->ApplyResources(this->gboxTitleID, L"gboxTitleID");
	resources->ApplyResources(this->labAppType, L"labAppType");
	resources->ApplyResources(this->labMedia, L"labMedia");
	resources->ApplyResources(this->tboxAppType, L"tboxAppType");
	resources->ApplyResources(this->tboxMedia, L"tboxMedia");
	resources->ApplyResources(this->labHex2, L"labHex2");
	resources->ApplyResources(this->tboxTitleIDLo, L"tboxTitleIDLo");
	resources->ApplyResources(this->labTitleIDLo, L"labTitleIDLo");
	resources->ApplyResources(this->labTitleIDHi, L"labTitleIDHi");
	resources->ApplyResources(this->tboxTitleIDHi, L"tboxTitleIDHi");
	resources->ApplyResources(this->tboxAppTypeOther, L"tboxAppTypeOther");
	resources->ApplyResources(this->labAppTypeOther, L"labAppTypeOther");
	resources->ApplyResources(this->labCaptionEx, L"labCaptionEx");
	resources->ApplyResources(this->tboxCaptionEx, L"tboxCaptionEx");
	resources->ApplyResources(this->gboxProd, L"gboxProd");
	resources->ApplyResources(this->menuStripAbove, L"menuStripAbove");
	resources->ApplyResources(this->stripFile, L"stripFile");
	resources->ApplyResources(this->stripItemOpenRom, L"stripItemOpenRom");
	resources->ApplyResources(this->stripItemSepFile1, L"stripItemSepFile1");
	resources->ApplyResources(this->stripItemSaveTemp, L"stripItemSaveTemp");
	resources->ApplyResources(this->stripItemLoadTemp, L"stripItemLoadTemp");
	resources->ApplyResources(this->exportToolStripMenuItem, L"exportToolStripMenuItem");
	resources->ApplyResources(this->stripItemRomInfoList, L"stripItemRomInfoList");
	resources->ApplyResources(this->stripItemErrorListRead, L"stripItemErrorListRead");
	resources->ApplyResources(this->stripItemErrorListCurrent, L"stripItemErrorListCurrent");
	resources->ApplyResources(this->stripItemRomInfoAndErrorListRead, L"stripItemRomInfoAndErrorListRead");
	resources->ApplyResources(this->stripItemRomInfoAndErrorListCurrent, L"stripItemRomInfoAndErrorListCurrent");
	resources->ApplyResources(this->stripMaster, L"stripMaster");
	resources->ApplyResources(this->stripItemSheet, L"stripItemSheet");
	resources->ApplyResources(this->stripItemSepMaster1, L"stripItemSepMaster1");
	resources->ApplyResources(this->stripItemMasterRom, L"stripItemMasterRom");
	resources->ApplyResources(this->stripItemMiddlewareXml, L"stripItemMiddlewareXml");
	resources->ApplyResources(this->stripItemMiddlewareHtml, L"stripItemMiddlewareHtml");
	resources->ApplyResources(this->stripLang, L"stripLang");
	resources->ApplyResources(this->stripItemEnglish, L"stripItemEnglish");
	resources->ApplyResources(this->stripItemJapanese, L"stripItemJapanese");
	resources->ApplyResources(this->tabMain, L"tabMain");
	resources->ApplyResources(this->tabRomInfo, L"tabRomInfo");
	resources->ApplyResources(this->gridLibrary, L"gridLibrary");
	resources->ApplyResources(this->colLibPublisher, L"colLibPublisher");
	resources->ApplyResources(this->colLibName, L"colLibName");
	resources->ApplyResources(this->tboxGuideRomInfo, L"tboxGuideRomInfo");
	resources->ApplyResources(this->tabTWLInfo, L"tabTWLInfo");
	resources->ApplyResources(this->gboxLaunch, L"gboxLaunch");
	resources->ApplyResources(this->labConnectIcon, L"labConnectIcon");
	resources->ApplyResources(this->tboxGuideTWLInfo, L"tboxGuideTWLInfo");
	resources->ApplyResources(this->gboxExFlags, L"gboxExFlags");
	resources->ApplyResources(this->tabNandSizeInfo, L"tabNandSizeInfo");
	//resources->ApplyResources(this->tboxGuideDLCategory, L"tboxGuideDLCategory");
	//resources->ApplyResources(this->gboxDLCategory, L"gboxDLCategory");
	resources->ApplyResources(this->tboxGuideNandSizeInfo, L"tboxGuideNandSizeInfo");
	resources->ApplyResources(this->gboxNandSize, L"gboxNandSize");
	resources->ApplyResources(this->labNandSize, L"labNandSize");
	resources->ApplyResources(this->labNandSizeFS, L"labNandSizeFS");
	resources->ApplyResources(this->labSrlSize, L"labSrlSize");
	resources->ApplyResources(this->labSumSize2, L"labSumSize2");
	resources->ApplyResources(this->labSumSize, L"labSumSize");
	resources->ApplyResources(this->labSubBannerSize, L"labSubBannerSize");
	resources->ApplyResources(this->labTmdSize, L"labTmdSize");
	resources->ApplyResources(this->tabRomEditInfo, L"tabRomEditInfo");
	resources->ApplyResources(this->gboxOtherSpec, L"gboxOtherSpec");
	resources->ApplyResources(this->cboxIsUGC, L"cboxIsUGC");
	resources->ApplyResources(this->cboxIsPhotoEx, L"cboxIsPhotoEx");
	resources->ApplyResources(this->butSetBack, L"butSetBack");
	resources->ApplyResources(this->tboxGuideRomEditInfo, L"tboxGuideRomEditInfo");
	resources->ApplyResources(this->gboxParental, L"gboxParental");
	resources->ApplyResources(this->cboxIsUnnecessaryRating, L"cboxIsUnnecessaryRating");
	resources->ApplyResources(this->tabSubmitInfo, L"tabSubmitInfo");
	resources->ApplyResources(this->labProductNameLimit, L"labProductNameLimit");
	resources->ApplyResources(this->tboxGuideSubmitInfo, L"tboxGuideSubmitInfo");
	resources->ApplyResources(this->gboxForeign, L"gboxForeign");
	resources->ApplyResources(this->labProductNameLimitForeign, L"labProductNameLimitForeign");
	resources->ApplyResources(this->labMultiForeign2, L"labMultiForeign2");
	resources->ApplyResources(this->tabCompanyInfo, L"tabCompanyInfo");
	resources->ApplyResources(this->tboxGuideCompanyInfo, L"tboxGuideCompanyInfo");
	resources->ApplyResources(this->tabErrorInfo, L"tabErrorInfo");
	resources->ApplyResources(this->tboxGuideErrorInfo, L"tboxGuideErrorInfo");
	resources->ApplyResources(this->gboxErrorTiming, L"gboxErrorTiming");
	resources->ApplyResources(this->rErrorCurrent, L"rErrorCurrent");
	resources->ApplyResources(this->rErrorReading, L"rErrorReading");
	resources->ApplyResources(this->labWarn, L"labWarn");
	resources->ApplyResources(this->labError, L"labError");
	resources->ApplyResources(this->gridWarn, L"gridWarn");
	resources->ApplyResources(this->gridError, L"gridError");
	resources->ApplyResources(this->labFile, L"labFile");
	resources->ApplyResources(this->labAssemblyVersion, L"labAssemblyVersion");
	resources->ApplyResources(this->colErrorName, L"colErrorName");
	resources->ApplyResources(this->colErrorBegin, L"colErrorBegin");
	resources->ApplyResources(this->colErrorEnd, L"colErrorEnd");
	resources->ApplyResources(this->colErrorCause, L"colErrorCause");
	resources->ApplyResources(this->colWarnName, L"colWarnName");
	resources->ApplyResources(this->colWarnBegin, L"colWarnBegin");
	resources->ApplyResources(this->colWarnEnd, L"colWarnEnd");
	resources->ApplyResources(this->colWarnCause, L"colWarnCause");
	resources->ApplyResources(this, L"$this");

	// テキストボックスの表記を変更
	if( !System::String::IsNullOrEmpty( this->tboxFile->Text ) )
	{
		this->setSrlFormsTextBox();
	}

	// 中国用特別メッセージ
	// デフォルトでVisible=falseなのでリソースを読み込みなおすと表示されていてもVisible=falseに初期化されてしまう
	bool visible = this->tboxWarningChinaRating->Visible;
	resources->ApplyResources(this->tboxWarningChinaRating, L"tboxWarningChinaRating");
	this->tboxWarningChinaRating->Visible = visible;

	// バックアップメモリのテキストボックス
	this->combBackup_SelectedIndexChanged( nullptr, nullptr );
	this->cboxReleaseForeign_CheckedChanged( nullptr, nullptr );
	this->cboxIsInputPerson2_CheckedChanged( nullptr, nullptr );
	this->rUsageOther_CheckedChanged( nullptr, nullptr );

	// 「レーティング表示不要」にチェックが入っているときのみレーティングのコンボボックスのテキストを変更
	// (それ以外のときにはApplyResourcesで自動的に切り替わる)
	if( this->cboxIsUnnecessaryRating->Checked )
	{
		this->unnecessaryRating( this->combCERO );
		this->unnecessaryRating( this->combESRB );
		this->unnecessaryRating( this->combUSK );
		this->unnecessaryRating( this->combPEGI );
		this->unnecessaryRating( this->combPEGI_PRT );
		this->unnecessaryRating( this->combPEGI_BBFC );
		this->unnecessaryRating( this->combOFLC );
		this->unnecessaryRating( this->combGRB );
	}

	// フォームのEnableやVisibleを切り替え
	this->changeFormInput( langname );

	// 複数行表示の改行を挿入
	this->tboxGuideRomEditInfo->Text = this->tboxGuideRomEditInfo->Text->Replace( "<newline>", "\r\n" );
	this->tboxGuideNandSizeInfo->Text = this->tboxGuideNandSizeInfo->Text->Replace( "<newline>", "\r\n" );
	this->tboxGuideErrorInfo->Text   = this->tboxGuideErrorInfo->Text->Replace( "<newline>", "\r\n" );

	// バージョンがなくなるので再設定
	System::Reflection::Assembly ^ass = System::Reflection::Assembly::GetEntryAssembly();
	this->labAssemblyVersion->Text = "ver." + this->getVersion();

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
}

// end of file