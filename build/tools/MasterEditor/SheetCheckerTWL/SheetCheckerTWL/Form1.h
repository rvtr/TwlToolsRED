#pragma once

#include "SheetCheckerTWL.h"
#include "crc_whole.h"
#include <twl/types.h>
#include <twl/os/common/format_rom.h>
#include <cstdio>

namespace SheetCheckerTWL {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Form1 �̊T�v
	///
	/// �x��: ���̃N���X�̖��O��ύX����ꍇ�A���̃N���X���ˑ����邷�ׂĂ� .resx �t�@�C���Ɋ֘A�t����ꂽ
	///          �}�l�[�W ���\�[�X �R���p�C�� �c�[���ɑ΂��� 'Resource File Name' �v���p�e�B��
	///          �ύX����K�v������܂��B���̕ύX���s��Ȃ��ƁA
	///          �f�U�C�i�ƁA���̃t�H�[���Ɋ֘A�t����ꂽ���[�J���C�Y�ς݃��\�[�X�Ƃ��A
	///          ���������݂ɗ��p�ł��Ȃ��Ȃ�܂��B
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	private:
		ROM_Header *rh;
		SheetItem  ^sheet;
		System::UInt16 ^fileCRC;
		SheetCheckerError ^error;
		System::Boolean ^bReadSrl;		// ��x�ł��ǂݍ��܂ꂽ��true�ɂ���
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  colTitle;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  colSrl;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  colSheet;






			 System::Boolean ^bReadSheet;

	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: �����ɃR���X�g���N�^ �R�[�h��ǉ����܂�
			//
			this->rh    = new ROM_Header();
			this->sheet = gcnew SheetItem();
			this->fileCRC = gcnew System::UInt16;
			this->error   = gcnew SheetCheckerError(SheetCheckerError::NOERROR);
			this->bReadSrl   = gcnew System::Boolean(false);
			this->bReadSheet = gcnew System::Boolean(false);
			memset( this->rh, 0, sizeof(ROM_Header) );
			this->labVersion->Text = "ver. " + this->getVersion();
		}

	protected:
		/// <summary>
		/// �g�p���̃��\�[�X�����ׂăN���[���A�b�v���܂��B
		/// </summary>
		~Form1()
		{
			delete this->rh;		// �Y�ꂸ��
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::Label^  labVersion;



	private: System::Windows::Forms::TextBox^  tboxSrl;
	private: System::Windows::Forms::TextBox^  tboxSheet;
	private: System::Windows::Forms::Button^  butSrl;
	private: System::Windows::Forms::Button^  butSheet;
	private: System::Windows::Forms::DataGridView^  gridCompare;
	private: System::Windows::Forms::TextBox^  tboxResult;
	private: System::Windows::Forms::Label^  labResult;

	private:
		/// <summary>
		/// �K�v�ȃf�U�C�i�ϐ��ł��B
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// �f�U�C�i �T�|�[�g�ɕK�v�ȃ��\�b�h�ł��B���̃��\�b�h�̓��e��
		/// �R�[�h �G�f�B�^�ŕύX���Ȃ��ł��������B
		/// </summary>
		void InitializeComponent(void)
		{
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle5 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle6 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle7 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle8 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			this->tboxSrl = (gcnew System::Windows::Forms::TextBox());
			this->tboxSheet = (gcnew System::Windows::Forms::TextBox());
			this->butSrl = (gcnew System::Windows::Forms::Button());
			this->butSheet = (gcnew System::Windows::Forms::Button());
			this->gridCompare = (gcnew System::Windows::Forms::DataGridView());
			this->tboxResult = (gcnew System::Windows::Forms::TextBox());
			this->labResult = (gcnew System::Windows::Forms::Label());
			this->labVersion = (gcnew System::Windows::Forms::Label());
			this->colTitle = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->colSrl = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->colSheet = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->gridCompare))->BeginInit();
			this->SuspendLayout();
			// 
			// tboxSrl
			// 
			this->tboxSrl->AllowDrop = true;
			this->tboxSrl->Location = System::Drawing::Point(31, 32);
			this->tboxSrl->Name = L"tboxSrl";
			this->tboxSrl->ReadOnly = true;
			this->tboxSrl->Size = System::Drawing::Size(250, 19);
			this->tboxSrl->TabIndex = 0;
			this->tboxSrl->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &Form1::tboxSrl_DragDrop);
			this->tboxSrl->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &Form1::tboxSrl_DragEnter);
			// 
			// tboxSheet
			// 
			this->tboxSheet->AllowDrop = true;
			this->tboxSheet->Location = System::Drawing::Point(31, 66);
			this->tboxSheet->Name = L"tboxSheet";
			this->tboxSheet->ReadOnly = true;
			this->tboxSheet->Size = System::Drawing::Size(250, 19);
			this->tboxSheet->TabIndex = 1;
			this->tboxSheet->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &Form1::tboxSheet_DragDrop);
			this->tboxSheet->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &Form1::tboxSheet_DragEnter);
			// 
			// butSrl
			// 
			this->butSrl->Location = System::Drawing::Point(287, 30);
			this->butSrl->Name = L"butSrl";
			this->butSrl->Size = System::Drawing::Size(107, 23);
			this->butSrl->TabIndex = 4;
			this->butSrl->Text = L"SRL���J��";
			this->butSrl->UseVisualStyleBackColor = true;
			this->butSrl->Click += gcnew System::EventHandler(this, &Form1::butSrl_Click);
			// 
			// butSheet
			// 
			this->butSheet->Location = System::Drawing::Point(287, 64);
			this->butSheet->Name = L"butSheet";
			this->butSheet->Size = System::Drawing::Size(107, 23);
			this->butSheet->TabIndex = 5;
			this->butSheet->Text = L"��o�m�F�����J��";
			this->butSheet->UseVisualStyleBackColor = true;
			this->butSheet->Click += gcnew System::EventHandler(this, &Form1::butSheet_Click);
			// 
			// gridCompare
			// 
			dataGridViewCellStyle5->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleCenter;
			dataGridViewCellStyle5->BackColor = System::Drawing::SystemColors::Control;
			dataGridViewCellStyle5->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			dataGridViewCellStyle5->ForeColor = System::Drawing::SystemColors::WindowText;
			dataGridViewCellStyle5->SelectionBackColor = System::Drawing::SystemColors::Highlight;
			dataGridViewCellStyle5->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
			dataGridViewCellStyle5->WrapMode = System::Windows::Forms::DataGridViewTriState::True;
			this->gridCompare->ColumnHeadersDefaultCellStyle = dataGridViewCellStyle5;
			this->gridCompare->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->gridCompare->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(3) {this->colTitle, 
				this->colSrl, this->colSheet});
			this->gridCompare->Location = System::Drawing::Point(31, 113);
			this->gridCompare->Name = L"gridCompare";
			this->gridCompare->RowHeadersVisible = false;
			this->gridCompare->RowTemplate->Height = 21;
			this->gridCompare->Size = System::Drawing::Size(363, 170);
			this->gridCompare->TabIndex = 6;
			// 
			// tboxResult
			// 
			this->tboxResult->Location = System::Drawing::Point(294, 289);
			this->tboxResult->Name = L"tboxResult";
			this->tboxResult->ReadOnly = true;
			this->tboxResult->Size = System::Drawing::Size(100, 19);
			this->tboxResult->TabIndex = 7;
			// 
			// labResult
			// 
			this->labResult->AutoSize = true;
			this->labResult->Location = System::Drawing::Point(235, 292);
			this->labResult->Name = L"labResult";
			this->labResult->Size = System::Drawing::Size(53, 12);
			this->labResult->TabIndex = 8;
			this->labResult->Text = L"���茋��";
			// 
			// labVersion
			// 
			this->labVersion->AutoSize = true;
			this->labVersion->Location = System::Drawing::Point(353, 9);
			this->labVersion->Name = L"labVersion";
			this->labVersion->Size = System::Drawing::Size(41, 12);
			this->labVersion->TabIndex = 9;
			this->labVersion->Text = L"ver. 0.0";
			// 
			// colTitle
			// 
			dataGridViewCellStyle6->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleCenter;
			dataGridViewCellStyle6->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)), 
				static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->colTitle->DefaultCellStyle = dataGridViewCellStyle6;
			this->colTitle->HeaderText = L"";
			this->colTitle->Name = L"colTitle";
			this->colTitle->Width = 120;
			// 
			// colSrl
			// 
			dataGridViewCellStyle7->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleCenter;
			this->colSrl->DefaultCellStyle = dataGridViewCellStyle7;
			this->colSrl->HeaderText = L"SRL";
			this->colSrl->Name = L"colSrl";
			this->colSrl->Width = 120;
			// 
			// colSheet
			// 
			dataGridViewCellStyle8->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleCenter;
			this->colSheet->DefaultCellStyle = dataGridViewCellStyle8;
			this->colSheet->HeaderText = L"��o�m�F��";
			this->colSheet->Name = L"colSheet";
			this->colSheet->Width = 120;
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(429, 321);
			this->Controls->Add(this->labVersion);
			this->Controls->Add(this->labResult);
			this->Controls->Add(this->tboxResult);
			this->Controls->Add(this->gridCompare);
			this->Controls->Add(this->butSheet);
			this->Controls->Add(this->butSrl);
			this->Controls->Add(this->tboxSheet);
			this->Controls->Add(this->tboxSrl);
			this->Name = L"Form1";
			this->Text = L"TWL��o�m�F���`�F�b�J�[";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->gridCompare))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

private:
	// �o�[�W���������擾
	System::String^ getVersion( void )
	{
		System::Reflection::Assembly ^ass = System::Reflection::Assembly::GetEntryAssembly();
		System::Version ^ver =  ass->GetName()->Version;
		return ( ver->Major.ToString() + "." + ver->Minor.ToString() );
	}

private:
	// ��r��\�����Ĉ�v���邩����
	void updateResult()
	{
		if( !(*this->bReadSrl) || !(*this->bReadSheet) )	// �����Ƃ��ǂݍ��܂Ȃ��Ɣ��肵�Ȃ�
		{
			return;
		}

		// ��v���邩����
		if( *this->error == SheetCheckerError::NOERROR )
		{
			*this->error = checkSheet( this->rh, *this->fileCRC, this->sheet );
		}

		// �ЂƂЂƂ�grid�ɓo�^

		System::String ^tmp1, ^tmp2;	// grid �ɂ͕�����Œǉ����Ȃ���΂Ȃ�Ȃ�

		this->gridCompare->Rows->Clear();
		
		System::Text::UTF8Encoding^ utf8 = gcnew System::Text::UTF8Encoding( true );	// char->String�ϊ��ɕK�v
		tmp1 = gcnew System::String( this->rh->s.game_code,  0, GAME_CODE_MAX,  utf8 );
		tmp2 = gcnew System::String( this->sheet->GameCode,  0, GAME_CODE_MAX,  utf8 );
		this->gridCompare->Rows->Add( "�C�j�V�����R�[�h", tmp1, tmp2 );
		if( *this->error == SheetCheckerError::ERROR_VERIFY_GAME_CODE )
		{
			System::Int32 last = this->gridCompare->Rows->Count - 2;	// �ǉ�����̍s
			this->gridCompare->Rows[ last ]->DefaultCellStyle->ForeColor = System::Drawing::Color::Red;
		}

		tmp1 = this->rh->s.rom_version.ToString( "X02" );
		tmp2 = this->sheet->RomVersion.ToString( "X02" );
		this->gridCompare->Rows->Add( "���}�X�^�[�o�[�W����", tmp1, tmp2 );
		if( *this->error == SheetCheckerError::ERROR_VERIFY_ROM_VERSION )
		{
			System::Int32 last = this->gridCompare->Rows->Count - 2;
			this->gridCompare->Rows[ last ]->DefaultCellStyle->ForeColor = System::Drawing::Color::Red;
		}

		tmp1 = this->fileCRC->ToString( "X02" );
		tmp2 = this->sheet->FileCRC.ToString( "X02" );
		this->gridCompare->Rows->Add( "�t�@�C���S�̂�CRC", tmp1, tmp2 );
		if( *this->error == SheetCheckerError::ERROR_VERIFY_CRC )
		{
			System::Int32 last = this->gridCompare->Rows->Count - 2;
			this->gridCompare->Rows[ last ]->DefaultCellStyle->ForeColor = System::Drawing::Color::Red;
		}

		tmp2 = this->sheet->SubmitVersion.ToString() + " (" + this->sheet->SubmitVersion.ToString( "X1" ) + ")";
		this->gridCompare->Rows->Add( "��o�o�[�W����", "-", tmp2 );

		System::UInt16 tadver = sheet->RomVersion;
		tadver = (tadver << 8) | sheet->SubmitVersion;
		tmp2 = tadver.ToString() + " (" + tadver.ToString( "X04" ) + ")";
		this->gridCompare->Rows->Add( "TAD�o�[�W����", "-", tmp2);

		if( this->sheet->IsUnnecessaryRating )
		{
			this->gridCompare->Rows->Add( "TITLE_TYPE", "-", "TWL_APP");
			System::Int32 last = this->gridCompare->Rows->Count - 2;
			this->gridCompare->Rows[ last ]->DefaultCellStyle->ForeColor = System::Drawing::Color::Blue;	// �F�ŋ���
		}
		else
		{
			this->gridCompare->Rows->Add( "TITLE_TYPE", "-", "TWL_GAME");
		}

		if( *this->error == SheetCheckerError::NOERROR )
		{
			this->tboxResult->Text = "OK";
		}
		else
		{
			int code = (int)(*this->error);
			this->tboxResult->Text = "NG" + " (" + code.ToString() + ")";
		}

	}

private:
	// �t�@�C�����_�C�A���O�Ŏ擾
	System::String ^prevDir;	// �����l��nullptr
	System::String^ openFile( System::String ^filter )	// �t�@�C���̊g���q�t�B���^�������Ŏw��
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
		this->prevDir = System::IO::Path::GetDirectoryName( dlg->FileName );
		return dlg->FileName;
	}

private:
	// SRL���J�� �{�^���ƃh���b�O�A���h�h���b�v��2��ނ����邽�߃��b�v���Ă���
	void openSrl( System::String ^filename )
	{
		if( !filename )
		{
			memset( this->rh, 0, sizeof(ROM_Header) );
			this->tboxSrl->Text = "";
			*this->fileCRC = 0;
			*this->error = SheetCheckerError::ERROR_READ_SRL;
		}
		else
		{
			if( !readRomHeader( filename, this->rh ) )
			{
				memset( this->rh, 0, sizeof(ROM_Header) );
				this->tboxSrl->Text = "";
				*this->fileCRC = 0;
				*this->error = SheetCheckerError::ERROR_READ_SRL;
			}
			else
			{
				this->tboxSrl->Text = System::IO::Path::GetFileName( filename );
				u16 crc;
				getWholeCRCInFile( filename, &crc );
				*this->fileCRC = crc;
				*this->bReadSrl = true;		// ��x�ł��ǂݍ��񂾂�true
				*this->error = SheetCheckerError::NOERROR;
			}
		}
		this->updateResult();
	}

private:
	// ��o�m�F�����J��
	void openSheet( System::String ^filename )
	{
		if( !filename )
		{
			this->tboxSheet->Text = "";
			*this->error = SheetCheckerError::ERROR_READ_SHEET;
		}
		else
		{
			if( !readSheet( filename, this->sheet ) )
			{
				this->tboxSheet->Text = "";
				*this->error = SheetCheckerError::ERROR_READ_SHEET;
			}
			else
			{
				this->tboxSheet->Text = System::IO::Path::GetFileName( filename );
				*this->error = SheetCheckerError::NOERROR;
				*this->bReadSheet = true;
			}
		}
		this->updateResult();
	}

private:
	// SRL�̓ǂݍ��݃{�^��
	System::Void butSrl_Click(System::Object^  sender, System::EventArgs^  e)
	{
		System::String ^filename = openFile( "srl format (*.srl)|*.srl|All files (*.*)|*.*" );
		this->openSrl( filename );
	}

private:
	// ��o�m�F���̓ǂݍ��݃{�^��
	System::Void butSheet_Click(System::Object^  sender, System::EventArgs^  e)
	{
		System::String ^filename = openFile( "xml format (*.xml)|*.xml|All files (*.*)|*.*" );
		this->openSheet( filename );
	}

private:
	// �e�L�X�g�{�b�N�X�Ƀh���b�O���ꂽ�Ƃ�
	System::Void tboxSrl_DragEnter(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e)
	{
		if( e->Data->GetDataPresent( DataFormats::FileDrop ) )
		{
			e->Effect = DragDropEffects::All;
		}
	}
private:
	// �h���b�O���ă{�^���������ꂽ�Ƃ�
	System::Void tboxSrl_DragDrop(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e)
	{
		array<System::String^> ^files = dynamic_cast< array<System::String^> ^>(e->Data->GetData( DataFormats::FileDrop ));
		System::String ^filename = files[0];

		if( System::IO::File::Exists(filename) == false )
		{
			this->openSrl( nullptr );		// ���s�����Ƃ��̏����͌Ăяo�������\�b�h���Ŏ��s
		}
		else
		{
			this->openSrl( filename );
		}
	}
private:
	System::Void tboxSheet_DragEnter(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e)
	{
		if( e->Data->GetDataPresent( DataFormats::FileDrop ) )
		{
			e->Effect = DragDropEffects::All;
		}
	}
private:
	System::Void tboxSheet_DragDrop(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e)
	{
		array<System::String^> ^files = dynamic_cast< array<System::String^> ^>(e->Data->GetData( DataFormats::FileDrop ));
		System::String ^filename = files[0];

		if( System::IO::File::Exists(filename) == false )
		{
			this->openSheet( nullptr );
		}
		else
		{
			this->openSheet( filename );
		}
	}
};
}

