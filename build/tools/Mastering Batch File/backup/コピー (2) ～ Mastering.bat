@echo off

rem
rem mastering & comparing sample batch file
rem
rem    Simple usage: SRL�t�@�C�����h���b�O�����
rem                  %output_dir%�Ƀ}�X�^�����O���ʂ��o�͂��A�������ł��������؂��s��
rem                  TAD�̏ꍇ�ɂ�CLS�p�̃t�H���_���\������
rem
rem    �⑫: %~d0%~p0�̓o�b�`�t�@�C���̃p�X�ł�
rem    �⑫: %~d1%~p1�͊e���̓t�@�C���̃p�X�ł�
rem

rem -------------------------------------------------------
rem �����`�F�b�N
rem -------------------------------------------------------
set usage=USAGE: %~nx0 INPUT_FILE...
if ""%1"" == """" (
	echo %usage%
	echo �g�p���@���Ԉ���Ă��܂��B
	echo �}�X�^�����O�O��SRL/TAD���h���b�O�A���h�h���b�v���Ă��������B
	echo.
	goto end
)


rem -------------------------------------------------------
rem �v���O�����̃p�X�ݒ�
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
rem ���[�v�J�n (%0���g���̂͂����܂ŁA%1���g���̂͂�������A%2�ȍ~�͎g��Ȃ�)
rem -------------------------------------------------------
:begin

rem �v���O�����m�F
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
rem ���o�̓t�@�C���̃p�X�ݒ�
rem -------------------------------------------------------

rem ���̓p�X
set input_rom=%~dpnx1
set input_rom_short=%~nx1
set input_ext=%~x1

if not exist "%input_rom%" (
	echo %input_rom% is not found.
	goto end
)

rem �o�̓p�X
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

rem �o�̓t�H���_�̍쐬
if not exist "%output_dir%" (
	mkdir "%output_dir%"
)

rem �o�b�N�A�b�v�t�@�C�����쐬�p�Ɏ������擾���Ă���
set datetmp=%date: =0%
set datestr=%datetmp:~-10,4%%datetmp:~-5,2%%datetmp:~-2,2%
set timetmp=%time: =0%
set timestr=%timetmp:~0,2%%timetmp:~3,2%%timetmp:~6,2%

echo.
echo ================================================================================================
echo %input_rom_short% �̃}�X�^�����O���J�n���܂��B
echo ================================================================================================
echo.

rem ����ȍ~ %1���g��Ȃ�


rem -------------------------------------------------------
rem �o�̓t�H���_�� ���O�t�@�C�� ���쐬����
rem -------------------------------------------------------

rem ���O�t�@�C���͂��łɑ��݂��Ă����Ƃ��ǋL���Ă���

echo =================================================>>"%output_log%"
echo [�쐬����] %datetmp% %timetmp%>>"%output_log%"
echo [�쐬��]   %this_bat%>>"%output_log%"
echo =================================================>>"%output_log%"


rem -------------------------------------------------------
rem �����t�@�C�������݂���Ƃ��o�b�N�A�b�v
rem -------------------------------------------------------

rem �o�b�N�A�b�v�p�t�H���_���쐬(��Ȃ炠�Ƃŏ���)
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
echo *** �o�̓t�@�C�������݂���ꍇ�A�o�b�N�A�b�v���쐬���܂��B***
echo.

rem ���O�t�@�C�����������ׂẴt�@�C�����o�b�N�A�b�v
rem (CLS�p�t�H���_�̓t�H���_���ƃo�b�N�A�b�v)
if exist "%output_srl%" (
	move /y "%output_srl%" "%backup_dir%"
	echo *���* "%output_srl%" ���o�b�N�A�b�v���܂����B
	echo [���] "%output_srl%" �̃o�b�N�A�b�v�� "%backup_dir%" �ɍ쐬���܂����B>>"%output_log%"
	set isempty="NO"
)
if exist "%output_pro%" (
	move /y "%output_pro%" "%backup_dir%"
	echo *���* "%output_pro%" ���o�b�N�A�b�v���܂����B
	echo [���] "%output_pro%" �̃o�b�N�A�b�v�� "%backup_dir%" �ɍ쐬���܂����B>>"%output_log%"
	set isempty="NO"
)
if exist "%output_tad%" (
	move /y "%output_tad%" "%backup_dir%"
	echo *���* "%output_tad%" ���o�b�N�A�b�v���܂����B
	echo [���] "%output_tad%" �̃o�b�N�A�b�v�� "%backup_dir%" �ɍ쐬���܂����B>>"%output_log%"
	set isempty="NO"
)
if exist "%output_cls_dir%" (
	xcopy /i /e /q "%output_cls_dir:~0,-1%" "%backup_dir%%output_cls_dir_short%"
	echo *���* "%output_cls_dir%" ���o�b�N�A�b�v���܂����B
	echo [���] "%output_cls_dir%" �̃o�b�N�A�b�v�� "%backup_dir%" �ɍ쐬���܂����B>>"%output_log%"
	set isempty="NO"
)
if exist "%output_tmp_srl%" (
	move /y "%output_tmp_srl%" "%backup_dir%"
	echo *���* "%output_tmp_srl%" ���o�b�N�A�b�v���܂����B
	echo [���] "%output_tmp_srl%" �̃o�b�N�A�b�v�� "%backup_dir%" �ɍ쐬���܂����B>>"%output_log%"
	set isempty="NO"
)
if exist "%output_readme%" (
	move /y "%output_readme%" "%backup_dir%"
	echo *���* "%output_readme%" ���o�b�N�A�b�v���܂����B
	echo [���] "%output_readme%" �̃o�b�N�A�b�v�� "%backup_dir%" �ɍ쐬���܂����B>>"%output_log%"
	set isempty="NO"
)

rem ��Ȃ�o�b�N�A�b�v�p�t�H���_���폜
if %isempty%=="YES" (
	rmdir "%backup_dir%"
	echo �o�b�N�A�b�v�͕K�v����܂���ł����B
) else (
	echo.
	echo �o�b�N�A�b�v�t�H���_�́A"%backup_dir%" �ł��B
)


rem -------------------------------------------------------
rem �o�̓t�H���_�� Readme ���쐬����
rem -------------------------------------------------------

echo =================================================>>"%output_readme%"
echo �}�X�^�����O���ʂ̐���>>"%output_readme%"
echo =================================================>>"%output_readme%"
echo [�쐬����] %datetmp% %timetmp%>>"%output_readme%"
echo [�쐬��]   %this_bat%>>"%output_readme%"
echo.>>"%output_readme%"


rem -------------------------------------------------------
rem TAD�̂Ƃ��� split_tad ����
rem -------------------------------------------------------

echo.
echo *** TAD�`���̏ꍇ�ASRL�`���ɕϊ����܂��B(SRL�`���̏ꍇ�ɂ͂��̏����̓X�L�b�v����܂��B) ***
echo.


if /i "%input_ext%"==".tad" (
	"%splittad_tool%" "%input_rom%" "%output_tmp_srl%"
	set input_srl=%output_tmp_srl%
) else (
	set input_srl=%input_rom%
)
rem echo "%input_srl%"


rem -------------------------------------------------------
rem ���̓t�@�C���̃p�����[�^���擾���Ă���
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

rem ���̓t�@�C���̏��� Readme �ɏo��
echo ------------------------------------------>>"%output_readme%"
echo ���̓t�@�C��>>"%output_readme%"
echo ------------------------------------------>>"%output_readme%"
echo �v���b�g�t�H�[��[TWL/NTR] : %platform%>>"%output_readme%"
echo �Q�[���R�[�h              : %gamecode%>>"%output_readme%"
echo �A�v�����[USER/SYSTEM]   : %apptype%>>"%output_readme%"
echo ���f�B�A[CARD/NAND]       : %media%>>"%output_readme%"
echo.>>"%output_readme%"

rem -------------------------------------------------------
rem �}�X�^�����O
rem -------------------------------------------------------

rem "NTRJ" ���ǂ����� -t �I�v�V���������邩�ǂ���������
set ntrj_option=
if "%gamecode%"=="NTRJ" (
	set ntrj_option=-t
)

rem �Z�L���A�A�v���̂Ƃ��ɂ�SD�u�[�g�p�̃}�X�^�����O�ɂ��邩�����[�U�Ɍ��߂Ă��炤
set user_input=
if "%issecure%"=="YES" (
	echo Secure�A�v���Ɍ���A�ʏ�̃}�X�^�����O��SD�u�[�g�p�̃}�X�^�����O�̂ǂ��炩��I���ł��܂��B
	set /p user_input="SD�u�[�g�p�̃}�X�^�����O�����s���܂���?(Yes/No)>  "
)
set sdboot_option=
if /i "%user_input%"=="yes" (
	echo SD�u�[�g�p�̃}�X�^�����O�����s���܂��B
	set sdboot_option=-s
)
if /i "%user_input%"=="y" (
	echo SD�u�[�g�p�̃}�X�^�����O�����s���܂��B
	set sdboot_option=-s
)

rem �v���b�g�t�H�[���� NTR/TWL ���ǂ����Ń}�X�^�����O�̃I�v�V������ύX
if "%platform%"=="NTR" (
	set mastering_option=
) else (
	set mastering_option=-i "%mastering_ini%" -p "%output_pro%" %ntrj_option% %sdboot_option%
)
rem echo %mastering_option%

echo.
echo *** �}�X�^�����O�c�[�������s���܂��B***
echo.
"%mastering_tool%" "%input_srl%" "%output_srl%" %mastering_option%

if not %ERRORLEVEL% == 0 (
	echo [�G���[] �}�X�^�����O�c�[�� Errno. %ERRORLEVEL%>>"%output_log%"
	echo %input_srl% �̃}�X�^�����O�ŃG���[���������܂����B
	goto mastering_end
)

rem Readme �Ƀt�@�C���̐������o��
echo.>>"%output_readme%"
echo ------------------------------------------>>"%output_readme%"
echo �o�̓t�@�C��>>"%output_readme%"
echo ------------------------------------------>>"%output_readme%"
if /i "%input_ext%"==".tad" (
echo * %output_tmp_srl_short%         @ ����TAD�����Ƃ� split_tad �ɂ���č쐬���ꂽ�}�X�^�����O�OSRL >>"%output_readme%"
)
if /i "%user_input%"=="yes" (
echo * %output_srl_short%  @ �}�X�^�����O���SRL [����: SD�u�[�g�p�̏����Ń}�X�^�����O�����s���܂����B]>>"%output_readme%"
) else (
echo * %output_srl_short%  @ �}�X�^�����O���SRL >>"%output_readme%"
)
echo * %output_pro_short% @ �}�X�^�����O�p�����[�^ [�t�}�X�^�����O�ŕK�v�ɂȂ�܂��B] >>"%output_readme%"
echo.>>"%output_readme%"


rem -------------------------------------------------------
rem �R���y�A
rem -------------------------------------------------------

rem "NTRJ"�̂Ƃ��̓R���y�A�Ɏ��s����̂ł��Ȃ�
if not "%gamecode%"=="NTRJ" (
	echo.
	echo *** �R���y�A�c�[�������s���܂��B***
	echo.
	"%comparing_tool%" "%input_srl%" "%output_srl%"
) else (
	echo.
	echo *** �Q�[���R�[�h��"NTRJ"�������̂ŃR���y�A�c�[���̎��s���X�L�b�v���܂����B***
	echo.
	echo [����] �Q�[���R�[�h��"NTRJ"�������̂ŃR���y�A�c�[���̎��s���X�L�b�v���܂����B>>"%output_log%"
)
if not %ERRORLEVEL% == 0 (
	echo [�G���[] �R���y�A�c�[�� Errno. %ERRORLEVEL%>>"%output_log%"
	echo %input_srl% �̃}�X�^�����O��̃`�F�b�N�ŃG���[���������܂����B
	goto mastering_end
)

rem *******************************************************
rem �J�[�h�A�v���Ȃ炱���ŏI��
rem *******************************************************
if "%media%"=="CARD" (
	goto mastering_end
)


rem -------------------------------------------------------
rem maketad
rem -------------------------------------------------------

rem �V�X�e���A�v�����ǂ����� -s �I�v�V���������邩�ǂ���������
set maketad_option=
if "%apptype%"=="SYSTEM" (
	set maketad_option=-s
)

echo.
echo *** maketad �����s���܂��B***
echo.

rem dll�������N�����邽�߂ɃJ�����g�t�H���_��ύX
chdir /d "%tooldir%"

"%maketad_tool%" -p "%output_srl%" -o "%output_tad%" %maketad_option%

if not %ERRORLEVEL% == 0 (
	echo [�G���[] maketad Errno. %ERRORLEVEL%>>"%output_log%"
	echo tad�ϊ��ŃG���[���������܂����B
	goto mastering_end
)


rem -------------------------------------------------------
rem CLS�p�̃t�H���_�\��
rem -------------------------------------------------------

echo.
echo *** CLS�p�̃t�H���_���\�����܂� ***
echo.

rem CLS�p�̃t�H���_���쐬
chdir /D "%output_dir%"
if not exist "%output_cls_dir%" (
	echo make directory "%output_cls_dir%"
	mkdir "%output_cls_dir%"
)

rem TAD�o�[�W�����ɂ��킹�� v*** �t�H���_���쐬
set version_dir=%output_cls_dir%v%tadversion%

if not exist "%version_dir%" (
	echo make directory "%version_dir%"
	mkdir "%version_dir%"
)

rem TAD �� v*** �t�H���_�Ɉړ�����
move /-y "%output_tad%" "%version_dir%"

rem properties�t�@�C���� %CD% �ɍ쐬����Ă���̂ňړ�����K�v������
move "%tooldir%properties" "%output_cls_dir%\properties-v%tadversion%"

rem properties �����݂��Ȃ��ꍇ�A���[�g properties �ɂ���
set is_root_prop="NO"
if not exist "%output_cls_dir%\properties" (
	copy /y "%output_cls_dir%\properties-v%tadversion%" "%output_cls_dir%\properties"
	set is_root_prop="YES"
)

rem Readme �Ƀt�H���_�\�����o��
echo * %output_cls_dir_short% @ CLS�p�̃t�H���_�\��>>"%output_readme%"
echo   +- v%tadversion%\>>"%output_readme%"
echo      +- %output_tad_short%	@ �}�X�^�����O���SRL�����Ƃ� maketad �ɂ���č쐬���ꂽTAD [�ʎY���@�ɃC���|�[�g����ɂ͂����CLS�������K�v�ł��B]>>"%output_readme%"
echo   +- properties-v%tadversion%	@ maketad �ɂ���č쐬���ꂽ properties �t�@�C��>>"%output_readme%"
if %is_root_prop%=="YES" (
echo   +- properties		@ CLS�̃p�����[�^ [properties-v%tadversion% ���R�s�[�������̂ł��B]>>"%output_readme%"
) else (
echo   +- properties		@ CLS�̃p�����[�^ [���ł� properties �����݂��Ă������ߏ㏑�����܂���ł����B�X�V�������ꍇ�Aproperties-v%tadversion%�����l�[�����Ă��������B]>>"%output_readme%"
)
echo.>>"%output_readme%"

rem -------------------------------------------------------
rem ���̓t�@�C�����c���Ă���Ȃ�begin�ɖ߂�
rem -------------------------------------------------------

:mastering_end

rem Readme �Ƀo�b�N�A�b�v�t�H���_�̐�����ǋL
if %isempty%=="NO" (
echo * %backup_dir_short% @ �o�b�N�A�b�v�t�H���_ [�}�X�^�����O�O�̃t�H���_�̒��g�������Ă��܂��B]>>"%output_readme%"
echo   +- ...>>"%output_readme%"
echo.>>"%output_readme%"
)

rem Readme �� properties �t�@�C���ɂ��Ă̒��ӂ�ǋL
if "%media%"=="NAND" (
echo.>>"%output_readme%"
echo ------------------------------------------>>"%output_readme%"
echo properties �t�@�C���ɂ��Ă̒���>>"%output_readme%"
echo ------------------------------------------>>"%output_readme%"
type "%caution_pro%">>"%output_readme%"
echo.>>"%output_readme%"
)

echo �ȏ�>>"%output_readme%"

echo.
echo ================================================================================================
echo %input_rom_short% �̃}�X�^�����O���I�����܂����B
echo.
echo �o�̓t�@�C���� %output_dir% �ɍ쐬����܂����B
echo �ڍׂ̓t�H���_���� Readme.txt ���������������B
echo �G���[��x��������ꍇ�t�H���_���� log.txt �ɋL�q����܂��B
echo ================================================================================================
echo.
shift
if ""%1"" neq """" goto begin


rem -------------------------------------------------------
rem ���ׂẴt�@�C���̃}�X�^�����O���I�� (�L�[���͑҂�)
rem -------------------------------------------------------

:end
echo.
echo ���ׂẴ}�X�^�����O���I�����܂����B
pause
