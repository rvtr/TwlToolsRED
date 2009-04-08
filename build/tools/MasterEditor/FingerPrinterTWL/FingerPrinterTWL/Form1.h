#pragma once

#include <twl/types.h>
#include <twl/os/common/format_rom.h>

namespace FingerPrinterTWL {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Form1 の概要
	///
	/// 警告: このクラスの名前を変更する場合、このクラスが依存するすべての .resx ファイルに関連付けられた
	///          マネージ リソース コンパイラ ツールに対して 'Resource File Name' プロパティを
	///          変更する必要があります。この変更を行わないと、
	///          デザイナと、このフォームに関連付けられたローカライズ済みリソースとが、
	///          正しく相互に利用できなくなります。
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: ここにコンストラクタ コードを追加します
			//
			this->construct();
		}

	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~Form1()
		{
			this->!Form1();
		}
		!Form1()
		{
			this->destruct();
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TextBox^  tboxFile;
	protected: 

	protected: 
	private: System::Windows::Forms::Label^  labFile;
	private: System::Windows::Forms::Button^  butFile;
	private: System::Windows::Forms::GroupBox^  gboxFP;
	private: System::Windows::Forms::TextBox^  tboxExample;

	private: System::Windows::Forms::TextBox^  tboxFP;
	private: System::Windows::Forms::GroupBox^  gboxType;
	private: System::Windows::Forms::RadioButton^  rHex;

	private: System::Windows::Forms::RadioButton^  rString;
	private: System::Windows::Forms::Label^  labShorter;
	private: System::Windows::Forms::Button^  butFP;



	private: System::Windows::Forms::Label^  labFP;


	private:
		/// <summary>
		/// 必要なデザイナ変数です。
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// デザイナ サポートに必要なメソッドです。このメソッドの内容を
		/// コード エディタで変更しないでください。
		/// </summary>
		void InitializeComponent(void)
		{
			this->tboxFile = (gcnew System::Windows::Forms::TextBox());
			this->labFile = (gcnew System::Windows::Forms::Label());
			this->butFile = (gcnew System::Windows::Forms::Button());
			this->gboxFP = (gcnew System::Windows::Forms::GroupBox());
			this->labFP = (gcnew System::Windows::Forms::Label());
			this->labShorter = (gcnew System::Windows::Forms::Label());
			this->butFP = (gcnew System::Windows::Forms::Button());
			this->tboxExample = (gcnew System::Windows::Forms::TextBox());
			this->tboxFP = (gcnew System::Windows::Forms::TextBox());
			this->gboxType = (gcnew System::Windows::Forms::GroupBox());
			this->rHex = (gcnew System::Windows::Forms::RadioButton());
			this->rString = (gcnew System::Windows::Forms::RadioButton());
			this->gboxFP->SuspendLayout();
			this->gboxType->SuspendLayout();
			this->SuspendLayout();
			// 
			// tboxFile
			// 
			this->tboxFile->Location = System::Drawing::Point(85, 31);
			this->tboxFile->Name = L"tboxFile";
			this->tboxFile->ReadOnly = true;
			this->tboxFile->Size = System::Drawing::Size(313, 19);
			this->tboxFile->TabIndex = 0;
			// 
			// labFile
			// 
			this->labFile->AutoSize = true;
			this->labFile->Location = System::Drawing::Point(26, 34);
			this->labFile->Name = L"labFile";
			this->labFile->Size = System::Drawing::Size(53, 12);
			this->labFile->TabIndex = 1;
			this->labFile->Text = L"ROM File";
			// 
			// butFile
			// 
			this->butFile->Location = System::Drawing::Point(404, 29);
			this->butFile->Name = L"butFile";
			this->butFile->Size = System::Drawing::Size(82, 23);
			this->butFile->TabIndex = 2;
			this->butFile->Text = L"Open File";
			this->butFile->UseVisualStyleBackColor = true;
			this->butFile->Click += gcnew System::EventHandler(this, &Form1::butFile_Click);
			// 
			// gboxFP
			// 
			this->gboxFP->Controls->Add(this->labFP);
			this->gboxFP->Controls->Add(this->labShorter);
			this->gboxFP->Controls->Add(this->butFP);
			this->gboxFP->Controls->Add(this->tboxExample);
			this->gboxFP->Controls->Add(this->tboxFP);
			this->gboxFP->Controls->Add(this->gboxType);
			this->gboxFP->Location = System::Drawing::Point(28, 71);
			this->gboxFP->Name = L"gboxFP";
			this->gboxFP->Size = System::Drawing::Size(458, 261);
			this->gboxFP->TabIndex = 3;
			this->gboxFP->TabStop = false;
			// 
			// labFP
			// 
			this->labFP->AutoSize = true;
			this->labFP->Location = System::Drawing::Point(20, 150);
			this->labFP->Name = L"labFP";
			this->labFP->Size = System::Drawing::Size(60, 12);
			this->labFP->TabIndex = 5;
			this->labFP->Text = L"Fingerprint";
			// 
			// labShorter
			// 
			this->labShorter->AutoSize = true;
			this->labShorter->Location = System::Drawing::Point(320, 204);
			this->labShorter->Name = L"labShorter";
			this->labShorter->Size = System::Drawing::Size(124, 12);
			this->labShorter->TabIndex = 4;
			this->labShorter->Text = L"(Shorter than 32 bytes)";
			// 
			// butFP
			// 
			this->butFP->Location = System::Drawing::Point(151, 225);
			this->butFP->Name = L"butFP";
			this->butFP->Size = System::Drawing::Size(150, 23);
			this->butFP->TabIndex = 3;
			this->butFP->Text = L"Register Fingerprint";
			this->butFP->UseVisualStyleBackColor = true;
			this->butFP->Click += gcnew System::EventHandler(this, &Form1::butFP_Click);
			// 
			// tboxExample
			// 
			this->tboxExample->BackColor = System::Drawing::SystemColors::Info;
			this->tboxExample->Location = System::Drawing::Point(232, 18);
			this->tboxExample->Multiline = true;
			this->tboxExample->Name = L"tboxExample";
			this->tboxExample->ReadOnly = true;
			this->tboxExample->Size = System::Drawing::Size(212, 122);
			this->tboxExample->TabIndex = 2;
			// 
			// tboxFP
			// 
			this->tboxFP->Location = System::Drawing::Point(22, 165);
			this->tboxFP->Multiline = true;
			this->tboxFP->Name = L"tboxFP";
			this->tboxFP->Size = System::Drawing::Size(422, 36);
			this->tboxFP->TabIndex = 1;
			// 
			// gboxType
			// 
			this->gboxType->Controls->Add(this->rHex);
			this->gboxType->Controls->Add(this->rString);
			this->gboxType->Location = System::Drawing::Point(22, 18);
			this->gboxType->Name = L"gboxType";
			this->gboxType->Size = System::Drawing::Size(204, 113);
			this->gboxType->TabIndex = 0;
			this->gboxType->TabStop = false;
			this->gboxType->Text = L"Input Type";
			// 
			// rHex
			// 
			this->rHex->AutoSize = true;
			this->rHex->Location = System::Drawing::Point(18, 68);
			this->rHex->Name = L"rHex";
			this->rHex->Size = System::Drawing::Size(137, 16);
			this->rHex->TabIndex = 1;
			this->rHex->TabStop = true;
			this->rHex->Text = L"Hexadecimal Numbers";
			this->rHex->UseVisualStyleBackColor = true;
			// 
			// rString
			// 
			this->rString->AutoSize = true;
			this->rString->Checked = true;
			this->rString->Location = System::Drawing::Point(18, 37);
			this->rString->Name = L"rString";
			this->rString->Size = System::Drawing::Size(160, 16);
			this->rString->TabIndex = 0;
			this->rString->TabStop = true;
			this->rString->Text = L"String of ASCII Charactors";
			this->rString->UseVisualStyleBackColor = true;
			// 
			// Form1
			// 
			this->AllowDrop = true;
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::None;
			this->ClientSize = System::Drawing::Size(503, 352);
			this->Controls->Add(this->gboxFP);
			this->Controls->Add(this->butFile);
			this->Controls->Add(this->labFile);
			this->Controls->Add(this->tboxFile);
			this->Name = L"Form1";
			this->Text = L"TWL Fingerprinter";
			this->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &Form1::Form1_DragDrop);
			this->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &Form1::Form1_DragEnter);
			this->gboxFP->ResumeLayout(false);
			this->gboxFP->PerformLayout();
			this->gboxType->ResumeLayout(false);
			this->gboxType->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	// ================================================================================
	// internal functions
	// ================================================================================
	private:
		ROM_Header *rh;
		System::String ^prevDir;	// 前に選択したディレクトリ

	private:
		void construct();
		void destruct();

	// フィンガープリント
	private:
		void fingerprintRomHeader();

	// ファイル処理を共通化させる
	private:
		void commonOpenRom( System::String ^srcpath );
		void commonSaveRom( System::String ^dstpath );

	// ボタンが押されたときの処理(例外catchを入れること)
	private:
		void procOpenRomButton();
		void procSaveRomButton();

	// ダイアログ
	private:
		void sucMsg( System::String ^fmt, ... cli::array<System::String^> ^args );
		void errMsg( System::String ^fmt, ... cli::array<System::String^> ^args );


	// ================================================================================
	// GUI処理
	// ================================================================================
		// ボタン
	private:
		System::Void butFile_Click(System::Object^  sender, System::EventArgs^  e)
		{
			this->procOpenRomButton();
		}
	private:
		System::Void butFP_Click(System::Object^  sender, System::EventArgs^  e)
		{
			this->procSaveRomButton();
		}
		// ドラッグアンドドロップ
	private:
		System::Void Form1_DragEnter(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e)
		{
			if( e->Data->GetDataPresent( DataFormats::FileDrop ) )
			{
				e->Effect = DragDropEffects::All;
			}
		}
	private:
		System::Void Form1_DragDrop(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e)
		{
			array<String^> ^files = dynamic_cast< array<String^> ^>(e->Data->GetData( DataFormats::FileDrop ) );
			String ^path = files[0];
			this->commonOpenRom( path );
		}
};
}

