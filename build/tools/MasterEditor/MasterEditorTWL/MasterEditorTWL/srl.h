#pragma once

// ROMデータ(SRL)クラスと関連クラスの宣言

#include <apptype.h>
#include "common.h"
#include <twl/types.h>
#include <twl/os/common/format_rom.h>
#include <twl/os/common/ownerInfoEx.h>
#include "utility.h"

namespace MasterEditorTWL
{
	// -------------------------------------------------------------------
	// Type : enum class
	// Name : ECSrlResult
	//
	// Description : RCSrlクラスの操作でのエラーを宣言
	// -------------------------------------------------------------------
	enum class ECSrlResult
	{
		NOERROR   = 0,
		// エラー特定しなくても原因がわかるときの返り値
		// (エラーが生じる可能性のある箇所が1つ etc.)
		ERROR,
		// ファイル操作でのエラー
		ERROR_FILE_OPEN,
		ERROR_FILE_READ,
		ERROR_FILE_WRITE,
		// 署名でのエラー
		ERROR_SIGN_ENCRYPT,	// 署名できない
		ERROR_SIGN_DECRYPT,	// 署名解除できない
		ERROR_SIGN_VERIFY,	// 署名解除した後のダイジェストが合わない
		// CRC算出でのエラー
		ERROR_SIGN_CRC,
		// SDKバージョン取得でのエラー
		ERROR_SDK,
		// プラットフォームがNTR専用 or 不正バイナリ
		ERROR_PLATFORM,
	};

	// -------------------------------------------------------------------
	// Type : enum class
	// Name : ECAppType
	//
	// Description : RCSrlクラスのアプリ種別を宣言
	// -------------------------------------------------------------------
	enum class ECAppType
	{
		USER   = 0,
		SYSTEM,
		SECURE,
		LAUNCHER,
		ILLEGAL,	// 不定
	}; //ECAppType

	// -------------------------------------------------------------------
	// Type : ref class
	// Name : RCSDKVersion
	//
	// Description : RCSrlクラスに持たせるSDKバージョン情報クラス
	// 
	// Role : 構造体としてデータをまとめておく
	// -------------------------------------------------------------------
	ref class RCSDKVersion
	{
	private:
		System::String  ^hVersion;
		System::UInt32   code;
		System::Boolean  isStatic;
	private:
		RCSDKVersion(){}	// 封じる
	public:
		RCSDKVersion( System::String ^ver, System::UInt32 code, System::Boolean isStatic )	// 生成時にのみフィールドを設定可能
		{
			if( ver == nullptr )
				this->hVersion = gcnew System::String("");	// NULL参照バグを避ける
			else
				this->hVersion  = ver;
			this->code     = code;
			this->isStatic = isStatic;
		}
	public:
		property System::String ^Version	// 生成後にはフィールドはRead Only
		{
			System::String^ get(){ return System::String::Copy(this->hVersion); }
		}
	public:
		property System::Boolean IsStatic
		{
			System::Boolean get(){ return (this->isStatic); }
		}
	public:
		property System::UInt32 Code
		{
			System::UInt32 get(){ return (this->code); }
		}
	}; //RCSDKVersion

	// -------------------------------------------------------------------
	// Type : ref class
	// Name : RCLicense
	//
	// Description : RCSrlクラスに持たせるライセンス情報クラス
	// 
	// Role : 構造体としてデータをまとめておく
	// -------------------------------------------------------------------
	ref class RCLicense
	{
	private:
		System::String ^hPublisher;
		System::String ^hName;
	private:
		RCLicense(){}	// 封じる
	public:
		RCLicense( System::String ^pub, System::String ^name )	// 生成時にのみフィールドを設定可能
		{
			if( pub == nullptr )
				this->hPublisher = gcnew System::String("");
			else
				this->hPublisher = pub;

			if( name == nullptr )
				this->hName = gcnew System::String("");
			else
				this->hName = name;
		}
	public:
		property System::String ^Name	// 生成後にはフィールドはRead Only
		{
			System::String^ get(){ return System::String::Copy(this->hName); }
		}
	public:
		property System::String ^Publisher
		{
			System::String^ get(){ return System::String::Copy(this->hPublisher); }
		}
	}; //RCLicense

	// -------------------------------------------------------------------
	// Type : ref class
	// Name : RCMrcError
	//
	// Description : RCSrlクラスに持たせるMRCエラー情報クラス
	// 
	// Role : 構造体としてデータをまとめておく
	// -------------------------------------------------------------------
	public ref class RCMrcError
	{
	private:
		System::String  ^hName;		// 項目名
		System::UInt32   begin;		// 開始アドレス
		System::UInt32   end;		// 終了アドレス
		System::String  ^hMsg;		// エラーメッセージ
		System::String  ^hNameE;	// 英語版
		System::String  ^hMsgE;
		System::Boolean  isEnableModify;	// マスタエディタで修正可能かどうか
		System::Boolean  isAffectRom;		// 変更するとSRL(ROMバイナリ)が変更されるか
	private:
		RCMrcError(){}		// 封じる
	public:
		RCMrcError( 
			System::String ^name,  System::UInt32 beg,   System::UInt32 end, System::String ^msg, 
			System::String ^nameE, System::String ^msgE, System::Boolean isEnableModify, System::Boolean isAffectRom )
		{
			if( name == nullptr )
				this->hName = gcnew System::String("");
			else
				this->hName = name;

			if( nameE == nullptr )
				this->hNameE = gcnew System::String("");
			else
				this->hNameE = nameE;

			this->begin = beg;
			this->end   = end;

			if( msg == nullptr )
				this->hMsg = gcnew System::String("");
			else
				this->hMsg = msg;

			if( msgE == nullptr )
				this->hMsgE = gcnew System::String("");
			else
				this->hMsgE = msgE;

			this->isEnableModify = isEnableModify;
			this->isAffectRom    = isAffectRom;
		}
	public:
		property System::Boolean IsEnableModify
		{
			System::Boolean get(){ return this->isEnableModify; }		// Read Only
		}
		property System::Boolean IsAffectRom
		{
			System::Boolean get(){ return this->isAffectRom; }
		}
	public:
		// gridViewの表示形式にあわせる
		cli::array<System::Object^>^ getAll( System::Boolean isJapanese )
		{
			if( (this->begin == METWL_ERRLIST_NORANGE) && (this->end == METWL_ERRLIST_NORANGE) )
			{
				if( isJapanese )
					return (gcnew array<System::Object^>{this->hName,  "-", "-", this->hMsg});
				else
					return (gcnew array<System::Object^>{this->hNameE,  "-", "-", this->hMsgE});
			}

			if( isJapanese )
				return (gcnew array<System::Object^>{this->hName,  this->begin.ToString("X04")+"h", this->end.ToString("X04")+"h", this->hMsg});
			else
				return (gcnew array<System::Object^>{this->hNameE, this->begin.ToString("X04")+"h", this->end.ToString("X04")+"h", this->hMsgE});
		}
	}; //RCMrcError

	// -------------------------------------------------------------------
	// Type : value class
	// Name : VCReservedArea
	//
	// Description : 予約領域の範囲クラス
	// 
	// Role : 構造体としてデータをまとめておく
	// -------------------------------------------------------------------
	value class VCReservedArea
	{
	private:
		System::UInt32  begin;
		System::UInt32  end;
	public:
		VCReservedArea( System::UInt32 begin, System::UInt32 end )
		{
			this->begin = begin;
			this->end   = end;
		}
	public:
		property System::UInt32 Begin		// read only
		{
			System::UInt32 get(){ return this->begin; }
		}
		property System::UInt32 End
		{
			System::UInt32 get(){ return this->end; }
		}
	}; //VCReservedArea

	// -------------------------------------------------------------------
	// Type : ref class
	// Name : RCMrcExternalCheckItems
	//
	// Description : MRCチェック項目の外部パラメータクラス
	// 
	// Role : 構造体としてデータをまとめておく
	// -------------------------------------------------------------------
	ref class RCMrcExternalCheckItems
	{
	public:
		property System::Boolean  IsAppendCheck;						// 追加チェックをするかどうか(すべての項目が追加チェックとは限らない)
		property System::UInt32   SDKVer;								// SDKのバージョン
		property System::Boolean  IsPermitNormalJump;					// ノーマルジャンプがアクセス許可されているか
		property cli::array<System::Boolean> ^hIsPermitShared2Array;	// Shared2ファイルアクセスが許可されているか
		property cli::array<System::UInt32>  ^hShared2SizeArray;		// Shared2ファイルサイズ
		property System::Collections::Generic::List<VCReservedArea> ^hReservedAreaList;	// 予約領域の範囲
	public:
		RCMrcExternalCheckItems()
		{
			this->IsAppendCheck         = false;
			this->SDKVer                = 0;
			this->IsPermitNormalJump    = false;
			this->hIsPermitShared2Array = gcnew cli::array<System::Boolean>(METWL_NUMOF_SHARED2FILES);
			this->hShared2SizeArray     = gcnew cli::array<System::UInt32>(METWL_NUMOF_SHARED2FILES);
			System::Int32 i;
			for( i=0; i < METWL_NUMOF_SHARED2FILES; i++ )
			{
				this->hIsPermitShared2Array[i] = false;
				this->hShared2SizeArray[i]     = 0;
			}
			this->hReservedAreaList = gcnew System::Collections::Generic::List<VCReservedArea>;
			this->hReservedAreaList->Clear();
		}
	}; //RCMrcExternalCheckItems

	// -------------------------------------------------------------------
	// Type : ref class
	// Name : RCNandUsedSize
	//
	// Description : アプリのインポート時NAND消費サイズクラス
	// 
	// Role : 構造体としてデータをまとめておく
	// -------------------------------------------------------------------
	ref class RCNandUsedSize
	{
	public:
		property System::UInt32  SrlSize;
		property System::UInt32  PublicSaveSize;
		property System::UInt32  PrivateSaveSize;
		property System::Boolean IsMediaNand;		// NANDアプリかどうか
		property System::Boolean IsUseSubBanner;	// サブバナーを使用するかどうか
	public:
		static const System::UInt32  TmdSize = 16 * 1024;
		static const System::UInt32  SubBannerSize = 16 * 1024;
	private:
		static const System::UInt32 NandClusterSize =  16 * 1024;
		static const System::UInt32 ShopBlockSize   = 128 * 1024;
	public:
		property System::UInt32  SrlSizeRoundUp
		{
			System::UInt32 get()
			{
				if( !this->IsMediaNand )
				{
					return 0;
				}
				return (MasterEditorTWL::roundUp( this->SrlSize, NandClusterSize ));
			}
		}
		property System::UInt32  PublicSaveSizeRoundUp
		{
			System::UInt32 get(){ return (MasterEditorTWL::roundUp( this->PublicSaveSize, NandClusterSize )); }
		}
		property System::UInt32  PrivateSaveSizeRoundUp
		{
			System::UInt32 get(){ return (MasterEditorTWL::roundUp( this->PrivateSaveSize, NandClusterSize )); }
		}
		property System::UInt32  TmdSizeRoundUp
		{
			System::UInt32 get()
			{
				if( !this->IsMediaNand )
				{
					return 0;
				}
				return (MasterEditorTWL::roundUp( this->TmdSize, NandClusterSize ));
			}
		}
		property System::UInt32  SubBannerSizeRoundUp
		{
			System::UInt32 get()
			{
				if( !this->IsUseSubBanner )
				{
					return 0;
				}
				return (MasterEditorTWL::roundUp( this->SubBannerSize, NandClusterSize ));
			}
		}
		property System::UInt32  NandUsedSize		// 全体のNAND消費サイズは変数で持たず property で提供
		{
			System::UInt32 get()
			{
				// NAND消費量の計算式
				// 以下のファイルをNANDクラスタ単位(=16KB)に切り上げて合計する
				//
				//     SRLの実ファイルサイズ
				//     Publicセーブデータサイズ
				//     Privateセーブデータサイズ
				//     TMDサイズ(=16KB固定)
				//     サブバナーサイズ(使用時には16KB固定 不使用時には0KB)
				if( !this->IsMediaNand )
				{
					return 0;	// カードアプリのとき0
				}
				System::UInt32 size = this->SrlSizeRoundUp + this->PublicSaveSizeRoundUp + this->PrivateSaveSizeRoundUp
									  + this->TmdSizeRoundUp + this->SubBannerSizeRoundUp;
				return size;
			}
		}
		property System::UInt32  NandUsedSizeBlock	// ショップでのブロック数
		{
			System::UInt32 get()
			{
				return (MasterEditorTWL::roundUp( this->NandUsedSize, ShopBlockSize ));
			}
		}
	}; //RCNandUsedSize

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
		property System::Collections::Generic::List<RCMrcError^> ^hParentalErrorList;	// ペアレンタルコントロールのチェックはsetと同時に行なう

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
