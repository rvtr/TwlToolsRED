#pragma once

#include "twl/types.h"
#include <cstring>
#include <cstdio>


// ------------------------------------------------------
// APIs
// ------------------------------------------------------

//
// ファイルから部分バイト列を抜き出す
//
// @ret 抜き出したバイト列
//
cli::array<System::Byte>^ subStr( FILE *fp, const int offset, const int size );

//
// バイト列から部分バイト列を抜き出す
//
// @ret 抜き出したバイト列
//
cli::array<System::Byte>^ subStr( cli::array<System::Byte> ^bytes, const int offset, const int size );

//
// ファイルを作成してバイト列を格納する
//
// @ret 成功したら0 失敗したら負の値
//
int saveFp( FILE *fp, cli::array<System::Byte> ^bytes );
int saveFile( System::String ^filename, cli::array<System::Byte> ^bytes );

//
// エンディアンを逆転させる(tadはビッグエンディアンなのであったほうが便利)
//
// @ret エンディアン逆転後の値
//
u32 reverseEndian( const u32 v );
u16 reverseEndian( const u16 v );

//
// 多バイト値をバイト列に変換(リトルエンディアン)  *** perlのpackを意識 ***
//
// @ret 変換後のバイト列
//
cli::array<System::Byte> ^pack32( u32 v );
cli::array<System::Byte> ^pack16( u16 v );

//
// バイト列の部分バイト列から多バイト値として解釈(リトルエンディアン)  *** perlのunpackを意識 ***
//
// @arg [in] バイト列
// @arg [in] 何バイト目からを多バイト値とみなすか
//
// @ret 解釈した後の多バイト値
//
u32 unpack32( cli::array<System::Byte> ^bytes, const int index );
u16 unpack16( cli::array<System::Byte> ^bytes, const int index );

//
// 丸める
//
u32 roundUp( const u32 v, const u32 align );
u16 roundUp( const u16 v, const u16 align );

//
// バイト列の長さを拡張して末尾を0で埋める
// (0x12345678 => 0x1234567800000000)
//
// @arg [in] 拡張前のバイト列
// @arg [in] 何バイト拡張するか(拡張分だけを指定 例えば4バイトを5バイトにする場合には1を指定)
//
// @ret 拡張後のバイト列(新たなバイト列を内部で生成)
//
cli::array<System::Byte> ^resizeBytes( cli::array<System::Byte> ^org, const int difSize );

// バイト列を16進で表示
void dumpBytes( cli::array<System::Byte> ^bytes );

