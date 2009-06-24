// ----------------------------------------------
// �G���[���X�g�̏����o��
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

static System::Xml::XmlElement^ CreateErrorListElement(System::Xml::XmlDocument ^doc, RCMrcError ^err, int errtype, 
													   System::String ^tagname, System::Boolean isJapanese)
{
	System::Xml::XmlElement ^tag = doc->CreateElement(tagname);
	MasterEditorTWL::appendXmlTag( doc, tag, "name",  (isJapanese)?err->Name:err->NameE );
	MasterEditorTWL::appendXmlTag( doc, tag, "type",  errtype.ToString() );
	MasterEditorTWL::appendXmlTag( doc, tag, "begin", err->Begin.ToString("X04")+"h" );
	MasterEditorTWL::appendXmlTag( doc, tag, "end",   err->End.ToString("X04")+"h" );
	MasterEditorTWL::appendXmlTag( doc, tag, "msg",   (isJapanese)?err->Msg:err->MsgE );
	return tag;
}

// XML�`���Ń��X�g���쐬
System::Void Form1::makeErrorListXml(System::Xml::XmlDocument ^doc, System::Boolean isCurrent)
{
	System::Xml::XmlElement ^root = doc->CreateElement( "twl-master-editor" );
	System::Reflection::Assembly ^ass = System::Reflection::Assembly::GetEntryAssembly();
	root->SetAttribute( "version", this->getVersion() );
	doc->AppendChild( root );

	// �Q�[�����
	System::Xml::XmlElement ^game = doc->CreateElement( "game" );
	root->AppendChild( game );
	if( System::String::IsNullOrEmpty( this->tboxProductName->Text ) )
	{
		MasterEditorTWL::appendXmlTag( doc, game, "product-name", this->tboxTitleName->Text );	// ���i���������͂̂Ƃ��̓\�t�g�^�C�g���ő�p
	}
	else
	{
		MasterEditorTWL::appendXmlTag( doc, game, "product-name", this->tboxProductName->Text );
	}
	MasterEditorTWL::appendXmlTag( doc, game, "title-name",   this->tboxTitleName->Text );
	MasterEditorTWL::appendXmlTag( doc, game, "game-code",    this->tboxGameCode->Text );
	MasterEditorTWL::appendXmlTag( doc, game, "rom-version",  this->tboxRemasterVer->Text );
	MasterEditorTWL::appendXmlTag( doc, game, "submit-version", System::Decimal::ToByte(this->numSubmitVersion->Value).ToString("X") );

	// �G���[���X�g
	System::Xml::XmlElement ^errorlist = doc->CreateElement( "error-list" );
	root->AppendChild( errorlist );
	if( isCurrent )	// ���݂̓��͂𔽉f
	{
		if( this->hSrl->hErrorList != nullptr )		// ROM�ǂݍ��ݎ��ɔ������ꂽ�G���[
		{
			for each( RCMrcError ^err in this->hSrl->hErrorList )
			{
				if( !err->IsEnableModify )	// �C���\�ȏ��͏o�͂��Ȃ�
				{
					errorlist->AppendChild( CreateErrorListElement(doc, err, 0, "error", this->isJapanese()) );
				}
			}
		}
		if( this->hErrorList != nullptr )	// ���݂̃t�H�[�����͂��甭�����ꂽ�G���[
		{
			for each( RCMrcError ^err in this->hErrorList )
			{
				if( err->IsAffectRom )	// ���ޏ�̃G���[�͏o�͂��Ȃ�
				{
					errorlist->AppendChild( CreateErrorListElement(doc, err, 1, "error", this->isJapanese()) );
				}
			}
		}
	}
	else	// ROM�ǂݍ��ݎ��̏��
	{
		if( this->hSrl->hErrorList )
		{
			for each( RCMrcError ^err in this->hSrl->hErrorList )
			{
				int errtype = 0;
				if( err->IsEnableModify )
				{
					errtype = 1;
				}
				errorlist->AppendChild( CreateErrorListElement(doc, err, errtype, "error", this->isJapanese()) );
			}
		}
	}

	// �x�����X�g
	System::Xml::XmlElement ^warnlist = doc->CreateElement( "warning-list" );
	root->AppendChild( warnlist );
	if( isCurrent )	// ���݂̓��͂𔽉f
	{
		if( this->hSrl->hWarnList != nullptr )		// ROM�ǂݍ��ݎ��ɔ������ꂽ�G���[
		{
			for each( RCMrcError ^err in this->hSrl->hWarnList )
			{
				if( !err->IsEnableModify )	// �C���\�ȏ��͏o�͂��Ȃ�
				{
					warnlist->AppendChild( CreateErrorListElement(doc, err, 0, "warning", this->isJapanese()) );
				}
			}
		}
		if( this->hWarnList != nullptr )	// ���݂̃t�H�[�����͂��甭�����ꂽ�G���[
		{
			for each( RCMrcError ^err in this->hWarnList )
			{
				if( err->IsAffectRom )	// ���ޏ�̃G���[�͏o�͂��Ȃ�
				{
					warnlist->AppendChild( CreateErrorListElement(doc, err, 1, "warning", this->isJapanese()) );
				}
			}
		}
	}
	else	// ROM�ǂݍ��ݎ��̏��
	{
		if( this->hSrl->hWarnList )
		{
			for each( RCMrcError ^err in this->hSrl->hWarnList )
			{
				int errtype = 0;
				if( err->IsEnableModify )
				{
					errtype = 1;
				}
				warnlist->AppendChild( CreateErrorListElement(doc, err, errtype, "warning", this->isJapanese()) );
			}
		}
	}
} //System::Void Form1::makeErrorListXml

System::Void Form1::makeErrorListHtml(System::String ^filepath, System::Boolean isCurrent)
{
	System::String ^tmpxml = gcnew System::String( this->getXmlToHtmlTmpFile() );
	try
	{
		// TMP�t�@�C����XML�ō쐬
		System::Xml::XmlDocument ^doc = gcnew System::Xml::XmlDocument();
		doc->AppendChild( doc->CreateXmlDeclaration("1.0","UTF-8",nullptr) );
		this->makeErrorListXml( doc, isCurrent );
		doc->Save( tmpxml );
		// XML=>HTML�ϊ�
		System::Xml::Xsl::XslCompiledTransform ^xslt = gcnew System::Xml::Xsl::XslCompiledTransform;
		xslt->Load( this->getErrorListXsl() );
		xslt->Transform( tmpxml, filepath );
	}
	catch (System::Exception ^s)
	{
		System::IO::File::Delete( tmpxml );	// ���̂��߂�TMP�t�@�C�����폜���Ă���
		throw s;	// �Ăяo�����ɗ�O����throw
		return;
	}
	System::IO::File::Delete( tmpxml );
} //System::Void Form1::makeErrorListHtml