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
private:
	System::Boolean    ^hbSubmitVersion;	// オプションフラグ
	System::Boolean    ^hbResult;
	System::Boolean    ^hbTadVersion;
	System::Boolean    ^hbConsole;
	SheetCheckerError  ^hErrorCode;			// エラー情報
public:
	SheetCheckerContext()
	{
		this->hbSubmitVersion = gcnew System::Boolean(false);
		this->hbResult        = gcnew System::Boolean(false);
		this->hbTadVersion    = gcnew System::Boolean(false);
		this->hbConsole       = gcnew System::Boolean(false);
		this->hErrorCode      = gcnew SheetCheckerError( SheetCheckerError::NOERROR );
	}
	property System::Boolean bSubmitVersion
	{
		void set( System::Boolean flg ){ this->hbSubmitVersion = gcnew System::Boolean(flg); }
		System::Boolean get(){ return *this->hbSubmitVersion; }
	};
	property System::Boolean bResult
	{
		void set( System::Boolean flg ){ this->hbResult = gcnew System::Boolean(flg); }
		System::Boolean get(){ return *this->hbResult; }
	};
	property System::Boolean bTadVersion
	{
		void set( System::Boolean flg ){ this->hbTadVersion = gcnew System::Boolean(flg); }
		System::Boolean get(){ return *this->hbTadVersion; }
	};
	property System::Boolean bConsole
	{
		void set( System::Boolean flg ){ this->hbConsole = gcnew System::Boolean(flg); }
		System::Boolean get(){ return *this->hbConsole; }
	};
	property SheetCheckerError ErrorCode
	{
		void set( SheetCheckerError code ){ this->hErrorCode = gcnew SheetCheckerError(code); }
		SheetCheckerError get(){ return *this->hErrorCode; }
	};
};

// 提出確認書内の情報
ref class SheetItem
{
private:
	char           *pGameCode;
	System::Byte   ^hRomVersion;
	System::UInt16 ^hFileCRC;
	System::Byte   ^hSubmitVersion;
public:
	System::String ^hMedia;
public:
	SheetItem()
	{
		this->pGameCode   = new char[4];
		std::memset( this->pGameCode, 0, 4 );
		this->hRomVersion = gcnew System::Byte(0);
		this->hFileCRC    = gcnew System::UInt16(0);
		this->hSubmitVersion = gcnew System::Byte(0);
		this->hMedia      = gcnew System::String("");
	}
	~SheetItem()
	{
		delete []this->pGameCode;
	}
	property char* GameCode
	{
		void set( char* p ){ memcpy( this->pGameCode, p, 4 ); }
		char* get(){ return this->pGameCode; }
	}
	property System::Byte RomVersion
	{
		void set( System::Byte v ){ *this->hRomVersion = v; }
		System::Byte get(){ return *this->hRomVersion; }
	}
	property System::UInt16 FileCRC
	{
		void set( System::UInt16 v ){ *this->hFileCRC = v; }
		System::UInt16 get(){ return *this->hFileCRC; }
	}
	property System::Byte SubmitVersion
	{
		void set( System::Byte v ){ *this->hSubmitVersion = v; }
		System::Byte get(){ return *this->hSubmitVersion; }
	}
	property System::String ^Media
	{
		void set( System::String ^str )
		{
			if( str != nullptr )
				this->hMedia = System::String::Copy(str);
			else
				this->hMedia = gcnew System::String("");
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