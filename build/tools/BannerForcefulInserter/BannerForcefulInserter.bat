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
rem ���샂�[�h
rem -------------------------------------------------------

rem light_mode [YES/NO]
rem �ȈՔŏ����ɂ��邩�ǂ���
rem �ȈՔł̂Ƃ��o�̓t�H���_���쐬�����ASRL�̃}�X�^�����O�܂łŏ������I������
set light_mode="NO"


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
set banner_insert_tool=%tooldir%BannerForcefulInserter.exe
set banner_template_file=%tooldir%default.bnr
set analyzer_tool=%tooldir%SrlAnalyzer.TWL.exe
set this_bat=%~dpnx0


rem -------------------------------------------------------
rem ���[�v�J�n (%0���g���̂͂����܂ŁA%1���g���̂͂�������A%2�ȍ~�͎g��Ȃ�)
rem -------------------------------------------------------
:begin

rem �v���O�����m�F
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
set output_banner_srl_short=%~n1.banner.srl

set output_dir=%~d1%~p1%~n1.banner.out\
if %light_mode%=="YES" (
	set output_dir=%~d1%~p1%\
)
set output_parent_dir=%~d1%~p1\

set output_banner_srl=%output_dir%%output_banner_srl_short%
set output_readme=%output_dir%Readme.txt
set output_log=%output_dir%log.txt

rem �o�̓t�H���_�̍쐬
if not exist "%output_dir%" (
	mkdir "%output_dir%"
)

rem ����ȍ~ %1���g��Ȃ�

set input_srl=%input_rom%

rem -------------------------------------------------------
rem �o�̓t�H���_�� ���O�t�@�C�� ���쐬����
rem -------------------------------------------------------

rem ���O�t�@�C���͂��łɑ��݂��Ă����Ƃ��ǋL���Ă���

echo =================================================>>"%output_log%"
echo [�쐬����] %datetmp% %timetmp%>>"%output_log%"
echo [�쐬��]   %this_bat%>>"%output_log%"
echo =================================================>>"%output_log%"


rem -------------------------------------------------------
rem �o�̓t�H���_�� Readme ���쐬����
rem -------------------------------------------------------

echo =================================================>>"%output_readme%"
echo ���ʂ̐���>>"%output_readme%"
echo =================================================>>"%output_readme%"
echo [�쐬����] %datetmp% %timetmp%>>"%output_readme%"
echo [�쐬��]   %this_bat%>>"%output_readme%"
echo.>>"%output_readme%"


rem -------------------------------------------------------
rem ���̓t�@�C���̃p�����[�^���擾���Ă���
rem -------------------------------------------------------

set tmpprog="%analyzer_tool%" "%input_srl%" -p
for /F "delims=" %%a in ('"%tmpprog%"') do set platform=%%a

set tmpprog="%banner_insert_tool%" "%input_srl%" "%output_banner_srl%" "%banner_template_file%" -c
for /F "delims=" %%a in ('"%tmpprog%"') do set isbannerexist=%%a


rem -------------------------------------------------------
rem �o�i�[�𖳗���}������
rem -------------------------------------------------------

if "%platform%"=="NTR" (
	if "%isbannerexist%"=="NOTEXIST" (
		echo.
		echo.
		echo ****************************************************************
		echo *                                                              *
		echo *                                                              *
		echo * �o�i�[�����݂��Ȃ��̂Ńo�i�[�𖳗���}�����܂��B           *
		echo *                                                              *
		echo *                                                              *
		echo ****************************************************************
		echo.
		echo.
		echo [����] �o�i�[�����݂��Ȃ��̂Ńo�i�[�𖳗���}�����܂����B>>"%output_log%"
		echo.>>"%output_readme%"
		echo ------------------------------------------>>"%output_readme%"
		echo ����>>"%output_readme%"
		echo ------------------------------------------>>"%output_readme%"
		echo �o�i�[�����݂��Ȃ��̂Ńo�i�[�𖳗���}�����܂����B>>"%output_readme%"
		echo.>>"%output_readme%"
		"%banner_insert_tool%" "%input_srl%" "%output_banner_srl%" "%banner_template_file%"
		set input_srl=%output_banner_srl%
	) else (
		echo.
		echo.
		echo ****************************************************************
		echo *                                                              *
		echo *                                                              *
		echo * �o�i�[�����݂���̂Ńo�i�[��}������K�v�͂���܂���B       *
		echo *                                                              *
		echo *                                                              *
		echo ****************************************************************
		echo.
		echo.
		echo [����] �o�i�[�����݂���̂Ńo�i�[��}������K�v�͂���܂���B>>"%output_log%"
		echo.>>"%output_readme%"
		echo ------------------------------------------>>"%output_readme%"
		echo ����>>"%output_readme%"
		echo ------------------------------------------>>"%output_readme%"
		echo �o�i�[�����݂���̂Ńo�i�[��}������K�v�͂���܂���B>>"%output_readme%"
		echo.>>"%output_readme%"
	)
)

rem -------------------------------------------------------
rem ���̓t�@�C�����c���Ă���Ȃ�begin�ɖ߂�
rem -------------------------------------------------------

:mastering_end

echo �ȏ�>>"%output_readme%"

echo.
echo ================================================================================================
echo %input_rom_short% �̃o�i�[���������I�����܂����B
echo.
echo �o�̓t�@�C���� %output_dir% �ɍ쐬����܂����B
echo �ڍׂ̓t�H���_���� Readme.txt ���������������B
echo �G���[��x��������ꍇ�t�H���_���� log.txt �ɋL�q����܂��B
echo ================================================================================================
echo.
shift
if ""%1"" neq """" goto begin


rem -------------------------------------------------------
rem �I�� (�L�[���͑҂�)
rem -------------------------------------------------------

:end
echo.
echo ���ׂẴ}�X�^�����O���I�����܂����B
pause
