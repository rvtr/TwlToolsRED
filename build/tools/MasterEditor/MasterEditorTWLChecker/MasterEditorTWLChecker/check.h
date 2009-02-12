#pragma once

//
// �t�@�C�������璊�o�����t�@�C�����̍\����
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
	System::Boolean parseFilename( System::String ^filepath );
};

//
// ��o�m�F�����璊�o�������̍\����
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
	System::Boolean readSheet( System::String ^sheetfile );
};

System::String^ checkSheet( FilenameItem ^fItem, SheetItem ^sItem );
