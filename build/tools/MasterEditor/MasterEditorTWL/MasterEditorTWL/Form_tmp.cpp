// ----------------------------------------------
// �ꎞ�t�@�C���̎�舵��
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

// �ꎞ�ۑ�
System::Void Form1::saveTmp( System::String ^filename )
{
	System::Xml::XmlDocument ^doc = gcnew System::Xml::XmlDocument();

	doc->AppendChild( doc->CreateXmlDeclaration("1.0","UTF-8",nullptr) );
	System::Xml::XmlElement ^root = doc->CreateElement( "MasterEditorTWL" );
	doc->AppendChild( root );

	// SRL�̃p�X
	MasterEditorTWL::appendXmlTag( doc, root, "Srl", this->tboxFile->Text );

	// ����
	MasterEditorTWL::appendXmlTag( doc, root, "Lang", (this->stripItemJapanese->Checked)?"J":"E" );

	// �t�H�[��
	System::Xml::XmlElement ^form = doc->CreateElement( "Form" );
	root->AppendChild( form );

	MasterEditorTWL::appendXmlTag( doc, form, "ProductName", this->tboxProductName->Text );
	MasterEditorTWL::appendXmlTag( doc, form, "ProductCode1", this->tboxProductCode1->Text );
	MasterEditorTWL::appendXmlTag( doc, form, "ProductCode2", this->tboxProductCode2->Text );
	MasterEditorTWL::appendXmlTag( doc, form, "ProductNameForeign", this->tboxProductNameForeign->Text );
	MasterEditorTWL::appendXmlTag( doc, form, "ProductCode1Foreign", this->tboxProductCode1Foreign->Text );
	MasterEditorTWL::appendXmlTag( doc, form, "ProductCode2Foreign1", this->tboxProductCode2Foreign1->Text );
	MasterEditorTWL::appendXmlTag( doc, form, "ProductCode2Foreign2", this->tboxProductCode2Foreign2->Text );
	MasterEditorTWL::appendXmlTag( doc, form, "ProductCode2Foreign3", this->tboxProductCode2Foreign3->Text );
	MasterEditorTWL::appendXmlTag( doc, form, "SubmitVersion", this->numSubmitVersion->Value.ToString() );
	MasterEditorTWL::appendXmlTag( doc, form, "Backup", this->combBackup->SelectedIndex.ToString() );
	MasterEditorTWL::appendXmlTag( doc, form, "BackupOther", this->tboxBackupOther->Text );
	MasterEditorTWL::appendXmlTag( doc, form, "ReleaseForeign", (this->cboxReleaseForeign->Checked)?"Y":"N" );
	MasterEditorTWL::appendXmlTag( doc, form, "Remarks", this->tboxCaption->Text );
	MasterEditorTWL::appendXmlTag( doc, form, "SubmitWay", (this->rSubmitHand->Checked)?"Hand":"Mail" );
	if( this->rUsageSale->Checked )
		MasterEditorTWL::appendXmlTag( doc, form, "Purpose", "Sale" );
	else if( this->rUsageSample->Checked )
		MasterEditorTWL::appendXmlTag( doc, form, "Purpose", "Sample" );
	else if( this->rUsageDst->Checked )
		MasterEditorTWL::appendXmlTag( doc, form, "Purpose", "Dst" );
	else if( this->rUsageOther->Checked )
		MasterEditorTWL::appendXmlTag( doc, form, "Purpose", "Other" );
	MasterEditorTWL::appendXmlTag( doc, form, "PurposeOther", this->tboxUsageOther->Text );
	MasterEditorTWL::appendXmlTag( doc, form, "ReleaseDate", this->dateRelease->Value.ToString() );
	MasterEditorTWL::appendXmlTag( doc, form, "SubmitDate", this->dateSubmit->Value.ToString() );

	MasterEditorTWL::appendXmlTag( doc, form, "Company1", this->tboxCompany1->Text );
	MasterEditorTWL::appendXmlTag( doc, form, "Depart1", this->tboxDepart1->Text );
	MasterEditorTWL::appendXmlTag( doc, form, "Name1", this->tboxPerson1->Text );
	MasterEditorTWL::appendXmlTag( doc, form, "Furigana1", this->tboxFurigana1->Text );
	MasterEditorTWL::appendXmlTag( doc, form, "Tel1", this->tboxTel1->Text );
	MasterEditorTWL::appendXmlTag( doc, form, "Fax1", this->tboxFax1->Text );
	MasterEditorTWL::appendXmlTag( doc, form, "Mail1", this->tboxMail1->Text );
	MasterEditorTWL::appendXmlTag( doc, form, "NTSC1", this->tboxNTSC1->Text );
	MasterEditorTWL::appendXmlTag( doc, form, "InputPerson2", (this->cboxIsInputPerson2->Checked)?"Y":"N" );
	MasterEditorTWL::appendXmlTag( doc, form, "Company2", this->tboxCompany2->Text );
	MasterEditorTWL::appendXmlTag( doc, form, "Depart2", this->tboxDepart2->Text );
	MasterEditorTWL::appendXmlTag( doc, form, "Name2", this->tboxPerson2->Text );
	MasterEditorTWL::appendXmlTag( doc, form, "Furigana2", this->tboxFurigana2->Text );
	MasterEditorTWL::appendXmlTag( doc, form, "Tel2", this->tboxTel2->Text );
	MasterEditorTWL::appendXmlTag( doc, form, "Fax2", this->tboxFax2->Text );
	MasterEditorTWL::appendXmlTag( doc, form, "Mail2", this->tboxMail2->Text );
	MasterEditorTWL::appendXmlTag( doc, form, "NTSC2", this->tboxNTSC2->Text );

	if( this->rIsWirelessIcon->Checked )
		MasterEditorTWL::appendXmlTag( doc, form, "DisplayIcon", "Wireless" );
	else if( this->rIsWiFiIcon->Checked )
		MasterEditorTWL::appendXmlTag( doc, form, "DisplayIcon", "WiFi" );
	else
		MasterEditorTWL::appendXmlTag( doc, form, "DisplayIcon", "None" );

	MasterEditorTWL::appendXmlTag( doc, form, "IsEULA", (this->cboxIsEULA->Checked)?"Y":"N" );
	MasterEditorTWL::appendXmlTag( doc, form, "Region", this->combRegion->SelectedIndex.ToString() );
	MasterEditorTWL::appendXmlTag( doc, form, "RatingCERO", this->combCERO->SelectedIndex.ToString() );
	MasterEditorTWL::appendXmlTag( doc, form, "RatingESRB", this->combESRB->SelectedIndex.ToString() );
	MasterEditorTWL::appendXmlTag( doc, form, "RatingUSK", this->combUSK->SelectedIndex.ToString() );
	MasterEditorTWL::appendXmlTag( doc, form, "RatingPEGI", this->combPEGI->SelectedIndex.ToString() );
	MasterEditorTWL::appendXmlTag( doc, form, "RatingPEGI_PRT", this->combPEGI_PRT->SelectedIndex.ToString() );
	MasterEditorTWL::appendXmlTag( doc, form, "RatingPEGI_BBFC", this->combPEGI_BBFC->SelectedIndex.ToString() );
	MasterEditorTWL::appendXmlTag( doc, form, "RatingOFLC", this->combOFLC->SelectedIndex.ToString() );

	MasterEditorTWL::appendXmlTag( doc, form, "IsUGC", (this->cboxIsUGC->Checked)?"Y":"N" );
	MasterEditorTWL::appendXmlTag( doc, form, "IsPhotoEx", (this->cboxIsPhotoEx->Checked)?"Y":"N" );

	try
	{
		doc->Save( filename );
	}
	catch( System::Exception ^ex )
	{
		(void)ex;
		this->errMsg( "�ꎞ���̕ۑ��Ɏ��s���܂����B", "Saving a temporary file failed." );
	}
} //saveTmp()

// �ꎞ�t�@�C���̓ǂݏo��
void Form1::loadTmp( System::String ^filename )
{
	System::Xml::XmlDocument ^doc = gcnew System::Xml::XmlDocument;
	try
	{
		doc->Load( filename );
	}
	catch( System::Exception ^ex )
	{
		(void)ex;
		this->errMsg( "�ꎞ���̓ǂݍ��݂Ɏ��s���܂����B", "Loading a temporary file failed." );
	}
	System::Xml::XmlElement  ^root = doc->DocumentElement;
	System::String ^text;

	// SRL
	text = MasterEditorTWL::getXPathText( root, "/MasterEditorTWL/Srl" );
	if( !System::String::IsNullOrEmpty(text) )		// SRL�t�@�C�������Ȃ��Ƃ��̓X���[
	{
		this->loadSrl(text);
	}

	// ����
	text = MasterEditorTWL::getXPathText( root, "/MasterEditorTWL/Lang" );
	if( !System::String::IsNullOrEmpty(text) && text->Equals("E") )
	{
		this->stripItemEnglish->Checked  = true;
		this->stripItemJapanese->Checked = false;
		this->changeEnglish();
	}
	else
	{
		this->stripItemEnglish->Checked  = false;
		this->stripItemJapanese->Checked = true;
		this->changeJapanese();
	}

	// �t�H�[��
	this->parseTmp( root, "/MasterEditorTWL/Form/ProductName", this->tboxProductName );
	this->parseTmp( root, "/MasterEditorTWL/Form/ProductCode1", this->tboxProductCode1 );
	this->parseTmp( root, "/MasterEditorTWL/Form/ProductCode2", this->tboxProductCode2 );
	this->parseTmp( root, "/MasterEditorTWL/Form/SubmitVersion", this->numSubmitVersion );
	this->parseTmp( root, "/MasterEditorTWL/Form/Backup", this->combBackup );
	this->parseTmp( root, "/MasterEditorTWL/Form/ReleaseDate", this->dateRelease );
	this->parseTmp( root, "/MasterEditorTWL/Form/SubmitDate",  this->dateSubmit );

	this->tboxBackupOther->Enabled = false;
	this->tboxBackupOther->Clear();
	if( this->combBackup->SelectedIndex == (this->combBackup->Items->Count - 1) )
	{
		this->parseTmp( root, "/MasterEditorTWL/Form/BackupOther", this->tboxBackupOther );
		this->tboxBackupOther->Enabled = true;
	}

	cli::array<System::Windows::Forms::RadioButton^> ^rbuts;
	cli::array<System::String^> ^strs;

	rbuts = gcnew cli::array<System::Windows::Forms::RadioButton^>{this->rSubmitHand, this->rSubmitPost};
	strs  = gcnew cli::array<System::String^>{"Hand","Mail"};
	this->parseTmp( root, "/MasterEditorTWL/Form/SubmitWay", rbuts, strs );

	rbuts = gcnew cli::array<System::Windows::Forms::RadioButton^>{this->rUsageSale, this->rUsageSample, this->rUsageDst, this->rUsageOther};
	strs  = gcnew cli::array<System::String^>{"Sale","Sample","Dst","Other"};
	this->parseTmp( root, "/MasterEditorTWL/Form/Purpose", rbuts, strs );
	this->tboxUsageOther->Enabled = false;
	this->tboxUsageOther->Clear();
	if( this->rUsageOther->Checked )
	{
		this->tboxUsageOther->Enabled = true;
		this->parseTmp( root, "/MasterEditorTWL/Form/PurposeOther", this->tboxUsageOther );
	}

	this->parseTmp( root, "/MasterEditorTWL/Form/ReleaseForeign", this->cboxReleaseForeign );
	this->tboxProductNameForeign->Enabled   = false;
	this->tboxProductCode1Foreign->Enabled  = false;
	this->tboxProductCode2Foreign1->Enabled = false;
	this->tboxProductCode2Foreign2->Enabled = false;
	this->tboxProductCode2Foreign3->Enabled = false;
	this->tboxProductNameForeign->Clear();
	this->tboxProductCode1Foreign->Clear();
	this->tboxProductCode2Foreign1->Clear();
	this->tboxProductCode2Foreign2->Clear();
	this->tboxProductCode2Foreign3->Clear();
	if( this->cboxReleaseForeign->Checked )
	{
		this->tboxProductNameForeign->Enabled   = true;
		this->tboxProductCode1Foreign->Enabled  = true;
		this->tboxProductCode2Foreign1->Enabled = true;
		this->tboxProductCode2Foreign2->Enabled = true;
		this->tboxProductCode2Foreign3->Enabled = true;
		this->parseTmp( root, "/MasterEditorTWL/Form/ProductNameForeign", this->tboxProductNameForeign );
		this->parseTmp( root, "/MasterEditorTWL/Form/ProductCode1Foreign", this->tboxProductCode1Foreign );
		this->parseTmp( root, "/MasterEditorTWL/Form/ProductCode2Foreign1", this->tboxProductCode2Foreign1 );
		this->parseTmp( root, "/MasterEditorTWL/Form/ProductCode2Foreign2", this->tboxProductCode2Foreign2 );
		this->parseTmp( root, "/MasterEditorTWL/Form/ProductCode2Foreign3", this->tboxProductCode2Foreign3 );
	}
	this->parseTmp( root, "/MasterEditorTWL/Form/Remarks", this->tboxCaption );

	this->parseTmp( root, "/MasterEditorTWL/Form/Company1", this->tboxCompany1 );
	this->parseTmp( root, "/MasterEditorTWL/Form/Depart1", this->tboxDepart1 );
	this->parseTmp( root, "/MasterEditorTWL/Form/Name1", this->tboxPerson1 );
	this->parseTmp( root, "/MasterEditorTWL/Form/Furigana1", this->tboxFurigana1 );
	this->parseTmp( root, "/MasterEditorTWL/Form/Tel1", this->tboxTel1 );
	this->parseTmp( root, "/MasterEditorTWL/Form/Fax1", this->tboxFax1 );
	this->parseTmp( root, "/MasterEditorTWL/Form/Mail1", this->tboxMail1 );
	this->parseTmp( root, "/MasterEditorTWL/Form/NTSC1", this->tboxNTSC1 );
	this->parseTmp( root, "/MasterEditorTWL/Form/InputPerson2", this->cboxIsInputPerson2 );

	this->parseTmp( root, "/MasterEditorTWL/Form/Company2", this->tboxCompany2 );
	this->parseTmp( root, "/MasterEditorTWL/Form/Depart2", this->tboxDepart2 );
	this->parseTmp( root, "/MasterEditorTWL/Form/Name2", this->tboxPerson2 );
	this->parseTmp( root, "/MasterEditorTWL/Form/Furigana2", this->tboxFurigana2 );
	this->parseTmp( root, "/MasterEditorTWL/Form/Tel2", this->tboxTel2 );
	this->parseTmp( root, "/MasterEditorTWL/Form/Fax2", this->tboxFax2 );
	this->parseTmp( root, "/MasterEditorTWL/Form/Mail2", this->tboxMail2 );
	this->parseTmp( root, "/MasterEditorTWL/Form/NTSC2", this->tboxNTSC2 );

	rbuts = gcnew cli::array<System::Windows::Forms::RadioButton^>{this->rIsWirelessIcon, this->rIsWiFiIcon, this->rIsNoIcon};
	strs  = gcnew cli::array<System::String^>{"Wireless","WiFi","None"};
	this->parseTmp( root, "/MasterEditorTWL/Form/DisplayIcon", rbuts, strs );

	this->parseTmp( root, "/MasterEditorTWL/Form/IsEULA", this->cboxIsEULA );
	this->parseTmp( root, "/MasterEditorTWL/Form/Region", this->combRegion );
	this->parseTmp( root, "/MasterEditorTWL/Form/RatingCERO", this->combCERO );
	this->parseTmp( root, "/MasterEditorTWL/Form/RatingESRB", this->combESRB );
	this->parseTmp( root, "/MasterEditorTWL/Form/RatingUSK", this->combUSK );
	this->parseTmp( root, "/MasterEditorTWL/Form/RatingPEGI", this->combPEGI );
	this->parseTmp( root, "/MasterEditorTWL/Form/RatingPEGI_PRT", this->combPEGI_PRT );
	this->parseTmp( root, "/MasterEditorTWL/Form/RatingPEGI_BBFC", this->combPEGI_BBFC );
	this->parseTmp( root, "/MasterEditorTWL/Form/RatingOFLC", this->combOFLC );

	this->parseTmp( root, "/MasterEditorTWL/Form/IsUGC", this->cboxIsUGC );
	this->parseTmp( root, "/MasterEditorTWL/Form/IsPhotoEx", this->cboxIsPhotoEx );

	this->maskParentalForms();	// �y�A�����^���R���g���[���������[�W�����ɍ��킹��

} //loadTmp()

// �ꎞ�ۑ������t�H�[�����ɕϊ�
System::Boolean Form1::parseTmp( System::Xml::XmlElement ^root, System::String ^xpath, System::Windows::Forms::ComboBox ^comb )
{
	// �R���{�{�b�N�X�̕ۑ���� - �C���f�b�N�X�͈̔͂Ɋ܂܂�邩���ׂ�(�܂܂�Ȃ��ꍇ��-1)
	System::String ^text = MasterEditorTWL::getXPathText( root, xpath );
	if( System::String::IsNullOrEmpty( text ) )
		return false;

	try
	{
		System::Int32 index = System::Int32::Parse( text );	// �e�L�X�g�ɂ̓C���f�b�N�X���ۑ�����Ă���
		if( (0 <= index) && (index < comb->Items->Count) )
		{
			comb->SelectedIndex = index;
			return true;
		}
		else
		{
			comb->SelectedIndex = -1;
			return false;
		}
	}
	catch ( System::Exception ^ex )	// �����񂳂�Ă����Ƃ�
	{
		(void)ex;
		comb->SelectedIndex = -1;
		return false;
	}
	return false;
}
System::Boolean Form1::parseTmp( System::Xml::XmlElement ^root, System::String ^xpath, System::Windows::Forms::NumericUpDown ^num )
{
	// �e�L�X�g�{�b�N�X�̕ۑ���� - ���̂܂ܑ��(�������ő�𒴂���ꍇ��0�ɂ���)
	System::String ^text = MasterEditorTWL::getXPathText( root, xpath );
	if( System::String::IsNullOrEmpty( text ) )
		return false;

	try
	{
		System::Int32 val = System::Int32::Parse( text );	// �e�L�X�g�ɂ͒l���ۑ�����Ă���
		if( (0 <= val) && (val <= num->Maximum) )
		{
			num->Value = val;
			return true;
		}
		else
		{
			num->Value = 0;
			return false;
		}
	}
	catch ( System::Exception ^ex )	// �����񂳂�Ă����Ƃ�
	{
		(void)ex;
		num->Value = 0;
		return false;
	}
	return false;
}
System::Boolean Form1::parseTmp( System::Xml::XmlElement ^root, System::String ^xpath, 
						         cli::array<System::Windows::Forms::RadioButton^>^rbuts, cli::array<System::String ^>^textCands )
{
	// ���W�I�{�^���̕ۑ���� - �e�L�X�g���ɍ��v���邩���ׂ�(���v���Ȃ��ꍇ�͐擪�̃��W�I�{�^�����`�F�b�N)
	System::String ^text = MasterEditorTWL::getXPathText( root, xpath );
	if( System::String::IsNullOrEmpty( text ) )
		return false;

	System::Int32 i;
	System::Boolean bSearch = false;
	for( i=0; i < rbuts->Length; i++ )	// XML���̃e�L�X�g���ǂ̃��W�I�{�^���̃e�L�X�g�ɍ��v���邩�`�F�b�N
	{
		rbuts[i]->Checked = false;
		if( textCands[i]->Equals( text ) )
		{
			rbuts[i]->Checked = true;
			bSearch = true;
		}
	}
	if( !bSearch )	// ���v���Ȃ��ꍇ�͍ŏ��̃��W�I�{�^�����`�F�b�N
	{
		rbuts[0]->Checked = true;
	}
	return bSearch;
}
System::Boolean Form1::parseTmp( System::Xml::XmlElement ^root, System::String ^xpath, System::Windows::Forms::CheckBox ^cbox )
{
	// �`�F�b�N�{�^���̕ۑ���� - Y/N�𒲂ׂ�(����ȊO�̏ꍇ�ɂ̓`�F�b�N���Ȃ�)
	System::String ^text = MasterEditorTWL::getXPathText( root, xpath );
	if( System::String::IsNullOrEmpty( text ) )
		return false;

	if( text->Equals("Y") )
		cbox->Checked = true;
	else
		cbox->Checked = false;	// �s���Ȓl���܂�
	return cbox->Checked;
}
System::Boolean Form1::parseTmp( System::Xml::XmlElement ^root, System::String ^xpath, System::Windows::Forms::TextBox ^tbox )
{
	// �e�L�X�g�{�b�N�X�̕ۑ���� - ���̂܂ܑ��(����������������Ȃ��ꍇ�͋󔒂ɂ���)
	System::String ^text = MasterEditorTWL::getXPathText( root, xpath );
	if( System::String::IsNullOrEmpty( text ) )
		return false;

	if( text->Length > tbox->MaxLength )
	{
		tbox->Text = gcnew System::String("");
		return false;
	}
	tbox->Text = text;
	return true;
}
System::Boolean Form1::parseTmp( System::Xml::XmlElement ^root, System::String ^xpath, System::Windows::Forms::DateTimePicker ^date )
{
	System::String ^text = MasterEditorTWL::getXPathText( root, xpath );
	if( System::String::IsNullOrEmpty( text ) )
		return false;

	System::Diagnostics::Debug::WriteLine( text );

	try
	{
		date->Value = System::DateTime::Parse( text );
	}
	catch( System::Exception ^ex )
	{
		(void)ex;
		date->Value = System::DateTime::Now;
		return false;
	}
	return true;
}
