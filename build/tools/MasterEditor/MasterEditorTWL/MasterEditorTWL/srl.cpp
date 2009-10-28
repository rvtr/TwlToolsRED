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

// ****************************************************************
// RCSrl �N���X
// ****************************************************************

// constructor
RCSrl::RCSrl( System::String ^msgfileJ, System::String ^msgfileE )
{
	this->pRomHeader = new (ROM_Header);
	std::memset( pRomHeader, 0, sizeof(ROM_Header) );
	this->hMrcExternalCheckItems = gcnew RCMrcExternalCheckItems();
	this->hMrcMsg = gcnew RCMessageBank( msgfileJ, msgfileE );
}

// destructor
RCSrl::~RCSrl()
{
	this->!RCSrl();
}

// finalizer
RCSrl::!RCSrl()
{
	// destructor�͖����I��delete����Ȃ��ƌĂ΂�Ȃ�(GC����͌Ă΂�Ȃ�)�悤�Ȃ̂�
	// GC�ɉ�����Ă�������Ƃ��Ɏ������������ɂ� finalizer ���K�v

	// �|�C���^��(���Ԃ�) unmanaged �Ȃ̂Ŏ���I�ɉ������
	delete (this->pRomHeader);
}

// -------------------------------------------------------------------
// ROM�w�b�_���t�@�C�����烊�[�h
//
// @arg [in] ���̓t�@�C����
// -------------------------------------------------------------------
ECSrlResult RCSrl::readFromFile( System::String ^srlfile )
{
	FILE       *fp = NULL;
	const char *pchFilename = 
		(const char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( srlfile ).ToPointer();
	ECSrlResult r;

	// �t�@�C�����J����ROM�w�b�_�̂ݓǂݏo��
	if( fopen_s( &fp, pchFilename, "rb" ) != NULL )
	{
		return (ECSrlResult::ERROR_FILE_OPEN);
	}
	(void)fseek( fp, 0, SEEK_SET );		// ROM�w�b�_��srl�̐擪����

	// 1�o�C�g��sizeof(~)�������[�h (�t���ƕԂ�l��sizeof(~)�ɂȂ�Ȃ��̂Œ���)
	ROM_Header tmprh;
	if( fread( (void*)&tmprh, 1, sizeof(ROM_Header), fp ) != sizeof(ROM_Header) )
	{
		return (ECSrlResult::ERROR_FILE_READ);
	}

#ifdef METWL_WHETHER_SIGN_DECRYPT
	// �܂������`�F�b�N
	r = this->decryptRomHeader( &tmprh );
	if( r != ECSrlResult::NOERROR )
	{
		(void)fclose(fp);
		return r;
	}
#endif //#ifdef METWL_WHETHER_SIGN_DECRYPT

#ifdef METWL_WHETHER_PLATFORM_CHECK
	// �v���b�g�t�H�[���̃`�F�b�N
	if( ((tmprh.s.platform_code == PLATFORM_CODE_NTR) && (tmprh.s.enable_signature != 0)) ||	// PictoChat/DS-download-play �݂̂���ɊY��
		(tmprh.s.platform_code == PLATFORM_CODE_TWL_HYBLID) ||
		(tmprh.s.platform_code == PLATFORM_CODE_TWL_LIMITED) )
	{
		// OK
	}
	else
	{
		return ECSrlResult::ERROR_PLATFORM;
	}
#endif

	// �����`�F�b�N��ʂ��Ă���t�B�[���h��ROM�w�b�_�ɃR�s�[
	// (�������Ȃ��ƕs��SRL��ǂݍ��񂾂Ƃ���ROM�w�b�_���㏑������Ă��܂�)
	memcpy( this->pRomHeader, &tmprh, sizeof(ROM_Header) );

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

	// �G���[���X�g���N���A
	this->hErrorList = gcnew System::Collections::Generic::List<RCMrcError^>;
	this->hWarnList  = gcnew System::Collections::Generic::List<RCMrcError^>;
	this->hParentalErrorList = gcnew System::Collections::Generic::List<RCMrcError^>;
	this->hParentalWarnList  = gcnew System::Collections::Generic::List<RCMrcError^>;
	this->hErrorList->Clear();
	this->hWarnList->Clear();
	this->hParentalErrorList->Clear();
	this->hParentalWarnList->Clear();

	// ROM�w�b�_�̒l��ROM�ŗL���t�B�[���h�ɔ��f������
	(void)this->setRomInfo();
	(void)this->calcNandUsedSize( fp );	// NAND����T�C�Y

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

// -------------------------------------------------------------------
// ROM�w�b�_���t�@�C���Ƀ��C�g
//
// @arg [in] �o�̓t�@�C����
// -------------------------------------------------------------------
ECSrlResult RCSrl::writeToFile( System::String ^srlfile )
{
	FILE       *fp = NULL;
	const char *pchFilename = 
		(const char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( srlfile ).ToPointer();
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

// -------------------------------------------------------------------
// ROM�w�b�_����擾����ROM�ŗL�����t�B�[���h�ɔ��f������
// -------------------------------------------------------------------
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
	this->RomVersion = this->pRomHeader->s.rom_version;
	this->HeaderCRC  = this->pRomHeader->s.header_crc16;
	this->IsOldDevEncrypt = (this->pRomHeader->s.developer_encrypt_old != 0)?true:false; 

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
	this->IsNormalJump = (this->pRomHeader->s.permit_landing_normal_jump != 0)?true:false;
	this->IsTmpJump    = (this->pRomHeader->s.permit_landing_tmp_jump    != 0)?true:false;
	this->NormalRomOffset   = (u32)(this->pRomHeader->s.twl_card_normal_area_rom_offset)   * 0x80000;
	this->KeyTableRomOffset = (u32)(this->pRomHeader->s.twl_card_keytable_area_rom_offset) * 0x80000;
	this->PublicSize  = this->pRomHeader->s.public_save_data_size;
	this->PrivateSize = this->pRomHeader->s.private_save_data_size;

	u8  *idL = this->pRomHeader->s.titleID_Lo;
	u32  idH = this->pRomHeader->s.titleID_Hi;
	//u32  val;
	//val = ((u32)(idL[0]) << 24) | ((u32)(idL[1]) << 16) | ((u32)(idL[2]) << 8) | ((u32)(idL[3]));	// �r�b�O�G���f�B�A��
	//this->hTitleIDLo   = gcnew System::UInt32( val );
	this->hTitleIDLo  = gcnew System::String( (char*)idL, 0, 4, utf8 );
	this->TitleIDHi   = idH;

	// TitleID����킩����
	this->IsAppLauncher = false;
	this->IsAppUser     = false;
	this->IsAppSystem   = false;
	this->IsAppSecure   = false;
	this->IsLaunch      = false;
	this->IsMediaNand   = false;
	this->IsDataOnly    = false;
	if( (idL[3]=='H') && (idL[2]=='N') && (idL[1]=='A') )	// �����`���A�v�����ǂ�����TitleID_Lo�̒l�Ō���
    {
		this->IsAppLauncher = true;
    }
    else if( idH & TITLE_ID_HI_SECURE_FLAG_MASK )				// �e�r�b�g�͔r���I�Ƃ͌���Ȃ��̂�else if�ɂ͂Ȃ�Ȃ�
    {
		this->IsAppSecure = true;
    }
    else if( (idH & TITLE_ID_HI_APP_TYPE_MASK) == 1 )
    {
		this->IsAppSystem = true;
    }
    else if( (idH & TITLE_ID_HI_APP_TYPE_MASK) == 0 )
    {
		this->IsAppUser = true;
    }
	if( idH & TITLE_ID_HI_DATA_ONLY_FLAG_MASK )
	{
		this->IsDataOnly = true;
	}
	if( idH & TITLE_ID_HI_MEDIA_MASK )
	{
		this->IsMediaNand = true;
	}
	if( (idH & TITLE_ID_HI_NOT_LAUNCH_FLAG_MASK) == 0 )		// �r�b�g��0�̂Ƃ�Launch
	{
		this->IsLaunch = true;
	}
	u16 pub = (u16)((idH & TITLE_ID_HI_PUBLISHER_CODE_MASK) >> TITLE_ID_HI_PUBLISHER_CODE_SHIFT);
	this->PublisherCode = pub;

	// TWL�g���t���O
	this->IsCodecTWL  = (this->pRomHeader->s.exFlags.codec_mode != 0)?true:false;
	this->IsEULA      = (this->pRomHeader->s.exFlags.agree_EULA != 0)?true:false;
	this->IsSubBanner = (this->pRomHeader->s.exFlags.availableSubBannerFile != 0)?true:false;
	this->IsWiFiIcon  = (this->pRomHeader->s.exFlags.WiFiConnectionIcon != 0)?true:false;
	this->IsWirelessIcon = (this->pRomHeader->s.exFlags.DSWirelessIcon != 0)?true:false;
	this->IsWL        = (this->pRomHeader->s.exFlags.enable_nitro_whitelist_signature != 0)?true:false;

	// TWL�A�N�Z�X�R���g���[��
	this->IsCommonClientKey = (this->pRomHeader->s.access_control.common_client_key != 0)?true:false;
	this->IsAesSlotBForES   = (this->pRomHeader->s.access_control.hw_aes_slot_B != 0)?true:false;
	this->IsAesSlotCForNAM  = (this->pRomHeader->s.access_control.hw_aes_slot_C != 0)?true:false;
	this->IsSD              = (this->pRomHeader->s.access_control.sd_card_access != 0)?true:false;
	this->IsNAND            = (this->pRomHeader->s.access_control.nand_access != 0)?true:false;
	this->IsGameCardOn      = (this->pRomHeader->s.access_control.game_card_on != 0)?true:false;
	this->IsShared2         = (this->pRomHeader->s.access_control.shared2_file != 0)?true:false;
	this->IsAesSlotBForJpegEnc = (this->pRomHeader->s.access_control.hw_aes_slot_B_SignJPEGForLauncher != 0)?true:false;
	this->IsAesSlotBForJpegEncUser = (this->pRomHeader->s.access_control.hw_aes_slot_B_SignJPEGForUser != 0)?true:false;
	this->IsGameCardNitro   = (this->pRomHeader->s.access_control.game_card_nitro_mode != 0)?true:false;
	this->IsAesSlotAForSSL  = (this->pRomHeader->s.access_control.hw_aes_slot_A_SSLClientCert != 0)?true:false;
	this->IsCommonClientKeyForDebugger 
		= (this->pRomHeader->s.access_control.common_client_key_for_debugger_sysmenu != 0)?true:false;
	this->IsPhotoWrite      = (this->pRomHeader->s.access_control.photo_access_write != 0)?true:false;
	this->IsPhotoRead       = (this->pRomHeader->s.access_control.photo_access_read  != 0)?true:false;
	this->IsSDWrite         = (this->pRomHeader->s.access_control.sdmc_access_write  != 0)?true:false;
	this->IsSDRead          = (this->pRomHeader->s.access_control.sdmc_access_read   != 0)?true:false;

	// ���ؐݒ�t���O
	this->IsForChina = (this->pRomHeader->s.for_china != 0)?true:false;
	this->IsForKorea = (this->pRomHeader->s.for_korea != 0)?true:false;

	// 5.2 RELEASE�ȍ~�̂Ƃ�SD�A�N�Z�X���𒲂ׂ�K�v����
	u32 sdkver = 0;
	for each ( RCSDKVersion ^sdk in this->hSDKList )
	{
		if( sdk->IsStatic )
		{
			sdkver = sdk->Code;
		}
	}
	this->IsOldSDK52Release = MasterEditorTWL::IsOldSDKVersion(sdkver, METWL_SDK52_RELEASE, true);	// PR/RC�łł��G���[���o��
	this->IsOldSDK51PR      = MasterEditorTWL::IsOldSDKVersion(sdkver, METWL_SDK51_PR, true);

	// SCFG �����b�N����邩
	if( (this->pRomHeader->s.arm7_scfg_ext >> 31) != 0 )
	{
		this->IsSCFGAccess = true;
	}
	else
	{
		this->IsSCFGAccess = false;
	}

	// Shared2�t�@�C���T�C�Y
	this->hShared2SizeArray = gcnew cli::array<System::UInt32>(METWL_NUMOF_SHARED2FILES);
	for( i=0; i < METWL_NUMOF_SHARED2FILES; i++ )
	{
		this->hShared2SizeArray[i] = 0;
	}
	u32  unit = 16 * 1024;		// 16KB�̏搔���i�[����Ă���
	if( this->pRomHeader->s.shared2_file0_size != 0 )
	{
		this->hShared2SizeArray[0] = (this->pRomHeader->s.shared2_file0_size * unit) + unit;
	}
	if( this->pRomHeader->s.shared2_file1_size != 0 )
	{
		this->hShared2SizeArray[1] = (this->pRomHeader->s.shared2_file1_size * unit) + unit;
	}
	if( this->pRomHeader->s.shared2_file2_size != 0 )
	{
		this->hShared2SizeArray[2] = (this->pRomHeader->s.shared2_file2_size * unit) + unit;
	}
	if( this->pRomHeader->s.shared2_file3_size != 0 )
	{
		this->hShared2SizeArray[3] = (this->pRomHeader->s.shared2_file3_size * unit) + unit;
	}
	if( this->pRomHeader->s.shared2_file4_size != 0 )
	{
		this->hShared2SizeArray[4] = (this->pRomHeader->s.shared2_file4_size * unit) + unit;
	}
	if( this->pRomHeader->s.shared2_file5_size != 0 )
	{
		this->hShared2SizeArray[5] = (this->pRomHeader->s.shared2_file5_size * unit) + unit;
	}

	// �y�A�����^���R���g���[�����̎擾

	const u32  region = this->pRomHeader->s.card_region_bitmap;

	// ���ׂĂ̒c�̂��u�s�v�ɏ�����
	this->hArrayParentalIndex = gcnew cli::array<int>(PARENTAL_CONTROL_INFO_SIZE);
	for( i=0; i < PARENTAL_CONTROL_INFO_SIZE; i++ )
	{
		this->hArrayParentalIndex[i] = -1;
	}
	this->IsUnnecessaryRating = false;	// ���[�W�������s���̂Ƃ��ɐݒ肳��Ȃ��̂ŏ��������Ă���

	// ���[�W�����ƃ��[�e�B���O���擾
	if( this->setRegionInfo( region ) )
	{
		this->setUnnecessaryRatingInfo( region );	// ���[�e�B���O�\�����s�v���ǂ����𒲂ׂ�(���ꂪ�Ȃ��ƑS�N��Ƌ�ʂł��Ȃ�)
		if( !this->IsUnnecessaryRating )
		{
			this->setRatingInfo( region );			// ���[�W�����Ɋ܂܂��c�̂̃��[�e�B���O�����擾
		}

		// �����łɌ���S�c�̂̃��[�e�B���O���S�N��łȂ���΂Ȃ�Ȃ�
		if( region == METWL_MASK_REGION_CHINA )
		{
			bool all_free = true;
			for( i=0; i < PARENTAL_CONTROL_INFO_SIZE; i++ )
			{
				if( this->pRomHeader->s.parental_control_rating_info[i] != (OS_TWL_PCTL_OGNINFO_ENABLE_MASK | 0) )
				{
					all_free = false;
				}
			}
			if( !all_free )
			{
				this->hParentalWarnList->Add( this->makeMrcError("ChinaAllRatingFree") );
			}
		}
	}

	return ECSrlResult::NOERROR;
} // ECSrlResult RCSrl::setRomInfo(void)

// -------------------------------------------------------------------
// ROM�w�b�_���̃��[�W���������t�B�[���h�ɔ��f������
// -------------------------------------------------------------------
bool RCSrl::setRegionInfo( u32 region )
{
	this->IsRegionJapan     = ((region & METWL_MASK_REGION_JAPAN)     != 0)?true:false;
	this->IsRegionAmerica   = ((region & METWL_MASK_REGION_AMERICA)   != 0)?true:false;
	this->IsRegionEurope    = ((region & METWL_MASK_REGION_EUROPE)    != 0)?true:false;
	this->IsRegionAustralia = ((region & METWL_MASK_REGION_AUSTRALIA) != 0)?true:false;
	this->IsRegionKorea     = ((region & METWL_MASK_REGION_KOREA)     != 0)?true:false;
	this->IsRegionChina     = ((region & METWL_MASK_REGION_CHINA)     != 0)?true:false;

	// ���[�W�����Ɋ܂܂��c�̂��Ȃ������烊�[�W�����͕s��
	if( (MasterEditorTWL::getOgnListInRegion( region ) == nullptr) && (region != METWL_MASK_REGION_CHINA) )	// �����͗�O
	{
		this->hParentalErrorList->Add( this->makeMrcError("IllegalRegion") );
		return false;
	}
	if( !this->IsAppUser && (region == METWL_MASK_REGION_ALL) )
	{
		this->hParentalWarnList->Add( this->makeMrcError("AllRegion") );
	}
	return true;
}

// ----------------------------------------------------------------------
// ROM�w�b�_���̃��[�e�B���O�\���s�v�t���O�𒲂ׂăt�B�[���h�ɔ��f������
// ----------------------------------------------------------------------
void RCSrl::setUnnecessaryRatingInfo( u32 region )
{
	// ROM�w�b�_�̃t���O�𒲂ׂ�
	this->IsUnnecessaryRating = (this->pRomHeader->s.unnecessary_rating_display != 0)?true:false;
	if( !this->IsUnnecessaryRating )
	{
		return;		// �s�v�łȂ��Ȃ炱��ȍ~�̃`�F�b�N�͕K�v�Ȃ�(���[�e�B���O���̎擾�Ɉڂ�)
	}

	// ���[�W�����Ɋ܂܂�郌�[�e�B���O�c�̂��擾
	System::Collections::Generic::List<int> ^ognlist = MasterEditorTWL::getOgnListInRegion( region );
	if( ognlist == nullptr )
	{
		// �����̂Ƃ����[�e�B���O�c�̂͑��݂��Ȃ������[�e�B���O�\���s�v�̑I���͉\�Ȃ̂Ōx�����Ă���
		if( this->IsUnnecessaryRating )
		{
			this->hParentalWarnList->Add( this->makeMrcError("UnnecessaryRatingSetting") );
		}
		return;
	}

	// ���[�W�����Ɋ܂܂��c�̂̃��[�e�B���O���ɗ]�v�ȃf�[�^���o�^����Ă��Ȃ����`�F�b�N
	bool noerror = false;
	for each( int ogn in ognlist )
	{
		bool b = false;
		if( this->pRomHeader->s.parental_control_rating_info[ ogn ] == (OS_TWL_PCTL_OGNINFO_ENABLE_MASK | 0) )	// �S�N��������Ȃ�
		{
			b = true;
		}
		noerror = noerror | b;		// ���ׂđS�N��ƂȂ��Ă��Ȃ��Ƃ��̓G���[�Ƃ݂Ȃ�
	}
	if( !noerror )
	{
		this->IsUnnecessaryRating = false;	// �G���[�̂Ƃ���ROM�w�b�_�ɕs�v�Ɠo�^����Ă��Ă���������
		this->hParentalErrorList->Add( this->makeMrcError("UnnecessaryRatingIllegal") );
	}
	else
	{
		this->hParentalWarnList->Add( this->makeMrcError("UnnecessaryRatingSetting") );
	}
}

// -------------------------------------------------------------------
// ROM�w�b�_���̃y�A�����^���R���g���[�������t�B�[���h�ɔ��f������
// -------------------------------------------------------------------
void RCSrl::setRatingInfo( u32 region )
{
	// ���[�W�����Ɋ܂܂�Ă���c�̂����X�g�A�b�v
	// (�܂܂�Ă��Ȃ��c�̂̃��[�e�B���O��ǂݍ��܂Ȃ�)
	System::Collections::Generic::List<int> ^ognlist = MasterEditorTWL::getOgnListInRegion( region );
	if( ognlist == nullptr )	// ���X�g��nullptr�Ȃ�s��
	{
		return;
	}

	// ���[�W�����Ɋ܂܂��c�̂̃��[�e�B���O���𒲂ׂ�
	for each( int ogn in ognlist )
	{
		this->setOneRatingOrgInfo( ogn );
	}
}

// -------------------------------------------------------------------
// �c�̂̃��[�e�B���O��񂩂�R���{�{�b�N�X�̃C���f�b�N�X�����肷��
// -------------------------------------------------------------------
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
		this->hParentalErrorList->Add( this->makeMrcError("RatingUndefine", name) );
	}
	else
	{
		if( rp )	// �R����
		{
			if( age == 0 ) // �N�0�̂Ƃ��͐R�����Ƃ݂Ȃ�
			{
				index = ages->Length;	// �z��̍Ō�̗v�f�̎����u�R�����v
				this->hParentalWarnList->Add( this->makeMrcError("RatingPendingSetting", name) );
			}
			else
			{
				index = -1;		// ����ȊO�̓G���[
				this->hParentalErrorList->Add( this->makeMrcError("RatingPendingConcurrent", name) );
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
				this->hParentalErrorList->Add( this->makeMrcError("IllegalRating", name) );
			}
		}
	}
	this->hArrayParentalIndex[ ogn ] = index;
}

// -------------------------------------------------------------------
// ROM�w�b�_��ROM�ŗL���t�B�[���h�̒l�𔽉f������
// -------------------------------------------------------------------
ECSrlResult RCSrl::setRomHeader(void)
{
	ECSrlResult result;

	// ROM�w�b�_��[0,0x160)�̗̈��Read Only�ŕύX���Ȃ�

	// ���[�W����
	u32  map = 0;
	if( this->IsRegionJapan   == true )  { map |= METWL_MASK_REGION_JAPAN; }
	if( this->IsRegionAmerica == true )  { map |= METWL_MASK_REGION_AMERICA; }
	if( this->IsRegionEurope  == true )  { map |= METWL_MASK_REGION_EUROPE; }
	if( this->IsRegionAustralia == true ){ map |= METWL_MASK_REGION_AUSTRALIA; }
	if( this->IsRegionKorea == true ){ map |= METWL_MASK_REGION_KOREA; }
	if( this->IsRegionChina == true ){ map |= METWL_MASK_REGION_CHINA; }
#if defined(METWL_VER_APPTYPE_SYSTEM) || defined(METWL_VER_APPTYPE_SECURE) || defined(METWL_VER_APPTYPE_LAUNCHER)
	if( (this->IsRegionJapan == true ) && (this->IsRegionAmerica == true)
		&& (this->IsRegionEurope == true ) && (this->IsRegionAustralia == true)
		&& (this->IsRegionKorea == true) && (this->IsRegionChina == true)
	  )
	{
		map |= METWL_MASK_REGION_ALL;	// �I�[�����[�W����������
	}
#endif
	this->pRomHeader->s.card_region_bitmap = map;

	// ���[�e�B���O
	this->setRatingRomHeader( map );

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
void RCSrl::setRatingRomHeader( u32 region )
{
	// ���[�W�����Ɋ܂܂�Ȃ��c�̂͂��ׂāu�s�v(����`)
	int j;
	for( j=0; j < PARENTAL_CONTROL_INFO_SIZE; j++ )
	{
		this->pRomHeader->s.parental_control_rating_info[j] = 0x00;
	}

	// �u���[�e�B���O�\���s�v�v�t���O�𗧂Ă�
	this->pRomHeader->s.unnecessary_rating_display = (this->IsUnnecessaryRating == true)?1:0;

	// �������[�W��������уI�[�����[�W�����̂Ƃ��\��̈�����ׂāu�S�N��v(0x80)�Ŗ��߂Ă���
	// �u���[�e�B���O�\���s�v�v���ǂ����ɂ�����炸���߂�
	if( this->IsRegionChina )	// �I�[�����[�W�����̂Ƃ��������r�b�g�͗���
	{
		for( j=0; j < PARENTAL_CONTROL_INFO_SIZE; j++ )
		{
			this->pRomHeader->s.parental_control_rating_info[j] = OS_TWL_PCTL_OGNINFO_ENABLE_MASK;
		}
	}

	// ���[�W�����Ɋ܂܂��c�݂̂̂�ݒ�
	System::Collections::Generic::List<int> ^ognlist = MasterEditorTWL::getOgnListInRegion( region );
	if( !ognlist )
	{
		return;		// �����̂Ƃ����X�g�͋�ɂȂ�
	}
	for each( int ogn in ognlist )
	{
		u8 rating = 0;
		if( this->IsUnnecessaryRating )
		{
			rating = OS_TWL_PCTL_OGNINFO_ENABLE_MASK | 0;	// ���[�e�B���O�\�����s�v�̂Ƃ��́u�S�N��v�Ɠ����l�ɂ���
		}
		else
		{
			cli::array<System::Byte> ^ages = MasterEditorTWL::getOgnRatingAges( ogn );	// �ݒ�\�N��X�g���擾

			if( this->hArrayParentalIndex[ ogn ] < 0 )		// ����`
			{
				rating = 0x00;
			}
			else
			{
				int index = this->hArrayParentalIndex[ ogn ];
				if( index == ages->Length )				// �R����
				{
					rating = OS_TWL_PCTL_OGNINFO_ENABLE_MASK | OS_TWL_PCTL_OGNINFO_ALWAYS_MASK;
				}
				else									// ���[�e�B���O�N���ݒ�
				{
					rating = OS_TWL_PCTL_OGNINFO_ENABLE_MASK | ages[ index ];
				}
			}
		}
		this->pRomHeader->s.parental_control_rating_info[ ogn ] = rating;
	}
}

// -------------------------------------------------------------------
// �w�b�_��CRC���Z�o
// -------------------------------------------------------------------
u16 calcCRC( const u16 start, const u8 *data, const int allsize )
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

// -------------------------------------------------------------------
// ROM�w�b�_���ď���
// -------------------------------------------------------------------
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
	if( this->IsAppLauncher )
	{
		privateKey = (u8*)g_devPrivKey_DER_launcher;
		publicKey  = (u8*)g_devPubKey_DER_launcher;
	}
	else
#endif //METWL_VER_APPTYPE_LAUNCHER
#ifdef METWL_VER_APPTYPE_SECURE
	if( this->IsAppSecure )
	{
		privateKey = (u8*)g_devPrivKey_DER_secure;
		publicKey  = (u8*)g_devPubKey_DER_secure;
	}
	else
#endif //METWL_VER_APPTYPE_SECURE
#ifdef METWL_VER_APPTYPE_SYSTEM
	if( this->IsAppSystem )
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

// -------------------------------------------------------------------
// ROM�w�b�_�̏������O��
// -------------------------------------------------------------------
ECSrlResult RCSrl::decryptRomHeader( ROM_Header *prh )
{
	u8     original[ RSA_KEY_LENGTH ];	// �����O������̃f�[�^�i�[��
	s32    pos = 0;						// �u���b�N�̐擪�A�h���X
	u8     digest[ DIGEST_SIZE_SHA1 ];	// ROM�w�b�_�̃_�C�W�F�X�g
	u8    *publicKey = (u8*)g_devPubKey_DER;

	// <�f�[�^�̗���>
	// (1) ���J���ŕ�����������(�u���b�N)�����[�J���ϐ�(original)�Ɋi�[
	// (2) �u���b�N����]���ȕ�������菜���Ĉ���(pDst)�ɃR�s�[

	ECAppType  type = this->selectAppType( prh );

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
	if( !ACSign_Decrypto( original, publicKey, prh->signature, RSA_KEY_LENGTH ) )
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
	ROM_Header tmprh;		// �}�l�[�W�q�[�v��ɂ���ꍇ���A�h���X���擾�ł��Ȃ��̂ŃT�C�Y�v�Z�p��ROM�w�b�_��p��
	ACSign_DigestUnit( digest,	prh, (u32)&(tmprh.certificate) - (u32)&(tmprh) );
	if( memcmp( &(original[pos+1]), digest, DIGEST_SIZE_SHA1 ) != 0 )
	{
		return ECSrlResult::ERROR_SIGN_VERIFY;
	}
	return (ECSrlResult::NOERROR);
}

// -------------------------------------------------------------------
// �A�v����ʂ̔���
// -------------------------------------------------------------------
ECAppType RCSrl::selectAppType( ROM_Header *prh )
{
	ECAppType  type = ECAppType::ILLEGAL;

	u8  *idL = prh->s.titleID_Lo;
	u32  idH = prh->s.titleID_Hi;

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

// -------------------------------------------------------------------
// DS�_�E�����[�h������SRL�Ɋi�[����Ă��邩���ׂ�
// -------------------------------------------------------------------
ECSrlResult RCSrl::hasDSDLPlaySign( FILE *fp )
{
	const s32 DSDLPLAY_SIZE = 0x88;
	u8        buf[ DSDLPLAY_SIZE ];
	u32       offset;

	// �����̈��SRL����ǂݍ���
	offset = this->pRomHeader->s.rom_valid_size;
	if( fseek( fp, offset, SEEK_SET ) != 0 )
	{
		this->HasDSDLPlaySign = false;			// ������NTR��ROM�̈�̍Ō�ɓ���̂�
		return ECSrlResult::ERROR_FILE_READ;	// NTR��SRL�ɏ������i�[����Ă��Ȃ��ꍇ�̓V�[�N�����[�h���ł��Ȃ�
	}
	if( DSDLPLAY_SIZE != fread( buf, 1, DSDLPLAY_SIZE, fp ) )
	{
		this->HasDSDLPlaySign = false;
		return ECSrlResult::ERROR_FILE_READ;
	}

	// �ŏ���2�o�C�g���Œ�̒l�ƂȂ邱�Ƃ��ۏ؂���Ă���̂ł��̒l���ǂ����Ŕ���
	if( (buf[0] == 'a') && (buf[1] == 'c') )
	{
		this->HasDSDLPlaySign = true;
	}
	else
	{
		this->HasDSDLPlaySign = false;
	}
	return (ECSrlResult::NOERROR);
}

// -------------------------------------------------------------------
// SDK�o�[�W�������擾����
// -------------------------------------------------------------------
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

			// ARM9 static ���ɂ��邩����
			u32 statbegin = this->pRomHeader->s.main_rom_offset;
			u32 statend   = this->pRomHeader->s.main_rom_offset + this->pRomHeader->s.main_size - 1;
			System::Boolean isstat = ((statbegin <= offset) && (offset <= statend))?true:false;
			this->hSDKList->Add( gcnew RCSDKVersion(sdkcode, isstat) );
			//System::Diagnostics::Debug::WriteLine( "SDK " + str );
		}
	}
	return ECSrlResult::NOERROR;
}

// -------------------------------------------------------------------
// �g�p���C�Z���X���擾����
// -------------------------------------------------------------------
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

// -------------------------------------------------------------------
// NAND����T�C�Y���v�Z����
// -------------------------------------------------------------------
void RCSrl::calcNandUsedSize(FILE *fp)
{
	this->hNandUsedSize = gcnew MasterEditorTWL::RCNandUsedSize;

	this->hNandUsedSize->IsMediaNand = this->IsMediaNand;
	this->hNandUsedSize->IsUseSubBanner = this->IsSubBanner;

	// SRL�̏���o�^
	this->hNandUsedSize->PublicSaveSize  = this->PublicSize;
	this->hNandUsedSize->PrivateSaveSize = this->PrivateSize;
	fseek( fp, 0, SEEK_END );
	this->hNandUsedSize->SrlSize = ftell(fp);

	//System::Diagnostics::Debug::WriteLine( "SRL " + MasterEditorTWL::transSizeToString(this->hNandUsedSize->SrlSizeRoundUp) );
	//System::Diagnostics::Debug::WriteLine( "PUB " + MasterEditorTWL::transSizeToString(this->hNandUsedSize->PublicSaveSizeRoundUp) );
	//System::Diagnostics::Debug::WriteLine( "PRI " + MasterEditorTWL::transSizeToString(this->hNandUsedSize->PrivateSaveSizeRoundUp) );
	//System::Diagnostics::Debug::WriteLine( "TMD " + MasterEditorTWL::transSizeToString(this->hNandUsedSize->TmdSizeRoundUp) );
	//System::Diagnostics::Debug::WriteLine( "SUB " + MasterEditorTWL::transSizeToString(this->hNandUsedSize->SubBannerSizeRoundUp) );
	//System::Diagnostics::Debug::WriteLine( "ALL " + MasterEditorTWL::transSizeToString(this->hNandUsedSize->NandUsedSize) );
}//RCSrl::calcNandUsedSize()

// -------------------------------------------------------------------
// MRC���b�Z�[�W���擾
// -------------------------------------------------------------------

// �J�n�ƏI���A�h���X��ROM�ɂ���ĈقȂ�/���݂��Ȃ��G���[
RCMrcError^ RCSrl::makeMrcError( System::UInt32 beg, System::UInt32 end, System::Boolean isEnableModify, System::Boolean isAffectRom,
								 RCMrcError::PurposeType purpose,
								 System::String ^tag, ... cli::array<System::String^> ^args )
{
	// �O���t�@�C�����獀�ږ����擾
	System::String ^nameJ = this->hMrcMsg->getMessage( tag+"/name", "J" );
	System::String ^nameE = this->hMrcMsg->getMessage( tag+"/name", "E" );
	// ���b�Z�[�W���擾
	System::String ^fmtJ  = this->hMrcMsg->getMessage( tag+"/sentence", "J" );	// ���b�Z�[�W�t�@�C�����珑�����擾
	System::String ^msgJ = System::String::Format( fmtJ, args );					// ������String�ɓW�J
	System::String ^fmtE  = this->hMrcMsg->getMessage( tag+"/sentence", "E" );
	System::String ^msgE = System::String::Format( fmtE, args );

	return (gcnew RCMrcError( nameJ, beg, end, msgJ, nameE, msgE, isEnableModify, isAffectRom, purpose ));
}

// �J�n�ƏI���A�h���X�ɐݒ�t�@�C���ɏ�����Ă�����̂��̗p����G���[ (�p�r�����肳���Ƃ��p)
RCMrcError^ RCSrl::makeMrcError( RCMrcError::PurposeType purpose, System::String ^tag, ... cli::array<System::String^> ^args )
{
	// �p�����[�^���擾(�p�����[�^�͉p��łɂ����{��łɂ��o�^����Ă���ǂ���ł��悢�����{��ł̂��̂ɂ��Ă���)
	System::UInt32  beg = System::UInt32::Parse( this->hMrcMsg->getMessage( tag+"/begin", "J" ), System::Globalization::NumberStyles::HexNumber );
	System::UInt32  end = System::UInt32::Parse( this->hMrcMsg->getMessage( tag+"/end",   "J" ), System::Globalization::NumberStyles::HexNumber );
	System::Boolean isEnableModify = System::Boolean::Parse( this->hMrcMsg->getMessage( tag+"/modify", "J" ) );
	System::Boolean isAffectRom    = System::Boolean::Parse( this->hMrcMsg->getMessage( tag+"/affect", "J" ) );

	return (this->makeMrcError( beg, end, isEnableModify, isAffectRom, purpose, tag, args ));
}

// �p�r�ɂ�����炸���ʂ̃G���[
RCMrcError^ RCSrl::makeMrcError( System::String ^tag, ... cli::array<System::String^> ^args )
{
	return (this->makeMrcError( RCMrcError::PurposeType::Common, tag, args ));	// ���ʃG���[
}
