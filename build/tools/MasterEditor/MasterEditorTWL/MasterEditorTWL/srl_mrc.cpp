// srl.h のクラス実装

#include "stdafx.h"
#include <apptype.h>
#include "common.h"
#include "srl.h"
#include "utility.h"
#include "keys.h"
#include "crc_whole.h"
#include "banner_char.h"
#include <acsign/include/acsign.h>
#include <format_rom_private.h>
#include <cstring>
#include <cstdio>

using namespace MasterEditorTWL;

extern u16 calcCRC( const u16 start, const u8 *data, const int allsize );

// ****************************************************************
// RCSrl クラスの MRC 機能を別ファイルに独立させた
// ****************************************************************

// -------------------------------------------------------------------
// MRC本体
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
	this->hErrorList->AddRange( this->hParentalErrorList );	// ペアレンタルコントロールのリストは別につくっているので追加
	this->hWarnList->AddRange( this->hParentalWarnList );
#endif
	return ECSrlResult::NOERROR;
}

// -------------------------------------------------------------------
// NTR互換MRC
// -------------------------------------------------------------------
ECSrlResult RCSrl::mrcNTR( FILE *fp )
{
	System::Int32  i;
	System::Boolean result;

	// タイトル名の文字コードチェック
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
		char c = this->pRomHeader->s.title_name[TITLE_NAME_MAX - i - 1];	// 末尾から探索
		if( ((0x21 <= c ) && (c <= 0x5f)) || (c == 0x00) )					// 00hと20h以外の使用可能な文字が出てくるまでにスペースが存在したらダメ
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
		if( (prev == 0x00) && (0x21 <= curr) && (curr <= 0x5f) )	// 途中に00hがあるとダメ
		{
			result = false;
			break;
		}
	}
	if( !result )
	{
		this->hErrorList->Add( this->makeMrcError("TitleNameSpace") );
	}

	// ゲームコードの文字コードチェック
	result = true;
	for( i=0; i < GAME_CODE_MAX; i++ )
	{
		char c = this->pRomHeader->s.game_code[i];
		if( (c < 0x20) || (0x5f < c) )		// 00hを許さない
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

	// メーカコードの文字コードチェック
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

	// デバイスタイプは不使用なので00h固定
	if( this->pRomHeader->s.rom_type != 0x00 )
	{
		this->hErrorList->Add( this->makeMrcError("DeviceType") );
	}

	// リマスターバージョンが16進2桁のときエラー
	u8 romver = this->pRomHeader->s.rom_version;
	if( ((romver < 0x00) || (0x0f < romver)) && (romver != 0xE0) )
	{
		this->hErrorList->Add( this->makeMrcError("RemasterVersion") );
	}

	// 必ず設定されていなければならないパラメータ
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
	// セキュア領域
	if( !this->IsMediaNand )	// カードアプリのときのみ
	{
		// セキュア領域はROMヘッダ外なのでファイルから別途読み出してチェックする
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

	// ロゴ領域
	crc = 0xcf56;
	if( crc != this->pRomHeader->s.nintendo_logo_crc16 )
	{
		this->hErrorList->Add( this->makeMrcError("LogoCRC") );
	}

	// ヘッダCRC
	crc = calcCRC( CRC16_INIT_VALUE, (u8*)this->pRomHeader, CALC_CRC16_SIZE );
	if( crc != this->pRomHeader->s.header_crc16 )
	{
		this->hErrorList->Add( this->makeMrcError("HeaderCRC") );
	}

	// ROMヘッダ以外の領域のチェック (ファイルから適宜リードする)
	
	// システムコールライブラリ
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
	if( i == 32 )	// 全部0
	{
		this->hErrorList->Add( this->makeMrcError("Syscall") );
	}
	return ECSrlResult::NOERROR;
} // mrcNTR()

// -------------------------------------------------------------------
// TWL専用MRC
// -------------------------------------------------------------------
ECSrlResult RCSrl::mrcTWL( FILE *fp )
{
	// ROMヘッダのチェック (NTR互換領域)

	// NANDアプリがHYBRIDとなるのはクローンブートのときのみ
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

	// 旧開発用暗号フラグとクローンブートの組み合わせはマスタリングで矛盾が生じる
	if( this->IsOldDevEncrypt && this->HasDSDLPlaySign )
	{
		this->hErrorList->Add( this->makeMrcError("OldDevFlag") );
	}

	// デバイス容量と実ファイルサイズ
	fseek( fp, 0, SEEK_END );
	u32  filesize = ftell(fp);	// 実ファイルサイズ
	u32  romsize = 1 << (this->pRomHeader->s.rom_size);	// ROM容量(単位Mbit)
	if( !this->IsMediaNand )		// カードアプリのときのみのチェック
	{
		u32 filesizeMb = (filesize / (1024*1024)) * 8;	// 単位をMbitに直す
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
	else	// NANDアプリのときのみのチェック
	{
		if( (romsize < METWL_ROMSIZE_MIN_NAND) || (METWL_ROMSIZE_MAX_NAND < romsize) )
		{
			this->hErrorList->Add( this->makeMrcError("CapacityNandRange") );
		}
		//u32  allsize = filesize + this->pRomHeader->s.public_save_data_size + this->pRomHeader->s.private_save_data_size;
		u32  allsize = this->hNandUsedSize->NandUsedSize;	// TMDやサブバナーのサイズを含める
		if( allsize > METWL_ALLSIZE_MAX_NAND )
		{
			this->hErrorList->Add( this->makeMrcError("UsedNandSizeLimit") );
		}
		if( (allsize > METWL_ALLSIZE_MAX_NAND_LIC) && this->IsAppUser )		// ユーザアプリのときのみ
		{
			this->hErrorList->Add( this->makeMrcError("UsedNandSizeExceed") );
		}
	}

	// パディング
	if( !this->IsMediaNand )
	{
		// 1Gbit以上のときの最終領域が固定値かどうか
		this->mrcPadding( fp );

		// セグメント3のCRC
		u16  crcseg3;
		BOOL crcret = getSeg3CRCInFp( fp, &crcseg3 );
		if( !crcret || (crcseg3 != METWL_SEG3_CRC) )
		{
			this->hErrorList->Add( this->makeMrcError("Seg3CRC") );
		}
	}

	// ROMオフセット
	if( !this->IsMediaNand )
	{
		u16  NA = this->pRomHeader->s.twl_card_normal_area_rom_offset & 0x7fffUL;
		u16  KA = this->pRomHeader->s.twl_card_keytable_area_rom_offset;
		if( (NA == 0) || (KA == 0) || (NA > KA) )
		{
			this->hErrorList->Add( this->makeMrcError("TwlRomOffset") );
		}
	}

	// デバッガ動作禁止フラグはユーザアプリでは立っていてはいけない
	// システムアプリではデバッガで解析されないように通常では立っていなければならない
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

	// ノーマルジャンプ
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
				!this->hMrcExternalCheckItems->IsPermitNormalJump )			// 設定ファイルでアクセス許可されていないときにチェック
			{
				this->hErrorList->Add( this->makeMrcError("NormalJump") );
			}
		}
	}

	// NANDアプリのときにはROMの種類(MROM/1TROM)が設定されていなくてもよい
	if( (this->pRomHeader->s.warning_no_spec_rom_speed != 0) && !this->IsMediaNand )
	{
		this->hErrorList->Add( this->makeMrcError("RomSpeedType") );
	}
	// 1TROMのみ許可(ROMの種類が設定されないときデフォルトで1TROMとなる)
	if( (this->pRomHeader->s.game_cmd_param & CARD_LATENCY_MASK) != CARD_1TROM_GAME_LATENCY )
	{
		this->hErrorList->Add( this->makeMrcError("RomControl") );
	}

	// AESがかかっていないとエラー
	if( !this->IsMediaNand )
	{
		if( (this->pRomHeader->s.enable_aes == 0) || (this->pRomHeader->s.aes_target_size == 0) )
		{
			this->hErrorList->Add( this->makeMrcError("AES") );
		}
	}
	else		// NANDアプリ
	{
		if( (this->pRomHeader->s.enable_aes == 0) || (this->pRomHeader->s.aes_target_size == 0) || (this->pRomHeader->s.aes_target2_size == 0) )
		{
			this->hErrorList->Add( this->makeMrcError("AES") );
		}
	}

	// アプリ種別のチェック
	this->mrcAppType(fp);

	// 各種デバイスへのアクセスフラグのチェック
	this->mrcAccessControl(fp);
	this->mrcShared2(fp);

	// NTRホワイトリスト署名
	if( this->IsWL )
	{
		this->hErrorList->Add( this->makeMrcError("WhiteList") );
	}

	// 通信アイコン表示
	if( this->IsWiFiIcon && this->IsWirelessIcon )
	{
		this->hErrorList->Add( this->makeMrcError("ConnectIcon") );
	}

	// TWL Limited static の位置が KeyTable 領域よりも前にあってはいけない
	if( !this->IsMediaNand )
	{
		u32 ideal  = (this->pRomHeader->s.twl_card_keytable_area_rom_offset * 0x80000) + 0x3000;	// TWL KeyTable領域の開始 + KeyTableサイズ
		u32 actual = this->pRomHeader->s.main_ltd_rom_offset;
		if( ideal > actual )
		{
			this->hErrorList->Add( this->makeMrcError("LimitedStatic") );
		}
	}

	// ランチャー以外はSCFGにアクセスしてはならない
	if( !this->IsAppLauncher && this->IsSCFGAccess )
	{
		this->hWarnList->Add( this->makeMrcError("SCFGAccess") );
	}

	// TitleID Lo はゲームコードの逆順
	if( (this->pRomHeader->s.titleID_Lo[0] != this->pRomHeader->s.game_code[3]) ||
		(this->pRomHeader->s.titleID_Lo[1] != this->pRomHeader->s.game_code[2]) ||
		(this->pRomHeader->s.titleID_Lo[2] != this->pRomHeader->s.game_code[1]) ||
		(this->pRomHeader->s.titleID_Lo[3] != this->pRomHeader->s.game_code[0]) )
	{
		this->hWarnList->Add( this->makeMrcError("TitleID") );
	}

	// カードアプリでPublic/Privateセーブデータを設定してはいけない
	if( !this->IsMediaNand )
	{
		if( (this->PublicSize > 0) || (this->PrivateSize) )
		{
			this->hErrorList->Add( this->makeMrcError("SaveData") );
		}
	}

	// バナーの文字コード
	this->mrcBanner( fp );

	// 予約領域に余計なデータが書かれていないか
	this->mrcReservedArea(fp);

	// FINALROMビルドでないときSDにアクセスできてしまうのでエラー
	for each( RCLicense ^lic in this->hLicenseList )
	{
		if( lic->Publisher->Equals( "NINTENDO" ) && lic->Name->Equals( "DEBUG" ) )
		{
			this->hErrorList->Add( this->makeMrcError("DebugBuild") );
		}
	}

	// 追加チェック
	this->mrcSDKVersion(fp);

	return ECSrlResult::NOERROR;
} // mrcTWL()

// -------------------------------------------------------------------
// アプリ種別のチェック
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

	if( (idL[3]=='H') && (idL[2]=='N') && (idL[1]=='A') )   // ランチャアプリかどうかはTitleID_Loの値で決定
	{
		apptype = appLauncher;
		appstr  = "Launcher";
	}
	else if( idH & TITLE_ID_HI_SECURE_FLAG_MASK )           // 立ってたらセキュアアプリ
	{
		apptype = appSecure;
		appstr  = "Secure";
	}
	else if( (idH & TITLE_ID_HI_APP_TYPE_MASK) == 1 )       // 立ってたらシステムアプリ
	{
		apptype = appSystem;
		appstr  = "System";
	}
	else if( (idH & TITLE_ID_HI_APP_TYPE_MASK) == 0 )       // 残るはユーザアプリ
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
// アクセスコントロール情報のチェック
// -------------------------------------------------------------------
void RCSrl::mrcAccessControl(FILE *fp)
{
	if( (this->pRomHeader->s.access_control.game_card_on != 0) &&
		(this->pRomHeader->s.access_control.game_card_nitro_mode != 0) )
	{
		this->hErrorList->Add( this->makeMrcError("CardAccess") );
	}

	if( !this->IsAppUser )
	{
		if( this->pRomHeader->s.access_control.common_client_key != 0 )
		{
			this->hWarnList->Add( this->makeMrcError("IllegalAccessSystem", "Common Client Key") );
		}
		if( this->pRomHeader->s.access_control.hw_aes_slot_B != 0 )
		{
			this->hWarnList->Add( this->makeMrcError("IllegalAccessSystem", "HW AES Slot B for ES") );
		}
		if( this->pRomHeader->s.access_control.hw_aes_slot_C != 0 )
		{
			this->hWarnList->Add( this->makeMrcError("IllegalAccessSystem", "HW AES Slot C for NAM") );
		}
		if( this->pRomHeader->s.access_control.sd_card_access != 0 )
		{
			this->hWarnList->Add( this->makeMrcError("SDAccessSystem") );
		}
		if( this->pRomHeader->s.access_control.game_card_on != 0 )
		{
			this->hWarnList->Add( this->makeMrcError("GameCardNormalAccessSystem") );
		}
		if( this->pRomHeader->s.access_control.hw_aes_slot_B_SignJPEGForLauncher != 0 )
		{
			this->hWarnList->Add( this->makeMrcError("IllegalAccessSystem", "HW AES Slot B (JPEG signature) for the launcher") );
		}
		if( this->pRomHeader->s.access_control.game_card_nitro_mode != 0 )
		{
			this->hWarnList->Add( this->makeMrcError("GameCardNTRAccessSystem") );
		}
		if( this->pRomHeader->s.access_control.hw_aes_slot_A_SSLClientCert != 0 )
		{
			this->hWarnList->Add( this->makeMrcError("IllegalAccessSystem", "HW AES Slot A for the SSL client certification") );
		}
		if( (this->pRomHeader->s.access_control.hw_aes_slot_B_SignJPEGForUser != 0) )
		{
			this->hWarnList->Add( this->makeMrcError("JpegSignAccessSystem") );
		}
		if( this->pRomHeader->s.access_control.common_client_key_for_debugger_sysmenu != 0 )
		{
			this->hWarnList->Add( this->makeMrcError("IllegalAccessSystem", "Common Client Key for the debugger system menu") );
		}

		// その他のビット
		u32 okbits = 0x80001FFF;
		u32 *p = (u32*)&(this->pRomHeader->s);
		if( p[ 0x1b4 / 4 ] & ~okbits )
		{
			this->hErrorList->Add( this->makeMrcError("AccessDefault") );
		}
	}
	else	// ユーザアプリ
	{
		if( this->pRomHeader->s.access_control.common_client_key != 0 )
		{
			this->hErrorList->Add( this->makeMrcError("IllegalAccessUser", "Common Client Key") );
		}
		if( this->pRomHeader->s.access_control.hw_aes_slot_B != 0 )
		{
			this->hErrorList->Add( this->makeMrcError("IllegalAccessUser", "HW AES Slot B for ES") );
		}
		if( this->pRomHeader->s.access_control.hw_aes_slot_C != 0 )
		{
			this->hErrorList->Add( this->makeMrcError("IllegalAccessUser", "HW AES Slot C for NAM") );
		}
		if( !this->IsMediaNand && (this->pRomHeader->s.access_control.nand_access != 0) )	// カードアプリのときのみ
		{
			this->hErrorList->Add( this->makeMrcError("NandAccessUser") );
		}
		if( this->pRomHeader->s.access_control.game_card_on != 0 )
		{
			this->hErrorList->Add( this->makeMrcError("GameCardNormalAccessUser") );
		}
		if( this->pRomHeader->s.access_control.hw_aes_slot_B_SignJPEGForLauncher != 0 )
		{
			this->hErrorList->Add( this->makeMrcError("IllegalAccessUser", "HW AES Slot B (JPEG signature) for the launcher") );
		}
		if( this->pRomHeader->s.access_control.game_card_nitro_mode != 0 )
		{
			this->hErrorList->Add( this->makeMrcError("GameCardNTRAccessUser") );
		}
		if( this->pRomHeader->s.access_control.hw_aes_slot_A_SSLClientCert != 0 )
		{
			this->hErrorList->Add( this->makeMrcError("IllegalAccessUser", "HW AES Slot A for the SSL client certification") );
		}

		// SDアクセスは 5.2RELEASE で特定のアプリには許可される
		if( !this->IsMediaNand )
		{
			// カードアプリはSDアクセス禁止
			if( this->pRomHeader->s.access_control.sd_card_access != 0 )
			{
				this->hErrorList->Add( this->makeMrcError("SDAccessUser") );
			}
		}
		else
		{
			// 5.2 RELEASEかどうかで判定がかわる
			if( this->IsSDK52Release )
			{
				// 5.2 RELEASE以降はアクセス権さえ設定されていればエラーを出さない
				if( (this->pRomHeader->s.access_control.sd_card_access != 0) &&		// SDカードアクセスが有効になっているのに
					(this->pRomHeader->s.access_control.sdmc_access_write == 0) &&	// アクセス権のフラグが下りている
					(this->pRomHeader->s.access_control.sdmc_access_read  == 0 ) )
				{
					this->hErrorList->Add( this->makeMrcError("SDAccessPriv") );
				}
			}
			else
			{
				// 5.2 RELEASE以前は原則SDアクセス禁止
				if( this->pRomHeader->s.access_control.sd_card_access != 0 )
				{
					this->hErrorList->Add( this->makeMrcError("SDAccessUser") );
				}
			}
		}

		if( !this->IsMediaNand )
		{
			// カードだと写真領域へのアクセスは禁止
			if( this->pRomHeader->s.access_control.photo_access_read != 0 )
			{
				this->hErrorList->Add( this->makeMrcError("PhotoReadAccessUser") );
			}
			if( this->pRomHeader->s.access_control.photo_access_write != 0 )
			{
				this->hErrorList->Add( this->makeMrcError("PhotoWriteAccessUser") );
			}
			// Jpeg署名も禁止
			if( this->pRomHeader->s.access_control.hw_aes_slot_B_SignJPEGForUser != 0 )
			{
				this->hErrorList->Add( this->makeMrcError("JpegSignAccessUserCard") );
			}
		}
		else
		{
			// photoへのライトアクセス権があるのにJpeg署名がないとき
			if( (this->pRomHeader->s.access_control.photo_access_write != 0) && 
				(this->pRomHeader->s.access_control.hw_aes_slot_B_SignJPEGForUser == 0) )
			{
				this->hErrorList->Add( this->makeMrcError("PhotoJpegSignAccessUser") );
			}
			// photoへライトしないくせにJpeg署名が有効なとき
			if( (this->pRomHeader->s.access_control.photo_access_write == 0) &&
				(this->pRomHeader->s.access_control.hw_aes_slot_B_SignJPEGForUser != 0) )
			{
				this->hErrorList->Add( this->makeMrcError("JpegSignAccessUserNand") );
			}
			// photoアクセスするのにTCLライブラリを使用していないとき
			if( (this->pRomHeader->s.access_control.photo_access_read  != 0) ||
				(this->pRomHeader->s.access_control.photo_access_write != 0) )
			{
				System::Boolean useTcl = false;
				for each( RCLicense ^lic in this->hLicenseList )
				{
					if( lic->Publisher->StartsWith("NINTENDO") && lic->Name->StartsWith("TCL") )
					{
						useTcl = true;
					}
				}
				if( !useTcl )
				{
					this->hErrorList->Add( this->makeMrcError("PhotoTclAccessUser") );
				}
			}
		}

		if( this->pRomHeader->s.access_control.common_client_key_for_debugger_sysmenu != 0 )
		{
			this->hErrorList->Add( this->makeMrcError("IllegalAccessUser", "Common Client Key for the debugger system menu") );
		}

		u32 okbits = 0x80007FFF;
		u32 *p = (u32*)&(this->pRomHeader->s);
		if( p[ 0x1b4 / 4 ] & ~okbits )
		{
			this->hErrorList->Add( this->makeMrcError("AccessDefault") );
		}
	}
} //RCSrl::mrcAccessControl()

// -------------------------------------------------------------------
// パディングのチェック
// -------------------------------------------------------------------
void RCSrl::mrcPadding(FILE *fp)
{
	u32  romsize = 1 << (this->pRomHeader->s.rom_size);	// ROM容量
	u32  padsize = 0;	// 単位はByte
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
	u32  filesize = ftell(fp);	// 実ファイルサイズ(単位Mbit)
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
// 予約領域
// -------------------------------------------------------------------
void RCSrl::mrcReservedArea(FILE *fp)
{
	// 予約領域の情報はリストに格納されている
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
// Shared2ファイル
// -------------------------------------------------------------------
void RCSrl::mrcShared2(FILE *fp)
{
	// ROMヘッダ中のアクセス許可ビットが立っていないとき
	// Shared2ファイルサイズが定義されていないことを確認
	if( !this->IsShared2 )
	{
		if( (this->hShared2SizeArray[0] != 0) || (this->hShared2SizeArray[1] != 0) || (this->hShared2SizeArray[2] != 0) || 
			(this->hShared2SizeArray[3] != 0) || (this->hShared2SizeArray[4] != 0) || (this->hShared2SizeArray[5] != 0) )
		{
			this->hErrorList->Add( this->makeMrcError("Shared2Disable") );
		}
		return;
	}

	// アクセス許可ビットが立っているとき
	// ファイルサイズのいずれかが設定されているかをまず確認する
	if( (this->hShared2SizeArray[0] == 0) && (this->hShared2SizeArray[1] == 0) && (this->hShared2SizeArray[2] == 0) && 
		(this->hShared2SizeArray[3] == 0) && (this->hShared2SizeArray[4] == 0) && (this->hShared2SizeArray[5] == 0) )
	{
		this->hErrorList->Add( this->makeMrcError("Shared2Enable") );
		return;
	}

	// アクセス許可ビットが立っているとき
	// ファイルごとにアクセス許可されているかどうかとファイルサイズの値を確認
	int i;
	for( i=0; i < METWL_NUMOF_SHARED2FILES; i++ )
	{
		if( this->hShared2SizeArray[i] > 0 )	// 0以外の値が設定されている = そのファイルを使用したい を意味する
		{
			if( !this->hMrcExternalCheckItems->hIsPermitShared2Array[i] )
			{
				// 一般公開されていないのにファイルを使用する場合を考慮してシステムアプリのときのメッセージを変更する
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
				else
				{
					this->hWarnList->Add( this->makeMrcError("Shared2SizeUser", 
										                     i.ToString(), MasterEditorTWL::transSizeToString(this->hShared2SizeArray[i])) );
				}
			}
		} //if( this->hShared2SizeArray[i] > 0 )
	} //for
} //RCSrl::mrcShared2()

// -------------------------------------------------------------------
// SDKバージョンのチェック
// -------------------------------------------------------------------
void RCSrl::mrcSDKVersion(FILE *fp)
{
	// SDKバージョン
	System::Boolean isOld  = false;
	System::Boolean isOldExTWL = false;
	System::Boolean isOldExNTR = false;
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
				isOld = MasterEditorTWL::IsOldSDKVersion( sdk->Code, this->hMrcExternalCheckItems->SDKVer, false );
			}
		}
		else	// 常駐モジュール以外
		{
			if( this->hMrcExternalCheckItems->IsAppendCheck )
			{
				u32 major = sdk->Code >> 24;
				if( major >= 5 )	// TWLSDK
				{
					isOldExTWL = MasterEditorTWL::IsOldSDKVersion( sdk->Code, this->hMrcExternalCheckItems->SDKVerNotStaticTWL, false );
				}
				else				// NTRSDK
				{
					isOldExNTR = MasterEditorTWL::IsOldSDKVersion( sdk->Code, this->hMrcExternalCheckItems->SDKVerNotStaticNTR, false );
				}
			}
		}
	}
	if( isOld )
	{
		this->hWarnList->Add( this->makeMrcError("SDKVersionOld") );
	}
	if( isOldExTWL )
	{
		this->hWarnList->Add( this->makeMrcError("SDKVersionExModuleTwlOld") );
	}
	if( isOldExNTR )
	{
		this->hWarnList->Add( this->makeMrcError("SDKVersionExModuleNtrOld") );
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
// バナー文字のチェック
// -------------------------------------------------------------------
void RCSrl::mrcBanner(FILE *fp)
{
	u8 mapWorldwide[ 0x10000 ];
	u8 mapChina[ 0x10000 ];
	u8 mapKorea[ 0x10000 ];
	int i;

	// バナーオフセットにエラーがあるときには調べない
	if( this->pRomHeader->s.banner_offset == 0 )
	{
		return;
	}

	// 使用可能な文字のマップをつくる
	// 文字コードをインデックスとして使用可能なら 1 が入る
	// そうでないなら 0 が入る
	for( i=0; i < 0x10000; i++ )
	{
		mapWorldwide[i] = 0;
		mapChina[i] = 0;
		mapKorea[i] = 0;
	}
	for( i=0; i < MasterEditorTWL::NumOf_BannerCharCodes_Worldwide; i++ )
	{
		mapWorldwide[ MasterEditorTWL::BannerCharCodes_Worldwide[i] ] = 1;
	}
	for( i=0; i < MasterEditorTWL::NumOf_BannerCharCodes_China; i++ )
	{
		mapChina[ MasterEditorTWL::BannerCharCodes_China[i] ] = 1;
	}
	for( i=0; i < MasterEditorTWL::NumOf_BannerCharCodes_Korea; i++ )
	{
		mapKorea[ MasterEditorTWL::BannerCharCodes_Korea[i] ] = 1;
	}
	mapWorldwide[ 0x0000 ] = 1;		// 0x0000は許される
	mapChina[ 0x0000 ]     = 1;
	mapKorea[ 0x0000 ]     = 1;

	// バナーを読み込む
	u32 size = this->pRomHeader->s.banner_size;
	u8 *banner = new u8[size];
	fseek( fp, this->pRomHeader->s.banner_offset, SEEK_SET );
	if( size != fread( banner, 1, size, fp ) )
	{
		this->hErrorList->Add( this->makeMrcError("BannerRead") );
	}

	// バナーバージョン
	u8 banner_version = banner[0];

	// バナーの各文字がマップに登録されているかチェック
	System::Boolean bResultWorldwide = true;
	System::Boolean bResultChina     = true;
	System::Boolean bResultKorea     = true;
	int  start = 0x240;
	int  end   = (size < 0x1240)?(size):(0x1240);	// NTR互換領域までのときはTWL拡張領域をサーチしない
	for( i=0x240; i < end; i+=2 )
	{
		u16 code = banner[i+1];
		code = (code << 8) + banner[i];

		// 中国のフォント箇所
		if( (0x840 <= i) && (i < 0x940) && (banner_version >= 2) )	// バナーバージョンが1のときは中国版バナーは読み込まれない
		{
			if( (mapWorldwide[ code ] == 0) && (mapChina[ code ] == 0) )	// ワールドワイド版が使われててもOK
			{
				bResultChina = false;
			}
		}
		// 韓国のフォント箇所
		else if( (0x940 <= i) && (i < 0xA40) && (banner_version >= 3) )
		{
			if( (mapWorldwide[ code ] == 0) && (mapKorea[ code ] == 0) )
			{
				bResultKorea = false;
			}
		}
		// ワールドワイド版
		else
		{
			if( mapWorldwide[ code ] == 0 )
			{
				bResultWorldwide = false;
			}
		}
	}
	if( !bResultWorldwide )
	{
		//if( memcmp( this->pRomHeader->s.game_code, "HNB", 3 ) != 0 )	// 本体設定は特別に許される
		//{
			this->hErrorList->Add( this->makeMrcError("BannerChar") );
		//}
	}
	if( !bResultChina )
	{
		this->hErrorList->Add( this->makeMrcError("ChinaBannerCharSet") );
	}
	if( !bResultKorea )
	{
		this->hErrorList->Add( this->makeMrcError("KoreaBannerCharSet") );
	}
	delete []banner;
} //RCSrl::mrcBanner()
