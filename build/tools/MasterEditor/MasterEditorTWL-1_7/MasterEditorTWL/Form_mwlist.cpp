// ----------------------------------------------
// ミドルウェアリストの書き出し
// ----------------------------------------------

#include "stdafx.h"
#include <apptype.h>
#include "common.h"
#include "srl.h"
#include "deliverable.h"
#include "crc_whole.h"
#include "utility.h"
#include "lang.h"
#include "split_tad.h"
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
// XML形式のリストに付属させるタイトル情報の作成
// ----------------------------------------------
System::Xml::XmlElement^ Form1::makeGameInfoXmlElement(System::Xml::XmlDocument ^doc)
{
	System::Xml::XmlElement ^game = doc->CreateElement( "game" );
	if( System::String::IsNullOrEmpty( this->tboxProductName->Text ) )
	{
		MasterEditorTWL::appendXmlTag( doc, game, "product-name", this->tboxTitleName->Text );	// 製品名が未入力のときはソフトタイトルで代用
	}
	else
	{
		MasterEditorTWL::appendXmlTag( doc, game, "product-name", this->tboxProductName->Text );
	}
	MasterEditorTWL::appendXmlTag( doc, game, "title-name",   this->tboxTitleName->Text );
	MasterEditorTWL::appendXmlTag( doc, game, "game-code",    this->tboxGameCode->Text );
	MasterEditorTWL::appendXmlTag( doc, game, "rom-version",  this->tboxRemasterVer->Text );
	MasterEditorTWL::appendXmlTag( doc, game, "submit-version", System::Decimal::ToByte(this->numSubmitVersion->Value).ToString("X") );
	return game;
}

// ----------------------------------------------
// XML形式のリストの本体となるミドルウェアリストの作成(他でも使うので独立させる)
// ----------------------------------------------
System::Xml::XmlElement^ Form1::makeMiddlewareListXmlElement(System::Xml::XmlDocument ^doc)
{
	System::Xml::XmlElement ^midlist = doc->CreateElement( "middleware-list" );
	if( this->hSrl->hLicenseList != nullptr )
	{
		for each( RCLicense ^lic in this->hSrl->hLicenseList )
		{
			System::Xml::XmlElement ^mid = doc->CreateElement( "middleware" );
			MasterEditorTWL::appendXmlTag( doc, mid, "publisher", lic->Publisher );
			MasterEditorTWL::appendXmlTag( doc, mid, "name", lic->Name );
			System::String ^note = "";
			if( this->isJapanese() )
			{
				note = this->hMiddlewareNameList->search(lic->Publisher, lic->Name, true );
			}
			else
			{
				note = this->hMiddlewareNameList->search(lic->Publisher, lic->Name, false );
			}
			if( note == nullptr )
			{
				note = "";
			}
			MasterEditorTWL::appendXmlTag( doc, mid, "note", note );
			midlist->AppendChild( mid );
		}
	}
	return midlist;
}

// ----------------------------------------------
// XML形式のリストを作成
// ----------------------------------------------
System::Void Form1::makeMiddlewareListXml(System::Xml::XmlDocument^ doc)
{
	System::Xml::XmlElement ^root = doc->CreateElement( "twl-master-editor" );
	System::Reflection::Assembly ^ass = System::Reflection::Assembly::GetEntryAssembly();
	root->SetAttribute( "version", this->getVersion() );
	doc->AppendChild( root );

	// ゲーム情報
	root->AppendChild( this->makeGameInfoXmlElement(doc) );

	// ミドルウェアリスト
	root->AppendChild( this->makeMiddlewareListXmlElement(doc) );
}

// ----------------------------------------------
// XML形式で書き出し
// ----------------------------------------------
System::Boolean Form1::saveMiddlewareListXml( System::String ^filename )
{
	System::Xml::XmlDocument ^doc = gcnew System::Xml::XmlDocument();
	doc->AppendChild( doc->CreateXmlDeclaration("1.0","UTF-8",nullptr) );

	this->makeMiddlewareListXml( doc );

	try
	{
		doc->Save( filename );
	}
	catch ( System::Exception ^ex )
	{
		(void)ex;
		return false;
	}
	return true;
}

// ----------------------------------------------
// XSL埋め込みXML形式で書き出し
// ----------------------------------------------
System::Boolean Form1::saveMiddlewareListXmlEmbeddedXsl( System::String ^filename )
{
	System::Xml::XmlDocument ^doc = gcnew System::Xml::XmlDocument();
	doc->AppendChild( doc->CreateXmlDeclaration("1.0","UTF-8",nullptr) );

	// XSLを埋め込むための宣言たち
	System::Xml::XmlProcessingInstruction ^proc = doc->CreateProcessingInstruction( "xml-stylesheet", "type='text/xml' href='#stylesheet'" );
	doc->AppendChild( proc );
	System::Xml::XmlDocumentType ^doctype = doc->CreateDocumentType( "twl-master-editor", nullptr, nullptr, "<!ATTLIST xsl:stylesheet id ID #REQUIRED>" );
	doc->AppendChild( doctype );

	this->makeMiddlewareListXml( doc );

	// 埋め込みスタイルシート
	System::Xml::XmlDocument ^xsl  = gcnew System::Xml::XmlDocument();
	System::Xml::XmlElement  ^root = doc->DocumentElement;
	try
	{
		xsl->Load( this->getMiddlewareListStyleFileEmbedded() );
		root->AppendChild( doc->ImportNode(xsl->DocumentElement, true) );
	}
	catch ( System::Exception ^ex )
	{
		(void)ex;
		return false;
	}

	try
	{
		doc->Save( filename );
	}
	catch ( System::Exception ^ex )
	{
		(void)ex;
		return false;
	}
	return true;
}

// ----------------------------------------------
// HTML形式で書き出し(XML->HTML変換)
// ----------------------------------------------
System::Boolean Form1::saveMiddlewareListHtml( System::String ^filename )
{
	System::Xml::Xsl::XslCompiledTransform ^xslt = gcnew System::Xml::Xsl::XslCompiledTransform;
	System::String ^tmpxml = gcnew System::String( this->getMiddlewareListTmpFile() );

	if( !saveMiddlewareListXml(tmpxml) )
	{
		return false;
	}

	try
	{
		xslt->Load( this->getMiddlewareListStyleFile() );
		xslt->Transform( tmpxml, filename );
	}
	catch (System::Exception ^s)
	{
		(void)s;
		System::IO::File::Delete( tmpxml );
		return false;
	}
	System::IO::File::Delete( tmpxml );
	return true;
}
