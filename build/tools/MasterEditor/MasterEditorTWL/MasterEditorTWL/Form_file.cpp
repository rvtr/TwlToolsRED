// ----------------------------------------------
// �t�@�C����R/W
// ----------------------------------------------

#include "stdafx.h"
#include <apptype.h>
#include "common.h"
#include "srl.h"
#include "deliverable.h"
#include "crc_whole.h"
#include "utility.h"
#include "lang.h"
#include "FormError.h"
#include "Form1.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace MasterEditorTWL;

// �ݒ�t�@�C���̓ǂݍ���
void Form1::loadInit(void)
{
	System::Xml::XmlDocument ^doc = gcnew System::Xml::XmlDocument();

	// xml�t�@�C���̓ǂݍ���
	try
	{
		doc->Load( "../resource/ini.xml" );
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
	this->hIsReadOnly = System::Boolean( bReadOnly );
	if( bReadOnly )
	{
		this->readOnly();
	}

	// <lang>�^�O
	if( MasterEditorTWL::isXmlEqual( root, "lang", "E" ) )
	{
		this->stripItemEnglish->Checked  = true;
		this->stripItemJapanese->Checked = false;
		this->changeEnglish();
	}

	// <output>�^�O
	System::Boolean bXML = MasterEditorTWL::isXmlEqual( root, "output", "XML" );

	// <spcheck>�^�O
	System::Boolean bCheck = MasterEditorTWL::isXmlEqual( root, "spcheck", "ON" );

	if( bCheck )	// �`�F�b�N����Ƃ��̂ݒǉ��`�F�b�N���ڂ�ݒ�
	{
		// �`�F�b�N���邩�ǂ���
		this->hSrl->hMrcSpecialList->hIsCheck = gcnew System::Boolean( true );

		// SDK
		try
		{
			u32 major   = System::UInt32::Parse( MasterEditorTWL::getXPathText( root, "/init/sdk/major" ) );
			u32 minor   = System::UInt32::Parse( MasterEditorTWL::getXPathText( root, "/init/sdk/minor" ) );
			u32 relstep = System::UInt32::Parse( MasterEditorTWL::getXPathText( root, "/init/sdk/relstep" ) );
			u32 sdkver  = (major << 24) | (minor << 16) | (relstep & 0xFFFF);
			this->hSrl->hMrcSpecialList->hSDKVer = gcnew System::UInt32( sdkver );
		}
		catch ( System::Exception ^ex )
		{
			(void)ex;
			this->errMsg( "�ݒ�t�@�C������SDK�o�[�W�������ǂݍ��߂܂���ł����B�o�[�W������0�Ƃ݂Ȃ���܂��B", 
				          "SDK ver. can't be read from setting file. Therefore it is set by 0." );
			this->hSrl->hMrcSpecialList->hSDKVer = gcnew System::UInt32( 0 );
		}

		// Shared2File
		try
		{
			System::Int32 i;
			for( i=0; i < METWL_NUMOF_SHARED2FILES; i++ )
			{
				u32 size = System::UInt32::Parse( MasterEditorTWL::getXPathText( root, "/init/shared2/size" + i.ToString() ) );
				this->hSrl->hMrcSpecialList->hShared2SizeArray[i] = gcnew System::UInt32( size );
			}
		}
		catch ( System::Exception ^ex )
		{
			(void)ex;
			this->errMsg( "�ݒ�t�@�C������Shared2�t�@�C���T�C�Y���ǂݍ��߂܂���ł����B�T�C�Y�͂��ׂ�0�Ƃ݂Ȃ���܂��B", 
				          "One of shared2 file sizes can't be read from setting file. Therefore they are set by 0." );
			System::Int32 i;
			for( i=0; i < METWL_NUMOF_SHARED2FILES; i++ )
			{
				this->hSrl->hMrcSpecialList->hShared2SizeArray[i] = gcnew System::UInt32( 0 );
			}
		}
	} //if( bCheck )

	if( bReadOnly || bXML | bCheck )
	{
		System::String ^msgJ = gcnew System::String("[���샂�[�h]");
		System::String ^msgE = gcnew System::String("[Processing Mode]");
		if( bReadOnly )
		{
			msgJ += "\n���[�h�I�����[���[�h";
			msgE += "\nRead Only Mode";
		}
		if( bXML )
		{
			msgJ += "\nXML�o�̓��[�h";
			msgE += "\nXML Output Mode";
		}
		if( bCheck )
		{
			msgJ += "\n�ǉ��`�F�b�N���[�h";
			msgE += "\nExtra Check Mode";
		}
		this->sucMsg( msgJ, msgE );
	}
} // loadInit()

// SRL�̃I�[�v��
System::Void Form1::loadSrl( System::String ^filename )
{
	ECSrlResult result = this->hSrl->readFromFile( filename );
	if( result != ECSrlResult::NOERROR )
	{
		switch( result )
		{
			case ECSrlResult::ERROR_PLATFORM:
				this->errMsg( "�{�c�[����TWL�p�ł��BNTR��pROM�Ȃǂ�TWL��Ή�ROM��ǂݍ��ނ��Ƃ͂ł��܂���B",
							  "This tool can only read TWL ROM. This can't read an other data e.g. NTR limited ROM." );
			break;

			case ECSrlResult::ERROR_SIGN_DECRYPT:
			case ECSrlResult::ERROR_SIGN_VERIFY:
				this->errMsg( "�s����ROM�f�[�^�ł��BTWL�Ή�/��pROM�łȂ���ROM�f�[�^�������񂳂�Ă���\��������܂��B",
							  "Illegal ROM data. It is not for TWL ROM, or is altered illegally." );
			break;

			default:
				this->errMsg( "ROM�f�[�^�t�@�C���̓ǂݍ��݂Ɏ��s���܂����B\n�ēxROM�f�[�^��ǂݍ��܂��Ă��������B", 
					          "Reading the ROM data file failed. \nPlease read a ROM data file again, with \"Open a ROM data file\"" );
			break;
		}
		return;
	}
	this->tboxFile->Text = filename;

	// GUI��ROM�����i�[
	this->setSrlForms();
	if( *(this->hIsReadOnly) )
	{
		this->readOnly();	// ���[�h�I�����[���[�h�̂Ƃ��t�H�[����Enable�ɂ���
	}

	// SRL�ɓo�^����Ȃ�ROM�d�l�̃t�H�[�������߂�{�^���œǂݍ��ݎ��̏�Ԃɖ߂�������
	// �ǂݍ��ݎ��̏�Ԃ�SRL�ɕۑ����Ă����Ȃ��̂őޔ����Ă���
	this->saveOtherForms();

	// �S�̂�CRC���Z�o
	u16  crc;
	if( !getWholeCRCInFile( filename, &crc ) )
	{
		this->errMsg( "ROM�f�[�^��CRC�v�Z�Ɏ��s���܂����BROM�f�[�^�̓ǂݍ��݂̓L�����Z������܂����B",
			          "Calculating CRC of the ROM data failed. Therefore reading ROM data is canceled." );
		return;
	}
	System::UInt16 ^hcrc = gcnew System::UInt16( crc );
	this->tboxWholeCRC->Clear();
	this->tboxWholeCRC->AppendText( "0x" );
	this->tboxWholeCRC->AppendText( hcrc->ToString("X") );

	// �ǂݍ��ݎ��G���[��o�^����
	this->rErrorReading->Checked = true;
	this->setGridError();
	this->setGridWarn();
	//if( this->hSrl->hErrorList->Count > 0 )
	//{
	//	this->errMsg( "ROM�f�[�^�ɃG���[������܂��B�u�G���[���v�^�u�����m�F���������B",
	//				  "ROM data include error. Please look the tab \"Setting Error\"." );
	//	return;
	//}

	return;
} // loadSrl()

// SRL�̕ۑ�
System::Boolean Form1::saveSrl( System::String ^filename )
{
	// �R�s�[�����t�@�C����ROM�w�b�_���㏑��
	if( !this->saveSrlCore( filename ) )
	{
		return false;
	}

	// �ă��[�h
	this->loadSrl( filename );
	return true;
} // saveSrl()

// SRL�̕ۑ�
System::Boolean Form1::saveSrlCore( System::String ^filename )
{
	// ROM�����t�H�[������擾����SRL�o�C�i���ɔ��f������
	this->setSrlProperties();

	// �t�@�C�����R�s�[
	if( !(filename->Equals( this->tboxFile->Text )) )
	{
		System::IO::File::Copy( this->tboxFile->Text, filename, true );
	}

	// �R�s�[�����t�@�C����ROM�w�b�_���㏑��
	if( this->hSrl->writeToFile( filename ) != ECSrlResult::NOERROR )
	{
		return false;
	}
	return true;
}

// �~�h���E�F�A���X�g�̍쐬
System::Void Form1::makeMiddlewareListXml(System::Xml::XmlDocument^ doc)
{
	System::Xml::XmlElement ^root = doc->CreateElement( "twl-master-editor" );
	System::Reflection::Assembly ^ass = System::Reflection::Assembly::GetEntryAssembly();
	root->SetAttribute( "version", this->getVersion() );
	doc->AppendChild( root );

	// �Q�[�����
	System::Xml::XmlElement ^game = doc->CreateElement( "game" );
	root->AppendChild( game );
	if( System::String::IsNullOrEmpty( this->tboxProductName->Text ) )
	{
		MasterEditorTWL::appendXmlTag( doc, game, "product-name", this->tboxTitleName->Text );	// ���i���������͂̂Ƃ��̓\�t�g�^�C�g���ő�p
	}
	else
	{
		MasterEditorTWL::appendXmlTag( doc, game, "product-name", this->tboxProductName->Text );
	}
	MasterEditorTWL::appendXmlTag( doc, game, "title-name",   this->tboxTitleName->Text );
	MasterEditorTWL::appendXmlTag( doc, game, "game-code",    this->tboxGameCode->Text );
	MasterEditorTWL::appendXmlTag( doc, game, "rom-version",  this->tboxRemasterVer->Text );
	MasterEditorTWL::appendXmlTag( doc, game, "submit-version", System::Decimal::ToByte(this->numSubmitVersion->Value).ToString("X") );

	// �~�h���E�F�A���X�g
	System::Xml::XmlElement ^midlist = doc->CreateElement( "middleware-list" );
	root->AppendChild( midlist );
	if( this->hSrl->hLicenseList != nullptr )
	{
		for each( RCLicense ^lic in this->hSrl->hLicenseList )
		{
			System::Xml::XmlElement ^mid = doc->CreateElement( "middleware" );
			MasterEditorTWL::appendXmlTag( doc, mid, "publisher", lic->Publisher );
			MasterEditorTWL::appendXmlTag( doc, mid, "name", lic->Name );
			midlist->AppendChild( mid );
		}
	}
}

// �~�h���E�F�A���X�g�̕ۑ�
System::Boolean Form1::saveMiddlewareListXml( System::String ^filename )
{
	System::Xml::XmlDocument ^doc = gcnew System::Xml::XmlDocument();
	doc->AppendChild( doc->CreateXmlDeclaration("1.0","UTF-8",nullptr) );

	this->makeMiddlewareListXml( doc );

	try
	{
		doc->Save( filename );
	}
	catch ( System::Exception ^ex )
	{
		(void)ex;
		return false;
	}
	return true;
}

// �~�h���E�F�A���X�g�̕ۑ�
System::Boolean Form1::saveMiddlewareListXmlEmbeddedXsl( System::String ^filename )
{
	System::Xml::XmlDocument ^doc = gcnew System::Xml::XmlDocument();
	doc->AppendChild( doc->CreateXmlDeclaration("1.0","UTF-8",nullptr) );

	// XSL�𖄂ߍ��ނ��߂̐錾����
	System::Xml::XmlProcessingInstruction ^proc = doc->CreateProcessingInstruction( "xml-stylesheet", "type='text/xml' href='#stylesheet'" );
	doc->AppendChild( proc );
	System::Xml::XmlDocumentType ^doctype = doc->CreateDocumentType( "twl-master-editor", nullptr, nullptr, "<!ATTLIST xsl:stylesheet id ID #REQUIRED>" );
	doc->AppendChild( doctype );

	this->makeMiddlewareListXml( doc );

	// ���ߍ��݃X�^�C���V�[�g
	System::Xml::XmlDocument ^xsl  = gcnew System::Xml::XmlDocument();
	System::Xml::XmlElement  ^root = doc->DocumentElement;
	try
	{
		xsl->Load("../resource/middleware_e.xsl");
		root->AppendChild( doc->ImportNode(xsl->DocumentElement, true) );
	}
	catch ( System::Exception ^ex )
	{
		(void)ex;
		return false;
	}

	try
	{
		doc->Save( filename );
	}
	catch ( System::Exception ^ex )
	{
		(void)ex;
		return false;
	}
	return true;
}

// �~�h���E�F�A���X�g�̕ۑ�(XML->HTML�ϊ�)
System::Boolean Form1::saveMiddlewareListHtml( System::String ^filename )
{
	System::Xml::Xsl::XslCompiledTransform ^xslt = gcnew System::Xml::Xsl::XslCompiledTransform;
	System::String ^tmpxml = gcnew System::String( "middleware-tmp.xml" );

	if( !saveMiddlewareListXml(tmpxml) )
	{
		return false;
	}

	try
	{
		xslt->Load("../resource/middleware.xsl");
		xslt->Transform( tmpxml, filename );
	}
	catch (System::Exception ^s)
	{
		(void)s;
		System::IO::File::Delete( tmpxml );
		return false;
	}
	System::IO::File::Delete( tmpxml );
	return true;
}
