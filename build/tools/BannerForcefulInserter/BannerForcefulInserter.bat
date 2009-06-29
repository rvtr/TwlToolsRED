@echo off

rem
rem mastering & comparing sample batch file
rem
rem    Simple usage: SRLファイルをドラッグすると
rem                  %output_dir%にマスタリング結果を出力し、正しくできたか検証を行う
rem                  TADの場合にはCLS用のフォルダを構成する
rem
rem    補足: %~d0%~p0はバッチファイルのパスです
rem    補足: %~d1%~p1は各入力ファイルのパスです
rem

rem -------------------------------------------------------
rem 動作モード
rem -------------------------------------------------------

rem light_mode [YES/NO]
rem 簡易版処理にするかどうか
rem 簡易版のとき出力フォルダを作成せず、SRLのマスタリングまでで処理を終了する
set light_mode="NO"


rem -------------------------------------------------------
rem 引数チェック
rem -------------------------------------------------------
set usage=USAGE: %~nx0 INPUT_FILE...
if ""%1"" == """" (
	echo %usage%
	echo 使用方法が間違っています。
	echo マスタリング前のSRL/TADをドラッグアンドドロップしてください。
	echo.
	goto end
)


rem -------------------------------------------------------
rem プログラムのパス設定
rem -------------------------------------------------------

set tooldir=%~d0%~p0\bin\
set banner_insert_tool=%tooldir%BannerForcefulInserter.exe
set banner_template_file=%tooldir%default.bnr
set analyzer_tool=%tooldir%SrlAnalyzer.TWL.exe
set this_bat=%~dpnx0


rem -------------------------------------------------------
rem ループ開始 (%0を使うのはここまで、%1を使うのはここから、%2以降は使わない)
rem -------------------------------------------------------
:begin

rem プログラム確認
if not exist "%banner_insert_tool%" (
	echo mastering tool "%banner_insert_tool%" is not found.
	goto end
)
if not exist "%banner_template_file%" (
	echo comparing tool "%banner_template_file%" is not found.
	goto end
)
if not exist "%analyzer_tool%" (
	echo srl analyzer tool "%analyzer_tool%" is not found.
	goto end
)

rem -------------------------------------------------------
rem 入出力ファイルのパス設定
rem -------------------------------------------------------

rem 入力パス
set input_rom=%~dpnx1
set input_rom_short=%~nx1
set input_ext=%~x1

if not exist "%input_rom%" (
	echo %input_rom% is not found.
	goto end
)

rem 出力パス
set output_banner_srl_short=%~n1.banner.srl

set output_dir=%~d1%~p1%~n1.banner.out\
if %light_mode%=="YES" (
	set output_dir=%~d1%~p1%\
)
set output_parent_dir=%~d1%~p1\

set output_banner_srl=%output_dir%%output_banner_srl_short%
set output_readme=%output_dir%Readme.txt
set output_log=%output_dir%log.txt

rem 出力フォルダの作成
if not exist "%output_dir%" (
	mkdir "%output_dir%"
)

rem これ以降 %1を使わない

set input_srl=%input_rom%

rem -------------------------------------------------------
rem 出力フォルダに ログファイル を作成する
rem -------------------------------------------------------

rem ログファイルはすでに存在していたとき追記していく

echo =================================================>>"%output_log%"
echo [作成日時] %datetmp% %timetmp%>>"%output_log%"
echo [作成元]   %this_bat%>>"%output_log%"
echo =================================================>>"%output_log%"


rem -------------------------------------------------------
rem 出力フォルダに Readme を作成する
rem -------------------------------------------------------

echo =================================================>>"%output_readme%"
echo 結果の説明>>"%output_readme%"
echo =================================================>>"%output_readme%"
echo [作成日時] %datetmp% %timetmp%>>"%output_readme%"
echo [作成元]   %this_bat%>>"%output_readme%"
echo.>>"%output_readme%"


rem -------------------------------------------------------
rem 入力ファイルのパラメータを取得しておく
rem -------------------------------------------------------

set tmpprog="%analyzer_tool%" "%input_srl%" -p
for /F "delims=" %%a in ('"%tmpprog%"') do set platform=%%a

set tmpprog="%banner_insert_tool%" "%input_srl%" "%output_banner_srl%" "%banner_template_file%" -c
for /F "delims=" %%a in ('"%tmpprog%"') do set isbannerexist=%%a


rem -------------------------------------------------------
rem バナーを無理矢理挿入する
rem -------------------------------------------------------

if "%platform%"=="NTR" (
	if "%isbannerexist%"=="NOTEXIST" (
		echo.
		echo.
		echo ****************************************************************
		echo *                                                              *
		echo *                                                              *
		echo * バナーが存在しないのでバナーを無理矢理挿入します。           *
		echo *                                                              *
		echo *                                                              *
		echo ****************************************************************
		echo.
		echo.
		echo [注意] バナーが存在しないのでバナーを無理矢理挿入しました。>>"%output_log%"
		echo.>>"%output_readme%"
		echo ------------------------------------------>>"%output_readme%"
		echo 注意>>"%output_readme%"
		echo ------------------------------------------>>"%output_readme%"
		echo バナーが存在しないのでバナーを無理矢理挿入しました。>>"%output_readme%"
		echo.>>"%output_readme%"
		"%banner_insert_tool%" "%input_srl%" "%output_banner_srl%" "%banner_template_file%"
		set input_srl=%output_banner_srl%
	) else (
		echo.
		echo.
		echo ****************************************************************
		echo *                                                              *
		echo *                                                              *
		echo * バナーが存在するのでバナーを挿入する必要はありません。       *
		echo *                                                              *
		echo *                                                              *
		echo ****************************************************************
		echo.
		echo.
		echo [注意] バナーが存在するのでバナーを挿入する必要はありません。>>"%output_log%"
		echo.>>"%output_readme%"
		echo ------------------------------------------>>"%output_readme%"
		echo 注意>>"%output_readme%"
		echo ------------------------------------------>>"%output_readme%"
		echo バナーが存在するのでバナーを挿入する必要はありません。>>"%output_readme%"
		echo.>>"%output_readme%"
	)
)

rem -------------------------------------------------------
rem 入力ファイルが残っているならbeginに戻る
rem -------------------------------------------------------

:mastering_end

echo 以上>>"%output_readme%"

echo.
echo ================================================================================================
echo %input_rom_short% のバナーつけかえが終了しました。
echo.
echo 出力ファイルは %output_dir% に作成されました。
echo 詳細はフォルダ内の Readme.txt をご覧ください。
echo エラーや警告がある場合フォルダ内の log.txt に記述されます。
echo ================================================================================================
echo.
shift
if ""%1"" neq """" goto begin


rem -------------------------------------------------------
rem 終了 (キー入力待ち)
rem -------------------------------------------------------

:end
echo.
echo すべてのマスタリングが終了しました。
pause
