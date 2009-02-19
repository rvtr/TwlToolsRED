#pragma once

#include <twl/types.h>

//
// �t�@�C�������璊�o�����^�l�̍\����
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
	// �e�����o��ROM�w�b�_�̒l�ɕϊ�
	u32 getRegionBitmap();
	int getOgnNumber();
	System::String ^getOgnString(int ogn);
	u8  getRatingValue();
};

//
// ��o�m�F�����璊�o�������̍\����
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

// ROM�w�b�_�̒l�Ɛ^�l(�t�@�C����)���r
System::Void checkRom( FilenameItem ^fItem, System::String ^srlpath );

// ��o�m�F���̕�����Ɛ^�l���r
System::Void checkSheet( FilenameItem ^fItem, SheetItem ^sItem );

// �f�o�b�O�\��
void setDebugPrint( System::Boolean b );
void DebugPrint( System::String ^str );
void DebugPrint( System::String ^fmt, System::Object ^arg0 );
void DebugPrint( System::String ^fmt, System::Object ^arg0, System::Object ^arg1 );
void DebugPrint( System::String ^fmt, System::Object ^arg0, System::Object ^arg1, System::Object ^arg2 );
