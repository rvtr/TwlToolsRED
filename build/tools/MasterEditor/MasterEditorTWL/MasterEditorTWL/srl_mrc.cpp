// srl.h �̃N���X����

#include "stdafx.h"
#include <apptype.h>
#include "common.h"
#include "srl.h"
#include "utility.h"
#include "keys.h"
#include "crc_whole.h"
#include <acsign/include/acsign.h>
#include <format_rom_private.h>
#include <cstring>
#include <cstdio>

using namespace MasterEditorTWL;

extern const u8 gBannerFont[];
extern u16 calcCRC( const u16 start, const u8 *data, const int allsize );

// ****************************************************************
// RCSrl �N���X�� MRC �@�\��ʃt�@�C���ɓƗ�������
// ****************************************************************

// -------------------------------------------------------------------
// MRC�{��
// -------------------------------------------------------------------
ECSrlResult RCSrl::mrc( FILE *fp )
{
#ifdef METWL_WHETHER_MRC
	ECSrlResult result;
	result = this->mrcNTR( fp );
	if( result != ECSrlResult::NOERROR )
	{
		return result;
	}
	result = this->mrcTWL( fp );
	if( result != ECSrlResult::NOERROR )
	{
		return result;
	}
	this->hErrorList->AddRange( this->hParentalErrorList );	// �y�A�����^���R���g���[���̃��X�g�͕ʂɂ����Ă���̂Œǉ�
	this->hWarnList->AddRange( this->hParentalWarnList );
#endif
	return ECSrlResult::NOERROR;
}

// -------------------------------------------------------------------
// NTR�݊�MRC
// -------------------------------------------------------------------
ECSrlResult RCSrl::mrcNTR( FILE *fp )
{
	System::Int32  i;
	System::Boolean result;

	// �^�C�g�����̕����R�[�h�`�F�b�N
	result = true;
	for( i=0; i < TITLE_NAME_MAX; i++ )
	{
		char c = this->pRomHeader->s.title_name[i];
		if( ((c < 0x20) || (0x5f < c)) && (c != 0x00) )
		{
			result = false;
		}
	}
	if( !result )
	{
		this->hErrorList->Add( this->makeMrcError("TitleNameAscii") );
	}

	result = true;
	for( i=0; i < TITLE_NAME_MAX; i++ )
	{
		char c = this->pRomHeader->s.title_name[TITLE_NAME_MAX - i - 1];	// ��������T��
		if( ((0x21 <= c ) && (c <= 0x5f)) || (c == 0x00) )					// 00h��20h�ȊO�̎g�p�\�ȕ������o�Ă���܂łɃX�y�[�X�����݂�����_��
		{
			break;
		}
		else if( c == 0x20 )
		{
			result = false;
		}
	}
	if( !result )
	{
		this->hErrorList->Add( this->makeMrcError("TitleNameSuffix") );
	}

	result = true;
	for( i=1; i < TITLE_NAME_MAX; i++ )
	{
		char prev = this->pRomHeader->s.title_name[i-1];
		char curr = this->pRomHeader->s.title_name[i];
		if( (prev == 0x00) && (0x21 <= curr) && (curr <= 0x5f) )	// �r����00h������ƃ_��
		{
			result = false;
			break;
		}
	}
	if( !result )
	{
		this->hErrorList->Add( this->makeMrcError("TitleNameSpace") );
	}

	// �Q�[���R�[�h�̕����R�[�h�`�F�b�N
	result = true;
	for( i=0; i < GAME_CODE_MAX; i++ )
	{
		char c = this->pRomHeader->s.game_code[i];
		if( (c < 0x20) || (0x5f < c) )		// 00h�������Ȃ�
		{
			result = false;
		}
	}
	if( !result )
	{
		this->hErrorList->Add( this->makeMrcError("InitialCodeAscii") );
	}
	if( memcmp( this->pRomHeader->s.game_code, "NTRJ", GAME_CODE_MAX ) == 0 )
	{
		this->hErrorList->Add( this->makeMrcError("InitialCodeSDKDefault") );
	}

	// ���[�J�R�[�h�̕����R�[�h�`�F�b�N
	result = true;
	for( i=0; i < MAKER_CODE_MAX; i++ )
	{
		char c = this->pRomHeader->s.maker_code[i];
		if( (c < 0x20) || (0x5f < c) )
		{
			result = false;
		}
	}
	if( !result )
	{
		this->hErrorList->Add( this->makeMrcError("MakerCodeAscii") );
	}

	// �f�o�C�X�^�C�v�͕s�g�p�Ȃ̂�00h�Œ�
	if( this->pRomHeader->s.rom_type != 0x00 )
	{
		this->hErrorList->Add( this->makeMrcError("DeviceType") );
	}

	// ���}�X�^�[�o�[�W������16�i2���̂Ƃ��G���[
	u8 romver = this->pRomHeader->s.rom_version;
	if( ((romver < 0x00) || (0x0f < romver)) && (romver != 0xE0) )
	{
		this->hErrorList->Add( this->makeMrcError("RemasterVersion") );
	}

	// �K���ݒ肳��Ă��Ȃ���΂Ȃ�Ȃ��p�����[�^
	if( this->pRomHeader->s.banner_offset == 0 )
	{
		this->hErrorList->Add( this->makeMrcError("BannerOffset") );
	}
	if( this->pRomHeader->s.rom_valid_size == 0 )
	{
		this->hErrorList->Add( this->makeMrcError("RomValidSize") );
	}

	// CRC
	u16  crc;
	// �Z�L���A�̈�
	if( !this->IsMediaNand )	// �J�[�h�A�v���̂Ƃ��̂�
	{
		// �Z�L���A�̈��ROM�w�b�_�O�Ȃ̂Ńt�@�C������ʓr�ǂݏo���ă`�F�b�N����
		u8     *secures;
		s32     secure_size = SECURE_AREA_END - this->pRomHeader->s.main_rom_offset;
		if (secure_size > SECURE_AREA_SIZE)
		{
			secure_size = SECURE_AREA_SIZE;
		}
		if (secure_size <= 0)
		{
			this->hErrorList->Add( this->makeMrcError("SecureCRCAddress") );
		}
		secures = new u8[secure_size];      // never return if not allocated
		fseek( fp, (u32)this->pRomHeader->s.main_rom_offset, SEEK_SET );
		if( secure_size != fread( secures, 1, secure_size, fp ) )
		{
			delete []secures;
			return ECSrlResult::ERROR_FILE_READ;
		}
		crc = calcCRC(CRC16_INIT_VALUE, (u8 *)secures, secure_size);
		delete []secures;
		if( crc != this->pRomHeader->s.secure_area_crc16 )
		{
			this->hErrorList->Add( this->makeMrcError("SecureCRCVerify") );
		}
	}

	// ���S�̈�
	crc = 0xcf56;
	if( crc != this->pRomHeader->s.nintendo_logo_crc16 )
	{
		this->hErrorList->Add( this->makeMrcError("LogoCRC") );
	}

	// �w�b�_CRC
	crc = calcCRC( CRC16_INIT_VALUE, (u8*)this->pRomHeader, CALC_CRC16_SIZE );
	if( crc != this->pRomHeader->s.header_crc16 )
	{
		this->hErrorList->Add( this->makeMrcError("HeaderCRC") );
	}

	// ROM�w�b�_�ȊO�̗̈�̃`�F�b�N (�t�@�C������K�X���[�h����)
	
	// �V�X�e���R�[�����C�u����
	u8  syscall[32];
	u32 offset = this->pRomHeader->s.main_rom_offset;
	fseek( fp, offset, SEEK_SET );
	if( 32 != fread( syscall, 1, 32, fp ) )
	{
		return ECSrlResult::ERROR_FILE_READ;
	}
	for( i=0; i < 32; i++ )
	{
		if( syscall[i] != 0x00 )
			break;
	}
	if( i == 32 )	// �S��0
	{
		this->hErrorList->Add( this->makeMrcError("Syscall") );
	}
	return ECSrlResult::NOERROR;
} // mrcNTR()

// -------------------------------------------------------------------
// TWL��pMRC
// -------------------------------------------------------------------
ECSrlResult RCSrl::mrcTWL( FILE *fp )
{
	// ROM�w�b�_�̃`�F�b�N (NTR�݊��̈�)

#ifdef METWL_WHETHER_PLATFORM_CHECK
	// �v���b�g�t�H�[���̃`�F�b�N
	if( (this->pRomHeader->s.platform_code != PLATFORM_CODE_TWL_HYBLID) && 
		(this->pRomHeader->s.platform_code != PLATFORM_CODE_TWL_LIMITED) &&
		(this->pRomHeader->s.enable_signature == 0) )
	{
		return ECSrlResult::ERROR_PLATFORM;
	}
#endif

	// NAND�A�v����HYBRID�ƂȂ�̂̓N���[���u�[�g�̂Ƃ��̂�
	if( this->IsMediaNand )
	{
		if( !this->HasDSDLPlaySign && (this->pRomHeader->s.platform_code == PLATFORM_CODE_TWL_HYBLID) )
		{
			this->hErrorList->Add( this->makeMrcError("NandHybrid") );
		}
		if( this->HasDSDLPlaySign && (this->pRomHeader->s.platform_code == PLATFORM_CODE_TWL_LIMITED) )
		{
			this->hErrorList->Add( this->makeMrcError("NandLimited") );
		}
	}

	// ���J���p�Í��t���O�ƃN���[���u�[�g�̑g�ݍ��킹�̓}�X�^�����O�Ŗ�����������
	if( this->IsOldDevEncrypt && this->HasDSDLPlaySign )
	{
		this->hErrorList->Add( this->makeMrcError("OldDevFlag") );
	}

	// �f�o�C�X�e�ʂƎ��t�@�C���T�C�Y
	fseek( fp, 0, SEEK_END );
	u32  filesize = ftell(fp);	// ���t�@�C���T�C�Y
	u32  romsize = 1 << (this->pRomHeader->s.rom_size);	// ROM�e��(�P��Mbit)
	if( !this->IsMediaNand )		// �J�[�h�A�v���̂Ƃ��݂̂̃`�F�b�N
	{
		u32 filesizeMb = (filesize / (1024*1024)) * 8;	// �P�ʂ�Mbit�ɒ���
		if( romsize < filesizeMb )
		{
			this->hErrorList->Add( this->makeMrcError("CapacityCardLess") );
		}
		else if( filesizeMb < romsize )
		{
			this->hWarnList->Add( this->makeMrcError("CapacityCardWaste") );
		}
		if( (romsize < METWL_ROMSIZE_MIN) || (METWL_ROMSIZE_MAX < romsize) )
		{
			this->hErrorList->Add( this->makeMrcError("CapacityCardRange") );
		}
		if( (filesizeMb < 1) || (MasterEditorTWL::countBits(filesizeMb) != 1) )
		{
			this->hWarnList->Add( this->makeMrcError("FilesizeFraction") );
		}
	} //if( *(this->hIsNAND) == false )
	else	// NAND�A�v���̂Ƃ��݂̂̃`�F�b�N
	{
		if( (romsize < METWL_ROMSIZE_MIN_NAND) || (METWL_ROMSIZE_MAX_NAND < romsize) )
		{
			this->hErrorList->Add( this->makeMrcError("CapacityNandRangel") );
		}
		//u32  allsize = filesize + this->pRomHeader->s.public_save_data_size + this->pRomHeader->s.private_save_data_size;
		u32  allsize = this->hNandUsedSize->NandUsedSize;	// TMD��T�u�o�i�[�̃T�C�Y���܂߂�
		if( allsize > METWL_ALLSIZE_MAX_NAND )
		{
			this->hErrorList->Add( this->makeMrcError("UsedNandSizeLimit") );
		}
		if( (allsize > METWL_ALLSIZE_MAX_NAND_LIC) && this->IsAppUser )		// ���[�U�A�v���̂Ƃ��̂�
		{
			this->hErrorList->Add( this->makeMrcError("UsedNandSizeExceed") );
		}
	}

	// �p�f�B���O
	if( !this->IsMediaNand )
	{
		// 1Gbit�ȏ�̂Ƃ��̍ŏI�̈悪�Œ�l���ǂ���
		this->mrcPadding( fp );

		// �Z�O�����g3��CRC
		u16  crcseg3;
		BOOL crcret = getSeg3CRCInFp( fp, &crcseg3 );
		if( !crcret || (crcseg3 != METWL_SEG3_CRC) )
		{
			this->hErrorList->Add( this->makeMrcError("Seg3CRC") );
		}
	}

	// ROM�I�t�Z�b�g
	if( !this->IsMediaNand )
	{
		u16  NA = this->pRomHeader->s.twl_card_normal_area_rom_offset & 0x7fffUL;
		u16  KA = this->pRomHeader->s.twl_card_keytable_area_rom_offset;
		if( (NA == 0) || (KA == 0) || (NA > KA) )
		{
			this->hErrorList->Add( this->makeMrcError("TwlRomOffset") );
		}
	}

	// �f�o�b�K����֎~�t���O�̓��[�U�A�v���ł͗����Ă��Ă͂����Ȃ�
	// �V�X�e���A�v���ł̓f�o�b�K�ŉ�͂���Ȃ��悤�ɒʏ�ł͗����Ă��Ȃ���΂Ȃ�Ȃ�
	if( !this->IsAppUser )
	{
		if( this->pRomHeader->s.disable_debug == 0 )
		{
			this->hWarnList->Add( this->makeMrcError("DisableDebugSystem") );
		}
	}
	else
	{
		if( this->pRomHeader->s.disable_debug != 0 )
		{
			this->hErrorList->Add( this->makeMrcError("DisableDebugUser") );
		}
	}

	// �m�[�}���W�����v
	{
		u8 okbits = 0x01 | 0x02 | 0x40 | 0x80;
		u8 *p = (u8*)&(this->pRomHeader->s);
		if( p[0x1d] & ~okbits )
		{
			this->hErrorList->Add( this->makeMrcError("ExtraRegion") );
		}
		if( this->IsAppUser )
		{
			if( (this->pRomHeader->s.permit_landing_normal_jump != 0) && 
				!this->hMrcExternalCheckItems->IsPermitNormalJump )			// �ݒ�t�@�C���ŃA�N�Z�X������Ă��Ȃ��Ƃ��Ƀ`�F�b�N
			{
				this->hErrorList->Add( this->makeMrcError("NormalJump") );
			}
		}
	}

	// NAND�A�v���̂Ƃ��ɂ�ROM�̎��(MROM/1TROM)���ݒ肳��Ă��Ȃ��Ă��悢
	if( (this->pRomHeader->s.warning_no_spec_rom_speed != 0) && !this->IsMediaNand )
	{
		this->hErrorList->Add( this->makeMrcError("RomSpeedType") );
	}
	// 1TROM�̂݋���(ROM�̎�ނ��ݒ肳��Ȃ��Ƃ��f�t�H���g��1TROM�ƂȂ�)
	if( (this->pRomHeader->s.game_cmd_param & CARD_LATENCY_MASK) != CARD_1TROM_GAME_LATENCY )
	{
		this->hErrorList->Add( this->makeMrcError("RomControl") );
	}

	// AES���������Ă��Ȃ��ƃG���[
	if( !this->IsMediaNand )
	{
		if( (this->pRomHeader->s.enable_aes == 0) || (this->pRomHeader->s.aes_target_size == 0) )
		{
			this->hErrorList->Add( this->makeMrcError("AES") );
		}
	}
	else		// NAND�A�v��
	{
		if( (this->pRomHeader->s.enable_aes == 0) || (this->pRomHeader->s.aes_target_size == 0) || (this->pRomHeader->s.aes_target2_size == 0) )
		{
			this->hErrorList->Add( this->makeMrcError("AES") );
		}
	}

	// �A�v����ʂ̃`�F�b�N
	this->mrcAppType(fp);

	// �e��f�o�C�X�ւ̃A�N�Z�X�t���O�̃`�F�b�N
	this->mrcAccessControl(fp);
	this->mrcShared2(fp);

	// NTR�z���C�g���X�g����
	if( this->IsWL )
	{
		this->hErrorList->Add( this->makeMrcError("WhiteList") );
	}

	// �ʐM�A�C�R���\��
	if( this->IsWiFiIcon && this->IsWirelessIcon )
	{
		this->hErrorList->Add( this->makeMrcError("ConnectIcon") );
	}

	// TWL Limited static �̈ʒu�� KeyTable �̈�����O�ɂ����Ă͂����Ȃ�
	if( !this->IsMediaNand )
	{
		u32 ideal  = (this->pRomHeader->s.twl_card_keytable_area_rom_offset * 0x80000) + 0x3000;	// TWL KeyTable�̈�̊J�n + KeyTable�T�C�Y
		u32 actual = this->pRomHeader->s.main_ltd_rom_offset;
		if( ideal > actual )
		{
			this->hErrorList->Add( this->makeMrcError("LimitedStatic") );
		}
	}

	// �����`���[�ȊO��SCFG�ɃA�N�Z�X���Ă͂Ȃ�Ȃ�
	if( !this->IsAppLauncher && this->IsSCFGAccess )
	{
		this->hWarnList->Add( this->makeMrcError("SCFGAccess") );
	}

	// TitleID Lo �̓Q�[���R�[�h�̋t��
	if( (this->pRomHeader->s.titleID_Lo[0] != this->pRomHeader->s.game_code[3]) ||
		(this->pRomHeader->s.titleID_Lo[1] != this->pRomHeader->s.game_code[2]) ||
		(this->pRomHeader->s.titleID_Lo[2] != this->pRomHeader->s.game_code[1]) ||
		(this->pRomHeader->s.titleID_Lo[3] != this->pRomHeader->s.game_code[0]) )
	{
		this->hWarnList->Add( this->makeMrcError("TitleID") );
	}

	// �J�[�h�A�v����Public/Private�Z�[�u�f�[�^��ݒ肵�Ă͂����Ȃ�
	if( !this->IsMediaNand )
	{
		if( (this->PublicSize > 0) || (this->PrivateSize) )
		{
			this->hErrorList->Add( this->makeMrcError("SaveData") );
		}
	}

	// �o�i�[�̕����R�[�h
	this->mrcBanner( fp );

	// �\��̈�ɗ]�v�ȃf�[�^��������Ă��Ȃ���
	this->mrcReservedArea(fp);

	// FINALROM�r���h�łȂ��Ƃ�SD�ɃA�N�Z�X�ł��Ă��܂��̂ŃG���[
	for each( RCLicense ^lic in this->hLicenseList )
	{
		if( lic->Publisher->Equals( "NINTENDO" ) && lic->Name->Equals( "DEBUG" ) )
		{
			this->hErrorList->Add( this->makeMrcError("DebugBuild") );
		}
	}

	// �ǉ��`�F�b�N
	this->mrcSDKVersion(fp);

	return ECSrlResult::NOERROR;
} // mrcTWL()

// -------------------------------------------------------------------
// �A�v����ʂ̃`�F�b�N
// -------------------------------------------------------------------
void RCSrl::mrcAppType(FILE *fp)
{
	u32  idH;
	u8   idL[4];
	int  apptype;
	const int appUser     = 0;
	const int appSystem   = 1;
	const int appSecure   = 2;
	const int appLauncher = 3;
	System::String ^appstr = gcnew System::String("");

	idH = this->pRomHeader->s.titleID_Hi;
	memcpy( idL, &(this->pRomHeader->s.titleID_Lo[0]), 4 );

	if( (idL[3]=='H') && (idL[2]=='N') && (idL[1]=='A') )   // �����`���A�v�����ǂ�����TitleID_Lo�̒l�Ō���
	{
		apptype = appLauncher;
		appstr  = "Launcher";
	}
	else if( idH & TITLE_ID_HI_SECURE_FLAG_MASK )           // �����Ă���Z�L���A�A�v��
	{
		apptype = appSecure;
		appstr  = "Secure";
	}
	else if( (idH & TITLE_ID_HI_APP_TYPE_MASK) == 1 )       // �����Ă���V�X�e���A�v��
	{
		apptype = appSystem;
		appstr  = "System";
	}
	else if( (idH & TITLE_ID_HI_APP_TYPE_MASK) == 0 )       // �c��̓��[�U�A�v��
	{
		apptype = appUser;
		appstr  = "User";
	}
	else
	{
		apptype = -1;
	}

	if( apptype < 0 )
	{
		this->hWarnList->Add( this->makeMrcError("IllegalAppType") );
	}

	System::Boolean bApp = false;
#ifdef METWL_VER_APPTYPE_LAUNCHER
	if( apptype == appLauncher )
	{
		bApp = true;
	}
	else
#endif //#ifdef METWL_VER_APPTYPE_LAUNCHER
#ifdef METWL_VER_APPTYPE_SECURE
	if( apptype == appSecure )
	{
		bApp = true;
	}
	else
#endif //#ifdef METWL_VER_APPTYPE_SECURE
#ifdef METWL_VER_APPTYPE_SYSTEM
	if( apptype == appSystem )
	{
		bApp = true;
	}
	else
#endif //#ifdef METWL_VER_APPTYPE_SYSTEM
#ifdef METWL_VER_APPTYPE_USER
	if( apptype == appUser )
	{
		bApp = true;
	}
#endif //#ifdef METWL_VER_APPTYPE_USER

	if( !bApp )
	{
		this->hWarnList->Add( this->makeMrcError("DifferentAppType", appstr) );
	}
} //RCSrl::mrcAppType()

// -------------------------------------------------------------------
// �A�N�Z�X�R���g���[�����̃`�F�b�N
// -------------------------------------------------------------------
void RCSrl::mrcAccessControl(FILE *fp)
{
	if( !this->IsAppUser )
	{
		if( (this->pRomHeader->s.access_control.game_card_on != 0) &&
			(this->pRomHeader->s.access_control.game_card_nitro_mode != 0) )
		{
			this->hErrorList->Add( this->makeMrcError("CardAccess") );
		}
	}
	else	// ���[�U�A�v��
	{
		if( this->pRomHeader->s.access_control.sd_card_access != 0 )
		{
			this->hErrorList->Add( this->makeMrcError("SDAccess") );
		}
		if( !this->IsMediaNand )	// �J�[�h�A�v���̂Ƃ��̂�
		{
			this->hErrorList->Add( this->makeMrcError("NandAccess") );
		}
		u32 okbits;
		if( !this->IsMediaNand )
		{
			okbits = 0x00000008 | 0x00000010 | 0x00000040;	// NAND | SD | Shared2�t�@�C�� (���ꂼ��ʂŃ`�F�b�N���邽�߂����ł̓`�F�b�N���Ȃ�)
		}
		else
		{
			okbits = 0x00000008 | 0x00000010 | 0x00000040 | 0x00000400;		// NAND�A�v���̂Ƃ���JpegSign�t���O�͋������
		}
		u32 *p = (u32*)&(this->pRomHeader->s);
		if( p[ 0x1b4 / 4 ] & ~okbits )
		{
			this->hErrorList->Add( this->makeMrcError("AccessDefault") );
		}
	}
} //RCSrl::mrcAccessControl()

// -------------------------------------------------------------------
// �p�f�B���O�̃`�F�b�N
// -------------------------------------------------------------------
void RCSrl::mrcPadding(FILE *fp)
{
	u32  romsize = 1 << (this->pRomHeader->s.rom_size);	// ROM�e��
	u32  padsize = 0;	// �P�ʂ�Byte
	System::String ^padstr = gcnew System::String("");
	System::String ^romstr  = gcnew System::String("");

	switch( romsize )
	{
		case 1024:
			padsize = 0x00280000;	// 20Mbit = 2.5MB
			padstr  = "20Mbit";
			romstr  = "1Gbit";
		break;
		case 2048:
			padsize = 0x00500000;	// 40Mbit = 5MB
			padstr  = "40Mbit";
			romstr  = "2Gbit";
		break;
		case 4096:
			padsize = 0x00A60000;	// 83Mbit = 10.375MB
			padstr  = "83Mbit";
			romstr  = "4Gbit";
		break;
		default:
			padsize = 0;
		break;
	}
	if( padsize == 0 )
	{
		return;
	}

	fseek( fp, 0, SEEK_END );
	u32  filesize = ftell(fp);	// ���t�@�C���T�C�Y(�P��Mbit)
	u32  offset = ((romsize / 8) * 1024 * 1024) - padsize;
	if( (offset + padsize) > filesize )
	{
		this->hErrorList->Add( this->makeMrcError("PaddingRead", padstr) );
		return;
	}

	u8 *buf = new u8[ padsize ];
	fseek( fp, offset, SEEK_SET );
	if( padsize != fread( buf, 1, padsize, fp ) )
	{
		this->hErrorList->Add( this->makeMrcError("PaddingRead", padstr) );
		delete []buf;
		return;
	}

	System::Boolean bResult = true;
	u32 i;
	for( i=0; i < padsize; i++ )
	{
		if( buf[i] != 0xff )
		{
			bResult = false;
		}
	}
	if( !bResult )
	{
		this->hErrorList->Add( this->makeMrcError("PaddingValue", padstr, romstr) );
	}
	delete []buf;
} //RCSrl::mrcPadding

// -------------------------------------------------------------------
// �\��̈�
// -------------------------------------------------------------------
void RCSrl::mrcReservedArea(FILE *fp)
{
	// �\��̈�̏��̓��X�g�Ɋi�[����Ă���
	for each( MasterEditorTWL::VCReservedArea area in this->hMrcExternalCheckItems->hReservedAreaList )
	{
		System::UInt32 begin = area.Begin;
		System::UInt32 end   = area.End;
		System::UInt32 size  = end - begin + 1;
		System::UInt32 i;
		System::Boolean bReserved = true;
		for( i=0; i < size; i++ )
		{
			u8 *p = (u8*)this->pRomHeader;
			if( p[ begin + i ] != 0 )
			{
				bReserved = false;
				break;
			}
		}
		if( !bReserved )
		{
			this->hErrorList->Add( this->makeMrcError(begin, end, false, true, "ReservedArea") );
		}
	} //for each
}

// -------------------------------------------------------------------
// Shared2�t�@�C��
// -------------------------------------------------------------------
void RCSrl::mrcShared2(FILE *fp)
{
	// ROM�w�b�_���̃A�N�Z�X���r�b�g�������Ă��Ȃ��Ƃ�
	// Shared2�t�@�C���T�C�Y����`����Ă��Ȃ����Ƃ��m�F
	if( !this->IsShared2 )
	{
		if( (this->hShared2SizeArray[0] != 0) || (this->hShared2SizeArray[1] != 0) || (this->hShared2SizeArray[2] != 0) || 
			(this->hShared2SizeArray[3] != 0) || (this->hShared2SizeArray[4] != 0) || (this->hShared2SizeArray[5] != 0) )
		{
			this->hErrorList->Add( this->makeMrcError("Shared2Disable") );
		}
		return;
	}

	// �A�N�Z�X���r�b�g�������Ă���Ƃ�
	// �t�@�C���T�C�Y�̂����ꂩ���ݒ肳��Ă��邩���܂��m�F����
	if( (this->hShared2SizeArray[0] == 0) && (this->hShared2SizeArray[1] == 0) && (this->hShared2SizeArray[2] == 0) && 
		(this->hShared2SizeArray[3] == 0) && (this->hShared2SizeArray[4] == 0) && (this->hShared2SizeArray[5] == 0) )
	{
		this->hErrorList->Add( this->makeMrcError("Shared2Enable") );
		return;
	}

	// �A�N�Z�X���r�b�g�������Ă���Ƃ�
	// �t�@�C�����ƂɃA�N�Z�X������Ă��邩�ǂ����ƃt�@�C���T�C�Y�̒l���m�F
	int i;
	for( i=0; i < METWL_NUMOF_SHARED2FILES; i++ )
	{
		if( this->hShared2SizeArray[i] > 0 )	// 0�ȊO�̒l���ݒ肳��Ă��� = ���̃t�@�C�����g�p������ ���Ӗ�����
		{
			if( !this->hMrcExternalCheckItems->hIsPermitShared2Array[i] )
			{
				// ��ʌ��J����Ă��Ȃ��̂Ƀt�@�C�����g�p����ꍇ���l�����ăV�X�e���A�v���̂Ƃ��̃��b�Z�[�W��ύX����
				if( !this->IsAppUser )
				{
					this->hWarnList->Add( this->makeMrcError("Shared2AccessSystem", i.ToString()) );
				}
				else
				{
					this->hErrorList->Add( this->makeMrcError("Shared2AccessUser", i.ToString()) );
				}
			}
			if( this->hShared2SizeArray[i] != this->hMrcExternalCheckItems->hShared2SizeArray[i] )
			{
				if( !this->IsAppUser )
				{
					this->hWarnList->Add( this->makeMrcError("Shared2SizeSystem", 
										                     i.ToString(), MasterEditorTWL::transSizeToString(this->hShared2SizeArray[i])) );
				}
				{
					this->hWarnList->Add( this->makeMrcError("Shared2SizeUser", 
										                     i.ToString(), MasterEditorTWL::transSizeToString(this->hShared2SizeArray[i])) );
				}
			}
		} //if( this->hShared2SizeArray[i] > 0 )
	} //for
} //RCSrl::mrcShared2()

// -------------------------------------------------------------------
// SDK�o�[�W�����̃`�F�b�N
// -------------------------------------------------------------------
void RCSrl::mrcSDKVersion(FILE *fp)
{
	// SDK�o�[�W����
	System::Boolean isOld  = false;
	System::Boolean isPR   = false;
	System::Boolean isRC   = false;
	for each( RCSDKVersion ^sdk in this->hSDKList )
	{
		if( sdk->IsStatic )
		{
			isPR  = MasterEditorTWL::IsSDKVersionPR( sdk->Code );
			isRC  = MasterEditorTWL::IsSDKVersionRC( sdk->Code );
			if( this->hMrcExternalCheckItems->IsAppendCheck )
			{
				isOld = MasterEditorTWL::IsOldSDKVersion( sdk->Code, this->hMrcExternalCheckItems->SDKVer );
			}
		}
	}
	if( isOld )
	{
		this->hWarnList->Add( this->makeMrcError("SDKVersionOld") );
	}
	if( isPR )
	{
		this->hWarnList->Add( this->makeMrcError("SDKVersionPR") );
	}
	if( isRC )
	{
		this->hWarnList->Add( this->makeMrcError("SDKVersionRC") );
	}
}

// -------------------------------------------------------------------
// �o�i�[�����̃`�F�b�N
// -------------------------------------------------------------------
void RCSrl::mrcBanner(FILE *fp)
{
	u8  map[ 0x10000 ];
	u32 i;

	// �{�̐ݒ肾���̓K�C�h���C������
	if( memcmp( this->pRomHeader->s.game_code, "HNB", 3 ) == 0 )
	{
		//this->hWarnList->Add( gcnew RCMrcError( 
		//	"�o�i�[�t�@�C��", METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE,
		//	"�{�̐ݒ�A�v���Ɍ����Ă̓o�i�[�̕����R�[�h�`�F�b�N���X�L�b�v���܂��B",
		//	"Banner File",
		//	"Only a machine setting app., a charactor code check of the banner file is skip.",
		//	false, true ) );
		return;
	}

	// �o�i�[�I�t�Z�b�g�ɃG���[������Ƃ��ɂ͒��ׂȂ�
	if( this->pRomHeader->s.banner_offset == 0 )
	{
		return;
	}

	// �g�p�\�ȕ����̃}�b�v������
	// �����R�[�h���C���f�b�N�X�Ƃ��Ďg�p�\�Ȃ� 1 ������
	// �����łȂ��Ȃ� 0 ������
	for( i=0; i < 0x10000; i++ )
	{
		map[i] = 0;
	}
	for( i=2; ; i+=2 )	// 0�ڂ̗v�f�̓}�W�b�N�R�[�h�Ȃ̂Ŋ܂߂Ȃ�
	{
		u16 index = (u16)gBannerFont[i+1];
		index = (index << 8) + gBannerFont[i];
		if( index == 0xFEFF )
		{
			break;
		}
		map[ index ] = 1;
	}
	map[ 0x0000 ] = 1;		// 0x0000�͋������

	// �o�i�[��ǂݍ���
	u32 size = this->pRomHeader->s.banner_size;
	u8 *banner = new u8[size];
	fseek( fp, this->pRomHeader->s.banner_offset, SEEK_SET );
	if( size != fread( banner, 1, size, fp ) )
	{
		this->hErrorList->Add( this->makeMrcError("BannerRead") );
	}

	// �o�i�[�̊e�������}�b�v�ɓo�^����Ă��邩�`�F�b�N
	System::Boolean bResult = true;
	u32  start = 0x240;
	u32  end   = (size < 0x1240)?(size):(0x1240);	// NTR�݊��̈�܂ł̂Ƃ���TWL�g���̈���T�[�`���Ȃ�
	for( i=0x240; i < end; i+=2 )
	{
		if( (0x840 <= i) && (i < 0xA40) )	// ���؂̃t�H���g�ӏ��̓`�F�b�N���Ȃ�
		{
			continue;
		}
		u16 index = banner[i+1];
		u16 indexbak = index;
		index = (index << 8) + banner[i];
		if( map[ index ] == 0 )
		{
			//System::Diagnostics::Debug::WriteLine( i.ToString("X") + ":" + index.ToString("X") + ":" + indexbak.ToString("X") );
			bResult = false;
			break;
		}
	}
	if( !bResult )
	{
		this->hErrorList->Add( this->makeMrcError("BannerChar") );
	}
	delete []banner;
} //RCSrl::mrcBanner()

// �o�i�[�Ƃ��Ďg�p�\�ȃt�H���g���X�g(u16���g���G���f�B�A��)
const u8 gBannerFont[] =
{
    0xff, 0xfe, 0x20, 0x00, 0x21, 0x00, 0x22, 0x00, 0x23, 0x00, 0x24, 0x00, 0x25, 0x00, 0x26, 0x00, 
    0x27, 0x00, 0x28, 0x00, 0x29, 0x00, 0x2a, 0x00, 0x2b, 0x00, 0x2c, 0x00, 0x2d, 0x00, 0x2e, 0x00, 
    0x2f, 0x00, 0x30, 0x00, 0x31, 0x00, 0x32, 0x00, 0x33, 0x00, 0x34, 0x00, 0x35, 0x00, 0x36, 0x00, 
    0x37, 0x00, 0x38, 0x00, 0x39, 0x00, 0x3a, 0x00, 0x3b, 0x00, 0x3c, 0x00, 0x3d, 0x00, 0x3e, 0x00, 
    0x3f, 0x00, 0x40, 0x00, 0x41, 0x00, 0x42, 0x00, 0x43, 0x00, 0x44, 0x00, 0x45, 0x00, 0x46, 0x00, 
    0x47, 0x00, 0x48, 0x00, 0x49, 0x00, 0x4a, 0x00, 0x4b, 0x00, 0x4c, 0x00, 0x4d, 0x00, 0x4e, 0x00, 
    0x4f, 0x00, 0x50, 0x00, 0x51, 0x00, 0x52, 0x00, 0x53, 0x00, 0x54, 0x00, 0x55, 0x00, 0x56, 0x00, 
    0x57, 0x00, 0x58, 0x00, 0x59, 0x00, 0x5a, 0x00, 0x5b, 0x00, 0x5c, 0x00, 0x5d, 0x00, 0x5e, 0x00, 
    0x5f, 0x00, 0x60, 0x00, 0x61, 0x00, 0x62, 0x00, 0x63, 0x00, 0x64, 0x00, 0x65, 0x00, 0x66, 0x00, 
    0x67, 0x00, 0x68, 0x00, 0x69, 0x00, 0x6a, 0x00, 0x6b, 0x00, 0x6c, 0x00, 0x6d, 0x00, 0x6e, 0x00, 
    0x6f, 0x00, 0x70, 0x00, 0x71, 0x00, 0x72, 0x00, 0x73, 0x00, 0x74, 0x00, 0x75, 0x00, 0x76, 0x00, 
    0x77, 0x00, 0x78, 0x00, 0x79, 0x00, 0x7a, 0x00, 0x7b, 0x00, 0x7c, 0x00, 0x7d, 0x00, 0x7e, 0x00, 
    0x7f, 0x00, 0xac, 0x20, 0x1a, 0x20, 0x1e, 0x20, 0x26, 0x20, 0xc6, 0x02, 0x52, 0x01, 0x18, 0x20, 
    0x19, 0x20, 0x1c, 0x20, 0x1d, 0x20, 0x22, 0x20, 0xdc, 0x02, 0x22, 0x21, 0x3a, 0x20, 0x53, 0x01, 
    0xa1, 0x00, 0xa2, 0x00, 0xa3, 0x00, 0xa8, 0x00, 0xa9, 0x00, 0xae, 0x00, 0xb7, 0x00, 0xbf, 0x00, 
    0xc0, 0x00, 0xc1, 0x00, 0xc2, 0x00, 0xc3, 0x00, 0xc4, 0x00, 0xc5, 0x00, 0xc6, 0x00, 0xc7, 0x00, 
    0xc8, 0x00, 0xc9, 0x00, 0xca, 0x00, 0xcb, 0x00, 0xcc, 0x00, 0xcd, 0x00, 0xce, 0x00, 0xcf, 0x00, 
    0xd0, 0x00, 0xd1, 0x00, 0xd2, 0x00, 0xd3, 0x00, 0xd4, 0x00, 0xd5, 0x00, 0xd6, 0x00, 0xd8, 0x00, 
    0xd9, 0x00, 0xda, 0x00, 0xdb, 0x00, 0xdc, 0x00, 0xdd, 0x00, 0xdf, 0x00, 0xe0, 0x00, 0xe1, 0x00, 
    0xe2, 0x00, 0xe3, 0x00, 0xe4, 0x00, 0xe5, 0x00, 0xe6, 0x00, 0xe7, 0x00, 0xe8, 0x00, 0xe9, 0x00, 
    0xea, 0x00, 0xeb, 0x00, 0xec, 0x00, 0xed, 0x00, 0xee, 0x00, 0xef, 0x00, 0xf0, 0x00, 0xf1, 0x00, 
    0xf2, 0x00, 0xf3, 0x00, 0xf4, 0x00, 0xf5, 0x00, 0xf6, 0x00, 0xf7, 0x00, 0xf8, 0x00, 0xf9, 0x00, 
    0xfa, 0x00, 0xfb, 0x00, 0xfc, 0x00, 0xfd, 0x00, 0x00, 0x30, 0x01, 0x30, 0x02, 0x30, 0x0c, 0xff, 
    0x0e, 0xff, 0xfb, 0x30, 0x3a, 0x00, 0x3b, 0x00, 0x1f, 0xff, 0x01, 0xff, 0xfc, 0x30, 0x41, 0x30, 
    0x42, 0x30, 0x43, 0x30, 0x44, 0x30, 0x45, 0x30, 0x46, 0x30, 0x47, 0x30, 0x48, 0x30, 0x49, 0x30, 
    0x4a, 0x30, 0x4b, 0x30, 0x4c, 0x30, 0x4d, 0x30, 0x4e, 0x30, 0x4f, 0x30, 0x50, 0x30, 0x51, 0x30, 
    0x52, 0x30, 0x53, 0x30, 0x54, 0x30, 0x55, 0x30, 0x56, 0x30, 0x57, 0x30, 0x58, 0x30, 0x59, 0x30, 
    0x5a, 0x30, 0x5b, 0x30, 0x5c, 0x30, 0x5d, 0x30, 0x5e, 0x30, 0x5f, 0x30, 0x60, 0x30, 0x61, 0x30, 
    0x62, 0x30, 0x63, 0x30, 0x64, 0x30, 0x65, 0x30, 0x66, 0x30, 0x67, 0x30, 0x68, 0x30, 0x69, 0x30, 
    0x6a, 0x30, 0x6b, 0x30, 0x6c, 0x30, 0x6d, 0x30, 0x6e, 0x30, 0x6f, 0x30, 0x70, 0x30, 0x71, 0x30, 
    0x72, 0x30, 0x73, 0x30, 0x74, 0x30, 0x75, 0x30, 0x76, 0x30, 0x77, 0x30, 0x78, 0x30, 0x79, 0x30, 
    0x7a, 0x30, 0x7b, 0x30, 0x7c, 0x30, 0x7d, 0x30, 0x7e, 0x30, 0x7f, 0x30, 0x80, 0x30, 0x81, 0x30, 
    0x82, 0x30, 0x83, 0x30, 0x84, 0x30, 0x85, 0x30, 0x86, 0x30, 0x87, 0x30, 0x88, 0x30, 0x89, 0x30, 
    0x8a, 0x30, 0x8b, 0x30, 0x8c, 0x30, 0x8d, 0x30, 0x8e, 0x30, 0x8f, 0x30, 0x90, 0x30, 0x91, 0x30, 
    0x92, 0x30, 0x93, 0x30, 0x9b, 0x30, 0x9c, 0x30, 0x9d, 0x30, 0x9e, 0x30, 0xa1, 0x30, 0xa2, 0x30, 
    0xa3, 0x30, 0xa4, 0x30, 0xa5, 0x30, 0xa6, 0x30, 0xa7, 0x30, 0xa8, 0x30, 0xa9, 0x30, 0xaa, 0x30, 
    0xab, 0x30, 0xac, 0x30, 0xad, 0x30, 0xae, 0x30, 0xaf, 0x30, 0xb0, 0x30, 0xb1, 0x30, 0xb2, 0x30, 
    0xb3, 0x30, 0xb4, 0x30, 0xb5, 0x30, 0xb6, 0x30, 0xb7, 0x30, 0xb8, 0x30, 0xb9, 0x30, 0xba, 0x30, 
    0xbb, 0x30, 0xbc, 0x30, 0xbd, 0x30, 0xbe, 0x30, 0xbf, 0x30, 0xc0, 0x30, 0xc1, 0x30, 0xc2, 0x30, 
    0xc3, 0x30, 0xc4, 0x30, 0xc5, 0x30, 0xc6, 0x30, 0xc7, 0x30, 0xc8, 0x30, 0xc9, 0x30, 0xca, 0x30, 
    0xcb, 0x30, 0xcc, 0x30, 0xcd, 0x30, 0xce, 0x30, 0xcf, 0x30, 0xd0, 0x30, 0xd1, 0x30, 0xd2, 0x30, 
    0xd3, 0x30, 0xd4, 0x30, 0xd5, 0x30, 0xd6, 0x30, 0xd7, 0x30, 0xd8, 0x30, 0xd9, 0x30, 0xda, 0x30, 
    0xdb, 0x30, 0xdc, 0x30, 0xdd, 0x30, 0xde, 0x30, 0xdf, 0x30, 0xe0, 0x30, 0xe1, 0x30, 0xe2, 0x30, 
    0xe3, 0x30, 0xe4, 0x30, 0xe5, 0x30, 0xe6, 0x30, 0xe7, 0x30, 0xe8, 0x30, 0xe9, 0x30, 0xea, 0x30, 
    0xeb, 0x30, 0xec, 0x30, 0xed, 0x30, 0xee, 0x30, 0xef, 0x30, 0xf0, 0x30, 0xf1, 0x30, 0xf2, 0x30, 
    0xf3, 0x30, 0xf4, 0x30, 0xf5, 0x30, 0xf6, 0x30, 0x01, 0x30, 0x02, 0x30, 0x0c, 0xff, 0x0e, 0xff, 
    0xfb, 0x30, 0x1a, 0xff, 0x1b, 0xff, 0x1f, 0xff, 0x01, 0xff, 0x9b, 0x30, 0x9c, 0x30, 0xb4, 0x00, 
    0x40, 0xff, 0x3e, 0xff, 0xe3, 0xff, 0x3f, 0xff, 0x05, 0x30, 0xfc, 0x30, 0x15, 0x20, 0x10, 0x20, 
    0x0f, 0xff, 0x3c, 0xff, 0x5e, 0xff, 0x5c, 0xff, 0x26, 0x20, 0x18, 0x20, 0x19, 0x20, 0x1c, 0x20, 
    0x1d, 0x20, 0x08, 0xff, 0x09, 0xff, 0x14, 0x30, 0x15, 0x30, 0x3b, 0xff, 0x3d, 0xff, 0x5b, 0xff, 
    0x5d, 0xff, 0x08, 0x30, 0x09, 0x30, 0x0c, 0x30, 0x0d, 0x30, 0x0b, 0xff, 0x0d, 0xff, 0xb1, 0x00, 
    0xd7, 0x00, 0xf7, 0x00, 0x1d, 0xff, 0x1e, 0x22, 0x34, 0x22, 0xb0, 0x00, 0x32, 0x20, 0x33, 0x20, 
    0x06, 0xff, 0x06, 0x26, 0x05, 0x26, 0xcb, 0x25, 0xcf, 0x25, 0xce, 0x25, 0xc7, 0x25, 0xc6, 0x25, 
    0xa1, 0x25, 0xa0, 0x25, 0xb3, 0x25, 0xb2, 0x25, 0xbd, 0x25, 0xbc, 0x25, 0x3b, 0x20, 0x12, 0x30, 
    0x92, 0x21, 0x90, 0x21, 0x91, 0x21, 0x93, 0x21, 0x6f, 0x26, 0x6d, 0x26, 0x6a, 0x26, 0x0a, 0x00, 
	0xff, 0xfe,		// �I����\���Ӗ���0xFEFF���Ō�ɂ���
};
