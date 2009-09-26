#pragma once

// 多言語メッセージの管理クラス

namespace MasterEditorTWL
{

	// -------------------------------------------------------------------
	// Type : ref class
	// Name : RCMessageBank
	//
	// Description : メッセージを内部に持ち要求があれば返す
	// 
	// Role : 多言語メッセージの管理
	// -------------------------------------------------------------------
	ref class RCMessageBank
	{
	private:
		System::Xml::XmlElement ^rootJ;
		System::Xml::XmlElement ^rootE;
	private:
		RCMessageBank(){};
	public:
		// Caution: throwing Exception
		RCMessageBank( System::String ^fileJ, System::String ^fileE );
	private:
		// 外部ファイルからメッセージを読み出す
		// Caution: throwing Exception
		void loadMessage( System::String ^filepath, System::String ^lang );
	public:
		// 特定のメッセージを返す
		System::String^ getMessage( System::String ^tag, System::String ^lang );
	}; //RCMessageBank

} //namespace MasterEditorTWL
