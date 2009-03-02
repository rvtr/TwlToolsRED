#include "stdafx.h"

// RCMessageBank�N���X�̎���

#include <cstring>
#include <cstdio>
#include "message_bank.h"
#include "utility.h"

using namespace MasterEditorTWL;

// ****************************************************************
// RCMessageBank �N���X
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
// �O���t�@�C�����烁�b�Z�[�W��ǂݏo��
// -------------------------------------------------------------------
void RCMessageBank::loadMessage( System::String ^filepath, System::String ^lang )
{
	System::Xml::XmlDocument ^doc = gcnew System::Xml::XmlDocument();
	doc->Load( filepath );
	System::Xml::XmlElement ^root = doc->DocumentElement;

	// ����ɂ����XML���[�g�̊i�[���ύX
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
// ����̃��b�Z�[�W��Ԃ�
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

	// �ǂ̃t�@�C���������^�O�������Ă��邱�Ƃ��O��
	System::String ^msg = MasterEditorTWL::getXPathText( root, "/message/" + tag );
	return msg;
}