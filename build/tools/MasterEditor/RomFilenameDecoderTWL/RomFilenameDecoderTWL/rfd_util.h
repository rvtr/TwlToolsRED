#pragma once

#include <twl/types.h>
#include <twl/os/common/format_rom.h>

namespace RomFilenameDecoderTWL
{

// ------------------------------------------------------------------
// ROM�w�b�_�̓ǂݍ���
// ------------------------------------------------------------------
System::Boolean readRomHeader( System::String ^srlfile, ROM_Header *rh );

// ------------------------------------------------------------------
// String �� char �z��Ɋi�[
// ------------------------------------------------------------------

// @arg [out] �i�[��
// @arg [in]  �i�[��
// @arg [in]  ������
// @arg [in]  �]��𖄂߂� padding
void setStringToChars( char *pDst, System::String ^hSrc, 
				       const System::Int32 nMax, const System::SByte pad );

} //namespace RomFilenameDecoderTWL
