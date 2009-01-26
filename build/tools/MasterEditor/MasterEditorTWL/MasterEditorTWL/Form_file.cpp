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
	if( System::IO::Path::GetExtension( infile )->ToUpper()->Equals( ".TAD" ) )	// 拡張子で判別
	{
		result = this->loadTad( infile );
		if( result )
		{
			this->IsLoadTad = true;		// 成功したときのみでないと次のTADの読み込みに失敗したときに
										// リードフラグの情報が初期化されて失われてしまう
		}
	}
	else
	{
		result = this->loadSrl( infile );
		if( result )
		{
			this->IsLoadTad = false;
			System::String ^tmpsrl = this->getSplitTadTmpFile();
			if( System::IO::File::Exists( tmpsrl ) )
			{
				System::IO::File::Delete( tmpsrl );		// SRLの読み込みが成功したら以前のTAD読み込みで使用した一時ファイルは不要
			}
		}
	}
	return result;
}

// ----------------------------------------------
// ファイルの書き出し (TAD/SRL書き出しをラップ)
// ----------------------------------------------
ECFormResult Form1::saveRom( System::String ^outfile )
{
	ECFormResult result = ECFormResult::NOERROR;
	if( this->IsLoadTad )
	{
		// 一時ファイルにSRLを書き出しているのでその一時ファイルから出力ファイルを作成
		System::String ^tmpsrl = this->getSplitTadTmpFile();
		result = this->saveSrl( tmpsrl, outfile );
		if( result != ECFormResult::NOERROR )
		{
			System::IO::File::Delete( tmpsrl );
			this->IsLoadTad = false;			// 出力したSRLが次のソースとなる
		}
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
	System::String ^srlfile = this->getSplitTadTmpFile();
	if( splitTad( tadfile, srlfile ) != 0 )		// 上書きで保存
	{
		this->errMsg( "TADファイルの読み込みに失敗しました。", "Reading TAD file failed." );
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
ECFormResult Form1::saveSrl( System::String ^infile, System::String ^outfile )
{
	if( !System::IO::File::Exists( infile ) )
	{
		return ECFormResult::ERROR_FILE_EXIST;
	}

	// コピーしたファイルにROMヘッダを上書き
	ECFormResult result = this->saveSrlCore( infile, outfile );
	if( result != ECFormResult::NOERROR )
	{
		return result;
	}

	// 再リード
	this->loadSrl( outfile );
	return ECFormResult::NOERROR;
} // saveSrl()

ECFormResult Form1::saveSrlCore( System::String ^infile, System::String ^outfile )
{
	// ROM情報をフォームから取得してSRLバイナリに反映させる
	this->setSrlProperties();

	// ファイルをコピー
	if( !(outfile->Equals( infile )) )
	{
		try
		{
			System::IO::File::Copy( infile, outfile, true );
		}
		catch( System::Exception ^ex )
		{
			(void)ex;
			return ECFormResult::ERROR_FILE_COPY;
		}
	}

	// コピーしたファイルにROMヘッダを上書き
	ECSrlResult srlRes = this->hSrl->writeToFile( outfile );
	if( srlRes != ECSrlResult::NOERROR )
	{
		ECFormResult formRes = ECFormResult::NOERROR; 
		switch( srlRes )
		{
			case ECSrlResult::ERROR_FILE_OPEN:  formRes = ECFormResult::ERROR_FILE_OPEN;  break;
			case ECSrlResult::ERROR_FILE_WRITE: formRes = ECFormResult::ERROR_FILE_WRITE; break;
			case ECSrlResult::ERROR_FILE_READ:  formRes = ECFormResult::ERROR_FILE_READ;  break;
			case ECSrlResult::ERROR_SIGN_ENCRYPT:
			case ECSrlResult::ERROR_SIGN_DECRYPT:
			case ECSrlResult::ERROR_SIGN_VERIFY:
				formRes = ECFormResult::ERROR_FILE_SIGN;
			break;
			default:
				formRes = ECFormResult::ERROR;
			break;
		}
		return formRes;
	}
	return ECFormResult::NOERROR;
}
