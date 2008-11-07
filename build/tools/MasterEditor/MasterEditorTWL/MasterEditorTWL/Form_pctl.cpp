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
	this->hSrl->hIsRegionJapan     = gcnew System::Boolean(false);
	this->hSrl->hIsRegionAmerica   = gcnew System::Boolean(false);
	this->hSrl->hIsRegionEurope    = gcnew System::Boolean(false);
	this->hSrl->hIsRegionAustralia = gcnew System::Boolean(false);
	switch( this->combRegion->SelectedIndex )
	{
		case 0:
			this->hSrl->hIsRegionJapan = gcnew System::Boolean(true);
		break;

		case 1:
			this->hSrl->hIsRegionAmerica = gcnew System::Boolean(true);
		break;

		case 2:
			this->hSrl->hIsRegionEurope = gcnew System::Boolean(true);
		break;

		case 3:
			this->hSrl->hIsRegionAustralia = gcnew System::Boolean(true);
		break;

		case 4:
			this->hSrl->hIsRegionEurope    = gcnew System::Boolean(true);
			this->hSrl->hIsRegionAustralia = gcnew System::Boolean(true);
		break;

#if defined(METWL_VER_APPTYPE_SYSTEM) || defined(METWL_VER_APPTYPE_SECURE) || defined(METWL_VER_APPTYPE_LAUNCHER)
		case 5:
			this->hSrl->hIsRegionJapan     = gcnew System::Boolean(true);
			this->hSrl->hIsRegionAmerica   = gcnew System::Boolean(true);
			this->hSrl->hIsRegionEurope    = gcnew System::Boolean(true);
			this->hSrl->hIsRegionAustralia = gcnew System::Boolean(true);
		break;
#endif //defined(METWL_VER_APPTYPE_SYSTEM) || defined(METWL_VER_APPTYPE_SECURE) || defined(METWL_VER_APPTYPE_LAUNCHER)
		default:
		break;
	}
}

// ---------------------------------------------------------------------
// �y�A�����^���R���g���[���ݒ�͕��G�Ȃ̂ŕʂɐ؂�o��
// ---------------------------------------------------------------------

// SRL�����t�H�[���ɔ��f������
void Form1::setRegionForms(void)
{
	System::Boolean isJapan   = *(this->hSrl->hIsRegionJapan);			// ���[�W����
	System::Boolean isAmerica = *(this->hSrl->hIsRegionAmerica);
	System::Boolean isEurope  = *(this->hSrl->hIsRegionEurope);
	System::Boolean isAustralia = *(this->hSrl->hIsRegionAustralia);
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
	else
		index = -1;	// �s��
#if defined(METWL_VER_APPTYPE_SYSTEM) || defined(METWL_VER_APPTYPE_SECURE) || defined(METWL_VER_APPTYPE_LAUNCHER)
	if( isJapan && isAmerica && isEurope && isAustralia )
	index = 5;
#endif
	this->combRegion->SelectedIndex = index;
	this->maskParentalForms();		// �y�A�����^���R���g���[���p�t�H�[���̕\��/��\���؂�ւ�
}


// �t�H�[�����͂�SRL�ɔ��f������
void Form1::setParentalSrlProperties(void)
{
	// �e�c�̂̃t�H�[�����͂𔽉f
	this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_CERO ] = this->combCERO->SelectedIndex;
	this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_ESRB ] = this->combESRB->SelectedIndex;
	this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_USK ]  = this->combUSK->SelectedIndex;
	this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_PEGI_GEN ]  = this->combPEGI->SelectedIndex;
	this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_PEGI_PRT ]  = this->combPEGI_PRT->SelectedIndex;
	this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_PEGI_BBFC ] = this->combPEGI_BBFC->SelectedIndex;
	this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_OFLC ] = this->combOFLC->SelectedIndex;
} //setParentalSrlProperties()

// SRL���̃y�A�����^���R���g���[�����𔲂��o���ăt�H�[���ɔ��f������
void Form1::setParentalForms(void)
{
	// �e�c�̂̃R���{�{�b�N�X�̃C���f�b�N�X��ݒ�
	this->combCERO->SelectedIndex = this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_CERO ];
	this->combESRB->SelectedIndex = this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_ESRB ];
	this->combUSK->SelectedIndex  = this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_USK ];
	this->combPEGI->SelectedIndex = this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_PEGI_GEN ];
	this->combPEGI_PRT->SelectedIndex  = this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_PEGI_PRT ];
	this->combPEGI_BBFC->SelectedIndex = this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_PEGI_BBFC ];
	this->combOFLC->SelectedIndex = this->hSrl->hArrayParentalIndex[ OS_TWL_PCTL_OGN_OFLC ];
} //setParentalForms()

// ���[�W������񂩂�y�A�����^���R���g���[���̕ҏW�\�c�̂��}�X�N����
void Form1::maskParentalForms(void)
{
	this->enableParental( this->combCERO, this->labCERO, nullptr );
	this->enableParental( this->combESRB, this->labESRB, nullptr );
	this->enableParental( this->combUSK,  this->labUSK,  nullptr );
	this->enableParental( this->combPEGI, this->labPEGI, nullptr );
	this->enableParental( this->combPEGI_PRT,  this->labPEGI_PRT,  nullptr );
	this->enableParental( this->combPEGI_BBFC, this->labPEGI_BBFC, nullptr );
	this->enableParental( this->combOFLC, this->labOFLC, nullptr );
	switch( this->combRegion->SelectedIndex )
	{
		case 0:
			// ���{
			this->enableParental( this->combCERO, this->labCERO, nullptr );
			this->disableParental( this->combESRB, this->labESRB, nullptr );
			this->disableParental( this->combUSK,  this->labUSK,  nullptr );
			this->disableParental( this->combPEGI, this->labPEGI, nullptr );
			this->disableParental( this->combPEGI_PRT,  this->labPEGI_PRT,  nullptr );
			this->disableParental( this->combPEGI_BBFC, this->labPEGI_BBFC, nullptr );
			this->disableParental( this->combOFLC, this->labOFLC, nullptr );
		break;

		case 1:
			// �č�
			this->disableParental( this->combCERO, this->labCERO, nullptr );
			this->enableParental( this->combESRB,  this->labESRB, nullptr );
			this->disableParental( this->combUSK,  this->labUSK,  nullptr );
			this->disableParental( this->combPEGI, this->labPEGI, nullptr );
			this->disableParental( this->combPEGI_PRT,  this->labPEGI_PRT,  nullptr );
			this->disableParental( this->combPEGI_BBFC, this->labPEGI_BBFC, nullptr );
			this->disableParental( this->combOFLC, this->labOFLC, nullptr );
		break;

		case 2:
			// ���B
			this->disableParental( this->combCERO, this->labCERO, nullptr );
			this->disableParental( this->combESRB, this->labESRB, nullptr );
			this->enableParental( this->combUSK,   this->labUSK,  nullptr );
			this->enableParental( this->combPEGI,  this->labPEGI, nullptr );
			this->enableParental( this->combPEGI_PRT,  this->labPEGI_PRT,  nullptr );
			this->enableParental( this->combPEGI_BBFC, this->labPEGI_BBFC, nullptr );
			this->disableParental( this->combOFLC, this->labOFLC, nullptr );
		break;

		case 3:
			// ���B
			this->disableParental( this->combCERO, this->labCERO, nullptr );
			this->disableParental( this->combESRB, this->labESRB, nullptr );
			this->disableParental( this->combUSK,  this->labUSK,  nullptr );
			this->disableParental( this->combPEGI, this->labPEGI, nullptr );
			this->disableParental( this->combPEGI_PRT,  this->labPEGI_PRT,  nullptr );
			this->disableParental( this->combPEGI_BBFC, this->labPEGI_BBFC, nullptr );
			this->enableParental( this->combOFLC,  this->labOFLC, nullptr );
		break;

		case 4:
			// ���B�ƍ��B
			this->disableParental( this->combCERO, this->labCERO, nullptr );
			this->disableParental( this->combESRB, this->labESRB, nullptr );
			this->enableParental( this->combUSK,   this->labUSK,  nullptr );
			this->enableParental( this->combPEGI,  this->labPEGI, nullptr );
			this->enableParental( this->combPEGI_PRT,  this->labPEGI_PRT,  nullptr );
			this->enableParental( this->combPEGI_BBFC, this->labPEGI_BBFC, nullptr );
			this->enableParental( this->combOFLC,  this->labOFLC, nullptr );
		break;

		// �S���[�W�����̂Ƃ��͉���disable�ɂ��Ȃ�
		default:
		break;
	}
} //maskParentalForms()

// �y�A�����^���R���g���[���֘A�̃t�H�[�����͂����������������ݑO�`�F�b�N
void Form1::checkParentalForms( System::Boolean inRegion, System::Windows::Forms::ComboBox ^comb, System::String ^msg )
{
	// ���[�W�����Ɋ܂܂�Ă��Ȃ��Ƃ�: 0�N���A���ۏ؂����̂Ń`�F�b�N�K�v�Ȃ�
	if( !inRegion )
		return;

	// �ݒ肳��Ă��Ȃ��Ƃ��G���[
	if( (comb->SelectedIndex < 0) || (comb->SelectedIndex >= comb->Items->Count)  )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			"�y�A�����^���R���g���[�����", METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE,
			msg + ": ���[�e�B���O��I�����Ă��������B",
			"Parental Control", 
			msg + ": Rating Pending is setting. When rating age is examined, Please submit again.", true, true ) );
	}

	// �R�����̂Ƃ��x��
	if( comb->SelectedIndex == (comb->Items->Count - 1) )
	{
		this->hWarnList->Add( gcnew RCMrcError( 
			"�y�A�����^���R���g���[�����", METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE,
			msg + ": �R�����w�肪����Ă��܂��B�R�������܂肵�����A�Ē�o���Ă��������B",
			"Parental Control", msg + ": Save ROM data as Game soft which needs rating examinination.", true, true ) );
	}
} //checkParentalForms()


// �y�A�����^���R���g���[�������N���A����
void Form1::clearParental( System::Windows::Forms::ComboBox ^comb )
{
	comb->SelectedIndex = -1;	// �󔒂ɂ���
}


// �y�A�����^���R���g���[������ҏW�ł���悤�ɂ���
void Form1::enableParental( System::Windows::Forms::ComboBox ^comb, 
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
}

// �y�A�����^���R���g���[������ҏW�ł��Ȃ�����
void Form1::disableParental( System::Windows::Forms::ComboBox ^comb, 
							 System::Windows::Forms::Label    ^lab1, 
							 System::Windows::Forms::Label    ^lab2 )
{
	this->clearParental( comb );
	comb->Enabled   = false;
	comb->Visible   = false;
	lab1->Visible   = false;
	if( lab2 != nullptr )
	{
		lab2->Visible   = false;
	}
}

// end of file