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
set mastering_tool=%tooldir%mastering.TWL.exe
set mastering_ini=%tooldir%mastering.TWL.ini
set comparing_tool=%tooldir%comparing.TWL.exe
set maketad_tool=%tooldir%maketad.exe
set splittad_tool=%tooldir%split_tad_console.exe
set analyzer_tool=%tooldir%SrlAnalyzer.TWL.exe
set caution_pro=%tooldir%caution_properties.txt
set this_bat=%~dpnx0


rem -------------------------------------------------------
rem ループ開始 (%0を使うのはここまで、%1を使うのはここから、%2以降は使わない)
rem -------------------------------------------------------
:begin

rem プログラム確認
if not exist "%mastering_tool%" (
	echo mastering tool "%mastering_tool%" is not found.
	goto end
)
if not exist "%comparing_tool%" (
	echo comparing tool "%comparing_tool%" is not found.
	goto end
)
if not exist "%maketad_tool%" (
	echo mastering tool "%maketad_tool%" is not found.
	goto end
)
if not exist "%splittad_tool%" (
	echo comparing tool "%splittad_tool%" is not found.
	goto end
)
if not exist "%analyzer_tool%" (
	echo comparing tool "%analyzer_tool%" is not found.
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
set output_srl_short=%~n1.master.srl
set output_pro_short=%~n1.master.prop
set output_tad_short=%~n1.master.tad
set output_cls_dir_short=for_cls\
set output_tmp_srl_short=%~n1.srl

set output_dir=%~d1%~p1%~n1.master.out\
set output_parent_dir=%~d1%~p1\

set output_srl=%output_dir%%output_srl_short%
set output_pro=%output_dir%%output_pro_short%
set output_tad=%output_dir%%output_tad_short%
set output_cls_dir=%output_dir%%output_cls_dir_short%
set output_tmp_srl=%output_dir%%output_tmp_srl_short%
set output_readme=%output_dir%Readme.txt
set output_log=%output_dir%log.txt

rem 出力フォルダの作成
if not exist "%output_dir%" (
	mkdir "%output_dir%"
)

rem バックアップファイル名作成用に時刻を取得しておく
set datetmp=%date: =0%
set datestr=%datetmp:~-10,4%%datetmp:~-5,2%%datetmp:~-2,2%
set timetmp=%time: =0%
set timestr=%timetmp:~0,2%%timetmp:~3,2%%timetmp:~6,2%

echo.
echo ================================================================================================
echo %input_rom_short% のマスタリングを開始します。
echo ================================================================================================
echo.

rem これ以降 %1を使わない


rem -------------------------------------------------------
rem 出力フォルダに ログファイル を作成する
rem -------------------------------------------------------

rem ログファイルはすでに存在していたとき追記していく

echo =================================================>>"%output_log%"
echo [作成日時] %datetmp% %timetmp%>>"%output_log%"
echo [作成元]   %this_bat%>>"%output_log%"
echo =================================================>>"%output_log%"


rem -------------------------------------------------------
rem 同名ファイルが存在するときバックアップ
rem -------------------------------------------------------

rem バックアップ用フォルダを作成(空ならあとで消す)
set backup_dir_short=backup_%datestr%%timestr%\
set backup_dir=%output_dir%%backup_dir_short%
set isempty=
if not exist "%backup_dir%" (
	mkdir "%backup_dir%"
	set isempty="YES"
) else (
	set isempty="NO"
)

echo.
echo *** 出力ファイルが存在する場合、バックアップを作成します。***
echo.

rem ログファイルを除くすべてのファイルをバックアップ
rem (CLS用フォルダはフォルダごとバックアップ)
if exist "%output_srl%" (
	move /y "%output_srl%" "%backup_dir%"
	echo *情報* "%output_srl%" をバックアップしました。
	echo [情報] "%output_srl%" のバックアップを "%backup_dir%" に作成しました。>>"%output_log%"
	set isempty="NO"
)
if exist "%output_pro%" (
	move /y "%output_pro%" "%backup_dir%"
	echo *情報* "%output_pro%" をバックアップしました。
	echo [情報] "%output_pro%" のバックアップを "%backup_dir%" に作成しました。>>"%output_log%"
	set isempty="NO"
)
if exist "%output_tad%" (
	move /y "%output_tad%" "%backup_dir%"
	echo *情報* "%output_tad%" をバックアップしました。
	echo [情報] "%output_tad%" のバックアップを "%backup_dir%" に作成しました。>>"%output_log%"
	set isempty="NO"
)
if exist "%output_cls_dir%" (
	xcopy /i /e /q "%output_cls_dir:~0,-1%" "%backup_dir%%output_cls_dir_short%"
	echo *情報* "%output_cls_dir%" をバックアップしました。
	echo [情報] "%output_cls_dir%" のバックアップを "%backup_dir%" に作成しました。>>"%output_log%"
	set isempty="NO"
)
if exist "%output_tmp_srl%" (
	move /y "%output_tmp_srl%" "%backup_dir%"
	echo *情報* "%output_tmp_srl%" をバックアップしました。
	echo [情報] "%output_tmp_srl%" のバックアップを "%backup_dir%" に作成しました。>>"%output_log%"
	set isempty="NO"
)
if exist "%output_readme%" (
	move /y "%output_readme%" "%backup_dir%"
	echo *情報* "%output_readme%" をバックアップしました。
	echo [情報] "%output_readme%" のバックアップを "%backup_dir%" に作成しました。>>"%output_log%"
	set isempty="NO"
)

rem 空ならバックアップ用フォルダを削除
if %isempty%=="YES" (
	rmdir "%backup_dir%"
	echo バックアップは必要ありませんでした。
) else (
	echo.
	echo バックアップフォルダは、"%backup_dir%" です。
)


rem -------------------------------------------------------
rem 出力フォルダに Readme を作成する
rem -------------------------------------------------------

echo =================================================>>"%output_readme%"
echo マスタリング結果の説明>>"%output_readme%"
echo =================================================>>"%output_readme%"
echo [作成日時] %datetmp% %timetmp%>>"%output_readme%"
echo [作成元]   %this_bat%>>"%output_readme%"
echo.>>"%output_readme%"


rem -------------------------------------------------------
rem TADのときに split_tad する
rem -------------------------------------------------------

echo.
echo *** TAD形式の場合、SRL形式に変換します。(SRL形式の場合にはこの処理はスキップされます。) ***
echo.


if /i "%input_ext%"==".tad" (
	"%splittad_tool%" "%input_rom%" "%output_tmp_srl%"
	set input_srl=%output_tmp_srl%
) else (
	set input_srl=%input_rom%
)
rem echo "%input_srl%"


rem -------------------------------------------------------
rem 入力ファイルのパラメータを取得しておく
rem -------------------------------------------------------

set tmpprog="%analyzer_tool%" "%input_srl%" -p
for /F "delims=" %%a in ('"%tmpprog%"') do set platform=%%a

set tmpprog="%analyzer_tool%" "%input_srl%" -g
for /F "delims=" %%a in ('"%tmpprog%"') do set gamecode=%%a

set tmpprog="%analyzer_tool%" "%input_srl%" -a
for /F "delims=" %%a in ('"%tmpprog%"') do set apptype=%%a

set tmpprog="%analyzer_tool%" "%input_srl%" -s
for /F "delims=" %%a in ('"%tmpprog%"') do set issecure=%%a

set tmpprog="%analyzer_tool%" "%input_srl%" -m
for /F "delims=" %%a in ('"%tmpprog%"') do set media=%%a

set tmpprog="%analyzer_tool%" "%input_srl%" -t
for /F "delims=" %%a in ('"%tmpprog%"') do set tadversion=%%a

rem 入力ファイルの情報を Readme に出力
echo ------------------------------------------>>"%output_readme%"
echo 入力ファイル>>"%output_readme%"
echo ------------------------------------------>>"%output_readme%"
echo プラットフォーム[TWL/NTR] : %platform%>>"%output_readme%"
echo ゲームコード              : %gamecode%>>"%output_readme%"
echo アプリ種別[USER/SYSTEM]   : %apptype%>>"%output_readme%"
echo メディア[CARD/NAND]       : %media%>>"%output_readme%"
echo.>>"%output_readme%"

rem -------------------------------------------------------
rem マスタリング
rem -------------------------------------------------------

rem "NTRJ" かどうかで -t オプションをつけるかどうかを決定
set ntrj_option=
if "%gamecode%"=="NTRJ" (
	set ntrj_option=-t
)

rem セキュアアプリのときにはSDブート用のマスタリングにするかをユーザに決めてもらう
set user_input=
if "%issecure%"=="YES" (
	echo Secureアプリに限り、通常のマスタリングとSDブート用のマスタリングのどちらかを選択できます。
	set /p user_input="SDブート用のマスタリングを実行しますか?(Yes/No)>  "
)
set sdboot_option=
if /i "%user_input%"=="yes" (
	echo SDブート用のマスタリングを実行します。
	set sdboot_option=-s
)
if /i "%user_input%"=="y" (
	echo SDブート用のマスタリングを実行します。
	set sdboot_option=-s
)

rem プラットフォームが NTR/TWL かどうかでマスタリングのオプションを変更
if "%platform%"=="NTR" (
	set mastering_option=
) else (
	set mastering_option=-i "%mastering_ini%" -p "%output_pro%" %ntrj_option% %sdboot_option%
)
rem echo %mastering_option%

echo.
echo *** マスタリングツールを実行します。***
echo.
"%mastering_tool%" "%input_srl%" "%output_srl%" %mastering_option%

if not %ERRORLEVEL% == 0 (
	echo [エラー] マスタリングツール Errno. %ERRORLEVEL%>>"%output_log%"
	echo %input_srl% のマスタリングでエラーが発生しました。
	goto mastering_end
)

rem Readme にファイルの説明を出力
echo.>>"%output_readme%"
echo ------------------------------------------>>"%output_readme%"
echo 出力ファイル>>"%output_readme%"
echo ------------------------------------------>>"%output_readme%"
if /i "%input_ext%"==".tad" (
echo * %output_tmp_srl_short%         @ 入力TADをもとに split_tad によって作成されたマスタリング前SRL >>"%output_readme%"
)
if /i "%user_input%"=="yes" (
echo * %output_srl_short%  @ マスタリング後のSRL [注意: SDブート用の署名でマスタリングを実行しました。]>>"%output_readme%"
) else (
echo * %output_srl_short%  @ マスタリング後のSRL >>"%output_readme%"
)
echo * %output_pro_short% @ マスタリングパラメータ [逆マスタリングで必要になります。] >>"%output_readme%"
echo.>>"%output_readme%"


rem -------------------------------------------------------
rem コンペア
rem -------------------------------------------------------

rem "NTRJ"のときはコンペアに失敗するのでやらない
if not "%gamecode%"=="NTRJ" (
	echo.
	echo *** コンペアツールを実行します。***
	echo.
	"%comparing_tool%" "%input_srl%" "%output_srl%"
) else (
	echo.
	echo *** ゲームコードが"NTRJ"だったのでコンペアツールの実行をスキップしました。***
	echo.
	echo [注意] ゲームコードが"NTRJ"だったのでコンペアツールの実行をスキップしました。>>"%output_log%"
)
if not %ERRORLEVEL% == 0 (
	echo [エラー] コンペアツール Errno. %ERRORLEVEL%>>"%output_log%"
	echo %input_srl% のマスタリング後のチェックでエラーが発生しました。
	goto mastering_end
)

rem *******************************************************
rem カードアプリならここで終了
rem *******************************************************
if "%media%"=="CARD" (
	goto mastering_end
)


rem -------------------------------------------------------
rem maketad
rem -------------------------------------------------------

rem システムアプリかどうかで -s オプションをつけるかどうかを決定
set maketad_option=
if "%apptype%"=="SYSTEM" (
	set maketad_option=-s
)

echo.
echo *** maketad を実行します。***
echo.

rem dllをリンクさせるためにカレントフォルダを変更
chdir /d "%tooldir%"

"%maketad_tool%" -p "%output_srl%" -o "%output_tad%" %maketad_option%

if not %ERRORLEVEL% == 0 (
	echo [エラー] maketad Errno. %ERRORLEVEL%>>"%output_log%"
	echo tad変換でエラーが発生しました。
	goto mastering_end
)


rem -------------------------------------------------------
rem CLS用のフォルダ構成
rem -------------------------------------------------------

echo.
echo *** CLS用のフォルダを構成します ***
echo.

rem CLS用のフォルダを作成
chdir /D "%output_dir%"
if not exist "%output_cls_dir%" (
	echo make directory "%output_cls_dir%"
	mkdir "%output_cls_dir%"
)

rem TADバージョンにあわせて v*** フォルダを作成
set version_dir=%output_cls_dir%v%tadversion%

if not exist "%version_dir%" (
	echo make directory "%version_dir%"
	mkdir "%version_dir%"
)

rem TAD を v*** フォルダに移動する
move /-y "%output_tad%" "%version_dir%"

rem propertiesファイルは %CD% に作成されているので移動する必要がある
move "%tooldir%properties" "%output_cls_dir%\properties-v%tadversion%"

rem properties が存在しない場合、ルート properties にする
set is_root_prop="NO"
if not exist "%output_cls_dir%\properties" (
	copy /y "%output_cls_dir%\properties-v%tadversion%" "%output_cls_dir%\properties"
	set is_root_prop="YES"
)

rem Readme にフォルダ構成を出力
echo * %output_cls_dir_short% @ CLS用のフォルダ構成>>"%output_readme%"
echo   +- v%tadversion%\>>"%output_readme%"
echo      +- %output_tad_short%	@ マスタリング後のSRLをもとに maketad によって作成されたTAD [量産実機にインポートするにはさらにCLS処理が必要です。]>>"%output_readme%"
echo   +- properties-v%tadversion%	@ maketad によって作成された properties ファイル>>"%output_readme%"
if %is_root_prop%=="YES" (
echo   +- properties		@ CLSのパラメータ [properties-v%tadversion% をコピーしたものです。]>>"%output_readme%"
) else (
echo   +- properties		@ CLSのパラメータ [すでに properties が存在していたため上書きしませんでした。更新したい場合、properties-v%tadversion%をリネームしてください。]>>"%output_readme%"
)
echo.>>"%output_readme%"

rem -------------------------------------------------------
rem 入力ファイルが残っているならbeginに戻る
rem -------------------------------------------------------

:mastering_end

rem Readme にバックアップフォルダの説明を追記
if %isempty%=="NO" (
echo * %backup_dir_short% @ バックアップフォルダ [マスタリング前のフォルダの中身が入っています。]>>"%output_readme%"
echo   +- ...>>"%output_readme%"
echo.>>"%output_readme%"
)

rem Readme に properties ファイルについての注意を追記
if "%media%"=="NAND" (
echo.>>"%output_readme%"
echo ------------------------------------------>>"%output_readme%"
echo properties ファイルについての注意>>"%output_readme%"
echo ------------------------------------------>>"%output_readme%"
type "%caution_pro%">>"%output_readme%"
echo.>>"%output_readme%"
)

echo 以上>>"%output_readme%"

echo.
echo ================================================================================================
echo %input_rom_short% のマスタリングが終了しました。
echo.
echo 出力ファイルは %output_dir% に作成されました。
echo 詳細はフォルダ内の Readme.txt をご覧ください。
echo エラーや警告がある場合フォルダ内の log.txt に記述されます。
echo ================================================================================================
echo.
shift
if ""%1"" neq """" goto begin


rem -------------------------------------------------------
rem すべてのファイルのマスタリングが終了 (キー入力待ち)
rem -------------------------------------------------------

:end
echo.
echo すべてのマスタリングが終了しました。
pause
