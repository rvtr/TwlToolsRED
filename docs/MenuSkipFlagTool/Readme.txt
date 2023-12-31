=============================================================================
 TWLメニュースキップフラグ立てツール
=============================================================================

* 目次

  * `TWLメニュースキップフラグ立てツールとは?`_
  * `パッケージ内容`_
  * `使用上の注意`_
  * `対応タイトル`_
  * `動作`_
  * `実行方法`_
  * `プログラムの返り値`_
  * `オプション`_
  * `変更履歴`_


TWLメニュースキップフラグ立てツールとは?
=============================================================================

TWL試遊台タイトルでは、
「健康と安全について」の画面およびDSiメニューをスキップする必要があります。

本ツールは、入力されたROMデータに対して、
上記をスキップするためのフラグを立て、別ファイルに出力します。


パッケージ内容
=============================================================================

* MenuSkipFlagTool.exe : フラグ立てツール
* MenuSkipFlagTool_Checker.exe : MenuSkipFlagTool.exe の検査ツール


対応タイトル
=============================================================================

* TWLタイトル

  * SRL形式のみに対応しています。TAD形式には対応していませんので、ご注意ください。

* NITROタイトル


使用上の注意
=============================================================================

本ツールは、一般開発者が行なうことができないROM設定を可能にするツールです。

「健康と安全について」の画面をスキップするタイトルがユーザに渡るのは好ましくなく、
さらに、DSiメニューをスキップする方法が知られることが思わぬセキュリティホールにつながる可能性もあるため、
本ツールの部外への配布をしないように注意してください。
部外に配布する必要が生じた場合には、開発技術部にご確認ください。



動作
=============================================================================

概要
---------------------

* 入力

  * SRLファイル (TWL-ROM/NITRO-ROM両対応)
  * "1" もしくは "0" (フラグを立てる場合 "1"、立てない場合 "0")

* 出力

  * SRLファイル (入力ファイルとは別のファイル名)

* 処理

  * "1" が入力されたとき、フラグを立てたROMファイルを出力する。
  * "0" が入力されたとき、データを加工しない。(つまり、入力ROMファイルを出力ROMファイルにコピーだけ。)


フローチャート
---------------------

.. image:: tool_flowchart.gif


実行方法
=============================================================================

フラグ立て処理の実行
-----------------------

WindowsXP付属のコマンドプロンプト、もしくはcygwinから実行してください。

  * MenuSkipFlagTool.exe  (入力SRLファイル名)  (出力SRLファイル名)  (1もしくは0)

1を指定した場合、フラグを立てる処理をします。
0を指定した場合には、何もデータを加工せずに、入力SRLファイルを
出力SRLファイルにコピーするだけの処理をします。

プログラムの途中で、エラーがあった場合、処理を中断します。
このとき、作成途中であった、出力SRLファイルは削除されます。

フラグ立て処理の検査
-----------------------

MenuSkipFlagTool.exe の処理が正しいかどうかを同梱の MenuSkipFlagTool_Checker.exe を用いて
チェックすることができます。

MenuSkipFlagTool_Checker.exe は、MenuSkipFlagTool.exe において、
フラグが立てられたかどうか(もしくは、立てられていないかどうか)、および、余分なデータが書き換えられていないかをチェックします。

  * MenuSkipFlagTool_Checker.exe  (フラグ立て処理前のSRLファイル名)  (フラグ立て処理後のSRLファイル名)  (1もしくは0)

(注意)
MenuSkipFlagTool_Checker.exe のコマンドライン引数には、 **MenuSkipFlagTool.exe と同じもの** を指定してください。


プログラムの返り値
=============================================================================

MenuSkipFlagTool.exe
-----------------------

コマンドラインから MenuSkipFlagTool.exe を実行したとき、
プログラムの終了後、以下の返り値が設定されます。

  * 正常終了の場合 : 0 が設定されます。

  * エラーが発生した場合 : -1 が設定されます。

返り値は、Windowsコマンドプロンプトの場合、
%ERRORLEVEL%環境変数に格納されますので、
バッチファイルなどの分岐に利用することができます。

また、cygwinのシェルを使用した場合には、返り値の格納先は、
%ERRORLEVEL%環境変数ではなく、シェル変数 $? となります。
ただし、cygwinシェルの場合、先述の返り値 -1 は 255 となります。

MenuSkipFlagTool_Checker.exe
------------------------------

一方、MenuSkipFlagTool_Checker.exe では、以下の返り値が設定されます。

  * 処理が正常であると判断した場合 : 0 が設定されます。

  * 異常を発見した場合 : -1 が設定されます。


オプション
=============================================================================

MenuSkipFlagTool.exe
-----------------------

実行の際に、以下のオプションを指定することができます。

* -d オプション

通常、1が指定されているとき、メニュースキップフラグを立てる処理が実行されますが、
-d オプションを指定した場合、フラグを下ろす処理となります。
フラグがすでに下りている場合、何もしません。

* -f オプション

通常、出力ファイルがすでに存在する場合、上書き確認を行いますが、-fオプションを指定した場合、
上書き確認をせずに、強制的に上書きします。


* -h オプション

使用方法を表示して終了します。


MenuSkipFlagTool_Checker.exe
------------------------------

* -d オプション

通常、1が指定されているとき、メニュースキップフラグが立てられたかどうかが検査されますが、
-d オプションを指定した場合、フラグが下ろされたかどうかが検査されます。
MenuSkipFlagTool.exe において -d オプションを指定した場合に、
MenuSkipFlatTool_Checker.exe においても -d オプションをご指定ください。

* -h オプション

使用方法を表示して終了します。


変更履歴
=============================================================================

* ver.1.0 (2009/06/22)

  初版

