#include "stdafx.h"

#include <apptype.h>
#include <twl/types.h>
#include <twl/os/common/format_rom.h>
#include <acsign/include/acsign.h>
#include <cstring>
#include <cstdio>
#include <MasterEditorTWL/keys.h>
#include "fingerprint_util.h"

using namespace MasterEditorTWL;

// ----------------------------------------------------------------------
// ファイル処理
// ----------------------------------------------------------------------

// ファイル全体の読み込み
cli::array<System::Byte>^ ReadBin( System::String ^path )
{
	System::IntPtr ptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( path );
	const char *pch = (const char*)ptr.ToPointer();
	FILE       *fp = NULL;
	System::Exception ^ex = nullptr;
	cli::array<System::Byte> ^bin = nullptr;

	try
	{
		if( fopen_s( &fp, pch, "rb" ) != NULL )
		{
			ex = gcnew System::Exception( "Failed to open file:" + path );
			throw ex;
		}

		// ファイルサイズ分のメモリ割り当て
		fseek( fp, 0, SEEK_END );
		int size = ftell(fp);
		bin = gcnew cli::array<System::Byte>(size);
		pin_ptr<unsigned char> pbin = &bin[0];
		
		fseek( fp, 0, SEEK_SET );
		if( fread( (void*)pbin, 1, size, fp ) != size )
		{
			ex = gcnew System::Exception( "Failed to read data from " + path );
			throw ex;
		}
	}
	finally
	{
		System::Runtime::InteropServices::Marshal::FreeHGlobal( ptr );
		if( fp ) fclose(fp);
		if( ex )
		{
			throw ex;
		}
	}
	return bin;
}

// ファイル全体のライト
void WriteBin( System::String ^path, cli::array<System::Byte> ^bin )
{
	System::IntPtr ptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( path );
	const char *pch = (const char*)ptr.ToPointer();
	FILE       *fp = NULL;
	System::Exception ^ex = nullptr;

	try
	{
		if( fopen_s( &fp, pch, "w+b" ) != NULL )	// 上書き・バイナリ
		{
			ex = gcnew System::Exception( "Failed to open file:" + path );
			throw ex;
		}

		int size = bin->Length;
		pin_ptr<unsigned char> pbin = &bin[0];

		(void)fseek( fp, 0, SEEK_SET );
		if( fwrite( (const void*)pbin, 1, size, fp ) != size )
		{
			ex = gcnew System::Exception( "Failed to write data to " + path );
			throw ex;
		}
	}
	finally
	{
		System::Runtime::InteropServices::Marshal::FreeHGlobal( ptr );
		if( fp ) fclose( fp );
		if( ex )
		{
			throw ex;
		}
	}
}

// ROMヘッダのリード
void ReadRomHeader( System::String ^srlpath, ROM_Header *dstrh )
{
	System::IntPtr ptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( srlpath );
	const char *pchFile = (const char*)ptr.ToPointer();
	FILE       *fp = NULL;
	System::Exception ^ex = nullptr;

	try
	{
		if( fopen_s( &fp, pchFile, "rb" ) != NULL )
		{
			ex = gcnew System::Exception( "Failed to open file:" + srlpath );
			throw ex;
		}
		fseek( fp, 0, SEEK_SET );
		if( fread( (void*)dstrh, 1, sizeof(ROM_Header), fp ) != sizeof(ROM_Header) )
		{
			ex = gcnew System::Exception( "Failed to read data from " + srlpath );
			throw ex;
		}
	}
	finally
	{
		System::Runtime::InteropServices::Marshal::FreeHGlobal( ptr );
		if( fp ) fclose(fp);
		if( ex )
		{
			throw ex;
		}
	}
}

// ROMヘッダのライト
void WriteRomHeader( System::String ^srlpath, ROM_Header *srcrh )
{
	System::IntPtr ptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( srlpath );
	const char *pchFile = (const char*)ptr.ToPointer();
	FILE       *fp = NULL;
	System::Exception ^ex = nullptr;

	try
	{
		if( fopen_s( &fp, pchFile, "r+b" ) != NULL )	// 上書き・バイナリ
		{
			ex = gcnew System::Exception( "Failed to open file:" + srlpath );
			throw ex;
		}
		(void)fseek( fp, 0, SEEK_SET );

		if( fwrite( (const void*)srcrh, 1, sizeof(ROM_Header), fp ) != sizeof(ROM_Header) )
		{
			ex = gcnew System::Exception( "Failed to write data to " + srlpath );
			throw ex;
		}
	}
	finally
	{
		System::Runtime::InteropServices::Marshal::FreeHGlobal( ptr );
		if( fp ) fclose( fp );
		if( ex )
		{
			throw ex;
		}
	}
}

// バイナリからのROMヘッダの抽出
void ExtractRomHeader( cli::array<System::Byte> ^bin, ROM_Header *dstrh )
{
	int size = bin->Length;
	pin_ptr<unsigned char> pbin = &bin[0];

	if( size < sizeof(ROM_Header) )
	{
		throw gcnew System::Exception( "The binary size is less than size of the ROM Header." );
	}
	memcpy( dstrh, pbin, sizeof(ROM_Header) );
}

// バイナリへのROMヘッダの上書き
void OverwriteRomHeader( cli::array<System::Byte> ^bin, ROM_Header *srcrh )
{
	int size = bin->Length;
	pin_ptr<unsigned char> pbin = &bin[0];

	if( size < sizeof(ROM_Header) )
	{
		throw gcnew System::Exception( "The binary size is less than size of the ROM Header." );
	}
	memcpy( pbin, srcrh, sizeof(ROM_Header) );
}

// ファイルコピー
#define COPY_FILE_BUFSIZE   (10*1024*1024)
void CopyFile( System::String ^srcpath, System::String ^dstpath )
{
	System::IntPtr srcptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( srcpath );
	System::IntPtr dstptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( dstpath );
	const char *pchSrcFile = (const char*)srcptr.ToPointer();
	const char *pchDstFile = (const char*)dstptr.ToPointer();
	FILE       *ifp = NULL;
	FILE       *ofp = NULL;
	System::Exception ^ex = nullptr;

	try
	{
		if( fopen_s( &ifp, pchSrcFile, "rb" ) != NULL )
		{
			ex = gcnew System::Exception( "Failed to open file:" + srcpath );
			throw ex;
		}
		if( fopen_s( &ofp, pchDstFile, "wb" ) != NULL )	// 同名ファイルを削除して新規にライト・バイナリ
		{
			ex = gcnew System::Exception( "Failed to open file:" + dstpath );
			throw ex;
		}

		// 入力ファイルのサイズ取得
		fseek( ifp, 0, SEEK_END );
		System::Int32 size = ftell(ifp);

		cli::array<System::Byte> ^mbuf = gcnew cli::array<System::Byte>(COPY_FILE_BUFSIZE);	// 解放の必要なし
		pin_ptr<unsigned char> buf = &mbuf[0];
		fseek( ifp, 0, SEEK_SET );
		fseek( ofp, 0, SEEK_SET );
		while( size > 0 )
		{
			System::Int32 datasize = (COPY_FILE_BUFSIZE < size)?COPY_FILE_BUFSIZE:size;

			if( datasize != fread(buf, 1, datasize, ifp) )
			{
				ex = gcnew System::Exception( "Failed to read data from " + srcpath );
				throw ex;
			}
			if( datasize != fwrite(buf, 1, datasize, ofp) )
			{
				ex = gcnew System::Exception( "Failed to write data to " + dstpath );
				throw ex;
			}
			size -= datasize;
		}
	}
	finally
	{
		System::Runtime::InteropServices::Marshal::FreeHGlobal( srcptr );
		System::Runtime::InteropServices::Marshal::FreeHGlobal( dstptr );
		if( ifp ) fclose(ifp);
		if( ofp ) fclose(ofp);
		if( ex )
		{
			throw ex;
		}
	}
}

// ----------------------------------------------------------------------
// 署名
// ----------------------------------------------------------------------

// ROMヘッダに署名をつける
void SignRomHeader( ROM_Header *rh )
{
	SignatureData   signSrc;						// 署名のもととなるダイジェスト値
	u8              signDst[ RSA_KEY_LENGTH ];		// 署名の格納先Tmpバッファ
	u8              decryptBlock[ RSA_KEY_LENGTH ];	// 署名を解除後ブロックバッファ
	BOOL            result = false;
	int             pos;
	u8              *privateKey = (u8*)g_devPrivKey_DER;
	u8              *publicKey  = (u8*)g_devPubKey_DER;

	// ROMヘッダのダイジェストを算出(先頭から証明書領域の直前までが対象)
	ACSign_DigestUnit(
		signSrc.digest,
		rh,
		(u32)&(rh->certificate) - (u32)rh
	);

	// 鍵を選ぶ
#ifdef METWL_VER_APPTYPE_USER
	{
		privateKey = (u8*)g_devPrivKey_DER;
		publicKey  = (u8*)g_devPubKey_DER;
	}
#endif //METWL_VER_APPTYPE_USER

	// ダイジェストに署名をかける
	result = ACSign_Encrypto( signDst, privateKey, &signSrc, sizeof(SignatureData) ); 
	if( !result )
	{
		throw gcnew System::Exception( "Exception: Failed to calc the hash of the ROM header." );
	}

	// 署名を解除してダイジェストと一致するかベリファイする
	result = ACSign_Decrypto( decryptBlock, publicKey, signDst, RSA_KEY_LENGTH );
	for( pos=0; pos < RSA_KEY_LENGTH; pos++ )
	{
		if( decryptBlock[pos] == 0x0 )			// 解除後ブロックから実データをサーチ
			break;
	}
	if( !result || (memcmp( &signSrc, &(decryptBlock[pos+1]), sizeof(SignatureData) ) != 0) )
	{
		throw gcnew System::Exception( "Exception: Failed to compare the hash of the ROM header." );
	}

	// ROMヘッダに署名を上書き
	memcpy( rh->signature, signDst, RSA_KEY_LENGTH );

} // ECSrlResult RCSrl::signRomHeader(void)

// ROMヘッダの署名チェック
void AuthenticateRomHeader( ROM_Header *rh )
{
	u8     original[ RSA_KEY_LENGTH ];	// 署名外した後のデータ格納先
	s32    pos = 0;						// ブロックの先頭アドレス
	u8     digest[ DIGEST_SIZE_SHA1 ];	// ROMヘッダのダイジェスト
	u8    *publicKey = (u8*)g_devPubKey_DER;

	// <データの流れ>
	// (1) 公開鍵で復号した結果(ブロック)をローカル変数(original)に格納
	// (2) ブロックから余分な部分を取り除いて引数(pDst)にコピー

#ifdef METWL_VER_APPTYPE_USER
	{
		publicKey  = (u8*)g_devPubKey_DER;
	}
#endif //METWL_VER_APPTYPE_USER

	// 署名の解除 = 公開鍵で復号
	if( !ACSign_Decrypto( original, publicKey, rh->signature, RSA_KEY_LENGTH ) )
	{
		throw gcnew System::Exception("Failed to decrypt the ROM signature.");
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
	ACSign_DigestUnit( digest,	rh, (u32)&(rh->certificate) - (u32)rh );
	if( memcmp( &(original[pos+1]), digest, DIGEST_SIZE_SHA1 ) != 0 )
	{
		System::String ^msg =
			"Failed to verify the ROM digest data.\r\n" 
			+"Either this is not TWL-compatible or TWL-exclusive ROM data, or the ROM data may have been modified.";
		throw gcnew System::Exception(msg);
	}
}

// ----------------------------------------------------------------------
// 変換
// ----------------------------------------------------------------------

// 文字列をバイト配列に変換
cli::array<System::Byte>^ TransStringToBytes( System::String ^src, const int len )
{
	cli::array<System::Byte> ^bytes = gcnew cli::array<System::Byte>(len);

	int i;
	for( i=0; i < len; i++ )
	{
		bytes[i] = 0;
	}

	int n = (len < src->Length)?len:src->Length;
	for( i=0; i < src->Length; i++ )
	{
		if( i < len )
		{
			bytes[i] = (System::Byte)src[i];
		}
	}
	return bytes;
}

// 16進文字列をバイト配列に変換
cli::array<System::Byte>^ TransHexStringToBytes( System::String ^src, const int len )
{
	cli::array<System::Byte> ^bytes = gcnew cli::array<System::Byte>(len);

	int i;
	for( i=0; i < len; i++ )
	{
		bytes[i] = 0;
	}

	int j=0;
	for( i=0; i < src->Length; )
	{
		if( src[i] == ' ' )
		{
			i++;
			continue;
		}
		if( (src[i] != ' ') && (j < len) )
		{
			if( (i+1) >= src->Length )
			{
				throw gcnew System::Exception("One hexadecimal number must be expressed by 2 charactors, for example \"6B\".");
			}
			bytes[j++] = System::Byte::Parse(src->Substring(i,2), System::Globalization::NumberStyles::HexNumber);
			i += 2;
		}
	}
	return bytes;
}

// バイト配列を文字列に変換
System::String^ TransBytesToString( cli::array<System::Byte> ^bytes )
{
	System::String ^str = gcnew System::String("");
	int i;
	for( i=0; i < bytes->Length; i++ )
	{
		if( (0x20 <= bytes[i]) && (bytes[i] <= 0x7E) )
		{
			char c[2];
			c[0] = bytes[i]; c[1] = 0;
			str += gcnew System::String( c );
		}
		else
		{
			str += " ";
		}
		if( !((i+1) % 16) )
		{
			str += "\r\n";
		}
	}
	return str;
}

// バイト配列を16進文字列に変換
System::String^ TransBytesToHexString( cli::array<System::Byte> ^bytes )
{
	System::String ^str = gcnew System::String("");

	int i;
	for( i=0; i < bytes->Length; i++ )
	{
		System::Byte b = bytes[i];
		str += System::String::Format( "{0:X02}", (char)b );
		if( (i+1) % 16 )
		{
			str += " ";
		}
		else
		{
			str += "\r\n";
		}
	}
	return str;
}


// ----------------------------------------------------------------------
// GUI
// ----------------------------------------------------------------------

// ファイルをダイアログで取得
// @arg [in] デフォルトのディレクトリ
// @arg [in] 拡張子フィルタ
// @ret 取得したファイル名 エラーのとき nullptr
System::String^ OpenFileUsingDialog( System::String ^defdir, System::String ^filter )
{
	System::Windows::Forms::OpenFileDialog ^dlg = gcnew (System::Windows::Forms::OpenFileDialog);
	if( System::String::IsNullOrEmpty( defdir ) || !System::IO::Directory::Exists( defdir ) )
	{
		dlg->InitialDirectory = System::Environment::GetFolderPath( System::Environment::SpecialFolder::Desktop );
	}
	else
	{
		dlg->InitialDirectory = defdir;	// 前に選んだディレクトリをデフォルトにする
	}
	dlg->Filter      = filter;
	dlg->FilterIndex = 1;
	dlg->RestoreDirectory = true;

	if( dlg->ShowDialog() != System::Windows::Forms::DialogResult::OK )
	{
		return nullptr;
	}
	return System::String::Copy(dlg->FileName);
}

// セーブするファイルをダイアログで取得
// @arg [in] デフォルトのディレクトリ
// @arg [in] 拡張子フィルタ
// @arg [in] ファイルの拡張子が不正なときに追加するときの正しい拡張子
// @ret 取得したファイル名 エラーのとき nullptr
System::String^ SaveFileUsingDialog( System::String ^defdir, System::String ^filter, System::String ^extension )
{
	return (SaveFileUsingDialog( defdir, nullptr, filter, extension ));
}

// セーブするファイルをダイアログで取得
// @arg [in] デフォルトのディレクトリ
// @arg [in] デフォルトのファイル名
// @arg [in] 拡張子フィルタ
// @arg [in] ファイルの拡張子が不正なときに追加するときの正しい拡張子
// @ret 取得したファイル名 エラーのとき nullptr
System::String^ SaveFileUsingDialog( System::String ^defdir, System::String ^defname, System::String ^filter, System::String ^extension )
{
	System::String ^retfile;
	System::Windows::Forms::SaveFileDialog ^dlg = gcnew (System::Windows::Forms::SaveFileDialog);

	if( System::String::IsNullOrEmpty( defdir ) || !System::IO::Directory::Exists( defdir ) )
	{
		dlg->InitialDirectory = System::Environment::GetFolderPath( System::Environment::SpecialFolder::Desktop );
	}
	else
	{
		dlg->InitialDirectory = defdir;
	}
	dlg->Filter      = filter;
	dlg->FilterIndex = 1;
	dlg->RestoreDirectory = true;
	if( defname )
	{
		dlg->FileName = defname;
	}

	if( dlg->ShowDialog() != System::Windows::Forms::DialogResult::OK )
	{
		return nullptr;
	}
	retfile = dlg->FileName;
	if( !System::String::IsNullOrEmpty(extension) && !(dlg->FileName->ToLower()->EndsWith( extension->ToLower() )) )
	{
		retfile += extension;
	}
	return retfile;
}

// ----------------------------------------------------------------------
// 外部プログラムの実行
// ----------------------------------------------------------------------

// maketadの実行
// @arg [in] maketadのパス
// @arg [in] 入力SRLのパス
// @arg [in] 出力SRLのパス
void makeTad( System::String ^maketad_path, System::String ^srlpath, System::String ^tadpath )
{
	System::Diagnostics::ProcessStartInfo ^info = gcnew System::Diagnostics::ProcessStartInfo();
	info->FileName  = maketad_path; 
	info->Arguments = "\"" + srlpath + "\" -o \"" + tadpath + "\"";
	info->CreateNoWindow  = true;
	info->UseShellExecute = false;
	info->RedirectStandardError  = true;

	System::Diagnostics::Process ^p = System::Diagnostics::Process::Start( info );
	//p->WaitForExit();
	System::String ^output = p->StandardError->ReadToEnd();
	output->Replace( (char)'\r\r\n', (char)'\n' );
	System::Console::WriteLine( "{0}", output );

	if( output != "" )
	{
		throw gcnew System::Exception("Failed to transform SRL to TAD.");
	}
	else
	{
		System::Console::WriteLine( "Succeeded to transform." );
	}
}

// ----------------------------------------------------------------------
// コンソール
// ----------------------------------------------------------------------

// getopt : 引数を解析してオプションが見つかったら1つ返す
// @arg [in/out] 引数 (オプションが見つかった場合削除されていく)
// @arg [in]     オプション許可リスト
// @arg [out]    オプション引数の格納場所 (ない場合 nullptrが入る)
// @ret          最初に見つかったオプション
char getopt( cli::array<System::String^> ^%args, System::String ^condition, System::String ^% optarg )
{
	char opt=0;

	// 初期化
	optarg = nullptr;

	// オプションを探して条件に合うかどうかを調べる
	int ai;
	int ci = 0;
	for( ai=0; ai < args->Length; ai++ )
	{
		System::String ^arg = args[ai];
		if( arg->StartsWith("-") && arg->Length >= 2 )
		{
			// 条件に合うオプションかどうかを調べる
			ci = condition->IndexOf( arg[1] );
			if( ci >= 0 )
			{
				opt = (char)condition[ci];
			}
			break;
		}
	}
	// オプションはあったが条件にない不正なオプションだった
	if( ci < 0 )
	{
		return ((char)0);
	}
	// オプションが存在しなかった
	if( ai == args->Length )
	{
		return ((char)-1);
	}

	// オプション引数があるかチェック
	if( ((ci+1) < condition->Length) && (condition[ci+1] == ':') )
	{
		if( ((ai+1) < args->Length) && !args[ai+1]->StartsWith("-") )
		{
			optarg = System::String::Copy(args[ai+1]);
			shrinkArgs( args, ai+1);
		}
		else
		{
			return ((char)0);	// オプション引数がない
		}
	}

	// オプションを削除する
	shrinkArgs( args, ai );
	return opt;	
}

// args から特定の要素を削除
// @arg [in] 引数
// @arg [in] 削除する要素のインデックス
void shrinkArgs( cli::array<System::String^> ^%args, const int index )
{
	int i;
	for( i=index+1; i < args->Length; i++ )
	{
		args[i-1] = args[i];
	}
	cli::array<System::String^>::Resize( args, args->Length - 1 );
}
