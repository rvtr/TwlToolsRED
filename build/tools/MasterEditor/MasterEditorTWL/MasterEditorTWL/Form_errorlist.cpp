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

// ======================================================
// XML�̈�v�f���쐬
// ======================================================
static System::Xml::XmlElement^ CreateErrorListElement(System::Xml::XmlDocument ^doc, RCMrcError ^err, int errtype, 
													   System::String ^tagname, System::Boolean isJapanese)
{
	System::Xml::XmlElement ^tag = doc->CreateElement(tagname);
	MasterEditorTWL::appendXmlTag( doc, tag, "errname",  (isJapanese)?err->Name:err->NameE );
	MasterEditorTWL::appendXmlTag( doc, tag, "errtype",  errtype.ToString() );
	MasterEditorTWL::appendXmlTag( doc, tag, "errbegin", err->Begin.ToString("X04")+"h" );
	MasterEditorTWL::appendXmlTag( doc, tag, "errend",   err->End.ToString("X04")+"h" );
	MasterEditorTWL::appendXmlTag( doc, tag, "errmsg",   (isJapanese)?err->Msg:err->MsgE );
	return tag;
}

// ======================================================
// XML�`���̃��X�g�̖{�̂ƂȂ�G���[���X�g�̍쐬(���ł��g���̂œƗ�������)
// ======================================================
System::Xml::XmlElement^ Form1::makeErrorListXmlElement(System::Xml::XmlDocument ^doc, System::Boolean isCurrent)
{
	// �G���[���X�g
	System::Xml::XmlElement ^list = doc->CreateElement( "error-list" );
	if( isCurrent )	// ���݂̓��͂𔽉f
	{
		if( this->hSrl->hErrorList != nullptr )		// ROM�ǂݍ��ݎ��ɔ������ꂽ�G���[
		{
			for each( RCMrcError ^err in this->hSrl->hErrorList )
			{
				if( !err->IsEnableModify )	// �C���\�ȏ��͏o�͂��Ȃ�
				{
					list->AppendChild( CreateErrorListElement(doc, err, 0, "error", this->isJapanese()) );
				}
			}
		}
		if( this->hErrorList != nullptr )	// ���݂̃t�H�[�����͂��甭�����ꂽ�G���[
		{
			for each( RCMrcError ^err in this->hErrorList )
			{
				if( err->IsAffectRom )	// ���ޏ�̃G���[�͏o�͂��Ȃ�
				{
					list->AppendChild( CreateErrorListElement(doc, err, 1, "error", this->isJapanese()) );
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
				int errtype = 0;			// �C���\�ȏ��ƕs�\�ȏ��ƂŃ^�C�v�𕪂���
				if( err->IsEnableModify )
				{
					errtype = 1;
				}
				list->AppendChild( CreateErrorListElement(doc, err, errtype, "error", this->isJapanese()) );
			}
		}
	}
	return list;
}//System::Xml::XmlElement^ Form1::makeErrorListXmlElement

System::Xml::XmlElement^ Form1::makeWarningListXmlElement(System::Xml::XmlDocument ^doc, System::Boolean isCurrent)
{
	// �x�����X�g
	System::Xml::XmlElement ^list = doc->CreateElement( "warning-list" );
	if( isCurrent )	// ���݂̓��͂𔽉f
	{
		if( this->hSrl->hWarnList != nullptr )		// ROM�ǂݍ��ݎ��ɔ������ꂽ�G���[
		{
			for each( RCMrcError ^err in this->hSrl->hWarnList )
			{
				if( !err->IsEnableModify )	// �C���\�ȏ��͏o�͂��Ȃ�
				{
					list->AppendChild( CreateErrorListElement(doc, err, 0, "warning", this->isJapanese()) );
				}
			}
		}
		if( this->hWarnList != nullptr )	// ���݂̃t�H�[�����͂��甭�����ꂽ�G���[
		{
			for each( RCMrcError ^err in this->hWarnList )
			{
				if( err->IsAffectRom )	// ���ޏ�̃G���[�͏o�͂��Ȃ�
				{
					list->AppendChild( CreateErrorListElement(doc, err, 1, "warning", this->isJapanese()) );
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
				int errtype = 0;			// �C���\�ȏ��ƕs�\�ȏ��ƂŃ^�C�v�𕪂���
				if( err->IsEnableModify )
				{
					errtype = 1;
				}
				list->AppendChild( CreateErrorListElement(doc, err, errtype, "warning", this->isJapanese()) );
			}
		}
	}
	return list;
} //System::Xml::XmlElement^ Form1::makeWarningListXmlElement

System::Xml::XmlElement^ Form1::makeErrorListCaptionXmlElement(System::Xml::XmlDocument ^doc)
{
	// �x�����X�g
	System::Xml::XmlElement ^list = doc->CreateElement( "error-caption-list" );
	{
		System::Xml::XmlElement ^tag = doc->CreateElement( "error-caption" );
		System::String ^msg = "* Type 0 indicates problems with the ROM data that cannot be corrected using MasterEditorTWL.";
		if(this->isJapanese())
		{
			msg = "* Type 0 �̍��ڂ̓}�X�^�[�G�f�B�^�ŏC���ł��܂���B";
		}
		MasterEditorTWL::appendXmlTag( doc, tag, "text", msg );
		list->AppendChild(tag);
	}
	{
		System::Xml::XmlElement ^tag = doc->CreateElement( "error-caption" );
		System::String ^msg = "* Type 1 indicates problems with the ROM data that can be corrected using MasterEditorTWL.";
		if(this->isJapanese())
		{
			msg = "* Type 1 �̍��ڂ̓}�X�^�[�G�f�B�^�ŏC���\�ł��B";
		}
		MasterEditorTWL::appendXmlTag( doc, tag, "text", msg );
		list->AppendChild(tag);
	}
	return list;
} //System::Xml::XmlElement^ Form1::makeErrorListCaptionXmlElement


// ======================================================
// XML�`���Ń��X�g���쐬
// ======================================================
System::Void Form1::makeErrorListXml(System::Xml::XmlDocument ^doc, System::Boolean isCurrent)
{
	// �����ŐV�ɍX�V
	this->hErrorList->Clear();
	this->hWarnList->Clear();
	this->checkSrlForms();
	this->checkDeliverableForms();

	System::Xml::XmlElement ^root = doc->CreateElement( "twl-master-editor" );
	System::Reflection::Assembly ^ass = System::Reflection::Assembly::GetEntryAssembly();
	root->SetAttribute( "version", this->getVersion() );
	doc->AppendChild( root );

	// �Q�[�����
	root->AppendChild( this->makeGameInfoXmlElement(doc) );

	// �G���[���
	root->AppendChild( this->makeErrorListXmlElement(doc, isCurrent) );

	// �x�����
	root->AppendChild( this->makeWarningListXmlElement(doc, isCurrent) );

	// �ǋL
	root->AppendChild( this->makeErrorListCaptionXmlElement(doc) );

} //System::Void Form1::makeErrorListXml

// ======================================================
// HTML�`���Ńt�@�C�����o��
// ======================================================
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