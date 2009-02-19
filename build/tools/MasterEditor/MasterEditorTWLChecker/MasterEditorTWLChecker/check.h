#pragma once

#include <twl/types.h>

//
// ファイル名から抽出される真値の構造体
//
ref class FilenameItem
{
public:
	property System::String ^region;
	property System::String ^ogn;
	property System::String ^rating;
	property System::String ^lang;
public:
	FilenameItem(){}
public:
	System::Void parseFilename( System::String ^filepath );
public:
	// 各メンバをROMヘッダの値に変換
	u32 getRegionBitmap();
	int getOgnNumber();
	System::String ^getOgnString(int ogn);
	u8  getRatingValue();
};

//
// 提出確認書から抽出される情報の構造体
//
ref class SheetItem
{
public:
	property System::String ^region;
	property cli::array<System::String^> ^ratings;
	property System::Boolean IsUnnecessaryRating;
public:
	SheetItem(){}
public:
	System::Void readSheet( System::String ^sheetfile );
};

// ROMヘッダの値と真値(ファイル名)を比較
System::Void checkRom( FilenameItem ^fItem, System::String ^srlpath );

// 提出確認書の文字列と真値を比較
System::Void checkSheet( FilenameItem ^fItem, SheetItem ^sItem );

// デバッグ表示
void setDebugPrint( System::Boolean b );
void DebugPrint( System::String ^str );
void DebugPrint( System::String ^fmt, System::Object ^arg0 );
void DebugPrint( System::String ^fmt, System::Object ^arg0, System::Object ^arg1 );
void DebugPrint( System::String ^fmt, System::Object ^arg0, System::Object ^arg1, System::Object ^arg2 );
