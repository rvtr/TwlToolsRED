// ----------------------------------------------
// ROM情報リストの書き出し
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
// XMLの一要素を作成
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
// XML形式でリストを作成
// ======================================================
void Form1::makeRomInfoListXml(System::Xml::XmlDocument ^doc, System::Boolean withError, System::Boolean isCurrent)
{
	if( withError )
	{
		// エラー情報を最新に更新
		this->hErrorList->Clear();
		this->hWarnList->Clear();
		this->checkSrlForms();
		this->checkDeliverableForms();
	}

	System::Xml::XmlElement ^root = doc->CreateElement( "twl-master-editor" );
	System::Reflection::Assembly ^ass = System::Reflection::Assembly::GetEntryAssembly();
	root->SetAttribute( "version", this->getVersion() );
	doc->AppendChild( root );

	// ゲーム情報
	root->AppendChild( this->makeGameInfoXmlElement(doc) );

	//
	// 各タブごとに情報をまとめる
	//
	System::Xml::XmlElement ^section;

	// ROM基本情報
	section = doc->CreateElement("basic-rom-info");
	MasterEditorTWL::appendXmlTag( doc, section, "index", this->isJapanese()?"ROM基本情報":"Basic ROM Info" );
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

	// TWL拡張情報タブ
	section = doc->CreateElement("twl-extended-info");
	MasterEditorTWL::appendXmlTag( doc, section, "index", this->isJapanese()?"TWL拡張情報":"TWL Extended Info" );
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

	// アクセス情報
	section = doc->CreateElement("access-control-info");
	MasterEditorTWL::appendXmlTag( doc, section, "index", this->isJapanese()?"アクセスコントロール情報":"Access Control" );
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

	// DSiウェア情報
	section = doc->CreateElement("dsi-ware-info");
	MasterEditorTWL::appendXmlTag( doc, section, "index", this->isJapanese()?"DSiウェア情報":"DSiWare Info" );
	{
		System::Xml::XmlElement ^tag = doc->CreateElement("info-list");
		tag->AppendChild( CreateRomInfoListElement(doc, this->labMedia->Text, this->tboxMedia->Text, nullptr) );
		if( this->hSrl->IsNAND )	// カードアプリでは不要な情報
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

	// リージョンとレーティング
	section = doc->CreateElement("market-and-rating-info");
	MasterEditorTWL::appendXmlTag( doc, section, "index", this->isJapanese()?"リージョンとレーティング":"Market and Rating" );
	{
		System::Xml::XmlElement ^tag = doc->CreateElement("info-list");

		// リージョン
		System::String ^region = "";
		if( this->combRegion->DropDownStyle == System::Windows::Forms::ComboBoxStyle::DropDown )
		{
			// 中韓のときはリストで選択不可のテキストがコンボボックスに書かれているのでそれを登録
			region = this->combRegion->Text;
		}
		else
		{
			// WorldWide のときはリストで選択可能な文字列を登録
			if( this->combRegion->SelectedIndex < 0 )
			{
				region = this->isJapanese()?METWL_STRING_UNDEFINED_REGION_J:METWL_STRING_UNDEFINED_REGION_E;	// 起こり得ない
			}
			else
			{
				// リストで選択されているテキストを登録
				region = dynamic_cast<System::String^>(this->combRegion->SelectedItem);
			}
		}
		tag->AppendChild( CreateRomInfoListElement(doc, this->labRegion->Text, region, nullptr) );
		section->AppendChild(tag);

		// レーティング
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
	// SDKバージョンは独立した表
	//
	section = doc->CreateElement("sdk-version-info");
	MasterEditorTWL::appendXmlTag( doc, section, "index", this->isJapanese()?"SDKバージョン":"SDK Version" );
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
	// ミドルウェアリストも独立した表
	//
	section = doc->CreateElement("middleware-info");
	MasterEditorTWL::appendXmlTag( doc, section, "index", this->isJapanese()?"使用ライブラリ":"Libraries" );
	section->AppendChild( this->makeMiddlewareListXmlElement(doc) );
	root->AppendChild(section);

	//
	// エラー情報も独立した表
	//
	if( withError )
	{
		section = doc->CreateElement("error-info");
		MasterEditorTWL::appendXmlTag( doc, section, "index", this->isJapanese()?"エラー":"Error" );
		section->AppendChild( this->makeErrorListXmlElement(doc, isCurrent) );
		section->AppendChild( this->makeErrorListCaptionXmlElement(doc) );
		root->AppendChild(section);

		section = doc->CreateElement("warning-info");
		MasterEditorTWL::appendXmlTag( doc, section, "index", this->isJapanese()?"警告":"Warning" );
		section->AppendChild( this->makeWarningListXmlElement(doc, isCurrent) );
		section->AppendChild( this->makeErrorListCaptionXmlElement(doc) );
		root->AppendChild(section);
	}

} //Form1::makeRomInfoListXml

// ======================================================
// HTML形式でファイルを出力
// ======================================================
void Form1::makeRomInfoListHtml(System::String ^filepath, System::Boolean withError, System::Boolean isCurrent)
{
	System::String ^tmpxml = gcnew System::String( this->getXmlToHtmlTmpFile() );
	try
	{
		// TMPファイルにXML版作成
		System::Xml::XmlDocument ^doc = gcnew System::Xml::XmlDocument();
		doc->AppendChild( doc->CreateXmlDeclaration("1.0","UTF-8",nullptr) );
		this->makeRomInfoListXml( doc, withError, isCurrent );
		doc->Save( tmpxml );
		// XML=>HTML変換
		System::Xml::Xsl::XslCompiledTransform ^xslt = gcnew System::Xml::Xsl::XslCompiledTransform;
		xslt->Load( this->getRomInfoListXsl() );
		xslt->Transform( tmpxml, filepath );
	}
	catch (System::Exception ^s)
	{
		System::IO::File::Delete( tmpxml );	// 次のためにTMPファイルを削除しておく
		throw s;	// 呼び出し元に例外を再throw
		return;
	}
	System::IO::File::Delete( tmpxml );

} //Form1::makeRomInfoListHtml(System::String ^filepath)

