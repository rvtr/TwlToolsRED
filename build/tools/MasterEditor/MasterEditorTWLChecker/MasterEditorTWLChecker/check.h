#pragma once

#include <twl/types.h>

//
// 提出確認書から抽出される情報の構造体
//
ref class SheetItem
{
public:
	property System::String ^region;
	property cli::array<System::String^> ^ratings;
	property System::String ^IsUnnecessaryRating;
public:
	SheetItem(){}
public:
	System::Void readSheet( System::String ^sheetfile );
};

// ROMヘッダの値と真値(ファイル名)を比較
System::Void checkRom( System::String ^orgSrl, System::String ^targetSrl );

// 提出確認書の文字列と真値を比較
System::Void checkSheet( SheetItem ^sItem );

// ROMヘッダの値を解釈
System::String^ getOgnString(int ogn);
System::String^ getRatingString( u8 rating );

// デバッグ表示
void setDebugPrint( System::Boolean b );
void DebugPrint( System::String ^fmt, ... cli::array<System::Object^> ^args );
