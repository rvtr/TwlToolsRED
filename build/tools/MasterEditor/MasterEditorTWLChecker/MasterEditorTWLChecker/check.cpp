#include "stdafx.h"
#include "check.h"

using namespace System;

// ------------------------------------------------------------------
// ƒtƒ@ƒCƒ‹–¼‚Ì‰ğÍ
// ------------------------------------------------------------------

System::Void FilenameItem::parseFilename( System::String ^filepath )
{
	System::String ^filename = System::IO::Path::GetFileNameWithoutExtension(filepath);

	cli::array<System::String^> ^list = filename->Split( '_' );
	this->region = System::String::Copy(list[0]);
	this->ogn    = System::String::Copy(list[1]);
	this->rating = System::String::Copy(list[2]);
	this->lang   = System::String::Copy(list[3]);

	Console::WriteLine( "[In Filename]" );
	Console::WriteLine( "Region: " + this->region );
	Console::WriteLine( "Ogn:    " + this->ogn );
	Console::WriteLine( "Rating: " + this->rating );
	Console::WriteLine( "Lang:   " + this->lang );
	return;
}
