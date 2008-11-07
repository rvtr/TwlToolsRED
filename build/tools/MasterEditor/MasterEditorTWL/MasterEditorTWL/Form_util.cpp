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

// ----------------------------------------------
// �t�H�[���̃`�F�b�N
// ----------------------------------------------

// �e�L�X�g���͂�����Ă��邩�`�F�b�N
System::Boolean Form1::checkTextForm( System::String ^formtext, System::String ^labelJ, System::String ^labelE, System::Boolean affectRom )
{
	System::String ^msgJ = gcnew System::String( "���͂���Ă��܂���B" );
	System::String ^msgE = gcnew System::String( "No item is set. Please retry to input." );

	System::String ^tmp = formtext->Replace( " ", "" );		// �X�y�[�X�݂̂̕�������G���[
	if( (formtext == nullptr) || formtext->Equals("") || tmp->Equals("") )
	{
		this->hErrorList->Add( gcnew RCMrcError( labelJ, METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE, msgJ, labelE, msgE, true, affectRom ) );
		return false;
	}
	return true;
}
// ���l���͂����킩�ǂ����`�F�b�N
System::Boolean Form1::checkNumRange( 
	System::Int32 val, System::Int32 min, System::Int32 max, System::String ^labelJ, System::String ^labelE, System::Boolean affectRom )
{
	System::String ^msgJ = gcnew System::String( "�l�͈̔͂��s���ł��B��蒼���Ă��������B" );
	System::String ^msgE = gcnew System::String( "Invalidate range of value. Please retry." );

	if( (val < min) || (max < val) )
	{
		this->hErrorList->Add( gcnew RCMrcError( labelJ, METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE, msgJ, labelE, msgE, true, affectRom ) );
		return false;
	}
	return true;
}
System::Boolean Form1::checkNumRange( System::String ^strval, System::Int32 min, System::Int32 max, 
							          System::String ^labelJ, System::String ^labelE, System::Boolean affectRom )
{
	try
	{
		System::Int32  i = System::Int32::Parse(strval);
		return (this->checkNumRange( i, min, max, labelJ, labelE, affectRom ));
	}
	catch ( System::FormatException ^ex )
	{
		(void)ex;
		return (this->checkNumRange( max+1, min, max, labelJ, labelE, affectRom ));		// �K�����s����悤�� max+1 ������
	}
}
// �R���{�{�b�N�X���`�F�b�N
System::Boolean Form1::checkBoxIndex( System::Windows::Forms::ComboBox ^box, System::String ^labelJ, System::String ^labelE, System::Boolean affectRom )
{
	System::String ^msgJ = gcnew System::String( "�I������Ă��܂���B" );
	System::String ^msgE = gcnew System::String( "One item is not selected." );
	
	if( box->SelectedIndex < 0 )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			labelJ, METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE, msgJ, labelE, msgE, true, affectRom ) );
	}
	return true;
}


// --------------------------------------------------------
// �G���[���̓o�^
// --------------------------------------------------------

// �ǂݍ��ݎ��G���[�̓o�^
void Form1::setGridError( void )
{
	this->gridError->Rows->Clear();
	if( this->hSrl->hErrorList != nullptr )
	{
		for each( RCMrcError ^err in this->hSrl->hErrorList )
		{
			this->gridError->Rows->Add( err->getAll( this->stripItemJapanese->Checked ) );
			this->colorGridError( err );
		}
	}
}

void Form1::setGridWarn( void )
{
	this->gridWarn->Rows->Clear();
	if( this->hSrl->hWarnList != nullptr )
	{
		for each( RCMrcError ^err in this->hSrl->hWarnList )
		{
			this->gridWarn->Rows->Add( err->getAll( this->stripItemJapanese->Checked ) );
			this->colorGridWarn( err );
		}
	}
}

// �ǂݍ��ݎ��Ɍ��o�����C���\�G���[�Ɍ��݂̓��͂𔽉f
void Form1::overloadGridError( void )
{
	this->gridError->Rows->Clear();
	if( this->hSrl->hErrorList != nullptr )
	{
		for each( RCMrcError ^err in this->hSrl->hErrorList )
		{
			if( !err->EnableModify )	// �C���\�ȏ��͕\�����Ȃ�
			{
				this->gridError->Rows->Add( err->getAll( this->stripItemJapanese->Checked ) );
				this->colorGridError( err );
			}
		}
	}
	if( this->hErrorList != nullptr )
	{
		for each( RCMrcError ^err in this->hErrorList )
		{
			this->gridError->Rows->Add( err->getAll( this->stripItemJapanese->Checked ) );
			this->colorGridError( err );
		}
	}
}
void Form1::overloadGridWarn( void )
{
	this->gridWarn->Rows->Clear();
	if( this->hSrl->hWarnList != nullptr )
	{
		for each( RCMrcError ^err in this->hSrl->hWarnList )
		{
			if( !err->EnableModify )
			{
				this->gridWarn->Rows->Add( err->getAll( this->stripItemJapanese->Checked ) );
				this->colorGridWarn( err );
			}
		}
	}
	if( this->hWarnList != nullptr )
	{
		for each( RCMrcError ^err in this->hWarnList )
		{
			this->gridWarn->Rows->Add( err->getAll( this->stripItemJapanese->Checked ) );
			this->colorGridWarn( err );
		}
	}
}

// �Z���̐F��ς���
void Form1::colorGridError( RCMrcError ^err )
{
	if( err->AffectRom && !err->EnableModify )		// SRL�Ɋ֌W����ŏC���s��
	{
		System::Int32 last = this->gridError->Rows->Count - 2;	// �ǉ�����̍s
		this->gridError->Rows[ last ]->DefaultCellStyle->ForeColor = System::Drawing::Color::Red;
	}
	else if( err->AffectRom && err->EnableModify )	// SRL�Ɋ֌W����ŏC����
	{
		System::Int32 last = this->gridError->Rows->Count - 2;
		this->gridError->Rows[ last ]->DefaultCellStyle->ForeColor = System::Drawing::Color::Blue;
	}
}
void Form1::colorGridWarn( RCMrcError ^err )
{
	if( err->AffectRom && !err->EnableModify )
	{
		System::Int32 last = this->gridWarn->Rows->Count - 2;
		this->gridWarn->Rows[ last ]->DefaultCellStyle->ForeColor = System::Drawing::Color::Red;
	}
	else if( err->AffectRom && err->EnableModify )
	{
		System::Int32 last = this->gridWarn->Rows->Count - 2;
		this->gridWarn->Rows[ last ]->DefaultCellStyle->ForeColor = System::Drawing::Color::Blue;
	}
}

// �܂Ƃ߂čX�V
void Form1::updateGrid(void)
{
	if( this->rErrorReading->Checked == true )
	{
		this->setGridError();
		this->setGridWarn();
	}
	else
	{
		if( !System::String::IsNullOrEmpty(this->tboxFile->Text) )
		{
			this->hErrorList->Clear();
			this->hWarnList->Clear();
			this->checkSrlForms();
			this->checkDeliverableForms();
			this->overloadGridError();
			this->overloadGridWarn();
		}
	}
}

// ----------------------------------------------
// �G���[�J�E���g
// ----------------------------------------------

// SRL�ɂ͊֌W���Ȃ����ޏ�̃G���[���`�F�b�N
System::Boolean Form1::isValidOnlyDeliverable(void)
{
	System::Int32 count = 0;

	// SRL�N���X�̃G���[���X�g�͂��ׂ�SRL�Ɋ֌W����̂Ń`�F�b�N���Ȃ�
	// -> ���̓G���[�݂̂̃`�F�b�N�ł悢
	for each( RCMrcError ^err in this->hErrorList )
	{
		if( !err->AffectRom )
			count++;
	}
	return (count == 0);
}

// SRL�̃o�C�i���ɉe�����鍀�ڂɃG���[�����邩�`�F�b�N
System::Boolean Form1::isValidAffectRom(void)
{
	System::Int32 count = 0;

	// SRL�N���X�̏C���s�G���[���J�E���g
	// (�C���G���[�͓��͂ɂ���ďC������Ă邩������Ȃ��̂Ń`�F�b�N���Ȃ�)
	for each( RCMrcError ^err in this->hSrl->hErrorList )
	{
		if( !err->EnableModify )	// ���ׂ�SRL�o�C�i���ɉe������
			count++;
	}

	// SRL�o�C�i���ɉe������G���[�̒���
	// �C���G���[���t�H�[�����͂ɂ���ďC������Ă��邩�J�E���g
	// (�G���[���X�g���X�V����Ă��邱�Ƃ��O��)
	for each( RCMrcError ^err in this->hErrorList )
	{
		if( err->AffectRom )		// �C���s�G���[�͑��݂��Ȃ�
			count++;
	}
	return (count == 0);
}

// SRL�̃o�C�i���ɉe�����鍀�ڂ̒��ŏC���\�ȃG���[�������`�F�b�N
System::Boolean Form1::isValidAffectRomModified(void)
{
	System::Int32 count = 0;
	for each( RCMrcError ^err in this->hErrorList )
	{
		if( err->AffectRom )		// �C���s�G���[�͑��݂��Ȃ�
			count++;
	}
	return (count == 0);
}

// end of file