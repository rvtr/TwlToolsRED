#pragma once

// ROMデータ(SRL)クラスの宣言

#include <apptype.h>
#include "common.h"
#include <twl/types.h>
#include <twl/os/common/format_rom.h>
#include <twl/os/common/ownerInfoEx.h>
#include "srl_const.h"
#include "srl_element.h"
#include "utility.h"

namespace MasterEditorTWL
{
	// -------------------------------------------------------------------
	// Type : ref class
	// Name : RCSrl
	//
	// Description : ROMデータ(SRL)の設定情報クラス
	// 
	// Role : ROMデータのファイル入出力・内部情報の更新
	// -------------------------------------------------------------------
	ref class RCSrl
	{
		// field
	private:
		// ROMヘッダ
		ROM_Header *pRomHeader;

	public:
		// (GUIに表示される)ROMヘッダ固有情報

		// NTR互換情報 ReadOnly
		property System::String  ^hTitleName;
		property System::String  ^hGameCode;
		property System::String  ^hMakerCode;
		property System::String  ^hPlatform;
		property System::String  ^hRomSize;
		property System::Byte     RomVersion;
		property System::UInt16   HeaderCRC;
		property System::String  ^hLatency;		// MROM/1TROM/Illegal

		// ペアレンタルコントロール
		property cli::array<System::Int32> ^hArrayParentalIndex;	// 表示用のコンボボックスのインデックス
		property System::Boolean  IsUnnecessaryRating;				// レーティング表示不要フラグ

		// TWL専用情報 Read Only
		property System::UInt32   NormalRomOffset;
		property System::UInt32   KeyTableRomOffset;
		property System::String  ^hTitleIDLo;
		property System::UInt32   TitleIDHi;
		property System::Boolean  IsAppLauncher;	// TitleIDLoからわかるアプリ種別
		property System::Boolean  IsAppUser;		// TitleIDHiからわかるアプリ種別
		property System::Boolean  IsAppSystem;		//
		property System::Boolean  IsAppSecure;		//
		property System::Boolean  IsLaunch;			//
		property System::Boolean  IsMediaNand;		//
		property System::Boolean  IsDataOnly;		//
		property System::UInt16   PublisherCode;	//
		property System::Boolean  IsNormalJump;
		property System::Boolean  IsTmpJump;
		property System::Boolean  HasDSDLPlaySign;	// ROMヘッダ外のSRLからわかる署名の有無
		property System::Boolean  IsOldDevEncrypt;	// 旧開発用暗号フラグが立っている
		property System::Boolean  IsSCFGAccess;		// SCFGレジスタをロックしている
		property System::UInt32   PublicSize;		// セーブデータサイズ
		property System::UInt32   PrivateSize;

		// NAND消費サイズ
		RCNandUsedSize ^hNandUsedSize;

		// TWL拡張フラグ Read Only
		property System::Boolean IsCodecTWL;
		property System::Boolean IsEULA;
		property System::Boolean IsSubBanner;
		property System::Boolean IsWiFiIcon;
		property System::Boolean IsWirelessIcon;
		property System::Boolean IsWL;

		// TWLアクセスコントロール Read Only
		property System::Boolean IsCommonClientKey;
		property System::Boolean IsAesSlotBForES;
		property System::Boolean IsAesSlotCForNAM;
		property System::Boolean IsSD;
		property System::Boolean IsNAND;
		property System::Boolean IsGameCardOn;
		property System::Boolean IsShared2;
		property System::Boolean IsAesSlotBForJpegEnc;
		property System::Boolean IsAesSlotBForJpegEncUser;
		property System::Boolean IsGameCardNitro;
		property System::Boolean IsAesSlotAForSSL;
		property System::Boolean IsCommonClientKeyForDebugger;

		// Shared2ファイルサイズ Read Only
		property cli::array<System::UInt32> ^hShared2SizeArray;

		// カードリージョン Read Only
		property System::Boolean IsRegionJapan;
		property System::Boolean IsRegionAmerica;
		property System::Boolean IsRegionEurope;
		property System::Boolean IsRegionAustralia;

		// SDKバージョンと使用ライブラリのリスト
		property System::Collections::Generic::List<RCSDKVersion^> ^hSDKList;
		property System::Collections::Generic::List<RCLicense^> ^hLicenseList;

		// MRC機能でチェックされたエラー情報のリスト
		property System::Collections::Generic::List<RCMrcError^> ^hErrorList;
		property System::Collections::Generic::List<RCMrcError^> ^hWarnList;
		property System::Collections::Generic::List<RCMrcError^> ^hParentalErrorList;	// 読み込み時に発見されるペアレンタルコントロール情報のエラー
		property System::Collections::Generic::List<RCMrcError^> ^hParentalWarnList;

		// MRC追加項目
		property RCMrcExternalCheckItems ^hMrcExternalCheckItems;

		// constructor / destructor / finalizer 
	public:
		RCSrl();
		~RCSrl();
		!RCSrl();

		// method
	public:

		//
		// ROMヘッダのファイル入出力
		//
		// @arg [in/out] 入出力ファイル名
		//
		ECSrlResult readFromFile ( System::String ^filename );
		ECSrlResult writeToFile( System::String ^filename );
		
		// internal method
	private:
		// ROM固有情報とROMヘッダの設定
		ECSrlResult setRomInfo(void);		// ROMヘッダから取得したROM固有情報をフィールドに反映させる
		ECSrlResult setRomHeader(void);		// ROMヘッダにROM固有情報フィールドの値を反映させる

		// ペアレンタルコントロールの設定
		bool setRegionInfo( u32 region );				// リージョンを取得(エラー情報も登録) @ret リージョンが正しいかどうか
		void setUnnecessaryRatingInfo( u32 region );	// レーティング情報が不要かどうかを取得(エラー情報も登録)
		void setRatingInfo( u32 region );				// リージョンに含まれる団体のレーティングを取得
		void setOneRatingOrgInfo( int ogn );			// 1つの団体のレーティングを取得(エラー情報も登録)
		void setRatingRomHeader( u32 region );			// ROMヘッダにフィールドの値を反映させる

		// ROMヘッダの更新
		ECSrlResult calcRomHeaderCRC(void);					// ROMヘッダのCRCを再計算
		ECSrlResult signRomHeader(void);					// ROMヘッダ更新後の再署名
		ECSrlResult decryptRomHeader( ROM_Header *prh );	// ROMヘッダの署名を外す

		// SRLバイナリから特殊な設定を調べる
		ECSrlResult hasDSDLPlaySign( FILE *fp );
				// DSダウンロード署名がSRLに格納されているか調べる
				// @arg [in]  入力ファイルのFP (->SRL読み込み時に実行されるべき)]
		ECSrlResult searchSDKVersion( FILE *fp );		// SDKバージョンを取得する
		ECSrlResult searchLicenses( FILE *fp );			// 使用ライセンスを取得する
		ECAppType selectAppType( ROM_Header *prh );		// TitleIDからアプリ種別を決定する
		void calcNandUsedSize( FILE *fp );				// NAND消費サイズを計算する

		// MRC(Master ROM Checker)機能
		ECSrlResult mrc( FILE *fp );
		ECSrlResult mrcNTR( FILE *fp );
		ECSrlResult mrcTWL( FILE *fp );
		void mrcAppType( FILE *fp );
		void mrcAccessControl( FILE *fp );
		void mrcPadding( FILE *fp );
		void mrcBanner( FILE *fp );
		void mrcReservedArea( FILE *fp );
		void mrcShared2( FILE *fp );
		void mrcSDKVersion( FILE *fp );

	}; // end of ref class RCSrl

} // end of namespace MasterEditorTWL
