#pragma once

#include <twl/types.h>
#include <twl/os/common/format_rom.h>

// ----------------------------------------------------------------------
// ファイル処理
// ----------------------------------------------------------------------

// ファイルコピー
void CopyFile( System::String ^srcpath, System::String ^dstpath );

// ROMヘッダのリード
void ReadRomHeader( System::String ^srlpath, ROM_Header *dstrh );

// ROMヘッダのライト
void WriteRomHeader( System::String ^srlpath, ROM_Header *srcrh );

// ----------------------------------------------------------------------
// 署名
// ----------------------------------------------------------------------

// ROMヘッダに署名をつける
void SignRomHeader( ROM_Header *rh );

// ----------------------------------------------------------------------
// 変換
// ----------------------------------------------------------------------

// 文字列をバイト配列に変換
cli::array<System::Byte>^ TransStringToBytes( System::String ^src, const int len );

// 16進文字列をバイト配列に変換
cli::array<System::Byte>^ TransHexStringToBytes( System::String ^src, const int len );

// バイト配列を16進文字列に変換
System::String^ TransBytesToString( cli::array<System::Byte> ^bytes );

// ----------------------------------------------------------------------
// GUI
// ----------------------------------------------------------------------

// ファイルをダイアログで取得
// @arg [in] デフォルトのディレクトリ
// @arg [in] 拡張子フィルタ
// @ret 取得したファイル名 エラーのとき nullptr
System::String^ OpenFileUsingDialog( System::String ^defdir, System::String ^filter );

// セーブするファイルをダイアログで取得
// @arg [in] デフォルトのディレクトリ
// @arg [in] 拡張子フィルタ
// @arg [in] ファイルの拡張子が不正なときに追加するときの正しい拡張子
// @ret 取得したファイル名 エラーのとき nullptr
System::String^ SaveFileUsingDialog( System::String ^defdir, System::String ^filter, System::String ^extension );
