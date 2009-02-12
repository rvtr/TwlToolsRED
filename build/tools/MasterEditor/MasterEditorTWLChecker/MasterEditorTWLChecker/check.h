#pragma once

//
// ファイル名から抽出されるファイル情報の構造体
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
};

//
// 提出確認書から抽出される情報の構造体
//
ref class SheetItem
{
public:
	property System::String ^region;
	property System::String ^CERO;
	property System::String ^ESRB;
	property System::String ^USK;
	property System::String ^PEGI;
	property System::String ^PEGIPRT;
	property System::String ^PEGIBBFC;
	property System::String ^OFLC;
	property System::Boolean IsUnnecessaryRating;
public:
	SheetItem(){}
public:
	System::Void readSheet( System::String ^sheetfile );
};

// 提出確認書の文字列と真値を比較
System::Void checkSheet( FilenameItem ^fItem, SheetItem ^sItem );
