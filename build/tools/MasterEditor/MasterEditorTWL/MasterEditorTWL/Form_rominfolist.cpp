// ----------------------------------------------
// ROM��񃊃X�g�̏����o��
// ----------------------------------------------

#include "stdafx.h"
#include "Form1.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace MasterEditorTWL;

// ======================================================
// XML�̈�v�f���쐬
// ======================================================
static System::Xml::XmlElement^ CreateRomInfoListElement(System::Xml::XmlDocument ^doc, 
														 System::String ^label, System::String ^val, System::String ^type)
{
	System::Xml::XmlElement ^tag = doc->CreateElement("info");
	MasterEditorTWL::appendXmlTag( doc, tag, "label",  label );
	MasterEditorTWL::appendXmlTag( doc, tag, "value", val );
	if( type )
	{
		MasterEditorTWL::appendXmlTag( doc, tag, "type",  type );
	}
	return tag;
}

static System::Xml::XmlElement^ CreateRomInfoListElement(System::Xml::XmlDocument ^doc,
														 System::String ^label, System::Boolean isCheck, System::String ^type)
{
	System::Xml::XmlElement ^tag = doc->CreateElement("info");
	MasterEditorTWL::appendXmlTag( doc, tag, "label",  label );
	MasterEditorTWL::appendXmlTag( doc, tag, "value", (isCheck)?"Yes":"No" );
	if( type )
	{
		MasterEditorTWL::appendXmlTag( doc, tag, "type",  type );
	}
	return tag;
}

static System::Xml::XmlElement^ CreateSDKVersionListElement(System::Xml::XmlDocument ^doc,
															System::String ^version, System::Boolean isStatic)
{
	System::Xml::XmlElement ^tag = doc->CreateElement("sdk-version");
	MasterEditorTWL::appendXmlTag( doc, tag, "version",  version );
	MasterEditorTWL::appendXmlTag( doc, tag, "is-static", (isStatic)?"Resident module":"" );
	return tag;
}

// ======================================================
// XML�`���Ń��X�g���쐬
// ======================================================
void Form1::makeRomInfoListXml(System::Xml::XmlDocument ^doc, System::Boolean withError, System::Boolean isCurrent)
{
	if( withError )
	{
		// �G���[�����ŐV�ɍX�V
		this->hErrorList->Clear();
		this->hWarnList->Clear();
		this->checkSrlForms();
		this->checkDeliverableForms();
	}

	System::Xml::XmlElement ^root = doc->CreateElement( "twl-master-editor" );
	System::Reflection::Assembly ^ass = System::Reflection::Assembly::GetEntryAssembly();
	root->SetAttribute( "version", this->getVersion() );
	doc->AppendChild( root );

	// �Q�[�����
	root->AppendChild( this->makeGameInfoXmlElement(doc) );

	//
	// �e�^�u���Ƃɏ����܂Ƃ߂�
	//
	System::Xml::XmlElement ^section;

	// ROM��{���
	section = doc->CreateElement("basic-rom-info");
	MasterEditorTWL::appendXmlTag( doc, section, "index", this->isJapanese()?"ROM��{���":"Basic ROM Info" );
	{
		System::Xml::XmlElement ^tag = doc->CreateElement("info-list");
		tag->AppendChild( CreateRomInfoListElement(doc, this->labTitleName->Text, this->tboxTitleName->Text, nullptr) );
		tag->AppendChild( CreateRomInfoListElement(doc, this->labGameCode->Text, this->tboxGameCode->Text, nullptr) );
		tag->AppendChild( CreateRomInfoListElement(doc, this->labMakerCode->Text, this->tboxMakerCode->Text, nullptr) );
		tag->AppendChild( CreateRomInfoListElement(doc, this->labPlatform->Text, this->tboxPlatform->Text, nullptr) );
		tag->AppendChild( CreateRomInfoListElement(doc, this->labRomType->Text, this->tboxRomLatency->Text, nullptr) );
		tag->AppendChild( CreateRomInfoListElement(doc, this->labRomSize->Text, this->tboxRomSize->Text, nullptr) );
		tag->AppendChild( CreateRomInfoListElement(doc, this->labRemasterVer->Text, this->tboxRemasterVer->Text, nullptr) );
		tag->AppendChild( CreateRomInfoListElement(doc, this->labCaptionEx->Text, this->tboxCaptionEx->Text, nullptr) );
		section->AppendChild(tag);
	}
	root->AppendChild(section);

	// TWL�g�����^�u
	section = doc->CreateElement("twl-extended-info");
	MasterEditorTWL::appendXmlTag( doc, section, "index", this->isJapanese()?"TWL�g�����":"TWL Extended Info" );
	{
		System::Xml::XmlElement ^tag = doc->CreateElement("info-list");
		tag->AppendChild( CreateRomInfoListElement(doc, this->labTitleIDLo->Text, this->tboxTitleIDLo->Text, nullptr) );
		tag->AppendChild( CreateRomInfoListElement(doc, this->labTitleIDHi->Text, this->tboxTitleIDHi->Text, nullptr) );
		tag->AppendChild( CreateRomInfoListElement(doc, this->labAppType->Text, this->tboxAppType->Text, nullptr) );
		tag->AppendChild( CreateRomInfoListElement(doc, this->labAppTypeOther->Text, this->tboxAppTypeOther->Text, nullptr) );
		tag->AppendChild( CreateRomInfoListElement(doc, this->labIsCodec->Text, this->tboxIsCodec->Text, nullptr) );
		tag->AppendChild( CreateRomInfoListElement(doc, this->labKeyTableRomOffset->Text, this->tboxKeyTableRomOffset->Text, nullptr) );
		//tag->AppendChild( CreateRomInfoListElement(doc, this->labNormalRomOffset->Text, this->tboxNormalRomOffset->Text, nullptr) );
		tag->AppendChild( CreateRomInfoListElement(doc, this->cboxIsSubBanner->Text, this->cboxIsSubBanner->Checked, nullptr) );
		tag->AppendChild( CreateRomInfoListElement(doc, this->cboxIsTmpJump->Text, this->cboxIsTmpJump->Checked, nullptr) );
		tag->AppendChild( CreateRomInfoListElement(doc, this->cboxIsNormalJump->Text, this->cboxIsNormalJump->Checked, nullptr) );
		tag->AppendChild( CreateRomInfoListElement(doc, this->cboxIsEULA->Text, this->cboxIsEULA->Checked, nullptr) );
		tag->AppendChild( CreateRomInfoListElement(doc, this->labConnectIcon->Text, this->tboxConnectIcon->Text, nullptr) );
		section->AppendChild(tag);
	}
	root->AppendChild(section);

	// �A�N�Z�X���
	section = doc->CreateElement("access-control-info");
	MasterEditorTWL::appendXmlTag( doc, section, "index", this->isJapanese()?"�A�N�Z�X�R���g���[�����":"Access Control" );
	{
		System::Xml::XmlElement ^tag = doc->CreateElement("info-list");
		tag->AppendChild( CreateRomInfoListElement(doc, this->cboxIsSD->Text, this->cboxIsSD->Checked, nullptr) );
		tag->AppendChild( CreateRomInfoListElement(doc, this->cboxIsNAND->Text, this->cboxIsNAND->Checked, nullptr) );
		tag->AppendChild( CreateRomInfoListElement(doc, this->labSDAccessRight->Text, this->tboxSDAccessRight->Text, nullptr) );
		tag->AppendChild( CreateRomInfoListElement(doc, this->labPhotoAccess->Text, this->tboxPhotoAccess->Text, nullptr) );
		tag->AppendChild( CreateRomInfoListElement(doc, this->labAccessOther->Text, this->tboxAccessOther->Text, nullptr) );
		section->AppendChild(tag);
	}
	root->AppendChild(section);

	// DSi�E�F�A���
	section = doc->CreateElement("dsi-ware-info");
	MasterEditorTWL::appendXmlTag( doc, section, "index", this->isJapanese()?"DSi�E�F�A���":"DSiWare Info" );
	{
		System::Xml::XmlElement ^tag = doc->CreateElement("info-list");
		tag->AppendChild( CreateRomInfoListElement(doc, this->labMedia->Text, this->tboxMedia->Text, nullptr) );
		if( this->hSrl->IsNAND )	// �J�[�h�A�v���ł͕s�v�ȏ��
		{
			tag->AppendChild( CreateRomInfoListElement(doc, this->labSrlSize->Text, this->tboxSrlSizeFS->Text, nullptr) );
			tag->AppendChild( CreateRomInfoListElement(doc, this->labPublicSize->Text, this->tboxPublicSizeFS->Text, nullptr) );
			tag->AppendChild( CreateRomInfoListElement(doc, this->labPrivateSize->Text, this->tboxPrivateSizeFS->Text, nullptr) );
			tag->AppendChild( CreateRomInfoListElement(doc, this->labSubBannerSize->Text, this->tboxSubBannerSizeFS->Text, nullptr) );
			tag->AppendChild( CreateRomInfoListElement(doc, this->labTmdSize->Text, this->tboxTmdSizeFS->Text, nullptr) );
			tag->AppendChild( CreateRomInfoListElement(doc, this->labSumSize->Text + " " + this->labSumSize2->Text,
														this->tboxSumSize->Text + " (" + this->tboxSumSizeMB->Text + ")", nullptr) );
		}
		section->AppendChild(tag);
	}
	root->AppendChild(section);

	// ���[�W�����ƃ��[�e�B���O
	section = doc->CreateElement("market-and-rating-info");
	MasterEditorTWL::appendXmlTag( doc, section, "index", this->isJapanese()?"���[�W�����ƃ��[�e�B���O":"Market and Rating" );
	{
		System::Xml::XmlElement ^tag = doc->CreateElement("info-list");

		// ���[�W����
		System::String ^region = "";
		if( this->combRegion->DropDownStyle == System::Windows::Forms::ComboBoxStyle::DropDown )
		{
			// ���؂̂Ƃ��̓��X�g�őI��s�̃e�L�X�g���R���{�{�b�N�X�ɏ�����Ă���̂ł����o�^
			region = this->combRegion->Text;
		}
		else
		{
			// WorldWide �̂Ƃ��̓��X�g�őI���\�ȕ������o�^
			if( this->combRegion->SelectedIndex < 0 )
			{
				region = this->isJapanese()?METWL_STRING_UNDEFINED_REGION_J:METWL_STRING_UNDEFINED_REGION_E;	// �N���蓾�Ȃ�
			}
			else
			{
				// ���X�g�őI������Ă���e�L�X�g��o�^
				region = dynamic_cast<System::String^>(this->combRegion->SelectedItem);
			}
		}
		tag->AppendChild( CreateRomInfoListElement(doc, this->labRegion->Text, region, nullptr) );
		section->AppendChild(tag);

		// ���[�e�B���O
		tag->AppendChild( CreateRomInfoListElement(doc, this->labCERO->Text, 
							this->setDeliverableRatingOgnProperties(this->combCERO), nullptr) );
		tag->AppendChild( CreateRomInfoListElement(doc, this->labESRB->Text, 
							this->setDeliverableRatingOgnProperties(this->combESRB), nullptr) );
		tag->AppendChild( CreateRomInfoListElement(doc, this->labUSK->Text, 
							this->setDeliverableRatingOgnProperties(this->combUSK), nullptr) );
		tag->AppendChild( CreateRomInfoListElement(doc, this->labPEGI->Text, 
							this->setDeliverableRatingOgnProperties(this->combPEGI), nullptr) );
		tag->AppendChild( CreateRomInfoListElement(doc, this->labPEGI_PRT->Text, 
							this->setDeliverableRatingOgnProperties(this->combPEGI_PRT), nullptr) );
		tag->AppendChild( CreateRomInfoListElement(doc, this->labPEGI_BBFC->Text, 
							this->setDeliverableRatingOgnProperties(this->combPEGI_BBFC), nullptr) );
		tag->AppendChild( CreateRomInfoListElement(doc, this->labOFLC->Text, 
							this->setDeliverableRatingOgnProperties(this->combOFLC), nullptr) );
		tag->AppendChild( CreateRomInfoListElement(doc, this->labGRB->Text, 
							this->setDeliverableRatingOgnProperties(this->combGRB), nullptr) );
		section->AppendChild(tag);
	}
	root->AppendChild(section);

	//
	// SDK�o�[�W�����͓Ɨ������\
	//
	section = doc->CreateElement("sdk-version-info");
	MasterEditorTWL::appendXmlTag( doc, section, "index", this->isJapanese()?"SDK�o�[�W����":"SDK Version" );
	{
		System::Xml::XmlElement ^tag = doc->CreateElement("sdk-version-list");
		for each( RCSDKVersion ^sdk in this->hSrl->hSDKList )
		{
			tag->AppendChild( CreateSDKVersionListElement(doc, sdk->Version, sdk->IsStatic) );
		}
		section->AppendChild(tag);
	}
	root->AppendChild(section);

	//
	// �~�h���E�F�A���X�g���Ɨ������\
	//
	section = doc->CreateElement("middleware-info");
	MasterEditorTWL::appendXmlTag( doc, section, "index", this->isJapanese()?"�g�p���C�u����":"Libraries" );
	section->AppendChild( this->makeMiddlewareListXmlElement(doc) );
	root->AppendChild(section);

	//
	// �G���[�����Ɨ������\
	//
	if( withError )
	{
		section = doc->CreateElement("error-info");
		MasterEditorTWL::appendXmlTag( doc, section, "index", this->isJapanese()?"�G���[":"Error" );
		section->AppendChild( this->makeErrorListXmlElement(doc, isCurrent) );
		section->AppendChild( this->makeErrorListCaptionXmlElement(doc) );
		root->AppendChild(section);

		section = doc->CreateElement("warning-info");
		MasterEditorTWL::appendXmlTag( doc, section, "index", this->isJapanese()?"�x��":"Warning" );
		section->AppendChild( this->makeWarningListXmlElement(doc, isCurrent) );
		section->AppendChild( this->makeErrorListCaptionXmlElement(doc) );
		root->AppendChild(section);
	}

} //Form1::makeRomInfoListXml

// ======================================================
// HTML�`���Ńt�@�C�����o��
// ======================================================
void Form1::makeRomInfoListHtml(System::String ^filepath, System::Boolean withError, System::Boolean isCurrent)
{
	System::String ^tmpxml = gcnew System::String( this->getXmlToHtmlTmpFile() );
	try
	{
		// TMP�t�@�C����XML�ō쐬
		System::Xml::XmlDocument ^doc = gcnew System::Xml::XmlDocument();
		doc->AppendChild( doc->CreateXmlDeclaration("1.0","UTF-8",nullptr) );
		this->makeRomInfoListXml( doc, withError, isCurrent );
		doc->Save( tmpxml );
		// XML=>HTML�ϊ�
		System::Xml::Xsl::XslCompiledTransform ^xslt = gcnew System::Xml::Xsl::XslCompiledTransform;
		xslt->Load( this->getRomInfoListXsl() );
		xslt->Transform( tmpxml, filepath );
	}
	catch (System::Exception ^s)
	{
		System::IO::File::Delete( tmpxml );	// ���̂��߂�TMP�t�@�C�����폜���Ă���
		throw s;	// �Ăяo�����ɗ�O����throw
		return;
	}
	System::IO::File::Delete( tmpxml );

} //Form1::makeRomInfoListHtml(System::String ^filepath)

