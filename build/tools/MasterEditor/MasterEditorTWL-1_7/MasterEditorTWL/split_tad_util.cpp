#include "stdafx.h"
#include "twl/types.h"
#include <cstring>
#include <cstdio>
#include "split_tad_util.h"


// ------------------------------------------------------------------------
// 部分バイト列を抜き出す
// ------------------------------------------------------------------------

// @ret 抜き出したバイト列
cli::array<System::Byte>^ subStr( FILE *fp, const int offset, const int size )
{
	if( size <= 0 )
	{
		return nullptr;
	}
	cli::array<System::Byte> ^mbuf = gcnew cli::array<System::Byte>(size);	// メモリ解放の必要なし
	pin_ptr<unsigned char> buf = &mbuf[0];		// fread が unmanaged 配列を引数にするので変換

	(void)fseek( fp, offset, SEEK_SET );
	if( fread( buf, 1, size, fp ) != size )
	{
		return nullptr;
	}
	return mbuf;	// managed のほうを返す
}

// @ret 抜き出したバイト列
cli::array<System::Byte>^ subStr( cli::array<System::Byte> ^bytes, const int offset, const int size )
{
	cli::array<System::Byte> ^sub = gcnew cli::array<System::Byte>(size);

	cli::array<System::Byte>::Copy( bytes, offset, sub, 0, size );
	return sub;
}

// ------------------------------------------------------------------------
// ファイルを作成してバイト列を格納する
// ------------------------------------------------------------------------

// @ret 成功したら0 失敗したら負の値
int saveFp( FILE *fp, cli::array<System::Byte> ^bytes )
{
	if( bytes == nullptr )
	{
		return 0;	// 空のファイルをつくりたいということなので正常終了とみなす
	}
	pin_ptr<unsigned char> tmp = &bytes[0];		// array型はふつうの配列ではないのでバイト配列に変換
	int size = bytes->Length;

	if( fwrite( tmp, 1, size, fp ) != size )
	{
		return -1;
	}
	return 0;
}

// @ret 成功したら0 失敗したら負の値
int saveFile( System::String ^filename, cli::array<System::Byte> ^bytes )
{
	FILE       *fp = NULL;
	const char *pchFilename = 
		(const char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( filename ).ToPointer();

	// ファイルにROMヘッダをライト
	if( fopen_s( &fp, pchFilename, "wb" ) != NULL )	// 同名ファイルを削除して新規にライト・バイナリ
	{
		return -1;
	}
	fseek( fp, 0, SEEK_SET );
	int r = saveFp( fp, bytes );	
	fclose( fp );

	return r;
}

// ------------------------------------------------------------------------
// エンディアンを逆転させる(tadはビッグエンディアンなのであったほうが便利)
// ------------------------------------------------------------------------

// @ret エンディアン逆転後の値
u32 reverseEndian( const u32 v )
{
	u32 ret = (v<<24) | ((v<<8) & 0x00FF0000) | ((v>>8) & 0x0000FF00) | (v>>24);
	return ret;
}

u16 reverseEndian( const u16 v )
{
	u16 ret = (v<<8) | (v>>8);
	return ret;
}

// ------------------------------------------------------------------------
// 多バイト値をバイト列に変換(リトルエンディアン)
// *** perlのpackを意識 ***
// ------------------------------------------------------------------------

// @ret 変換後のバイト列
cli::array<System::Byte> ^pack32( u32 v )
{
	cli::array<System::Byte> ^bytes = gcnew cli::array<System::Byte>(4);
	bytes[0] = v & 0xFF;
	bytes[1] = (v >> 8)  & 0xFF;
	bytes[2] = (v >> 16) & 0xFF;
	bytes[3] = (v >> 24) & 0xFF;
	return bytes;
}

cli::array<System::Byte> ^pack16( u16 v )
{
	cli::array<System::Byte> ^bytes = gcnew cli::array<System::Byte>(2);
	bytes[0] = v & 0xFF;
	bytes[1] = v >>8;
	return bytes;
}

// ------------------------------------------------------------------------
// バイト列の部分バイト列から多バイト値として解釈(リトルエンディアン)
// *** perlのunpackを意識 ***
// ------------------------------------------------------------------------

// @arg [in] バイト列
// @arg [in] 何バイト目からを多バイト値とみなすか
// @ret 解釈した後の多バイト値
u32 unpack32( cli::array<System::Byte> ^bytes, const int index )
{
	pin_ptr<unsigned char> tmp = &bytes[0];
	u32 v = (u32)*((u32*)(tmp+index));
	return v;
}

u16 unpack16( cli::array<System::Byte> ^bytes, const int index )
{
	pin_ptr<unsigned char> tmp = &bytes[0];
	u16 v = (u16)*((u16*)(tmp+index));
	return v;
}

// ------------------------------------------------------------------------
// 丸める
// ------------------------------------------------------------------------

u32 roundUp( const u32 v, const u32 align )
{
	u32 r = ((v + align - 1) / align) * align;
	return r;
}

u16 roundUp( const u16 v, const u16 align )
{
	u16 r = ((v + align - 1) / align) * align;
	return r;
}

// ------------------------------------------------------------------------
// バイト列の長さを拡張して末尾を0で埋める
// (0x12345678 => 0x1234567800000000)
// ------------------------------------------------------------------------

// @arg [in] 拡張前のバイト列
// @arg [in] 何バイト拡張するか(拡張分だけを指定 例えば4バイトを5バイトにする場合には1を指定)
// @ret 拡張後のバイト列(新たなバイト列を内部で生成)
cli::array<System::Byte> ^resizeBytes( cli::array<System::Byte> ^org, const int difSize )
{
	cli::array<System::Byte> ^r = gcnew cli::array<System::Byte>( org->Length + difSize );
	cli::array<System::Byte>::Copy( org, 0, r, 0, org->Length );
	return r;
}

// ------------------------------------------------------------------------
// バイト列を16進で表示
// ------------------------------------------------------------------------

void dumpBytes( cli::array<System::Byte> ^bytes )
{
	for each( System::Byte b in bytes )
	{
		printf( "%02x", b );
	}
	printf( "\n" );
}

// end of file
