#pragma once

// ROMデータ(SRL)クラスの関連クラスの宣言と定義

#include <apptype.h>
#include <twl/types.h>

namespace MasterEditorTWL
{
	// -------------------------------------------------------------------
	// Type : ref class
	// Name : RCMiddlewareName
	//
	// Description : 1つのミドルウェアの名前情報
	// 
	// Role : バイナリに埋め込まれているミドルウェア(ライセンス情報)から
	//        その一般名称を返す
	// -------------------------------------------------------------------
	ref class RCMiddlewareName
	{
	private:
		System::String ^publisher;	// バイナリに埋め込まれているはずの提供元
		System::String ^code;		// バイナリに埋め込まれているはずの識別コード
		System::String ^noteJ;		// 一般名称
		System::String ^noteE;
	private:
		RCMiddlewareName(){}			// 封じる
	public:
		RCMiddlewareName( System::String ^pub, System::String ^c, System::String ^nJ, System::String ^nE )
		{
			this->publisher = pub;
			this->code      = c;
			this->noteJ     = nJ;
			this->noteE     = nE;
		}
	public:
		// 提供元と識別コードが一致する場合に一般名称を返す
		// 一致しない場合には nullptr を返す
		System::String^ check(System::String ^pub, System::String ^code, System::Boolean isJapanese)
		{
			if( pub->Equals(this->publisher) && code->StartsWith(this->code) )
			{
				if( isJapanese )
				{
					return System::String::Copy(this->noteJ);
				}
				else
				{
					return System::String::Copy(this->noteE);
				}
			}
			return nullptr;
		}
	}; //ref class RCMiddlewareName

	// -------------------------------------------------------------------
	// Type : ref class
	// Name : RCMiddlewareName
	//
	// Description : ミドルウェアの名前情報のリスト
	// 
	// Role : 設定ファイル中のミドルウェア名称リストから
	//        各ミドルウェアの名称を取得し
	//        名称を検索する
	// -------------------------------------------------------------------
	ref class RCMiddlewareNameList
	{
	private:
		System::Collections::Generic::List<RCMiddlewareName^> ^list;
	private:
		RCMiddlewareNameList(){}
	public:
		RCMiddlewareNameList( System::String^ filepath )
		{
			this->makelist(filepath);
		}
	private:
		// ファイルから各ミドルウェアの名称を取得しリストを作成する
		void makelist( System::String^ filepath )
		{
			this->list = gcnew System::Collections::Generic::List<RCMiddlewareName^>;
			this->list->Clear();

			// xmlファイルの読み込み
			System::Xml::XmlDocument ^doc = gcnew System::Xml::XmlDocument();
			try
			{
				doc->Load( filepath );
			}
			catch( System::Exception ^s )
			{
				(void)s;
				return;
			}
			System::Xml::XmlElement ^root = doc->DocumentElement;

			// ミドルウェアの名称情報のリストをファイルから取得してリストに登録する
			System::Xml::XmlNodeList ^nodelist = root->SelectNodes( "/middleware-name-list/middleware" );	// 同じタグを持つ要素をすべて取得
			System::Collections::IEnumerator^ iter = nodelist->GetEnumerator();
			while( iter->MoveNext() )
			{
				System::Xml::XmlNode ^node = safe_cast<System::Xml::XmlNode^>(iter->Current);
				System::Xml::XmlNode ^n_pub = node->SelectSingleNode( "publisher" );	// 相対パス
				System::Xml::XmlNode ^n_c   = node->SelectSingleNode( "code" );
				System::Xml::XmlNode ^n_nJ  = node->SelectSingleNode( "note-j" );
				System::Xml::XmlNode ^n_nE  = node->SelectSingleNode( "note-e" );

				// XMLノードからデータを抜き出してリストに順次登録していく
				System::String ^pub = "";	// 比較に使うのでnullptrを許さない
				if( n_pub && n_pub->FirstChild && n_pub->FirstChild->Value )
				{
					pub = n_pub->FirstChild->Value;
				}
				System::String ^c = "";
				if( n_c && n_c->FirstChild && n_c->FirstChild->Value )
				{
					c = n_c->FirstChild->Value;
				}
				System::String ^nJ = nullptr;	// 比較に使用しないのでnullptr(登録されていないことを表す)
				if( n_nJ && n_nJ->FirstChild && n_nJ->FirstChild->Value )
				{
					nJ = n_nJ->FirstChild->Value;
				}
				System::String ^nE = nullptr;
				if( n_nE && n_nE->FirstChild && n_nE->FirstChild->Value )
				{
					nE = n_nE->FirstChild->Value;
				}
				this->list->Add( gcnew RCMiddlewareName(pub, c, nJ, nE ) );	// リストに登録
			} //while
		} //void makelist()
	public:
		// リスト内を検索してミドルウェアの名称を返す
		// 登録されていない場合は nullptr を返す
		System::String^ search( System::String ^pub, System::String ^code, System::Boolean isJapanese )
		{
			for each (RCMiddlewareName ^mid in this->list )
			{
				System::String ^note = mid->check(pub, code, isJapanese);
				if( note != nullptr )
				{
					return note;
				}
			}
			return nullptr;
		} //System::String^ search()
	}; //ref class RCMiddlewareNameList

}; // namespace MasterEditorTWL