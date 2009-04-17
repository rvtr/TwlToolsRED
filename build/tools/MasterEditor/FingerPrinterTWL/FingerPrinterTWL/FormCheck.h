#pragma once

#include "fingerprint_util.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace FingerPrinterTWL {

	/// <summary>
	/// FormCheck の概要
	///
	/// 警告: このクラスの名前を変更する場合、このクラスが依存するすべての .resx ファイルに関連付けられた
	///          マネージ リソース コンパイラ ツールに対して 'Resource File Name' プロパティを
	///          変更する必要があります。この変更を行わないと、
	///          デザイナと、このフォームに関連付けられたローカライズ済みリソースとが、
	///          正しく相互に利用できなくなります。
	/// </summary>
	public ref class FormCheck : public System::Windows::Forms::Form
	{
	public:
		FormCheck(void)
		{
			InitializeComponent();
			//
			//TODO: ここにコンストラクタ コードを追加します
			//
		}

	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormCheck()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  labMsg;
	private: System::Windows::Forms::Label^  labWarning;
	private: System::Windows::Forms::Label^  labAscii;
	private: System::Windows::Forms::Label^  labHex;
	private: System::Windows::Forms::TextBox^  tboxAscii;
	private: System::Windows::Forms::TextBox^  tboxHex;
	private: System::Windows::Forms::Button^  butOK;



	protected: 


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
			this->labMsg = (gcnew System::Windows::Forms::Label());
			this->labWarning = (gcnew System::Windows::Forms::Label());
			this->labAscii = (gcnew System::Windows::Forms::Label());
			this->labHex = (gcnew System::Windows::Forms::Label());
			this->tboxAscii = (gcnew System::Windows::Forms::TextBox());
			this->tboxHex = (gcnew System::Windows::Forms::TextBox());
			this->butOK = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// labMsg
			// 
			this->labMsg->AutoSize = true;
			this->labMsg->Location = System::Drawing::Point(21, 27);
			this->labMsg->Name = L"labMsg";
			this->labMsg->Size = System::Drawing::Size(290, 12);
			this->labMsg->TabIndex = 0;
			this->labMsg->Text = L"The fingerprint has been registered in the ROM already.";
			// 
			// labWarning
			// 
			this->labWarning->AutoSize = true;
			this->labWarning->Location = System::Drawing::Point(21, 47);
			this->labWarning->Name = L"labWarning";
			this->labWarning->Size = System::Drawing::Size(261, 12);
			this->labWarning->TabIndex = 1;
			this->labWarning->Text = L"You can replace it with new one by re-registering.";
			// 
			// labAscii
			// 
			this->labAscii->AutoSize = true;
			this->labAscii->Location = System::Drawing::Point(21, 96);
			this->labAscii->Name = L"labAscii";
			this->labAscii->Size = System::Drawing::Size(94, 12);
			this->labAscii->TabIndex = 2;
			this->labAscii->Text = L"ASCII Characters";
			// 
			// labHex
			// 
			this->labHex->AutoSize = true;
			this->labHex->Location = System::Drawing::Point(21, 167);
			this->labHex->Name = L"labHex";
			this->labHex->Size = System::Drawing::Size(119, 12);
			this->labHex->TabIndex = 3;
			this->labHex->Text = L"Hexadecimal Numbers";
			// 
			// tboxAscii
			// 
			this->tboxAscii->Location = System::Drawing::Point(23, 111);
			this->tboxAscii->Multiline = true;
			this->tboxAscii->Name = L"tboxAscii";
			this->tboxAscii->ReadOnly = true;
			this->tboxAscii->Size = System::Drawing::Size(288, 37);
			this->tboxAscii->TabIndex = 4;
			// 
			// tboxHex
			// 
			this->tboxHex->Location = System::Drawing::Point(23, 182);
			this->tboxHex->Multiline = true;
			this->tboxHex->Name = L"tboxHex";
			this->tboxHex->ReadOnly = true;
			this->tboxHex->Size = System::Drawing::Size(288, 37);
			this->tboxHex->TabIndex = 5;
			// 
			// butOK
			// 
			this->butOK->Location = System::Drawing::Point(172, 257);
			this->butOK->Name = L"butOK";
			this->butOK->Size = System::Drawing::Size(75, 23);
			this->butOK->TabIndex = 6;
			this->butOK->Text = L"OK";
			this->butOK->UseVisualStyleBackColor = true;
			this->butOK->Click += gcnew System::EventHandler(this, &FormCheck::butOK_Click);
			// 
			// FormCheck
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(418, 296);
			this->Controls->Add(this->butOK);
			this->Controls->Add(this->tboxHex);
			this->Controls->Add(this->tboxAscii);
			this->Controls->Add(this->labHex);
			this->Controls->Add(this->labAscii);
			this->Controls->Add(this->labWarning);
			this->Controls->Add(this->labMsg);
			this->Name = L"FormCheck";
			this->Text = L"Fingerprint Checker";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	// ================================================================================
	// フィンガープリントの表示
	// ================================================================================
	public:
		void displayFingerprint( unsigned char fingerprint[32] )
		{
			cli::array<System::Byte> ^bytes = gcnew cli::array<System::Byte>(32);
			int i;
			for( i=0; i < 32; i++ )
			{
				bytes[i] = fingerprint[i];
			}
			this->displayFingerprint( bytes );
		}
		void displayFingerprint( cli::array<System::Byte> ^fingerprint )
		{
			this->tboxAscii->Text = TransBytesToString( fingerprint );
			this->tboxHex->Text   = TransBytesToHexString( fingerprint );

			this->tboxAscii->SelectionStart = 0;
			this->tboxHex->SelectionStart   = 0;
		}

	// ================================================================================
	// GUI処理
	// ================================================================================
	private:
		System::Void butOK_Click(System::Object^  sender, System::EventArgs^  e)
		{
			this->Close();
		}
};
}
