#pragma once

#include <twl/types.h>
#include <twl/os/common/format_rom.h>

namespace RomFilenameDecoderTWL
{

// ------------------------------------------------------------------
// ROMwb_ΜΗέέ
// ------------------------------------------------------------------
System::Boolean readRomHeader( System::String ^srlfile, ROM_Header *rh );

// ------------------------------------------------------------------
// String π char zρΙi[
// ------------------------------------------------------------------

// @arg [out] i[ζ
// @arg [in]  i[³
// @arg [in]  Άρ·
// @arg [in]  ]θπίι padding
void setStringToChars( char *pDst, System::String ^hSrc, 
				       const System::Int32 nMax, const System::SByte pad );

} //namespace RomFilenameDecoderTWL
