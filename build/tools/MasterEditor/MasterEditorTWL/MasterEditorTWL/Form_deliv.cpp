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
	// �v���O�����̃o�[�W����
	this->hDeliv->hMasterEditorVersion = this->getVersion();

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
	if( this->rSubmitInternet->Checked == true )
	{
		this->hDeliv->hSubmitWay = this->rSubmitInternet->Text;
	}
	else if( this->rSubmitPost->Checked == true )
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
	}
	else if( this->rUsageSample->Checked == true )
	{
		this->hDeliv->hUsage = this->rUsageSample->Text;
	}
	else if( this->rUsageDst->Checked == true )
	{
		this->hDeliv->hUsage = this->rUsageDst->Text;
	}
	else if( this->rUsageOther->Checked == true )
	{
		this->hDeliv->hUsage = this->tboxUsageOther->Text;
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

	// ����ȃA�v�����
	this->hDeliv->hAppTypeOther = this->tboxAppTypeOther->Text;
	if( this->hDeliv->hAppTypeOther != nullptr )
	{
		this->hDeliv->hAppTypeOther->Replace("\r\n","");
	}

	// �o�b�N�A�b�v��������ROM�w�b�_�ɂ͋L�q����Ȃ��̂Œ�o�m�F���ɂ̂݋L��
	if( this->combBackup->SelectedIndex != (this->combBackup->Items->Count - 1) )
	{
		this->hDeliv->hBackupMemory = this->combBackup->SelectedItem->ToString();
	}
	else
	{
		this->hDeliv->hBackupMemory = this->tboxBackupOther->Text;
	}

	//// DSi Ware�̔̔��J�e�S��
	//if( this->combDLCategory->SelectedIndex != (this->combDLCategory->Items->Count - 1) )
	//{
	//	if( this->combDLCategory->SelectedIndex > 0 )
	//	{
	//		this->hDeliv->hDLCategory = this->combDLCategory->SelectedItem->ToString();
	//	}
	//	else
	//	{
	//		this->hDeliv->hDLCategory = gcnew System::String("");
	//	}
	//}
	//else
	//{
	//	this->hDeliv->hDLCategory = this->tboxDLCategoryOther->Text;
	//}

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
	this->hDeliv->hGRB  = this->setDeliverableRatingOgnProperties( this->combGRB );

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
	this->checkTextForm( this->tboxProductName->Text, "LabelProductName" );	// SRL�쐬�ɂ͖��̂Ȃ��G���[
	this->checkTextForm( this->tboxProductCode1->Text, "LabelProductCode" );
	this->checkTextForm( this->tboxProductCode2->Text, "LabelProductCode" );
	if( this->cboxReleaseForeign->Checked == true )
	{
		this->checkTextForm( this->tboxProductNameForeign->Text, "LabelProductNameForeign" );
		this->checkTextForm( this->tboxProductCode1Foreign->Text, "LabelProductCodeForeign" );
		this->checkTextForm( this->tboxProductCode2Foreign1->Text,"LabelProductCodeForeign" );
	}
	if( this->rUsageOther->Checked == true )
	{
		this->checkTextForm( this->tboxUsageOther->Text, "LabelUsage" );
	}

	// ��Џ��
	this->checkTextForm( this->tboxPerson1->Text, "LabelPerson1" );
	this->checkTextForm( this->tboxCompany1->Text, "LabelCompany1" );
	this->checkTextForm( this->tboxDepart1->Text, "LabelDepart2" );
	if( this->isJapanese() == true )
	{
		this->checkTextForm( this->tboxFurigana1->Text, "LabelFurigana1" );
	}
	this->checkTextForm( this->tboxTel1->Text, "LabelTel1" );
	this->checkTextForm( this->tboxMail1->Text, "LabelMail1" );

	if( this->cboxIsInputPerson2->Checked == true )
	{
		this->checkTextForm( this->tboxPerson2->Text, "LabelPerson2" );
		this->checkTextForm( this->tboxCompany2->Text, "LabelCompany2" );
		this->checkTextForm( this->tboxDepart2->Text, "LabelDepart2" );
		if( this->isJapanese() == true )
		{
			this->checkTextForm( this->tboxFurigana2->Text, "LabelFurigana2" );
		}
		this->checkTextForm( this->tboxTel2->Text, "LabelTel2" );
		this->checkTextForm( this->tboxMail2->Text, "LabelMail2" );
	}

	// �ꕔ��ROM���(SRL�o�C�i���ɔ��f����Ȃ����)�������Ń`�F�b�N����
	this->checkComboBoxIndex( this->combBackup, "LabelBackup", false );
	if( this->combBackup->SelectedIndex == (this->combBackup->Items->Count - 1) )
	{
		this->checkTextForm( this->tboxBackupOther->Text, "LabelBackup" );
	}

	// �ЂƂƂ���G���[�o�^���������
	// ���ޏ�̃G���[(SRL�o�C�i���ɂ͉e�����Ȃ�)�����݂��邩�`�F�b�N
	return this->isValidOnlyDeliverable();
}

// end of file