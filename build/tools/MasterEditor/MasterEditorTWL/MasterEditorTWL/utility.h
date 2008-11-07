#pragma once

// ���p���C�u�����E�N���X�̐錾

#include <twl/types.h>
#include <cstring>
#include <cstdio>

namespace MasterEditorTWL
{

	// -------------------------------------------------------------------
	// functions
	// -------------------------------------------------------------------

	//
	// String �� char �z��Ɋi�[
	//
	// @arg [out] �i�[��
	// @arg [in]  �i�[��
	// @arg [in]  ������
	// @arg [in]  �]��𖄂߂� padding
	//
	void setStringToChars( char *pDst, System::String ^hSrc, const System::Int32 nMax, const System::SByte pad  );

	//
	// ROM�w�b�_����ROM�e�ʐݒ�o�C�g����ROM�e�ʂ�\����������擾
	//
	// @arg [in]  ROM�e�ʐݒ�o�C�g
	//
	// @ret ������(�P�ʂ�)
	//
	System::String^ transRomSizeToString( System::Byte ^byte );

	//
	// �T�C�Y����P�ʂ��̕�������擾
	//
	// @arg [in] �o�C�g��
	//
	// @ret ������(�P�ʂ�)
	//
	System::String^ transSizeToString( System::UInt32 ^size );

	//
	// �y�A�����^���R���g���[���̊e�c�̂̐ݒ�\�N����擾����
	//
	// @arg [in] �c��
	//
	cli::array<System::Byte>^ getOgnRatingAges( int ogn );

	//
	// �y�A�����^���R���g���[���̊e�c�̂̒c�̖����擾����
	//
	// @arg [in] �c��
	//
	System::String^ getOgnName( int ogn );

	//
	// �o�C�g��ɓ���̃p�^�[�����܂܂�邩�ǂ����}�b�`���O����
	//
	// @arg [in] �e�L�X�g
	//      [in] �e�L�X�g�̒���
	//      [in] �p�^�[��
	//      [in] �p�^�[���̒���
	//      [in] �e�L�X�g�̏I�[�܂Œ��ׂ����_�Ńe�L�X�g���r���܂Ń}�b�`���Ă����ꍇ�𐬌��Ƃ݂Ȃ���
	//
	// @ret �}�b�`�����e�L�X�g�̃C���f�b�N�X�����X�g�ŕԂ��B
	//
	System::Collections::Generic::List<u32>^ patternMatch( 
		const u8 *text, const u32 textLen, const u8 *pattern, const u32 patternLen, const System::Boolean enableLast );

	//
	// �t�@�C���Ƀo�C�g��̃p�^�[�����܂܂�邩�ǂ����}�b�`���O����
	//
	// @arg [in] �}�b�`�ΏۂƂȂ�t�@�C���|�C���^
	// @arg [in] �p�^�[��
	// @arg [in] �p�^�[���̒���
	//
	// @ret �}�b�`�����e�L�X�g�̃C���f�b�N�X�����X�g�ŕԂ��B
	//      �Ō�܂Ń}�b�`�����ꍇ�̂ݐ��������Ƃ݂Ȃ��B
	//
	System::Collections::Generic::List<u32>^ patternMatch( FILE *fp, const u8 *pattern, const u32 patternLen );

	//
	// XML�̃��[�g�m�[�h����w�肵�����O�̃^�O���������ĕԂ�
	//
	// @arg [in] XML�̃��[�g�m�[�h
	// @arg [in] �^�O��
	//
	// @ret �����Ń}�b�`�����Ƃ��m�[�h��Ԃ��B�Ȃ��Ƃ��� nullptr�B
	//      �������A�ŏ��Ƀ}�b�`�������̂̂ݕԂ�
	//
	System::Xml::XmlNode^ searchXmlNode( System::Xml::XmlElement ^root, System::String ^tag );

	//
	// �^�O���������Ă��̃e�L�X�g���w�肵���e�L�X�g�ƈ�v���邩���ׂ�
	//
	// @arg [in] XML�̃��[�g�m�[�h
	// @arg [in] �^�O��
	// @arg [in] �l
	//
	// @ret ��v����Ƃ�true�B��v���Ȃ��Ƃ��A�^�O�����݂��Ȃ��Ƃ���false�B
	//
	System::Boolean isXmlEqual( System::Xml::XmlElement ^root, System::String ^tag, System::String ^val );

	//
	// �^�O���������Ă��̃e�L�X�g��Ԃ�
	//
	// @arg [in] XML�̃��[�g�m�[�h
	// @arg [in] �^�O��XPath
	//
	// @ret �e�L�X�g�����݂���Ƃ����̃e�L�X�g��Ԃ��B���݂��Ȃ��Ƃ�nullptr�B
	//
	System::String^ getXPathText( System::Xml::XmlElement ^root, System::String ^xpath );

	//
	// XML�Ƀ^�O��ǉ�����
	//
	// @arg [in] XML�h�L�������g
	// @arg [in] �e�m�[�h
	// @arg [in] �^�O��
	// @arg [in] �e�L�X�g
	//
	void appendXmlTag( System::Xml::XmlDocument ^doc, System::Xml::XmlElement ^parent,
					   System::String ^tag, System::String ^text );

	//
	// �r�b�g�����J�E���g����
	//
	// @arg [in] �l
	//
	// @ret �r�b�g��
	//
	u32 countBits( const u32 val );

} // end of namespace MasterEditorTWL
