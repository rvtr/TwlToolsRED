#pragma once

// ROMデータ(SRL)クラスの関連クラスの宣言と定義

#include <apptype.h>
#include "common.h"
#include <twl/types.h>
#include <twl/os/common/format_rom.h>
#include <twl/os/common/ownerInfoEx.h>
#include "srl_const.h"
#include "utility.h"

namespace MasterEditorTWL
{
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
		RCSDKVersion( System::UInt32 code, System::Boolean isStatic )	// 生成時にのみフィールドを設定可能
		{
			this->code     = code;
			this->isStatic = isStatic;
			this->hVersion = MasterEditorTWL::analyzeSDKVersion( this->code );
		}
	public:
		property System::String ^Version	// 生成後にはフィールドは Read Only
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
		// value class ではデフォルトコンストラクタを private にして封じることはできない
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
	// Name : RCNandUsedSize
	//
	// Description : アプリのインポート時NAND消費サイズクラス
	// 
	// Role : 構造体としてデータをまとめておく
	// -------------------------------------------------------------------
	ref class RCNandUsedSize
	{
		// 値へのアクセス方法
		// - 生のデータサイズ : publicメンバ変数
		// - 切り上げたサイズ : property (ReadOnlyなメンバ変数のように見える)
	public:
		property System::UInt32  SrlSize;
		property System::UInt32  PublicSaveSize;
		property System::UInt32  PrivateSaveSize;
		property System::Boolean IsMediaNand;		// NANDアプリかどうか
		property System::Boolean IsUseSubBanner;	// サブバナーを使用するかどうか
	private:
		static const System::UInt32  cTmdSize = 16 * 1024;
		static const System::UInt32  cSubBannerSize  = 16  * 1024;
		static const System::UInt32  NandClusterSize = 16  * 1024;
		static const System::UInt32  ShopBlockSize   = 128 * 1024;
	public:
		property System::UInt32  SrlSizeRoundUp		// 切り上げた値はメンバ変数としては持たず計算した値を property で提供
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
		property System::UInt32  SubBannerSize	// サブバナーのサイズは使用するかしないかで異なるので計算した値を property で提供
		{
			System::UInt32 get()
			{
				if( !this->IsUseSubBanner )
				{
					return 0;
				}
				return this->cSubBannerSize;
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
		property System::UInt32  TmdSize
		{
			System::UInt32 get()
			{
				if( !this->IsMediaNand )
				{
					return 0;
				}
				return this->cTmdSize;
			}
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
				System::UInt32 blocks = MasterEditorTWL::roundUp( this->NandUsedSize, this->ShopBlockSize)
										/ this->ShopBlockSize;
				return blocks;
			}
		}
	}; //RCNandUsedSize

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
		RCMrcError(			// この形式でしかインスタンスを作成できない
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
			if( (this->begin == 0) && (this->end == 0) )
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
		property System::UInt32   SDKVerNotStaticTWL;					// SDKのバージョン(TWL常駐モジュール以外)
		property System::UInt32   SDKVerNotStaticNTR;					// SDKのバージョン(NTR常駐モジュール以外)
		property System::Boolean  IsPermitNormalJump;					// ノーマルジャンプがアクセス許可されているか
		property cli::array<System::Boolean> ^hIsPermitShared2Array;	// Shared2ファイルアクセスが許可されているか
		property cli::array<System::UInt32>  ^hShared2SizeArray;		// Shared2ファイルサイズ
		property System::Collections::Generic::List<VCReservedArea> ^hReservedAreaList;	// 予約領域の範囲
	public:
		RCMrcExternalCheckItems()
		{
			this->IsAppendCheck         = false;
			this->SDKVer                = 0;
			this->SDKVerNotStaticTWL    = 0;
			this->SDKVerNotStaticNTR    = 0;
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

} // end of namespace MasterEditorTWL
