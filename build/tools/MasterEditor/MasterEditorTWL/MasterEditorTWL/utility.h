#pragma once

// 共用ライブラリ・クラスの宣言

#include <twl/types.h>
#include <cstring>
#include <cstdio>

namespace MasterEditorTWL
{

	// -------------------------------------------------------------------
	// functions
	// -------------------------------------------------------------------

	//
	// String を char 配列に格納
	//
	// @arg [out] 格納先
	// @arg [in]  格納元
	// @arg [in]  文字列長
	// @arg [in]  余りを埋める padding
	//
	void setStringToChars( char *pDst, System::String ^hSrc, const System::Int32 nMax, const System::SByte pad  );

	//
	// ROMヘッダ中のROM容量設定バイトからROM容量を表す文字列を取得
	//
	// @arg [in]  ROM容量設定バイト
	//
	// @ret 文字列(単位つき)
	//
	System::String^ transRomSizeToString( System::Byte ^byte );

	//
	// サイズから単位つきの文字列を取得
	//
	// @arg [in] バイト数
	//
	// @ret 文字列(単位つき)
	//
	System::String^ transSizeToString( const System::UInt32 size );

	//
	// 各レーティング団体の設定可能年齢を取得する
	//
	// @arg [in] 団体
	//
	cli::array<System::Byte>^ getOgnRatingAges( int ogn );

	//
	// 各レーティング団体の団体名を取得する
	//
	// @arg [in] 団体
	//
	System::String^ getOgnName( int ogn );

	//
	// リージョンに含まれる団体のリストを返す(不正なリージョンのときはnullptr)
	//
	// @arg [in] リージョン
	//
	System::Collections::Generic::List<int>^ getOgnListInRegion( u32 region );

	//
	// バイト列に特定のパターンが含まれるかどうかマッチングする
	//
	// @arg [in] テキスト
	//      [in] テキストの長さ
	//      [in] パターン
	//      [in] パターンの長さ
	//      [in] テキストの終端まで調べた時点でテキストが途中までマッチしていた場合を成功とみなすか
	//
	// @ret マッチしたテキストのインデックスをリストで返す。
	//
	System::Collections::Generic::List<u32>^ patternMatch( 
		const u8 *text, const u32 textLen, const u8 *pattern, const u32 patternLen, const System::Boolean enableLast );

	//
	// ファイルにバイト列のパターンが含まれるかどうかマッチングする
	//
	// @arg [in] マッチ対象となるファイルポインタ
	// @arg [in] パターン
	// @arg [in] パターンの長さ
	//
	// @ret マッチしたテキストのインデックスをリストで返す。
	//      最後までマッチした場合のみ成功したとみなす。
	//
	System::Collections::Generic::List<u32>^ patternMatch( FILE *fp, const u8 *pattern, const u32 patternLen );

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
	//System::Collections::Generic::List<u32>^ patternMatchBM(
	//	const u8 *text, const int textLen, const u8 *pattern, const int patternLen, const int skip[] );

	////
	//// Boyer-Moore法のskipマップの生成
	////
	//// @arg [in]  パターン
	////      [in]  パターンの長さ
	////      [out] skipマップの格納先
	////
	//// @ret なし
	////
	//void makeSkipBM( const u8 *pattern, const int patternLen, int skip[] );

	//
	// XMLのルートノードから指定した名前のタグを検索して返す
	//
	// @arg [in] XMLのルートノード
	// @arg [in] タグ名
	//
	// @ret 検索でマッチしたときノードを返す。ないときは nullptr。
	//      ただし、最初にマッチしたもののみ返す
	//
	System::Xml::XmlNode^ searchXmlNode( System::Xml::XmlElement ^root, System::String ^tag );

	//
	// タグを検索してそのテキストが指定したテキストと一致するか調べる
	//
	// @arg [in] XMLのルートノード
	// @arg [in] タグ名
	// @arg [in] 値
	//
	// @ret 一致するときtrue。一致しないとき、タグが存在しないときはfalse。
	//
	System::Boolean isXmlEqual( System::Xml::XmlElement ^root, System::String ^tag, System::String ^val );

	//
	// タグを検索してそのテキストを返す
	//
	// @arg [in] XMLのルートノード
	// @arg [in] タグのXPath
	//
	// @ret テキストが存在するときそのテキストを返す。存在しないときnullptr。
	//
	System::String^ getXPathText( System::Xml::XmlElement ^root, System::String ^xpath );

	//
	// XMLにタグを追加する
	//
	// @arg [in] XMLドキュメント
	// @arg [in] 親ノード
	// @arg [in] タグ名
	// @arg [in] テキスト
	//
	void appendXmlTag( System::Xml::XmlDocument ^doc, System::Xml::XmlElement ^parent,
					   System::String ^tag, System::String ^text );

	//
	// ビット数をカウントする
	//
	// @arg [in] 値
	//
	// @ret ビット数
	//
	u32 countBits( const u32 val );

	//
	// SDKバージョンの大小判定をする
	//
	// @arg [in] 判定対象のSDKバージョン(SRLに含まれるもの)
	// @arg [in] 判定基準のSDKバージョン(設定ファイルに記述されるもの)
	//
	// @ret 判定対象が基準よりも旧バージョンのとき(認められないとき) true 
	//
	System::Boolean IsOldSDKVersion( u32 target, u32 criterion );

	//
	// SDKバージョンがPR版かどうか調べる
	//
	// @arg [in] 判定対象のSDKバージョン(SRLに含まれるもの)
	//
	// @ret PR版のとき true 
	//
	System::Boolean IsSDKVersionPR( u32 target );

	//
	// SDKバージョンがRC版かどうか調べる
	//
	// @arg [in] 判定対象のSDKバージョン(SRLに含まれるもの)
	//
	// @ret PR版のとき true 
	//
	System::Boolean IsSDKVersionRC( u32 target );

} // end of namespace MasterEditorTWL
