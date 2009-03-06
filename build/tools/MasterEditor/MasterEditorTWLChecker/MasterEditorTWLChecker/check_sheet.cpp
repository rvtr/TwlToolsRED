//
// 提出確認書のチェック
//

#include "stdafx.h"
#include "check.h"
#include <utility.h>
#include <twl/types.h>
#include <twl/os/common/ownerInfoEx.h>
#include <twl/os/common/format_rom.h>

using namespace System;

// ------------------------------------------------------------------
// 提出確認書の読み込み
// ------------------------------------------------------------------

System::Void SheetItem::readSheet( System::String ^sheetfile )
{
	// メンバの初期化
	this->ratings = gcnew cli::array<System::String^>(PARENTAL_CONTROL_INFO_SIZE);

	// XSLによってXML変換
	System::String ^tmpfile = ".\\temp" + System::DateTime::Now.ToString("yyyyMMddHHmmss") + ".xml";
	System::Xml::Xsl::XslCompiledTransform ^xslt = gcnew System::Xml::Xsl::XslCompiledTransform;
	System::String ^xslpath = System::IO::Path::GetDirectoryName( System::Reflection::Assembly::GetEntryAssembly()->Location )
		                      + "\\extract_sheet.xsl";

	//Console::WriteLine( "xslpath: " + xslpath );
	xslt->Load( xslpath );
	xslt->Transform( sheetfile, tmpfile );

	// 変換したXMLを読み込み
	System::Xml::XmlDocument ^doc = gcnew System::Xml::XmlDocument;
	doc->Load( tmpfile );

	// XMLからデータを抽出
	System::Xml::XmlElement  ^root = doc->DocumentElement;
	{
		this->region   = MasterEditorTWL::getXPathText( root, "/Sheet/Region" );
		this->ratings[ OS_TWL_PCTL_OGN_CERO ] = MasterEditorTWL::getXPathText( root, "/Sheet/RatingCERO" );
		this->ratings[ OS_TWL_PCTL_OGN_ESRB ] = MasterEditorTWL::getXPathText( root, "/Sheet/RatingESRB" );
		this->ratings[ OS_TWL_PCTL_OGN_USK  ] = MasterEditorTWL::getXPathText( root, "/Sheet/RatingUSK" );
		this->ratings[ OS_TWL_PCTL_OGN_PEGI_GEN ]  = MasterEditorTWL::getXPathText( root, "/Sheet/RatingPEGI" );
		this->ratings[ OS_TWL_PCTL_OGN_PEGI_PRT ]  = MasterEditorTWL::getXPathText( root, "/Sheet/RatingPEGIPRT" );
		this->ratings[ OS_TWL_PCTL_OGN_PEGI_BBFC ] = MasterEditorTWL::getXPathText( root, "/Sheet/RatingPEGIBBFC" );
		this->ratings[ OS_TWL_PCTL_OGN_OFLC ] = MasterEditorTWL::getXPathText( root, "/Sheet/RatingOFLC" );
		//this->ratings[ OS_TWL_PCTL_OGN_GRB  ] = MasterEditorTWL::getXPathText( root, "/Sheet/RatingGRB" );

		this->IsUnnecessaryRating = MasterEditorTWL::getXPathText( root, "/Sheet/IsUnnecessaryRating" );
		//System::String ^text = MasterEditorTWL::getXPathText( root, "/Sheet/IsUnnecessaryRating" );
		//if( !System::String::IsNullOrEmpty( text ) && text->Equals( "○" ) )
		//{
		//	this->IsUnnecessaryRating = true;
		//}
		//else
		//{
		//	this->IsUnnecessaryRating = false;
		//}
	}

	// 中間ファイルを削除
	if( System::IO::File::Exists( tmpfile ) )
	{
		//System::IO::File::Delete( tmpfile );
	}
}


// ------------------------------------------------------------------
// 提出確認書のチェック (設定ファイル中の真値との比較)
// ------------------------------------------------------------------

// @arg [in] ファイル情報
// @arg [in] 提出確認書の情報
System::Void checkSheet( SheetItem ^sItem )
{
	Console::WriteLine( "--------------------------------------------------------" );
	Console::WriteLine( "{0,-15} {1,-20}", nullptr, "Sheet" );
	Console::WriteLine( "--" );

	// リージョンの文字列を表示
	Console::WriteLine( "{0,-15} {1,-20}", "Region", sItem->region );
	Console::WriteLine( "--" );

	// レーティングの文字列の表示
	int i;
	for( i=0; i < PARENTAL_CONTROL_INFO_SIZE; i++ )
	{
		Console::WriteLine( "{0,-15} {1,-20}", getOgnString(i), sItem->ratings[i] );
	}
	Console::WriteLine( "--" );
	Console::WriteLine( "{0,-15} {1,-20}", "Unnecessary", sItem->IsUnnecessaryRating );
	Console::WriteLine( "--------------------------------------------------------" );
	return;
}