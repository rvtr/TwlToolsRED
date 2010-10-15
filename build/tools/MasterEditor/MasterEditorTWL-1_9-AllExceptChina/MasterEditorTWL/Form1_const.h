#pragma once

namespace MasterEditorTWL {

// -------------------------------------------------------------------
// Type : enum class
// Name : ECFormResult
//
// Description : フォーム上のエラーを宣言
// -------------------------------------------------------------------
enum class ECFormResult
{
	NOERROR = 0,
	// 特定の必要がない雑多なエラー
	ERROR,
	// ファイル操作
	ERROR_FILE_OPEN,
	ERROR_FILE_READ,
	ERROR_FILE_WRITE,
	ERROR_FILE_COPY,
	ERROR_FILE_EXIST,
	ERROR_FILE_SIGN,	// ファイルに署名がつけられなかった

}; //enum class ECFormResult

} // end of namespace MasterEditorTWL
