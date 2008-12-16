// ----------------------------------------------
// �}�X�^���ޏ��(SRL�e���Ȃ�)���t�H�[������擾
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


// ----------------------------------------------
// [sheet <= Form]
// ��o�m�F���Ƀt�H�[���𔽉f
// ----------------------------------------------
void Form1::setDeliverableProperties(void)
{
	// ��o���
	this->hDeliv->hProductName     = this->tboxProductName->Text;
	this->hDeliv->hProductCode1    = this->tboxProductCode1->Text;
	this->hDeliv->hProductCode2    = this->tboxProductCode2->Text;
	this->hDeliv->IsReleaseForeign = this->cboxReleaseForeign->Checked;
	if( this->cboxReleaseForeign->Checked == true )
	{
		this->hDeliv->hProductNameForeign  = this->tboxProductNameForeign->Text;
		this->hDeliv->hProductCode1Foreign = this->tboxProductCode1Foreign->Text;
		this->hDeliv->hProductCode2Foreign = gcnew System::String("");
		this->hDeliv->hProductCode2Foreign = this->tboxProductCode2Foreign1->Text;
		if( !System::String::IsNullOrEmpty( this->tboxProductCode2Foreign2->Text ) )
		{
			this->hDeliv->hProductCode2Foreign += ("/" + this->tboxProductCode2Foreign2->Text);
		}
		if( !System::String::IsNullOrEmpty( this->tboxProductCode2Foreign3->Text ) )
		{
			this->hDeliv->hProductCode2Foreign += ("/" + this->tboxProductCode2Foreign3->Text);
		}
	}
	else
	{
		this->hDeliv->hProductNameForeign  = nullptr;
		this->hDeliv->hProductCode1Foreign = nullptr;
		this->hDeliv->hProductCode2Foreign = nullptr;
	}
	this->hDeliv->ReleaseYear   = this->dateRelease->Value.Year;
	this->hDeliv->ReleaseMonth  = this->dateRelease->Value.Month;
	this->hDeliv->ReleaseDay    = this->dateRelease->Value.Day;
	this->hDeliv->SubmitYear    = this->dateSubmit->Value.Year;
	this->hDeliv->SubmitMonth   = this->dateSubmit->Value.Month;
	this->hDeliv->SubmitDay     = this->dateSubmit->Value.Day;
	this->hDeliv->SubmitVersion = System::Decimal::ToInt32( this->numSubmitVersion->Value );
	// SDK
	this->hDeliv->hSDK = nullptr;
	if( this->hSrl->hSDKList )
	{
		for each( RCSDKVersion ^sdk in this->hSrl->hSDKList )	// ���ނɂ� ARM9 static �̃o�[�W���������L������
		{
			if( sdk->IsStatic )
				this->hDeliv->hSDK = sdk->Version;
		}
	}
	if( this->hDeliv->hSDK == nullptr )
	{
		this->hDeliv->hSDK = gcnew System::String( "" );
	}
	// ��o���@
	if( this->rSubmitPost->Checked == true )
	{
		this->hDeliv->hSubmitWay = this->rSubmitPost->Text;
	}
	else
	{
		this->hDeliv->hSubmitWay = this->rSubmitHand->Text;
	}
	// �p�r
	if( this->rUsageSale->Checked == true )
	{
		this->hDeliv->hUsage = this->rUsageSale->Text;
		this->hDeliv->hUsageOther = nullptr;
	}
	else if( this->rUsageSample->Checked == true )
	{
		this->hDeliv->hUsage = this->rUsageSample->Text;
		this->hDeliv->hUsageOther = nullptr;
	}
	else if( this->rUsageDst->Checked == true )
	{
		this->hDeliv->hUsage = this->rUsageDst->Text;
		this->hDeliv->hUsageOther = nullptr;
	}
	else if( this->rUsageOther->Checked == true )
	{
		this->hDeliv->hUsage = this->rUsageOther->Text;
		this->hDeliv->hUsageOther = this->tboxUsageOther->Text;
	}

	// ��Џ��
	this->hDeliv->hCompany1    = this->tboxCompany1->Text + " " + this->tboxDepart1->Text;
	this->hDeliv->hPerson1     = this->tboxPerson1->Text;
	if( this->isJapanese() == true )
	{
		this->hDeliv->hFurigana1 = this->tboxFurigana1->Text;
	}
	else
	{
		this->hDeliv->hFurigana1 = nullptr;
	}
	this->hDeliv->hTel1        = this->tboxTel1->Text;
	this->hDeliv->hFax1        = this->tboxFax1->Text;
	this->hDeliv->hMail1       = this->tboxMail1->Text;
	if( this->isJapanese() == true )
	{
		this->hDeliv->hNTSC1 = this->tboxNTSC1->Text;
	}
	else
	{
		this->hDeliv->hNTSC1 = nullptr;
	}
	if( this->cboxIsInputPerson2->Checked == true )
	{
		this->hDeliv->hCompany2    = this->tboxCompany2->Text + " " + this->tboxDepart2->Text;
		this->hDeliv->hPerson2     = this->tboxPerson2->Text;
		if( this->isJapanese() == true )
		{
			this->hDeliv->hFurigana2 = this->tboxFurigana2->Text;
		}
		else
		{
			this->hDeliv->hFurigana2 = nullptr;
		}
		this->hDeliv->hTel2        = this->tboxTel2->Text;
		this->hDeliv->hFax2        = this->tboxFax2->Text;
		this->hDeliv->hMail2       = this->tboxMail2->Text;
		if( this->isJapanese() == true )
		{
			this->hDeliv->hNTSC2 = this->tboxNTSC2->Text;
		}
		else
		{
			this->hDeliv->hNTSC2 = nullptr;
		}
	}
	else
	{
		this->hDeliv->hCompany2    = nullptr;
		this->hDeliv->hPerson2     = nullptr;
		this->hDeliv->hFurigana2   = nullptr;
		this->hDeliv->hTel2        = nullptr;
		this->hDeliv->hFax2        = nullptr;
		this->hDeliv->hMail2       = nullptr;
		this->hDeliv->hNTSC2       = nullptr;
	}

	// ���l
	System::String ^tmp = this->tboxCaption->Text->Replace( " ", "" );
	if( this->tboxCaption->Text->Equals("") || tmp->Equals("") )	// �X�y�[�X�݂̂̕�����͊܂߂Ȃ�
	{
		this->hDeliv->hCaption = nullptr;
	}
	else
	{
		this->hDeliv->hCaption = this->tboxCaption->Text;
	}
	// ���l���ɓ���Ȑݒ��ǋL
	tmp = this->tboxCaptionEx->Text->Replace( " ", "" );
	if( !(this->tboxCaptionEx->Text->Equals("")) && !(tmp->Equals("")) )
	{
		this->hDeliv->hCaption += ("(" + this->tboxCaptionEx->Text + ")");
	}

	// ����ȃA�v�����
	this->hDeliv->hAppTypeOther = this->tboxAppTypeOther->Text;
	if( this->hDeliv->hAppTypeOther != nullptr )
	{
		this->hDeliv->hAppTypeOther->Replace("\r\n","");
	}

	// �ꕔ��ROM����o�^
	if( this->combBackup->SelectedIndex != (this->combBackup->Items->Count - 1) )
	{
		if( this->combBackup->SelectedIndex > 0 )
		{
			this->hDeliv->hBackupMemory = this->combBackup->SelectedItem->ToString();
		}
		else
		{
			this->hDeliv->hBackupMemory = gcnew System::String("");
		}
	}
	else
	{
		this->hDeliv->hBackupMemory = this->tboxBackupOther->Text;
	}

	// SRL���𕶎���œo�^
	if( this->combRegion->SelectedIndex < 0 )
	{
		if( this->isJapanese() == true )
			this->hDeliv->hRegion = gcnew System::String("�s��");
		else
			this->hDeliv->hRegion = gcnew System::String("Undefined");
	}
	else
	{
		this->hDeliv->hRegion = dynamic_cast<System::String^>(this->combRegion->SelectedItem);
	}
	this->hDeliv->hCERO = this->setDeliverableRatingOgnProperties( this->combCERO );
	this->hDeliv->hESRB = this->setDeliverableRatingOgnProperties( this->combESRB );
	this->hDeliv->hUSK  = this->setDeliverableRatingOgnProperties( this->combUSK );
	this->hDeliv->hPEGI = this->setDeliverableRatingOgnProperties( this->combPEGI );
	this->hDeliv->hPEGI_PRT  = this->setDeliverableRatingOgnProperties( this->combPEGI_PRT );
	this->hDeliv->hPEGI_BBFC = this->setDeliverableRatingOgnProperties( this->combPEGI_BBFC );
	this->hDeliv->hOFLC = this->setDeliverableRatingOgnProperties( this->combOFLC );

	// SRL�ɂ͓o�^����Ȃ�ROM�d�l
	this->hDeliv->IsUGC     = this->cboxIsUGC->Checked;
	this->hDeliv->IsPhotoEx = this->cboxIsPhotoEx->Checked;
}

// ----------------------------------------------
// ��o�m�F���Ƀ��[�e�B���O(1�c��)�̃t�H�[���𔽉f
// ----------------------------------------------
System::String^ Form1::setDeliverableRatingOgnProperties( System::Windows::Forms::ComboBox ^box )
{
	System::String ^str;
	if( this->cboxIsUnnecessaryRating->Checked )
	{
		if( box->FlatStyle == System::Windows::Forms::FlatStyle::Standard )
		{
			str = System::String::Copy( box->Text );	// �e�L�X�g���͉ɂȂ��Ă���̂Ŏ擾�ł���͂�
		}
		else	// ���[�W�����Ɋ܂܂�Ă��Ȃ�(�R���{�{�b�N�X���\������Ă��Ȃ�)�Ƃ��ɂ͕s�Ƃ���
		{
			if( this->isJapanese() == true )
				str = gcnew System::String("�s��");
			else
				str = gcnew System::String("Undefined");
		}
	}
	else
	{
		if( box->SelectedIndex < 0 )
		{
			if( this->isJapanese() == true )
				str = gcnew System::String("�s��");
			else
				str = gcnew System::String("Undefined");
		}
		else
		{
			str = dynamic_cast<System::String^>(box->SelectedItem);
		}
	}
	return str;
}

// ----------------------------------------------
// �}�X�^���ޏ��(SRL�e���Ȃ�)�̃t�H�[���`�F�b�N
// ----------------------------------------------
System::Boolean Form1::checkDeliverableForms(void)
{
	// �s���ȏꍇ�̓_�C�A���O�Œ��ӂ���return

	// ��o���
	this->checkTextForm( this->tboxProductName->Text, LANG_PRODUCT_NAME_J, LANG_PRODUCT_NAME_E, false );	// SRL�쐬�ɂ͖��̂Ȃ��G���[
	this->checkTextForm( this->tboxProductCode1->Text, LANG_PRODUCT_CODE_J, LANG_PRODUCT_CODE_E, false );
	this->checkTextForm( this->tboxProductCode2->Text, LANG_PRODUCT_CODE_J, LANG_PRODUCT_CODE_E, false );
	if( this->cboxReleaseForeign->Checked == true )
	{
		this->checkTextForm( this->tboxProductNameForeign->Text, LANG_PRODUCT_NAME_F_J, LANG_PRODUCT_NAME_F_E, false );
		this->checkTextForm( this->tboxProductCode1Foreign->Text, LANG_PRODUCT_CODE_F_J, LANG_PRODUCT_CODE_F_E, false );
		this->checkTextForm( this->tboxProductCode2Foreign1->Text, LANG_PRODUCT_CODE_F_J, LANG_PRODUCT_CODE_F_E, false );
	}
	if( this->rUsageOther->Checked == true )
	{
		this->checkTextForm( this->tboxUsageOther->Text, LANG_USAGE_J, LANG_USAGE_E, false );
	}

	// ��Џ��
	this->checkTextForm( this->tboxPerson1->Text, LANG_PERSON_J, LANG_PERSON_E, false );
	this->checkTextForm( this->tboxCompany1->Text, LANG_COMPANY_J, LANG_COMPANY_E, false );
	this->checkTextForm( this->tboxDepart1->Text, LANG_DEPART_J, LANG_DEPART_E, false );
	if( this->isJapanese() == true )
	{
		this->checkTextForm( this->tboxFurigana1->Text, LANG_FURIGANA_J, LANG_FURIGANA_J, false );
	}
	this->checkTextForm( this->tboxTel1->Text, LANG_TEL_J, LANG_TEL_E, false );
	//this->checkTextForm( this->tboxFax1->Text, LANG_FAX_J, LANG_FAX_E, false );
	this->checkTextForm( this->tboxMail1->Text, LANG_MAIL_J, LANG_MAIL_E, false );
	//if( this->isJapanese() == true )
	//{
	//	this->checkTextForm( this->tboxNTSC1->Text, LANG_NTSC_1_J + " " + LANG_NTSC_2_J, LANG_NTSC_1_J + " " + LANG_NTSC_2_J, false );
	//}

	if( this->cboxIsInputPerson2->Checked == true )
	{
		this->checkTextForm( this->tboxPerson2->Text, LANG_PERSON_J, LANG_PERSON_E, false );
		this->checkTextForm( this->tboxCompany2->Text, LANG_COMPANY_J, LANG_COMPANY_E, false );
		this->checkTextForm( this->tboxDepart2->Text, LANG_DEPART_J, LANG_DEPART_E, false );
		if( this->isJapanese() == true )
		{
			this->checkTextForm( this->tboxFurigana2->Text, LANG_FURIGANA_J, LANG_FURIGANA_J, false );
		}
		this->checkTextForm( this->tboxTel2->Text, LANG_TEL_J, LANG_TEL_E, false );
		//this->checkTextForm( this->tboxFax2->Text, LANG_FAX_J, LANG_FAX_E, false );
		this->checkTextForm( this->tboxMail2->Text, LANG_MAIL_J, LANG_MAIL_E, false );
		//if( this->isJapanese() == true )
		//{
		//	this->checkTextForm( this->tboxNTSC2->Text, LANG_NTSC_1_J + " " + LANG_NTSC_2_J, LANG_NTSC_1_J + " " + LANG_NTSC_2_J, false );
		//}
	}

	// �ꕔ��ROM���(SRL�o�C�i���ɔ��f����Ȃ����)�������Ń`�F�b�N����
	this->checkBoxIndex( this->combBackup, LANG_BACKUP_J, LANG_BACKUP_E, false );
	if( this->combBackup->SelectedIndex == (this->combBackup->Items->Count - 1) )
	{
		this->checkTextForm( this->tboxBackupOther->Text, LANG_BACKUP_J, LANG_BACKUP_E, false );
	}

	// �ЂƂƂ���G���[�o�^���������
	// ���ޏ�̃G���[(SRL�o�C�i���ɂ͉e�����Ȃ�)�����݂��邩�`�F�b�N
	return this->isValidOnlyDeliverable();
}

// end of file