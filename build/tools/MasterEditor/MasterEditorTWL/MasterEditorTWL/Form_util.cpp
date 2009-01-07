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
			this->gridError->Rows->Add( err->getAll(this->isJapanese()) );
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
			this->gridWarn->Rows->Add( err->getAll(this->isJapanese()) );
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
			if( !err->IsEnableModify )	// �C���\�ȏ��͕\�����Ȃ�
			{
				this->gridError->Rows->Add( err->getAll(this->isJapanese()) );
				this->colorGridError( err );
			}
		}
	}
	if( this->hErrorList != nullptr )
	{
		for each( RCMrcError ^err in this->hErrorList )
		{
			this->gridError->Rows->Add( err->getAll(this->isJapanese()) );
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
			if( !err->IsEnableModify )
			{
				this->gridWarn->Rows->Add( err->getAll(this->isJapanese()) );
				this->colorGridWarn( err );
			}
		}
	}
	if( this->hWarnList != nullptr )
	{
		for each( RCMrcError ^err in this->hWarnList )
		{
			this->gridWarn->Rows->Add( err->getAll(this->isJapanese()) );
			this->colorGridWarn( err );
		}
	}
}

// �Z���̐F��ς���
void Form1::colorGridError( RCMrcError ^err )
{
	if( err->IsAffectRom && !err->IsEnableModify )		// SRL�Ɋ֌W����ŏC���s��
	{
		System::Int32 last = this->gridError->Rows->Count - 2;	// �ǉ�����̍s
		this->gridError->Rows[ last ]->DefaultCellStyle->ForeColor = System::Drawing::Color::Red;
	}
	else if( err->IsAffectRom && err->IsEnableModify )	// SRL�Ɋ֌W����ŏC����
	{
		System::Int32 last = this->gridError->Rows->Count - 2;
		this->gridError->Rows[ last ]->DefaultCellStyle->ForeColor = System::Drawing::Color::Blue;
	}
}
void Form1::colorGridWarn( RCMrcError ^err )
{
	if( err->IsAffectRom && !err->IsEnableModify )
	{
		System::Int32 last = this->gridWarn->Rows->Count - 2;
		this->gridWarn->Rows[ last ]->DefaultCellStyle->ForeColor = System::Drawing::Color::Red;
	}
	else if( err->IsAffectRom && err->IsEnableModify )
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
		if( !err->IsAffectRom )
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
		if( !err->IsEnableModify )	// ���ׂ�SRL�o�C�i���ɉe������
			count++;
	}

	// SRL�o�C�i���ɉe������G���[�̒���
	// �C���G���[���t�H�[�����͂ɂ���ďC������Ă��邩�J�E���g
	// (�G���[���X�g���X�V����Ă��邱�Ƃ��O��)
	for each( RCMrcError ^err in this->hErrorList )
	{
		if( err->IsAffectRom )		// �C���s�G���[�͑��݂��Ȃ�
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
		if( err->IsAffectRom )		// �C���s�G���[�͑��݂��Ȃ�
			count++;
	}
	return (count == 0);
}

// ----------------------------------------------
// �t�@�C�����샆�[�e�B���e�B
// ----------------------------------------------

// �t�@�C�����_�C�A���O�Ŏ擾
// @arg [in] �g���q�t�B���^
// @ret �擾�����t�@�C���� �G���[�̂Ƃ� nullptr
System::String^ Form1::openFileDlg( System::String ^filter )
{
	System::Windows::Forms::OpenFileDialog ^dlg = gcnew (OpenFileDialog);
	if( System::String::IsNullOrEmpty( this->prevDir ) || !System::IO::Directory::Exists( this->prevDir ) )
	{
		dlg->InitialDirectory = System::Environment::GetFolderPath( System::Environment::SpecialFolder::Desktop );
	}
	else
	{
		dlg->InitialDirectory = this->prevDir;	// �O�ɑI�񂾃f�B���N�g�����f�t�H���g�ɂ���
	}
	dlg->Filter      = filter;
	dlg->FilterIndex = 1;
	dlg->RestoreDirectory = true;

	if( dlg->ShowDialog() != System::Windows::Forms::DialogResult::OK )
	{
		return nullptr;
	}
	this->prevDir = System::IO::Path::GetDirectoryName( dlg->FileName );	// �f�t�H���g�f�B���N�g���̍X�V
	return System::String::Copy(dlg->FileName);
}

// �Z�[�u����t�@�C�����_�C�A���O�Ŏ擾
// @arg [in] �g���q�t�B���^
// @arg [in] �t�@�C���̊g���q���s���ȂƂ��ɒǉ�����Ƃ��̐������g���q
// @ret �擾�����t�@�C���� �G���[�̂Ƃ� nullptr
System::String^ Form1::saveFileDlg( System::String ^filter, System::String ^extension )
{
	System::String ^retfile;
	System::Windows::Forms::SaveFileDialog ^dlg = gcnew (SaveFileDialog);

	if( System::String::IsNullOrEmpty( this->prevDir ) || !System::IO::Directory::Exists( this->prevDir ) )
	{
		dlg->InitialDirectory = System::Environment::GetFolderPath( System::Environment::SpecialFolder::Desktop );
	}
	else
	{
		dlg->InitialDirectory = this->prevDir;
	}
	dlg->Filter      = filter;
	dlg->FilterIndex = 1;
	dlg->RestoreDirectory = true;

	if( dlg->ShowDialog() != System::Windows::Forms::DialogResult::OK )
	{
		return nullptr;
	}
	this->prevDir = System::IO::Path::GetDirectoryName( dlg->FileName );
	retfile = dlg->FileName;
	if( !System::String::IsNullOrEmpty(extension) && !(dlg->FileName->EndsWith( extension )) )
	{
		retfile += extension;
	}
	return retfile;
}

// �Z�[�u����f�B���N�g�����_�C�A���O�Ŏ擾
// @ret �擾�����f�B���N�g����(\\�ŏI���悤�ɒ��������) �G���[�̂Ƃ�nullptr
System::String^ Form1::saveDirDlg( System::String ^msgJ, System::String ^msgE )
{
	System::String ^dir;
	System::Windows::Forms::FolderBrowserDialog ^dlg = gcnew (System::Windows::Forms::FolderBrowserDialog);

	// �f�t�H���g�̃t�H���_��O�ɑI�������t�H���_�ɂ���
	dlg->RootFolder = System::Environment::SpecialFolder::Desktop;
	if( System::String::IsNullOrEmpty( this->prevDir ) || !System::IO::Directory::Exists( this->prevDir ) )
	{
		dlg->SelectedPath = System::Environment::GetFolderPath( System::Environment::SpecialFolder::Desktop );
	}
	else
	{
		dlg->SelectedPath = this->prevDir;
	}
	dlg->ShowNewFolderButton = true;

	// �_�C�A���O�̏㕔�ɏo�郁�b�Z�[�W
	if( this->isEnglish() && msgE )
	{
		dlg->Description = msgE;
	}
	else if( msgJ )
	{
		dlg->Description = msgJ;
	}

	if( dlg->ShowDialog() != System::Windows::Forms::DialogResult::OK )
	{
		return nullptr;
	}

	if( !dlg->SelectedPath->EndsWith("\\") )
	{
		dir = dlg->SelectedPath + "\\";
	}
	else
	{
		dir = System::String::Copy(dlg->SelectedPath);
	}
	this->prevDir = dir;	// �I�������t�H���_�����̃t�H���_�ɂ���
	return dir;
}

// �t�@�C�������݂��邩�𒲂ׂď㏑���m�F������
bool Form1::isOverwriteFile( System::String ^path )
{
	if( System::IO::File::Exists( path ) )
	{
		System::String ^msg;
		if( this->isJapanese() )
		{
			msg = gcnew System::String( path + "�͂��łɑ��݂��܂��B�㏑�����܂���?" );
		}
		else
		{
			msg = gcnew System::String( path + "already exists. Overwrite it?" );
		}

		if( MessageBox::Show( msg, "Information", MessageBoxButtons::YesNo, MessageBoxIcon::None ) 
			== System::Windows::Forms::DialogResult::No )
		{
			return false;
		}
	}
	return true;	// �t�@�C�������݂��Ȃ��ꍇ || �㏑��OK�̏ꍇ �t�@�C�����쐬���Ă��悢
}


// end of file