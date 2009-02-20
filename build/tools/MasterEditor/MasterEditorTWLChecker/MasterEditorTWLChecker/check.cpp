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

void DebugPrint( System::String ^str )
{
	if( gbDebugPrint )
	{
		Console::WriteLine( str );
	}
}

void DebugPrint( System::String ^fmt, System::Object ^arg0 )
{
	if( gbDebugPrint )
	{
		Console::WriteLine( fmt, arg0 );
	}
}

void DebugPrint( System::String ^fmt, System::Object ^arg0, System::Object ^arg1 )
{
	if( gbDebugPrint )
	{
		Console::WriteLine( fmt, arg0, arg1 );
	}
}

void DebugPrint( System::String ^fmt, System::Object ^arg0, System::Object ^arg1, System::Object ^arg2 )
{
	if( gbDebugPrint )
	{
		Console::WriteLine( fmt, arg0, arg1, arg2 );
	}
}

// ------------------------------------------------------------------
// ファイル名の解析
// ------------------------------------------------------------------

System::Void FilenameItem::parseFilename( System::String ^filepath )
{
	System::String ^filename = System::IO::Path::GetFileNameWithoutExtension(filepath);

	cli::array<System::String^> ^list = filename->Split( '_' );
	if( list->Length < 4 )
	{
		throw (gcnew System::Exception("Illegal filename format. REGION_OGN_AGE_LANG.[SRL/XML]"));
		return;
	}
	this->region = System::String::Copy(list[0]);
	this->ogn    = System::String::Copy(list[1]);
	this->rating = System::String::Copy(list[2]);
	this->lang   = System::String::Copy(list[3]);

	DebugPrint( "--------------------------------------------------------" );
	DebugPrint( "{0,-10} {1,-20}", "Filename",filename );
	DebugPrint( "{0,-10} {1,-20}", "Region",  this->region );
	DebugPrint( "{0,-10} {1,-20}", "Ogn",     this->ogn );
	DebugPrint( "{0,-10} {1,-20}", "Rating",  this->rating );
	DebugPrint( "{0,-10} {1,-20}", "Language",this->lang );
	DebugPrint( "--------------------------------------------------------" );

	// ファイル名の検査
	if( this->getRegionBitmap() == 0 )
	{
		throw (gcnew System::Exception("Illegal filename format. (Region.) REGION_OGN_RATING_LANG.[SRL/XML]"));
		return;
	}
	if( this->getOgnNumber() < -1 )
	{
		throw (gcnew System::Exception("Illegal filename format. (Ogn.) REGION_OGN_RATING_LANG.[SRL/XML]"));
		return;
	}
	if( this->getRatingValue() == 0 )
	{
		throw (gcnew System::Exception("Illegal filename format. (Rating.) REGION_OGN_RATING_LANG.[SRL/XML]"));
		return;
	}

	return;
}

// -----------------------------------------------------------------
// ファイル名の文字列をROMヘッダ中の値に変換
// -----------------------------------------------------------------

u32 FilenameItem::getRegionBitmap()
{
	u32  bitmap = 0;
	if( this->region == "JP" )
	{
		bitmap = METWL_MASK_REGION_JAPAN;
	}
	else if( this->region == "US" )
	{
		bitmap = METWL_MASK_REGION_AMERICA;
	}
	else if( this->region == "EU" )
	{
		bitmap = METWL_MASK_REGION_EUROPE;
	}
	else if( this->region == "AU" )
	{
		bitmap = METWL_MASK_REGION_AUSTRALIA;
	}
	else if( this->region == "EUAU" )
	{
		bitmap = (METWL_MASK_REGION_EUROPE | METWL_MASK_REGION_AUSTRALIA);
	}
	else if( this->region == "USAU" )
	{
		bitmap = (METWL_MASK_REGION_AMERICA | METWL_MASK_REGION_AUSTRALIA);
	}
	else if( this->region == "USEUAU" )
	{
		bitmap = (METWL_MASK_REGION_AMERICA | METWL_MASK_REGION_EUROPE | METWL_MASK_REGION_AUSTRALIA);
	}
	else if( this->region == "CN" )
	{
		bitmap = (METWL_MASK_REGION_CHINA);
	}
	else if( this->region == "KR" )
	{
		bitmap = (METWL_MASK_REGION_KOREA);
	}
	else if( this->region == "ALL" )
	{
		bitmap = METWL_MASK_REGION_ALL;
	}
	return bitmap;
}

int FilenameItem::getOgnNumber()
{
	int num = -2;
	if( this->ogn == "CERO" )
	{
		num = OS_TWL_PCTL_OGN_CERO;
	}
	else if( this->ogn == "ESRB" )
	{
		num = OS_TWL_PCTL_OGN_ESRB;
	}
	else if( this->ogn == "USK" )
	{
		num = OS_TWL_PCTL_OGN_USK;
	}
	else if( this->ogn == "PEGI" )
	{
		num = OS_TWL_PCTL_OGN_PEGI_GEN;
	}
	else if( this->ogn == "PRT" )
	{
		num = OS_TWL_PCTL_OGN_PEGI_PRT;
	}
	else if( this->ogn == "BBFC" )
	{
		num = OS_TWL_PCTL_OGN_PEGI_BBFC;
	}
	else if( this->ogn == "OFLC" )
	{
		num = OS_TWL_PCTL_OGN_OFLC;
	}
	//else if( this->ogn == "GRB" )
	//{
	//	num = OS_TWL_PCTL_OGN_GRB;
	//}
	else if( this->ogn == "UN" )
	{
		num = -1;
	}
	return num;
}

System::String ^FilenameItem::getOgnString(int ogn)
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
	else if( ogn < 0 )
	{
		str = "UN";
	}
	return str;
}

u8 FilenameItem::getRatingValue()
{
	u8 val = 0;
	if( this->rating == "RP" )
	{
		val = OS_TWL_PCTL_OGNINFO_ENABLE_MASK | OS_TWL_PCTL_OGNINFO_ALWAYS_MASK;
	}
	else
	{
		val = OS_TWL_PCTL_OGNINFO_ENABLE_MASK | (System::Byte::Parse(this->rating));
	}
	return val;
}