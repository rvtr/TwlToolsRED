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

extern const u8 gBannerFont[];
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
	this->hErrorList->AddRange( this->hParentalErrorList );		// ペアレンタルコントロールのリストは別につくっているので追加
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

	// ROMヘッダのチェック

	// 文字コードチェック
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
			"ソフトタイトル", 0x0, 0xb, "使用不可のASCIIコードが使用されています。",
			"Game Title", "Unusable ASCII code is used.", false, true ) );
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
		this->hErrorList->Add( gcnew RCMrcError( 
			"ソフトタイトル", 0x0, 0xb, "末尾の未使用部分には00hを登録してください。",
			"Game Title", "Please use 00h for an unused part.", false, true ) );
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
		this->hErrorList->Add( gcnew RCMrcError( 
			"ソフトタイトル", 0x0, 0xb, "スペース部分には20hを登録してください。",
			"Game Title", "Please use 20h for an space part.", false, true ) );
	}

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
		this->hErrorList->Add( gcnew RCMrcError( 
			"イニシャルコード", 0xc, 0xf, "使用不可のASCIIコードが使用されています。",
			"Game Code", "Unusable ASCII code is used.", false, true ) );
	}
	if( memcmp( this->pRomHeader->s.game_code, "NTRJ", GAME_CODE_MAX ) == 0 )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			"イニシャルコード", 0xc, 0xf, "SDKデフォルトのコード(NTRJ)が使用されています。",
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
			"メーカーコード", 0x10, 0x11, "使用不可のASCIIコードが使用されています。",
			"Maker Code", "Unusable ASCII code is used.", false, true ) );
	}

	// 値チェック

	if( this->pRomHeader->s.rom_type != 0x00 )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			"デバイスタイプ", 0x13, 0x13, "不正な値です。00hを設定してください。",
			"Device Type", "Invalid data. Please set 00h.", false, true ) );
	}

	u8 romver = this->pRomHeader->s.rom_version;
	if( ((romver < 0x00) || (0x0f < romver)) && (romver != 0xE0) )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			"リマスターバージョン", 0x1e, 0x1e, "不正な値です。正式版では01h-0Fhのいずれかの値、事前版ではE0hを設定してください。",
			"Release Ver.", "Invalid data. Please set either one of 01h-0Fh(Regular ver.), or E0h(Preliminary ver.)", false, true ) );
	}
	if( this->pRomHeader->s.banner_offset == 0 )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			"バナーオフセット", 0x68, 0x6b, "バナーデータが設定されていません。",
			"Banner Offset.", "Banner data is not set.", false, true ) );
	}
	if( this->pRomHeader->s.rom_valid_size == 0 )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			"ROM実効サイズ", 0x80, 0x83, "値が設定されていません。",
			"ROM Valid Size.", "Data is not set.", false, true ) );
	}

	// CRC
	u16  crc;
	// セキュア領域
	if( !this->IsMediaNand )	// カードアプリのときのみ
	{
		// セキュア領域はROMヘッダ外
		u8     *secures;
		s32     secure_size = SECURE_AREA_END - this->pRomHeader->s.main_rom_offset;
		if (secure_size > SECURE_AREA_SIZE)
		{
			secure_size = SECURE_AREA_SIZE;
		}
		if (secure_size <= 0)
		{
			this->hErrorList->Add( gcnew RCMrcError( 
				"セキュア領域CRC", 0x15e, 0x15f, "セキュア領域のアドレス指定が不正です。",
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
				"セキュア領域CRC", 0x07c, 0x07d, "計算結果と一致しません。セキュア領域が改ざんされた可能性があります。",
				"Secure Area CRC.", "Calclated CRC is different from Registered one.", false, true ) );
		}
	}

	// ロゴ領域
	crc = 0xcf56;
	if( crc != this->pRomHeader->s.nintendo_logo_crc16 )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			"キャラクターデータCRC", 0x15c, 0x15d, "計算結果と一致しません。キャラクターデータが改ざんされた可能性があります。",
			"Charactor Data CRC.", "Calclated CRC is different from Registered one.", false, true ) );
	}

	// ヘッダCRC
	crc = calcCRC( CRC16_INIT_VALUE, (u8*)this->pRomHeader, CALC_CRC16_SIZE );
	if( crc != this->pRomHeader->s.header_crc16 )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			"ROMヘッダCRC", 0x15e, 0x15f, "計算結果と一致しません。ROMヘッダが改ざんされた可能性があります。",
			"ROM Header CRC.", "Calclated CRC is different from Registered one.", false, true ) );
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
		this->hErrorList->Add( gcnew RCMrcError( 
			"システムコールライブラリ", METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE, "SDKデフォルトです。",
			"System-Call Library", "This Library is SDK default one.", false, true ) );
	}
	return ECSrlResult::NOERROR;
} // mrcNTR()

// -------------------------------------------------------------------
// TWL専用MRC
// -------------------------------------------------------------------
ECSrlResult RCSrl::mrcTWL( FILE *fp )
{
	// ROMヘッダのチェック (NTR互換領域)

#ifdef METWL_WHETHER_PLATFORM_CHECK
	// プラットフォームのチェック
	if( (this->pRomHeader->s.platform_code != PLATFORM_CODE_TWL_HYBLID) && 
		(this->pRomHeader->s.platform_code != PLATFORM_CODE_TWL_LIMITED) &&
		(this->pRomHeader->s.enable_signature == 0) )
	{
		return ECSrlResult::ERROR_PLATFORM;
	}
#endif

	if( this->IsOldDevEncrypt && this->HasDSDLPlaySign )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			"旧開発用暗号フラグ", 0x1c, 0x1c,
			"このROMはクローンブート対応アプリですが、製品用本体ではクローンブートができなくなります。",
			"Old Development Flag",
			"This ROM supports Clone-Boot, and the flag is old type. Therefore, Clone-Boot can't be done.",
			false, true ) );
	}

	// 値チェック

	fseek( fp, 0, SEEK_END );
	u32  filesize = ftell(fp);	// 実ファイルサイズ
	u32  romsize = 1 << (this->pRomHeader->s.rom_size);	// ROM容量(単位Mbit)
	if( !this->IsMediaNand )		// カードアプリのときのみのチェック
	{
		u32 filesizeMb = (filesize / (1024*1024)) * 8;	// 単位をMbitに直す
		if( romsize < filesizeMb )
		{
			this->hErrorList->Add( gcnew RCMrcError( 
				"デバイス容量", 0x14, 0x14, "実ファイルサイズよりも小さい値が指定されています。",
				"Device Capacity", "Setting data is less than the actual file size.", false, true ) );
		}
		else if( filesizeMb < romsize )
		{
			this->hWarnList->Add( gcnew RCMrcError(		// 警告
				"デバイス容量", 0x14, 0x14, "実ファイルサイズに比べて無駄のある値が設定されています。",
				"Device Capacity", "Setting data is larger than the actual file size.", false, true ) );
		}
		if( (romsize < METWL_ROMSIZE_MIN) || (METWL_ROMSIZE_MAX < romsize) )
		{
			this->hErrorList->Add( gcnew RCMrcError( 
				"デバイス容量", 0x14, 0x14, "指定可能な容量ではありません。",
				"Device Capacity", "Invalid capacity.", false, true ) );
		}
		if( (filesizeMb < 1) || (MasterEditorTWL::countBits(filesizeMb) != 1) )
		{
			this->hWarnList->Add( gcnew RCMrcError( 
				"実ファイルサイズ", METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE, "中途半端な値です。通常では2のべき乗[Mbit]の値です。",
				"Actual File Size", "Invalid size. This size is usually power of 2.", false, true ) );
		}
		// 1Gbit以上のときの最終領域が固定値かどうか
		this->mrcPadding( fp );

		// セグメント3のCRC
		u16  crcseg3;
		BOOL crcret = getSeg3CRCInFp( fp, &crcseg3 );
		if( !crcret || (crcseg3 != METWL_SEG3_CRC) )
		{
			this->hErrorList->Add( gcnew RCMrcError( 
				"セグメント3 CRC", METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE, 
				"セグメント3領域に誤りがあります。",
				"Segment-3 CRC", "Invalid data exist in Segment-3 area.", false, true ) );
		}

		u16  NA = this->pRomHeader->s.twl_card_normal_area_rom_offset & 0x7fffUL;
		u16  KA = this->pRomHeader->s.twl_card_keytable_area_rom_offset;
		if( (NA == 0) || (KA == 0) || (NA > KA) )
		{
			this->hErrorList->Add( gcnew RCMrcError( 
				"TWL ROMオフセット", 0x90, 0x93,
				"TWLノーマル領域ROMオフセット(NA)およびTWL専用領域ROMオフセット(KA)はともに1以上で、かつNAはKAを超えてはいけません。",
				"TWL ROM Offset", 
				"Both TWL Normal Area ROM Offset(NA) and TWL Limited Area ROM Offset(KA) are bigger than 0. And NA must be smaller than KA, or equals to KA.",
				false, true ) );
		}
	} //if( *(this->hIsNAND) == false )
	else	// NANDアプリのときのみのチェック
	{
		if( (romsize < METWL_ROMSIZE_MIN_NAND) || (METWL_ROMSIZE_MAX_NAND < romsize) )
		{
			this->hErrorList->Add( gcnew RCMrcError( 
				"デバイス容量", 0x14, 0x14, "NANDアプリに対して指定可能な容量ではありません。",
				"Device Capacity", "Invalid capacity.", false, true ) );
		}
		u32  allsize = filesize + this->pRomHeader->s.public_save_data_size + this->pRomHeader->s.private_save_data_size;
		if( allsize > METWL_ALLSIZE_MAX_NAND )
		{
			this->hErrorList->Add( gcnew RCMrcError( 
				"実ファイルサイズ", METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE,
				"ROMデータの実ファイルサイズとPublicセーブデータおよびPrivateセーブデータのサイズの総和が32MByteを超えています。",
				"Actual File Size", 
				"The sum of this size, the public save data size and private save data size exceed 32MByte.", false, true ) );
		}
	}

	if( this->IsAppLauncher || this->IsAppSecure || this->IsAppSystem )
	{
		if( this->pRomHeader->s.disable_debug == 0 )
		{
			this->hWarnList->Add( gcnew RCMrcError( 
				"デバッガ動作禁止フラグ", 0x1f, 0x1f, 
				"デバッガ上で解析可能な設定になっています。デバッガ向けソフトとしてROMをリリースする場合にはセキュリティ上の問題がないかご確認ください。",
				"Disable Launch on Debugger Flag", 
				"This ROM can be analyzed on the debugger. If the ROM is released for the debugger, please check the security settings.", 
				false, true ) );
		}
	}
	else	// ユーザのときのみ
	{
		if( this->pRomHeader->s.disable_debug != 0 )
		{
			this->hErrorList->Add( gcnew RCMrcError( 
				"デバッガ動作禁止フラグ", 0x1f, 0x1f, 
				"デバッガ上で解析不可な設定になっています。この設定は許可されていません。",
				"Disable Launch on Debugger Flag", 
				"This ROM can't be analyzed on the debugger. This setting is unavailable.", 
				false, true ) );
		}

		u8 okbits = 0x02 | 0x40 | 0x80;
		u8 *p = (u8*)&(this->pRomHeader->s);
		if( p[0x1d] & ~okbits )
		{
			this->hErrorList->Add( gcnew RCMrcError( 
				"特定仕向地", 0x1d, 0x1d, 
				"不正なビットが立っています。この設定は許可されていません。",
				"Extra Region Code", 
				"Illegal bit is setting. This setting is unavailable.", 
				false, true ) );
		}
	}

	if( this->pRomHeader->s.warning_no_spec_rom_speed != 0 )
	{
		this->hWarnList->Add( gcnew RCMrcError( 
			"諸フラグ", 0x1f, 0x1f, "rsfファイルでROMSpeedTypeが設定されていません。",
			"Setting Flags", "In a RSF file, the item \"ROMSpeedType\" is not set.", false, true ) );
	}
	if( (this->pRomHeader->s.game_cmd_param & CARD_LATENCY_MASK) != CARD_1TROM_GAME_LATENCY )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			"ROMコントロール情報", 0x60, 0x67, "TWLではマスクROMは用意されていません。ワンタイムPROM設定にしてください。",
			"ROM Control Info.", "Mask ROM can be set. Please set One-time PROM.", false, true ) );
	}

	if( !this->IsMediaNand )
	{
		if( (this->pRomHeader->s.enable_aes == 0) || (this->pRomHeader->s.aes_target_size == 0) )
		{
			this->hErrorList->Add( gcnew RCMrcError( 
				"AES暗号", 0x60, 0x67, "AES暗号が無効になっています。セキュリティ上の問題があります。",
				"AES Encryption", "AES Encryption is disable. It is a security problem.", false, true ) );
		}
	}
	else		// NANDアプリ
	{
		if( (this->pRomHeader->s.enable_aes == 0) || (this->pRomHeader->s.aes_target_size == 0) || (this->pRomHeader->s.aes_target2_size == 0) )
		{
			this->hErrorList->Add( gcnew RCMrcError( 
				"AES暗号", 0x60, 0x67, "AES暗号が無効になっています。セキュリティ上の問題があります。",
				"AES Encryption", "AES Encryption is disable. It is a security problem.", false, true ) );
		}
	}

	// ROMヘッダのチェック (TWL専用領域)

	// アプリ種別のチェック
	this->mrcAppType(fp);

	// 各種デバイスへのアクセスフラグのチェック
	this->mrcAccessControl(fp);
	this->mrcShared2(fp);

	if( this->IsWL )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			"TWL拡張諸フラグ", 0x1bf, 0x1bf, 
			"本来、立つはずのないフラグが立てられています。ビルド時の設定をご確認ください。",
			"TWL Extended Flag",
			"Unavailable flag is set. Please check building option.",
			false, true ) );
	}

	if( this->IsWiFiIcon && this->IsWirelessIcon )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			"アイコン表示フラグ", 0x1bf, 0x1bf, 
			"ワイヤレス通信アイコンとWi-Fi通信アイコンは同時に表示できません。",
			"Icon Displaying Flag",
			"Icon displayed on menu is either Wireless Icon or Wi-Fi Connection Icon.",
			false, true ) );
	}

	if( !this->IsMediaNand )	// カードアプリのときのみ
	{
		u32 ideal  = (this->pRomHeader->s.twl_card_keytable_area_rom_offset * 0x80000) + 0x3000;	// TWL KeyTable領域の開始 + KeyTableサイズ
		u32 actual = this->pRomHeader->s.main_ltd_rom_offset;
		if( ideal > actual )
		{
			this->hErrorList->Add( gcnew RCMrcError( 
				"拡張常駐モジュール", 0x1c0, 0x1c3,
				"TWL拡張常駐モジュールの配置に問題があります。弊社窓口にご連絡ください。",
				"Extended Static Module",
				"Alignment of TWL Extended Static Module is illegal. Please contact us, sorry.",
				false, true ) );
		}
	}

	if( !this->IsAppLauncher && this->IsSCFGAccess )
	{
		this->hWarnList->Add( gcnew RCMrcError( 
			"SCFG設定", 0x1b8, 0x1bb, "SCFGレジスタへアクセス可能になっています。",
			"SCFG Setting", "In this setting, SCFG register is accessible.", false, true ) );
	}

	if( (this->pRomHeader->s.titleID_Lo[0] != this->pRomHeader->s.game_code[3]) ||
		(this->pRomHeader->s.titleID_Lo[1] != this->pRomHeader->s.game_code[2]) ||
		(this->pRomHeader->s.titleID_Lo[2] != this->pRomHeader->s.game_code[1]) ||
		(this->pRomHeader->s.titleID_Lo[3] != this->pRomHeader->s.game_code[0]) )
	{
		this->hWarnList->Add( gcnew RCMrcError( 
			"タイトルID", 0x230, 0x233, "下位4バイトがイニシャルコードと一致しません。",
			"Title ID", "Lower 4 bytes don't match ones of Game Code.", false, true ) );
	}

	// ROMヘッダ以外の領域のチェック

	this->mrcBanner( fp );
	this->mrcReservedArea(fp);

	for each( RCLicense ^lic in this->hLicenseList )
	{
		if( lic->Publisher->Equals( "NINTENDO" ) && lic->Name->Equals( "DEBUG" ) )
		{
			this->hErrorList->Add( gcnew RCMrcError( 
				"Debug/Releaseビルド", METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE,
				"FINALROMビルドのROMではありません。必ずFINALROMビルドのROMを作成してご提出ください。",
				"Debug/Release Build",
				"This ROM isn't FINALROM-build. Please submit a FINALROM-build ROM.", false, true ) );
		}
	}

	// 追加チェック
	if( this->hMrcExternalCheckItems->IsAppendCheck )
	{
		this->mrcSDKVersion(fp);
	} //if( *(this->hMrcExternalCheckItems->hIsCheck) )

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
	System::String ^appstrJ = gcnew System::String("");
	System::String ^appstrE = gcnew System::String("");

	idH = this->pRomHeader->s.titleID_Hi;
	memcpy( idL, &(this->pRomHeader->s.titleID_Lo[0]), 4 );

	if( (idL[3]=='H') && (idL[2]=='N') && (idL[1]=='A') )   // ランチャアプリかどうかはTitleID_Loの値で決定
	{
		apptype = appLauncher;
		appstrJ = "ランチャーアプリ";
		appstrE = "Launcher-App.";
	}
	else if( idH & TITLE_ID_HI_SECURE_FLAG_MASK )           // 立ってたらセキュアアプリ
	{
		apptype = appSecure;
		appstrJ = "セキュアアプリ";
		appstrE = "Secure-App.";
	}
	else if( (idH & TITLE_ID_HI_APP_TYPE_MASK) == 1 )       // 立ってたらシステムアプリ
	{
		apptype = appSystem;
		appstrJ = "システムアプリ";
		appstrE = "System-App.";
	}
	else if( (idH & TITLE_ID_HI_APP_TYPE_MASK) == 0 )       // 残るはユーザアプリ
	{
		apptype = appUser;
		appstrJ = "ユーザアプリ";
		appstrE = "User-App.";
	}
	else
	{
		apptype = -1;
	}

	if( apptype < 0 )
	{
		this->hWarnList->Add( gcnew RCMrcError( 
			"アプリ種別", 0x230, 0x237, "不正な値です。",
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
			"アプリ種別", 0x230, 0x237, "このROMデータは" + appstrJ + "です。本プログラムでは非対応です。",
			"Application Type", "This ROM is " + appstrE + " which is unsurpported by this editor.", false, true ) );
	}
} //RCSrl::mrcAppType()

// -------------------------------------------------------------------
// アクセスコントロール情報のチェック
// -------------------------------------------------------------------
void RCSrl::mrcAccessControl(FILE *fp)
{
	if( this->IsAppLauncher || this->IsAppSecure || this->IsAppSystem )
	{
		if( (this->pRomHeader->s.access_control.game_card_on != 0) &&
			(this->pRomHeader->s.access_control.game_card_nitro_mode != 0) )
		{
			this->hErrorList->Add( gcnew RCMrcError( 
				"アクセスコントロール情報", 0x1b4, 0x1b7, "ゲームカードアクセス設定にノーマルモードとNTRモードの両方を設定することはできません。",
				"Access Control Info.", "Game card access setting is either normal mode or NTR mode.", false, true ) );
		}
	}
	else
	{
		if( this->pRomHeader->s.access_control.sd_card_access != 0 )
		{
			this->hErrorList->Add( gcnew RCMrcError( 
				"アクセスコントロール情報", 0x1b4, 0x1b7,
				"SDカードへのアクセスは許可されていません。",
				"Access Control Info.",
				"Application for Game Card doesn't access to NAND frash memory.",
				false, true ) );
		}
		if( !this->IsMediaNand )	// カードアプリのときのみ
		{
			if( this->pRomHeader->s.access_control.nand_access != 0 )
			{
				this->hErrorList->Add( gcnew RCMrcError( 
					"アクセスコントロール情報", 0x1b4, 0x1b7,
					"ゲームカード用ソフトは、NANDフラッシュメモリへのアクセスを許可されていません。",
					"Access Control Info.",
					"Application for Game Card doesn't access to NAND frash memory.",
					false, true ) );
			}
		}
		u32 okbits;
		if( !this->IsMediaNand )
		{
			okbits = 0x00000008 | 0x00000010 | 0x00000040;
		}
		else
		{
			okbits = 0x00000008 | 0x00000010 | 0x00000040 | 0x00000400;		// NANDアプリのときはJpegSignフラグは許される
		}
		u32 *p = (u32*)&(this->pRomHeader->s);
		if( p[ 0x1b4 / 4 ] & ~okbits )
		{
			this->hErrorList->Add( gcnew RCMrcError( 
				"アクセスコントロール情報", 0x1b4, 0x1b7,
				"許可されていないアクセスが設定されています。この設定は許可されていません。",
				"Access Control Info.",
				"Illegal Access is setting. This setting is unavailable.",
				false, true ) );
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
	System::String ^padstrJ = gcnew System::String("");
	System::String ^padstrE = gcnew System::String("");
	System::String ^romstr  = gcnew System::String("");

	switch( romsize )
	{
		case 1024:
			padsize = 0x00280000;	// 20Mbit = 2.5MB
			padstrJ = "最終20Mbit";
			padstrE = "Last 20Mbit";
			romstr  = "1Gbit";
		break;
		case 2048:
			padsize = 0x00500000;	// 40Mbit = 5MB
			padstrJ = "最終40Mbit";
			padstrE = "Last 40Mbit";
			romstr  = "2Gbit";
		break;
		case 4096:
			padsize = 0x00A60000;	// 83Mbit = 10.375MB
			padstrJ = "最終83Mbit";
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
	u32  filesize = ftell(fp);	// 実ファイルサイズ(単位Mbit)
	u32  offset = ((romsize / 8) * 1024 * 1024) - padsize;
	if( (offset + padsize) > filesize )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			padstrJ, METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE,
			"読み出すことができませんでした。",
			padstrE, "This area couldn't be read", false, true ) );
		return;
	}

	u8 *buf = new u8[ padsize ];
	fseek( fp, offset, SEEK_SET );
	if( padsize != fread( buf, 1, padsize, fp ) )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			padstrJ, METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE,
			"読み出すことができませんでした。",
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
			"FFh以外の値が格納されています。 " + romstr + " ROMの容量制限が守られていません。",
			padstrE,
			"This area includes illegai data. " + romstr + " Volume constraint is violated.",
			false, true ) );
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
			this->hErrorList->Add( gcnew RCMrcError( 
				"予約領域", begin, end, "不正な値が含まれています。この領域をすべて0で埋めてください。",
				"Reserved Area", "Invalid data is included. Please set 0 into this area.", false, true ) );
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
			this->hErrorList->Add( gcnew RCMrcError( 
				"Shared2ファイル", METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE,
				"Shared2ファイルへのアクセスが指定されていないにもかかわらずファイルサイズが設定されています。",
				"Shared2 File",
				"Although access to Shared2 files is not setting, file size is setting.",
				false, true ) );
		}
		return;
	}

	// アクセス許可ビットが立っているとき
	// ファイルサイズのいずれかが設定されているかをまず確認する
	if( (this->hShared2SizeArray[0] == 0) && (this->hShared2SizeArray[1] == 0) && (this->hShared2SizeArray[2] == 0) && 
		(this->hShared2SizeArray[3] == 0) && (this->hShared2SizeArray[4] == 0) && (this->hShared2SizeArray[5] == 0) )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			"Shared2ファイル", METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE,
			"Shared2ファイルへのアクセスが指定されているにもかかわらずファイルサイズが設定されていません。",
			"Shared2 File",
			"Although access to Shared2 files is setting, file size is not setting.",
			false, true ) );
		return;
	}

	// アクセス許可ビットが立っているとき
	// ファイルごとにアクセス許可されているかどうかとファイルサイズの値を確認
	int i;
	for( i=0; i < METWL_NUMOF_SHARED2FILES; i++ )
	{
		if( this->hShared2SizeArray[i] > 0 )	// 0以外の値が設定されている = そのファイルを使用したい を意味する
		{
			System::String ^filenoJ = "Shared2ファイル(No." + i.ToString() + ")";
			System::String ^filenoE = "Shared File(No." + i.ToString() + ")";
			if( !this->hMrcExternalCheckItems->hIsPermitShared2Array[i] )
			{
				this->hErrorList->Add( gcnew RCMrcError( 
					filenoJ, METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE,
					filenoJ + "へのアクセスは許可されていません。",
					filenoE,
					"Access to " + filenoE + " is not permitted.",
					false, true ) );
			}
			if( this->hShared2SizeArray[i] != this->hMrcExternalCheckItems->hShared2SizeArray[i] )
			{
				this->hErrorList->Add( gcnew RCMrcError( 
					filenoJ, METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE,
					filenoJ + "のファイルサイズに不正な値が設定されています。",
					filenoE,
					"Illegal file size of " + filenoE + ".",
					false, true ) );
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
	System::Boolean isPR   = false;
	System::Boolean isRC   = false;
	for each( RCSDKVersion ^sdk in this->hSDKList )
	{
		if( sdk->IsStatic )
		{
			isPR  = MasterEditorTWL::IsSDKVersionPR( sdk->Code );
			isRC  = MasterEditorTWL::IsSDKVersionRC( sdk->Code );
			isOld = MasterEditorTWL::IsOldSDKVersion( sdk->Code, this->hMrcExternalCheckItems->SDKVer );
		}
	}
	if( isOld )
	{
		this->hWarnList->Add( gcnew RCMrcError( 
			"SDKバージョン", METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE, "設定ファイルに登録されているバージョンよりも古いバージョンです。",
			"SDK Version", "Older version (comparing with a setting file)", false, true ) );
	}
	if( isPR )
	{
		this->hWarnList->Add( gcnew RCMrcError( 
			"SDKバージョン", METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE, "ご使用のSDKバージョンはPR版です。",
			"SDK Version", "Used version is PR.", false, true ) );
	}
	if( isRC )
	{
		this->hWarnList->Add( gcnew RCMrcError( 
			"SDKバージョン", METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE, "ご使用のSDKバージョンはRC版です。",
			"SDK Version", "Used version is RC.", false, true ) );
	}
}

// -------------------------------------------------------------------
// バナー文字のチェック
// -------------------------------------------------------------------
void RCSrl::mrcBanner(FILE *fp)
{
	u8  map[ 0x10000 ];
	u32 i;

	// 本体設定だけはガイドライン無視
	if( memcmp( this->pRomHeader->s.game_code, "HNB", 3 ) == 0 )
	{
		//this->hWarnList->Add( gcnew RCMrcError( 
		//	"バナーファイル", METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE,
		//	"本体設定アプリに限ってはバナーの文字コードチェックをスキップします。",
		//	"Banner File",
		//	"Only a machine setting app., a charactor code check of the banner file is skip.",
		//	false, true ) );
		return;
	}

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
		map[i] = 0;
	}
	for( i=2; ; i+=2 )	// 0個目の要素はマジックコードなので含めない
	{
		u16 index = (u16)gBannerFont[i+1];
		index = (index << 8) + gBannerFont[i];
		if( index == 0xFEFF )
		{
			break;
		}
		map[ index ] = 1;
	}
	map[ 0x0000 ] = 1;		// 0x0000は許される

	// バナーを読み込む
	u32 size = this->pRomHeader->s.banner_size;
	u8 *banner = new u8[size];
	fseek( fp, this->pRomHeader->s.banner_offset, SEEK_SET );
	if( size != fread( banner, 1, size, fp ) )
	{
		this->hErrorList->Add( gcnew RCMrcError( 
			"バナーファイル", METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE,
			"読み出すことができませんでした。",
			"Banner File",
			"The file can't be read.",
			false, true ) );
	}

	// バナーの各文字がマップに登録されているかチェック
	System::Boolean bResult = true;
	u32  start = 0x240;
	u32  end   = (size < 0x1240)?(size):(0x1240);	// NTR互換領域までのときはTWL拡張領域をサーチしない
	for( i=0x240; i < end; i+=2 )
	{
		if( (0x840 <= i) && (i < 0xA40) )	// 中韓のフォント箇所はチェックしない
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
			"バナーファイル", METWL_ERRLIST_NORANGE, METWL_ERRLIST_NORANGE,
			"使用不可能な文字セットが使用されています。",
			"Banner File",
			"A set of illegal charactor code is used.",
			false, true ) );
	}
	delete []banner;
} //RCSrl::mrcBanner()

// バナーとして使用可能なフォントリスト(u16リトルエンディアン)
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
	0xff, 0xfe,		// 終わりを表す意味で0xFEFFを最後につける
};
