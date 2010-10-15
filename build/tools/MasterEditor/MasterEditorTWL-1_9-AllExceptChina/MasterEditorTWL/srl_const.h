#pragma once

// ROMデータ(SRL)クラス関連の定数宣言

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

} // end of namespace MasterEditorTWL