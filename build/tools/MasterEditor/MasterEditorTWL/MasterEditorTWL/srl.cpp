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

//
// RCSrl �N���X
//

// constructor
RCSrl::RCSrl()
{
	this->pRomHeader = new (ROM_Header);
	std::memset( pRomHeader, 0, sizeof(ROM_Header) );

	this->hMrcSpecialList = gcnew RCMrcSpecialList();
}

// destructor
RCSrl::~RCSrl()
{
	// �|�C���^��(���Ԃ�) unmanaged �Ȃ̂Ŏ���I�ɉ������
	delete (this->pRomHeader);
}

//
// ROM�w�b�_���t�@�C�����烊�[�h
//
// @arg [in] ���̓t�@�C����
//
ECSrlResult RCSrl::readFromFile( System::String ^filename )
{
	FILE       *fp = NULL;
	const char *pchFilename = 
		(const char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( filename ).ToPointer();
	ECSrlResult r;

	// �t�@�C�����J����ROM�w�b�_�̂ݓǂݏo��
	if( fopen_s( &fp, pchFilename, "rb" ) != NULL )
	{
		return (ECSrlResult::ERROR_FILE_OPEN);
	}
	(void)fseek( fp, 0, SEEK_SET );		// ROM�w�b�_��srl�̐擪����

	// 1�o�C�g��sizeof(~)�������[�h (�t���ƕԂ�l��sizeof(~)�ɂȂ�Ȃ��̂Œ���)
	if( fread( (void*)(this->pRomHeader), 1, sizeof(ROM_Header), fp ) != sizeof(ROM_Header) )
	{
		return (ECSrlResult::ERROR_FILE_READ);
	}
	{
		// �t�@�C�������O��ROM�w�b�_�ȊO�̗̈悩��ݒ�����o��
		(void)this->hasDSDLPlaySign( fp );
		r = this->searchSDKVersion( fp );
		if( r != ECSrlResult::NOERROR )
		{
			(void)fclose(fp);
			return r;
		}
		r = this->searchLicenses( fp );
		if( r != ECSrlResult::NOERROR )
		{
			(void)fclose(fp);
			return r;
		}
	}

#ifdef METWL_WHETHER_SIGN_DECRYPT
	// �܂������`�F�b�N
	r = this->decryptRomHeader();
	if( r != ECSrlResult::NOERROR )
	{
		(void)fclose(fp);
		return r;
	}
#endif //#ifdef METWL_WHETHER_SIGN_DECRYPT

	// �G���[���X�g���N���A
	this->hErrorList = gcnew System::Collections::Generic::List<RCMrcError^>;
	this->hWarnList  = gcnew System::Collections::Generic::List<RCMrcError^>;
	this->hParentalErrorList = gcnew System::Collections::Generic::List<RCMrcError^>;
	this->hErrorList->Clear();
	this->hWarnList->Clear();
	this->hParentalErrorList->Clear();

	// ROM�w�b�_�̒l��ROM�ŗL���t�B�[���h�ɔ��f������
	(void)this->setRomInfo();

	// ���ׂĐݒ肵�����Ƃ�MRC
	{
		ECSrlResult r;
		r = this->mrc( fp );
		if( r != ECSrlResult::NOERROR )
		{
			(void)fclose(fp);
			return r;
		}
	}
	(void)fclose( fp );
	return (ECSrlResult::NOERROR);
}

//
// ROM�w�b�_���t�@�C���Ƀ��C�g
//
// @arg [in] �o�̓t�@�C����
//
ECSrlResult RCSrl::writeToFile( System::String ^filename )
{
	FILE       *fp = NULL;
	const char *pchFilename = 
		(const char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( filename ).ToPointer();
	ECSrlResult result;

	// ROM�w�b�_��ROM�ŗL���𔽉f������
	result = this->setRomHeader();
	if( result != ECSrlResult::NOERROR )
	{
		return result;
	}

	// �t�@�C����ROM�w�b�_�����C�g
	if( fopen_s( &fp, pchFilename, "r+b" ) != NULL )	// �㏑���E�o�C�i��
	{
		return (ECSrlResult::ERROR_FILE_OPEN);
	}
	(void)fseek( fp, 0, SEEK_SET );

	if( fwrite( (const void*)(this->pRomHeader), 1, sizeof(ROM_Header), fp ) != sizeof(ROM_Header) )
	{
		return (ECSrlResult::ERROR_FILE_WRITE);
	}
	(void)fclose( fp );

	return (ECSrlResult::NOERROR);
}

//
// ROM�w�b�_����擾����ROM�ŗL�����t�B�[���h�ɔ��f������
//
ECSrlResult RCSrl::setRomInfo(void)
{
	System::Int32 i;
	System::Text::UTF8Encoding^ utf8 = gcnew System::Text::UTF8Encoding( true );	// char->String�ϊ��ɕK�v

	// NTR�݊����
	this->hTitleName  = gcnew System::String( this->pRomHeader->s.title_name, 0, TITLE_NAME_MAX, utf8 );
	this->hGameCode   = gcnew System::String( this->pRomHeader->s.game_code,  0, GAME_CODE_MAX,  utf8 );
	this->hMakerCode  = gcnew System::String( this->pRomHeader->s.maker_code, 0, MAKER_CODE_MAX, utf8 );
	switch( this->pRomHeader->s.platform_code )
	{
		case PLATFORM_CODE_NTR:         this->hPlatform = gcnew System::String( "NTR Limited" );    break;
		case PLATFORM_CODE_TWL_HYBLID:  this->hPlatform = gcnew System::String( "NTR/TWL Hybrid" ); break;
		case PLATFORM_CODE_TWL_LIMITED: this->hPlatform = gcnew System::String( "TWL Limited" );    break;
		default:
			this->hPlatform = nullptr;
		break;
	}
	this->hRomSize = MasterEditorTWL::transRomSizeToString( this->pRomHeader->s.rom_size );
	//this->hForKorea   = gcnew System::Byte( this->pRomHeader->s.for_korea );
	//this->hForChina   = gcnew System::Byte( this->pRomHeader->s.for_china );
	this->hRomVersion = gcnew System::Byte( this->pRomHeader->s.rom_version );
	this->hHeaderCRC  = gcnew System::UInt16( this->pRomHeader->s.header_crc16 );
	this->hIsOldDevEncrypt = gcnew System::Boolean( (this->pRomHeader->s.developer_encrypt_old != 0)?true:false ); 

	switch( this->pRomHeader->s.game_cmd_param & CARD_LATENCY_MASK )
	{
		case CARD_MROM_GAME_LATENCY:
			this->hLatency = "MROM";
		break;

		case CARD_1TROM_GAME_LATENCY:
			this->hLatency = "1TROM";
		break;

		default:
			this->hLatency = "Illegal";
		break;
	}

	// TWL��p���
	this->hIsNormalJump = gcnew System::Boolean( (this->pRomHeader->s.permit_landing_normal_jump != 0)?true:false );
	this->hIsTmpJump    = gcnew System::Boolean( (this->pRomHeader->s.permit_landing_tmp_jump    != 0)?true:false );
	this->hNormalRomOffset   = gcnew System::UInt32( (u32)(this->pRomHeader->s.twl_card_normal_area_rom_offset)   * 0x80000 );
	this->hKeyTableRomOffset = gcnew System::UInt32( (u32)(this->pRomHeader->s.twl_card_keytable_area_rom_offset) * 0x80000 );
	this->hPublicSize  = gcnew System::UInt32( this->pRomHeader->s.public_save_data_size );
	this->hPrivateSize = gcnew System::UInt32( this->pRomHeader->s.private_save_data_size );

	u8  *idL = this->pRomHeader->s.titleID_Lo;
	u32  idH = this->pRomHeader->s.titleID_Hi;
	//u32  val;
	//val = ((u32)(idL[0]) << 24) | ((u32)(idL[1]) << 16) | ((u32)(idL[2]) << 8) | ((u32)(idL[3]));	// �r�b�O�G���f�B�A��
	//this->hTitleIDLo   = gcnew System::UInt32( val );
	this->hTitleIDLo   = gcnew System::String( (char*)idL, 0, 4, utf8 );
	this->hTitleIDHi   = gcnew System::UInt32( idH );

	// TitleID����킩����
	this->hIsAppLauncher = gcnew System::Boolean( false );
	this->hIsAppUser     = gcnew System::Boolean( false );
	this->hIsAppSystem   = gcnew System::Boolean( false );
	this->hIsAppSecure   = gcnew System::Boolean( false );
	this->hIsLaunch      = gcnew System::Boolean( false );
	this->hIsMediaNand   = gcnew System::Boolean( false );
	this->hIsDataOnly    = gcnew System::Boolean( false );
	if( (idL[3]=='H') && (idL[2]=='N') && (idL[1]=='A') )	// �����`���A�v�����ǂ�����TitleID_Lo�̒l�Ō���
    {
		*(this->hIsAppLauncher) = true;
    }
    else if( idH & TITLE_ID_HI_SECURE_FLAG_MASK )				// �e�r�b�g�͔r���I�Ƃ͌���Ȃ��̂�else if�ɂ͂Ȃ�Ȃ�
    {
		*(this->hIsAppSecure) = true;
    }
    else if( (idH & TITLE_ID_HI_APP_TYPE_MASK) == 1 )
    {
		*(this->hIsAppSystem) = true;
    }
    else if( (idH & TITLE_ID_HI_APP_TYPE_MASK) == 0 )
    {
		*(this->hIsAppUser) = true;
    }
	if( idH & TITLE_ID_HI_DATA_ONLY_FLAG_MASK )
	{
		*(this->hIsDataOnly) = true;
	}
	if( idH & TITLE_ID_HI_MEDIA_MASK )
	{
		*(this->hIsMediaNand) = true;
	}
	if( (idH & TITLE_ID_HI_NOT_LAUNCH_FLAG_MASK) == 0 )		// �r�b�g��0�̂Ƃ�Launch
	{
		*(this->hIsLaunch) = true;
	}
	u16 pub = (u16)((idH & TITLE_ID_HI_PUBLISHER_CODE_MASK) >> TITLE_ID_HI_PUBLISHER_CODE_SHIFT);
	this->hPublisherCode = gcnew System::UInt16( pub );

	// TWL�g���t���O
	this->hIsCodecTWL  = gcnew System::Boolean( (this->pRomHeader->s.exFlags.codec_mode != 0)?true:false );
	this->hIsEULA      = gcnew System::Boolean( (this->pRomHeader->s.exFlags.agree_EULA != 0)?true:false );
	this->hIsSubBanner = gcnew System::Boolean( (this->pRomHeader->s.exFlags.availableSubBannerFile != 0)?true:false );
	this->hIsWiFiIcon  = gcnew System::Boolean( (this->pRomHeader->s.exFlags.WiFiConnectionIcon != 0)?true:false );
	this->hIsWirelessIcon = gcnew System::Boolean( (this->pRomHeader->s.exFlags.DSWirelessIcon != 0)?true:false );
	this->hIsWL        = gcnew System::Boolean( (this->pRomHeader->s.exFlags.enable_nitro_whitelist_signature != 0)?true:false );

	// TWL�A�N�Z�X�R���g���[��
	this->hIsCommonClientKey = gcnew System::Boolean( (this->pRomHeader->s.access_control.common_client_key != 0)?true:false );
	this->hIsAesSlotBForES   = gcnew System::Boolean( (this->pRomHeader->s.access_control.hw_aes_slot_B != 0)?true:false );
	this->hIsAesSlotCForNAM  = gcnew System::Boolean( (this->pRomHeader->s.access_control.hw_aes_slot_C != 0)?true:false );
	this->hIsSD              = gcnew System::Boolean( (this->pRomHeader->s.access_control.sd_card_access != 0)?true:false );
	this->hIsNAND            = gcnew System::Boolean( (this->pRomHeader->s.access_control.nand_access != 0)?true:false );
	this->hIsGameCardOn      = gcnew System::Boolean( (this->pRomHeader->s.access_control.game_card_on != 0)?true:false );
	this->hIsShared2         = gcnew System::Boolean( (this->pRomHeader->s.access_control.shared2_file != 0)?true:false );
	this->hIsAesSlotBForJpegEnc = gcnew System::Boolean( (this->pRomHeader->s.access_control.hw_aes_slot_B_SignJPEGForLauncher != 0)?true:false );
	this->hIsAesSlotBForJpegEncUser = gcnew System::Boolean( (this->pRomHeader->s.access_control.hw_aes_slot_B_SignJPEGForUser != 0)?true:false );
	this->hIsGameCardNitro   = gcnew System::Boolean( (this->pRomHeader->s.access_control.game_card_nitro_mode != 0)?true:false );
	this->hIsAesSlotAForSSL  = gcnew System::Boolean( (this->pRomHeader->s.access_control.hw_aes_slot_A_SSLClientCert != 0)?true:false );
	this->hIsCommonClientKeyForDebugger 
		= gcnew System::Boolean( (this->pRomHeader->s.access_control.common_client_key_for_debugger_sysmenu != 0)?true:false );

	// SCFG �����b�N����邩
	if( (this->pRomHeader->s.arm7_scfg_ext >> 31) != 0 )
	{
		this->hIsSCFGAccess = gcnew System::Boolean( true );
	}
	else
	{
		this->hIsSCFGAccess = gcnew System::Boolean( false );
	}


	// Shared2�t�@�C���T�C�Y
	this->hShared2SizeArray = gcnew cli::array<System::UInt32^>(METWL_NUMOF_SHARED2FILES);
	for( i=0; i < METWL_NUMOF_SHARED2FILES; i++ )
	{
		this->hShared2SizeArray[i] = gcnew System::UInt32( 0 );
	}
	u32  unit = 16 * 1024;		// 16KB�̏搔���i�[����Ă���
	if( this->pRomHeader->s.shared2_file0_size != 0 )
	{
		*(this->hShared2SizeArray[0]) = (this->pRomHeader->s.shared2_file0_size * unit) + unit;
	}
	if( this->pRomHeader->s.shared2_file1_size != 0 )
	{
		*(this->hShared2SizeArray[1]) = (this->pRomHeader->s.shared2_file1_size * unit) + unit;
	}
	if( this->pRomHeader->s.shared2_file2_size != 0 )
	{
		*(this->hShared2SizeArray[2]) = (this->pRomHeader->s.shared2_file2_size * unit) + unit;
	}
	if( this->pRomHeader->s.shared2_file3_size != 0 )
	{
		*(this->hShared2SizeArray[3]) = (this->pRomHeader->s.shared2_file3_size * unit) + unit;
	}
	if( this->pRomHeader->s.shared2_file4_size != 0 )
	{
		*(this->hShared2SizeArray[4]) = (this->pRomHeader->s.shared2_file4_size * unit) + unit;
	}
	if( this->pRomHeader->s.shared2_file5_size != 0 )
	{
		*(this->hShared2SizeArray[5]) = (this->pRomHeader->s.shared2_file5_size * unit) + unit;
	}

	// �J�[�h���[�W����
	const u32  map           = this->pRomHeader->s.card_region_bitmap;
	this->hIsRegionJapan     = gcnew System::Boolean( ((map & METWL_MASK_REGION_JAPAN)     != 0)?true:false );
	this->hIsRegionAmerica   = gcnew System::Boolean( ((map & METWL_MASK_REGION_AMERICA)   != 0)?true:false );
	this->hIsRegionEurope    = gcnew System::Boolean( ((map & METWL_MASK_REGION_EUROPE)    != 0)?true:false );
	this->hIsRegionAustralia = gcnew System::Boolean( ((map & METWL_MASK_REGION_AUSTRALIA) != 0)?true:false );

	// �y�A�����^���R���g���[��
	this->setParentalControlInfo();

	return ECSrlResult::NOERROR;
} // ECSrlResult RCSrl::setRomInfo(void)

// ROM�w�b�_���̃y�A�����^���R���g���[�������t�B�[���h�ɔ��f������
void RCSrl::setParentalControlInfo(void)
{
	// ���[�W�����Ɋ܂܂�Ă��Ȃ����͓̂ǂݍ��܂Ȃ�
	this->hArrayParentalIndex = gcnew cli::array<int>(PARENTAL_CONTROL_INFO_SIZE);

	int i;
	for( i=0; i < PARENTAL_CONTROL_INFO_SIZE; i++ )
	{
		this->hArrayParentalIndex[i] = -1;
	}

	// ���[�W�����Ɋ܂܂�Ă���c�̂����X�g�A�b�v
	u32 region = this->pRomHeader->s.card_region_bitmap;
	switch( region )
	{
		case METWL_MASK_REGION_JAPAN:
			this->setOneRatingOrgInfo( OS_TWL_PCTL_OGN_CERO );	// ���[�W�����Ɋ܂܂�Ȃ��c�̂̏��͓ǂݍ��܂Ȃ�
		break;

		case METWL_MASK_REGION_AMERICA:
			this->setOneRatingOrgInfo( OS_TWL_PCTL_OGN_ESRB );
		break;

		case METWL_MASK_REGION_EUROPE:
			this->setOneRatingOrgInfo( OS_TWL_PCTL_OGN_USK );
			this->setOneRatingOrgInfo( OS_TWL_PCTL_OGN_PEGI_GEN );
			this->setOneRatingOrgInfo( OS_TWL_PCTL_OGN_PEGI_PRT );
			this->setOneRatingOrgInfo( OS_TWL_PCTL_OGN_PEGI_BBFC );
		break;

		case METWL_MASK_REGION_AUSTRALIA:
			this->setOneRatingOrgInfo( OS_TWL_PCTL_OGN_OFLC );
		break;

		case (METWL_MASK_REGION_EUROPE|METWL_MASK_REGION_AUSTRALIA):
			this->setOneRatingOrgInfo( OS_TWL_PCTL_OGN_USK );
			this->setOneRatingOrgInfo( OS_TWL_PCTL_OGN_PEGI_GEN );
			this->setOneRatingOrgInfo( OS_TWL_PCTL_OGN_PEGI_PRT );
			this->setOneRatingOrgInfo( OS_TWL_PCTL_OGN_PEGI_BBFC );
			this->setOneRatingOrgInfo( OS_TWL_PCTL_OGN_OFLC );
		break;

#if defined(METWL_VER_APPTYPE_SYSTEM) || defined(METWL_VER_APPTYPE_SECURE) || defined(METWL_VER_APPTYPE_LAUNCHER)
		case METWL_MASK_REGION_ALL:
			this->setOneRatingOrgInfo( OS_TWL_PCTL_OGN_CERO );
			this->setOneRatingOrgInfo( OS_TWL_PCTL_OGN_ESRB );
			this->setOneRatingOrgInfo( OS_TWL_PCTL_OGN_USK );
			this->setOneRatingOrgInfo( OS_TWL_PCTL_OGN_PEGI_GEN );
			this->setOneRatingOrgInfo( OS_TWL_PCTL_OGN_PEGI_PRT );
			this->setOneRatingOrgInfo( OS_TWL_PCTL_OGN_PEGI_BBFC );
			this->setOneRatingOrgInfo( OS_TWL_PCTL_OGN_OFLC );
		break;
#endif //#if defined(METWL_VER_APPTYPE_SYSTEM) || defined(METWL_VER_APPTYPE_SECURE) || defined(METWL_VER_APPTYPE_LAUNCHER)

		default:
			this->hParentalErrorList->Add( gcnew RCMrcError( 
				"�J�[�h���[�W����", 0x1b0, 0x1b3, "�d���n�̑g�ݍ��킹���s���ł��B�y�A�����^���R���g���[�����͖������ēǂݍ��܂�܂����B",
				"Card Region", "Illigal Region. Parental Control Information is ignored in reading.", true, true ) );
		break;
	}
}

// �c�̂̃��[�e�B���O��񂩂�R���{�{�b�N�X�̃C���f�b�N�X�����肷��
void RCSrl::setOneRatingOrgInfo( int ogn )
{
	System::Boolean enable = ((this->pRomHeader->s.parental_control_rating_info[ ogn ] & OS_TWL_PCTL_OGNINFO_ENABLE_MASK) != 0)?true:false;
	System::Boolean rp     = ((this->pRomHeader->s.parental_control_rating_info[ ogn ] & OS_TWL_PCTL_OGNINFO_ALWAYS_MASK) != 0)?true:false;
	System::Byte    age    = this->pRomHeader->s.parental_control_rating_info[ ogn ] & OS_TWL_PCTL_OGNINFO_AGE_MASK;

	cli::array<System::Byte> ^ages = MasterEditorTWL::getOgnRatingAges( ogn );
	System::String           ^name = MasterEditorTWL::getOgnName( ogn );

	int index = -1;
	if( !enable )	// ����`
	{
		index = -1;
		this->hParentalErrorList->Add( gcnew RCMrcError( 
			"�y�A�����^���R���g���[�����", 0x2f0, 0x2ff,
			name + ": ����`�ł��B",
			"Parental Control Info.", 
			name + ": Undefined.", true, true ) );
	}
	else
	{
		if( rp )	// �R����
		{
			if( age == 0 ) // �N�0�̂Ƃ��͐R�����Ƃ݂Ȃ�
			{
				index = ages->Length;	// �z��̍Ō�̗v�f�̎����u�R�����v
			}
			else
			{
				index = -1;		// ����ȊO�̓G���[
				this->hParentalErrorList->Add( gcnew RCMrcError( 
					"�y�A�����^���R���g���[�����", 0x2f0, 0x2ff,
					name + ": �R�����w��ƃ��[�e�B���O�N������ɐݒ肳��Ă��܂��B",
					"Parental Control Info.", 
					name + ": Both the Rating-Peding setting and the rating age are setting.", true, true ) );
			}
		}
		else
		{
			index = -1;		// �c�̂��F�߂Ă��Ȃ��N��i�[����Ă���Ƃ��G���[
			int i;
			for( i=0; i < ages->Length; i++ )
			{
				if( age == ages[i] )
				{
					index = i;
				}
			}
			if( index < 0 )
			{
				this->hParentalErrorList->Add( gcnew RCMrcError( 
					"�y�A�����^���R���g���[�����", 0x2f0, 0x2ff,
					name + ": ���[�e�B���O�N��c�̂ɂ���Ďw�肳�ꂽ�l�ł͂���܂���B",
					"Parental Control Info.", 
					name + ": An age isn't accepted by the organization.", true, true ) );
			}
		}
	}
	this->hArrayParentalIndex[ ogn ] = index;
}

//
// ROM�w�b�_��ROM�ŗL���t�B�[���h�̒l�𔽉f������
//
ECSrlResult RCSrl::setRomHeader(void)
{
	ECSrlResult result;

	// ROM�w�b�_��[0,0x160)�̗̈��Read Only�ŕύX���Ȃ�

	// �������̃t���O��ROM�w�b�_�ɔ��f
	this->pRomHeader->s.exFlags.agree_EULA = (*(this->hIsEULA) == true)?1:0;
	this->pRomHeader->s.exFlags.WiFiConnectionIcon = (*(this->hIsWiFiIcon) == true)?1:0;
	this->pRomHeader->s.exFlags.DSWirelessIcon     = (*(this->hIsWirelessIcon) == true)?1:0;

	// ���[�e�B���O
	u32  map = 0;
	if( *(this->hIsRegionJapan)   == true )  { map |= METWL_MASK_REGION_JAPAN; }
	if( *(this->hIsRegionAmerica) == true )  { map |= METWL_MASK_REGION_AMERICA; }
	if( *(this->hIsRegionEurope)  == true )  { map |= METWL_MASK_REGION_EUROPE; }
	if( *(this->hIsRegionAustralia) == true ){ map |= METWL_MASK_REGION_AUSTRALIA; }
#if defined(METWL_VER_APPTYPE_SYSTEM) || defined(METWL_VER_APPTYPE_SECURE) || defined(METWL_VER_APPTYPE_LAUNCHER)
	if( (*(this->hIsRegionJapan) == true ) && (*(this->hIsRegionAmerica) == true)
		&& (*(this->hIsRegionEurope) == true ) && (*(this->hIsRegionAustralia) == true)
	  )
	{
		map |= METWL_MASK_REGION_ALL;	// �I�[�����[�W����������
	}
#endif
	this->pRomHeader->s.card_region_bitmap = map;

	// �y�A�����^���R���g���[��
	this->setParentalControlHeader();

	// ROM�w�b�_��CRC�Ə������X�V����
	result = this->calcRomHeaderCRC();
	if( result != ECSrlResult::NOERROR )
	{
		return result;
	}
	result = this->signRomHeader();
	if( result != ECSrlResult::NOERROR )
	{
		return result;
	}

	return ECSrlResult::NOERROR;
} // ECSrlResult RCSrl::setRomHeader(void)

// �y�A�����^���R���g���[���̃v���p�e�B��ROM�w�b�_�ɔ��f������
void RCSrl::setParentalControlHeader(void)
{
	int i;
	for( i=0; i < PARENTAL_CONTROL_INFO_SIZE; i++ )
	{
		cli::array<System::Byte> ^ages = MasterEditorTWL::getOgnRatingAges( i );	// �ݒ�\�N��X�g���擾

		u8 rating;
		if( this->hArrayParentalIndex[i] < 0 )		// ����`
		{
			rating = 0x00;
		}
		else
		{
			int index = this->hArrayParentalIndex[i];
			if( index == ages->Length )				// �R����
			{
				rating = OS_TWL_PCTL_OGNINFO_ENABLE_MASK | OS_TWL_PCTL_OGNINFO_ALWAYS_MASK;
			}
			else									// ���[�e�B���O�N���ݒ�
			{
				rating = OS_TWL_PCTL_OGNINFO_ENABLE_MASK | ages[ index ];
			}
		}
		this->pRomHeader->s.parental_control_rating_info[i] = rating;
	}
}

//
// �w�b�_��CRC���Z�o
//
static u16 calcCRC( const u16 start, const u8 *data, const int allsize )
{
	u16       r1;
	u16       total = start;
	int       size  = allsize;

	// CRC�e�[�u���͌Œ�
	const u16 CRC16Table[16] =
    {
		0x0000, 0xCC01, 0xD801, 0x1400,
		0xF001, 0x3C00, 0x2800, 0xE401,
		0xA001, 0x6C00, 0x7800, 0xB401,
		0x5000, 0x9C01, 0x8801, 0x4400
    };

	// CRC�v�Z
	while (size-- > 0)
    {
        // ����4bit
        r1 = CRC16Table[total & 0xf];
        total = (total >> 4) & 0x0fff;
        total = total ^ r1 ^ CRC16Table[*data & 0xf];

        // ���4bit
        r1 = CRC16Table[total & 0xf];
        total = (total >> 4) & 0x0fff;
        total = total ^ r1 ^ CRC16Table[(*data >> 4) & 0xf];

        data++;
    }
	return total;
} // calcRomHeaderCRCCore()

ECSrlResult RCSrl::calcRomHeaderCRC(void)
{
	// ROM�w�b�_��CRC�̈���㏑��
	this->pRomHeader->s.header_crc16 = calcCRC( CRC16_INIT_VALUE, (u8*)this->pRomHeader, CALC_CRC16_SIZE );

	return (ECSrlResult::NOERROR);
} // ECSrlResult RCSrl::calcRomHeaderCRC(void)

//
// ROM�w�b�_���ď���
//
ECSrlResult RCSrl::signRomHeader(void)
{
	SignatureData   signSrc;						// �����̂��ƂƂȂ�_�C�W�F�X�g�l
	u8              signDst[ RSA_KEY_LENGTH ];		// �����̊i�[��Tmp�o�b�t�@
	u8              decryptBlock[ RSA_KEY_LENGTH ];	// ������������u���b�N�o�b�t�@
	BOOL            result = false;
	ROM_Header      rh;
	int             pos;
	u8              *privateKey = (u8*)g_devPrivKey_DER;
	u8              *publicKey  = (u8*)g_devPubKey_DER;

	// ROM�w�b�_�̃_�C�W�F�X�g���Z�o(�擪����ؖ����̈�̒��O�܂ł��Ώ�)
	ACSign_DigestUnit(
		signSrc.digest,
		this->pRomHeader,
		(u32)&(rh.certificate) - (u32)&(rh)		// this->pRomHeader �̓}�l�[�W�q�[�v��ɂ���̂Ŏ��A�h���X���擾�ł��Ȃ�
	);

	// ����I��
#ifdef METWL_VER_APPTYPE_LAUNCHER
	if( *this->hIsAppLauncher )
	{
		privateKey = (u8*)g_devPrivKey_DER_launcher;
		publicKey  = (u8*)g_devPubKey_DER_launcher;
	}
	else
#endif //METWL_VER_APPTYPE_LAUNCHER
#ifdef METWL_VER_APPTYPE_SECURE
	if( *this->hIsAppSecure )
	{
		privateKey = (u8*)g_devPrivKey_DER_secure;
		publicKey  = (u8*)g_devPubKey_DER_secure;
	}
	else
#endif //METWL_VER_APPTYPE_SECURE
#ifdef METWL_VER_APPTYPE_SYSTEM
	if( *this->hIsAppSystem )
	{
		privateKey = (u8*)g_devPrivKey_DER_system;
		publicKey  = (u8*)g_devPubKey_DER_system;
	}
	else
#endif //METWL_VER_APPTYPE_SYSTEM
#ifdef METWL_VER_APPTYPE_USER
	{
		privateKey = (u8*)g_devPrivKey_DER;
		publicKey  = (u8*)g_devPubKey_DER;
	}
#endif //METWL_VER_APPTYPE_USER

	// �_�C�W�F�X�g�ɏ�����������
	result = ACSign_Encrypto( signDst, privateKey, &signSrc, sizeof(SignatureData) ); 
	if( !result )
	{
		return (ECSrlResult::ERROR_SIGN_ENCRYPT);
	}

	// �������������ă_�C�W�F�X�g�ƈ�v���邩�x���t�@�C����
	result = ACSign_Decrypto( decryptBlock, publicKey, signDst, RSA_KEY_LENGTH );
	for( pos=0; pos < RSA_KEY_LENGTH; pos++ )
	{
		if( decryptBlock[pos] == 0x0 )			// ������u���b�N������f�[�^���T�[�`
			break;
	}
	if( !result || (memcmp( &signSrc, &(decryptBlock[pos+1]), sizeof(SignatureData) ) != 0) )
	{
		return (ECSrlResult::ERROR_SIGN_DECRYPT);
	}

	// ROM�w�b�_�ɏ������㏑��
	memcpy( this->pRomHeader->signature, signDst, RSA_KEY_LENGTH );

	return (ECSrlResult::NOERROR);
} // ECSrlResult RCSrl::signRomHeader(void)

//
// ROM�w�b�_�̏������O��
//
ECSrlResult RCSrl::decryptRomHeader(void)
{
	u8     original[ RSA_KEY_LENGTH ];	// �����O������̃f�[�^�i�[��
	s32    pos = 0;						// �u���b�N�̐擪�A�h���X
	u8     digest[ DIGEST_SIZE_SHA1 ];	// ROM�w�b�_�̃_�C�W�F�X�g
	u8    *publicKey = (u8*)g_devPubKey_DER;
	ROM_Header rh;

	// <�f�[�^�̗���>
	// (1) ���J���ŕ�����������(�u���b�N)�����[�J���ϐ�(original)�Ɋi�[
	// (2) �u���b�N����]���ȕ�������菜���Ĉ���(pDst)�ɃR�s�[

	ECAppType  type = this->selectAppType();

	// ����I��
#ifdef METWL_VER_APPTYPE_LAUNCHER
	if( type == ECAppType::LAUNCHER )
	{
		publicKey  = (u8*)g_devPubKey_DER_launcher;
	}
	else
#endif //METWL_VER_APPTYPE_LAUNCHER
#ifdef METWL_VER_APPTYPE_SECURE
	if( type == ECAppType::SECURE )
	{
		publicKey  = (u8*)g_devPubKey_DER_secure;
	}
	else
#endif //METWL_VER_APPTYPE_SECURE
#ifdef METWL_VER_APPTYPE_SYSTEM
	if( type == ECAppType::SYSTEM )
	{
		publicKey  = (u8*)g_devPubKey_DER_system;
	}
	else
#endif //METWL_VER_APPTYPE_SYSTEM
#ifdef METWL_VER_APPTYPE_USER
	{
		publicKey  = (u8*)g_devPubKey_DER;
	}
#endif //METWL_VER_APPTYPE_USER

	// �����̉��� = ���J���ŕ���
	if( !ACSign_Decrypto( original, publicKey, this->pRomHeader->signature, RSA_KEY_LENGTH ) )
	{
		return ECSrlResult::ERROR_SIGN_DECRYPT;
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
	ACSign_DigestUnit( digest,	this->pRomHeader, (u32)&(rh.certificate) - (u32)&(rh) );
		// this->pRomHeader �̓}�l�[�W�q�[�v��ɂ���̂Ŏ��A�h���X���擾�ł��Ȃ�
	if( memcmp( &(original[pos+1]), digest, DIGEST_SIZE_SHA1 ) != 0 )
	{
		return ECSrlResult::ERROR_SIGN_VERIFY;
	}
	return (ECSrlResult::NOERROR);
}

//
// �A�v����ʂ̔���
//
ECAppType RCSrl::selectAppType(void)
{
	ECAppType  type = ECAppType::ILLEGAL;

	u8  *idL = this->pRomHeader->s.titleID_Lo;
	u32  idH = this->pRomHeader->s.titleID_Hi;

	if( (idL[3]=='H') && (idL[2]=='N') && (idL[1]=='A') )	// �����`���A�v�����ǂ�����TitleID_Lo�̒l�Ō���
    {
		type = ECAppType::LAUNCHER;
    }
    else if( idH & TITLE_ID_HI_SECURE_FLAG_MASK )				// �e�r�b�g�͔r���I�Ƃ͌���Ȃ��̂�else if�ɂ͂Ȃ�Ȃ�
    {
		type = ECAppType::SECURE;
    }
    else if( (idH & TITLE_ID_HI_APP_TYPE_MASK) == 1 )
    {
		type = ECAppType::SYSTEM;
    }
    else if( (idH & TITLE_ID_HI_APP_TYPE_MASK) == 0 )
    {
		type = ECAppType::USER;
    }
	return type;
}

//
// DS�_�E�����[�h������SRL�Ɋi�[����Ă��邩���ׂ�
//
ECSrlResult RCSrl::hasDSDLPlaySign( FILE *fp )
{
	const s32 DSDLPLAY_SIZE = 0x88;
	u8        buf[ DSDLPLAY_SIZE ];
	u32       offset;

	// �����̈��SRL����ǂݍ���
	offset = this->pRomHeader->s.rom_valid_size;
	if( fseek( fp, offset, SEEK_SET ) != 0 )
	{
		this->hHasDSDLPlaySign = gcnew System::Boolean( false );	// ������NTR��ROM�̈�̍Ō�ɓ���̂�
		return ECSrlResult::ERROR_FILE_READ;						// NTR��SRL�ɏ������i�[����Ă��Ȃ��ꍇ�̓V�[�N�����[�h���ł��Ȃ�
	}
	if( DSDLPLAY_SIZE != fread( buf, 1, DSDLPLAY_SIZE, fp ) )
	{
		this->hHasDSDLPlaySign = gcnew System::Boolean( false );
		return ECSrlResult::ERROR_FILE_READ;
	}

	// �ŏ���2�o�C�g���Œ�̒l�ƂȂ邱�Ƃ��ۏ؂���Ă���̂ł��̒l���ǂ����Ŕ���
	if( (buf[0] == 'a') && (buf[1] == 'c') )
	{
		this->hHasDSDLPlaySign = gcnew System::Boolean( true );
	}
	else
	{
		this->hHasDSDLPlaySign = gcnew System::Boolean( false );
	}
	return (ECSrlResult::NOERROR);
}

//
// SDK�o�[�W�������擾����
//
ECSrlResult RCSrl::searchSDKVersion( FILE *fp )
{
	// SDK�o�[�W������SRL�o�C�i�����Ɉȉ��̃}�W�b�N�i���o�ƂƂ��ɖ��߂��Ă���
	const u8  pattern[8] = {0x21, 0x06, 0xc0, 0xde, 0xde, 0xc0, 0x06, 0x21};
	System::Collections::Generic::List<u32> ^list;

	this->hSDKList = gcnew System::Collections::Generic::List<RCSDKVersion ^>;
	this->hSDKList->Clear();

	list = MasterEditorTWL::patternMatch( fp, pattern , 8 );
	if( list == nullptr )
	{
		//System::Diagnostics::Debug::WriteLine( "no list" );
		return ECSrlResult::ERROR_SDK;
	}
	for each( u32 item in list )
	{
		// �}�W�b�N�R�[�h�̃I�t�Z�b�g�̎�O4�o�C�g��SDK�o�[�W����
		if( item >= 4 )
		{
			u32       offset;
			u32       sdkcode;

			offset = item - 4;
			fseek( fp, offset, SEEK_SET );
			if( 4 != fread( (void*)&sdkcode, 1, 4, fp ) )
			{
				return ECSrlResult::ERROR_SDK;
			}

			// ���
			System::Byte   major = (System::Byte)(0xff & (sdkcode >> 24));
			System::Byte   minor = (System::Byte)(0xff & (sdkcode >> 16));
			System::UInt16 relstep = (System::UInt16)(0xffff & sdkcode);
			System::String ^str = nullptr;
			str += (major.ToString() + "." + minor.ToString() + " ");
			//System::Diagnostics::Debug::WriteLine( "relstep = " + relstep.ToString() );

			// RELSTEP�̉���
			//   PR1=10100 PR2=10200 ...
			//   RC1=20100 RC2=20200 ...
			//   RELEASE=30000
			System::UInt16 patch = relstep;
			while( patch >= 10000 )
			{
				patch -= 10000;
			}
			System::UInt16 rev = patch;
			System::String ^revstr = gcnew System::String( "" );
			while( rev >= 100 )
			{
				rev -= 100;
			}
			if( rev > 0 )
			{
				revstr = " plus" + rev.ToString();
			}
			patch = patch / 100;
			switch( relstep / 10000 )
			{
				case 1: str += ("PR " + patch.ToString() + revstr); break;
				case 2: str += ("RC " + patch.ToString() + revstr); break;
				case 3: str += ("RELEASE " + patch.ToString() + revstr); break;
				default: break;
			}
			u32 statbegin = this->pRomHeader->s.main_rom_offset;
			u32 statend   = this->pRomHeader->s.main_rom_offset + this->pRomHeader->s.main_size - 1;
			System::Boolean isstat = ((statbegin <= offset) && (offset <= statend))?true:false;
			this->hSDKList->Add( gcnew RCSDKVersion(str, sdkcode, isstat) );
			//System::Diagnostics::Debug::WriteLine( "SDK " + str );
		}
	}
	return ECSrlResult::NOERROR;
}

//
// �g�p���C�Z���X���擾����
//
#define  LICENSE_LEN_MAX   1024		// ����������C�Z���X�̕����񂪒����Ƃ��������擾�ł��Ȃ�
ECSrlResult RCSrl::searchLicenses(FILE *fp)
{
	// ���C�Z���X��SRL�o�C�i������[SDK+(�z�z��):(���C�u������)]�̃t�H�[�}�b�g�Ŗ��߂��Ă���
	const u8 pattern[5] = { '[', 'S', 'D', 'K', '+' };
	System::Collections::Generic::List<u32> ^list;

	this->hLicenseList = gcnew System::Collections::Generic::List<RCLicense ^>;
	this->hLicenseList->Clear();
	
	fseek( fp, 0, SEEK_END );
	const u32 filesize = ftell( fp );

	list = MasterEditorTWL::patternMatch( fp, pattern, 5 );
	if( list == nullptr )
	{
		return ECSrlResult::NOERROR;	// ���C�Z���X���Ȃ��ꍇ�����݂���̂�OK�Ƃ���
	}
	for each( u32 item in list )
	{
		char  buf[ LICENSE_LEN_MAX + 1 ];	// '\0'�̕��������߂ɂƂ��Ă���
		u32   offset = item + 5;			// "[SDK+"�̌ォ�烊�[�h����
		u32   len = ((filesize - offset) < LICENSE_LEN_MAX)?(filesize - offset):LICENSE_LEN_MAX;

		// "(�z�z��):(���C�u������)]"��]���ȕ������܂߂ĂЂƂ܂��o�b�t�@�Ɋi�[
		fseek( fp, offset, SEEK_SET );
		if( len != fread( buf, 1, len, fp ) )
		{
			return ECSrlResult::ERROR;
		}
		buf[ len ] = '\0';	// ���strlen�̐�����ۏ؂��邽��

		// "(�z�z��):(���C�u������)]"�����o���Ă���(�z�z��)��(���C�u������)�ɕ���
		System::Text::UTF8Encoding^ utf8 = gcnew System::Text::UTF8Encoding( true );	// char->String�ϊ��ɕK�v
		System::String ^str = gcnew System::String( buf, 0, strlen(buf), utf8 );		// �T���╪����String���\�b�h�ɗp�ӂ���Ă���
		str = (str->Split( ']' ))[0];							// ']'�����O�����o��
		cli::array<System::String^> ^spl = str->Split( ':' );	// (�z�z��):(���C�u������)�𕪊�����

		if( spl->Length >= 2  )		// �O�̂���
		{
			if( !spl[1]->StartsWith("BACKUP") && !spl[1]->StartsWith("SSL") && !spl[1]->StartsWith("CPS") )
			{
				this->hLicenseList->Add( gcnew RCLicense( spl[0], spl[1] ) );
			}
			//System::Diagnostics::Debug::WriteLine( "license " + spl[0] + " " + spl[1] );
		}
	}
	return ECSrlResult::NOERROR;
} //RCSrl::searchLicenses

//
// MRC
//
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
	this->hErrorList->AddRange( this->hParentalErrorList );		// �y�A�����^���R���g���[���̃��X�g�͕ʂɂ����Ă���̂Œǉ�
#endif
	return ECSrlResult::NOERROR;
}

// NTR�݊�MRC
ECSrlResult RCSrl::mrcNTR( FILE *fp )
{
	System::Int32  i;
	System::Boolean result;

	// ROM�w�b�_�̃`�F�b�N

	// �����R�[�h�`�F�b�N
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
		this->hErrorList->Add( gcnew RCMrcError( 
			"�\�t�g�^�C�g��", 0x0, 0xb, "�g�p�s��ASCII�R�[�h���g�p����Ă��܂��B",
			"Game Title", "Unusable ASCII code is used.", false, true ) );
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
		this->hErrorList->Add( gcnew RCMrcError( 
			"�\�t�g�^�C�g��", 0x0, 0xb, "�����̖��g�p�����ɂ�00h��o�^���Ă��������B",
			"Game Title", "Please use 00h for an unused part.", false, true ) );
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
		this->hErrorList->Add( gcnew RCMrcError( 
			"�\�t�g�^�C�g��", 0x0, 0xb, "�X�y�[�X�����ɂ�20h��o�^���Ă��������B",
			"Game Title", "Please use 20h for an space part.", false, true ) );
	}

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
		this->hErrorList->Add( gcnew RCMrcError( 
			"�C�j�V�����R�[�h", 0xc, 0xf, "�g�p�s��ASCII�R�[�h���g�p����Ă��܂��B",
			"Game Code", "Unusable ASCII code is used.", false, true ) );
	}
	if( memcmp( this->pRomHeader->s.game_code, "NTRJ", GAME_CODE_MAX ) == 0 )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			"�C�j�V�����R�[�h", 0xc, 0xf, "SDK�f�t�H���g�̃R�[�h(NTRJ)���g�p����Ă��܂��B",
			"Game Code", "SDK default code(NTRJ) is used.", false, true ) );
	}

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
		this->hErrorList->Add( gcnew RCMrcError(
			"���[�J�[�R�[�h", 0x10, 0x11, "�g�p�s��ASCII�R�[�h���g�p����Ă��܂��B",
			"Maker Code", "Unusable ASCII code is used.", false, true ) );
	}

	// �l�`�F�b�N

	if( this->pRomHeader->s.rom_type != 0x00 )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			"�f�o�C�X�^�C�v", 0x13, 0x13, "�s���Ȓl�ł��B00h��ݒ肵�Ă��������B",
			"Device Type", "Invalid data. Please set 00h.", false, true ) );
	}

	u8 romver = this->pRomHeader->s.rom_version;
	if( ((romver < 0x00) || (0x0f < romver)) && (romver != 0xE0) )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			"���}�X�^�[�o�[�W����", 0x1e, 0x1e, "�s���Ȓl�ł��B�����łł�01h-0Fh�̂����ꂩ�̒l�A���O�łł�E0h��ݒ肵�Ă��������B",
			"Release Ver.", "Invalid data. Please set either one of 01h-0Fh(Regular ver.), or E0h(Preliminary ver.)", false, true ) );
	}
	if( this->pRomHeader->s.banner_offset == 0 )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			"�o�i�[�I�t�Z�b�g", 0x68, 0x6b, "�o�i�[�f�[�^���ݒ肳��Ă��܂���B",
			"Banner Offset.", "Banner data is not set.", false, true ) );
	}
	if( this->pRomHeader->s.rom_valid_size == 0 )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			"ROM�����T�C�Y", 0x80, 0x83, "�l���ݒ肳��Ă��܂���B",
			"ROM Valid Size.", "Data is not set.", false, true ) );
	}

	// CRC
	u16  crc;
	// �Z�L���A�̈�
	if( *this->hIsMediaNand == false )	// �J�[�h�A�v���̂Ƃ��̂�
	{
		// �Z�L���A�̈��ROM�w�b�_�O
		u8     *secures;
		s32     secure_size = SECURE_AREA_END - this->pRomHeader->s.main_rom_offset;
		if (secure_size > SECURE_AREA_SIZE)
		{
			secure_size = SECURE_AREA_SIZE;
		}
		if (secure_size <= 0)
		{
			this->hErrorList->Add( gcnew RCMrcError( 
				"�Z�L���A�̈�CRC", 0x15e, 0x15f, "�Z�L���A�̈�̃A�h���X�w�肪�s���ł��B",
				"Secure Area CRC.", "Illegal address of secure area.", false, true ) );
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
			this->hErrorList->Add( gcnew RCMrcError( 
				"�Z�L���A�̈�CRC", 0x07c, 0x07d, "�v�Z���ʂƈ�v���܂���B�Z�L���A�̈悪�����񂳂ꂽ�\��������܂��B",
				"Secure Area CRC.", "Calclated CRC is different from Registered one.", false, true ) );
		}
	}

	// ���S�̈�
	crc = 0xcf56;
	if( crc != this->pRomHeader->s.nintendo_logo_crc16 )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			"�L�����N�^�[�f�[�^CRC", 0x15c, 0x15d, "�v�Z���ʂƈ�v���܂���B�L�����N�^�[�f�[�^�������񂳂ꂽ�\��������܂��B",
			"Charactor Data CRC.", "Calclated CRC is different from Registered one.", false, true ) );
	}

	// �w�b�_CRC
	crc = calcCRC( CRC16_INIT_VALUE, (u8*)this->pRomHeader, CALC_CRC16_SIZE );
	if( crc != this->pRomHeader->s.header_crc16 )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			"ROM�w�b�_CRC", 0x15e, 0x15f, "�v�Z���ʂƈ�v���܂���BROM�w�b�_�������񂳂ꂽ�\��������܂��B",
			"ROM Header CRC.", "Calclated CRC is different from Registered one.", false, true ) );
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
		this->hErrorList->Add( gcnew RCMrcError( 
			"�V�X�e���R�[�����C�u����", METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE, "SDK�f�t�H���g�ł��B",
			"System-Call Library", "This Library is SDK default one.", false, true ) );
	}
	return ECSrlResult::NOERROR;
} // mrcNTR()

// TWL��p
ECSrlResult RCSrl::mrcTWL( FILE *fp )
{
	System::Int32 i;

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

	if( *this->hIsOldDevEncrypt && *this->hHasDSDLPlaySign )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			"���J���p�Í��t���O", 0x1c, 0x1c,
			"����ROM�̓N���[���u�[�g�Ή��A�v���ł����A���i�p�{�̂ł̓N���[���u�[�g���ł��Ȃ��Ȃ�܂��B",
			"Old Development Flag",
			"This ROM supports Clone-Boot, and the flag is old type. Therefore, Clone-Boot can't be done.",
			false, true ) );
	}

	// �l�`�F�b�N

	fseek( fp, 0, SEEK_END );
	u32  filesize = ftell(fp);	// ���t�@�C���T�C�Y(�P��Mbit)
	u32  romsize = 1 << (this->pRomHeader->s.rom_size);	// ROM�e��
	if( *(this->hIsMediaNand) == false )		// �J�[�h�A�v���̂Ƃ��݂̂̃`�F�b�N
	{
		u32 filesizeMb = (filesize / (1024*1024)) * 8;
		if( romsize < filesizeMb )
		{
			this->hErrorList->Add( gcnew RCMrcError( 
				"�f�o�C�X�e��", 0x14, 0x14, "���t�@�C���T�C�Y�����������l���w�肳��Ă��܂��B",
				"Device Capacity", "Setting data is less than the actual file size.", false, true ) );
		}
		else if( filesizeMb < romsize )
		{
			this->hWarnList->Add( gcnew RCMrcError(		// �x��
				"�f�o�C�X�e��", 0x14, 0x14, "���t�@�C���T�C�Y�ɔ�ׂĖ��ʂ̂���l���ݒ肳��Ă��܂��B",
				"Device Capacity", "Setting data is larger than the actual file size.", false, true ) );
		}
		if( (romsize < METWL_ROMSIZE_MIN) || (METWL_ROMSIZE_MAX < romsize) )
		{
			this->hErrorList->Add( gcnew RCMrcError( 
				"�f�o�C�X�e��", 0x14, 0x14, "�w��\�ȗe�ʂł͂���܂���B",
				"Device Capacity", "Invalid capacity.", false, true ) );
		}
		if( (filesizeMb < 1) || (MasterEditorTWL::countBits(filesizeMb) != 1) )
		{
			this->hWarnList->Add( gcnew RCMrcError( 
				"���t�@�C���T�C�Y", METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE, "���r���[�Ȓl�ł��B�ʏ�ł�2�ׂ̂���[Mbit]�̒l�ł��B",
				"Actual File Size", "Invalid size. This size is usually power of 2.", false, true ) );
		}
		// 1Gbit�ȏ�̂Ƃ��̍ŏI�̈悪�Œ�l���ǂ���
		this->mrcPadding( fp );

		// �Z�O�����g3��CRC
		u16  crcseg3;
		BOOL crcret = getSeg3CRCInFp( fp, &crcseg3 );
		if( !crcret || (crcseg3 != METWL_SEG3_CRC) )
		{
			this->hErrorList->Add( gcnew RCMrcError( 
				"�Z�O�����g3 CRC", METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE, 
				"�Z�O�����g3�̈�Ɍ�肪����܂��B",
				"Segment-3 CRC", "Invalid data exist in Segment-3 area.", false, true ) );
		}

		u16  NA = this->pRomHeader->s.twl_card_normal_area_rom_offset & 0x7fffUL;
		u16  KA = this->pRomHeader->s.twl_card_keytable_area_rom_offset;
		if( (NA == 0) || (KA == 0) || (NA > KA) )
		{
			this->hErrorList->Add( gcnew RCMrcError( 
				"TWL ROM�I�t�Z�b�g", 0x90, 0x93,
				"TWL�m�[�}���̈�ROM�I�t�Z�b�g(NA)�����TWL��p�̈�ROM�I�t�Z�b�g(KA)�͂Ƃ���1�ȏ�ŁA����NA��KA�𒴂��Ă͂����܂���B",
				"TWL ROM Offset", 
				"Both TWL Normal Area ROM Offset(NA) and TWL Limited Area ROM Offset(KA) are bigger than 0. And NA must be smaller than KA, or equals to KA.",
				false, true ) );
		}
	} //if( *(this->hIsNAND) == false )
	else	// NAND�A�v���̂Ƃ��݂̂̃`�F�b�N
	{
		if( (romsize < METWL_ROMSIZE_MIN_NAND) || (METWL_ROMSIZE_MAX_NAND < romsize) )
		{
			this->hErrorList->Add( gcnew RCMrcError( 
				"�f�o�C�X�e��", 0x14, 0x14, "NAND�A�v���ɑ΂��Ďw��\�ȗe�ʂł͂���܂���B",
				"Device Capacity", "Invalid capacity.", false, true ) );
		}
		u32  allsizeMB = filesize + this->pRomHeader->s.public_save_data_size + this->pRomHeader->s.private_save_data_size;
		if( allsizeMB > METWL_ALLSIZE_MAX_NAND )
		{
			this->hErrorList->Add( gcnew RCMrcError( 
				"���t�@�C���T�C�Y", METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE,
				"ROM�f�[�^�̎��t�@�C���T�C�Y��Public�Z�[�u�f�[�^�����Private�Z�[�u�f�[�^�̃T�C�Y�̑��a��32MByte�𒴂��Ă��܂��B",
				"Actual File Size", 
				"The sum of this size, the public save data size and private save data size exceed 32MByte.", false, true ) );
		}
	}

	if( *this->hIsAppLauncher || *this->hIsAppSecure || *this->hIsAppSystem )
	{
		if( this->pRomHeader->s.disable_debug == 0 )
		{
			this->hWarnList->Add( gcnew RCMrcError( 
				"�f�o�b�K����֎~�t���O", 0x1f, 0x1f, 
				"�f�o�b�K��ŉ�͉\�Ȑݒ�ɂȂ��Ă��܂��B�f�o�b�K�����\�t�g�Ƃ���ROM�������[�X����ꍇ�ɂ̓Z�L�����e�B��̖�肪�Ȃ������m�F���������B",
				"Disable Launch on Debugger Flag", 
				"This ROM can be analyzed on the debugger. If the ROM is released for the debugger, please check the security settings.", 
				false, true ) );
		}
	}

	if( this->pRomHeader->s.warning_no_spec_rom_speed != 0 )
	{
		this->hWarnList->Add( gcnew RCMrcError( 
			"���t���O", 0x1f, 0x1f, "rsf�t�@�C����ROMSpeedType���ݒ肳��Ă��܂���B",
			"Setting Flags", "In a RSF file, the item \"ROMSpeedType\" is not set.", false, true ) );
	}
	if( (this->pRomHeader->s.game_cmd_param & CARD_LATENCY_MASK) != CARD_1TROM_GAME_LATENCY )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			"ROM�R���g���[�����", 0x60, 0x67, "TWL�ł̓}�X�NROM�͗p�ӂ���Ă��܂���B�����^�C��PROM�ݒ�ɂ��Ă��������B",
			"ROM Control Info.", "Mask ROM can be set. Please set One-time PROM.", false, true ) );
	}

	if( *this->hIsMediaNand == false )
	{
		if( (this->pRomHeader->s.enable_aes == 0) || (this->pRomHeader->s.aes_target_size == 0) )
		{
			this->hErrorList->Add( gcnew RCMrcError( 
				"AES�Í�", 0x60, 0x67, "AES�Í��������ɂȂ��Ă��܂��B�Z�L�����e�B��̖�肪����܂��B",
				"AES Encryption", "AES Encryption is disable. It is a security problem.", false, true ) );
		}
	}
	else		// NAND�A�v��
	{
		if( (this->pRomHeader->s.enable_aes == 0) || (this->pRomHeader->s.aes_target_size == 0) || (this->pRomHeader->s.aes_target2_size == 0) )
		{
			this->hErrorList->Add( gcnew RCMrcError( 
				"AES�Í�", 0x60, 0x67, "AES�Í��������ɂȂ��Ă��܂��B�Z�L�����e�B��̖�肪����܂��B",
				"AES Encryption", "AES Encryption is disable. It is a security problem.", false, true ) );
		}
	}

	// ROM�w�b�_�̃`�F�b�N (TWL��p�̈�)

	// �l�`�F�b�N
	{
		u32  idH;
		u8   idL[4];
		int  apptype;
		const int appUser     = 0;
		const int appSystem   = 1;
		const int appSecure   = 2;
		const int appLauncher = 3;
		System::String ^appstrJ = gcnew System::String("");
		System::String ^appstrE = gcnew System::String("");

		idH = this->pRomHeader->s.titleID_Hi;
		memcpy( idL, &(this->pRomHeader->s.titleID_Lo[0]), 4 );

		if( (idL[3]=='H') && (idL[2]=='N') && (idL[1]=='A') )   // �����`���A�v�����ǂ�����TitleID_Lo�̒l�Ō���
		{
			apptype = appLauncher;
			appstrJ = "�����`���[�A�v��";
			appstrE = "Launcher-App.";
		}
		else if( idH & TITLE_ID_HI_SECURE_FLAG_MASK )           // �����Ă���Z�L���A�A�v��
		{
			apptype = appSecure;
			appstrJ = "�Z�L���A�A�v��";
			appstrE = "Secure-App.";
		}
		else if( (idH & TITLE_ID_HI_APP_TYPE_MASK) == 1 )       // �����Ă���V�X�e���A�v��
		{
			apptype = appSystem;
			appstrJ = "�V�X�e���A�v��";
			appstrE = "System-App.";
		}
		else if( (idH & TITLE_ID_HI_APP_TYPE_MASK) == 0 )       // �c��̓��[�U�A�v��
		{
			apptype = appUser;
			appstrJ = "���[�U�A�v��";
			appstrE = "User-App.";
		}
		else
		{
			apptype = -1;
		}

		if( apptype < 0 )
		{
			this->hWarnList->Add( gcnew RCMrcError( 
				"�A�v�����", 0x230, 0x237, "�s���Ȓl�ł��B",
				"Application Type", "Illigal type.", false, true ) );
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
			this->hWarnList->Add( gcnew RCMrcError( 
				"�A�v�����", 0x230, 0x237, "����ROM�f�[�^��" + appstrJ + "�ł��B�{�v���O�����ł͔�Ή��ł��B",
				"Application Type", "This ROM is " + appstrE + " which is unsurpported by this editor.", false, true ) );
		}

	} // �A�v����ʂ̃`�F�b�N

	if( (this->pRomHeader->s.access_control.game_card_on != 0) &&
		(this->pRomHeader->s.access_control.game_card_nitro_mode != 0) )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			"�A�N�Z�X�R���g���[�����", 0x1b4, 0x1b7, "�Q�[���J�[�h�d���ݒ�Ƀm�[�}�����[�h��NTR���[�h�̗�����ݒ肷�邱�Ƃ͂ł��܂���B",
			"Access Control Info.", "Game card power setting is either normal mode or NTR mode.", false, true ) );
	}
	if( ((this->pRomHeader->s.titleID_Hi & TITLE_ID_HI_MEDIA_MASK) == 0) &&		// �J�[�h�A�v��
		((this->pRomHeader->s.access_control.game_card_on != 0) || (this->pRomHeader->s.access_control.game_card_nitro_mode != 0)) )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			"�A�N�Z�X�R���g���[�����", 0x1b4, 0x1b7, "�Q�[���J�[�h�p�\�t�g�ɑ΂��ăQ�[���J�[�h�d���ݒ�����邱�Ƃ͂ł��܂���B",
			"Access Control Info.", "Game card power setting is not for Game Card Soft.", false, true ) );
	}
	if( this->pRomHeader->s.access_control.shared2_file == 0 )
	{
		if( (this->pRomHeader->s.shared2_file0_size != 0) || (this->pRomHeader->s.shared2_file1_size != 0) ||
			(this->pRomHeader->s.shared2_file2_size != 0) || (this->pRomHeader->s.shared2_file3_size != 0) ||
			(this->pRomHeader->s.shared2_file4_size != 0) || (this->pRomHeader->s.shared2_file5_size != 0) )
		{
			this->hErrorList->Add( gcnew RCMrcError( 
				"�A�N�Z�X�R���g���[�����", 0x1b4, 0x1b7, "Shared2�t�@�C���̃T�C�Y���ݒ肳��Ă���ɂ�������炸�s�g�p�ݒ�ɂȂ��Ă��܂��B",
				"Access Control Info.", "Sizes of shared2 files is setting, but using them is not enabled.", false, true ) );
		}
	}
	if( *this->hIsMediaNand == false )	// �J�[�h�A�v���̂Ƃ��̂�
	{
		if( (this->pRomHeader->s.access_control.nand_access != 0) || (this->pRomHeader->s.access_control.sd_card_access != 0) )
		{
			this->hWarnList->Add( gcnew RCMrcError( 
				"�A�N�Z�X�R���g���[�����", 0x1b4, 0x1b7,
				"�Q�[���J�[�h�p�\�t�g�́ANAND�t���b�V����������SD�J�[�h�փA�N�Z�X�ł��܂���B�A�N�Z�X����]�����ꍇ�A���Б����ɂ����k���������B",
				"Access Control Info.",
				"Game soft for Game Card does'nt access to NAND frash memory and SD Card. If the soft wish to access them, please contact us.",
				false, true ) );
		}
	}

	if( (*this->hIsWiFiIcon == true) && (*this->hIsWirelessIcon == true) )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			"�A�C�R���\���t���O", 0x1bf, 0x1bf, 
			"���C�����X�ʐM�A�C�R����Wi-Fi�ʐM�A�C�R���͓����ɕ\���ł��܂���B�ݒ�𖳎����ēǂݍ��݂܂��B",
			"Icon Displaying",
			"Icon displayed on menu is either Wireless Icon or Wi-Fi Icon. This setting was ignored in reading.",
			true, true ) );		// �C���\
	}

	if( *this->hIsMediaNand == false )	// �J�[�h�A�v���̂Ƃ��̂�
	{
		u32 ideal  = (this->pRomHeader->s.twl_card_keytable_area_rom_offset * 0x80000) + 0x3000;	// TWL KeyTable�̈�̊J�n + KeyTable�T�C�Y
		u32 actual = this->pRomHeader->s.main_ltd_rom_offset;
		if( ideal > actual )
		{
			this->hErrorList->Add( gcnew RCMrcError( 
				"�g���풓���W���[��", 0x1c0, 0x1c3,
				"TWL�g���풓���W���[���̔z�u�ɖ�肪����܂��B���Б����ɂ��A�����������B",
				"Extended Static Module",
				"Alignment of TWL Extended Static Module is illegal. Please contact us, sorry.",
				false, true ) );
		}
	}

	if( (*this->hIsAppLauncher == false) && (*this->hIsSCFGAccess == true) )
	{
		this->hWarnList->Add( gcnew RCMrcError( 
			"SCFG�ݒ�", 0x1b8, 0x1bb, "SCFG���W�X�^�փA�N�Z�X�\�ɂȂ��Ă��܂��B",
			"SCFG Setting", "In this setting, SCFG register is accessible.", false, true ) );
	}

	if( (this->pRomHeader->s.titleID_Lo[0] != this->pRomHeader->s.game_code[3]) ||
		(this->pRomHeader->s.titleID_Lo[1] != this->pRomHeader->s.game_code[2]) ||
		(this->pRomHeader->s.titleID_Lo[2] != this->pRomHeader->s.game_code[1]) ||
		(this->pRomHeader->s.titleID_Lo[3] != this->pRomHeader->s.game_code[0]) )
	{
		this->hWarnList->Add( gcnew RCMrcError( 
			"�^�C�g��ID", 0x230, 0x233, "����4�o�C�g���C�j�V�����R�[�h�ƈ�v���܂���B",
			"Title ID", "Lower 4 bytes don't match ones of Game Code.", false, true ) );
	}

	// ROM�w�b�_�ȊO�̗̈�̃`�F�b�N

	this->mrcBanner( fp );
	this->mrcReservedArea(fp);

	// �ǉ��`�F�b�N
	if( *(this->hMrcSpecialList->hIsCheck) == true )
	{
		// SDK�o�[�W����
		System::Boolean match = true;
		for each( RCSDKVersion ^sdk in this->hSDKList )
		{
			if( sdk->IsStatic && (sdk->Code != *(this->hMrcSpecialList->hSDKVer) ) )
			{
				match = false;
			}
		}
		if( !match )
		{
			this->hWarnList->Add( gcnew RCMrcError( 
				"SDK�o�[�W����", METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE, "�{�v���O�����ɓo�^����Ă���o�[�W�������ƈ�v���܂���B",
				"SDK Version", "The data doesn't match one registered in this program.", false, true ) );
		}

		// Shared2�t�@�C���T�C�Y
		for( i=0; i < METWL_NUMOF_SHARED2FILES; i++ )
		{
			if( *(this->hShared2SizeArray[i]) > 0 )	// 0�͖��g�p��\���̂Ń`�F�b�N���Ȃ�
			{
				if( *(this->hShared2SizeArray[i]) != *(this->hMrcSpecialList->hShared2SizeArray[i]) )
				{
					this->hWarnList->Add( gcnew RCMrcError( 
						"Shared2�t�@�C��" + i.ToString(), METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE, 
						"�{�v���O�����ɓo�^����Ă���t�@�C���T�C�Y���ƈ�v���܂���B",
						"Shared2 file" + i.ToString(), "The size doesn't match one registered in this program.", false, true ) );
				}
			}
		}
	} //if( *(this->hMrcSpecialList->hIsCheck) )

	return ECSrlResult::NOERROR;
} // mrcTWL()

// �p�f�B���O�̃`�F�b�N
void RCSrl::mrcPadding(FILE *fp)
{
	u32  romsize = 1 << (this->pRomHeader->s.rom_size);	// ROM�e��
	u32  padsize = 0;	// �P�ʂ�Byte
	System::String ^padstrJ = gcnew System::String("");
	System::String ^padstrE = gcnew System::String("");
	System::String ^romstr  = gcnew System::String("");

	switch( romsize )
	{
		case 1024:
			padsize = 0x00280000;	// 20Mbit = 2.5MB
			padstrJ = "�ŏI20Mbit";
			padstrE = "Last 20Mbit";
			romstr  = "1Gbit";
		break;
		case 2048:
			padsize = 0x00500000;	// 40Mbit = 5MB
			padstrJ = "�ŏI40Mbit";
			padstrE = "Last 40Mbit";
			romstr  = "2Gbit";
		break;
		case 4096:
			padsize = 0x00A60000;	// 83Mbit = 10.375MB
			padstrJ = "�ŏI83Mbit";
			padstrE = "Last 83Mbit";
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
		this->hErrorList->Add( gcnew RCMrcError( 
			padstrJ, METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE,
			"�ǂݏo�����Ƃ��ł��܂���ł����B",
			padstrE, "This area couldn't be read", false, true ) );
		return;
	}

	u8 *buf = new u8[ padsize ];
	fseek( fp, offset, SEEK_SET );
	if( padsize != fread( buf, 1, padsize, fp ) )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			padstrJ, METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE,
			"�ǂݏo�����Ƃ��ł��܂���ł����B",
			padstrE, "This area couldn't be read", false, true ) );
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
		this->hErrorList->Add( gcnew RCMrcError( 
			padstrJ, METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE,
			"FFh�ȊO�̒l���i�[����Ă��܂��B " + romstr + " ROM�̗e�ʐ���������Ă��܂���B",
			padstrE,
			"This area includes illegai data. " + romstr + " Volume constraint is violated.",
			false, true ) );
	}
	delete []buf;
}

// �\��̈�
void RCSrl::mrcReservedArea(FILE *fp)
{
	System::Xml::XmlDocument ^doc = gcnew System::Xml::XmlDocument();
	try
	{
		doc->Load( "../resource/ini.xml" );
	}
	catch( System::Exception ^ex )
	{
		(void)ex;
		this->hErrorList->Add( gcnew RCMrcError( 
			"�\��̈�", METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE,
			"�ݒ�t�@�C������\��̈�̃��X�g��ǂݏo�����Ƃ��ł��܂���ł����B",
			"Reserved Area",
			"The list of reserved areas is not found in the setting file.",
			false, true ) );
		return;
	}

	// �ݒ�t�@�C������\��̈�̏���ǂݏo��
	System::Xml::XmlNodeList ^list = doc->SelectNodes( "/init/reserved-list/reserved" );
	System::Collections::IEnumerator^ iter = list->GetEnumerator();
	while( iter->MoveNext() )
	{
		System::Xml::XmlNode ^area = safe_cast<System::Xml::XmlNode^>(iter->Current);
		System::Xml::XmlNode ^begin = area->SelectSingleNode( "begin" );	// ���΃p�X
		System::Xml::XmlNode ^end   = area->SelectSingleNode( "end" );	// ���΃p�X
		if( begin && begin->FirstChild && begin->FirstChild->Value && 
			end   && end->FirstChild   && end->FirstChild->Value )
		{
			System::UInt32 ibeg = System::UInt32::Parse( begin->FirstChild->Value, System::Globalization::NumberStyles::AllowHexSpecifier );
			System::UInt32 iend = System::UInt32::Parse( end->FirstChild->Value, System::Globalization::NumberStyles::AllowHexSpecifier );
			System::UInt32 size = iend - ibeg + 1;
			System::UInt32 i;
			System::Boolean bReserved = true;
			for( i=0; i < size; i++ )
			{
				u8 *p = (u8*)this->pRomHeader;
				if( p[ ibeg + i ] != 0 )
				{
					bReserved = false;
					break;
				}
			}
			if( !bReserved )
			{
				this->hErrorList->Add( gcnew RCMrcError( 
					"�\��̈�", ibeg, iend, "�s���Ȓl���܂܂�Ă��܂��B���̗̈�����ׂ�0�Ŗ��߂Ă��������B",
					"Reserved Area", "Invalid data is included. Please set 0 into this area.", false, true ) );
			}
			
		}
	}
}

// �o�i�[
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
		this->hErrorList->Add( gcnew RCMrcError( 
			"�o�i�[�t�@�C��", METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE,
			"�ǂݏo�����Ƃ��ł��܂���ł����B",
			"Banner File",
			"The file can't be read.",
			false, true ) );
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
		this->hErrorList->Add( gcnew RCMrcError( 
			"�o�i�[�t�@�C��", METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE,
			"�g�p�s�\�ȕ����Z�b�g���g�p����Ă��܂��B",
			"Banner File",
			"A set of illegal charactor code is used.",
			false, true ) );
	}
	delete []banner;
}

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
