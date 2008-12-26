#pragma once

#include <cstdio>
#include <cstring>
#include <twl/types.h>
#include <twl/os/common/format_rom.h>

// ------------------------------------------------------------------
// 構造体
// ------------------------------------------------------------------

// エラーコード
enum class SheetCheckerError
{
	NOERROR                  = 0,
	ERROR_VERIFY_GAME_CODE   = -1,
	ERROR_VERIFY_ROM_VERSION = -2,
	ERROR_VERIFY_CRC         = -3,
	ERROR_READ_SRL           = -8,
	ERROR_READ_SHEET         = -9,
	ERROR_ARG                = -10,
};

// 実行Context
ref class SheetCheckerContext
{
public:
	property System::Boolean    bSubmitVersion;	// オプションフラグ
	property System::Boolean    bResult;
	property System::Boolean    bTadVersion;
	property System::Boolean    bUnnecessaryRating;
public:
	SheetCheckerContext()
	{
		this->bSubmitVersion = false;
		this->bResult        = false;
		this->bTadVersion    = false;
		this->bUnnecessaryRating = false;
	}
};

// 提出確認書内の情報
ref class SheetItem
{
private:
	char           *pGameCode;
	System::String ^hMedia;
public:
	property System::Byte     RomVersion;
	property System::UInt16   FileCRC;
	property System::Byte     SubmitVersion;
	property System::Boolean  IsUnnecessaryRating;
public:
	SheetItem()
	{
		this->pGameCode   = new char[4];
		std::memset( this->pGameCode, 0, 4 );
		this->hMedia         = gcnew System::String("");
		this->RomVersion    = 0;
		this->FileCRC       = 0;
		this->SubmitVersion = 0;
		this->IsUnnecessaryRating = false;
	}
	~SheetItem()
	{
		this->!SheetItem();
	}
	!SheetItem()
	{
		delete []this->pGameCode;
	}
	property char* GameCode
	{
		void set( char* p ){ memcpy( this->pGameCode, p, 4 ); }
		char* get(){ return this->pGameCode; }
	}
	property System::String ^Media
	{
		void set( System::String ^str )
		{
			if( str != nullptr )
				this->hMedia = System::String::Copy(str);
			else
				this->hMedia = gcnew System::String("");	// nullptrが代入されることはない
		}
		System::String^ get(){ return System::String::Copy( this->hMedia ); }
	}
};


// ------------------------------------------------------------------
// utlity functions
// ------------------------------------------------------------------
System::Boolean readRomHeader( System::String ^srlfile, ROM_Header *rh );
System::Boolean readSheet( System::String ^sheetfile, SheetItem ^item );
System::String^ getXPathText( System::Xml::XmlElement ^root, System::String ^xpath );
SheetCheckerError checkSheet( ROM_Header *rh, System::UInt16 crc, SheetItem ^item );

// eof