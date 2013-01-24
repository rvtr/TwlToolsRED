KENJCleaner README
									2012.07.23

＜KENJCleanerソフト仕様＞
・DSiWare「ほぼ日健康手帳」（InitialCode:KENJ）のセーブデータを
　乱数フィルした後に、ソフト本体を消去する。
・DSiWare「洞窟物語」（InitialCode:KCVJ）の有無を表示する。
・対応リージョンは日本のみ。
・検査カードフラグ有効で、ランチャースキップ自動アプリ起動。
・ユーザー操作は一切受け付けない。
・起動したら、自動実行して、以下の結果を表示。

　KENJが見つかった場合：
  ［動作］
　　　KENJのセーブデータクリア
　　　KENJのアプリ消去
　［結果表示］
　　　OK：上画面は上半分が 緑色（＋"KENJ Result"）。
          下画面に下記メッセージ表示。
          Initialcode : KENJ found.
              PrvSave : cleanup succeeded.（※"not existed."の場合もある）
              PubSave : cleanup succeeded.
              Title   : cleanup succeeded.
　　　NG：上画面は上半分が 赤色（＋"KENJ Result"）。
          下画面に下記メッセージ表示。
          Initialcode : KENJ found.
              PrvSave : cleanup failed.（※いずれか失敗したもののみ）
              PubSave : cleanup failed.（※同上）
              Title   : cleanup failed.（※同上）

　KENJが見つからなかった場合：
  ［動作］
　　　何もしない
　［結果表示］
　　　上画面は上半分が 黄色（＋"KENJ Result"）。
      下画面に下記メッセージ表示。
          Initialcode : KENJ not found.

　KCVJが見つかった場合：
  ［動作］
　　　何もしない
　［結果表示］
　　　上画面は下半分が 赤色（＋"KCVJ Result"）。
      下画面に下記メッセージ表示。
          Initialcode : KCVJ found.

　KCVJが見つからなかった場合：
  ［動作］
　　　何もしない
　［結果表示］
　　　上画面は下半分が 黄色（＋"KCVJ Result"）。
      下画面に下記メッセージ表示。
          Initialcode : KENJ not found.


