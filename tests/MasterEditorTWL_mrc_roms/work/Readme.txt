==============================================================
マスタエディタMRC機能チェック用不正ROM作成スクリプト
==============================================================

必要なもの
===============

スクリプト実行前に、以下を準備してください。

  * IllegalRomMaker.exe : TwlToolsRED/build/tools/IllegalRomMaker
  * FakeRomHeader.user.exe : TwlIPL/build/tests/FakeRomHeader
  * 正常ROM : ../original_rom
    - カードアプリ
    - NANDアプリ
    - システムアプリ

実行
===============

準備したファイルのファイル名にあわせて、
make_mrc_rom のシェル変数を変更してください。

あとは、コマンドラインから make_mrc_rom.sh を実行するだけです。

