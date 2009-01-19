// ----------------------------------------------
// 設定ファイルの読み込み
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
// 通常設定ファイルの読み込み
// ----------------------------------------------

void Form1::loadInit(void)
{
	System::Xml::XmlDocument ^doc = gcnew System::Xml::XmlDocument();

	// xmlファイルの読み込み
	try
	{
		doc->Load( this->getIniFile() );
	}
	catch( System::IO::FileNotFoundException ^s )
	{
		(void)s;
		this->errMsg( "設定ファイルが見つかりません。", "Setting file is not found." );
		return;
	}
	catch( System::Exception ^s )
	{
		(void)s;
		this->errMsg( "設定ファイルを開くことができませんでした。", "Setting file can't be opened." );
		return;
	}

	// <init>タグ : ルート
	System::Xml::XmlElement ^root = doc->DocumentElement;

	// <rw>タグ
	System::Boolean bReadOnly = MasterEditorTWL::isXmlEqual( root, "rw", "r" );
	this->IsReadOnly = bReadOnly;
	if( bReadOnly )
	{
		this->readOnly();
	}

	// <access>タグ
	{
		// Shared2ファイルのアクセス許可(ファイルごとに指定)
		System::String ^shared2 = MasterEditorTWL::getXPathText( root, "/init/access/shared2" );
		if( shared2 )
		{
			cli::array<System::String^> ^list = shared2->Split( ',' );
			if( list )
			{
				for each( System::String ^item in list )
				{
					//System::Diagnostics::Debug::WriteLine( "access permit shared2 file: " + item );
					System::Int32 number = System::Int32::Parse( item );
					this->hSrl->hMrcExternalCheckItems->hIsPermitShared2Array[ number ] = true;		// 設定ファイルで指定されている番号のみ許可
				}
			}
		}

		// ノーマルジャンプのアクセス許可
		System::String ^isNJ = MasterEditorTWL::getXPathText( root, "/init/access/normalJump" );
		if( isNJ && isNJ->Equals( "Permit" ) )
		{
			this->hSrl->hMrcExternalCheckItems->IsPermitNormalJump = true;
		}
	}
	// <access>タグに付随して非表示項目を表示するかどうかを決める
	{
		// Shared2ファイル
		System::Boolean bVisible;
		for each( System::Boolean permit in this->hSrl->hMrcExternalCheckItems->hIsPermitShared2Array )
		{
			if( permit && !this->gboxShared2Size->Visible )
			{
				bVisible = true;		// どれかひとつでもtrueならグループボックス全体を表示する
			}
		}
		this->gboxShared2Size->Visible = bVisible;
		this->cboxIsShared2->Visible   = bVisible;
		if( bVisible )
		{
			int i;
			for( i=0; i < METWL_NUMOF_SHARED2FILES; i++ )
			{
				// グループボックス全体が表示されていたら全ファイルの項目が表示されるので
				// アクセス許可されていないファイルのみ非表示にする
				if( this->hSrl->hMrcExternalCheckItems->hIsPermitShared2Array[i] )
				{
					switch(i)
					{
						case 0: this->tboxShared2Size0->Visible = true; this->labShared2Size0->Visible = true; break;
						case 1: this->tboxShared2Size1->Visible = true; this->labShared2Size1->Visible = true; break;
						case 2: this->tboxShared2Size2->Visible = true; this->labShared2Size2->Visible = true; break;
						case 3: this->tboxShared2Size3->Visible = true; this->labShared2Size3->Visible = true; break;
						case 4: this->tboxShared2Size4->Visible = true; this->labShared2Size4->Visible = true; break;
						case 5: this->tboxShared2Size5->Visible = true; this->labShared2Size5->Visible = true; break;
						default: break;
					}
				}
			}
		}

		// ノーマルジャンプ
		this->changeVisibleGboxExFlags( this->hSrl->hMrcExternalCheckItems->IsPermitNormalJump );
	}

	// <shared2>タグ : Shared2ファイルサイズの真値(アクセス許可されているファイルのみ記載)
	System::Int32 i;
	for( i=0; i < METWL_NUMOF_SHARED2FILES; i++ )
	{
		System::String ^str = MasterEditorTWL::getXPathText( root, "/init/shared2/size" + i.ToString() );
		if( str )
		{
			this->hSrl->hMrcExternalCheckItems->hShared2SizeArray[i] = System::UInt32::Parse( str );
		}
		else		// アクセス許可されていないファイルは非公開なのでサイズが設定ファイルに記載されない
		{
			this->hSrl->hMrcExternalCheckItems->hShared2SizeArray[i] = 0;
		}
	}

	// <reserved-area>タグ : 予約領域の範囲を記載
	System::Xml::XmlNodeList ^list = root->SelectNodes( "/init/reserved-list/reserved" );
	System::Collections::IEnumerator^ iter = list->GetEnumerator();
	while( iter->MoveNext() )
	{
		System::Xml::XmlNode ^area = safe_cast<System::Xml::XmlNode^>(iter->Current);
		System::Xml::XmlNode ^begin = area->SelectSingleNode( "begin" );	// 相対パス
		System::Xml::XmlNode ^end   = area->SelectSingleNode( "end" );
		if( begin && begin->FirstChild && begin->FirstChild->Value && 
			end   && end->FirstChild   && end->FirstChild->Value )
		{
			System::UInt32 ibeg = System::UInt32::Parse( begin->FirstChild->Value, System::Globalization::NumberStyles::AllowHexSpecifier );
			System::UInt32 iend = System::UInt32::Parse( end->FirstChild->Value, System::Globalization::NumberStyles::AllowHexSpecifier );
			MasterEditorTWL::VCReservedArea iarea(ibeg, iend);
			this->hSrl->hMrcExternalCheckItems->hReservedAreaList->Add( iarea );
		}
	}
} // loadInit()


// ----------------------------------------------
// 追加設定ファイルの読み込み
// ----------------------------------------------

void Form1::loadAppendInit(void)
{
	System::String ^initfile = this->getAppendIniFile();

	// 通常は存在しない
	if( !System::IO::File::Exists( initfile ) )
	{
		return;
	}

	System::Xml::XmlDocument ^doc = gcnew System::Xml::XmlDocument();
	try
	{
		doc->Load( initfile );
	}
	catch( System::IO::FileNotFoundException ^s )
	{
		(void)s;
		this->errMsg( "追加設定ファイルが見つかりません。", "Setting file is not found." );
		return;
	}
	catch( System::Exception ^s )
	{
		(void)s;
		this->errMsg( "追加設定ファイルを開くことができませんでした。", "Setting file can't be opened." );
		return;
	}
	System::Xml::XmlElement ^root = doc->DocumentElement;

	// <visible>タグ
	System::Boolean bVisible = MasterEditorTWL::isXmlEqual( root, "visible", "ON" );
	this->changeVisibleForms( bVisible );
	this->changeVisibleGboxExFlags( bVisible );

	// <spcheck>タグ
	System::Boolean bCheck = MasterEditorTWL::isXmlEqual( root, "spcheck", "ON" );

	if( bCheck )	// チェックするときのみ追加チェック項目を設定
	{
		// チェックするかどうか
		this->hSrl->hMrcExternalCheckItems->IsAppendCheck = true;

		// SDK
		try
		{
			u32 major   = System::UInt32::Parse( MasterEditorTWL::getXPathText( root, "/init/sdk/major" ) );
			u32 minor   = System::UInt32::Parse( MasterEditorTWL::getXPathText( root, "/init/sdk/minor" ) );
			u32 relstep = System::UInt32::Parse( MasterEditorTWL::getXPathText( root, "/init/sdk/relstep" ) );
			u32 sdkver  = (major << 24) | (minor << 16) | (relstep & 0xFFFF);
			this->hSrl->hMrcExternalCheckItems->SDKVer = sdkver;
		}
		catch ( System::Exception ^ex )
		{
			(void)ex;
			this->errMsg( "設定ファイル中のSDKバージョンが読み込めませんでした。バージョンは0とみなされます。", 
				          "SDK ver. can't be read from setting file. Therefore it is set by 0." );
			this->hSrl->hMrcExternalCheckItems->SDKVer = 0;
		}

	} //if( bCheck )

} // loadAppendInit()