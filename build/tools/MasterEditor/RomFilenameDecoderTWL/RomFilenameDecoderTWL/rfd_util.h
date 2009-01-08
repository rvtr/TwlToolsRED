#pragma once

#include <twl/types.h>
#include <twl/os/common/format_rom.h>

namespace RomFilenameDecoderTWL
{

// ------------------------------------------------------------------
// ROMƒwƒbƒ_‚Ì“Ç‚İ‚İ
// ------------------------------------------------------------------
System::Boolean readRomHeader( System::String ^srlfile, ROM_Header *rh );

// ------------------------------------------------------------------
// String ‚ğ char ”z—ñ‚ÉŠi”[
// ------------------------------------------------------------------

// @arg [out] Ši”[æ
// @arg [in]  Ši”[Œ³
// @arg [in]  •¶š—ñ’·
// @arg [in]  —]‚è‚ğ–„‚ß‚é padding
void setStringToChars( char *pDst, System::String ^hSrc, 
				       const System::Int32 nMax, const System::SByte pad );

} //namespace RomFilenameDecoderTWL
