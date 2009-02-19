#include "stdafx.h"
#include "check.h"
#include <apptype.h>
#include <keys.h>
#include <common.h>
#include <utility.h>
#include <twl/types.h>
#include <twl/os/common/format_rom.h>
#include <twl/os/common/ownerInfoEx.h>
#include <acsign/include/acsign.h>
#include <cstring>
#include <cstdio>

System::Void checkRomHeaderSign( ROM_Header *prh );


// -------------------------------------------------------------------
// �o��SRL�̃`�F�b�N
// -------------------------------------------------------------------
System::Void checkRom( FilenameItem ^fItem, System::String ^srlpath )
{
	const char *chpath = 
		(const char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( srlpath ).ToPointer();

	// ROM�w�b�_�̓ǂݍ���
	ROM_Header rh;
	FILE       *fp = NULL;
	if( fopen_s( &fp, chpath, "rb" ) != NULL )
	{
		throw (gcnew System::Exception("Fail to Open SRL File."));
		return;
	}
	// 1�o�C�g��sizeof(~)�������[�h (�t���ƕԂ�l��sizeof(~)�ɂȂ�Ȃ��̂Œ���)
	(void)fseek( fp, 0, SEEK_SET );		// ROM�w�b�_��srl�̐擪����
	if( fread( (void*)&rh, 1, sizeof(ROM_Header), fp ) != sizeof(ROM_Header) )
	{
		throw (gcnew System::Exception("Fail to Read ROM Header."));
		fclose(fp);
		return;
	}
	fclose(fp);

	// �����̃`�F�b�N
	checkRomHeaderSign( &rh );

	DebugPrint( "--------------------------------------------------------" );
	DebugPrint( "{0,-10} {1,-20}", nullptr, "RomHeader" );
	DebugPrint( "--" );

	// ���[�W�����̃`�F�b�N
	u32 region = fItem->getRegionBitmap();	// �t�@�C�����ɑΉ�����^�l���擾
	DebugPrint( "{0,-10} {1,-20:X04}", "Region", rh.s.card_region_bitmap );
	DebugPrint( "--" );
	if( rh.s.card_region_bitmap != region )
	{
		throw (gcnew System::Exception("Illegal Region in ROM Header."));
		return;
	}

	// ���[�W�����Ɋ܂܂��c�̂̃��[�e�B���Oenable�t���O�������Ă��邩�`�F�b�N
	System::Collections::Generic::List<int> ^ognlist = MasterEditorTWL::getOgnListInRegion( region );
	for each ( int ogn in ognlist )
	{
		if( (rh.s.parental_control_rating_info[ogn] & OS_TWL_PCTL_OGNINFO_ENABLE_MASK) == 0 )
		{
			throw (gcnew System::Exception("Rating Ogn " + ogn.ToString() + " is not Enabled."));
			return;
		}
	}

	// �ݒ肵�����[�e�B���O�����������ǂ������`�F�b�N
	int ogn = fItem->getOgnNumber();			// �t�@�C�����ɑΉ�����^�l���擾
	u8  rating = fItem->getRatingValue();
	if( rh.s.parental_control_rating_info[ ogn ] != rating )
	{
		throw (gcnew System::Exception("Mismatch Rating Ogn " + ogn.ToString() + ". "
										+ "filename = " + rating.ToString("2X")
										+ "srl = " + rh.s.parental_control_rating_info[ ogn ].ToString("2X") + "."));
		return;
	}

	// �\��
	System::Collections::Generic::List<int> ^alllist = gcnew System::Collections::Generic::List<int>();
	alllist->Clear();
	alllist->Add( OS_TWL_PCTL_OGN_CERO );
	alllist->Add( OS_TWL_PCTL_OGN_ESRB );
	alllist->Add( OS_TWL_PCTL_OGN_USK );
	alllist->Add( OS_TWL_PCTL_OGN_PEGI_GEN );
	alllist->Add( OS_TWL_PCTL_OGN_PEGI_PRT );
	alllist->Add( OS_TWL_PCTL_OGN_PEGI_BBFC );
	alllist->Add( OS_TWL_PCTL_OGN_OFLC );
	//alllist->Add( OS_TWL_PCTL_OGN_GRB );
	for each ( int ogn in alllist )
	{
		DebugPrint( "{0,-10} {1,-20:X02}", fItem->getOgnString(ogn), rh.s.parental_control_rating_info[ogn] );
	}

	// ���[�W�����Ɋ܂܂�Ȃ��c�̂̃��[�e�B���O���N���A����Ă��邩�`�F�b�N
	int i;
	for( i=0; i < PARENTAL_CONTROL_INFO_SIZE; i++ )
	{
		if( (ognlist->IndexOf(i) < 0) && (rh.s.parental_control_rating_info[i] != 0) )
		{
			throw (gcnew System::Exception("Rating Ogn " + i.ToString() + " is not Cleared in ROM Header."));
			return;
		}
	}
	DebugPrint( "--------------------------------------------------------" );
}

// -------------------------------------------------------------------
// ROM�w�b�_�̏������O��
// -------------------------------------------------------------------
System::Void checkRomHeaderSign( ROM_Header *prh )
{
	u8     original[ RSA_KEY_LENGTH ];	// �����O������̃f�[�^�i�[��
	s32    pos = 0;						// �u���b�N�̐擪�A�h���X
	u8     digest[ DIGEST_SIZE_SHA1 ];	// ROM�w�b�_�̃_�C�W�F�X�g
	u8    *publicKey = (u8*)MasterEditorTWL::g_devPubKey_DER;

	// <�f�[�^�̗���>
	// (1) ���J���ŕ�����������(�u���b�N)�����[�J���ϐ�(original)�Ɋi�[
	// (2) �u���b�N����]���ȕ�������菜���Ĉ���(pDst)�ɃR�s�[

	u8  *idL = prh->s.titleID_Lo;
	u32  idH = prh->s.titleID_Hi;

	if( (idL[3]=='H') && (idL[2]=='N') && (idL[1]=='A') )	// �����`���A�v�����ǂ�����TitleID_Lo�̒l�Ō���
    {
		publicKey  = (u8*)MasterEditorTWL::g_devPubKey_DER_launcher;
    }
    else if( idH & TITLE_ID_HI_SECURE_FLAG_MASK )				// �e�r�b�g�͔r���I�Ƃ͌���Ȃ��̂�else if�ɂ͂Ȃ�Ȃ�
    {
		publicKey  = (u8*)MasterEditorTWL::g_devPubKey_DER_secure;
    }
    else if( (idH & TITLE_ID_HI_APP_TYPE_MASK) == 1 )
    {
		publicKey  = (u8*)MasterEditorTWL::g_devPubKey_DER_system;
    }
    else if( (idH & TITLE_ID_HI_APP_TYPE_MASK) == 0 )
    {
		publicKey  = (u8*)MasterEditorTWL::g_devPubKey_DER;
    }

	// �����̉��� = ���J���ŕ���
	if( !ACSign_Decrypto( original, publicKey, prh->signature, RSA_KEY_LENGTH ) )
	{
		throw (gcnew System::Exception("Fail to decrypt sign."));
		return;
	}
	// �����O�f�[�^�𕜍���u���b�N����Q�b�g
	for( pos=0; pos < (RSA_KEY_LENGTH-2); pos++ )   // �{���u���b�N�̐擪��0x00�����������̓��������ɂ���ď�����d�l
	{
		// �Í��u���b�N�`�� = 0x00, BlockType, Padding, 0x00, ���f�[�^
		if( original[pos] == 0x00 )                               // ���f�[�^�̒��O��0x00���T�[�`
		{
			break;
		}
	}
	// �x���t�@�C
	// ROM�w�b�_�̃_�C�W�F�X�g���Z�o(�擪����ؖ����̈�̒��O�܂ł��Ώ�)
	ROM_Header tmprh;		// �}�l�[�W�q�[�v��ɂ���ꍇ���A�h���X���擾�ł��Ȃ��̂ŃT�C�Y�v�Z�p��ROM�w�b�_��p��
	ACSign_DigestUnit( digest,	prh, (u32)&(tmprh.certificate) - (u32)&(tmprh) );
	if( memcmp( &(original[pos+1]), digest, DIGEST_SIZE_SHA1 ) != 0 )
	{
		throw (gcnew System::Exception("Fail to verify sign."));
		return;
	}
	return;
}
