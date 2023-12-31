#pragma once

// 書類クラスの宣言

#include <apptype.h>
#include <twl/types.h>
#include <twl/os/common/format_rom.h>
#include <apptype.h>
#include "common.h"
#include "srl.h"

namespace MasterEditorTWL
{
	// -------------------------------------------------------------------
	// Type : enum class
	// Name : ECDeliverableResult
	//
	// Description : RCDeliverable クラスの操作でのエラーを宣言
	// -------------------------------------------------------------------
	enum class ECDeliverableResult
	{
		NOERROR   = 0,
		// エラー特定しなくても原因がわかるときの返り値
		// (エラーが生じる可能性のある箇所が1つ etc.)
		ERROR,
		// ファイル操作でのエラー
		ERROR_FILE_OPEN,
		ERROR_FILE_READ,
		ERROR_FILE_WRITE,
	};

	// -------------------------------------------------------------------
	// Type : ref class
	// Name : RCDeliverable
	//
	// Description : 提出書類クラス
	// 
	// Role : 提出情報の入出力
	// -------------------------------------------------------------------
	ref class RCDeliverable
	{
		// field
	public:
		// 提出情報
		property System::String  ^hProductName;		// 製品名
		property System::String  ^hProductCode1;	// 製品コード
		property System::String  ^hProductCode2;	// 製品コード
		property System::Int32   ReleaseYear;		// 発売予定日
		property System::Int32   ReleaseMonth;
		property System::Int32   ReleaseDay;
		property System::Int32   SubmitYear;		// 提出日
		property System::Int32   SubmitMonth;
		property System::Int32   SubmitDay;
		property System::String  ^hSubmitWay;		// 提出方法
		property System::String  ^hUsage;			// 用途
		property System::String  ^hUsageDetail;
		property System::Int32   SubmitVersion;		// 提出バージョン
		property System::String  ^hSDK;				// SDKバージョン
		property System::Boolean  IsReleaseForeign;	// 海外版の予定
		property System::String  ^hProductNameForeign;
		property System::String  ^hProductCode1Foreign;
		property System::String  ^hProductCode2Foreign;

		// 会社情報

		// 担当者(1人目)
		property System::String  ^hCompany1;		// 会社名
		property System::String  ^hPerson1;			// 名前
		property System::String  ^hFurigana1;		// ふりがな
		property System::String  ^hTel1;			// 電話番号
		property System::String  ^hFax1;			// FAX番号
		property System::String  ^hMail1;			// メアド
		property System::String  ^hNTSC1;			// NTSC User ID
		// 担当者(2人目)
		property System::String  ^hCompany2;
		property System::String  ^hPerson2;
		property System::String  ^hFurigana2;
		property System::String  ^hTel2;
		property System::String  ^hFax2;
		property System::String  ^hMail2;
		property System::String  ^hNTSC2;

		// 備考
		property System::String  ^hCaption;

		// ROMヘッダ不記載のROMバイナリ(SRL)固有情報
		property System::String  ^hBackupMemory;		// バックアップメモリの種別

		// DSi Wareの販売カテゴリ
		property System::String  ^hDLCategory;

		// SRL情報の一部を文字列で持っておく
		property System::String  ^hRegion;
		property System::String  ^hCERO;
		property System::String  ^hESRB;
		property System::String  ^hUSK;
		property System::String  ^hPEGI;
		property System::String  ^hPEGI_PRT;
		property System::String  ^hPEGI_BBFC;
		property System::String  ^hOFLC;
		property System::String  ^hGRB;
		property System::String  ^hAppTypeOther;		// 特記事項

		// SRLには登録されないROM仕様
		property System::Boolean  IsUGC;
		property System::Boolean  IsPhotoEx;

		// Privateセーブデータの用途
		property System::String ^hPrivateSaveDataPurpose;

		// プログラムのバージョン
		property System::String  ^hMasterEditorVersion;

		// constructor and destructor
	public:

		// method
	public:

		//
		// 提出確認書出力
		//
		// @arg [out] 出力ファイル名
		// @arg [in]  提出確認書のテンプレート
		// @arg [in]  ROMバイナリ(SRL)固有情報
		// @arg [in]  ファイル全体のCRC
		// @arg [in]  SRLのファイル名(書類に記述するために使用)
		// @arg [in]  英語フラグ
		//
		ECDeliverableResult writeSpreadsheet( 
			System::String ^hFilename, System::String ^hSheetTemplateFilename,
			RCSrl ^hSrl, System::UInt16 CRC, System::String ^hSrlFilename, System::Boolean english );

	}; // end of ref class RCDeliverable

} // end of namespace MasterEditorTWL
