#pragma once

#include <twl/types.h>

//
// ��o�m�F�����璊�o�������̍\����
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

// ROM�w�b�_�̒l�Ɛ^�l(�t�@�C����)���r
System::Void checkRom( System::String ^orgSrl, System::String ^targetSrl );

// ��o�m�F���̕�����Ɛ^�l���r
System::Void checkSheet( SheetItem ^sItem );

// ROM�w�b�_�̒l������
System::String^ getOgnString(int ogn);
System::String^ getRatingString( u8 rating );

// �f�o�b�O�\��
void setDebugPrint( System::Boolean b );
void DebugPrint( System::String ^fmt, ... cli::array<System::Object^> ^args );
