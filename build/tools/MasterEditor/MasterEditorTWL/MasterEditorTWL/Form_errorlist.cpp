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

static int s_NodeCount = 0;

// ======================================================
// XMLの一要素を作成
// ======================================================
static System::Xml::XmlElement^ CreateErrorListElement(System::Xml::XmlDocument ^doc, RCMrcError ^err, int errtype, 
													   System::String ^tagname, System::Boolean isJapanese)
{
	System::Xml::XmlElement ^tag = doc->CreateElement(tagname);
    tag->SetAttribute( "num", ( s_NodeCount++ ).ToString() );
	MasterEditorTWL::appendXmlTag( doc, tag, "errname",  (isJapanese)?err->Name:err->NameE );
	MasterEditorTWL::appendXmlTag( doc, tag, "errtype",  errtype.ToString() );
	MasterEditorTWL::appendXmlTag( doc, tag, "errbegin", err->Begin.ToString("X04")+"h" );
	MasterEditorTWL::appendXmlTag( doc, tag, "errend",   err->End.ToString("X04")+"h" );
	MasterEditorTWL::appendXmlTag( doc, tag, "errmsg",   (isJapanese)?err->Msg:err->MsgE );
	return tag;
}

// ======================================================
// XML形式のリストの本体となるエラーリストの作成(他でも使うので独立させる)
// ======================================================
System::Xml::XmlElement^ Form1::makeErrorListXmlElement(System::Xml::XmlDocument ^doc, System::Boolean isCurrent)
{
    s_NodeCount = 0;
    
    // エラーリスト
	System::Xml::XmlElement ^list = doc->CreateElement( "error-list" );
	if( isCurrent )	// 現在の入力を反映
	{
		if( this->hSrl->hErrorList != nullptr )		// ROM読み込み時に発見されたエラー
		{
			for each( RCMrcError ^err in this->hSrl->hErrorList )
			{
				if( !err->IsEnableModify && this->isDisplayOneGridErrorForPurpose( err ) )	// 修正可能な情報は出力しない
				{
					list->AppendChild( CreateErrorListElement(doc, err, 0, "error", this->isJapanese()) );
				}
			}
		}
		if( this->hErrorList != nullptr )	// 現在のフォーム入力から発見されたエラー
		{
			for each( RCMrcError ^err in this->hErrorList )
			{
				if( err->IsAffectRom && this->isDisplayOneGridErrorForPurpose( err ) )	// 書類上のエラーは出力しない
				{
					list->AppendChild( CreateErrorListElement(doc, err, 1, "error", this->isJapanese()) );
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
				int errtype = 0;			// 修正可能な情報と不可能な情報とでタイプを分ける
				if( err->IsEnableModify )
				{
					errtype = 1;
				}
				if( this->isDisplayOneGridErrorForPurpose( err ) )
				{
					list->AppendChild( CreateErrorListElement(doc, err, errtype, "error", this->isJapanese()) );
				}
			}
		}
	}
	return list;
}//System::Xml::XmlElement^ Form1::makeErrorListXmlElement

System::Xml::XmlElement^ Form1::makeWarningListXmlElement(System::Xml::XmlDocument ^doc, System::Boolean isCurrent)
{
    s_NodeCount = 0;

    // 警告リスト
	System::Xml::XmlElement ^list = doc->CreateElement( "warning-list" );
	if( isCurrent )	// 現在の入力を反映
	{
		if( this->hSrl->hWarnList != nullptr )		// ROM読み込み時に発見されたエラー
		{
			for each( RCMrcError ^err in this->hSrl->hWarnList )
			{
				if( !err->IsEnableModify && this->isDisplayOneGridErrorForPurpose( err ) )	// 修正可能な情報は出力しない
				{
					list->AppendChild( CreateErrorListElement(doc, err, 0, "warning", this->isJapanese()) );
				}
			}
		}
		if( this->hWarnList != nullptr )	// 現在のフォーム入力から発見されたエラー
		{
			for each( RCMrcError ^err in this->hWarnList )
			{
				if( err->IsAffectRom && this->isDisplayOneGridErrorForPurpose( err ) )	// 書類上のエラーは出力しない
				{
					list->AppendChild( CreateErrorListElement(doc, err, 1, "warning", this->isJapanese()) );
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
				int errtype = 0;			// 修正可能な情報と不可能な情報とでタイプを分ける
				if( err->IsEnableModify )
				{
					errtype = 1;
				}
				if( this->isDisplayOneGridErrorForPurpose( err ) )
				{
					list->AppendChild( CreateErrorListElement(doc, err, errtype, "warning", this->isJapanese()) );
				}
			}
		}
	}
	return list;
} //System::Xml::XmlElement^ Form1::makeWarningListXmlElement

System::Xml::XmlElement^ Form1::makeErrorListCaptionXmlElement(System::Xml::XmlDocument ^doc)
{
	// 警告リスト
	System::Xml::XmlElement ^list = doc->CreateElement( "error-caption-list" );
	{
		System::Xml::XmlElement ^tag = doc->CreateElement( "error-caption" );
		System::String ^msg = "* Type 0 indicates problems with the ROM data that cannot be corrected using MasterEditorTWL.";
		if(this->isJapanese())
		{
			msg = "* Type 0 の項目はマスターエディタで修正できません。";
		}
		MasterEditorTWL::appendXmlTag( doc, tag, "text", msg );
		list->AppendChild(tag);
	}
	{
		System::Xml::XmlElement ^tag = doc->CreateElement( "error-caption" );
		System::String ^msg = "* Type 1 indicates problems with the ROM data that can be corrected using MasterEditorTWL.";
		if(this->isJapanese())
		{
			msg = "* Type 1 の項目はマスターエディタで修正可能です。";
		}
		MasterEditorTWL::appendXmlTag( doc, tag, "text", msg );
		list->AppendChild(tag);
	}
	return list;
} //System::Xml::XmlElement^ Form1::makeErrorListCaptionXmlElement


// ======================================================
// XML形式のリストに載せるエラー数のカウント
// ======================================================
int Form1::countErrorListXmlElement(System::Boolean isCurrent)
{
    s_NodeCount = 0;
    
    int count = 0;

    // エラーリスト
	if( isCurrent )	// 現在の入力を反映
	{
		if( this->hSrl->hErrorList != nullptr )		// ROM読み込み時に発見されたエラー
		{
			for each( RCMrcError ^err in this->hSrl->hErrorList )
			{
				if( !err->IsEnableModify && this->isDisplayOneGridErrorForPurpose( err ) )	// 修正可能な情報は出力しない
				{
                    count++;
				}
			}
		}
		if( this->hErrorList != nullptr )	// 現在のフォーム入力から発見されたエラー
		{
			for each( RCMrcError ^err in this->hErrorList )
			{
				if( err->IsAffectRom && this->isDisplayOneGridErrorForPurpose( err ) )	// 書類上のエラーは出力しない
				{
                    count++;
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
				int errtype = 0;			// 修正可能な情報と不可能な情報とでタイプを分ける
				if( err->IsEnableModify )
				{
					errtype = 1;
				}
				if( this->isDisplayOneGridErrorForPurpose( err ) )
				{
                    count++;
				}
			}
		}
	}
    return count;
}//int Form1::countErrorListXmlElement

int Form1::countWarningListXmlElement(System::Boolean isCurrent)
{
    s_NodeCount = 0;

    int count = 0;

    // 警告リスト
	if( isCurrent )	// 現在の入力を反映
	{
		if( this->hSrl->hWarnList != nullptr )		// ROM読み込み時に発見されたエラー
		{
			for each( RCMrcError ^err in this->hSrl->hWarnList )
			{
				if( !err->IsEnableModify && this->isDisplayOneGridErrorForPurpose( err ) )	// 修正可能な情報は出力しない
				{
                    count++;
				}
			}
		}
		if( this->hWarnList != nullptr )	// 現在のフォーム入力から発見されたエラー
		{
			for each( RCMrcError ^err in this->hWarnList )
			{
				if( err->IsAffectRom && this->isDisplayOneGridErrorForPurpose( err ) )	// 書類上のエラーは出力しない
				{
                    count++;
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
				int errtype = 0;			// 修正可能な情報と不可能な情報とでタイプを分ける
				if( err->IsEnableModify )
				{
					errtype = 1;
				}
				if( this->isDisplayOneGridErrorForPurpose( err ) )
				{
                    count++;
				}
			}
		}
	}
	return count;
} //int Form1::countWarningListXmlElement


// ======================================================
// XML形式でリストを作成
// ======================================================
System::Void Form1::makeErrorListXml(System::Xml::XmlDocument ^doc, System::Boolean isCurrent)
{
	// 情報を最新に更新
	this->hErrorList->Clear();
	this->hWarnList->Clear();
	this->checkSrlForms();
	this->checkDeliverableForms();

	System::Xml::XmlElement ^root = doc->CreateElement( "twl-master-editor" );
	System::Reflection::Assembly ^ass = System::Reflection::Assembly::GetEntryAssembly();
	root->SetAttribute( "version", this->getVersion() );
	doc->AppendChild( root );

	// ゲーム情報
	root->AppendChild( this->makeGameInfoXmlElement(doc) );

    // ファイルパス
    MasterEditorTWL::appendXmlTag( doc, root, "path", this->tboxFile->Text );

    // 作成日時
    System::Xml::XmlElement ^time = doc->CreateElement( "time-info" );
    System::DateTime ^ datetime = System::DateTime::Now;
    time->SetAttribute( "year",  datetime->Year.ToString() );
    switch( datetime->Month )
    {
    case  1: time->SetAttribute( "month", "January" );   break;
    case  2: time->SetAttribute( "month", "February" );  break;
    case  3: time->SetAttribute( "month", "March" );     break;
    case  4: time->SetAttribute( "month", "April" );     break;
    case  5: time->SetAttribute( "month", "May" );       break;
    case  6: time->SetAttribute( "month", "June" );      break;
    case  7: time->SetAttribute( "month", "July" );      break;
    case  8: time->SetAttribute( "month", "August" );    break;
    case  9: time->SetAttribute( "month", "September" ); break;
    case 10: time->SetAttribute( "month", "October" );   break;
    case 11: time->SetAttribute( "month", "November" );  break;
    case 12: time->SetAttribute( "month", "December" );  break;
    default: time->SetAttribute( "month", datetime->Month.ToString() ); break;
    }
    time->SetAttribute( "day",   datetime->Day.ToString() );
    time->SetAttribute( "time",  datetime->Hour.ToString("D2") + ":" + datetime->Minute.ToString("D2") );
    root->AppendChild( time );
    
    System::Xml::XmlElement ^section;

    // エラー情報
	section = doc->CreateElement("error-info");
	MasterEditorTWL::appendXmlTag( doc, section, "index", this->isJapanese()?"エラー":"Error" );
	section->AppendChild( this->makeErrorListXmlElement(doc, isCurrent) );
	root->AppendChild(section);

	// 警告情報
	section = doc->CreateElement("warning-info");
	MasterEditorTWL::appendXmlTag( doc, section, "index", this->isJapanese()?"警告":"Warning" );
	section->AppendChild( this->makeWarningListXmlElement(doc, isCurrent) );
	root->AppendChild(section);

    // エラー数
    section = doc->CreateElement("error-num-info");
    section->SetAttribute( "error-title", this->isJapanese()?"エラー":"Error" );
    section->SetAttribute( "error-num", this->countErrorListXmlElement( isCurrent ).ToString() );
    section->SetAttribute( "warning-title", this->isJapanese()?"警告":"Warning" );
    section->SetAttribute( "warning-num", this->countWarningListXmlElement( isCurrent ).ToString() );
	root->AppendChild(section);

	// 追記
	root->AppendChild( this->makeErrorListCaptionXmlElement(doc) );

} //System::Void Form1::makeErrorListXml

// ======================================================
// HTML形式でファイルを出力
// ======================================================
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