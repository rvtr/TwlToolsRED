// ----------------------------------------------
// �ݒ�t�@�C���̓ǂݍ���
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
// �ʏ�ݒ�t�@�C���̓ǂݍ���
// ----------------------------------------------

void Form1::loadInit(void)
{
	System::Xml::XmlDocument ^doc = gcnew System::Xml::XmlDocument();

	// xml�t�@�C���̓ǂݍ���
	try
	{
		doc->Load( this->getIniFile() );
	}
	catch( System::IO::FileNotFoundException ^s )
	{
		(void)s;
		this->errMsg( "�ݒ�t�@�C����������܂���B", "Setting file is not found." );
		return;
	}
	catch( System::Exception ^s )
	{
		(void)s;
		this->errMsg( "�ݒ�t�@�C�����J�����Ƃ��ł��܂���ł����B", "Setting file can't be opened." );
		return;
	}

	// <init>�^�O : ���[�g
	System::Xml::XmlElement ^root = doc->DocumentElement;

	// <rw>�^�O
	System::Boolean bReadOnly = MasterEditorTWL::isXmlEqual( root, "rw", "r" );
	this->IsReadOnly = bReadOnly;
	if( bReadOnly )
	{
		this->readOnly();
	}

	// <access>�^�O
	{
		// Shared2�t�@�C���̃A�N�Z�X����(�t�@�C�����ƂɎw��)
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
					this->hSrl->hMrcExternalCheckItems->hIsPermitShared2Array[ number ] = true;		// �ݒ�t�@�C���Ŏw�肳��Ă���ԍ��̂݋���
				}
			}
		}

		// �m�[�}���W�����v�̃A�N�Z�X����
		System::String ^isNJ = MasterEditorTWL::getXPathText( root, "/init/access/normalJump" );
		if( isNJ && isNJ->Equals( "Permit" ) )
		{
			this->hSrl->hMrcExternalCheckItems->IsPermitNormalJump = true;
		}
	}
	// <access>�^�O�ɕt�����Ĕ�\�����ڂ�\�����邩�ǂ��������߂�
	{
		// Shared2�t�@�C��
		System::Boolean bVisible;
		for each( System::Boolean permit in this->hSrl->hMrcExternalCheckItems->hIsPermitShared2Array )
		{
			if( permit && !this->gboxShared2Size->Visible )
			{
				bVisible = true;		// �ǂꂩ�ЂƂł�true�Ȃ�O���[�v�{�b�N�X�S�̂�\������
			}
		}
		this->gboxShared2Size->Visible = bVisible;
		this->cboxIsShared2->Visible   = bVisible;
		if( bVisible )
		{
			int i;
			for( i=0; i < METWL_NUMOF_SHARED2FILES; i++ )
			{
				// �O���[�v�{�b�N�X�S�̂��\������Ă�����S�t�@�C���̍��ڂ��\�������̂�
				// �A�N�Z�X������Ă��Ȃ��t�@�C���̂ݔ�\���ɂ���
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

		// �m�[�}���W�����v
		this->changeVisibleGboxExFlags( this->hSrl->hMrcExternalCheckItems->IsPermitNormalJump );
	}

	// <shared2>�^�O : Shared2�t�@�C���T�C�Y�̐^�l(�A�N�Z�X������Ă���t�@�C���̂݋L��)
	System::Int32 i;
	for( i=0; i < METWL_NUMOF_SHARED2FILES; i++ )
	{
		System::String ^str = MasterEditorTWL::getXPathText( root, "/init/shared2/size" + i.ToString() );
		if( str )
		{
			this->hSrl->hMrcExternalCheckItems->hShared2SizeArray[i] = System::UInt32::Parse( str );
		}
		else		// �A�N�Z�X������Ă��Ȃ��t�@�C���͔���J�Ȃ̂ŃT�C�Y���ݒ�t�@�C���ɋL�ڂ���Ȃ�
		{
			this->hSrl->hMrcExternalCheckItems->hShared2SizeArray[i] = 0;
		}
	}

	// <reserved-area>�^�O : �\��̈�͈̔͂��L��
	System::Xml::XmlNodeList ^list = root->SelectNodes( "/init/reserved-list/reserved" );
	System::Collections::IEnumerator^ iter = list->GetEnumerator();
	while( iter->MoveNext() )
	{
		System::Xml::XmlNode ^area = safe_cast<System::Xml::XmlNode^>(iter->Current);
		System::Xml::XmlNode ^begin = area->SelectSingleNode( "begin" );	// ���΃p�X
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
// �ǉ��ݒ�t�@�C���̓ǂݍ���
// ----------------------------------------------

void Form1::loadAppendInit(void)
{
	System::String ^initfile = this->getAppendIniFile();

	// �ʏ�͑��݂��Ȃ�
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
		this->errMsg( "�ǉ��ݒ�t�@�C����������܂���B", "Setting file is not found." );
		return;
	}
	catch( System::Exception ^s )
	{
		(void)s;
		this->errMsg( "�ǉ��ݒ�t�@�C�����J�����Ƃ��ł��܂���ł����B", "Setting file can't be opened." );
		return;
	}
	System::Xml::XmlElement ^root = doc->DocumentElement;

	// <visible>�^�O
	System::Boolean bVisible = MasterEditorTWL::isXmlEqual( root, "visible", "ON" );
	this->changeVisibleForms( bVisible );
	this->changeVisibleGboxExFlags( bVisible );

	// <spcheck>�^�O
	System::Boolean bCheck = MasterEditorTWL::isXmlEqual( root, "spcheck", "ON" );

	if( bCheck )	// �`�F�b�N����Ƃ��̂ݒǉ��`�F�b�N���ڂ�ݒ�
	{
		// �`�F�b�N���邩�ǂ���
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
			this->errMsg( "�ݒ�t�@�C������SDK�o�[�W�������ǂݍ��߂܂���ł����B�o�[�W������0�Ƃ݂Ȃ���܂��B", 
				          "SDK ver. can't be read from setting file. Therefore it is set by 0." );
			this->hSrl->hMrcExternalCheckItems->SDKVer = 0;
		}

	} //if( bCheck )

} // loadAppendInit()