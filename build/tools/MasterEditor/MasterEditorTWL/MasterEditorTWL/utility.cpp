// utility.h �̎���

#include "stdafx.h"
#include <apptype.h>
#include "common.h"
#include "utility.h"
#include <cstring>
#include <cstdio>
#include <twl/types.h>
#include <twl/os/common/format_rom.h>
#include <twl/os/common/ownerInfoEx.h>

//
// String �� char �z��Ɋi�[
//
// @arg [out] �i�[��
// @arg [in]  �i�[��
// @arg [in]  ������
// @arg [in]  �]��𖄂߂� padding
//
void MasterEditorTWL::setStringToChars( char *pDst, System::String ^hSrc, 
									    const System::Int32 nMax, const System::SByte pad )
{
	System::Int32 i;

	memset( pDst, pad, nMax );
	for( i=0; (i < hSrc->Length) && (i < nMax); i++ )
	{
		pDst[i] = (char)hSrc[i];
	}
}

//
// ROM�w�b�_����ROM�e�ʐݒ�o�C�g����ROM�e�ʂ�\����������擾
//
// @arg [in]  ROM�e�ʐݒ�o�C�g
//
// @ret ������(�P�ʂ�)
//
System::String^ MasterEditorTWL::transRomSizeToString( System::Byte ^byte )
{
	System::UInt32  size;
	System::String  ^unit;

	// RomSize = 2^(byte) [Mbit] 
	size = 1 << *(byte);

	if(size >= 1024)
	{
		unit = gcnew System::String("Gbit");
		size = size / 1024;
	}
	else
	{
		unit = gcnew System::String("Mbit");
	}
	return (size.ToString() + unit);
}

//
// �T�C�Y����P�ʂ��̕�������擾
//
// @arg [in] �o�C�g��
//
// @ret ������(�P�ʂ�)
//
System::String^ MasterEditorTWL::transSizeToString( System::UInt32 ^size )
{
	if( size == nullptr )
	{
		return (gcnew System::String(""));
	}

	System::UInt32  val = *size;

	// Byte�P��
	if( val == 0 )
	{
		return val.ToString();
	}
	if( val < 1024 )
	{
		return (val.ToString() + " Byte");
	}

	// KB�P�ʂɕϊ�
	if( (val % 1024) != 0 )
	{
		return (val.ToString() + " Byte");	// �[���̂Ƃ��͒P�ʕϊ����Ȃ�
	}
	val = val / 1024;
	if( val < 1024 )
	{
		return (val.ToString() + " KB");
	}
	
	// MB�P�ʂɕϊ�
	if( (val % 1024) != 0 )
	{
		return (val.ToString() + " KB");
	}
	val = val / 1024;
	if( val < 1024)
	{
		return (val.ToString() + " MB");
	}

	// GB�P�ʂɕϊ�
	if( (val % 1024) != 0 )
	{
		return (val.ToString() + " MB");
	}
	val = val / 1024;
	return (val.ToString() + " GB");
}

// �ݒ�\�ȃC���f�b�N�X�̃��X�g���擾����
cli::array<System::Byte>^ MasterEditorTWL::getOgnRatingAges( int ogn )
{
	cli::array<System::Byte> ^ages = gcnew cli::array<System::Byte>{0};

	if( ogn == OS_TWL_PCTL_OGN_CERO )
	{
		ages = gcnew cli::array<System::Byte>{0,12,15,17,18};
	}
	else if( ogn == OS_TWL_PCTL_OGN_ESRB )
	{
		ages = gcnew cli::array<System::Byte>{0,3,6,10,13,17};
	}
	else if( ogn == OS_TWL_PCTL_OGN_USK )
	{
		ages = gcnew cli::array<System::Byte>{0,6,12,16,18};
	}
	else if( ogn == OS_TWL_PCTL_OGN_PEGI_GEN )
	{
		ages = gcnew cli::array<System::Byte>{0,3,7,12,16,18};
	}
	else if( ogn == OS_TWL_PCTL_OGN_PEGI_PRT )
	{
		ages = gcnew cli::array<System::Byte>{0,4,6,12,16,18};
	}
	else if( ogn == OS_TWL_PCTL_OGN_PEGI_BBFC )
	{
		ages = gcnew cli::array<System::Byte>{0,3,4,7,8,12,15,16,18};
	}
	else if( ogn == OS_TWL_PCTL_OGN_OFLC )
	{
		ages = gcnew cli::array<System::Byte>{0,7,14,15};
	}
	return ages;
}

// �ݒ�\�ȃC���f�b�N�X�̃��X�g���擾����
System::String^ MasterEditorTWL::getOgnName( int ogn )
{
	System::String ^name = gcnew System::String("");

	if( ogn == OS_TWL_PCTL_OGN_CERO )
	{
		name = gcnew System::String("CERO");
	}
	else if( ogn == OS_TWL_PCTL_OGN_ESRB )
	{
		name = gcnew System::String("ESRB");
	}
	else if( ogn == OS_TWL_PCTL_OGN_USK )
	{
		name = gcnew System::String("USK");
	}
	else if( ogn == OS_TWL_PCTL_OGN_PEGI_GEN )
	{
		name = gcnew System::String("PEGI(General)");
	}
	else if( ogn == OS_TWL_PCTL_OGN_PEGI_PRT )
	{
		name = gcnew System::String("PEGI(Portugal)");
	}
	else if( ogn == OS_TWL_PCTL_OGN_PEGI_BBFC )
	{
		name = gcnew System::String("PEGI and BBFC");
	}
	else if( ogn == OS_TWL_PCTL_OGN_OFLC )
	{
		name = gcnew System::String("OFLC");
	}
	return name;
}

//
// �o�C�g��ɓ���̃p�^�[�����܂܂�邩�ǂ����}�b�`���O����
//
// @arg [in] �e�L�X�g
//      [in] �e�L�X�g�̒���
//      [in] �p�^�[��
//      [in] �p�^�[���̒���
//      [in] �e�L�X�g�̏I�[�܂Œ��ׂ����_�Ńe�L�X�g���r���܂Ń}�b�`���Ă����ꍇ�𐬌��Ƃ݂Ȃ���
//
// @ret �}�b�`�����e�L�X�g�̃I�t�Z�b�g�����X�g�ŕԂ��B
//
System::Collections::Generic::List<u32>^ MasterEditorTWL::patternMatch( 
	const u8 *text, const u32 textLen, const u8 *pattern, const u32 patternLen, const System::Boolean enableLast )
{
	// �ЂƂ܂��V���v���ȕ��@�Ŏ�������
	u32  first;
	u32  len;
	System::Collections::Generic::List<u32> ^list = gcnew System::Collections::Generic::List<u32>();
	list->Clear();

	// �p�^�[����1���������炵�Ȃ���}�b�`���O
	for( first=0; first < textLen; first++ )
	{
		len = (patternLen < (textLen-first))?patternLen:(textLen-first);	// �Ō�̂ق��͓r���܂ł����}�b�`���O���Ȃ�
		if( *(text+first) == pattern[0] )		// �������̂��ߍŏ��̕�������v�����Ƃ��̂ݑS���}�b�`���O
		{
			if( memcmp( text+first, pattern, len ) == 0 )
			{
				if( (enableLast == true) )
				{
					list->Add( first );
				}
				else if( len == patternLen )	// ���S��v���Ȃ��ƃ_��
				{
					list->Add( first );
				}
			}
		}
	}
	return list;
} // MasterEditorTWL::patternMatch

//
// �t�@�C���Ƀo�C�g��̃p�^�[�����܂܂�邩�ǂ����}�b�`���O����
//
// @arg [in] �}�b�`�ΏۂƂȂ�t�@�C���|�C���^
// @arg [in] �p�^�[��
// @arg [in] �p�^�[���̒���(PATTERN_MATCH_LEN_MAX�ȉ��łȂ���΂Ȃ�Ȃ�)
//
// @ret �}�b�`�����e�L�X�g�̃I�t�Z�b�g�����X�g�ŕԂ��B
//      �Ō�܂Ń}�b�`�����ꍇ�̂ݐ��������Ƃ݂Ȃ��B
//
#define PATTERN_MATCH_LEN_MAX	(10*1024)
System::Collections::Generic::List<u32>^ MasterEditorTWL::patternMatch( FILE *fp, const u8 *pattern, const u32 patternLen )
{
	u8  text[ 2 * PATTERN_MATCH_LEN_MAX ];		// �o�b�t�@�̐؂�ڂ�T�����邽�ߑ��߂ɓǂݍ��ނ̂ōő�Ńp�^�[����2�{�����o�b�t�@���K�v�ɂȂ�
	u32 cur;
	u32 filesize;
	u32 len;
	u32 extra;
	System::Collections::Generic::List<u32> ^tmplist;
	System::Collections::Generic::List<u32> ^list = gcnew System::Collections::Generic::List<u32>;
	list->Clear();

	if( patternLen > PATTERN_MATCH_LEN_MAX )
		return nullptr;

	fseek( fp, 0, SEEK_END );
	filesize = ftell( fp );

	cur = 0;
	while( cur < filesize )
	{
		// �o�b�t�@�̐؂�ڂ𒲂ׂ������ߎ��ۂɂ�(�p�^�[���̒���-1)�������߂Ƀ��[�h����
		len   = ((filesize - cur) < PATTERN_MATCH_LEN_MAX)?(filesize - cur):PATTERN_MATCH_LEN_MAX;
		extra = (len <= PATTERN_MATCH_LEN_MAX)?0:(patternLen-1);	// �Ō�܂ł������Ƃ��ɂ͗]���Ƀ��[�h���Ă͂����Ȃ�
		fseek( fp, cur, SEEK_SET );
		u32 readlen = fread( text, 1, len + extra, fp );
		if( (len + extra) != readlen )
		{
			//System::Diagnostics::Debug::WriteLine( "actual len = " + readlen.ToString() );
			//System::Diagnostics::Debug::WriteLine( "expect len = " + (len + extra).ToString() );
			return nullptr;
		}

		// �e�L�X�g�I�[�ɓr���܂Ń}�b�`���O�����Ƃ��͎��s�Ƃ݂Ȃ�
		tmplist = MasterEditorTWL::patternMatch( text, len+extra, pattern, patternLen, false );
		if( tmplist != nullptr )
		{
			for each( u32 tmpval in tmplist )
			{
				list->Add( tmpval + cur );	// ���ۂ̃I�t�Z�b�g�̓t�@�C���I�t�Z�b�g���������l
			}
		}

		// ����SEEK�ʒu:���߂Ƀ��[�h�����͂��炷
		cur += len;
	}
	return list;
}

//
// XML�̃��[�g�m�[�h����w�肵�����O�̃^�O���������ĕԂ�
//
// @arg [in] XML�̃��[�g�m�[�h
// @arg [in] �^�O��
//
// @ret �����Ń}�b�`�����Ƃ��m�[�h��Ԃ��B�Ȃ��Ƃ��� nullptr�B
//      �������A�ŏ��Ƀ}�b�`�������̂̂ݕԂ�
//
System::Xml::XmlNode^ MasterEditorTWL::searchXmlNode( System::Xml::XmlElement ^root, System::String ^tag )
{
	System::Xml::XmlNodeList ^list = root->GetElementsByTagName( tag );
	System::Xml::XmlNode ^item = nullptr;
	if( list != nullptr )
	{
		item = list->Item(0);
	}
	return item;
}

//
// �^�O���������Ă��̃e�L�X�g���w�肵���e�L�X�g�ƈ�v���邩���ׂ�
//
// @arg [in] XML�̃��[�g�m�[�h
// @arg [in] �^�O��
// @arg [in] �l
//
// @ret ��v����Ƃ�true�B��v���Ȃ��Ƃ��A�^�O�����݂��Ȃ��Ƃ���false�B
//
System::Boolean MasterEditorTWL::isXmlEqual( System::Xml::XmlElement ^root, System::String ^tag, System::String ^val )
{
	System::Xml::XmlNode ^item = MasterEditorTWL::searchXmlNode( root, tag );
	if( (item != nullptr) && (item->FirstChild != nullptr) && (item->FirstChild->Value->Equals( val )) )
	{
		return true;
	}
	return false;
}

//
// �^�O���������Ă��̃e�L�X�g��Ԃ�
//
// @arg [in] XML�̃��[�g�m�[�h
// @arg [in] �^�O��XPath
//
// @ret �e�L�X�g�����݂���Ƃ����̃e�L�X�g��Ԃ��B���݂��Ȃ��Ƃ�nullptr�B
//
System::String^ MasterEditorTWL::getXPathText( System::Xml::XmlElement ^root, System::String ^xpath )
{
	System::Xml::XmlNode ^tmp = root->SelectSingleNode( xpath );
	if( tmp && tmp->FirstChild && tmp->FirstChild->Value )
	{
		return tmp->FirstChild->Value;
	}
	return nullptr;
}

//
// XML�Ƀ^�O��ǉ�����
//
// @arg [in] XML�h�L�������g
// @arg [in] �e�m�[�h
// @arg [in] �^�O��
// @arg [in] �e�L�X�g
//
void MasterEditorTWL::appendXmlTag( System::Xml::XmlDocument ^doc, System::Xml::XmlElement ^parent,
									System::String ^tag, System::String ^text )
{
	if( System::String::IsNullOrEmpty( text ) )
		return;

	System::Xml::XmlElement ^node = doc->CreateElement( tag );
	node->AppendChild( doc->CreateTextNode( text ) );
	parent->AppendChild( node );
}

//
// �r�b�g�����J�E���g����
//
// @arg [in] �l
//
// @ret �r�b�g��
//
u32 MasterEditorTWL::countBits( const u32 val )
{
	// ���������@:�ׂ荇�������܂育�ƂɃr�b�g�����J�E���g���Ă���
	u32 n = val;
	
	n = ((n>>1) & 0x55555555) + (n & 0x55555555);
    n = ((n>>2) & 0x33333333) + (n & 0x33333333);
    n = ((n>>4) & 0x0f0f0f0f) + (n & 0x0f0f0f0f);
    n = ((n>>8) & 0x00ff00ff) + (n & 0x00ff00ff);
    n = (n>>16) + (n & 0x0000ffff);
    return n;
}
