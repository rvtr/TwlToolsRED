// ----------------------------------------------
// エラーリストの書き出し
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

// XML形式でリストを作成
System::Void Form1::makeErrorListXml(System::Xml::XmlDocument ^doc, System::Boolean isCurrent)
{
	System::Xml::XmlElement ^root = doc->CreateElement( "twl-master-editor" );
	System::Reflection::Assembly ^ass = System::Reflection::Assembly::GetEntryAssembly();
	root->SetAttribute( "version", this->getVersion() );
	doc->AppendChild( root );

	// ゲーム情報
	System::Xml::XmlElement ^game = doc->CreateElement( "game" );
	root->AppendChild( game );
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

	// エラーリスト
	System::Xml::XmlElement ^errorlist = doc->CreateElement( "error-list" );
	root->AppendChild( errorlist );
	if( isCurrent )	// 現在の入力を反映
	{
		if( this->hSrl->hErrorList != nullptr )		// ROM読み込み時に発見されたエラー
		{
			for each( RCMrcError ^err in this->hSrl->hErrorList )
			{
				if( !err->IsEnableModify )	// 修正可能な情報は出力しない
				{
					errorlist->AppendChild( CreateErrorListElement(doc, err, 0, "error", this->isJapanese()) );
				}
			}
		}
		if( this->hErrorList != nullptr )	// 現在のフォーム入力から発見されたエラー
		{
			for each( RCMrcError ^err in this->hErrorList )
			{
				if( err->IsAffectRom )	// 書類上のエラーは出力しない
				{
					errorlist->AppendChild( CreateErrorListElement(doc, err, 1, "error", this->isJapanese()) );
				}
			}
		}
	}
	else	// ROM読み込み時の情報
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

	// 警告リスト
	System::Xml::XmlElement ^warnlist = doc->CreateElement( "warning-list" );
	root->AppendChild( warnlist );
	if( isCurrent )	// 現在の入力を反映
	{
		if( this->hSrl->hWarnList != nullptr )		// ROM読み込み時に発見されたエラー
		{
			for each( RCMrcError ^err in this->hSrl->hWarnList )
			{
				if( !err->IsEnableModify )	// 修正可能な情報は出力しない
				{
					warnlist->AppendChild( CreateErrorListElement(doc, err, 0, "warning", this->isJapanese()) );
				}
			}
		}
		if( this->hWarnList != nullptr )	// 現在のフォーム入力から発見されたエラー
		{
			for each( RCMrcError ^err in this->hWarnList )
			{
				if( err->IsAffectRom )	// 書類上のエラーは出力しない
				{
					warnlist->AppendChild( CreateErrorListElement(doc, err, 1, "warning", this->isJapanese()) );
				}
			}
		}
	}
	else	// ROM読み込み時の情報
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
		// TMPファイルにXML版作成
		System::Xml::XmlDocument ^doc = gcnew System::Xml::XmlDocument();
		doc->AppendChild( doc->CreateXmlDeclaration("1.0","UTF-8",nullptr) );
		this->makeErrorListXml( doc, isCurrent );
		doc->Save( tmpxml );
		// XML=>HTML変換
		System::Xml::Xsl::XslCompiledTransform ^xslt = gcnew System::Xml::Xsl::XslCompiledTransform;
		xslt->Load( this->getErrorListXsl() );
		xslt->Transform( tmpxml, filepath );
	}
	catch (System::Exception ^s)
	{
		System::IO::File::Delete( tmpxml );	// 次のためにTMPファイルを削除しておく
		throw s;	// 呼び出し元に例外を再throw
		return;
	}
	System::IO::File::Delete( tmpxml );
} //System::Void Form1::makeErrorListHtml