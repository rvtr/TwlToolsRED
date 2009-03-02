#pragma once

// �����ꃁ�b�Z�[�W�̊Ǘ��N���X

namespace MasterEditorTWL
{

	// -------------------------------------------------------------------
	// Type : ref class
	// Name : RCMessageBank
	//
	// Description : ���b�Z�[�W������Ɏ����v��������ΕԂ�
	// 
	// Role : �����ꃁ�b�Z�[�W�̊Ǘ�
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
		// �O���t�@�C�����烁�b�Z�[�W��ǂݏo��
		// Caution: throwing Exception
		void loadMessage( System::String ^filepath, System::String ^lang );
	public:
		// ����̃��b�Z�[�W��Ԃ�
		System::String^ getMessage( System::String ^tag, System::String ^lang );
	}; //RCMessageBank

} //namespace MasterEditorTWL
