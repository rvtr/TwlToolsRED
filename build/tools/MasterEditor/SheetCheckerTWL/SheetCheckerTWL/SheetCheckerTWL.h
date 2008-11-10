#pragma once

#include <cstdio>
#include <cstring>

// ------------------------------------------------------------------
// 構造体
// ------------------------------------------------------------------

// エラーコード
enum class SheetCheckerError
{
	NOERROR                  = 0,
	ERROR_VERIFY_CRC         = -1,
	ERROR_VERIFY_GAME_CODE   = -2,
	ERROR_VERIFY_ROM_VERSION = -3,
	ERROR_READ_SRL           = -8,
	ERROR_READ_SHEET         = -9,
	ERROR_ARG                = -0xA,
};

// 実行Context
ref class SheetCheckerContext
{
private:
	System::Boolean    ^hbSubmitVersion;	// オプションフラグ
	System::Boolean    ^hbResult;
	SheetCheckerError  ^hErrorCode;			// エラー情報
public:
	SheetCheckerContext()
	{
		this->hbSubmitVersion = gcnew System::Boolean(false);
		this->hbResult        = gcnew System::Boolean(false);
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
	SheetItem()
	{
		this->pGameCode   = new char[4];
		std::memset( this->pGameCode, 0, 4 );
		this->hRomVersion = gcnew System::Byte(0xFF);
		this->hFileCRC    = gcnew System::UInt16(0);
		this->hSubmitVersion = gcnew System::Byte(0xFF);
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
};
