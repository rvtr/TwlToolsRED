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

		System::String ^text = MasterEditorTWL::getXPathText( root, "/Sheet/IsUnnecessaryRating" );
		if( !System::String::IsNullOrEmpty( text ) && text->Equals( "○" ) )
		{
			this->IsUnnecessaryRating = true;
		}
		else
		{
			this->IsUnnecessaryRating = false;
		}

		//Console::WriteLine( "[In Sheet]" );
		//Console::WriteLine( "Region:   " + this->region );
		//Console::WriteLine( "Region: {0,-20} {1,-20}", this->region, this->region );
		//Console::WriteLine( "CERO:     " + this->ratings[ OS_TWL_PCTL_OGN_CERO ] );
		//Console::WriteLine( "ESRB:     " + this->ratings[ OS_TWL_PCTL_OGN_ESRB ] );
		//Console::WriteLine( "USK:      " + this->ratings[ OS_TWL_PCTL_OGN_USK  ] );
		//Console::WriteLine( "PEGI:     " + this->ratings[ OS_TWL_PCTL_OGN_PEGI_GEN ] );
		//Console::WriteLine( "PEGIPRT:  " + this->ratings[ OS_TWL_PCTL_OGN_PEGI_PRT ] );
		//Console::WriteLine( "PEGIBBFC: " + this->ratings[ OS_TWL_PCTL_OGN_PEGI_BBFC ] );
		//Console::WriteLine( "OFLC:     " + this->ratings[ OS_TWL_PCTL_OGN_OFLC ] );
		////Console::WriteLine( "GRB:      " + this->ratings[ OS_TWL_PCTL_OGN_GRB ] );
		//Console::WriteLine( "Unnecessary: " + this->IsUnnecessaryRating.ToString() );
	}

	// 中間ファイルを削除
	if( System::IO::File::Exists( tmpfile ) )
	{
		System::IO::File::Delete( tmpfile );
	}
}


// ------------------------------------------------------------------
// 提出確認書のチェック (設定ファイル中の真値との比較)
// ------------------------------------------------------------------

// @arg [in] ファイル情報
// @arg [in] 提出確認書の情報
System::Void checkSheet( FilenameItem ^fItem, SheetItem ^sItem )
{
	System::Xml::XmlDocument ^doc = gcnew System::Xml::XmlDocument;
	System::String ^cfgfile = System::IO::Path::GetDirectoryName( System::Reflection::Assembly::GetEntryAssembly()->Location )
	                          + "\\config.xml";
	doc->Load( cfgfile );

	// XMLからデータを抽出
	System::Xml::XmlElement  ^root = doc->DocumentElement;

	DebugPrint( "--------------------------------------------------------" );
	DebugPrint( "{0,-10} {1,-20} {2,-20}", nullptr, "Config", "Sheet" );
	DebugPrint( "--" );

	// 設定ファイル中の真値と提出確認書の記述を比較
	// (設定ファイルにはファイル名と対応させたタグ名で真値が記述されている)

	// リージョンの文字列をチェック
	System::String ^region = MasterEditorTWL::getXPathText( root, "/Config/Region/" + fItem->region + "/" + fItem->lang );

	DebugPrint( "{0,-10} {1,-20} {2,-20}", "Region", region, sItem->region );
	DebugPrint( "--" );

	if( sItem->region != region )
	{
		throw (gcnew System::Exception("In Sheet, region is an Illegal String."));
		return;
	}

	// レーティングの文字列のチェック
	System::Collections::Generic::List<int> ^ognlist = MasterEditorTWL::getOgnListInRegion(  fItem->getRegionBitmap() );
	if( fItem->ogn != fItem->getOgnString(-1) )
	{
		// 「レーティング表示不要」でないとき

		// 対象のレーティングの文字列を真値と比較
		System::String ^rating = MasterEditorTWL::getXPathText( root, "/Config/Rating/" + fItem->ogn + "/r" + fItem->rating + "/" + fItem->lang );
		DebugPrint( "{0,-10} {1,-20} {2,-20}", fItem->ogn, rating, sItem->ratings[fItem->getOgnNumber()] );
		if( sItem->ratings[fItem->getOgnNumber()] != rating )
		{
			throw (gcnew System::Exception("In Sheet, " + fItem->ogn + " mismatch."));
			return;
		}

		// その他のリージョンに含まれる団体が「全年齢」になっているかチェック
		for each ( int ogn in ognlist )
		{
			if( ogn != fItem->getOgnNumber() )
			{
				// 設定ファイルから 00 (全年齢)のときの文字列を抜き出す
				System::String ^str   = fItem->getOgnString( ogn );
				System::String ^other = MasterEditorTWL::getXPathText( root, "/Config/Rating/" + str + "/r00/" + fItem->lang );

				DebugPrint( "{0,-10} {1,-20} {2,-20}", str, other, sItem->ratings[ogn] );

				// 提出確認書の文字列をチェック
				if( sItem->ratings[ogn] != other )
				{
					throw (gcnew System::Exception("In Sheet, " + str + " mismatch a String for \"All ages\""));
					return;
				}
			}
		}
	}
	else
	{
		// 「レーティング表示不要」のとき

		// リージョンに含まれるすべての団体が「レーティング不要」になっているかチェック
		for each ( int ogn in ognlist )
		{
			// 設定ファイルから「レーティング表示不要」のときの文字列を抜き出す
			System::String ^str = fItem->getOgnString(-1);
			System::String ^unnecessary = MasterEditorTWL::getXPathText( root, "/Config/Rating/" + str + "/" + fItem->lang );

			DebugPrint( "{0,-10} {1,-20} {2,-20}", fItem->getOgnString(ogn), unnecessary, sItem->ratings[ogn] );

			// 提出確認書の文字列をチェック
			if( sItem->ratings[ogn] != unnecessary )
			{
				throw (gcnew System::Exception("In Sheet, " + str + " mismatch a string for \"Unnecessary\""));
				return;
			}
		}

		// フラグをチェック
		if( !sItem->IsUnnecessaryRating )
		{
			throw (gcnew System::Exception("In Sheet, \"Unnecessary\" Flag is Negated."));
			return;
		}
	}

	// リージョン設定可能な団体(Reservedでない団体)をリストアップ(ここに含まれない団体はチェックしなくてよい)
	System::Collections::Generic::List<int> ^alllist = gcnew System::Collections::Generic::List<int>();
	alllist->Clear();
	alllist->Add( OS_TWL_PCTL_OGN_CERO );
	alllist->Add( OS_TWL_PCTL_OGN_ESRB );
	alllist->Add( OS_TWL_PCTL_OGN_USK );
	alllist->Add( OS_TWL_PCTL_OGN_PEGI_GEN );
	alllist->Add( OS_TWL_PCTL_OGN_PEGI_PRT );
	alllist->Add( OS_TWL_PCTL_OGN_PEGI_BBFC );
	alllist->Add( OS_TWL_PCTL_OGN_OFLC );
	//alllist->Add( OS_TWL_PCTL_OGN_GRB );

	// リージョンに含まれない団体が「不可」になっているかチェック
	System::String ^disable = MasterEditorTWL::getXPathText( root, "/Config/Rating/DISABLE/" + fItem->lang );
	int i;
	for( i=0; i < PARENTAL_CONTROL_INFO_SIZE; i++ )
	{
		// Reserved の団体は調べない
		if( (alllist->IndexOf(i) >=0 ) && (ognlist->IndexOf(i) < 0) )
		{
			DebugPrint( "{0,-10} {1,-20} {2,-20}", fItem->getOgnString(i), disable, sItem->ratings[i] );
			if( sItem->ratings[i] != disable )
			{
				throw (gcnew System::Exception("In Sheet, " + fItem->getOgnString(i) + " mismatch a string for \"Disable\""));
				return;
			}
		}
	}
	DebugPrint( "--------------------------------------------------------" );
	return;
}