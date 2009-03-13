#include "stdafx.h"
#include "check.h"
#include <common.h>
#include <twl/types.h>
#include <twl/os/common/format_rom.h>
#include <twl/os/common/ownerInfoEx.h>

using namespace System;

// ------------------------------------------------------------------
// デバッグ表示
// ------------------------------------------------------------------

static System::Boolean gbDebugPrint = false;
void setDebugPrint( System::Boolean b )
{
	gbDebugPrint = b;
}

void DebugPrint( System::String ^fmt, ... cli::array<System::Object^> ^args )
{
	if( gbDebugPrint )
	{
		Console::WriteLine( fmt, args );
	}
}

// ------------------------------------------------------------------
// ROMヘッダ中の値の解釈
// ------------------------------------------------------------------
System::String^ getRegionString(System::UInt32 region)
{
	if( region == METWL_MASK_REGION_ALL )
	{
		return (gcnew System::String("ALL"));
	}

	System::String ^str = "";
	if( region & METWL_MASK_REGION_JAPAN )
	{
		str += "Japan ";
	}
	if( region & METWL_MASK_REGION_AMERICA )
	{
		str += "America ";
	}
	if( region & METWL_MASK_REGION_EUROPE )
	{
		str += "Europe ";
	}
	if( region & METWL_MASK_REGION_AUSTRALIA )
	{
		str += "Australia ";
	}
	if( region & METWL_MASK_REGION_KOREA )
	{
		str += "Korea ";
	}
	str = str->TrimEnd( ' ' );
	str = str->Replace( ' ', ',' );
	return str;
}

System::String^ getOgnString(int ogn)
{
	System::String ^str = nullptr;
	if( ogn == OS_TWL_PCTL_OGN_CERO )
	{
		str = "CERO";
	}
	else if( ogn == OS_TWL_PCTL_OGN_ESRB )
	{
		str = "ESRB";
	}
	else if( ogn == OS_TWL_PCTL_OGN_USK )
	{
		str = "USK";
	}
	else if( ogn == OS_TWL_PCTL_OGN_PEGI_GEN )
	{
		str = "PEGI";
	}
	else if( ogn == OS_TWL_PCTL_OGN_PEGI_PRT )
	{
		str = "PRT";
	}
	else if( ogn == OS_TWL_PCTL_OGN_PEGI_BBFC )
	{
		str = "BBFC";
	}
	else if( ogn == OS_TWL_PCTL_OGN_OFLC )
	{
		str = "OFLC";
	}
	//else if( ogn == OS_TWL_PCTL_OGN_GRB )
	//{
	//	str = "GRB";
	//}
	else
	{
		str = "rsv.";
	}
	return str;
}

System::String^ getRatingString( u8 rating )
{
	System::String ^str;
	if( rating == 0 )
	{
		str = "Undefined";
	}
	else if( rating == (OS_TWL_PCTL_OGNINFO_ENABLE_MASK | OS_TWL_PCTL_OGNINFO_ALWAYS_MASK) )
	{
		str = "RP";
	}
	else if( rating & OS_TWL_PCTL_OGNINFO_ENABLE_MASK )
	{
		u8 age = rating & ~(OS_TWL_PCTL_OGNINFO_ENABLE_MASK);
		str = age.ToString();
	}
	else
	{
		str = "Illegal";
	}
	return str;
}
