#include "stdafx.h"
#include <cstdio>
#include <cstring>
#include <twl/types.h>
#include <twl/os/common/format_rom.h>
#include "rfd_util.h"

// ------------------------------------------------------------------
// ROMヘッダの読み込み
// ------------------------------------------------------------------

System::Boolean RomFilenameDecoderTWL::readRomHeader( System::String ^srlfile, ROM_Header *rh )
{
	FILE       *fp = NULL;
	const char *pchFilename = 
		(const char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( srlfile ).ToPointer();

	// ファイルを開いてROMヘッダのみ読み出す
	if( fopen_s( &fp, pchFilename, "rb" ) != NULL )
	{
		return false;
	}
	(void)fseek( fp, 0, SEEK_SET );		// ROMヘッダはsrlの先頭から

	// 1バイトをsizeof(~)だけリード (逆だと返り値がsizeof(~)にならないので注意)
	if( fread( (void*)rh, 1, sizeof(ROM_Header), fp ) != sizeof(ROM_Header) )
	{
		fclose( fp );
		return false;
	}
	fclose( fp );
	return true;
}

// ------------------------------------------------------------------
// String を char 配列に格納
// ------------------------------------------------------------------

// @arg [out] 格納先
// @arg [in]  格納元
// @arg [in]  文字列長
// @arg [in]  余りを埋める padding
void RomFilenameDecoderTWL::setStringToChars(
	char *pDst, System::String ^hSrc, 
	const System::Int32 nMax, const System::SByte pad )
{
	System::Int32 i;

	memset( pDst, pad, nMax );
	for( i=0; (i < hSrc->Length) && (i < nMax); i++ )
	{
		pDst[i] = (char)hSrc[i];
	}
}

