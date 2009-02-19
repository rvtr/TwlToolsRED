// ----------------------------------------------
// フォームとSRL内情報とのデータのやりとり
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
// [SRL <= Form]
// ROM情報をフォームから取得して
// SRLクラスのプロパティに反映させる
// (ROMヘッダへの反映やCRCと署名の再計算をしない)
// ----------------------------------------------
void Form1::setSrlProperties(void)
{
	// ROMヘッダの[0,0x160)の領域はRead Onlyで変更しない

	// リージョン
	this->setRegionSrlPropaties();

	// ペアレンタルコントロール
	this->setRatingSrlProperties();
} //setSrlProperties()

// ----------------------------------------------
// [SRL => Form]
// SRLのROM情報をフォームに反映させる
// (ファイルが読み込まれていることが前提)
// ----------------------------------------------
void Form1::setSrlForms(void)
{
	// NTR互換情報
	this->tboxTitleName->Text = this->hSrl->hTitleName;
	this->tboxGameCode->Text  = this->hSrl->hGameCode;
	this->tboxMakerCode->Text = this->hSrl->hMakerCode;
	this->tboxPlatform->Text  = this->hSrl->hPlatform;
	this->tboxRomSize->Text   = this->hSrl->hRomSize;
	this->tboxRomLatency->Text = this->hSrl->hLatency;
	this->tboxRemasterVer->Text = this->hSrl->RomVersion.ToString("X2");
	if( this->hSrl->RomVersion == 0xE0 )
	{
		this->cboxRemasterVerE->Checked = true;
	}
	else
	{
		this->cboxRemasterVerE->Checked = false;
	}

	this->tboxHeaderCRC->Clear();
	this->tboxHeaderCRC->AppendText( "0x" );
	this->tboxHeaderCRC->AppendText( this->hSrl->HeaderCRC.ToString("X4") );

	if( this->hSrl->hPlatform == nullptr )
	{
		this->errMsg( "プラットホーム指定が不正です。ROMデータのビルド設定を見直してください。",
					  "Illegal Platform: Please check build settings of the ROM data.");
	}

	// TWL拡張情報
	this->tboxTitleIDLo->Text = this->hSrl->hTitleIDLo;
	this->tboxTitleIDHi->Text = this->hSrl->TitleIDHi.ToString("X8");
	this->tboxNormalRomOffset->Text   = this->hSrl->NormalRomOffset.ToString("X8");
	this->tboxKeyTableRomOffset->Text = this->hSrl->KeyTableRomOffset.ToString("X8");
	this->cboxIsNormalJump->Checked = this->hSrl->IsNormalJump;
	this->cboxIsTmpJump->Checked    = this->hSrl->IsTmpJump;
	this->cboxIsSubBanner->Checked  = this->hSrl->IsSubBanner;
	if( this->hSrl->IsCodecTWL == true )
	{
		this->tboxIsCodec->Text = gcnew System::String( "TWL" );
	}
	else
	{
		this->tboxIsCodec->Text = gcnew System::String( "DS" );
	}
	this->cboxIsSD->Checked   = this->hSrl->IsSD;
	this->cboxIsNAND->Checked = this->hSrl->IsNAND;
	if( this->hSrl->IsGameCardNitro )
	{
		this->tboxIsGameCardOn->Text = gcnew System::String( "ON(NTR)" );
	}
	else if( this->hSrl->IsGameCardOn == true )
	{
		this->tboxIsGameCardOn->Text = gcnew System::String( "ON(normal)" );
	}
	else
	{
		this->tboxIsGameCardOn->Text = gcnew System::String( "OFF" );
	}
	this->cboxIsShared2->Checked = this->hSrl->IsShared2;
	this->tboxShared2Size0->Text = MasterEditorTWL::transSizeToString( this->hSrl->hShared2SizeArray[0] );
	this->tboxShared2Size1->Text = MasterEditorTWL::transSizeToString( this->hSrl->hShared2SizeArray[1] );
	this->tboxShared2Size2->Text = MasterEditorTWL::transSizeToString( this->hSrl->hShared2SizeArray[2] );
	this->tboxShared2Size3->Text = MasterEditorTWL::transSizeToString( this->hSrl->hShared2SizeArray[3] );
	this->tboxShared2Size4->Text = MasterEditorTWL::transSizeToString( this->hSrl->hShared2SizeArray[4] );
	this->tboxShared2Size5->Text = MasterEditorTWL::transSizeToString( this->hSrl->hShared2SizeArray[5] );

	// NAND消費サイズ
	//this->tboxPublicSize->Text  = MasterEditorTWL::transSizeToString( this->hSrl->PublicSize );
	//this->tboxPrivateSize->Text = MasterEditorTWL::transSizeToString( this->hSrl->PrivateSize );
	this->tboxSrlSize->Text       = MasterEditorTWL::transSizeToString( this->hSrl->hNandUsedSize->SrlSize );
	this->tboxPublicSize->Text    = MasterEditorTWL::transSizeToString( this->hSrl->hNandUsedSize->PublicSaveSize );
	this->tboxPrivateSize->Text   = MasterEditorTWL::transSizeToString( this->hSrl->hNandUsedSize->PrivateSaveSize );
	this->tboxSubBannerSize->Text = MasterEditorTWL::transSizeToString( this->hSrl->hNandUsedSize->SubBannerSize );
	this->tboxTmdSize->Text       = MasterEditorTWL::transSizeToString( this->hSrl->hNandUsedSize->TmdSize );
	// NANDのクラスタサイズに切り上げた値
	this->tboxSrlSizeFS->Text       = MasterEditorTWL::transSizeToStringKB( this->hSrl->hNandUsedSize->SrlSizeRoundUp );
	this->tboxPublicSizeFS->Text    = MasterEditorTWL::transSizeToStringKB( this->hSrl->hNandUsedSize->PublicSaveSizeRoundUp );
	this->tboxPrivateSizeFS->Text   = MasterEditorTWL::transSizeToStringKB( this->hSrl->hNandUsedSize->PrivateSaveSizeRoundUp );
	this->tboxSubBannerSizeFS->Text = MasterEditorTWL::transSizeToStringKB( this->hSrl->hNandUsedSize->SubBannerSizeRoundUp );
	this->tboxTmdSizeFS->Text       = MasterEditorTWL::transSizeToStringKB( this->hSrl->hNandUsedSize->TmdSizeRoundUp );
	// 総和
	this->tboxSumSize->Text         = MasterEditorTWL::transSizeToStringKB( this->hSrl->hNandUsedSize->NandUsedSize );
	//this->tboxSumSizeMB->Text       = MasterEditorTWL::transSizeToStringMB( this->hSrl->hNandUsedSize->NandUsedSize, 2 );
	this->tboxSumSizeMB->Text       = MasterEditorTWL::transSizeToStringMB( this->hSrl->hNandUsedSize->NandUsedSize );

	// アプリ種別
	if( this->hSrl->IsMediaNand )
	{
		this->tboxMedia->Text = gcnew System::String( "NAND" );
	}
	else
	{
		this->tboxMedia->Text = gcnew System::String( "Game Card" );
	}


	if( this->hSrl->IsAppLauncher == true )
	{
		this->tboxAppType->Text = gcnew System::String( "Launcher" );
	}
	else if( this->hSrl->IsAppSecure == true )
	{
		this->tboxAppType->Text = gcnew System::String( "Secure" );
	}
	else if( this->hSrl->IsAppSystem == true )
	{
		this->tboxAppType->Text = gcnew System::String( "System" );
	}
	else if( this->hSrl->IsAppUser == true )
	{
		this->tboxAppType->Text = gcnew System::String( "User" );
	}
	else
	{
		this->tboxAppType->Text = gcnew System::String( "UNKNOWN" );
	}

	// アクセスコントロール その他
	System::String ^acc = gcnew System::String("");
	if( this->hSrl->IsCommonClientKey == true )
	{
		acc += "commonClientKey.\r\n";
	}
	if( this->hSrl->IsAesSlotBForES == true )
	{
		acc += "AES-SlotB(ES).\r\n";
	}
	if( this->hSrl->IsAesSlotCForNAM == true )
	{
		acc += "AES-SlotC(NAM).\r\n";
	}
	if( this->hSrl->IsAesSlotBForJpegEnc == true )
	{
		acc += "AES-SlotB(Jpeg Launcher).\r\n";
	}
	if( this->hSrl->IsAesSlotBForJpegEncUser == true )
	{
		acc += "AES-SlotB(Jpeg User).\r\n";
	}
	if( this->hSrl->IsAesSlotAForSSL == true )
	{
		acc += "AES-SlotA(SSL).\r\n";
	}
	if( this->hSrl->IsCommonClientKeyForDebugger == true )
	{
		acc += "commonClientKey(Debug).\r\n";
	}
	this->tboxAccessOther->Text = acc;

	// 起動制限
	this->cboxIsEULA->Checked = this->hSrl->IsEULA;

	// 特殊な設定をテキストボックスに反映
	this->setSrlFormsTextBox();

	// SDKバージョンとライブラリ
	this->gridSDK->Rows->Clear();
	if( this->hSrl->hSDKList != nullptr )
	{
		for each( RCSDKVersion ^ver in this->hSrl->hSDKList )
		{
			this->gridSDK->Rows->Add( gcnew cli::array<System::Object^>{ ver->Version } );
			if( ver->IsStatic )
			{
				System::Int32 last = this->gridSDK->Rows->Count - 2;	// 追加直後の行
				this->gridSDK->Rows[ last ]->DefaultCellStyle->ForeColor = System::Drawing::Color::Blue;
			}
		}
	}
	this->gridSDK->CurrentCell = nullptr;	// セルが選択(ハイライト)されていない状態にする

	this->gridLibrary->Rows->Clear();
	if( this->hSrl->hLicenseList != nullptr )
	{
		for each( RCLicense ^lic in this->hSrl->hLicenseList )
		{
			this->gridLibrary->Rows->Add( gcnew cli::array<System::Object^>{lic->Publisher, lic->Name} );
			if( lic->Publisher->Equals( "NINTENDO" ) && lic->Name->Equals( "DEBUG" ) )
			{
				System::Int32 last = this->gridLibrary->Rows->Count - 2;
				this->gridLibrary->Rows[ last ]->DefaultCellStyle->ForeColor = System::Drawing::Color::Red;
			}
		}
	}
	this->gridLibrary->CurrentCell = nullptr;

	// 編集可能情報
	this->setRegionForms();
	this->setRatingForms();			// ペアレンタルコントロール関連

	// ROMヘッダには関係ないが
	// NANDアプリのときにバックアップメモリを自動的に「なし」にしておく
	this->maskBackupMemoryForms();
	//// カードアプリのときにDSi Wareの販売カテゴリを自動的に「なし」にしておく
	//this->maskDLCategoryForms();
} // setSrlForms()

// ----------------------------------------------
// [SRL => Form]
// SRLの特殊な設定をフォームにセットする
// (言語切り替えで表示を変えたいので独立させる)
// ----------------------------------------------
void Form1::setSrlFormsTextBox()
{
	if( !this->hSrl->IsWiFiIcon && !this->hSrl->IsWirelessIcon )
	{
		if( this->isJapanese() )
			this->tboxConnectIcon->Text = "アイコンを表示しない";
		else
			this->tboxConnectIcon->Text = "No Icon";
	}
	else if( this->hSrl->IsWiFiIcon && !this->hSrl->IsWirelessIcon )
	{
		if( this->isJapanese() )
			this->tboxConnectIcon->Text = "Wi-Fiコネクションアイコン";
		else
			this->tboxConnectIcon->Text = "Wi-Fi Connection Icon";
	}
	else if( !this->hSrl->IsWiFiIcon && this->hSrl->IsWirelessIcon )
	{
		if( this->isJapanese() )
			this->tboxConnectIcon->Text = "ワイヤレス通信アイコン";
		else
			this->tboxConnectIcon->Text = "Wireless Icon";
	}
	else
	{
		if( this->isJapanese() )
			this->tboxConnectIcon->Text = "不正な設定";
		else
			this->tboxConnectIcon->Text = "Illegal Setting";
	}

	System::String ^appother = gcnew System::String("");
	if( !this->hSrl->IsLaunch )
	{
		if( this->isJapanese() == true )
			appother += "ランチャー非表示.\r\n";
		else
			appother += "Not Display On the Launcher.\r\n";
	}
	if( this->hSrl->IsDataOnly )
	{
		if( this->isJapanese() == true )
			appother += "データ専用.\r\n";
		else
			appother += "Data Only.\r\n";
	}
	this->tboxAppTypeOther->Text = appother;

	this->tboxCaptionEx->Clear();
	if( this->hSrl->HasDSDLPlaySign )
	{
		if( this->isJapanese() == true )
			this->tboxCaptionEx->Text += gcnew System::String( "DSクローンブート対応.\r\n" );
		else
			this->tboxCaptionEx->Text += gcnew System::String( "DS Clone Boot.\r\n" );
	}
	if( this->hSrl->IsSCFGAccess )
	{
		if( this->isJapanese() == true )
			this->tboxCaptionEx->Text += gcnew System::String( "SCFGレジスタアクセス可能.\r\n" );
		else
			this->tboxCaptionEx->Text += gcnew System::String( "SDFC Register Accessible.\r\n" );
	}
} // setSrlFormsTextBox()

// ----------------------------------------------
// フォームの入力をチェックする
// ----------------------------------------------
System::Boolean Form1::checkSrlForms(void)
{
	// リージョン
	if( this->checkBoxIndex( this->combRegion, LANG_REGION_J, LANG_REGION_E, true ) == false )
		return false;

	// リージョンを決める
	System::Boolean bJapan     = false;
	System::Boolean bAmerica   = false;
	System::Boolean bEurope    = false;
	System::Boolean bAustralia = false;
	switch( this->combRegion->SelectedIndex )
	{
		case 0:
			bJapan = true;
		break;
		case 1:
			bAmerica = true;
		break;
		case 2:
			bEurope = true;
		break;
		case 3:
			bAustralia = true;
		break;
		case 4:
			bEurope    = true;
			bAustralia = true;
		break;
		case 5:
			bAmerica = true;
			bAustralia = true;
		break;
		case 6:
			bAmerica = true;
			bEurope    = true;
			bAustralia = true;
		break;

#if defined(METWL_VER_APPTYPE_SYSTEM) || defined(METWL_VER_APPTYPE_SECURE) || defined(METWL_VER_APPTYPE_LAUNCHER)
		case 7:
			bJapan = true;
			bAmerica = true;
			bEurope = true;
			bAustralia = true;
			this->hWarnList->Add( gcnew RCMrcError( 
			"リージョン", 0x1b0, 0x1b3, "全リージョンが設定されています。仕向地別に設定する必要がないかご確認ください。",
			"Region", "All Region is set. Please check necessity for setting each region individually.", true, true ) );
		break;
#endif
		default:
		break;
	}

	// ペアレンタルコントロール
	if( this->cboxIsUnnecessaryRating->Checked )
	{
		this->hWarnList->Add( gcnew RCMrcError( 
			"ペアレンタルコントロール情報", METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE,
			"レーティング表示が不要であると指定されています。この指定は、ソフトがゲームでないツール類のときのみ選択可能です。レーティング表示が不要であるかどうかは弊社窓口にご相談ください。",
			"Parental Control",
			"In your selection, rating is unnecessary. This selection is available for only tool application which is not game. Please contact Nintendo for checking validation of this selection",
			true, true ) );
	}
	else
	{
		this->checkRatingForms( bJapan, this->combCERO, this->labCERO->Text );
		this->checkRatingForms( bAmerica, this->combESRB, this->labESRB->Text );
		this->checkRatingForms( bEurope, this->combUSK, this->labUSK->Text );
		this->checkRatingForms( bEurope, this->combPEGI, this->labPEGI->Text );
		this->checkRatingForms( bEurope, this->combPEGI_PRT, this->labPEGI_PRT->Text );
		this->checkRatingForms( bEurope, this->combPEGI_BBFC, this->labPEGI_BBFC->Text );
		this->checkRatingForms( bAustralia, this->combOFLC, this->labOFLC->Text );
	}

	// ひととおりエラー登録をした後で
	// SRLバイナリに影響を与えるエラーが存在するかチェック
#ifdef METWL_LIGHT_CHECK
	return this->isValidAffectRomModified();
#else
	return this->isValidAffectRom();
#endif
} // checkSrlForms()

// end of file