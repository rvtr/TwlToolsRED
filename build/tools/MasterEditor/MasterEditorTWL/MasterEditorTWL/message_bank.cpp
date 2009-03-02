#include "stdafx.h"

// RCMessageBankクラスの実装

#include <cstring>
#include <cstdio>
#include "message_bank.h"
#include "utility.h"

using namespace MasterEditorTWL;

// ****************************************************************
// RCMessageBank クラス
// ****************************************************************

// -------------------------------------------------------------------
// constructor
// -------------------------------------------------------------------
RCMessageBank::RCMessageBank( System::String ^fileJ, System::String ^fileE )
{
	this->loadMessage( fileJ, "J" );
	this->loadMessage( fileE, "E" );
}

// -------------------------------------------------------------------
// 外部ファイルからメッセージを読み出す
// -------------------------------------------------------------------
void RCMessageBank::loadMessage( System::String ^filepath, System::String ^lang )
{
	System::Xml::XmlDocument ^doc = gcnew System::Xml::XmlDocument();
	doc->Load( filepath );
	System::Xml::XmlElement ^root = doc->DocumentElement;

	// 言語によってXMLルートの格納先を変更
	if( lang == "J" )
	{
		this->rootJ = root;
	}
	else
	{
		this->rootE = root;
	}
}

// -------------------------------------------------------------------
// 特定のメッセージを返す
// -------------------------------------------------------------------
System::String^ RCMessageBank::getMessage( System::String ^tag, System::String ^lang )
{
	System::Xml::XmlElement ^root;
	if( lang == "J" )
	{
		root = this->rootJ;
	}
	else
	{
		root = this->rootE;
	}

	// どのファイルも同じタグがつけられていることが前提
	System::String ^msg = MasterEditorTWL::getXPathText( root, "/message/" + tag );
	return msg;
}