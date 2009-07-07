// ---------------------------------------------------------------------
// ���[�W�����ƃ��[�e�B���O�ݒ�
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

// =====================================================================
// ���[�W����
// =====================================================================

// ---------------------------------------------------------------------
// GUI�̃R���{�{�b�N�X����ROM�w�b�_�̃��[�W�����R�[�h�����肷��
// ---------------------------------------------------------------------

void Form1::setRegionSrlPropaties(void)
{
	this->hSrl->IsRegionJapan     = false;
	this->hSrl->IsRegionAmerica   = false;
	this->hSrl->IsRegionEurope    = false;
	this->hSrl->IsRegionAustralia = false;

	// �����łƊ؍��ł̓��[�W�����ύX�����Ȃ�
	if( this->combRegion->DropDownStyle == System::Windows::Forms::ComboBoxStyle::DropDown )	// �R���{�{�b�N�X�����ؐݒ�p�ɂȂ��Ă���Ƃ�
	{
		return;
	}

	// ALL���[�W�����̂Ƃ����؃r�b�g�������Ă���̂Œ��؃r�b�g�𗎂Ƃ��Ă����K�v������
	this->hSrl->IsRegionChina = false;
	this->hSrl->IsRegionKorea = false;

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

		// ���؂͐ݒ�s��

#if defined(METWL_VER_APPTYPE_SYSTEM) || defined(METWL_VER_APPTYPE_SECURE) || defined(METWL_VER_APPTYPE_LAUNCHER)
		case 7:
			this->hSrl->IsRegionJapan     = true;
			this->hSrl->IsRegionAmerica   = true;
			this->hSrl->IsRegionEurope    = true;
			this->hSrl->IsRegionAustralia = true;
			this->hSrl->IsRegionKorea     = true;
			this->hSrl->IsRegionChina     = true;
		break;
#endif //defined(METWL_VER_APPTYPE_SYSTEM) || defined(METWL_VER_APPTYPE_SECURE) || defined(METWL_VER_APPTYPE_LAUNCHER)
		default:
		break;
	}
}

// ---------------------------------------------------------------------
// ROM�w�b�_�̃��[�W�����R�[�h����GUI�̃R���{�{�b�N�X�̒l�����肷��
// ---------------------------------------------------------------------

void Form1::setRegionForms(void)
{
	System::Boolean isJapan   = this->hSrl->IsRegionJapan;			// ���[�W����
	System::Boolean isAmerica = this->hSrl->IsRegionAmerica;
	System::Boolean isEurope  = this->hSrl->IsRegionEurope;
	System::Boolean isAustralia = this->hSrl->IsRegionAustralia;
	System::Boolean isKorea   = this->hSrl->IsRegionKorea;
	System::Boolean isChina   = this->hSrl->IsRegionChina;

	// ���؂̂Ƃ��R���{�{�b�N�X��ҏW�s�ɂ���
	if( !isJapan && !isAmerica && !isEurope && !isAustralia && !isKorea && isChina )		// ����
	{
		this->combRegion->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDown;	// ���R�Ƀe�L�X�g����͂ł���
		this->combRegion->SelectedIndex = -1;	// �ݒ�̏����ɒ���: �e�L�X�g�̐ݒ�����O�ɓ���Ă����Ȃ��ƃe�L�X�g���͂����f����Ȃ����Ƃ�����
		this->combRegion->Text = this->isJapanese()?METWL_STRING_CHINA_REGION_J:METWL_STRING_CHINA_REGION_E;
		this->combRegion->Enabled = false;	// �ҏW�s��
	}
	else if( !isJapan && !isAmerica && !isEurope && !isAustralia && isKorea && !isChina )	// �؍�
	{
		this->combRegion->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDown;
		this->combRegion->SelectedIndex = -1;
		this->combRegion->Text = this->isJapanese()?METWL_STRING_KOREA_REGION_J:METWL_STRING_KOREA_REGION_E;
		this->combRegion->Enabled = false;
	}
	else	// WorldWide
	{
		// �I���\�ɂ���
		this->combRegion->Enabled = true;
		this->combRegion->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;	// ���X�g�̒�����I�������ł��Ȃ�

		// �R���{�{�b�N�X�̃f�t�H���g�̑I��l������
		System::Int32  index = -1;
		if( isJapan && !isAmerica && !isEurope && !isAustralia && !isKorea && !isChina )
			index = 0;
		else if( !isJapan && isAmerica && !isEurope && !isAustralia && !isKorea && !isChina )
			index = 1;
		else if( !isJapan && !isAmerica && isEurope && !isAustralia && !isKorea && !isChina )
			index = 2;
		else if( !isJapan && !isAmerica && !isEurope && isAustralia && !isKorea && !isChina )
			index = 3;
		else if( !isJapan && !isAmerica && isEurope && isAustralia && !isKorea && !isChina )
			index = 4;
		else if( !isJapan && isAmerica && !isEurope && isAustralia && !isKorea && !isChina )
			index = 5;
		else if( !isJapan && isAmerica && isEurope && isAustralia && !isKorea && !isChina )
			index = 6;
#if defined(METWL_VER_APPTYPE_SYSTEM) || defined(METWL_VER_APPTYPE_SECURE) || defined(METWL_VER_APPTYPE_LAUNCHER)
		if( isJapan && isAmerica && isEurope && isAustralia && isKorea && isChina )
			index = 7;
#endif
		this->combRegion->SelectedIndex = index;
	}
	this->maskRatingForms();		// �y�A�����^���R���g���[���p�t�H�[���̕\��/��\���؂�ւ�
}

// ---------------------------------------------------------------------
// ���[�W�����̃t�H�[���ɖ�肪�Ȃ������`�F�b�N����
// ---------------------------------------------------------------------

void Form1::checkRegionForms(void)
{
	// ���؂̂Ƃ��ɂ̓��[�W�����I���ł��Ȃ��̂ŃR���{�{�b�N�X���I������Ă��邩�̃`�F�b�N�͕K�v�Ȃ�
	if( this->combRegion->DropDownStyle == System::Windows::Forms::ComboBoxStyle::DropDownList )
	{
		this->checkComboBoxIndex( this->combRegion, "LabelRegion", true );
	}
}


// =====================================================================
// ���[�e�B���O
// =====================================================================

// ---------------------------------------------------------------------
// ROM�w�b�_�̃��[�e�B���O��񂩂�GUI�̃R���{�{�b�N�X��ݒ肷��
// ---------------------------------------------------------------------

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
	this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_GRB]   = this->combGRB->SelectedIndex;

	// ���[�e�B���O�\���s�v���ǂ�����ݒ�
	this->hSrl->IsUnnecessaryRating = this->cboxIsUnnecessaryRating->Checked;
} //setRatingSrlProperties()

// ---------------------------------------------------------------------
// GUI�̃R���{�{�b�N�X�̑I����ROM�w�b�_�ɔ��f������
// ---------------------------------------------------------------------

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
	this->combGRB->SelectedIndex  = this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_GRB ];
} //setRatingForms()

// ---------------------------------------------------------------------
// GUI�ŕ\�����郌�[�e�B���O�c�̂����[�W�����ɂ���ĕς���
// ---------------------------------------------------------------------

void Form1::maskRatingForms(void)
{
	this->enableRating( this->combCERO, this->labCERO, nullptr );
	this->enableRating( this->combESRB, this->labESRB, nullptr );
	this->enableRating( this->combUSK,  this->labUSK,  nullptr );
	this->enableRating( this->combPEGI, this->labPEGI, nullptr );
	this->enableRating( this->combPEGI_PRT,  this->labPEGI_PRT,  nullptr );
	this->enableRating( this->combPEGI_BBFC, this->labPEGI_BBFC, nullptr );
	this->enableRating( this->combOFLC, this->labOFLC, nullptr );
	this->enableRating( this->combGRB, this->labGRB, nullptr );

	// ���؃��[�W�����͐ݒ�s�Ȃ̂œ��ʏ���
	this->tboxWarningChinaRating->Visible = false;
	if( (this->combRegion->DropDownStyle == System::Windows::Forms::ComboBoxStyle::DropDown) // �R���{�{�b�N�X�����؃��[�W�����p�ɂȂ��Ă���Ƃ�
		&& this->hSrl->IsRegionChina )	// SRL���ǂݍ��܂�Ă��邱�Ƃ͕ۏ؂����
	{
		this->tboxWarningChinaRating->Visible = true;	// �����p�̓��ʃ��b�Z�[�W��\������

		// �����Ƀ��[�e�B���O�c�̂Ȃ�
		this->disableRating( this->combCERO, this->labCERO, nullptr );
		this->disableRating( this->combESRB,  this->labESRB, nullptr );
		this->disableRating( this->combUSK,   this->labUSK,  nullptr );
		this->disableRating( this->combPEGI,  this->labPEGI, nullptr );
		this->disableRating( this->combPEGI_PRT,  this->labPEGI_PRT,  nullptr );
		this->disableRating( this->combPEGI_BBFC, this->labPEGI_BBFC, nullptr );
		this->disableRating( this->combOFLC,  this->labOFLC, nullptr );
		this->disableRating( this->combGRB, this->labGRB, nullptr );
	}
	else if( (this->combRegion->DropDownStyle == System::Windows::Forms::ComboBoxStyle::DropDown) 
		&& this->hSrl->IsRegionKorea )
	{
		// �؍���GRB�̂�
		this->disableRating( this->combCERO, this->labCERO, nullptr );
		this->disableRating( this->combESRB,  this->labESRB, nullptr );
		this->disableRating( this->combUSK,   this->labUSK,  nullptr );
		this->disableRating( this->combPEGI,  this->labPEGI, nullptr );
		this->disableRating( this->combPEGI_PRT,  this->labPEGI_PRT,  nullptr );
		this->disableRating( this->combPEGI_BBFC, this->labPEGI_BBFC, nullptr );
		this->disableRating( this->combOFLC,  this->labOFLC, nullptr );
		this->enableRating( this->combGRB, this->labGRB, nullptr );
	}
	else
	{
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
				this->disableRating( this->combGRB, this->labGRB, nullptr );
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
				this->disableRating( this->combGRB, this->labGRB, nullptr );
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
				this->disableRating( this->combGRB, this->labGRB, nullptr );
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
				this->disableRating( this->combGRB, this->labGRB, nullptr );
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
				this->disableRating( this->combGRB, this->labGRB, nullptr );
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
				this->disableRating( this->combGRB, this->labGRB, nullptr );
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
				this->disableRating( this->combGRB, this->labGRB, nullptr );
			break;

			// �S���[�W�����̂Ƃ��͉���disable�ɂ��Ȃ�
			default:
			break;
		}// switch
	}
} //maskRatingForms()

// ---------------------------------------------------------------------
// GUI�̃��[�e�B���O�ݒ�ɖ�肪�Ȃ����`�F�b�N����
// ---------------------------------------------------------------------

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

// ---------------------------------------------------------------------
// �u���[�e�B���O�\���s�v�v���I�����ꂽ���ǂ����ŃR���{�{�b�N�X�̓��e��ς���
// ROM�ǂݍ��ݎ�����сu���[�e�B���O�\���s�v�v�`�F�b�N�{�b�N�X�ɕω����������Ƃ��ɌĂяo�����
// ---------------------------------------------------------------------

void Form1::changeUnnecessaryRatingForms( System::Boolean bInitial )
{
	if( this->cboxIsUnnecessaryRating->Checked )
	{
		this->unnecessaryRating( this->combCERO );		// �\��/��\���̑I���͕ʂ̂Ƃ���ł���̂ł����ł͑S�c�̂�s�v�ɂ��Ă����܂�Ȃ�
		this->unnecessaryRating( this->combESRB );
		this->unnecessaryRating( this->combUSK );
		this->unnecessaryRating( this->combPEGI );
		this->unnecessaryRating( this->combPEGI_PRT );
		this->unnecessaryRating( this->combPEGI_BBFC );
		this->unnecessaryRating( this->combOFLC );
		this->unnecessaryRating( this->combGRB );
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
		this->necessaryRating( this->combGRB, bInitial );
	}
}

// ---------------------------------------------------------------------
// ���[�e�B���O�̃R���{�{�b�N�X���N���A����
// ---------------------------------------------------------------------

void Form1::clearRating( System::Windows::Forms::ComboBox ^comb )
{
	comb->SelectedIndex = -1;	// �󔒂ɂ���
}

// ---------------------------------------------------------------------
// ���[�e�B���O�̃R���{�{�b�N�X��ҏW�\�ȏ�Ԃɂ���
// ---------------------------------------------------------------------

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
	// ��o�m�F���Ƀ��[�e�B���O���L�q����ۂɁu�c�̂����[�W�����Ɋ܂܂�Ă��邩�ǂ����v�̏�񂪕K�v�ƂȂ�
	// comb->Enable�͕ҏW�\���ǂ�����\���̂�
	// ���[�W�����Ɋ܂܂�Ă��邩�ǂ����̏��Ƃ��Ďg����悤�Ɏv���邪
	//�u���[�e�B���O�\���s�v�v��comb->Enabled��false�ɂ��Ȃ���΂Ȃ�Ȃ��̂�
	// ���[�W�����Ɋ܂܂�Ă���̂���comb->Enabled���g���Ĕ���ł��Ȃ�
	// (comb->Visible���g�����������c�O�Ȃ���ʂ̃^�u�Ɉړ�����Ə����false�ɂȂ��Ă��܂�)
	// ���̂��ߖ��֌W�Ȑݒ��ς��邱�ƂŃ��[�W�����Ɋ܂܂�Ă��邱�Ƃ�\������
	comb->FlatStyle = System::Windows::Forms::FlatStyle::Standard;
}

// ---------------------------------------------------------------------
// ���[�e�B���O�̃R���{�{�b�N�X��ҏW�s��Ԃɂ���
// ---------------------------------------------------------------------

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
	// ���[�W�����Ɋ܂܂�Ȃ����Ƃ�\�� (�\����������̂ł��̒l���ς���Ă����Ȃ�)
	comb->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
}

// ---------------------------------------------------------------------
// ���[�e�B���O�̃R���{�{�b�N�X�Ɂu���[�e�B���O�\���s�v�v�ƕ\����
// �ҏW�s��Ԃɂ���
// ---------------------------------------------------------------------

void Form1::unnecessaryRating( System::Windows::Forms::ComboBox ^comb )
{
	comb->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDown;	// ���R�ȃe�L�X�g��\���\�ɂ���
	comb->SelectedIndex = -1;	// �����I������Ă��Ȃ��Ƃ݂Ȃ�
	System::String ^msg = this->isJapanese()?METWL_STRING_UNNECESSARY_RATING_J:METWL_STRING_UNNECESSARY_RATING_E;
	comb->Text = msg;
	comb->Enabled = false;		// �ҏW�s�\�ɂ���
}

// ---------------------------------------------------------------------
// ���[�e�B���O�̃R���{�{�b�N�X�́u���[�e�B���O�\���s�v�v�̕\�����N���A��
// �ҏW�\��Ԃɂ���
// ---------------------------------------------------------------------

void Form1::necessaryRating( System::Windows::Forms::ComboBox ^comb, System::Boolean bInitial )
{
	comb->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;	// ���X�g�̒����炵���I���ł��Ȃ��悤�ɂ���
	if( !bInitial )
	{
		comb->SelectedIndex = -1;	// ROM�ǂݍ��ݎ��̏ꍇ�ɂ̓R���{�{�b�N�X�����������Ȃ�(���������ǂݍ��񂾏��������Ă��܂�����)
									// GUI��ŕύX�����Ƃ��ɂ͋󔒂ɏ���������(�ēx�ݒ肳����)
	}
	comb->Enabled = true;	// �ҏW�\�ɂ���
}

// end of file