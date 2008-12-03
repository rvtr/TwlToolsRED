#pragma once

#include "twl/types.h"
#include <cstring>
#include <cstdio>


// ------------------------------------------------------
// APIs
// ------------------------------------------------------

// tad ファイルから srl(0番目のコンテンツ)を抜き出す
// (split_tad_dev.pl の移植)
int splitTad( System::String ^filename );


// ------------------------------------------------------
// 内部処理用の構造体(宣言だけできないのでヘッダに置く)
// ------------------------------------------------------

// コンテンツ情報の構造体
ref class rcContentsInfo
{
private:
	System::UInt32  ^h_cid;
	System::UInt16  ^h_idx;
	System::UInt16  ^h_type;
	System::UInt32  ^h_size;
	cli::array<System::Byte> ^h_hash;
public:
	rcContentsInfo()
	{
		this->h_cid  = gcnew System::UInt32;
		this->h_idx  = gcnew System::UInt16;
		this->h_type = gcnew System::UInt16;
		this->h_size = gcnew System::UInt32;
		this->h_hash = gcnew cli::array<System::Byte>(20);	// 固定長 : 解放の必要なし
	}
public:
	property System::UInt32 cid
	{
		void set( System::UInt32 v ){ *this->h_cid = v; };
		System::UInt32 get(void){ return *this->h_cid; }
	}
	property System::UInt16 idx
	{
		void set( System::UInt16 v ){ *this->h_idx = v; };
		System::UInt16 get(void){ return *this->h_idx; }
	}
	property System::UInt16 type
	{
		void set( System::UInt16 v ){ *this->h_type = v; };
		System::UInt16 get(void){ return *this->h_type; }
	}
	property System::UInt32 size
	{
		void set( System::UInt32 v ){ *this->h_size = v; };
		System::UInt32 get(void){ return *this->h_size; }
	}
	property cli::array<System::Byte> ^hash
	{
		void set( cli::array<System::Byte> ^h )
		{
			cli::array<System::Byte>::Copy( h, this->h_hash, 20 );
		}
		cli::array<System::Byte> ^get(void)
		{
			cli::array<System::Byte> ^cp = gcnew cli::array<System::Byte>(20);	// コピーを返す
			cli::array<System::Byte>::Copy( this->h_hash, cp, 20 );
			return cp;
		}
	}
};
