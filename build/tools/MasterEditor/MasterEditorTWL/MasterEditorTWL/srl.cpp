// srl.h のクラス実装

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
// RCSrl クラス
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
	// destructorは明示的にdeleteされないと呼ばれない(GCからは呼ばれない)ようなので
	// GCに解放してもらったときに資源を解放するには finalizer が必要

	// ポインタは(たぶん) unmanaged なので自主的に解放する
	delete (this->pRomHeader);
}

// -------------------------------------------------------------------
// ROMヘッダをファイルからリード
//
// @arg [in] 入力ファイル名
// -------------------------------------------------------------------
ECSrlResult RCSrl::readFromFile( System::String ^srlfile )
{
	FILE       *fp = NULL;
	const char *pchFilename = 
		(const char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( srlfile ).ToPointer();
	ECSrlResult r;

	// ファイルを開いてROMヘッダのみ読み出す
	if( fopen_s( &fp, pchFilename, "rb" ) != NULL )
	{
		return (ECSrlResult::ERROR_FILE_OPEN);
	}
	(void)fseek( fp, 0, SEEK_SET );		// ROMヘッダはsrlの先頭から

	// 1バイトをsizeof(~)だけリード (逆だと返り値がsizeof(~)にならないので注意)
	ROM_Header tmprh;
	if( fread( (void*)&tmprh, 1, sizeof(ROM_Header), fp ) != sizeof(ROM_Header) )
	{
		return (ECSrlResult::ERROR_FILE_READ);
	}

#ifdef METWL_WHETHER_SIGN_DECRYPT
	// まず署名チェック
	r = this->decryptRomHeader( &tmprh );
	if( r != ECSrlResult::NOERROR )
	{
		(void)fclose(fp);
		return r;
	}
#endif //#ifdef METWL_WHETHER_SIGN_DECRYPT

#ifdef METWL_WHETHER_PLATFORM_CHECK
	// プラットフォームのチェック
	if( ((tmprh.s.platform_code == PLATFORM_CODE_NTR) && (tmprh.s.enable_signature != 0)) ||	// PictoChat/DS-download-play のみこれに該当
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

	// 署名チェックを通ってからフィールドのROMヘッダにコピー
	// (そうしないと不正SRLを読み込んだときにROMヘッダが上書きされてしまう)
	memcpy( this->pRomHeader, &tmprh, sizeof(ROM_Header) );

	{
		// ファイルを閉じる前にROMヘッダ以外の領域から設定を取り出す
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

	// エラーリストをクリア
	this->hErrorList = gcnew System::Collections::Generic::List<RCMrcError^>;
	this->hWarnList  = gcnew System::Collections::Generic::List<RCMrcError^>;
	this->hParentalErrorList = gcnew System::Collections::Generic::List<RCMrcError^>;
	this->hParentalWarnList  = gcnew System::Collections::Generic::List<RCMrcError^>;
	this->hErrorList->Clear();
	this->hWarnList->Clear();
	this->hParentalErrorList->Clear();
	this->hParentalWarnList->Clear();

	// ROMヘッダの値をROM固有情報フィールドに反映させる
	(void)this->setRomInfo();
	(void)this->calcNandUsedSize( fp );	// NAND消費サイズ

	// すべて設定したあとにMRC
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
// ROMヘッダをファイルにライト
//
// @arg [in] 出力ファイル名
// -------------------------------------------------------------------
ECSrlResult RCSrl::writeToFile( System::String ^srlfile )
{
	FILE       *fp = NULL;
	const char *pchFilename = 
		(const char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( srlfile ).ToPointer();
	ECSrlResult result;

	// ROMヘッダにROM固有情報を反映させる
	result = this->setRomHeader();
	if( result != ECSrlResult::NOERROR )
	{
		return result;
	}

	// ファイルにROMヘッダをライト
	if( fopen_s( &fp, pchFilename, "r+b" ) != NULL )	// 上書き・バイナリ
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
// ROMヘッダから取得したROM固有情報をフィールドに反映させる
// -------------------------------------------------------------------
ECSrlResult RCSrl::setRomInfo(void)
{
	System::Int32 i;
	System::Text::UTF8Encoding^ utf8 = gcnew System::Text::UTF8Encoding( true );	// char->String変換に必要

	// NTR互換情報
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

	// TWL専用情報
	this->IsNormalJump = (this->pRomHeader->s.permit_landing_normal_jump != 0)?true:false;
	this->IsTmpJump    = (this->pRomHeader->s.permit_landing_tmp_jump    != 0)?true:false;
	this->NormalRomOffset   = (u32)(this->pRomHeader->s.twl_card_normal_area_rom_offset)   * 0x80000;
	this->KeyTableRomOffset = (u32)(this->pRomHeader->s.twl_card_keytable_area_rom_offset) * 0x80000;
	this->PublicSize  = this->pRomHeader->s.public_save_data_size;
	this->PrivateSize = this->pRomHeader->s.private_save_data_size;

	u8  *idL = this->pRomHeader->s.titleID_Lo;
	u32  idH = this->pRomHeader->s.titleID_Hi;
	//u32  val;
	//val = ((u32)(idL[0]) << 24) | ((u32)(idL[1]) << 16) | ((u32)(idL[2]) << 8) | ((u32)(idL[3]));	// ビッグエンディアン
	//this->hTitleIDLo   = gcnew System::UInt32( val );
	this->hTitleIDLo  = gcnew System::String( (char*)idL, 0, 4, utf8 );
	this->TitleIDHi   = idH;

	// TitleIDからわかる情報
	this->IsAppLauncher = false;
	this->IsAppUser     = false;
	this->IsAppSystem   = false;
	this->IsAppSecure   = false;
	this->IsLaunch      = false;
	this->IsMediaNand   = false;
	this->IsDataOnly    = false;
	if( (idL[3]=='H') && (idL[2]=='N') && (idL[1]=='A') )	// ランチャアプリかどうかはTitleID_Loの値で決定
    {
		this->IsAppLauncher = true;
    }
    else if( idH & TITLE_ID_HI_SECURE_FLAG_MASK )				// 各ビットは排他的とは限らないのでelse ifにはならない
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
	if( (idH & TITLE_ID_HI_NOT_LAUNCH_FLAG_MASK) == 0 )		// ビットが0のときLaunch
	{
		this->IsLaunch = true;
	}
	u16 pub = (u16)((idH & TITLE_ID_HI_PUBLISHER_CODE_MASK) >> TITLE_ID_HI_PUBLISHER_CODE_SHIFT);
	this->PublisherCode = pub;

	// TWL拡張フラグ
	this->IsCodecTWL  = (this->pRomHeader->s.exFlags.codec_mode != 0)?true:false;
	this->IsEULA      = (this->pRomHeader->s.exFlags.agree_EULA != 0)?true:false;
	this->IsSubBanner = (this->pRomHeader->s.exFlags.availableSubBannerFile != 0)?true:false;
	this->IsWiFiIcon  = (this->pRomHeader->s.exFlags.WiFiConnectionIcon != 0)?true:false;
	this->IsWirelessIcon = (this->pRomHeader->s.exFlags.DSWirelessIcon != 0)?true:false;
	this->IsWL        = (this->pRomHeader->s.exFlags.enable_nitro_whitelist_signature != 0)?true:false;

	// TWLアクセスコントロール
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

	// 中韓設定フラグ
	this->IsForChina = (this->pRomHeader->s.for_china != 0)?true:false;
	this->IsForKorea = (this->pRomHeader->s.for_korea != 0)?true:false;

	// 5.2 RELEASE以降のときSDアクセス権を調べる必要あり
	u32 sdkver = 0;
	for each ( RCSDKVersion ^sdk in this->hSDKList )
	{
		if( sdk->IsStatic )
		{
			sdkver = sdk->Code;
		}
	}
	this->IsOldSDK52Release = MasterEditorTWL::IsOldSDKVersion(sdkver, METWL_SDK52_RELEASE, true);	// PR/RC版でもエラーを出す
	this->IsOldSDK51PR      = MasterEditorTWL::IsOldSDKVersion(sdkver, METWL_SDK51_PR, true);

	// SCFG がロックされるか
	if( (this->pRomHeader->s.arm7_scfg_ext >> 31) != 0 )
	{
		this->IsSCFGAccess = true;
	}
	else
	{
		this->IsSCFGAccess = false;
	}

	// Shared2ファイルサイズ
	this->hShared2SizeArray = gcnew cli::array<System::UInt32>(METWL_NUMOF_SHARED2FILES);
	for( i=0; i < METWL_NUMOF_SHARED2FILES; i++ )
	{
		this->hShared2SizeArray[i] = 0;
	}
	u32  unit = 16 * 1024;		// 16KBの乗数が格納されている
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

	// ペアレンタルコントロール情報の取得

	const u32  region = this->pRomHeader->s.card_region_bitmap;

	// すべての団体を「不可」に初期化
	this->hArrayParentalIndex = gcnew cli::array<int>(PARENTAL_CONTROL_INFO_SIZE);
	for( i=0; i < PARENTAL_CONTROL_INFO_SIZE; i++ )
	{
		this->hArrayParentalIndex[i] = -1;
	}
	this->IsUnnecessaryRating = false;	// リージョンが不正のときに設定されないので初期化しておく

	// リージョンとレーティングを取得
	if( this->setRegionInfo( region ) )
	{
		this->setUnnecessaryRatingInfo( region );	// レーティング表示が不要かどうかを調べる(これがないと全年齢と区別できない)
		if( !this->IsUnnecessaryRating )
		{
			this->setRatingInfo( region );			// リージョンに含まれる団体のレーティング情報を取得
		}

		// 中国版に限り全団体のレーティングが全年齢でなければならない
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
// ROMヘッダ内のリージョン情報をフィールドに反映させる
// -------------------------------------------------------------------
bool RCSrl::setRegionInfo( u32 region )
{
	this->IsRegionJapan     = ((region & METWL_MASK_REGION_JAPAN)     != 0)?true:false;
	this->IsRegionAmerica   = ((region & METWL_MASK_REGION_AMERICA)   != 0)?true:false;
	this->IsRegionEurope    = ((region & METWL_MASK_REGION_EUROPE)    != 0)?true:false;
	this->IsRegionAustralia = ((region & METWL_MASK_REGION_AUSTRALIA) != 0)?true:false;
	this->IsRegionKorea     = ((region & METWL_MASK_REGION_KOREA)     != 0)?true:false;
	this->IsRegionChina     = ((region & METWL_MASK_REGION_CHINA)     != 0)?true:false;

	// リージョンに含まれる団体がなかったらリージョンは不正
	if( (MasterEditorTWL::getOgnListInRegion( region ) == nullptr) && (region != METWL_MASK_REGION_CHINA) )	// 中国は例外
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
// ROMヘッダ内のレーティング表示不要フラグを調べてフィールドに反映させる
// ----------------------------------------------------------------------
void RCSrl::setUnnecessaryRatingInfo( u32 region )
{
	// ROMヘッダのフラグを調べる
	this->IsUnnecessaryRating = (this->pRomHeader->s.unnecessary_rating_display != 0)?true:false;
	if( !this->IsUnnecessaryRating )
	{
		return;		// 不要でないならこれ以降のチェックは必要ない(レーティング情報の取得に移る)
	}

	// リージョンに含まれるレーティング団体を取得
	System::Collections::Generic::List<int> ^ognlist = MasterEditorTWL::getOgnListInRegion( region );
	if( ognlist == nullptr )
	{
		// 中国のときレーティング団体は存在しないがレーティング表示不要の選択は可能なので警告しておく
		if( this->IsUnnecessaryRating )
		{
			this->hParentalWarnList->Add( this->makeMrcError("UnnecessaryRatingSetting") );
		}
		return;
	}

	// リージョンに含まれる団体のレーティング情報に余計なデータが登録されていないかチェック
	bool noerror = false;
	for each( int ogn in ognlist )
	{
		bool b = false;
		if( this->pRomHeader->s.parental_control_rating_info[ ogn ] == (OS_TWL_PCTL_OGNINFO_ENABLE_MASK | 0) )	// 全年齢しか許さない
		{
			b = true;
		}
		noerror = noerror | b;		// すべて全年齢となっていないときはエラーとみなす
	}
	if( !noerror )
	{
		this->IsUnnecessaryRating = false;	// エラーのときはROMヘッダに不要と登録されていても無視する
		this->hParentalErrorList->Add( this->makeMrcError("UnnecessaryRatingIllegal") );
	}
	else
	{
		this->hParentalWarnList->Add( this->makeMrcError("UnnecessaryRatingSetting") );
	}
}

// -------------------------------------------------------------------
// ROMヘッダ内のペアレンタルコントロール情報をフィールドに反映させる
// -------------------------------------------------------------------
void RCSrl::setRatingInfo( u32 region )
{
	// リージョンに含まれている団体をリストアップ
	// (含まれていない団体のレーティングを読み込まない)
	System::Collections::Generic::List<int> ^ognlist = MasterEditorTWL::getOgnListInRegion( region );
	if( ognlist == nullptr )	// リストがnullptrなら不正
	{
		return;
	}

	// リージョンに含まれる団体のレーティング情報を調べる
	for each( int ogn in ognlist )
	{
		this->setOneRatingOrgInfo( ogn );
	}
}

// -------------------------------------------------------------------
// 団体のレーティング情報からコンボボックスのインデックスを決定する
// -------------------------------------------------------------------
void RCSrl::setOneRatingOrgInfo( int ogn )
{
	System::Boolean enable = ((this->pRomHeader->s.parental_control_rating_info[ ogn ] & OS_TWL_PCTL_OGNINFO_ENABLE_MASK) != 0)?true:false;
	System::Boolean rp     = ((this->pRomHeader->s.parental_control_rating_info[ ogn ] & OS_TWL_PCTL_OGNINFO_ALWAYS_MASK) != 0)?true:false;
	System::Byte    age    = this->pRomHeader->s.parental_control_rating_info[ ogn ] & OS_TWL_PCTL_OGNINFO_AGE_MASK;

	cli::array<System::Byte> ^ages = MasterEditorTWL::getOgnRatingAges( ogn );
	System::String           ^name = MasterEditorTWL::getOgnName( ogn );

	int index = -1;
	if( !enable )	// 未定義
	{
		index = -1;
		this->hParentalErrorList->Add( this->makeMrcError("RatingUndefine", name) );
	}
	else
	{
		if( rp )	// 審査中
		{
			if( age == 0 ) // 年齢が0のときは審査中とみなす
			{
				index = ages->Length;	// 配列の最後の要素の次が「審査中」
				this->hParentalWarnList->Add( this->makeMrcError("RatingPendingSetting", name) );
			}
			else
			{
				index = -1;		// それ以外はエラー
				this->hParentalErrorList->Add( this->makeMrcError("RatingPendingConcurrent", name) );
			}
		}
		else
		{
			index = -1;		// 団体が認めていない年齢が格納されているときエラー
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
// ROMヘッダにROM固有情報フィールドの値を反映させる
// -------------------------------------------------------------------
ECSrlResult RCSrl::setRomHeader(void)
{
	ECSrlResult result;

	// ROMヘッダの[0,0x160)の領域はRead Onlyで変更しない

	// リージョン
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
		map |= METWL_MASK_REGION_ALL;	// オールリージョンを許す
	}
#endif
	this->pRomHeader->s.card_region_bitmap = map;

	// レーティング
	this->setRatingRomHeader( map );

	// ROMヘッダのCRCと署名を更新する
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

// ペアレンタルコントロールのプロパティをROMヘッダに反映させる
void RCSrl::setRatingRomHeader( u32 region )
{
	// リージョンに含まれない団体はすべて「不可」(未定義)
	int j;
	for( j=0; j < PARENTAL_CONTROL_INFO_SIZE; j++ )
	{
		this->pRomHeader->s.parental_control_rating_info[j] = 0x00;
	}

	// 「レーティング表示不要」フラグを立てる
	this->pRomHeader->s.unnecessary_rating_display = (this->IsUnnecessaryRating == true)?1:0;

	// 中国リージョンおよびオールリージョンのとき予約領域もすべて「全年齢」(0x80)で埋めておく
	// 「レーティング表示不要」かどうかにかかわらず埋める
	if( this->IsRegionChina )	// オールリージョンのときも中国ビットは立つ
	{
		for( j=0; j < PARENTAL_CONTROL_INFO_SIZE; j++ )
		{
			this->pRomHeader->s.parental_control_rating_info[j] = OS_TWL_PCTL_OGNINFO_ENABLE_MASK;
		}
	}

	// リージョンに含まれる団体のみを設定
	System::Collections::Generic::List<int> ^ognlist = MasterEditorTWL::getOgnListInRegion( region );
	if( !ognlist )
	{
		return;		// 中国のときリストは空になる
	}
	for each( int ogn in ognlist )
	{
		u8 rating = 0;
		if( this->IsUnnecessaryRating )
		{
			rating = OS_TWL_PCTL_OGNINFO_ENABLE_MASK | 0;	// レーティング表示が不要のときは「全年齢」と同じ値にする
		}
		else
		{
			cli::array<System::Byte> ^ages = MasterEditorTWL::getOgnRatingAges( ogn );	// 設定可能年齢リストを取得

			if( this->hArrayParentalIndex[ ogn ] < 0 )		// 未定義
			{
				rating = 0x00;
			}
			else
			{
				int index = this->hArrayParentalIndex[ ogn ];
				if( index == ages->Length )				// 審査中
				{
					rating = OS_TWL_PCTL_OGNINFO_ENABLE_MASK | OS_TWL_PCTL_OGNINFO_ALWAYS_MASK;
				}
				else									// レーティング年齢を設定
				{
					rating = OS_TWL_PCTL_OGNINFO_ENABLE_MASK | ages[ index ];
				}
			}
		}
		this->pRomHeader->s.parental_control_rating_info[ ogn ] = rating;
	}
}

// -------------------------------------------------------------------
// ヘッダのCRCを算出
// -------------------------------------------------------------------
u16 calcCRC( const u16 start, const u8 *data, const int allsize )
{
	u16       r1;
	u16       total = start;
	int       size  = allsize;

	// CRCテーブルは固定
	const u16 CRC16Table[16] =
    {
		0x0000, 0xCC01, 0xD801, 0x1400,
		0xF001, 0x3C00, 0x2800, 0xE401,
		0xA001, 0x6C00, 0x7800, 0xB401,
		0x5000, 0x9C01, 0x8801, 0x4400
    };

	// CRC計算
	while (size-- > 0)
    {
        // 下位4bit
        r1 = CRC16Table[total & 0xf];
        total = (total >> 4) & 0x0fff;
        total = total ^ r1 ^ CRC16Table[*data & 0xf];

        // 上位4bit
        r1 = CRC16Table[total & 0xf];
        total = (total >> 4) & 0x0fff;
        total = total ^ r1 ^ CRC16Table[(*data >> 4) & 0xf];

        data++;
    }
	return total;
} // calcRomHeaderCRCCore()

ECSrlResult RCSrl::calcRomHeaderCRC(void)
{
	// ROMヘッダのCRC領域を上書き
	this->pRomHeader->s.header_crc16 = calcCRC( CRC16_INIT_VALUE, (u8*)this->pRomHeader, CALC_CRC16_SIZE );

	return (ECSrlResult::NOERROR);
} // ECSrlResult RCSrl::calcRomHeaderCRC(void)

// -------------------------------------------------------------------
// ROMヘッダを再署名
// -------------------------------------------------------------------
ECSrlResult RCSrl::signRomHeader(void)
{
	SignatureData   signSrc;						// 署名のもととなるダイジェスト値
	u8              signDst[ RSA_KEY_LENGTH ];		// 署名の格納先Tmpバッファ
	u8              decryptBlock[ RSA_KEY_LENGTH ];	// 署名を解除後ブロックバッファ
	BOOL            result = false;
	ROM_Header      rh;
	int             pos;
	u8              *privateKey = (u8*)g_devPrivKey_DER;
	u8              *publicKey  = (u8*)g_devPubKey_DER;

	// ROMヘッダのダイジェストを算出(先頭から証明書領域の直前までが対象)
	ACSign_DigestUnit(
		signSrc.digest,
		this->pRomHeader,
		(u32)&(rh.certificate) - (u32)&(rh)		// this->pRomHeader はマネージヒープ上にあるので実アドレスを取得できない
	);

	// 鍵を選ぶ
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

	// ダイジェストに署名をかける
	result = ACSign_Encrypto( signDst, privateKey, &signSrc, sizeof(SignatureData) ); 
	if( !result )
	{
		return (ECSrlResult::ERROR_SIGN_ENCRYPT);
	}

	// 署名を解除してダイジェストと一致するかベリファイする
	result = ACSign_Decrypto( decryptBlock, publicKey, signDst, RSA_KEY_LENGTH );
	for( pos=0; pos < RSA_KEY_LENGTH; pos++ )
	{
		if( decryptBlock[pos] == 0x0 )			// 解除後ブロックから実データをサーチ
			break;
	}
	if( !result || (memcmp( &signSrc, &(decryptBlock[pos+1]), sizeof(SignatureData) ) != 0) )
	{
		return (ECSrlResult::ERROR_SIGN_DECRYPT);
	}

	// ROMヘッダに署名を上書き
	memcpy( this->pRomHeader->signature, signDst, RSA_KEY_LENGTH );

	return (ECSrlResult::NOERROR);
} // ECSrlResult RCSrl::signRomHeader(void)

// -------------------------------------------------------------------
// ROMヘッダの署名を外す
// -------------------------------------------------------------------
ECSrlResult RCSrl::decryptRomHeader( ROM_Header *prh )
{
	u8     original[ RSA_KEY_LENGTH ];	// 署名外した後のデータ格納先
	s32    pos = 0;						// ブロックの先頭アドレス
	u8     digest[ DIGEST_SIZE_SHA1 ];	// ROMヘッダのダイジェスト
	u8    *publicKey = (u8*)g_devPubKey_DER;

	// <データの流れ>
	// (1) 公開鍵で復号した結果(ブロック)をローカル変数(original)に格納
	// (2) ブロックから余分な部分を取り除いて引数(pDst)にコピー

	ECAppType  type = this->selectAppType( prh );

	// 鍵を選ぶ
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

	// 署名の解除 = 公開鍵で復号
	if( !ACSign_Decrypto( original, publicKey, prh->signature, RSA_KEY_LENGTH ) )
	{
		return ECSrlResult::ERROR_SIGN_DECRYPT;
	}
	// 署名前データを復号後ブロックからゲット
	for( pos=0; pos < (RSA_KEY_LENGTH-2); pos++ )   // 本来ブロックの先頭は0x00だが復号化の内部処理によって消える仕様
	{
		// 暗号ブロック形式 = 0x00, BlockType, Padding, 0x00, 実データ
		if( original[pos] == 0x00 )                               // 実データの直前の0x00をサーチ
		{
			break;
		}
	}
	// ベリファイ
	// ROMヘッダのダイジェストを算出(先頭から証明書領域の直前までが対象)
	ROM_Header tmprh;		// マネージヒープ上にある場合実アドレスを取得できないのでサイズ計算用のROMヘッダを用意
	ACSign_DigestUnit( digest,	prh, (u32)&(tmprh.certificate) - (u32)&(tmprh) );
	if( memcmp( &(original[pos+1]), digest, DIGEST_SIZE_SHA1 ) != 0 )
	{
		return ECSrlResult::ERROR_SIGN_VERIFY;
	}
	return (ECSrlResult::NOERROR);
}

// -------------------------------------------------------------------
// アプリ種別の判定
// -------------------------------------------------------------------
ECAppType RCSrl::selectAppType( ROM_Header *prh )
{
	ECAppType  type = ECAppType::ILLEGAL;

	u8  *idL = prh->s.titleID_Lo;
	u32  idH = prh->s.titleID_Hi;

	if( (idL[3]=='H') && (idL[2]=='N') && (idL[1]=='A') )	// ランチャアプリかどうかはTitleID_Loの値で決定
    {
		type = ECAppType::LAUNCHER;
    }
    else if( idH & TITLE_ID_HI_SECURE_FLAG_MASK )				// 各ビットは排他的とは限らないのでelse ifにはならない
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
// DSダウンロード署名がSRLに格納されているか調べる
// -------------------------------------------------------------------
ECSrlResult RCSrl::hasDSDLPlaySign( FILE *fp )
{
	const s32 DSDLPLAY_SIZE = 0x88;
	u8        buf[ DSDLPLAY_SIZE ];
	u32       offset;

	// 署名領域をSRLから読み込む
	offset = this->pRomHeader->s.rom_valid_size;
	if( fseek( fp, offset, SEEK_SET ) != 0 )
	{
		this->HasDSDLPlaySign = false;			// 署名はNTRのROM領域の最後に入るので
		return ECSrlResult::ERROR_FILE_READ;	// NTRのSRLに署名が格納されていない場合はシークもリードもできない
	}
	if( DSDLPLAY_SIZE != fread( buf, 1, DSDLPLAY_SIZE, fp ) )
	{
		this->HasDSDLPlaySign = false;
		return ECSrlResult::ERROR_FILE_READ;
	}

	// 最初の2バイトが固定の値となることが保証されているのでその値かどうかで判定
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
// SDKバージョンを取得する
// -------------------------------------------------------------------
ECSrlResult RCSrl::searchSDKVersion( FILE *fp )
{
	// SDKバージョンはSRLバイナリ中に以下のマジックナンバとともに埋められている
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
		// マジックコードのオフセットの手前4バイトがSDKバージョン
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

			// ARM9 static 内にあるか判定
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
// 使用ライセンスを取得する
// -------------------------------------------------------------------
#define  LICENSE_LEN_MAX   1024		// これよりもライセンスの文字列が長いとき正しく取得できない
ECSrlResult RCSrl::searchLicenses(FILE *fp)
{
	// ライセンスはSRLバイナリ中に[SDK+(配布元):(ライブラリ名)]のフォーマットで埋められている
	const u8 pattern[5] = { '[', 'S', 'D', 'K', '+' };
	System::Collections::Generic::List<u32> ^list;

	this->hLicenseList = gcnew System::Collections::Generic::List<RCLicense ^>;
	this->hLicenseList->Clear();
	
	fseek( fp, 0, SEEK_END );
	const u32 filesize = ftell( fp );

	list = MasterEditorTWL::patternMatch( fp, pattern, 5 );
	if( list == nullptr )
	{
		return ECSrlResult::NOERROR;	// ライセンスがない場合も存在するのでOKとする
	}
	for each( u32 item in list )
	{
		char  buf[ LICENSE_LEN_MAX + 1 ];	// '\0'の分だけ多めにとっておく
		u32   offset = item + 5;			// "[SDK+"の後からリードする
		u32   len = ((filesize - offset) < LICENSE_LEN_MAX)?(filesize - offset):LICENSE_LEN_MAX;

		// "(配布元):(ライブラリ名)]"を余分な部分を含めてひとまずバッファに格納
		fseek( fp, offset, SEEK_SET );
		if( len != fread( buf, 1, len, fp ) )
		{
			return ECSrlResult::ERROR;
		}
		buf[ len ] = '\0';	// 後のstrlenの成功を保証するため

		// "(配布元):(ライブラリ名)]"を取り出してから(配布元)と(ライブラリ名)に分割
		System::Text::UTF8Encoding^ utf8 = gcnew System::Text::UTF8Encoding( true );	// char->String変換に必要
		System::String ^str = gcnew System::String( buf, 0, strlen(buf), utf8 );		// 探索や分割はStringメソッドに用意されている
		str = (str->Split( ']' ))[0];							// ']'よりも前を取り出す
		cli::array<System::String^> ^spl = str->Split( ':' );	// (配布元):(ライブラリ名)を分割する

		if( spl->Length >= 2  )		// 念のため
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
// NAND消費サイズを計算する
// -------------------------------------------------------------------
void RCSrl::calcNandUsedSize(FILE *fp)
{
	this->hNandUsedSize = gcnew MasterEditorTWL::RCNandUsedSize;

	this->hNandUsedSize->IsMediaNand = this->IsMediaNand;
	this->hNandUsedSize->IsUseSubBanner = this->IsSubBanner;

	// SRLの情報を登録
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
// MRCメッセージを取得
// -------------------------------------------------------------------

// 開始と終了アドレスがROMによって異なる/存在しないエラー
RCMrcError^ RCSrl::makeMrcError( System::UInt32 beg, System::UInt32 end, System::Boolean isEnableModify, System::Boolean isAffectRom,
								 RCMrcError::PurposeType purpose,
								 System::String ^tag, ... cli::array<System::String^> ^args )
{
	// 外部ファイルから項目名を取得
	System::String ^nameJ = this->hMrcMsg->getMessage( tag+"/name", "J" );
	System::String ^nameE = this->hMrcMsg->getMessage( tag+"/name", "E" );
	// メッセージを取得
	System::String ^fmtJ  = this->hMrcMsg->getMessage( tag+"/sentence", "J" );	// メッセージファイルから書式を取得
	System::String ^msgJ = System::String::Format( fmtJ, args );					// 書式をStringに展開
	System::String ^fmtE  = this->hMrcMsg->getMessage( tag+"/sentence", "E" );
	System::String ^msgE = System::String::Format( fmtE, args );

	return (gcnew RCMrcError( nameJ, beg, end, msgJ, nameE, msgE, isEnableModify, isAffectRom, purpose ));
}

// 開始と終了アドレスに設定ファイルに書かれているものを採用するエラー (用途が限定されるとき用)
RCMrcError^ RCSrl::makeMrcError( RCMrcError::PurposeType purpose, System::String ^tag, ... cli::array<System::String^> ^args )
{
	// パラメータも取得(パラメータは英語版にも日本語版にも登録されておりどちらでもよいが日本語版のものにしておく)
	System::UInt32  beg = System::UInt32::Parse( this->hMrcMsg->getMessage( tag+"/begin", "J" ), System::Globalization::NumberStyles::HexNumber );
	System::UInt32  end = System::UInt32::Parse( this->hMrcMsg->getMessage( tag+"/end",   "J" ), System::Globalization::NumberStyles::HexNumber );
	System::Boolean isEnableModify = System::Boolean::Parse( this->hMrcMsg->getMessage( tag+"/modify", "J" ) );
	System::Boolean isAffectRom    = System::Boolean::Parse( this->hMrcMsg->getMessage( tag+"/affect", "J" ) );

	return (this->makeMrcError( beg, end, isEnableModify, isAffectRom, purpose, tag, args ));
}

// 用途にかかわらず共通のエラー
RCMrcError^ RCSrl::makeMrcError( System::String ^tag, ... cli::array<System::String^> ^args )
{
	return (this->makeMrcError( RCMrcError::PurposeType::Common, tag, args ));	// 共通エラー
}
