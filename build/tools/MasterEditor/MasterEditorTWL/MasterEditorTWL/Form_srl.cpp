// ----------------------------------------------
// �t�H�[����SRL�����Ƃ̃f�[�^�̂��Ƃ�
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
// ROM�����t�H�[������擾����
// SRL�N���X�̃v���p�e�B�ɔ��f������
// (ROM�w�b�_�ւ̔��f��CRC�Ə����̍Čv�Z�����Ȃ�)
// ----------------------------------------------
void Form1::setSrlProperties(void)
{
	// ROM�w�b�_��[0,0x160)�̗̈��Read Only�ŕύX���Ȃ�

	// ���[�W����
	this->setRegionSrlPropaties();

	// �y�A�����^���R���g���[��
	this->setRatingSrlProperties();
} //setSrlProperties()

// ----------------------------------------------
// [SRL => Form]
// SRL��ROM�����t�H�[���ɔ��f������
// (�t�@�C�����ǂݍ��܂�Ă��邱�Ƃ��O��)
// ----------------------------------------------
void Form1::setSrlForms(void)
{
	// NTR�݊����
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
		this->errMsg( "�v���b�g�z�[���w�肪�s���ł��BROM�f�[�^�̃r���h�ݒ���������Ă��������B",
					  "Illegal Platform: Please check build settings of the ROM data.");
	}

	// TWL�g�����
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

	// NAND����T�C�Y
	//this->tboxPublicSize->Text  = MasterEditorTWL::transSizeToString( this->hSrl->PublicSize );
	//this->tboxPrivateSize->Text = MasterEditorTWL::transSizeToString( this->hSrl->PrivateSize );
	this->tboxSrlSize->Text       = MasterEditorTWL::transSizeToString( this->hSrl->hNandUsedSize->SrlSize );
	this->tboxPublicSize->Text    = MasterEditorTWL::transSizeToString( this->hSrl->hNandUsedSize->PublicSaveSize );
	this->tboxPrivateSize->Text   = MasterEditorTWL::transSizeToString( this->hSrl->hNandUsedSize->PrivateSaveSize );
	this->tboxSubBannerSize->Text = MasterEditorTWL::transSizeToString( this->hSrl->hNandUsedSize->SubBannerSize );
	this->tboxTmdSize->Text       = MasterEditorTWL::transSizeToString( this->hSrl->hNandUsedSize->TmdSize );
	// NAND�̃N���X�^�T�C�Y�ɐ؂�グ���l
	this->tboxSrlSizeFS->Text       = MasterEditorTWL::transSizeToStringKB( this->hSrl->hNandUsedSize->SrlSizeRoundUp );
	this->tboxPublicSizeFS->Text    = MasterEditorTWL::transSizeToStringKB( this->hSrl->hNandUsedSize->PublicSaveSizeRoundUp );
	this->tboxPrivateSizeFS->Text   = MasterEditorTWL::transSizeToStringKB( this->hSrl->hNandUsedSize->PrivateSaveSizeRoundUp );
	this->tboxSubBannerSizeFS->Text = MasterEditorTWL::transSizeToStringKB( this->hSrl->hNandUsedSize->SubBannerSizeRoundUp );
	this->tboxTmdSizeFS->Text       = MasterEditorTWL::transSizeToStringKB( this->hSrl->hNandUsedSize->TmdSizeRoundUp );
	// ���a
	this->tboxSumSize->Text         = MasterEditorTWL::transSizeToStringKB( this->hSrl->hNandUsedSize->NandUsedSize );
	//this->tboxSumSizeMB->Text       = MasterEditorTWL::transSizeToStringMB( this->hSrl->hNandUsedSize->NandUsedSize, 2 );
	this->tboxSumSizeMB->Text       = MasterEditorTWL::transSizeToStringMB( this->hSrl->hNandUsedSize->NandUsedSize );

	// �A�v�����
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

	// �A�N�Z�X�R���g���[�� ���̑�
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

	// �N������
	this->cboxIsEULA->Checked = this->hSrl->IsEULA;

	// ����Ȑݒ���e�L�X�g�{�b�N�X�ɔ��f
	this->setSrlFormsTextBox();

	// SDK�o�[�W�����ƃ��C�u����
	this->gridSDK->Rows->Clear();
	if( this->hSrl->hSDKList != nullptr )
	{
		for each( RCSDKVersion ^ver in this->hSrl->hSDKList )
		{
			this->gridSDK->Rows->Add( gcnew cli::array<System::Object^>{ ver->Version } );
			if( ver->IsStatic )
			{
				System::Int32 last = this->gridSDK->Rows->Count - 2;	// �ǉ�����̍s
				this->gridSDK->Rows[ last ]->DefaultCellStyle->ForeColor = System::Drawing::Color::Blue;
			}
		}
	}
	this->gridSDK->CurrentCell = nullptr;	// �Z�����I��(�n�C���C�g)����Ă��Ȃ���Ԃɂ���

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

	// �ҏW�\���
	this->setRegionForms();
	this->setRatingForms();			// �y�A�����^���R���g���[���֘A

	// ROM�w�b�_�ɂ͊֌W�Ȃ���
	// NAND�A�v���̂Ƃ��Ƀo�b�N�A�b�v�������������I�Ɂu�Ȃ��v�ɂ��Ă���
	this->maskBackupMemoryForms();
	//// �J�[�h�A�v���̂Ƃ���DSi Ware�̔̔��J�e�S���������I�Ɂu�Ȃ��v�ɂ��Ă���
	//this->maskDLCategoryForms();
} // setSrlForms()

// ----------------------------------------------
// [SRL => Form]
// SRL�̓���Ȑݒ���t�H�[���ɃZ�b�g����
// (����؂�ւ��ŕ\����ς������̂œƗ�������)
// ----------------------------------------------
void Form1::setSrlFormsTextBox()
{
	if( !this->hSrl->IsWiFiIcon && !this->hSrl->IsWirelessIcon )
	{
		if( this->isJapanese() )
			this->tboxConnectIcon->Text = "�A�C�R����\�����Ȃ�";
		else
			this->tboxConnectIcon->Text = "No Icon";
	}
	else if( this->hSrl->IsWiFiIcon && !this->hSrl->IsWirelessIcon )
	{
		if( this->isJapanese() )
			this->tboxConnectIcon->Text = "Wi-Fi�R�l�N�V�����A�C�R��";
		else
			this->tboxConnectIcon->Text = "Wi-Fi Connection Icon";
	}
	else if( !this->hSrl->IsWiFiIcon && this->hSrl->IsWirelessIcon )
	{
		if( this->isJapanese() )
			this->tboxConnectIcon->Text = "���C�����X�ʐM�A�C�R��";
		else
			this->tboxConnectIcon->Text = "Wireless Icon";
	}
	else
	{
		if( this->isJapanese() )
			this->tboxConnectIcon->Text = "�s���Ȑݒ�";
		else
			this->tboxConnectIcon->Text = "Illegal Setting";
	}

	System::String ^appother = gcnew System::String("");
	if( !this->hSrl->IsLaunch )
	{
		if( this->isJapanese() == true )
			appother += "�����`���[��\��.\r\n";
		else
			appother += "Not Display On the Launcher.\r\n";
	}
	if( this->hSrl->IsDataOnly )
	{
		if( this->isJapanese() == true )
			appother += "�f�[�^��p.\r\n";
		else
			appother += "Data Only.\r\n";
	}
	this->tboxAppTypeOther->Text = appother;

	this->tboxCaptionEx->Clear();
	if( this->hSrl->HasDSDLPlaySign )
	{
		if( this->isJapanese() == true )
			this->tboxCaptionEx->Text += gcnew System::String( "DS�N���[���u�[�g�Ή�.\r\n" );
		else
			this->tboxCaptionEx->Text += gcnew System::String( "DS Clone Boot.\r\n" );
	}
	if( this->hSrl->IsSCFGAccess )
	{
		if( this->isJapanese() == true )
			this->tboxCaptionEx->Text += gcnew System::String( "SCFG���W�X�^�A�N�Z�X�\.\r\n" );
		else
			this->tboxCaptionEx->Text += gcnew System::String( "SDFC Register Accessible.\r\n" );
	}
} // setSrlFormsTextBox()

// ----------------------------------------------
// �t�H�[���̓��͂��`�F�b�N����
// ----------------------------------------------
System::Boolean Form1::checkSrlForms(void)
{
	// ���[�W����
	if( this->checkBoxIndex( this->combRegion, LANG_REGION_J, LANG_REGION_E, true ) == false )
		return false;

	// ���[�W���������߂�
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
			"���[�W����", 0x1b0, 0x1b3, "�S���[�W�������ݒ肳��Ă��܂��B�d���n�ʂɐݒ肷��K�v���Ȃ������m�F���������B",
			"Region", "All Region is set. Please check necessity for setting each region individually.", true, true ) );
		break;
#endif
		default:
		break;
	}

	// �y�A�����^���R���g���[��
	if( this->cboxIsUnnecessaryRating->Checked )
	{
		this->hWarnList->Add( gcnew RCMrcError( 
			"�y�A�����^���R���g���[�����", METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE,
			"���[�e�B���O�\�����s�v�ł���Ǝw�肳��Ă��܂��B���̎w��́A�\�t�g���Q�[���łȂ��c�[���ނ̂Ƃ��̂ݑI���\�ł��B���[�e�B���O�\�����s�v�ł��邩�ǂ����͕��Б����ɂ����k���������B",
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

	// �ЂƂƂ���G���[�o�^���������
	// SRL�o�C�i���ɉe����^����G���[�����݂��邩�`�F�b�N
#ifdef METWL_LIGHT_CHECK
	return this->isValidAffectRomModified();
#else
	return this->isValidAffectRom();
#endif
} // checkSrlForms()

// end of file