@echo off

rem
rem mastering & comparing sample batch file
rem
rem    Simple usage: SRLファイルをドラッグすると%output_path%にマスタリング
rem                  結果を出力し、正しくできたか検証を行う
rem
rem    要確認変数: mastering_tool, comparing_tool, output_path
rem    補足: %~d0%~p0はバッチファイルのパスです
rem    補足: %~d1%~p1は各入力ファイルのパスです
rem

rem 引数チェック
set usage=USAGE: %~nx0 INPUT_FILE...
if ""%1"" == """" (
	echo %usage%
	echo 使用方法が間違っています。
	echo SRLもしくはXMLをドラッグアンドドロップしてください。
	echo.
	goto end
)

rem パス設定
set checker_tool=%~d0%~p0MasterEditorTWLChecker.exe
set org_file=%~d0%~p0ORG.SRL

rem ループ開始 (%0を使うのはここまで、%1を使うのはここから、%2以降は使わない)
:begin

rem 出力パス設定
set input_file=%~dpnx1

rem プログラム確認
if not exist "%checker_tool%" (
	echo checker tool "%checker_tool%" is not found.
	goto end
)

if not exist "%org_file%" (
	echo original file "%org_file%" is not found.
	goto end
)

if not exist "%input_file%" (
	echo %input_file% is not found.
	goto end
)

rem 処理本体
echo.
echo %~nx1をチェックします。
echo.
"%checker_tool%" "%org_file%" "%input_file%"

if not %ERRORLEVEL% == 0 (
	echo "%input_file%" のチェックに失敗しました。
) else (
	echo "%input_file%" は正常です。
)

rem 入力ファイルが残っているならbeginに戻る
shift
if ""%1"" neq """" goto begin

rem 終了 (キー入力待ち)
:end
echo.
echo すべてのチェックが終了しました。
pause
