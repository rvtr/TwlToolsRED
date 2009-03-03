// ----------------------------------------------
// �������� / �I������
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
// �R���X�g���N�^����Ă΂�鏉������
// ----------------------------------------------
void Form1::construct(void)
{
	// �t�B�[���h������
	this->hSrl   = gcnew RCSrl( this->getMrcMessageFileJ(), this->getMrcMessageFileE() );
	this->hDeliv = gcnew RCDeliverable;
	this->IsLoadTad = false;
	this->hErrorList = gcnew System::Collections::Generic::List<RCMrcError^>();
	this->hErrorList->Clear();
	this->hWarnList = gcnew System::Collections::Generic::List<RCMrcError^>();
	this->hWarnList->Clear();
	this->IsCheckedUGC     = false;
	this->IsCheckedPhotoEx = false;
	this->IsSpreadSheet = true;
	this->IsReadOnly    = false;
	this->SizeGboxExFlags   = this->gboxExFlags->Size;
	this->hMsg = gcnew RCMessageBank( this->getGuiMessageFileJ(), this->getGuiMessageFileE() );

	// �o�[�W��������\��
	//this->labAssemblyVersion->Text = System::Windows::Forms::Application::ProductVersion;
	System::Reflection::Assembly ^ass = System::Reflection::Assembly::GetEntryAssembly();
	this->labAssemblyVersion->Text = "ver." + this->getVersion();

	// TAD�ǂݍ��݂̍ۂɍ쐬�����ꎞ�t�@�C���Ɠ����t�@�C�����������ꍇ�ɂ͍폜���Ă悢���m�F
	//System::Diagnostics::Debug::WriteLine( this->getSplitTadTmpFile() );
	if( System::IO::File::Exists( this->getSplitTadTmpFile() ) )
	{
		this->sucMsg( "E_Start_TmpExist" );
		System::IO::File::Delete( this->getSplitTadTmpFile() );
	}

	// ���t�t�H�[���̏����ݒ�����݂̎����ɂ���
	this->dateRelease->Value = System::DateTime::Now;
	this->dateSubmit->Value  = System::DateTime::Now;

	// �V�X�e���ł̂Ƃ��ɂ̓��[�W�����ɑS���[�W�����̍��ڂ�ǉ�
#if defined(METWL_VER_APPTYPE_SYSTEM) || defined(METWL_VER_APPTYPE_SECURE) || defined(METWL_VER_APPTYPE_LAUNCHER)
	this->combRegion->Items->Add( gcnew System::String( L"�S���[�W����" ) );
#endif

	// �A�v����ʂ�����
	System::String ^appstr = nullptr;
#ifdef METWL_VER_APPTYPE_LAUNCHER
	appstr += "Launcher/";
#endif
#ifdef METWL_VER_APPTYPE_SECURE
	appstr += "Secure/";
#endif
#ifdef METWL_VER_APPTYPE_SYSTEM
	appstr += "System/";
#endif
	if( appstr != nullptr)
	{
		this->Text += " [ Supported App: " + appstr + "User ]";
	}

	// �����s�\���̉��s��}��
	this->tboxGuideRomEditInfo->Text  = this->tboxGuideRomEditInfo->Text->Replace( "<newline>", "\r\n" );
	this->tboxGuideNandSizeInfo->Text = this->tboxGuideNandSizeInfo->Text->Replace( "<newline>", "\r\n" );
	this->tboxGuideErrorInfo->Text    = this->tboxGuideErrorInfo->Text->Replace( "<newline>", "\r\n" );

	// ����o�[�W�����ɂ��킹�ă��j���[�Ƀ`�F�b�N������
	if( System::Threading::Thread::CurrentThread->CurrentUICulture->Name->StartsWith( "ja" ) )
	{
		this->stripItemJapanese->Checked = true;
		this->stripItemEnglish->Checked  = false;
	}
	else
	{
		this->stripItemJapanese->Checked = false;
		this->stripItemEnglish->Checked  = true;
	}

	// ��\������
	this->changeVisibleForms( false );

	// �ݒ�t�@�C���̓ǂݍ���
	this->loadInit();
	this->loadAppendInit();	// �ǉ��ݒ�t�@�C��

	// ��������
	this->handleArgs();
	//System::Diagnostics::Debug::WriteLine( "constructed" );
}


// ----------------------------------------------
// �f�X�g���N�^����Ă΂��I������
// ----------------------------------------------
void Form1::destruct(void)
{
	// TAD�ǂݏo���̍ۂɍ쐬�����ꎞSRL�t�@�C�����폜(�����o���������ɏI�������Ƃ��ɋN���肤��)
	System::String ^srlfile = this->getSplitTadTmpFile();
	if( System::IO::File::Exists( srlfile ) )
	{
		System::IO::File::Delete( srlfile );	// ���łɑ��݂���ꍇ�͍폜
	}
}


// ----------------------------------------------
// �R�}���h���C����������
// ----------------------------------------------
void Form1::handleArgs(void)
{
	cli::array<System::String^> ^args = System::Environment::GetCommandLineArgs();
	if( args->Length < 2 )
	{
		return;
	}

	// ������srl���w�肳��Ă���Ƃ��͓ǂݍ���
	System::String ^romfile = args[1];
	if( !this->loadRom( romfile ) )
	{
		return;
	}
	System::String ^rompath;
	if( System::IO::Path::IsPathRooted( romfile ) )		// ��΃p�X�̂Ƃ��͂��̂܂ܕ\��
	{
		rompath = romfile;
	}
	else
	{
		// �\���̂��߂ɐ�΃p�X�ɕϊ�
		System::String ^currdir = System::Environment::CurrentDirectory;
		if( !currdir->EndsWith( "\\" ) )
		{
			currdir = currdir + "\\";
		}
		rompath = romfile->Replace( '/', '\\' );
		rompath = currdir + rompath;
	}
	this->tboxFile->Text = rompath;
	this->clearOtherForms();
}
