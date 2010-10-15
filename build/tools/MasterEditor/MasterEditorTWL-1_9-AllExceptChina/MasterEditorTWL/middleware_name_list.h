#pragma once

// ROM�f�[�^(SRL)�N���X�̊֘A�N���X�̐錾�ƒ�`

#include <apptype.h>
#include <twl/types.h>

namespace MasterEditorTWL
{
	// -------------------------------------------------------------------
	// Type : ref class
	// Name : RCMiddlewareName
	//
	// Description : 1�̃~�h���E�F�A�̖��O���
	// 
	// Role : �o�C�i���ɖ��ߍ��܂�Ă���~�h���E�F�A(���C�Z���X���)����
	//        ���̈�ʖ��̂�Ԃ�
	// -------------------------------------------------------------------
	ref class RCMiddlewareName
	{
	private:
		System::String ^publisher;	// �o�C�i���ɖ��ߍ��܂�Ă���͂��̒񋟌�
		System::String ^code;		// �o�C�i���ɖ��ߍ��܂�Ă���͂��̎��ʃR�[�h
		System::String ^noteJ;		// ��ʖ���
		System::String ^noteE;
	private:
		RCMiddlewareName(){}			// ������
	public:
		RCMiddlewareName( System::String ^pub, System::String ^c, System::String ^nJ, System::String ^nE )
		{
			this->publisher = pub;
			this->code      = c;
			this->noteJ     = nJ;
			this->noteE     = nE;
		}
	public:
		// �񋟌��Ǝ��ʃR�[�h����v����ꍇ�Ɉ�ʖ��̂�Ԃ�
		// ��v���Ȃ��ꍇ�ɂ� nullptr ��Ԃ�
		System::String^ check(System::String ^pub, System::String ^code, System::Boolean isJapanese)
		{
			if( pub->Equals(this->publisher) && code->StartsWith(this->code) )
			{
				if( isJapanese )
				{
					return System::String::Copy(this->noteJ);
				}
				else
				{
					return System::String::Copy(this->noteE);
				}
			}
			return nullptr;
		}
	}; //ref class RCMiddlewareName

	// -------------------------------------------------------------------
	// Type : ref class
	// Name : RCMiddlewareName
	//
	// Description : �~�h���E�F�A�̖��O���̃��X�g
	// 
	// Role : �ݒ�t�@�C�����̃~�h���E�F�A���̃��X�g����
	//        �e�~�h���E�F�A�̖��̂��擾��
	//        ���̂���������
	// -------------------------------------------------------------------
	ref class RCMiddlewareNameList
	{
	private:
		System::Collections::Generic::List<RCMiddlewareName^> ^list;
	private:
		RCMiddlewareNameList(){}
	public:
		RCMiddlewareNameList( System::String^ filepath )
		{
			this->makelist(filepath);
		}
	private:
		// �t�@�C������e�~�h���E�F�A�̖��̂��擾�����X�g���쐬����
		void makelist( System::String^ filepath )
		{
			this->list = gcnew System::Collections::Generic::List<RCMiddlewareName^>;
			this->list->Clear();

			// xml�t�@�C���̓ǂݍ���
			System::Xml::XmlDocument ^doc = gcnew System::Xml::XmlDocument();
			try
			{
				doc->Load( filepath );
			}
			catch( System::Exception ^s )
			{
				(void)s;
				return;
			}
			System::Xml::XmlElement ^root = doc->DocumentElement;

			// �~�h���E�F�A�̖��̏��̃��X�g���t�@�C������擾���ă��X�g�ɓo�^����
			System::Xml::XmlNodeList ^nodelist = root->SelectNodes( "/middleware-name-list/middleware" );	// �����^�O�����v�f�����ׂĎ擾
			System::Collections::IEnumerator^ iter = nodelist->GetEnumerator();
			while( iter->MoveNext() )
			{
				System::Xml::XmlNode ^node = safe_cast<System::Xml::XmlNode^>(iter->Current);
				System::Xml::XmlNode ^n_pub = node->SelectSingleNode( "publisher" );	// ���΃p�X
				System::Xml::XmlNode ^n_c   = node->SelectSingleNode( "code" );
				System::Xml::XmlNode ^n_nJ  = node->SelectSingleNode( "note-j" );
				System::Xml::XmlNode ^n_nE  = node->SelectSingleNode( "note-e" );

				// XML�m�[�h����f�[�^�𔲂��o���ă��X�g�ɏ����o�^���Ă���
				System::String ^pub = "";	// ��r�Ɏg���̂�nullptr�������Ȃ�
				if( n_pub && n_pub->FirstChild && n_pub->FirstChild->Value )
				{
					pub = n_pub->FirstChild->Value;
				}
				System::String ^c = "";
				if( n_c && n_c->FirstChild && n_c->FirstChild->Value )
				{
					c = n_c->FirstChild->Value;
				}
				System::String ^nJ = nullptr;	// ��r�Ɏg�p���Ȃ��̂�nullptr(�o�^����Ă��Ȃ����Ƃ�\��)
				if( n_nJ && n_nJ->FirstChild && n_nJ->FirstChild->Value )
				{
					nJ = n_nJ->FirstChild->Value;
				}
				System::String ^nE = nullptr;
				if( n_nE && n_nE->FirstChild && n_nE->FirstChild->Value )
				{
					nE = n_nE->FirstChild->Value;
				}
				this->list->Add( gcnew RCMiddlewareName(pub, c, nJ, nE ) );	// ���X�g�ɓo�^
			} //while
		} //void makelist()
	public:
		// ���X�g�����������ă~�h���E�F�A�̖��̂�Ԃ�
		// �o�^����Ă��Ȃ��ꍇ�� nullptr ��Ԃ�
		System::String^ search( System::String ^pub, System::String ^code, System::Boolean isJapanese )
		{
			for each (RCMiddlewareName ^mid in this->list )
			{
				System::String ^note = mid->check(pub, code, isJapanese);
				if( note != nullptr )
				{
					return note;
				}
			}
			return nullptr;
		} //System::String^ search()
	}; //ref class RCMiddlewareNameList

}; // namespace MasterEditorTWL