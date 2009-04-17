#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace FingerPrinterTWL {

	/// <summary>
	/// FormAbout の概要
	///
	/// 警告: このクラスの名前を変更する場合、このクラスが依存するすべての .resx ファイルに関連付けられた
	///          マネージ リソース コンパイラ ツールに対して 'Resource File Name' プロパティを
	///          変更する必要があります。この変更を行わないと、
	///          デザイナと、このフォームに関連付けられたローカライズ済みリソースとが、
	///          正しく相互に利用できなくなります。
	/// </summary>
	public ref class FormAbout : public System::Windows::Forms::Form
	{
	public:
		FormAbout(void)
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
		~FormAbout()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  lab;
	protected: 
	private: System::Windows::Forms::Label^  labVersion;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  tboxRemarks;
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
			this->lab = (gcnew System::Windows::Forms::Label());
			this->labVersion = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->tboxRemarks = (gcnew System::Windows::Forms::TextBox());
			this->butOK = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// lab
			// 
			this->lab->AutoSize = true;
			this->lab->Location = System::Drawing::Point(29, 25);
			this->lab->Name = L"lab";
			this->lab->Size = System::Drawing::Size(96, 12);
			this->lab->TabIndex = 0;
			this->lab->Text = L"TWL Fingerprinter";
			// 
			// labVersion
			// 
			this->labVersion->AutoSize = true;
			this->labVersion->Location = System::Drawing::Point(29, 46);
			this->labVersion->Name = L"labVersion";
			this->labVersion->Size = System::Drawing::Size(44, 12);
			this->labVersion->TabIndex = 1;
			this->labVersion->Text = L"Version";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(29, 67);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(138, 12);
			this->label1->TabIndex = 2;
			this->label1->Text = L"(C)2009 Nintendo Co., Ltd.";
			// 
			// tboxRemarks
			// 
			this->tboxRemarks->BackColor = System::Drawing::SystemColors::MenuBar;
			this->tboxRemarks->Location = System::Drawing::Point(23, 101);
			this->tboxRemarks->Multiline = true;
			this->tboxRemarks->Name = L"tboxRemarks";
			this->tboxRemarks->Size = System::Drawing::Size(238, 70);
			this->tboxRemarks->TabIndex = 3;
			// 
			// butOK
			// 
			this->butOK->Location = System::Drawing::Point(103, 190);
			this->butOK->Name = L"butOK";
			this->butOK->Size = System::Drawing::Size(75, 23);
			this->butOK->TabIndex = 4;
			this->butOK->Text = L"OK";
			this->butOK->UseVisualStyleBackColor = true;
			this->butOK->Click += gcnew System::EventHandler(this, &FormAbout::butOK_Click);
			// 
			// FormAbout
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(292, 230);
			this->Controls->Add(this->butOK);
			this->Controls->Add(this->tboxRemarks);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->labVersion);
			this->Controls->Add(this->lab);
			this->Name = L"FormAbout";
			this->Text = L"FormAbout";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	private:
		void construct()
		{
			// バージョン情報
			System::Reflection::Assembly ^ass = System::Reflection::Assembly::GetEntryAssembly();
			System::Version ^ver =  ass->GetName()->Version;
			this->labVersion->Text = "Version " + ver->Major.ToString() + "." + ver->Minor.ToString();
			// OpenSSL ライセンス注意書き
			this->tboxRemarks->Text = "This product includes software developed by the OpenSSL Project for use in the OpenSSL Toolkit (http://www.openssl.org/)";
			this->tboxRemarks->SelectionStart = 0;
		}

	private:
		System::Void butOK_Click(System::Object^  sender, System::EventArgs^  e)
		{
			this->Close();
		}
};
}
