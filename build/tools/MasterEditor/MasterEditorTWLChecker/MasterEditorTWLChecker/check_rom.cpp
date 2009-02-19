#include "stdafx.h"
#include "check.h"
#include <apptype.h>
#include <keys.h>
#include <common.h>
#include <utility.h>
#include <twl/types.h>
#include <twl/os/common/format_rom.h>
#include <twl/os/common/ownerInfoEx.h>
#include <acsign/include/acsign.h>
#include <cstring>
#include <cstdio>

System::Void checkRomHeaderSign( ROM_Header *prh );


// -------------------------------------------------------------------
// 出力SRLのチェック
// -------------------------------------------------------------------
System::Void checkRom( FilenameItem ^fItem, System::String ^srlpath )
{
	const char *chpath = 
		(const char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( srlpath ).ToPointer();

	// ROMヘッダの読み込み
	ROM_Header rh;
	FILE       *fp = NULL;
	if( fopen_s( &fp, chpath, "rb" ) != NULL )
	{
		throw (gcnew System::Exception("Fail to Open SRL File."));
		return;
	}
	// 1バイトをsizeof(~)だけリード (逆だと返り値がsizeof(~)にならないので注意)
	(void)fseek( fp, 0, SEEK_SET );		// ROMヘッダはsrlの先頭から
	if( fread( (void*)&rh, 1, sizeof(ROM_Header), fp ) != sizeof(ROM_Header) )
	{
		throw (gcnew System::Exception("Fail to Read ROM Header."));
		fclose(fp);
		return;
	}
	fclose(fp);

	// 署名のチェック
	checkRomHeaderSign( &rh );

	DebugPrint( "--------------------------------------------------------" );
	DebugPrint( "{0,-10} {1,-20}", nullptr, "RomHeader" );
	DebugPrint( "--" );

	// リージョンのチェック
	u32 region = fItem->getRegionBitmap();	// ファイル名に対応する真値を取得
	DebugPrint( "{0,-10} {1,-20:X04}", "Region", rh.s.card_region_bitmap );
	DebugPrint( "--" );
	if( rh.s.card_region_bitmap != region )
	{
		throw (gcnew System::Exception("Illegal Region in ROM Header."));
		return;
	}

	// リージョンに含まれる団体のレーティングenableフラグが立っているかチェック
	System::Collections::Generic::List<int> ^ognlist = MasterEditorTWL::getOgnListInRegion( region );
	for each ( int ogn in ognlist )
	{
		if( (rh.s.parental_control_rating_info[ogn] & OS_TWL_PCTL_OGNINFO_ENABLE_MASK) == 0 )
		{
			throw (gcnew System::Exception("Rating Ogn " + ogn.ToString() + " is not Enabled."));
			return;
		}
	}

	// 設定したレーティングが正しいかどうかをチェック
	int ogn = fItem->getOgnNumber();			// ファイル名に対応する真値を取得
	u8  rating = fItem->getRatingValue();
	if( rh.s.parental_control_rating_info[ ogn ] != rating )
	{
		throw (gcnew System::Exception("Mismatch Rating Ogn " + ogn.ToString() + ". "
										+ "filename = " + rating.ToString("2X")
										+ "srl = " + rh.s.parental_control_rating_info[ ogn ].ToString("2X") + "."));
		return;
	}

	// 表示
	System::Collections::Generic::List<int> ^alllist = gcnew System::Collections::Generic::List<int>();
	alllist->Clear();
	alllist->Add( OS_TWL_PCTL_OGN_CERO );
	alllist->Add( OS_TWL_PCTL_OGN_ESRB );
	alllist->Add( OS_TWL_PCTL_OGN_USK );
	alllist->Add( OS_TWL_PCTL_OGN_PEGI_GEN );
	alllist->Add( OS_TWL_PCTL_OGN_PEGI_PRT );
	alllist->Add( OS_TWL_PCTL_OGN_PEGI_BBFC );
	alllist->Add( OS_TWL_PCTL_OGN_OFLC );
	//alllist->Add( OS_TWL_PCTL_OGN_GRB );
	for each ( int ogn in alllist )
	{
		DebugPrint( "{0,-10} {1,-20:X02}", fItem->getOgnString(ogn), rh.s.parental_control_rating_info[ogn] );
	}

	// リージョンに含まれない団体のレーティングがクリアされているかチェック
	int i;
	for( i=0; i < PARENTAL_CONTROL_INFO_SIZE; i++ )
	{
		if( (ognlist->IndexOf(i) < 0) && (rh.s.parental_control_rating_info[i] != 0) )
		{
			throw (gcnew System::Exception("Rating Ogn " + i.ToString() + " is not Cleared in ROM Header."));
			return;
		}
	}
	DebugPrint( "--------------------------------------------------------" );
}

// -------------------------------------------------------------------
// ROMヘッダの署名を外す
// -------------------------------------------------------------------
System::Void checkRomHeaderSign( ROM_Header *prh )
{
	u8     original[ RSA_KEY_LENGTH ];	// 署名外した後のデータ格納先
	s32    pos = 0;						// ブロックの先頭アドレス
	u8     digest[ DIGEST_SIZE_SHA1 ];	// ROMヘッダのダイジェスト
	u8    *publicKey = (u8*)MasterEditorTWL::g_devPubKey_DER;

	// <データの流れ>
	// (1) 公開鍵で復号した結果(ブロック)をローカル変数(original)に格納
	// (2) ブロックから余分な部分を取り除いて引数(pDst)にコピー

	u8  *idL = prh->s.titleID_Lo;
	u32  idH = prh->s.titleID_Hi;

	if( (idL[3]=='H') && (idL[2]=='N') && (idL[1]=='A') )	// ランチャアプリかどうかはTitleID_Loの値で決定
    {
		publicKey  = (u8*)MasterEditorTWL::g_devPubKey_DER_launcher;
    }
    else if( idH & TITLE_ID_HI_SECURE_FLAG_MASK )				// 各ビットは排他的とは限らないのでelse ifにはならない
    {
		publicKey  = (u8*)MasterEditorTWL::g_devPubKey_DER_secure;
    }
    else if( (idH & TITLE_ID_HI_APP_TYPE_MASK) == 1 )
    {
		publicKey  = (u8*)MasterEditorTWL::g_devPubKey_DER_system;
    }
    else if( (idH & TITLE_ID_HI_APP_TYPE_MASK) == 0 )
    {
		publicKey  = (u8*)MasterEditorTWL::g_devPubKey_DER;
    }

	// 署名の解除 = 公開鍵で復号
	if( !ACSign_Decrypto( original, publicKey, prh->signature, RSA_KEY_LENGTH ) )
	{
		throw (gcnew System::Exception("Fail to decrypt sign."));
		return;
	}
	// 署名前データを復号後ブロックからゲット
	for( pos=0; pos < (RSA_KEY_LENGTH-2); pos++ )   // 本来ブロックの先頭は0x00だが復号化の内部処理によって消える仕様
	{
		// 暗号ブロック形式 = 0x00, BlockType, Padding, 0x00, 実データ
		if( original[pos] == 0x00 )                               // 実データの直前の0x00をサーチ
		{
			break;
		}
	}
	// ベリファイ
	// ROMヘッダのダイジェストを算出(先頭から証明書領域の直前までが対象)
	ROM_Header tmprh;		// マネージヒープ上にある場合実アドレスを取得できないのでサイズ計算用のROMヘッダを用意
	ACSign_DigestUnit( digest,	prh, (u32)&(tmprh.certificate) - (u32)&(tmprh) );
	if( memcmp( &(original[pos+1]), digest, DIGEST_SIZE_SHA1 ) != 0 )
	{
		throw (gcnew System::Exception("Fail to verify sign."));
		return;
	}
	return;
}
