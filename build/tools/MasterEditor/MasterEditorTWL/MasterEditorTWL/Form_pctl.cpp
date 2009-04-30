// ---------------------------------------------------------------------
// ���[�W�����ƃy�A�����^���R���g���[���ݒ�
// ---------------------------------------------------------------------

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

// ---------------------------------------------------------------------
// ���[�W�����ݒ�͕��G�Ȃ̂ŕʂɐ؂�o��
// ---------------------------------------------------------------------

// �t�H�[�����͂�SRL�ɔ��f������
void Form1::setRegionSrlPropaties(void)
{
	this->hSrl->IsRegionJapan     = false;
	this->hSrl->IsRegionAmerica   = false;
	this->hSrl->IsRegionEurope    = false;
	this->hSrl->IsRegionAustralia = false;
	switch( this->combRegion->SelectedIndex )
	{
		case 0:
			this->hSrl->IsRegionJapan = true;
		break;

		case 1:
			this->hSrl->IsRegionAmerica = true;
		break;

		case 2:
			this->hSrl->IsRegionEurope = true;
		break;

		case 3:
			this->hSrl->IsRegionAustralia = true;
		break;

		case 4:
			this->hSrl->IsRegionEurope    = true;
			this->hSrl->IsRegionAustralia = true;
		break;

		case 5:
			this->hSrl->IsRegionAmerica   = true;
			this->hSrl->IsRegionAustralia = true;
		break;

		case 6:
			this->hSrl->IsRegionAmerica   = true;
			this->hSrl->IsRegionEurope    = true;
			this->hSrl->IsRegionAustralia = true;
		break;

#if defined(METWL_VER_APPTYPE_SYSTEM) || defined(METWL_VER_APPTYPE_SECURE) || defined(METWL_VER_APPTYPE_LAUNCHER)
		case 7:
			this->hSrl->IsRegionJapan     = true;
			this->hSrl->IsRegionAmerica   = true;
			this->hSrl->IsRegionEurope    = true;
			this->hSrl->IsRegionAustralia = true;
		break;
#endif //defined(METWL_VER_APPTYPE_SYSTEM) || defined(METWL_VER_APPTYPE_SECURE) || defined(METWL_VER_APPTYPE_LAUNCHER)
		default:
		break;
	}
}

// SRL�����t�H�[���ɔ��f������
void Form1::setRegionForms(void)
{
	System::Boolean isJapan   = this->hSrl->IsRegionJapan;			// ���[�W����
	System::Boolean isAmerica = this->hSrl->IsRegionAmerica;
	System::Boolean isEurope  = this->hSrl->IsRegionEurope;
	System::Boolean isAustralia = this->hSrl->IsRegionAustralia;
	System::Int32  index;
	if( isJapan && !isAmerica && !isEurope && !isAustralia )
		index = 0;
	else if( !isJapan && isAmerica && !isEurope && !isAustralia )
		index = 1;
	else if( !isJapan && !isAmerica && isEurope && !isAustralia )
		index = 2;
	else if( !isJapan && !isAmerica && !isEurope && isAustralia )
		index = 3;
	else if( !isJapan && !isAmerica && isEurope && isAustralia )
		index = 4;
	else if( !isJapan && isAmerica && !isEurope && isAustralia )
		index = 5;
	else if( !isJapan && isAmerica && isEurope && isAustralia )
		index = 6;
	else
		index = -1;	// �s��
#if defined(METWL_VER_APPTYPE_SYSTEM) || defined(METWL_VER_APPTYPE_SECURE) || defined(METWL_VER_APPTYPE_LAUNCHER)
	if( isJapan && isAmerica && isEurope && isAustralia )
	index = 7;
#endif
	this->combRegion->SelectedIndex = index;
	this->maskRatingForms();		// �y�A�����^���R���g���[���p�t�H�[���̕\��/��\���؂�ւ�
}

// ---------------------------------------------------------------------
// �y�A�����^���R���g���[���ݒ�͕��G�Ȃ̂ŕʂɐ؂�o��
// ---------------------------------------------------------------------

// �t�H�[�����͂�SRL�ɔ��f������
void Form1::setRatingSrlProperties(void)
{
	// �e�c�̂̃t�H�[�����͂𔽉f
	this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_CERO ] = this->combCERO->SelectedIndex;
	this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_ESRB ] = this->combESRB->SelectedIndex;
	this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_USK ]  = this->combUSK->SelectedIndex;
	this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_PEGI_GEN ]  = this->combPEGI->SelectedIndex;
	this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_PEGI_PRT ]  = this->combPEGI_PRT->SelectedIndex;
	this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_PEGI_BBFC ] = this->combPEGI_BBFC->SelectedIndex;
	this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_AGCB ] = this->combOFLC->SelectedIndex;

	// ���[�e�B���O�\���s�v���ǂ�����ݒ�
	this->hSrl->IsUnnecessaryRating = this->cboxIsUnnecessaryRating->Checked;
} //setRatingSrlProperties()

// SRL���̃y�A�����^���R���g���[�����𔲂��o���ăt�H�[���ɔ��f������
void Form1::setRatingForms(void)
{
	// ���[�e�B���O�\���s�v���ǂ����𔻒f
	this->cboxIsUnnecessaryRating->Checked = this->hSrl->IsUnnecessaryRating;
	this->changeUnnecessaryRatingForms( true );	// �s�v���ǂ����ɉ����ăt�H�[����ݒ�

	// �e�c�̂̃R���{�{�b�N�X�̃C���f�b�N�X��ݒ�
	this->combCERO->SelectedIndex = this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_CERO ];
	this->combESRB->SelectedIndex = this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_ESRB ];
	this->combUSK->SelectedIndex  = this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_USK ];
	this->combPEGI->SelectedIndex = this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_PEGI_GEN ];
	this->combPEGI_PRT->SelectedIndex  = this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_PEGI_PRT ];
	this->combPEGI_BBFC->SelectedIndex = this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_PEGI_BBFC ];
	this->combOFLC->SelectedIndex = this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_AGCB ];
} //setRatingForms()

// ���[�W������񂩂�y�A�����^���R���g���[���̕ҏW�\�c�̂��}�X�N����
void Form1::maskRatingForms(void)
{
	this->enableRating( this->combCERO, this->labCERO, nullptr );
	this->enableRating( this->combESRB, this->labESRB, nullptr );
	this->enableRating( this->combUSK,  this->labUSK,  nullptr );
	this->enableRating( this->combPEGI, this->labPEGI, nullptr );
	this->enableRating( this->combPEGI_PRT,  this->labPEGI_PRT,  nullptr );
	this->enableRating( this->combPEGI_BBFC, this->labPEGI_BBFC, nullptr );
	this->enableRating( this->combOFLC, this->labOFLC, nullptr );
	switch( this->combRegion->SelectedIndex )
	{
		case 0:
			// ���{
			this->enableRating( this->combCERO, this->labCERO, nullptr );
			this->disableRating( this->combESRB, this->labESRB, nullptr );
			this->disableRating( this->combUSK,  this->labUSK,  nullptr );
			this->disableRating( this->combPEGI, this->labPEGI, nullptr );
			this->disableRating( this->combPEGI_PRT,  this->labPEGI_PRT,  nullptr );
			this->disableRating( this->combPEGI_BBFC, this->labPEGI_BBFC, nullptr );
			this->disableRating( this->combOFLC, this->labOFLC, nullptr );
		break;

		case 1:
			// �k��
			this->disableRating( this->combCERO, this->labCERO, nullptr );
			this->enableRating( this->combESRB,  this->labESRB, nullptr );
			this->disableRating( this->combUSK,  this->labUSK,  nullptr );
			this->disableRating( this->combPEGI, this->labPEGI, nullptr );
			this->disableRating( this->combPEGI_PRT,  this->labPEGI_PRT,  nullptr );
			this->disableRating( this->combPEGI_BBFC, this->labPEGI_BBFC, nullptr );
			this->disableRating( this->combOFLC, this->labOFLC, nullptr );
		break;

		case 2:
			// ���B
			this->disableRating( this->combCERO, this->labCERO, nullptr );
			this->disableRating( this->combESRB, this->labESRB, nullptr );
			this->enableRating( this->combUSK,   this->labUSK,  nullptr );
			this->enableRating( this->combPEGI,  this->labPEGI, nullptr );
			this->enableRating( this->combPEGI_PRT,  this->labPEGI_PRT,  nullptr );
			this->enableRating( this->combPEGI_BBFC, this->labPEGI_BBFC, nullptr );
			this->disableRating( this->combOFLC, this->labOFLC, nullptr );
		break;

		case 3:
			// ���B
			this->disableRating( this->combCERO, this->labCERO, nullptr );
			this->disableRating( this->combESRB, this->labESRB, nullptr );
			this->disableRating( this->combUSK,  this->labUSK,  nullptr );
			this->disableRating( this->combPEGI, this->labPEGI, nullptr );
			this->disableRating( this->combPEGI_PRT,  this->labPEGI_PRT,  nullptr );
			this->disableRating( this->combPEGI_BBFC, this->labPEGI_BBFC, nullptr );
			this->enableRating( this->combOFLC,  this->labOFLC, nullptr );
		break;

		case 4:
			// ���B�ƍ��B
			this->disableRating( this->combCERO, this->labCERO, nullptr );
			this->disableRating( this->combESRB, this->labESRB, nullptr );
			this->enableRating( this->combUSK,   this->labUSK,  nullptr );
			this->enableRating( this->combPEGI,  this->labPEGI, nullptr );
			this->enableRating( this->combPEGI_PRT,  this->labPEGI_PRT,  nullptr );
			this->enableRating( this->combPEGI_BBFC, this->labPEGI_BBFC, nullptr );
			this->enableRating( this->combOFLC,  this->labOFLC, nullptr );
		break;

		case 5:
			// �k�Ăƍ��B
			this->disableRating( this->combCERO, this->labCERO, nullptr );
			this->enableRating( this->combESRB,  this->labESRB, nullptr );
			this->disableRating( this->combUSK,  this->labUSK,  nullptr );
			this->disableRating( this->combPEGI, this->labPEGI, nullptr );
			this->disableRating( this->combPEGI_PRT,  this->labPEGI_PRT,  nullptr );
			this->disableRating( this->combPEGI_BBFC, this->labPEGI_BBFC, nullptr );
			this->enableRating( this->combOFLC,  this->labOFLC, nullptr );
		break;

		case 6:
			// �k�ĂƉ��B�ƍ��B
			this->disableRating( this->combCERO, this->labCERO, nullptr );
			this->enableRating( this->combESRB,  this->labESRB, nullptr );
			this->enableRating( this->combUSK,   this->labUSK,  nullptr );
			this->enableRating( this->combPEGI,  this->labPEGI, nullptr );
			this->enableRating( this->combPEGI_PRT,  this->labPEGI_PRT,  nullptr );
			this->enableRating( this->combPEGI_BBFC, this->labPEGI_BBFC, nullptr );
			this->enableRating( this->combOFLC,  this->labOFLC, nullptr );
		break;

		// �S���[�W�����̂Ƃ��͉���disable�ɂ��Ȃ�
		default:
		break;
	}
} //maskRatingForms()

// �S�c�̂��u���[�e�B���O�\���s�v�v�̐ݒ�/����������
void Form1::changeUnnecessaryRatingForms( System::Boolean bInitial )
{
	if( this->cboxIsUnnecessaryRating->Checked )
	{
		this->unnecessaryRating( this->combCERO );		// �}�X�N�͕ʂ̂Ƃ���ł���̂ł����ł͑S�c�̂�s�v�ɂ��Ă����܂�Ȃ�
		this->unnecessaryRating( this->combESRB );
		this->unnecessaryRating( this->combUSK );
		this->unnecessaryRating( this->combPEGI );
		this->unnecessaryRating( this->combPEGI_PRT );
		this->unnecessaryRating( this->combPEGI_BBFC );
		this->unnecessaryRating( this->combOFLC );
	}
	else
	{
		this->necessaryRating( this->combCERO, bInitial );
		this->necessaryRating( this->combESRB, bInitial );
		this->necessaryRating( this->combUSK, bInitial );
		this->necessaryRating( this->combPEGI, bInitial );
		this->necessaryRating( this->combPEGI_PRT, bInitial );
		this->necessaryRating( this->combPEGI_BBFC, bInitial );
		this->necessaryRating( this->combOFLC, bInitial );
	}
}

// �y�A�����^���R���g���[���֘A�̃t�H�[�����͂����������������ݑO�`�F�b�N
void Form1::checkRatingForms( System::Boolean inRegion, System::Windows::Forms::ComboBox ^comb, System::String ^ogn )
{
	// ���[�W�����Ɋ܂܂�Ă��Ȃ��Ƃ�: 0�N���A���ۏ؂����̂Ń`�F�b�N�K�v�Ȃ�
	if( !inRegion )
		return;

	// �ݒ肳��Ă��Ȃ��Ƃ��G���[
	if( (comb->SelectedIndex < 0) || (comb->SelectedIndex >= comb->Items->Count)  )
	{
		this->hErrorList->Add( this->makeErrorMsg(true, "LabelRating", "RatingInput", ogn) );
	}

	// �R�����̂Ƃ��x��
	if( comb->SelectedIndex == (comb->Items->Count - 1) )
	{
		this->hWarnList->Add( this->makeErrorMsg(true, "LabelRating", "RatingPendingSelect", ogn) );
	}
} //checkRatingForms()

// �y�A�����^���R���g���[�������N���A����
void Form1::clearRating( System::Windows::Forms::ComboBox ^comb )
{
	comb->SelectedIndex = -1;	// �󔒂ɂ���
}


// �y�A�����^���R���g���[������ҏW�ł���悤�ɂ���
void Form1::enableRating( System::Windows::Forms::ComboBox ^comb, 
							System::Windows::Forms::Label    ^lab1, 
							System::Windows::Forms::Label    ^lab2 )
{
	comb->Enabled   = true;
	comb->Visible   = true;
	lab1->Visible   = true;
	if( lab2 != nullptr )
	{
		lab2->Visible   = true;
	}
	// �{���͕s�K�v�Ȑݒ�
	//�u���[�e�B���O�\���s�v�v��Enable��false�ɂ���̂Ń��[�W�����Ɋ܂܂�Ă���̂��ǂ�����m��p���Ȃ�
	// (Visible�͐e�^�u���ς��Ə����False�ɂȂ��Ă��܂�)
	// ���̂��ߖ��֌W�Ȑݒ��ς��邱�ƂŃ��[�W�����Ɋ܂܂�Ă��邱�Ƃ�\������
	comb->FlatStyle = System::Windows::Forms::FlatStyle::Standard;
}

// �y�A�����^���R���g���[������ҏW�ł��Ȃ�����
void Form1::disableRating( System::Windows::Forms::ComboBox ^comb, 
							 System::Windows::Forms::Label    ^lab1, 
							 System::Windows::Forms::Label    ^lab2 )
{
	this->clearRating( comb );
	comb->Enabled   = false;
	comb->Visible   = false;
	lab1->Visible   = false;
	if( lab2 != nullptr )
	{
		lab2->Visible   = false;
	}
	comb->FlatStyle = System::Windows::Forms::FlatStyle::Popup;		// Disable��\��
}

// �u���[�e�B���O�\���s�v�v�ƕ\�����ĕҏW�ł��Ȃ�����
void Form1::unnecessaryRating( System::Windows::Forms::ComboBox ^comb )
{
	comb->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDown;	// �ꎞ�I�Ƀe�L�X�g���͉\�ɂ���
	comb->SelectedIndex = -1;	// �����I������Ă��Ȃ��Ƃ݂Ȃ�
	System::String ^msg;
	if( this->isJapanese() )
	{
		msg = gcnew System::String( "���[�e�B���O�\���s�v(�S�N��)" );
	}
	else
	{
		msg = gcnew System::String( "Unnecessary Rating(All ages)" );
	}
	comb->Text = msg;
	comb->Enabled = false;		// �ҏW�s�\�ɂ���
}

// �u���[�e�B���O�\���s�v�v�\���������Ēʏ�̐ݒ�ɖ߂�
void Form1::necessaryRating( System::Windows::Forms::ComboBox ^comb, System::Boolean bInitial )
{
	comb->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
	if( !bInitial )
	{
		comb->SelectedIndex = -1;	// �ǂݍ��݂̏ꍇ�ɂ̓R���{�{�b�N�X�����������Ȃ�(���������ǂݍ��񂾏��������Ă��܂�����)
	}
	comb->Enabled = true;	// �ҏW�\�ɂ���
}

// end of file