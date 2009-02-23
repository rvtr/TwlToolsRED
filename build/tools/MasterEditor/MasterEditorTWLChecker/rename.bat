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

rem ユーザ入力
set user_input_prefix=
set /P user_input_prefix="ファイル名の接頭辞を入力してください (例: JP_CERO_00_J) => "
echo 接頭辞を %user_input_prefix% としてファイル名を変換します。

rem ループ開始 (%0を使うのはここまで、%1を使うのはここから、%2以降は使わない)
:begin

rem 出力パス設定
set input_file=%~dpnx1
set output_file=%user_input_prefix%%~x1

ren "%input_file%" "%output_file%"

rem 入力ファイルが残っているならbeginに戻る
shift
if ""%1"" neq """" goto begin

rem 終了 (キー入力待ち)
:end
echo.
echo すべてのチェックが終了しました。
pause
