// deliverable.h �̃N���X����

#include "stdafx.h"
#include <apptype.h>
#include "common.h"
#include "srl.h"
#include "deliverable.h"
#include "utility.h"
#include <cstring>
#include <cstdio>

using namespace MasterEditorTWL;

//
// RCDeliverable �N���X
//

//
// ���ޏo��
//
// @arg [out] �o�̓t�@�C����
// @arg [in]  ROM�o�C�i��(SRL)�ŗL���
// @arg [in]  �t�@�C���S�̂�CRC
// @arg [in]  SRL�̃t�@�C����(���ނɋL�q���邽�߂Ɏg�p)
// @arg [in]  �p��t���O
//
ECDeliverableResult RCDeliverable::writeSpreadsheet(
	System::String ^hFilename, MasterEditorTWL::RCSrl ^hSrl, System::UInt16 ^hCRC, System::String ^hSrlFilename, System::Boolean english )
{
	// �e���v���[�g��ǂݍ���
	System::Xml::XmlDocument ^doc = gcnew System::Xml::XmlDocument();
	try
	{
		doc->Load( "../resource/sheet_templete.xml" );
	}
	catch ( System::Exception ^ex )
	{
		(void)ex;
		return ECDeliverableResult::ERROR_FILE_OPEN;
	}
	System::Xml::XmlElement ^root = doc->DocumentElement;

	// �\�t�g�^�C�g��etc.��1�����������
	char title_name[ TITLE_NAME_MAX ];
	char game_code[  GAME_CODE_MAX ];
	char maker_code[ MAKER_CODE_MAX ];
	MasterEditorTWL::setStringToChars( title_name, hSrl->hTitleName, TITLE_NAME_MAX, 0 );
	MasterEditorTWL::setStringToChars( game_code,  hSrl->hGameCode,  GAME_CODE_MAX,  0 );
	MasterEditorTWL::setStringToChars( maker_code, hSrl->hMakerCode, MAKER_CODE_MAX, 0 );
	//System::String ^str = gcnew System::String( hSrl->hTitleName[0], 1 );
	//System::Diagnostics::Debug::WriteLine( str );
	//System::Diagnostics::Debug::WriteLine( "hex 0x: " + title_name[0].ToString("X") );

	// �A�v�����
	System::String ^apptype = gcnew System::String("");
	if( *hSrl->hIsAppLauncher )
	{
		apptype = gcnew System::String( "Launcher" );
	}
	else if( *hSrl->hIsAppSecure )
	{
		apptype = gcnew System::String( "Secure" );
	}
	else if( *hSrl->hIsAppSystem )
	{
		apptype = gcnew System::String( "System" );
	}
	else if( *hSrl->hIsAppUser )
	{
		apptype = gcnew System::String( "User" );
	}
	System::String ^media = gcnew System::String("");
	if( *hSrl->hIsMediaNand )
	{
		media = gcnew System::String( "NAND" );
	}
	else
	{
		media = gcnew System::String( "Game Card" );
	}
	System::String ^appother = gcnew System::String("");
	if( *(hSrl->hIsLaunch) == false )
	{
		appother += "�����`���[��\��.";
	}
	if( *(hSrl->hIsDataOnly) == true )
	{
		apptype += "�f�[�^��p.";
	}

	// �A�N�Z�X�R���g���[�� ���̑�
	System::String ^access = gcnew System::String("");
	if( *(hSrl->hIsCommonClientKey) == true )
	{
		access += "commonClientKey. ";
	}
	if( *(hSrl->hIsAesSlotBForES) == true )
	{
		access += "AES-SlotB(ES). ";
	}
	if( *(hSrl->hIsAesSlotCForNAM) == true )
	{
		access += "AES-SlotC(NAM). ";
	}
	if( *(hSrl->hIsAesSlotBForJpegEnc) == true )
	{
		access += "AES-SlotB(Jpeg Launcher). ";
	}
	if( *(hSrl->hIsAesSlotBForJpegEncUser) == true )
	{
		access += "AES-SlotB(Jpeg User). ";
	}
	if( *(hSrl->hIsAesSlotAForSSL) == true )
	{
		access += "AES-SlotA(SSL). ";
	}
	if( *(hSrl->hIsCommonClientKeyForDebugger) == true )
	{
		access += "commonClientKey(Debug). ";
	}

	// �g�p���C�Z���X
	System::String ^lib = gcnew System::String("");
	if( hSrl->hLicenseList != nullptr )
	{
		for each( RCLicense ^lic in hSrl->hLicenseList )
		{
			lib += "[" + lic->Publisher + " / " + lic->Name + "]. ";
		}
	}

	// ���ރe���v���[�g�̊e�^�O����͏��ɒu������
	System::Xml::XmlNodeList ^list;
	list = root->GetElementsByTagName( "Data" );
	System::Int32 i;
	for( i=0; i < list->Count; i++ )
	{
		System::Xml::XmlNode ^node = list->Item(i);
		if( (node->FirstChild != nullptr) && (node->FirstChild->Value != nullptr) )
		{
			// ��o���
			if( node->FirstChild->Value->Equals( "TagProductName" ) )
			{
				node->FirstChild->Value = this->hProductName;
			}
			if( node->FirstChild->Value->Equals( "TagProductCode1" ) )
			{
				node->FirstChild->Value = this->hProductCode1;
			}
			if( node->FirstChild->Value->Equals( "TagProductCode2" ) )
			{
				node->FirstChild->Value = this->hProductCode2;
			}
			if( node->FirstChild->Value->Equals( "TagReleaseForeign" ) )
			{
				if( *(this->hReleaseForeign) )
					node->FirstChild->Value = gcnew System::String( "��" );
				else
					node->FirstChild->Value = nullptr;
			}
			if( node->FirstChild->Value->Equals( "TagProductNameForeign" ) )
			{
				node->FirstChild->Value = this->hProductNameForeign;
			}
			if( node->FirstChild->Value->Equals( "TagProductCode1Foreign" ) )
			{
				node->FirstChild->Value = this->hProductCode1Foreign;
			}
			if( node->FirstChild->Value->Equals( "TagProductCode2Foreign" ) )
			{
				node->FirstChild->Value = this->hProductCode2Foreign;
			}
			if( node->FirstChild->Value->Equals( "TagSubmitYear" ) )
			{
				node->FirstChild->Value = this->hSubmitYear->ToString();
			}
			if( node->FirstChild->Value->Equals( "TagSubmitMonth" ) )
			{
				node->FirstChild->Value = this->hSubmitMonth->ToString();
			}
			if( node->FirstChild->Value->Equals( "TagSubmitDay" ) )
			{
				node->FirstChild->Value = this->hSubmitDay->ToString();
			}
			if( node->FirstChild->Value->Equals( "TagReleaseYear" ) )
			{
				node->FirstChild->Value = this->hReleaseYear->ToString();
			}
			if( node->FirstChild->Value->Equals( "TagReleaseMonth" ) )
			{
				node->FirstChild->Value = this->hReleaseMonth->ToString();
			}
			if( node->FirstChild->Value->Equals( "TagReleaseDay" ) )
			{
				node->FirstChild->Value = this->hReleaseDay->ToString();
			}
			if( node->FirstChild->Value->Equals( "TagSubmitWay" ) )
			{
				node->FirstChild->Value = this->hSubmitWay;
			}
			if( node->FirstChild->Value->Equals( "TagUsage" ) )
			{
				node->FirstChild->Value = this->hUsage;
			}
			if( node->FirstChild->Value->Equals( "TagUsageOther" ) )
			{
				node->FirstChild->Value = this->hUsageOther;	// nullptr �̂Ƃ��̓Z������ɂȂ�̂ōD�s��
			}
			if( node->FirstChild->Value->Equals( "TagRomVersion" ) )
			{
				node->FirstChild->Value = hSrl->hRomVersion->ToString("X2");
				if( *(hSrl->hRomVersion) == 0xE0 )
				{
					if( english )
						node->FirstChild->Value += "(Preliminary ver.)";
					else
						node->FirstChild->Value += "(���O��)";
				}
			}
			if( node->FirstChild->Value->Equals( "TagSubmitVersion" ) )
			{
				node->FirstChild->Value = this->hSubmitVersion->ToString("X");
			}
			if( node->FirstChild->Value->Equals( "TagSrlFilename" ) )
			{
				node->FirstChild->Value = hSrlFilename;
			}
			if( node->FirstChild->Value->Equals( "TagCRC" ) )
			{
				node->FirstChild->Value = "0x" + hCRC->ToString("X");
			}
			// ROM���
			if( node->FirstChild->Value->Equals( "TagLatency" ) )
			{
				node->FirstChild->Value = hSrl->hLatency;
			}
			if( node->FirstChild->Value->Equals( "TagPlatform" ) )
			{
				node->FirstChild->Value = hSrl->hPlatform;
			}
			if( node->FirstChild->Value->Equals( "TagRomSize" ) )
			{
				node->FirstChild->Value = hSrl->hRomSize;
			}
			if( node->FirstChild->Value->Equals( "TagBackupMemory" ) )
			{
				node->FirstChild->Value = this->hBackupMemory;
			}
			if( node->FirstChild->Value->Equals( "TagSDK" ) )
			{
				node->FirstChild->Value = this->hSDK;
			}
			if( node->FirstChild->Value->Equals( "TagLibrary" ) )
			{
				node->FirstChild->Value = lib;
			}
			// ROM��� (TWL�g�����)
			if( node->FirstChild->Value->Equals( "TagTitleIDLo" ) )
			{
				node->FirstChild->Value = hSrl->hTitleIDLo;
			}
			if( node->FirstChild->Value->Equals( "TagTitleIDHi" ) )
			{
				node->FirstChild->Value = "0x" + hSrl->hTitleIDHi->ToString("X8");
			}
			if( node->FirstChild->Value->Equals( "TagAppType" ) )
			{
				node->FirstChild->Value = apptype;
			}
			if( node->FirstChild->Value->Equals( "TagMedia" ) )
			{
				node->FirstChild->Value = media;
			}
			if( node->FirstChild->Value->Equals( "TagAppTypeOther" ) )
			{
				node->FirstChild->Value = appother;
			}
			if( node->FirstChild->Value->Equals( "TagIsNormalJump" ) )
			{
				if( *(hSrl->hIsNormalJump) == true )
					node->FirstChild->Value = gcnew System::String("��");
				else
					node->FirstChild->Value = nullptr;
			}
			if( node->FirstChild->Value->Equals( "TagIsTmpJump" ) )
			{
				if( *(hSrl->hIsTmpJump) == true )
					node->FirstChild->Value = gcnew System::String("��");
				else
					node->FirstChild->Value = nullptr;
			}
			if( node->FirstChild->Value->Equals( "TagNormalRomOffset" ) )
			{
				node->FirstChild->Value = "0x" + hSrl->hNormalRomOffset->ToString("X8");
			}
			if( node->FirstChild->Value->Equals( "TagKeyTableRomOffset" ) )
			{
				node->FirstChild->Value = "0x" + hSrl->hKeyTableRomOffset->ToString("X8");
			}
			if( node->FirstChild->Value->Equals( "TagPublicSize" ) )
			{
				node->FirstChild->Value = MasterEditorTWL::transSizeToString( hSrl->hPublicSize );
			}
			if( node->FirstChild->Value->Equals( "TagPrivateSize" ) )
			{
				node->FirstChild->Value = MasterEditorTWL::transSizeToString( hSrl->hPrivateSize );
			}
			if( node->FirstChild->Value->Equals( "TagIsCodec" ) )
			{
				if( *(hSrl->hIsCodecTWL) == true )
					node->FirstChild->Value = gcnew System::String("TWL");
				else
					node->FirstChild->Value = gcnew System::String("NTR");
			}
			if( node->FirstChild->Value->Equals( "TagIsEULA" ) )
			{
				if( *(hSrl->hIsEULA) == true )
					node->FirstChild->Value = gcnew System::String("��");
				else
					node->FirstChild->Value = nullptr;
			}
			if( node->FirstChild->Value->Equals( "TagIsSubBanner" ) )
			{
				if( *(hSrl->hIsSubBanner) == true )
					node->FirstChild->Value = gcnew System::String("��");
				else
					node->FirstChild->Value = nullptr;
			}
			if( node->FirstChild->Value->Equals( "TagIsWiFiIcon" ) )
			{
				if( *(hSrl->hIsWiFiIcon) == true )
					node->FirstChild->Value = gcnew System::String("��");
				else
					node->FirstChild->Value = nullptr;
			}
			if( node->FirstChild->Value->Equals( "TagIsWirelessIcon" ) )
			{
				if( *(hSrl->hIsWirelessIcon) == true )
					node->FirstChild->Value = gcnew System::String("��");
				else
					node->FirstChild->Value = nullptr;
			}
			if( node->FirstChild->Value->Equals( "TagIsWL" ) )
			{
				if( *(hSrl->hIsWL) == true )
					node->FirstChild->Value = gcnew System::String("��");
				else
					node->FirstChild->Value = nullptr;
			}
			if( node->FirstChild->Value->Equals( "TagIsSD" ) )
			{
				if( *(hSrl->hIsSD) == true )
					node->FirstChild->Value = gcnew System::String("��");
				else
					node->FirstChild->Value = nullptr;
			}
			if( node->FirstChild->Value->Equals( "TagIsNAND" ) )
			{
				if( *(hSrl->hIsNAND) == true )
					node->FirstChild->Value = gcnew System::String("��");
				else
					node->FirstChild->Value = nullptr;
			}
			if( node->FirstChild->Value->Equals( "TagIsShared2" ) )
			{
				if( *(hSrl->hIsShared2) == true )
					node->FirstChild->Value = gcnew System::String("��");
				else
					node->FirstChild->Value = nullptr;
			}
			if( node->FirstChild->Value->Equals( "TagIsGameCardOn" ) )
			{
				if( *(hSrl->hIsGameCardNitro) == true )
					node->FirstChild->Value = gcnew System::String("ON(NTR)");
				else if( *(hSrl->hIsGameCardOn) == true )
					node->FirstChild->Value = gcnew System::String("ON(normal)");
				else
					node->FirstChild->Value = gcnew System::String("OFF");;
			}
			if( node->FirstChild->Value->Equals( "TagAccessOther" ) )
			{
				node->FirstChild->Value = access;
			}
			if( node->FirstChild->Value->Equals( "TagShared2Size0" ) )
			{
				node->FirstChild->Value = MasterEditorTWL::transSizeToString( hSrl->hShared2SizeArray[0] );
			}
			if( node->FirstChild->Value->Equals( "TagShared2Size1" ) )
			{
				node->FirstChild->Value = MasterEditorTWL::transSizeToString( hSrl->hShared2SizeArray[1] );
			}
			if( node->FirstChild->Value->Equals( "TagShared2Size2" ) )
			{
				node->FirstChild->Value = MasterEditorTWL::transSizeToString( hSrl->hShared2SizeArray[2] );
			}
			if( node->FirstChild->Value->Equals( "TagShared2Size3" ) )
			{
				node->FirstChild->Value = MasterEditorTWL::transSizeToString( hSrl->hShared2SizeArray[3] );
			}
			if( node->FirstChild->Value->Equals( "TagShared2Size4" ) )
			{
				node->FirstChild->Value = MasterEditorTWL::transSizeToString( hSrl->hShared2SizeArray[4] );
			}
			if( node->FirstChild->Value->Equals( "TagShared2Size5" ) )
			{
				node->FirstChild->Value = MasterEditorTWL::transSizeToString( hSrl->hShared2SizeArray[5] );
			}

			// ��Џ��
			if( node->FirstChild->Value->Equals( "TagCompany1" ) )
			{
				node->FirstChild->Value = this->hCompany1;
			}
			if( node->FirstChild->Value->Equals( "TagPerson1" ) )
			{
				node->FirstChild->Value = this->hPerson1;
			}
			if( node->FirstChild->Value->Equals( "TagFurigana1" ) )
			{
				node->FirstChild->Value = this->hFurigana1;
			}
			if( node->FirstChild->Value->Equals( "TagTel1" ) )
			{
				node->FirstChild->Value = this->hTel1;
			}
			if( node->FirstChild->Value->Equals( "TagFax1" ) )
			{
				node->FirstChild->Value = this->hFax1;
			}
			if( node->FirstChild->Value->Equals( "TagMail1" ) )
			{
				node->FirstChild->Value = this->hMail1;
			}
			if( node->FirstChild->Value->Equals( "TagNTSC1" ) )
			{
				node->FirstChild->Value = this->hNTSC1;
			}
			if( node->FirstChild->Value->Equals( "TagCompany2" ) )
			{
				node->FirstChild->Value = this->hCompany2;
			}
			if( node->FirstChild->Value->Equals( "TagPerson2" ) )
			{
				node->FirstChild->Value = this->hPerson2;
			}
			if( node->FirstChild->Value->Equals( "TagFurigana2" ) )
			{
				node->FirstChild->Value = this->hFurigana2;
			}
			if( node->FirstChild->Value->Equals( "TagTel2" ) )
			{
				node->FirstChild->Value = this->hTel2;
			}
			if( node->FirstChild->Value->Equals( "TagFax2" ) )
			{
				node->FirstChild->Value = this->hFax2;
			}
			if( node->FirstChild->Value->Equals( "TagMail2" ) )
			{
				node->FirstChild->Value = this->hMail2;
			}
			if( node->FirstChild->Value->Equals( "TagNTSC2" ) )
			{
				node->FirstChild->Value = this->hNTSC2;
			}

			// ���l
			if( node->FirstChild->Value->Equals( "TagCaption" ) )
			{
				node->FirstChild->Value = this->hCaption;
			}

			// ���[�W����
			if( node->FirstChild->Value->Equals( "TagRegion" ) )
			{
				node->FirstChild->Value = this->hRegion;
			}

			// �y�A�����^���R���g���[��
			if( node->FirstChild->Value->Equals( "TagRatingCERO" ) )
			{
				node->FirstChild->Value = this->hCERO;
			}
			if( node->FirstChild->Value->Equals( "TagRatingESRB" ) )
			{
				node->FirstChild->Value = this->hESRB;
			}
			if( node->FirstChild->Value->Equals( "TagRatingUSK" ) )
			{
				node->FirstChild->Value = this->hUSK;
			}
			if( node->FirstChild->Value->Equals( "TagRatingPEGI" ) )
			{
				node->FirstChild->Value = this->hPEGI;
			}
			if( node->FirstChild->Value->Equals( "TagRatingPEGI_PRT" ) )
			{
				node->FirstChild->Value = this->hPEGI_PRT;
			}
			if( node->FirstChild->Value->Equals( "TagRatingPEGI_BBFC" ) )
			{
				node->FirstChild->Value = this->hPEGI_BBFC;
			}
			if( node->FirstChild->Value->Equals( "TagRatingOFLC" ) )
			{
				node->FirstChild->Value = this->hOFLC;
			}

			// SRL�ɓo�^����Ȃ�ROM�d�l
			if( node->FirstChild->Value->Equals( "TagIsUGC" ) )
			{
				if( *(this->hIsUGC) == true )
					node->FirstChild->Value = gcnew System::String("��");
				else
					node->FirstChild->Value = nullptr;
			}
			if( node->FirstChild->Value->Equals( "TagIsPhotoEx" ) )
			{
				if( *(this->hIsPhotoEx) == true )
					node->FirstChild->Value = gcnew System::String("��");
				else
					node->FirstChild->Value = nullptr;
			}

			// ROM���o�^�f�[�^��1�o�C�g���\�ɏ�������
			if( node->FirstChild->Value->Equals( "TagRomVersionHex" ) )
			{
				node->FirstChild->Value = hSrl->hRomVersion->ToString("X2");
			}
			System::Int32  byte;
			for( byte=0; byte < TITLE_NAME_MAX; byte++ )
			{
				if( node->FirstChild->Value->Equals( "TagTitleName" + byte.ToString() ) )
				{
					System::String ^bstr = hSrl->hTitleName[byte].ToString();
					if( bstr == nullptr )
						node->FirstChild->Value = gcnew System::String( "null" );
					else if( bstr->Equals( "\0" ) )
						node->FirstChild->Value = gcnew System::String( "\\0" );
					else if( bstr->Equals( " " ) )
						node->FirstChild->Value = gcnew System::String( "\\s" );
					else
						node->FirstChild->Value = gcnew System::String( bstr );
					//node->FirstChild->Value = gcnew System::String( hSrl->hTitleName[byte].ToString() );
				}
				else if( node->FirstChild->Value->Equals( "TagTitleNameHex" + byte.ToString() ) )
				{
					node->FirstChild->Value = title_name[byte].ToString("X2");
				}
			}
			if( node->FirstChild->Value->Equals( "TagGameCode" ) )
			{
				node->FirstChild->Value = hSrl->hGameCode;
			}
			if( node->FirstChild->Value->Equals( "TagMakerCode" ) )
			{
				node->FirstChild->Value = hSrl->hMakerCode;
			}

		} // if( (node->FirstChild != nullptr) && (node->FirstChild->Value != nullptr) )
	} // for( i=0; i < list->Count; i++ )

	try
	{
		doc->Save( hFilename );
	}
	catch ( System::Exception ^ex )
	{
		(void)ex;
		return ECDeliverableResult::ERROR_FILE_WRITE;
	}

	return ECDeliverableResult::NOERROR;
} // ECDeliverableResult RCDeliverable::writeSpreadsheet(System::String ^hFilename, MasterEditorTWL::RCSrl ^hSrl)

