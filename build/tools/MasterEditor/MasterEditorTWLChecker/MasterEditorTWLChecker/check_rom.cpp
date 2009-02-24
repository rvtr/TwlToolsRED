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
System::Void verifyArea( FILE *fp1, FILE *fp2, const int offset, const int size );

// -------------------------------------------------------------------
// 出力SRLのチェック
// -------------------------------------------------------------------
System::Void checkRom( FilenameItem ^fItem, System::String ^orgSrl, System::String ^targetSrl )
{
	const char *chorg = 
		(const char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( orgSrl ).ToPointer();
	const char *chtarget = 
		(const char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( targetSrl ).ToPointer();

	// ROMヘッダの読み込み
	ROM_Header rh;
	FILE       *fp = NULL;
	if( fopen_s( &fp, chtarget, "rb" ) != 0 )
	{
		throw (gcnew System::Exception("Fail to open the target SRL File."));
		return;
	}
	// 1バイトをsizeof(~)だけリード (逆だと返り値がsizeof(~)にならないので注意)
	(void)fseek( fp, 0, SEEK_SET );		// ROMヘッダはsrlの先頭から
	if( fread( (void*)&rh, 1, sizeof(ROM_Header), fp ) != sizeof(ROM_Header) )
	{
		throw (gcnew System::Exception("Fail to read the ROM Header."));
		fclose(fp);
		return;
	}
	fclose(fp);

	// 署名のチェック
	checkRomHeaderSign( &rh );

	DebugPrint( "--------------------------------------------------------" );
	DebugPrint( "{0,-10} {1,-20} {2,-20}", nullptr, "TrueValue", "RomHeader" );
	DebugPrint( "--" );

	// リージョンのチェック
	u32 region = fItem->getRegionBitmap();	// ファイル名に対応する真値を取得
	DebugPrint( "{0,-10} {1,-20:X04} {2,-20:X04}", "Region", region, rh.s.card_region_bitmap );
	DebugPrint( "--" );
	if( rh.s.card_region_bitmap != region )
	{
		throw (gcnew System::Exception("In Rom Header, illegal region in the ROM Header."));
		return;
	}

	// 設定したレーティングが正しいかどうかをチェック
	System::Collections::Generic::List<int> ^ognlist = MasterEditorTWL::getOgnListInRegion( region );
	if( fItem->getOgnNumber() >= 0 )
	{
		// 「レーティング表示不要」でないとき

		int ogn = fItem->getOgnNumber();			// ファイル名に対応する真値を取得
		u8  rating = fItem->getRatingValue();
		DebugPrint( "{0,-10} {1,-20:X02} {2,-20:X02}", fItem->getOgnString(ogn), rating, rh.s.parental_control_rating_info[ogn] );
		if( rh.s.parental_control_rating_info[ ogn ] != rating )
		{
			throw (gcnew System::Exception("In Rom Header, mismatch Rating Ogn " + ogn.ToString() + "."));
			return;
		}

		// リージョンに含まれるその他の団体が「全年齢」になっているかチェック
		for each ( int ogn in ognlist )
		{
			if( ogn != fItem->getOgnNumber() )
			{
				u8  zero = OS_TWL_PCTL_OGNINFO_ENABLE_MASK | 0;
				DebugPrint( "{0,-10} {1,-20:X02} {2,-20:X02}", fItem->getOgnString(ogn), zero, rh.s.parental_control_rating_info[ogn] );
				if( rh.s.parental_control_rating_info[ogn] != zero )
				{
					throw (gcnew System::Exception("In Rom Header, Rating Ogn " + ogn.ToString() + " is not enabled."));
					return;
				}
			}
		}

		// 「レーティング不要」フラグが立っていてはいけない
		if( rh.s.unnecessary_rating_display != 0 )
		{
			throw (gcnew System::Exception("In Rom Header, \"Unnecessary\" flag is asserted."));
			return;
		}
	}
	else
	{
		// 「レーティング表示不要」のとき

		// リージョンに含まれるすべての団体が「全年齢」になっているかチェック
		for each ( int ogn in ognlist )
		{
			u8  zero = OS_TWL_PCTL_OGNINFO_ENABLE_MASK | 0;
			DebugPrint( "{0,-10} {1,-20:X02} {2,-20:X02}", fItem->getOgnString(ogn), zero, rh.s.parental_control_rating_info[ogn] );
			if( rh.s.parental_control_rating_info[ogn] != zero )
			{
				throw (gcnew System::Exception("In Rom Header, Rating Ogn " + ogn.ToString() + " is not enabled."));
				return;
			}
		}

		// フラグチェック
		if( rh.s.unnecessary_rating_display == 0 )
		{
			throw (gcnew System::Exception("In Rom Header, \"Unnecessary\" flag is negated."));
			return;
		}
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

	// リージョンに含まれない団体のレーティングがクリアされているかチェック
	int i;
	for( i=0; i < PARENTAL_CONTROL_INFO_SIZE; i++ )
	{
		if( ognlist->IndexOf(i) < 0 )
		{
			if( alllist->IndexOf(i) >= 0 )
			{
				DebugPrint( "{0,-10} {1,-20:X02} {2,-20:X02}", fItem->getOgnString(i), (u8)0, rh.s.parental_control_rating_info[i] );
			}
			else
			{
				DebugPrint( "{0,-10} {1,-20:X02} {2,-20:X02}", "Ogn" + i.ToString("D2") + "(rsv)", (u8)0, rh.s.parental_control_rating_info[i] );
			}
			if( rh.s.parental_control_rating_info[i] != 0 )
			{
				throw (gcnew System::Exception("In Rom Header, Rating Ogn " + i.ToString() + " is not cleared in ROM Header."));
				return;
			}
		}
	}
	DebugPrint( "--------------------------------------------------------" );
	
	// 全領域ベリファイ
	FILE       *fp1 = NULL;
	if( fopen_s( &fp1, chorg, "rb" ) != NULL )
	{
		throw (gcnew System::Exception("Fail to open the original SRL file."));
		return;
	}
	FILE       *fp2 = NULL;
	if( fopen_s( &fp2, chtarget, "rb" ) != NULL )
	{
		throw (gcnew System::Exception("Fail to open the target SRL file."));
		return;
	}
	// ファイルサイズをまずチェック
	fseek(fp1, 0, SEEK_END);
	u32 filesize1 = ftell( fp1 );
	fseek(fp2, 0, SEEK_END);
	u32 filesize2 = ftell( fp2 );
	DebugPrint( "{0,-10} {1,-20} {2,-20}", nullptr, "Original File", "Target File" );
	DebugPrint( "{0,-10} {1,-20:X08} {2,-20:X08}", "Filesize", filesize1, filesize2 );
	DebugPrint( "--------------------------------------------------------" );
	if( filesize1 != filesize2 )
	{
		throw (gcnew System::Exception("Incorrect filesize"));
		return;
	}
	// マスタエディタで書き換えられていない領域をチェック
	verifyArea( fp1, fp2, 0, 0x1b0 );
	verifyArea( fp1, fp2, 0x1b4, 0x2f0 - 0x1b4 );
	verifyArea( fp1, fp2, 0x300, filesize1 - 0x300 );
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

// -------------------------------------------------------------------
// 書き換えた箇所以外の全領域をベリファイしたいので
// 指定領域をベリファイする関数をつくる
// -------------------------------------------------------------------
#define VERIFY_AREA_BUFSIZE  (10*1024*1024)
System::Void verifyArea( FILE *fp1, FILE *fp2, const int offset, const int size )
{
    if( !fp1 || !fp2 )
    {
		throw (gcnew System::Exception("File pointer is NULL."));
        return;
    }

	DebugPrint( "{0,-10} {1:X08} - {2:X08}", "Verify", offset, offset+size-1 );

	cli::array<System::Byte> ^mbuf1 = gcnew cli::array<System::Byte>(VERIFY_AREA_BUFSIZE);	// 解放の必要なし
	pin_ptr<unsigned char> buf1 = &mbuf1[0];
	cli::array<System::Byte> ^mbuf2 = gcnew cli::array<System::Byte>(VERIFY_AREA_BUFSIZE);
	pin_ptr<unsigned char> buf2 = &mbuf2[0];

	fseek( fp1, offset, SEEK_SET );
    fseek( fp2, offset, SEEK_SET );

    // バッファよりも大きい場合は細切れにリードしてベリファイする
	int rest = size;
	while( rest > 0 )
    {
        int len = (rest > VERIFY_AREA_BUFSIZE)?(VERIFY_AREA_BUFSIZE):(rest);
        if( fread(buf1, 1, len, fp1) != len )
        {
			throw (gcnew System::Exception("In Verify, fail to fread fp1"));
            return;
        }
        if( fread(buf2, 1, len, fp2) != len )
        {
			throw (gcnew System::Exception("In Verify, fail to fread fp2"));
            return;
        }
        if( memcmp(buf1, buf2, len) != 0 )
        {
			throw (gcnew System::Exception("In Verify, incorrect area."));
        }
        rest = rest - len;
    }
	return;
}
