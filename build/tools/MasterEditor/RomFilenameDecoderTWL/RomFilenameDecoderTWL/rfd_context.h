#pragma once

#include <twl/types.h>
#include <twl/os/common/format_rom.h>
#include <cstdio>
#include <cstring>
#include "rfd_util.h"

namespace RomFilenameDecoderTWL
{

// -----------------------------------------------------------
// エラーコード
// -----------------------------------------------------------
enum class ECError
{
	NOERROR                =  0,
	ERROR_READ_FILE        = -1,
	ERROR_FILE_NAME        = -2,
	// -3..-7 予約
	ERROR_INITIAL_CODE     = -8,
	ERROR_REMASTER_VERSION = -9,
};

// -----------------------------------------------------------
// 実行context
// -----------------------------------------------------------
ref class RCContext
{
private:
	// 引数の解析結果
	System::String ^romfile;	// 入力ファイル名
	System::Int32   argc;		// 引数の数
	// オプションを表すフラグ
	System::Boolean isSubmitVersion;
	System::Boolean isTadVersion;
public:
	// ReadOnlyにする
	property System::String^ Romfile
	{
		System::String^ get(void)
		{ 
			if( !romfile )
			{
				return nullptr;
			}
			return System::String::Copy( this->romfile );
		}
	}
	property System::Int32 Argc
	{
		System::Int32 get(void){ return this->argc; }
	}
	property System::Boolean IsSubmitVersion
	{
		System::Boolean get(void){ return this->isSubmitVersion; }
	}
	property System::Boolean IsTadVersion
	{
		System::Boolean get(void){ return this->isTadVersion; }
	}
private:
	RCContext(){}	// 封じる
public:
	// コンストラクタでは引数をもらってフラグを解析する
	RCContext( cli::array<System::String ^> ^args )
	{
		this->parseArgs( args );
	}
private:
	// 引数解析
	void parseArgs( cli::array<System::String ^> ^args )
	{
		System::Collections::Generic::List<System::Int32> ^indexList
			= gcnew System::Collections::Generic::List<System::Int32>;

		int numopt = 0;
		int i;
		for( i=0; i < args->Length; i++ )
		{
			if( args[i]->StartsWith( "-s" ) )
			{
				this->isSubmitVersion = true;
				numopt++;
			}
			else if( args[i]->StartsWith( "-t" ) )
			{
				this->isTadVersion = true;
				numopt++;
			}
			else if( !args[i]->StartsWith( "-" ) )	// オプションでない引数のindexを記録
			{
				indexList->Add(i);
			}
		}
		i=0;
		for each( System::Int32 index in indexList )	// オプションでない引数を前につめていく
		{
			args[i] = args[index];
			i++;
		}
		this->argc    = args->Length - numopt;		// 引数の数からオプションの数を除く
		if( this->argc > 0 )
		{
			this->romfile = System::String::Copy( args[0] );
		}
		else
		{
			this->romfile = nullptr;
		}
	}
}; //ref class RCContext

// -----------------------------------------------------------
// ファイル名から取得できる情報
// -----------------------------------------------------------
ref class RCFilenameItems
{
private:
	// ファイル名から取得できる情報
	System::String^ initialCode;
	System::Byte    remasterVersion;
	System::Byte    submissionVersion;
	// ファイルから読み込んだROMヘッダ
	ROM_Header      *rh;
	// エラー情報
	RomFilenameDecoderTWL::ECError  error;
public:
	property System::String^ InitialCode
	{
		System::String^ get(void){ return System::String::Copy(this->initialCode); }
	}
	property System::Byte RemasterVersion
	{
		System::Byte get(void){ return this->remasterVersion; }
	}
	property System::Byte SubmissionVersion
	{
		System::Byte get(void){ return this->submissionVersion; }
	}
	property const ROM_Header* Rh
	{
		const ROM_Header* get(void){ return this->rh; }
	}
	property RomFilenameDecoderTWL::ECError Error
	{
		RomFilenameDecoderTWL::ECError get(void){ return this->error; }
	}
private:
	RCFilenameItems(){}
public:
	RCFilenameItems( System::String ^filepath )
	{
		this->initialCode = gcnew System::String("");
		this->remasterVersion   = 0;
		this->submissionVersion = 0;
		this->rh = new ROM_Header;
		memset( this->rh, 0, sizeof(ROM_Header) );
		this->error = RomFilenameDecoderTWL::ECError::NOERROR;

		this->parseFilename( filepath );
		if( this->error == RomFilenameDecoderTWL::ECError::NOERROR )
		{
			this->readRomHeader( filepath );
		}
	}
	~RCFilenameItems()
	{
		this->!RCFilenameItems();
	}
	!RCFilenameItems()
	{
		delete this->rh;
	}
private:
	// ROMヘッダを読み込む
	void readRomHeader( System::String ^filepath )
	{
		if( !RomFilenameDecoderTWL::readRomHeader( filepath, this->rh ) )
		{
			this->error = RomFilenameDecoderTWL::ECError::ERROR_READ_FILE;
		}
	}
	// ファイル名を解析してメンバ変数を設定する
	void parseFilename( System::String ^filepath )
	{
		if( !filepath )	// 指定されていない場合
		{
			this->error = RomFilenameDecoderTWL::ECError::ERROR_FILE_NAME;
			return;
		}
		System::String^ prefix = System::IO::Path::GetFileNameWithoutExtension( filepath );	// 拡張子とディレクトリパスを除く
		if( prefix->Length < 7 )	// T****@#.SRL / T****0#E.SRL のいずれか
		{
			this->error = RomFilenameDecoderTWL::ECError::ERROR_FILE_NAME;
			return;
		}
		this->initialCode       = prefix->Substring(1,4);
		this->remasterVersion   = this->parseStrToByte( prefix->Substring(5,1) );
		if( prefix->Length >= 8 )
		{
			if( ((char)prefix[7] == 'E') && (this->remasterVersion == 0) )
			{
				this->remasterVersion = 0xE0;
			}
		}
		this->submissionVersion = this->parseStrToByte( prefix->Substring(6,1) );
	} //void parseFilename()
public:
	// 1桁の数字を数値に変換
	System::Byte parseStrToByte( System::String ^str )
	{
		if( str->Length > 1 )
		{
			return 0xff;
		}
		System::Byte val;

		// 通常 0..F だが その上を G..Z で表す可能性がある
		char c = (char)str[0];
		if( ('G' <= c) && (c <= 'Z') )		// Fより上はG..Zで表現されていく(可能性あり)
		{
			 val = c - 'G' + 16;
		}
		else if( ('g' <= c) && (c <= 'z') )
		{
			val = c - 'g' + 16;
		}
		else
		{
			try
			{
				val = System::Byte::Parse( str, System::Globalization::NumberStyles::AllowHexSpecifier );
			}
			catch(System::Exception ^e )
			{
				(void)e;
				val = 0xff;
			}
		}
		return val;
	} //parseStrToByte()
public:
	// ROMヘッダの情報と自身の情報を比較(エラー情報はメンバ変数に設定)
	void compareRomHeader()
	{
		// すでにエラーがある場合には何もしない
		if( this->error != RomFilenameDecoderTWL::ECError::NOERROR )
		{
			return;
		}
		// イニシャルコードの比較
		char inicode[4];
		RomFilenameDecoderTWL::setStringToChars( inicode, this->initialCode, 4, 0 );
		if( memcmp( this->rh->s.game_code, inicode, 4 ) != 0 )
		{
			this->error = RomFilenameDecoderTWL::ECError::ERROR_INITIAL_CODE;
			return;
		}
		// リマスターバージョンの比較
		if( this->rh->s.rom_version != this->remasterVersion )
		{
			this->error = RomFilenameDecoderTWL::ECError::ERROR_REMASTER_VERSION;
			return;
		}
		// 提出バージョンはROMヘッダにないので比較できない
		this->error = RomFilenameDecoderTWL::ECError::NOERROR;
		return;
	} //void compareRomHeader()
}; //ref class RCFilenameItems

} //namespace RomFilenameDecoderTWL