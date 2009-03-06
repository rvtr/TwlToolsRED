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
System::Void verifyArea( FILE *fp1, FILE *fp2, const int offset, const int size );

using namespace System;

// -------------------------------------------------------------------
// �o��SRL�̃`�F�b�N
// -------------------------------------------------------------------
System::Void checkRom( System::String ^orgSrl, System::String ^targetSrl )
{
	const char *chorg = 
		(const char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( orgSrl ).ToPointer();
	const char *chtarget = 
		(const char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( targetSrl ).ToPointer();

	// ROM�w�b�_�̓ǂݍ���
	ROM_Header rh;
	FILE       *fp = NULL;
	if( fopen_s( &fp, chtarget, "rb" ) != 0 )
	{
		throw (gcnew System::Exception("Fail to open the target SRL File."));
		return;
	}
	// 1�o�C�g��sizeof(~)�������[�h (�t���ƕԂ�l��sizeof(~)�ɂȂ�Ȃ��̂Œ���)
	(void)fseek( fp, 0, SEEK_SET );		// ROM�w�b�_��srl�̐擪����
	if( fread( (void*)&rh, 1, sizeof(ROM_Header), fp ) != sizeof(ROM_Header) )
	{
		throw (gcnew System::Exception("Fail to read the ROM Header."));
		fclose(fp);
		return;
	}
	fclose(fp);

	// �����̃`�F�b�N
	checkRomHeaderSign( &rh );

	Console::WriteLine( "--------------------------------------------------------" );
	Console::WriteLine( "{0,-15} {1,-20}", nullptr, "RomHeader" );
	Console::WriteLine( "--" );

	// ���[�W�����̕\��
	Console::WriteLine( "{0,-15} {1,-20:X04}", "Region", rh.s.card_region_bitmap );
	Console::WriteLine( "--" );

	// ���[�e�B���O�̕\��
	int i;
	for( i=0; i < PARENTAL_CONTROL_INFO_SIZE; i++ )
	{
		Console::WriteLine( "{0,-15} {1,-10:X02} {2}", 
			getOgnString(i), rh.s.parental_control_rating_info[i], getRatingString(rh.s.parental_control_rating_info[i]) );
	}
	Console::WriteLine( "--" );
	Console::WriteLine( "{0,-15} {1:X}", "Unnecessary", rh.s.unnecessary_rating_display );
	Console::WriteLine( "--------------------------------------------------------" );
	
	// �S�̈�x���t�@�C
	FILE       *fp1 = NULL;
	if( fopen_s( &fp1, chorg, "rb" ) != NULL )
	{
		throw (gcnew System::Exception("Fail to open the original SRL file."));
		return;
	}
	FILE       *fp2 = NULL;
	if( fopen_s( &fp2, chtarget, "rb" ) != NULL )
	{
		throw (gcnew System::Exception("Fail to open the target SRL file."));
		return;
	}
	// �t�@�C���T�C�Y���܂��`�F�b�N
	fseek(fp1, 0, SEEK_END);
	u32 filesize1 = ftell( fp1 );
	fseek(fp2, 0, SEEK_END);
	u32 filesize2 = ftell( fp2 );
	Console::WriteLine( "{0,-15} {1,-20} {2,-20}", nullptr, "Original File", "Target File" );
	Console::WriteLine( "{0,-15} {1,-20:X08} {2,-20:X08}", "Filesize", filesize1, filesize2 );
	Console::WriteLine( "--------------------------------------------------------" );
	if( filesize1 != filesize2 )
	{
		throw (gcnew System::Exception("Incorrect filesize"));
		return;
	}
	// �}�X�^�G�f�B�^�ŏ����������Ă��Ȃ��̈���`�F�b�N
	verifyArea( fp1, fp2, 0, 0x1b0 );				// ���[�W�����܂�
	verifyArea( fp1, fp2, 0x1b4, 0x20f - 0x1b4 );	// Unnecessary�t���O�܂�
	verifyArea( fp1, fp2, 0x210, 0x2f0 - 0x210 );	// ���[�e�B���O�܂�
	verifyArea( fp1, fp2, 0x300, 0xf80 - 0x300 );	// �����܂�
	verifyArea( fp1, fp2, 0x1000, filesize1 - 0x1000 );
	Console::WriteLine( "--------------------------------------------------------" );
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

// -------------------------------------------------------------------
// �����������ӏ��ȊO�̑S�̈���x���t�@�C�������̂�
// �w��̈���x���t�@�C����֐�������
// -------------------------------------------------------------------
#define VERIFY_AREA_BUFSIZE  (10*1024*1024)
System::Void verifyArea( FILE *fp1, FILE *fp2, const int offset, const int size )
{
    if( !fp1 || !fp2 )
    {
		throw (gcnew System::Exception("File pointer is NULL."));
        return;
    }

	Console::WriteLine( "{0,-15} {1:X08} - {2:X08}", "Verify", offset, offset+size-1 );

	cli::array<System::Byte> ^mbuf1 = gcnew cli::array<System::Byte>(VERIFY_AREA_BUFSIZE);	// ����̕K�v�Ȃ�
	pin_ptr<unsigned char> buf1 = &mbuf1[0];
	cli::array<System::Byte> ^mbuf2 = gcnew cli::array<System::Byte>(VERIFY_AREA_BUFSIZE);
	pin_ptr<unsigned char> buf2 = &mbuf2[0];

	fseek( fp1, offset, SEEK_SET );
    fseek( fp2, offset, SEEK_SET );

    // �o�b�t�@�����傫���ꍇ�͍א؂�Ƀ��[�h���ăx���t�@�C����
	int rest = size;
	while( rest > 0 )
    {
        int len = (rest > VERIFY_AREA_BUFSIZE)?(VERIFY_AREA_BUFSIZE):(rest);
        if( fread(buf1, 1, len, fp1) != len )
        {
			throw (gcnew System::Exception("In Verify, fail to fread fp1"));
            return;
        }
        if( fread(buf2, 1, len, fp2) != len )
        {
			throw (gcnew System::Exception("In Verify, fail to fread fp2"));
            return;
        }
        if( memcmp(buf1, buf2, len) != 0 )
        {
			throw (gcnew System::Exception("In Verify, incorrect area."));
        }
        rest = rest - len;
    }
	return;
}
