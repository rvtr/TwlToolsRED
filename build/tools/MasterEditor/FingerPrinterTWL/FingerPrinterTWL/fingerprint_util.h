#pragma once

#include <twl/types.h>
#include <twl/os/common/format_rom.h>

// ----------------------------------------------------------------------
// ファイル処理
// ----------------------------------------------------------------------

// ファイル全体の読み込み
cli::array<System::Byte>^ ReadBin( System::String ^path );

// ファイル全体のライト
void WriteBin( System::String ^path, cli::array<System::Byte> ^bin );

// ROMヘッダのリード
void ReadRomHeader( System::String ^srlpath, ROM_Header *dstrh );

// ROMヘッダのライト
void WriteRomHeader( System::String ^srlpath, ROM_Header *srcrh );

// バイナリからのROMヘッダの抽出
void ExtractRomHeader( cli::array<System::Byte> ^bin, ROM_Header *dstrh );

// バイナリへのROMヘッダの上書き
void OverwriteRomHeader( cli::array<System::Byte> ^bin, ROM_Header *srcrh );

// ファイルコピー
void CopyFile( System::String ^srcpath, System::String ^dstpath );

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

// バイト配列を文字列に変換
System::String^ TransBytesToString( cli::array<System::Byte> ^bytes );

// バイト配列を16進文字列に変換
System::String^ TransBytesToHexString( cli::array<System::Byte> ^bytes );

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

// ----------------------------------------------------------------------
// 外部プログラムの実行
// ----------------------------------------------------------------------

// maketadの実行
// @arg [in] maketadのパス
// @arg [in] 入力SRLのパス
// @arg [in] 出力SRLのパス
void makeTad( System::String ^maketad_path, System::String ^srlpath, System::String ^tadpath );
