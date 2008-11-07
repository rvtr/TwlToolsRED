#pragma once

#include <apptype.h>
#include "common.h"
#include "srl.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace MasterEditorTWL {

	/// <summary>
	/// FormError �̊T�v
	///
	/// �x��: ���̃N���X�̖��O��ύX����ꍇ�A���̃N���X���ˑ����邷�ׂĂ� .resx �t�@�C���Ɋ֘A�t����ꂽ
	///          �}�l�[�W ���\�[�X �R���p�C�� �c�[���ɑ΂��� 'Resource File Name' �v���p�e�B��
	///          �ύX����K�v������܂��B���̕ύX���s��Ȃ��ƁA
	///          �f�U�C�i�ƁA���̃t�H�[���Ɋ֘A�t����ꂽ���[�J���C�Y�ς݃��\�[�X�Ƃ��A
	///          ���������݂ɗ��p�ł��Ȃ��Ȃ�܂��B
	/// </summary>
	public ref class FormError : public System::Windows::Forms::Form
	{
	public:
		FormError(void)
		{
			InitializeComponent();
			//
			//TODO: �����ɃR���X�g���N�^ �R�[�h��ǉ����܂�
			//
		}

	protected:
		/// <summary>
		/// �g�p���̃��\�[�X�����ׂăN���[���A�b�v���܂��B
		/// </summary>
		~FormError()
		{
			if (components)
			{
				delete components;
			}
		}
	public: System::Windows::Forms::DataGridView^  gridError;
	protected: 

	protected: 





	private: System::Windows::Forms::Label^  labError;
	public: System::Windows::Forms::DataGridView^  gridWarn;
	private: 


	public: 



	private: System::Windows::Forms::DataGridViewTextBoxColumn^  colWarnName;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  colWarnBegin;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  colWarnEnd;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  colWarnCause;
	private: System::Windows::Forms::Label^  labWarn;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  colErrorName;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  colErrorBegin;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  colErrorEnd;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  colErrorCause;
	private: System::Windows::Forms::Button^  butClose;

	private: 

	public: 







	protected: 

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
			this->gridError = (gcnew System::Windows::Forms::DataGridView());
			this->colErrorName = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->colErrorBegin = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->colErrorEnd = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->colErrorCause = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->labError = (gcnew System::Windows::Forms::Label());
			this->gridWarn = (gcnew System::Windows::Forms::DataGridView());
			this->colWarnName = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->colWarnBegin = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->colWarnEnd = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->colWarnCause = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->labWarn = (gcnew System::Windows::Forms::Label());
			this->butClose = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->gridError))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->gridWarn))->BeginInit();
			this->SuspendLayout();
			// 
			// gridError
			// 
			this->gridError->BackgroundColor = System::Drawing::SystemColors::Control;
			this->gridError->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->gridError->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(4) {this->colErrorName, 
				this->colErrorBegin, this->colErrorEnd, this->colErrorCause});
			this->gridError->GridColor = System::Drawing::SystemColors::Control;
			this->gridError->Location = System::Drawing::Point(28, 36);
			this->gridError->Name = L"gridError";
			this->gridError->ReadOnly = true;
			this->gridError->RowHeadersVisible = false;
			this->gridError->RowTemplate->Height = 21;
			this->gridError->Size = System::Drawing::Size(661, 172);
			this->gridError->TabIndex = 0;
			// 
			// colErrorName
			// 
			this->colErrorName->HeaderText = L"���ږ�";
			this->colErrorName->Name = L"colErrorName";
			this->colErrorName->ReadOnly = true;
			this->colErrorName->Width = 150;
			// 
			// colErrorBegin
			// 
			this->colErrorBegin->HeaderText = L"�J�n";
			this->colErrorBegin->Name = L"colErrorBegin";
			this->colErrorBegin->ReadOnly = true;
			this->colErrorBegin->Width = 60;
			// 
			// colErrorEnd
			// 
			this->colErrorEnd->HeaderText = L"�I��";
			this->colErrorEnd->Name = L"colErrorEnd";
			this->colErrorEnd->ReadOnly = true;
			this->colErrorEnd->Width = 60;
			// 
			// colErrorCause
			// 
			this->colErrorCause->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
			this->colErrorCause->HeaderText = L"�v��";
			this->colErrorCause->Name = L"colErrorCause";
			this->colErrorCause->ReadOnly = true;
			// 
			// labError
			// 
			this->labError->AutoSize = true;
			this->labError->ForeColor = System::Drawing::SystemColors::ActiveCaption;
			this->labError->Location = System::Drawing::Point(26, 21);
			this->labError->Name = L"labError";
			this->labError->Size = System::Drawing::Size(137, 12);
			this->labError->TabIndex = 1;
			this->labError->Text = L"�G���[(�K���C�����Ă�������)";
			// 
			// gridWarn
			// 
			this->gridWarn->BackgroundColor = System::Drawing::SystemColors::Control;
			this->gridWarn->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->gridWarn->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(4) {this->colWarnName, 
				this->colWarnBegin, this->colWarnEnd, this->colWarnCause});
			this->gridWarn->GridColor = System::Drawing::SystemColors::Control;
			this->gridWarn->Location = System::Drawing::Point(28, 245);
			this->gridWarn->Name = L"gridWarn";
			this->gridWarn->ReadOnly = true;
			this->gridWarn->RowHeadersVisible = false;
			this->gridWarn->RowTemplate->Height = 21;
			this->gridWarn->Size = System::Drawing::Size(661, 172);
			this->gridWarn->TabIndex = 2;
			// 
			// colWarnName
			// 
			this->colWarnName->HeaderText = L"���ږ�";
			this->colWarnName->Name = L"colWarnName";
			this->colWarnName->ReadOnly = true;
			this->colWarnName->Width = 150;
			// 
			// colWarnBegin
			// 
			this->colWarnBegin->HeaderText = L"�J�n";
			this->colWarnBegin->Name = L"colWarnBegin";
			this->colWarnBegin->ReadOnly = true;
			this->colWarnBegin->Width = 60;
			// 
			// colWarnEnd
			// 
			this->colWarnEnd->HeaderText = L"�I��";
			this->colWarnEnd->Name = L"colWarnEnd";
			this->colWarnEnd->ReadOnly = true;
			this->colWarnEnd->Width = 60;
			// 
			// colWarnCause
			// 
			this->colWarnCause->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
			this->colWarnCause->HeaderText = L"�v��";
			this->colWarnCause->Name = L"colWarnCause";
			this->colWarnCause->ReadOnly = true;
			// 
			// labWarn
			// 
			this->labWarn->AutoSize = true;
			this->labWarn->ForeColor = System::Drawing::SystemColors::ActiveCaption;
			this->labWarn->Location = System::Drawing::Point(26, 230);
			this->labWarn->Name = L"labWarn";
			this->labWarn->Size = System::Drawing::Size(331, 12);
			this->labWarn->TabIndex = 3;
			this->labWarn->Text = L"�x��(�C���͕K�{�ł͂���܂��񂪏��Ɍ�肪�Ȃ������m�F��������)";
			// 
			// butClose
			// 
			this->butClose->Location = System::Drawing::Point(557, 429);
			this->butClose->Name = L"butClose";
			this->butClose->Size = System::Drawing::Size(132, 23);
			this->butClose->TabIndex = 4;
			this->butClose->Text = L"����";
			this->butClose->UseVisualStyleBackColor = true;
			this->butClose->Click += gcnew System::EventHandler(this, &FormError::butClose_Click);
			// 
			// FormError
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(722, 464);
			this->Controls->Add(this->butClose);
			this->Controls->Add(this->labWarn);
			this->Controls->Add(this->gridWarn);
			this->Controls->Add(this->labError);
			this->Controls->Add(this->gridError);
			this->Name = L"FormError";
			this->Text = L"Error Information";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->gridError))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->gridWarn))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

		// --------------------------------------------------------
		// �G���[���̓o�^
		// --------------------------------------------------------
	public:
		void setGridError( System::Collections::Generic::List<RCMrcError ^> ^list, System::Boolean isJapanese )
		{
			this->gridError->Rows->Clear();
			if( list != nullptr )
			{
				for each( RCMrcError ^err in list )
				{
					this->gridError->Rows->Add( err->getAll( isJapanese ) );
				}
			}
		}

		void setGridWarn( System::Collections::Generic::List<RCMrcError ^> ^list, System::Boolean isJapanese )
		{
			this->gridWarn->Rows->Clear();
			if( list != nullptr )
			{
				for each( RCMrcError ^err in list )
				{
					this->gridWarn->Rows->Add( err->getAll( isJapanese ) );
				}
			}
		}

		// --------------------------------------------------------
		// �G���[���̏d�ˍ��킹
		// --------------------------------------------------------
	public:
		void overloadGridError( 
			System::Collections::Generic::List<RCMrcError ^> ^listLo,
			System::Collections::Generic::List<RCMrcError ^> ^listHi, System::Boolean isJapanese )
		{
			// listLo�̏C���\�ȏ���listHi�ŏ㏑������(listHi�͏C���\�ȏ��݂̂ł��邱�Ƃ��O��)
			this->gridError->Rows->Clear();
			if( listLo != nullptr )
			{
				for each( RCMrcError ^err in listLo )
				{
					if( !err->EnableModify )	// �C���\�ȏ��͕\�����Ȃ�
						this->gridError->Rows->Add( err->getAll( isJapanese ) );
				}
			}
			if( listHi != nullptr )
			{
				for each( RCMrcError ^err in listHi )
				{
					this->gridError->Rows->Add( err->getAll( isJapanese ) );
				}
			}
		}
		void overloadGridWarn( 
			System::Collections::Generic::List<RCMrcError ^> ^listLo,
			System::Collections::Generic::List<RCMrcError ^> ^listHi, System::Boolean isJapanese )
		{
			// listLo�̏C���\�ȏ���listHi�ŏ㏑������(listHi�͏C���\�ȏ��݂̂ł��邱�Ƃ��O��)
			this->gridWarn->Rows->Clear();
			if( listLo != nullptr )
			{
				for each( RCMrcError ^err in listLo )
				{
					if( !err->EnableModify )	// �C���\�ȏ��͕\�����Ȃ�
						this->gridWarn->Rows->Add( err->getAll( isJapanese ) );
				}
			}
			if( listHi != nullptr )
			{
				for each( RCMrcError ^err in listHi )
				{
					this->gridWarn->Rows->Add( err->getAll( isJapanese ) );
				}
			}
		}

		// --------------------------------------------------------
		// ����Ή�
		// --------------------------------------------------------
	public:
		void changeJapanese(void)
		{
			this->labError->Text = gcnew System::String( "�G���[(�K���C�����Ă��������B)" );
			this->colErrorName->HeaderText  = gcnew System::String( "���ږ�" );
			this->colErrorBegin->HeaderText = gcnew System::String( "�J�n" );
			this->colErrorEnd->HeaderText   = gcnew System::String( "�I��" );
			this->colErrorCause->HeaderText = gcnew System::String( "�v��" );

			this->labWarn->Text  = gcnew System::String( "�x��(�C���͕K�{�ł͂���܂��񂪏��Ɍ�肪�Ȃ������m�F���������B)" );
			this->colWarnName->HeaderText  = gcnew System::String( "���ږ�" );
			this->colWarnBegin->HeaderText = gcnew System::String( "�J�n" );
			this->colWarnEnd->HeaderText   = gcnew System::String( "�I��" );
			this->colWarnCause->HeaderText = gcnew System::String( "�v��" );

			this->butClose->Text = gcnew System::String( "����" );
		}

	public:
		void changeEnglish(void)
		{
			this->labError->Text = gcnew System::String( "Error Information(Modification is necessary.)" );
			this->colErrorName->HeaderText  = gcnew System::String( "Name" );
			this->colErrorBegin->HeaderText = gcnew System::String( "Begin" );
			this->colErrorEnd->HeaderText   = gcnew System::String( "End" );
			this->colErrorCause->HeaderText = gcnew System::String( "Reason" );

			this->labWarn->Text  = gcnew System::String( "Warning(Modification is not necessary. Please check validity of these information.)" );
			this->colWarnName->HeaderText  = gcnew System::String( "Name" );
			this->colWarnBegin->HeaderText = gcnew System::String( "Begin" );
			this->colWarnEnd->HeaderText   = gcnew System::String( "End" );
			this->colWarnCause->HeaderText = gcnew System::String( "Reason" );

			this->butClose->Text = gcnew System::String( "Close window" );
		}

	private:
		System::Void butClose_Click(System::Object^  sender, System::EventArgs^  e)
		{
			this->Close();
		}
};
}
