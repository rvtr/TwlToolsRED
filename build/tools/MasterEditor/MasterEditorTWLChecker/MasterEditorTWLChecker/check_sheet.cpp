//
// 提出確認書のチェック
//

#include "stdafx.h"
#include "check.h"
#include <utility.h>

using namespace System;


// ------------------------------------------------------------------
// 提出確認書の読み込み
// ------------------------------------------------------------------

System::Boolean SheetItem::readSheet( System::String ^sheetfile )
{
	// XSLによってXML変換
	System::String ^tmpfile = ".\\temp" + System::DateTime::Now.ToString("yyyyMMddHHmmss") + ".xml";
	System::Xml::Xsl::XslCompiledTransform ^xslt = gcnew System::Xml::Xsl::XslCompiledTransform;
	System::String ^xslpath = System::IO::Path::GetDirectoryName( System::Reflection::Assembly::GetEntryAssembly()->Location )
		                      + "\\extract_sheet.xsl";
	try
	{
		//Console::WriteLine( "xslpath: " + xslpath );
		xslt->Load( xslpath );
		xslt->Transform( sheetfile, tmpfile );
	}
	catch( System::Exception ^ex )
	{
		(void)ex;
		//Console::WriteLine( "XSLT Error" );
		return false;
	}
	// 変換したXMLを読み込み
	System::Xml::XmlDocument ^doc = gcnew System::Xml::XmlDocument;
	try
	{
		doc->Load( tmpfile );
	}
	catch( System::Exception ^ex )
	{
		(void)ex;
		//Console::WriteLine( "Load error" );
		return false;
	}

	// XMLからデータを抽出
	System::Xml::XmlElement  ^root = doc->DocumentElement;
	try
	{
		this->region   = MasterEditorTWL::getXPathText( root, "/Sheet/Region" );
		this->CERO     = MasterEditorTWL::getXPathText( root, "/Sheet/RatingCERO" );
		this->ESRB     = MasterEditorTWL::getXPathText( root, "/Sheet/RatingESRB" );
		this->USK      = MasterEditorTWL::getXPathText( root, "/Sheet/RatingUSK" );
		this->PEGI     = MasterEditorTWL::getXPathText( root, "/Sheet/RatingPEGI" );
		this->PEGIPRT  = MasterEditorTWL::getXPathText( root, "/Sheet/RatingPEGIPRT" );
		this->PEGIBBFC = MasterEditorTWL::getXPathText( root, "/Sheet/RatingPEGIBBFC" );
		this->OFLC     = MasterEditorTWL::getXPathText( root, "/Sheet/RatingOFLC" );

		System::String ^text = MasterEditorTWL::getXPathText( root, "/Sheet/IsUnnecessaryRating" );
		if( !System::String::IsNullOrEmpty( text ) && text->Equals( "○" ) )
		{
			this->IsUnnecessaryRating = true;
		}
		else
		{
			this->IsUnnecessaryRating = false;
		}

		Console::WriteLine( "[In Sheet]" );
		Console::WriteLine( "Region:   " + this->region );
		Console::WriteLine( "CERO:     " + this->CERO );
		Console::WriteLine( "ESRB:     " + this->ESRB );
		Console::WriteLine( "USK:      " + this->USK );
		Console::WriteLine( "PEGI:     " + this->PEGI );
		Console::WriteLine( "PEGIPRT:  " + this->PEGIPRT );
		Console::WriteLine( "PEGIBBFC: " + this->PEGIBBFC );
		Console::WriteLine( "OFLC:     " + this->OFLC );
		Console::WriteLine( "Unnecessary: " + this->IsUnnecessaryRating.ToString() );
	}
	catch( System::Exception ^ex )
	{
		//(void)ex;
		Console::WriteLine( ex->Message );
		return false;
	}

	// 中間ファイルを削除
	if( System::IO::File::Exists( tmpfile ) )
	{
		System::IO::File::Delete( tmpfile );
	}
	return true;
}


// ------------------------------------------------------------------
// 提出確認書のチェック (設定ファイル中の真値との比較)
// ------------------------------------------------------------------

// @arg [in] ファイル情報
// @arg [in] 提出確認書の情報
//
// @ret エラーメッセージ (エラーなしのときnullptr)
System::String^ checkSheet( FilenameItem ^fItem, SheetItem ^sItem )
{
	System::Xml::XmlDocument ^doc = gcnew System::Xml::XmlDocument;
	try
	{
		System::String ^cfgfile = System::IO::Path::GetDirectoryName( System::Reflection::Assembly::GetEntryAssembly()->Location )
		                          + "\\config.xml";
		doc->Load( cfgfile );
	}
	catch( System::Exception ^ex )
	{
		(void)ex;
		//Console::WriteLine( "Load error" );
		return (gcnew System::String("Failed to load XML"));
	}
	// XMLからデータを抽出
	System::Xml::XmlElement  ^root = doc->DocumentElement;

	// 設定ファイル中の真値と提出確認書の記述を比較
	// (設定ファイルにはファイル名と対応させたタグ名で真値が記述されている)
	System::String ^region = MasterEditorTWL::getXPathText( root, "/Config/Region/" + fItem->region + "/" + fItem->lang );
	System::String ^undef  = MasterEditorTWL::getXPathText( root, "/Config/Rating/Undefined/" + fItem->lang );
	System::String ^rating = MasterEditorTWL::getXPathText( root, "/Config/Rating/" + fItem->ogn + "/r" + fItem->rating + "/" + fItem->lang );
	Console::WriteLine( "[In Config file]" );
	Console::WriteLine( "Region:    " + region );
	Console::WriteLine( "Rating:    " + rating );
	Console::WriteLine( "Undefined: " + undef );
	System::String ^errmsg = nullptr;
	// リージョンの文字列をチェック
	if( sItem->region != region )
	{
		return (gcnew System::String("In Sheet, region is illegal string."));
	}
	// レーティングの文字列をチェック
	if( fItem->region == "JP" )
	{
		if( sItem->CERO != rating )
		{
			errmsg = "In Sheet, CERO is illegal string.";
		}
		if( (sItem->ESRB != undef) ||
			(sItem->USK  != undef) ||
			(sItem->PEGI != undef) ||
			(sItem->PEGIPRT != undef) ||
			(sItem->PEGIBBFC != undef) ||
			(sItem->OFLC != undef) )
		{
			errmsg = "In Sheet, Other Ogn is illegal string.";
		}
	}
	else if( fItem->region == "US" )
	{
		if( sItem->ESRB != rating )
		{
			errmsg = "In Sheet, ESRB is illegal string.";
		}
		if( (sItem->CERO != undef) ||
			(sItem->USK  != undef) ||
			(sItem->PEGI != undef) ||
			(sItem->PEGIPRT != undef) ||
			(sItem->PEGIBBFC != undef) ||
			(sItem->OFLC != undef) )
		{
			errmsg = "In Sheet, Other Ogn is illegal string.";
		}
	}
	else if( fItem->region == "EU" )
	{
		if( sItem->USK != rating )
		{
			errmsg = "In Sheet, USK is illegal string.";
		}
		if( sItem->PEGI != rating )
		{
			errmsg = "In Sheet, PEGI is illegal string.";
		}
		if( sItem->PEGIPRT != rating )
		{
			errmsg = "In Sheet, PEGIPRT is illegal string.";
		}
		if( sItem->PEGIBBFC != rating )
		{
			errmsg = "In Sheet, PEGIBBFC is illegal string.";
		}
		if( (sItem->CERO != undef) ||
			(sItem->ESRB != undef) ||
			(sItem->OFLC != undef) )
		{
			errmsg = "In Sheet, Other Ogn is illegal string.";
		}
	}
	else if( fItem->region == "AU" )
	{
		if( sItem->OFLC != rating )
		{
			errmsg = "In Sheet, OFLC is illegal string.";
		}
		if( (sItem->CERO != undef) ||
			(sItem->ESRB != undef) ||
			(sItem->USK  != undef) ||
			(sItem->PEGI != undef) ||
			(sItem->PEGIPRT != undef) ||
			(sItem->PEGIBBFC != undef) )
		{
			errmsg = "In Sheet, Other Ogn is illegal string.";
		}
	}
	else if( fItem->region == "EUAU" )
	{
		if( sItem->USK != rating )
		{
			errmsg = "In Sheet, USK is illegal string.";
		}
		if( sItem->PEGI != rating )
		{
			errmsg = "In Sheet, PEGI is illegal string.";
		}
		if( sItem->PEGIPRT != rating )
		{
			errmsg = "In Sheet, PEGIPRT is illegal string.";
		}
		if( sItem->PEGIBBFC != rating )
		{
			errmsg = "In Sheet, PEGIBBFC is illegal string.";
		}
		if( sItem->OFLC != rating )
		{
			errmsg = "In Sheet, OFLC is illegal string.";
		}
		if( (sItem->CERO != undef) ||
			(sItem->ESRB != undef) )
		{
			errmsg = "In Sheet, Other Ogn is illegal string.";
		}
	}
	if( errmsg != nullptr )
	{
		return errmsg;
	}

	return nullptr;
}