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
		doc->Load( "../resource/ini.xml" );
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
} // loadInit()


// ----------------------------------------------
// 追加設定ファイルの読み込み
// ----------------------------------------------

void Form1::loadAppendInit(void)
{
	System::String ^initfile = "../resource/append_ini.xml";

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

	// <spcheck>タグ
	System::Boolean bCheck = MasterEditorTWL::isXmlEqual( root, "spcheck", "ON" );

	if( bCheck )	// チェックするときのみ追加チェック項目を設定
	{
		// チェックするかどうか
		this->hSrl->hMrcSpecialList->IsCheck = true;

		// SDK
		try
		{
			u32 major   = System::UInt32::Parse( MasterEditorTWL::getXPathText( root, "/init/sdk/major" ) );
			u32 minor   = System::UInt32::Parse( MasterEditorTWL::getXPathText( root, "/init/sdk/minor" ) );
			u32 relstep = System::UInt32::Parse( MasterEditorTWL::getXPathText( root, "/init/sdk/relstep" ) );
			u32 sdkver  = (major << 24) | (minor << 16) | (relstep & 0xFFFF);
			this->hSrl->hMrcSpecialList->SDKVer = sdkver;
		}
		catch ( System::Exception ^ex )
		{
			(void)ex;
			this->errMsg( "設定ファイル中のSDKバージョンが読み込めませんでした。バージョンは0とみなされます。", 
				          "SDK ver. can't be read from setting file. Therefore it is set by 0." );
			this->hSrl->hMrcSpecialList->SDKVer = 0;
		}

		// Shared2File
		try
		{
			System::Int32 i;
			for( i=0; i < METWL_NUMOF_SHARED2FILES; i++ )
			{
				u32 size = System::UInt32::Parse( MasterEditorTWL::getXPathText( root, "/init/shared2/size" + i.ToString() ) );
				this->hSrl->hMrcSpecialList->hShared2SizeArray[i] = size;
			}
		}
		catch ( System::Exception ^ex )
		{
			(void)ex;
			this->errMsg( "設定ファイル中のShared2ファイルサイズが読み込めませんでした。サイズはすべて0とみなされます。", 
				          "One of shared2 file sizes can't be read from setting file. Therefore they are set by 0." );
			System::Int32 i;
			for( i=0; i < METWL_NUMOF_SHARED2FILES; i++ )
			{
				this->hSrl->hMrcSpecialList->hShared2SizeArray[i] = 0;
			}
		}
	} //if( bCheck )

} // loadAppendInit()