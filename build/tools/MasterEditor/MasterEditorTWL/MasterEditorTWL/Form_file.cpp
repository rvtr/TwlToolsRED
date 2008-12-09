// ----------------------------------------------
// ROMファイルのR/W
// ----------------------------------------------

#include "stdafx.h"
#include <apptype.h>
#include "common.h"
#include "srl.h"
#include "deliverable.h"
#include "crc_whole.h"
#include "utility.h"
#include "lang.h"
#include "split_tad.h"
#include "FormError.h"
#include "Form1.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace MasterEditorTWL;


// ----------------------------------------------
// ファイルの読み込み (TAD/SRL読み込みをラップ)
// ----------------------------------------------
System::Boolean Form1::loadRom( System::String ^infile )
{
	System::Boolean result = false;
	this->IsLoadTad = false;
	if( System::IO::Path::GetExtension( infile )->ToUpper()->Equals( ".TAD" ) )	// 拡張子で判別
	{
		this->IsLoadTad = true;
		result = this->loadTad( infile );
	}
	else
	{
		result = this->loadSrl( infile );
	}
	return result;
}

// ----------------------------------------------
// ファイルの書き出し (TAD/SRL書き出しをラップ)
// ----------------------------------------------
System::Boolean Form1::saveRom( System::String ^outfile )
{
	System::Boolean result = false;
	if( this->IsLoadTad )
	{
		// 一時ファイルにSRLを書き出しているのでその一時ファイルから出力ファイルを作成
		System::String ^tmpsrl = this->getSplitTadTmpFilename();
		result = this->saveSrl( tmpsrl, outfile );
		System::IO::File::Delete( tmpsrl );
	}
	else
	{
		result = this->saveSrl( this->tboxFile->Text, outfile );
	}
	return result;
}

// ----------------------------------------------
// tadの読み込み
// ----------------------------------------------
System::Boolean Form1::loadTad( System::String ^tadfile )
{
	// tadファイルを変換したSRLを一時ファイルに保存
	System::String ^srlfile = this->getSplitTadTmpFilename();
	if( System::IO::File::Exists( srlfile ) )
	{
		System::IO::File::Delete( srlfile );	// すでに存在する場合は削除(連続に読み込んだ場合に起こりうる)
	}
	if( splitTad( tadfile, srlfile ) != 0 )
	{
		this->errMsg( "TADファイルの読み出しに失敗しました。", "Reading TAD file failed." );
		System::IO::File::Delete( srlfile );
		return false;
	}
	System::Boolean result = this->loadSrl( srlfile );	// 一時保存したSRLを読み込み
	return result;
}

// ----------------------------------------------
// SRLの読み込み
// ----------------------------------------------
System::Boolean Form1::loadSrl( System::String ^srlfile )
{
	ECSrlResult result = this->hSrl->readFromFile( srlfile );
	if( result != ECSrlResult::NOERROR )
	{
		switch( result )
		{
			case ECSrlResult::ERROR_PLATFORM:
				this->errMsg( "本ツールはTWL用です。NTR専用ROMなどのTWL非対応ROMを読み込むことはできません。",
							  "This tool can only read TWL ROM. This can't read an other data e.g. NTR limited ROM." );
			break;

			case ECSrlResult::ERROR_SIGN_DECRYPT:
			case ECSrlResult::ERROR_SIGN_VERIFY:
				this->errMsg( "不正なROMデータです。TWL対応/専用ROMでないかROMデータが改ざんされている可能性があります。",
							  "Illegal ROM data. It is not for TWL ROM, or is altered illegally." );
			break;

			default:
				this->errMsg( "ROMデータファイルの読み込みに失敗しました。\n再度ROMデータを読み込ませてください。", 
					          "Reading the ROM data file failed. \nPlease read a ROM data file again, with \"Open a ROM data file\"" );
			break;
		}
		return false;
	}

	// GUIにROM情報を格納
	this->setSrlForms();
	if( this->IsReadOnly )
	{
		this->readOnly();	// リードオンリーモードのときフォームをEnableにする
	}

	// SRLに登録されないROM仕様のフォーム情報も戻るボタンで読み込み時の状態に戻したいが
	// 読み込み時の状態をSRLに保存しておけないので退避しておく
	this->saveOtherForms();

	// 全体のCRCを算出
	u16  crc;
	if( !getWholeCRCInFile( srlfile, &crc ) )
	{
		this->errMsg( "ROMデータのCRC計算に失敗しました。ROMデータの読み込みはキャンセルされました。",
			          "Calculating CRC of the ROM data failed. Therefore reading ROM data is canceled." );
		return false;
	}
	System::UInt16 ^hcrc = gcnew System::UInt16( crc );
	this->tboxWholeCRC->Clear();
	this->tboxWholeCRC->AppendText( "0x" );
	this->tboxWholeCRC->AppendText( hcrc->ToString("X") );

	// 読み込み時エラーを登録する
	this->rErrorReading->Checked = true;
	this->setGridError();
	this->setGridWarn();

	return true;
} // loadSrl()

// ----------------------------------------------
// SRLの保存
// ----------------------------------------------
System::Boolean Form1::saveSrl( System::String ^infile, System::String ^outfile )
{
	if( !System::IO::File::Exists( infile ) )
	{
		return false;
	}

	// コピーしたファイルにROMヘッダを上書き
	if( !this->saveSrlCore( infile, outfile ) )
	{
		return false;
	}

	// 再リード
	this->loadSrl( outfile );
	return true;
} // saveSrl()

System::Boolean Form1::saveSrlCore( System::String ^infile, System::String ^outfile )
{
	// ROM情報をフォームから取得してSRLバイナリに反映させる
	this->setSrlProperties();

	// ファイルをコピー
	if( !(outfile->Equals( infile )) )
	{
		System::IO::File::Copy( infile, outfile, true );
	}

	// コピーしたファイルにROMヘッダを上書き
	if( this->hSrl->writeToFile( outfile ) != ECSrlResult::NOERROR )
	{
		return false;
	}
	return true;
}
