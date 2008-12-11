// utility.h の実装

#include "stdafx.h"
#include <apptype.h>
#include "common.h"
#include "utility.h"
#include <cstring>
#include <cstdio>
#include <twl/types.h>
#include <twl/os/common/format_rom.h>
#include <twl/os/common/ownerInfoEx.h>

//
// String を char 配列に格納
//
// @arg [out] 格納先
// @arg [in]  格納元
// @arg [in]  文字列長
// @arg [in]  余りを埋める padding
//
void MasterEditorTWL::setStringToChars( char *pDst, System::String ^hSrc, 
									    const System::Int32 nMax, const System::SByte pad )
{
	System::Int32 i;

	memset( pDst, pad, nMax );
	for( i=0; (i < hSrc->Length) && (i < nMax); i++ )
	{
		pDst[i] = (char)hSrc[i];
	}
}

//
// ROMヘッダ中のROM容量設定バイトからROM容量を表す文字列を取得
//
// @arg [in]  ROM容量設定バイト
//
// @ret 文字列(単位つき)
//
System::String^ MasterEditorTWL::transRomSizeToString( System::Byte ^byte )
{
	System::UInt32  size;
	System::String  ^unit;

	// RomSize = 2^(byte) [Mbit] 
	size = 1 << *(byte);

	if(size >= 1024)
	{
		unit = gcnew System::String("Gbit");
		size = size / 1024;
	}
	else
	{
		unit = gcnew System::String("Mbit");
	}
	return (size.ToString() + unit);
}

//
// サイズから単位つきの文字列を取得
//
// @arg [in] バイト数
//
// @ret 文字列(単位つき)
//
System::String^ MasterEditorTWL::transSizeToString( const System::UInt32 size )
{
	System::UInt32 val = size;

	// Byte単位
	if( val == 0 )
	{
		return val.ToString();
	}
	if( val < 1024 )
	{
		return (val.ToString() + " Byte");
	}

	// KB単位に変換
	if( (val % 1024) != 0 )
	{
		return (val.ToString() + " Byte");	// 端数のときは単位変換しない
	}
	val = val / 1024;
	if( val < 1024 )
	{
		return (val.ToString() + " KB");
	}
	
	// MB単位に変換
	if( (val % 1024) != 0 )
	{
		return (val.ToString() + " KB");
	}
	val = val / 1024;
	if( val < 1024)
	{
		return (val.ToString() + " MB");
	}

	// GB単位に変換
	if( (val % 1024) != 0 )
	{
		return (val.ToString() + " MB");
	}
	val = val / 1024;
	return (val.ToString() + " GB");
}

// 設定可能なインデックスのリストを取得する
cli::array<System::Byte>^ MasterEditorTWL::getOgnRatingAges( int ogn )
{
	cli::array<System::Byte> ^ages = gcnew cli::array<System::Byte>{0};

	if( ogn == OS_TWL_PCTL_OGN_CERO )
	{
		ages = gcnew cli::array<System::Byte>{0,12,15,17,18};
	}
	else if( ogn == OS_TWL_PCTL_OGN_ESRB )
	{
		ages = gcnew cli::array<System::Byte>{0,3,6,10,13,17};
	}
	else if( ogn == OS_TWL_PCTL_OGN_USK )
	{
		ages = gcnew cli::array<System::Byte>{0,6,12,16,18};
	}
	else if( ogn == OS_TWL_PCTL_OGN_PEGI_GEN )
	{
		ages = gcnew cli::array<System::Byte>{0,3,7,12,16,18};
	}
	else if( ogn == OS_TWL_PCTL_OGN_PEGI_PRT )
	{
		ages = gcnew cli::array<System::Byte>{0,4,6,12,16,18};
	}
	else if( ogn == OS_TWL_PCTL_OGN_PEGI_BBFC )
	{
		ages = gcnew cli::array<System::Byte>{0,3,4,7,8,12,15,16,18};
	}
	else if( ogn == OS_TWL_PCTL_OGN_OFLC )
	{
		ages = gcnew cli::array<System::Byte>{0,7,14,15};
	}
	return ages;
}

// 設定可能なインデックスのリストを取得する
System::String^ MasterEditorTWL::getOgnName( int ogn )
{
	System::String ^name = gcnew System::String("");

	if( ogn == OS_TWL_PCTL_OGN_CERO )
	{
		name = gcnew System::String("CERO");
	}
	else if( ogn == OS_TWL_PCTL_OGN_ESRB )
	{
		name = gcnew System::String("ESRB");
	}
	else if( ogn == OS_TWL_PCTL_OGN_USK )
	{
		name = gcnew System::String("USK");
	}
	else if( ogn == OS_TWL_PCTL_OGN_PEGI_GEN )
	{
		name = gcnew System::String("PEGI(General)");
	}
	else if( ogn == OS_TWL_PCTL_OGN_PEGI_PRT )
	{
		name = gcnew System::String("PEGI(Portugal)");
	}
	else if( ogn == OS_TWL_PCTL_OGN_PEGI_BBFC )
	{
		name = gcnew System::String("PEGI and BBFC");
	}
	else if( ogn == OS_TWL_PCTL_OGN_OFLC )
	{
		name = gcnew System::String("OFLC");
	}
	return name;
}

//
// リージョンに含まれる団体のリストを返す(不正なリージョンのときはnullptr)
//
// @arg [in] リージョン
//
System::Collections::Generic::List<int>^ MasterEditorTWL::getOgnListInRegion( u32 region )
{
	System::Collections::Generic::List<int> ^list = gcnew System::Collections::Generic::List<int>;

	switch( region )
	{
		case METWL_MASK_REGION_JAPAN:
			list->Add( OS_TWL_PCTL_OGN_CERO );	// リージョンに含まれない団体の情報は読み込まない
		break;

		case METWL_MASK_REGION_AMERICA:
			list->Add( OS_TWL_PCTL_OGN_ESRB );
		break;

		case METWL_MASK_REGION_EUROPE:
			list->Add( OS_TWL_PCTL_OGN_USK );
			list->Add( OS_TWL_PCTL_OGN_PEGI_GEN );
			list->Add( OS_TWL_PCTL_OGN_PEGI_PRT );
			list->Add( OS_TWL_PCTL_OGN_PEGI_BBFC );
		break;

		case METWL_MASK_REGION_AUSTRALIA:
			list->Add( OS_TWL_PCTL_OGN_OFLC );
		break;

		case (METWL_MASK_REGION_EUROPE|METWL_MASK_REGION_AUSTRALIA):
			list->Add( OS_TWL_PCTL_OGN_USK );
			list->Add( OS_TWL_PCTL_OGN_PEGI_GEN );
			list->Add( OS_TWL_PCTL_OGN_PEGI_PRT );
			list->Add( OS_TWL_PCTL_OGN_PEGI_BBFC );
			list->Add( OS_TWL_PCTL_OGN_OFLC );
		break;

#if defined(METWL_VER_APPTYPE_SYSTEM) || defined(METWL_VER_APPTYPE_SECURE) || defined(METWL_VER_APPTYPE_LAUNCHER)
		case METWL_MASK_REGION_ALL:

			list->Add( OS_TWL_PCTL_OGN_CERO );
			list->Add( OS_TWL_PCTL_OGN_ESRB );
			list->Add( OS_TWL_PCTL_OGN_USK );
			list->Add( OS_TWL_PCTL_OGN_PEGI_GEN );
			list->Add( OS_TWL_PCTL_OGN_PEGI_PRT );
			list->Add( OS_TWL_PCTL_OGN_PEGI_BBFC );
			list->Add( OS_TWL_PCTL_OGN_OFLC );
		break;
#endif //#if defined(METWL_VER_APPTYPE_SYSTEM) || defined(METWL_VER_APPTYPE_SECURE) || defined(METWL_VER_APPTYPE_LAUNCHER)

		default:
			list = nullptr;
		break;
	}
	return list;
}

//
// バイト列に特定のパターンが含まれるかどうかマッチングする
//
// @arg [in] テキスト
//      [in] テキストの長さ
//      [in] パターン
//      [in] パターンの長さ
//      [in] テキストの終端まで調べた時点でテキストが途中までマッチしていた場合を成功とみなすか
//
// @ret マッチしたテキストのオフセットをリストで返す。
//
System::Collections::Generic::List<u32>^ MasterEditorTWL::patternMatch( 
	const u8 *text, const u32 textLen, const u8 *pattern, const u32 patternLen, const System::Boolean enableLast )
{
	// ひとまずシンプルな方法で実装する
	u32  first;
	u32  len;
	System::Collections::Generic::List<u32> ^list = gcnew System::Collections::Generic::List<u32>();
	list->Clear();

	// パターンを1文字ずつずらしながらマッチング
	for( first=0; first < textLen; first++ )
	{
		len = (patternLen < (textLen-first))?patternLen:(textLen-first);	// 最後のほうは途中までしかマッチングしない
		if( *(text+first) == pattern[0] )		// 高速化のため最初の文字が一致したときのみ全文マッチング
		{
			if( memcmp( text+first, pattern, len ) == 0 )
			{
				if( (enableLast == true) )
				{
					list->Add( first );
				}
				else if( len == patternLen )	// 完全一致しないとダメ
				{
					list->Add( first );
				}
			}
		}
	}
	return list;
} // MasterEditorTWL::patternMatch

////
//// バイト列に特定のパターンが含まれるかどうかマッチングする (Boyer-Moore法)
////
//// @arg [in] テキスト
////      [in] テキストの長さ
////      [in] パターン
////      [in] パターンの長さ
////      [in] skipマップ(あらかじめ生成しておく)
////
//// @ret マッチしたテキストのインデックスをリストで返す。
////
//System::Collections::Generic::List<u32>^ MasterEditorTWL::patternMatchBM(
//	const u8 *text, const int textLen, const u8 *pattern, const int patternLen, const int skip[] )
//{
//	System::Collections::Generic::List<u32> ^list = gcnew System::Collections::Generic::List<u32>();
//	list->Clear();
//
//	if( textLen < patternLen )
//	{
//		return list;
//	}
//
//	int pos = patternLen-1;
//	while( pos < textLen )		// イメージとしてはパターンを固定してテキストの方をずらしていく感じ
//	{
//		int first = pos - (patternLen-1);
//		if( memcmp( text+first, pattern, patternLen ) == 0 )	// 本来は末尾から探索するがめんどうなので先頭からmemcmp
//		{
//			list->Add( first );
//		}
//		pos = pos + skip[ (int)text[pos] ];	// パターンの末尾と照合するテキスト位置をずらす
//	}
//	return list;
//}
//
////
//// Boyer-Moore法のskipマップの生成
////
//// @arg [in]  パターン
////      [in]  パターンの長さ
////      [out] skipマップの格納先
////
//// @ret なし
////
//void MasterEditorTWL::makeSkipBM( const u8 *pattern, const int patternLen, int skip[] )
//{
//	int i;
//	for( i=0; i < 256; i++ )			// すべての文字(出現しうるデータ)についてマップを初期化
//	{
//		skip[i] = patternLen;			// パターンにない文字なのでパターンを丸ごとスキップできる
//	}
//	for( i=0; i < (patternLen-1); i++ )	// 末尾を除くすべてのパターン出現文字について(最後の)出現位置を登録
//	{
//		skip[ (int)pattern[i] ] = patternLen - 1 - i;
//	}
//}

//
// ファイルにバイト列のパターンが含まれるかどうかマッチングする
//
// @arg [in] マッチ対象となるファイルポインタ
// @arg [in] パターン
// @arg [in] パターンの長さ(PATTERN_MATCH_LEN_MAX以下でなければならない)
//
// @ret マッチしたテキストのオフセットをリストで返す。
//      最後までマッチした場合のみ成功したとみなす。
//
#define PATTERN_MATCH_LEN_MAX	(10*1024)
System::Collections::Generic::List<u32>^ MasterEditorTWL::patternMatch( FILE *fp, const u8 *pattern, const u32 patternLen )
{
	u8  text[ 2 * PATTERN_MATCH_LEN_MAX ];		// バッファの切れ目を探索するため多めに読み込むので最大でパターンの2倍だけバッファが必要になる
	u32 cur;
	u32 filesize;
	u32 len;
	u32 extra;
	System::Collections::Generic::List<u32> ^tmplist;
	System::Collections::Generic::List<u32> ^list = gcnew System::Collections::Generic::List<u32>;
	list->Clear();

	if( patternLen > PATTERN_MATCH_LEN_MAX )
		return nullptr;

	fseek( fp, 0, SEEK_END );
	filesize = ftell( fp );

	cur = 0;
	while( cur < filesize )
	{
		// バッファの切れ目を調べたいため実際には(パターンの長さ-1)だけ多めにリードする
		len   = ((filesize - cur) < PATTERN_MATCH_LEN_MAX)?(filesize - cur):PATTERN_MATCH_LEN_MAX;
		extra = (len <= PATTERN_MATCH_LEN_MAX)?0:(patternLen-1);	// 最後までいったときには余分にリードしてはいけない
		fseek( fp, cur, SEEK_SET );
		u32 readlen = fread( text, 1, len + extra, fp );
		if( (len + extra) != readlen )
		{
			//System::Diagnostics::Debug::WriteLine( "actual len = " + readlen.ToString() );
			//System::Diagnostics::Debug::WriteLine( "expect len = " + (len + extra).ToString() );
			return nullptr;
		}

		// テキスト終端に途中までマッチングしたときは失敗とみなす
		tmplist = MasterEditorTWL::patternMatch( text, len+extra, pattern, patternLen, false );
		if( tmplist != nullptr )
		{
			for each( u32 tmpval in tmplist )
			{
				list->Add( tmpval + cur );	// 実際のオフセットはファイルオフセットを加えた値
			}
		}

		// 次のSEEK位置:多めにリードしたはずらす
		cur += len;
	}
	return list;
}

//
// XMLのルートノードから指定した名前のタグを検索して返す
//
// @arg [in] XMLのルートノード
// @arg [in] タグ名
//
// @ret 検索でマッチしたときノードを返す。ないときは nullptr。
//      ただし、最初にマッチしたもののみ返す
//
System::Xml::XmlNode^ MasterEditorTWL::searchXmlNode( System::Xml::XmlElement ^root, System::String ^tag )
{
	System::Xml::XmlNodeList ^list = root->GetElementsByTagName( tag );
	System::Xml::XmlNode ^item = nullptr;
	if( list != nullptr )
	{
		item = list->Item(0);
	}
	return item;
}

//
// タグを検索してそのテキストが指定したテキストと一致するか調べる
//
// @arg [in] XMLのルートノード
// @arg [in] タグ名
// @arg [in] 値
//
// @ret 一致するときtrue。一致しないとき、タグが存在しないときはfalse。
//
System::Boolean MasterEditorTWL::isXmlEqual( System::Xml::XmlElement ^root, System::String ^tag, System::String ^val )
{
	System::Xml::XmlNode ^item = MasterEditorTWL::searchXmlNode( root, tag );
	if( (item != nullptr) && (item->FirstChild != nullptr) && (item->FirstChild->Value->Equals( val )) )
	{
		return true;
	}
	return false;
}

//
// タグを検索してそのテキストを返す
//
// @arg [in] XMLのルートノード
// @arg [in] タグのXPath
//
// @ret テキストが存在するときそのテキストを返す。存在しないときnullptr。
//
System::String^ MasterEditorTWL::getXPathText( System::Xml::XmlElement ^root, System::String ^xpath )
{
	System::Xml::XmlNode ^tmp = root->SelectSingleNode( xpath );
	if( tmp && tmp->FirstChild && tmp->FirstChild->Value )
	{
		return tmp->FirstChild->Value;
	}
	return nullptr;
}

//
// XMLにタグを追加する
//
// @arg [in] XMLドキュメント
// @arg [in] 親ノード
// @arg [in] タグ名
// @arg [in] テキスト
//
void MasterEditorTWL::appendXmlTag( System::Xml::XmlDocument ^doc, System::Xml::XmlElement ^parent,
									System::String ^tag, System::String ^text )
{
	if( System::String::IsNullOrEmpty( text ) )
		return;

	System::Xml::XmlElement ^node = doc->CreateElement( tag );
	node->AppendChild( doc->CreateTextNode( text ) );
	parent->AppendChild( node );
}

//
// ビット数をカウントする
//
// @arg [in] 値
//
// @ret ビット数
//
u32 MasterEditorTWL::countBits( const u32 val )
{
	// 分割統治法:隣り合うかたまりごとにビット数をカウントしていく
	u32 n = val;
	
	n = ((n>>1) & 0x55555555) + (n & 0x55555555);
    n = ((n>>2) & 0x33333333) + (n & 0x33333333);
    n = ((n>>4) & 0x0f0f0f0f) + (n & 0x0f0f0f0f);
    n = ((n>>8) & 0x00ff00ff) + (n & 0x00ff00ff);
    n = (n>>16) + (n & 0x0000ffff);
    return n;
}
