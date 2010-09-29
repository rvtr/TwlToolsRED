KENJCleaner チェックリスト
									2009.10.28

＜KENJCleanerソフト仕様＞
・DSiWare「ほぼ日健康手帳」（InitialCode:KENJ）のセーブデータを乱数フィルした後に、
　ソフト本体を消去する。
・日本リージョンのみ対象でOK。
・検査カードフラグ有効で、ランチャースキップ自動アプリ起動。
・ユーザー操作は一切受け付けない。
・起動したら、自動実行して、以下の結果を表示。
　KENJが見つかった場合：
  ［動作］
　　　KENJのセーブデータクリア
　　　KENJのアプリ消去
　［結果表示］
　　　OK：上画面は緑一色。
          下画面に下記メッセージ表示。
        　application found.
        　Private Save : cleanup succeedded
        　Public  Save : cleanup succeedded
        　Application  : cleanup succeedded
　　　NG：上画面は赤一色。
          下画面に下記メッセージ表示。
        　application found.
        　Private Save : cleanup failed.（※いずれか失敗したもののみ）
        　Public  Save : cleanup failed.（同上）
        　Application  : cleanup failed.（同上）
　KENJが見つからなかった場合：
  ［動作］
　　　何もしない
　［結果表示］
　　　上画面は黄色一色。
      下画面に下記メッセージ表示。
          application not found.
