@echo off

rem
rem mastering & comparing sample batch file
rem
rem    Simple usage: SRL�t�@�C�����h���b�O�����%output_path%�Ƀ}�X�^�����O
rem                  ���ʂ��o�͂��A�������ł��������؂��s��
rem
rem    �v�m�F�ϐ�: mastering_tool, comparing_tool, output_path
rem    �⑫: %~d0%~p0�̓o�b�`�t�@�C���̃p�X�ł�
rem    �⑫: %~d1%~p1�͊e���̓t�@�C���̃p�X�ł�
rem

rem �����`�F�b�N
set usage=USAGE: %~nx0 INPUT_FILE...
if ""%1"" == """" (
	echo %usage%
	echo �g�p���@���Ԉ���Ă��܂��B
	echo SRL��������XML���h���b�O�A���h�h���b�v���Ă��������B
	echo.
	goto end
)

rem �p�X�ݒ�
set checker_tool=%~d0%~p0MasterEditorTWLChecker.exe
set org_file=%~d0%~p0ORG.SRL

rem ���[�v�J�n (%0���g���̂͂����܂ŁA%1���g���̂͂�������A%2�ȍ~�͎g��Ȃ�)
:begin

rem �o�̓p�X�ݒ�
set input_file=%~dpnx1
set log_file=%~dp1%\log.txt

rem �v���O�����m�F
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

rem �����{��
echo.
echo %~nx1���`�F�b�N���܂��B
echo.
"%checker_tool%" "%org_file%" "%input_file%" >> "%log_file%"

if not %ERRORLEVEL% == 0 (
	echo "%input_file%" �̃`�F�b�N�Ɏ��s���܂����B
rem 	goto end
) else (
	echo "%input_file%" �͐���ł��B
)

rem ���̓t�@�C�����c���Ă���Ȃ�begin�ɖ߂�
shift
if ""%1"" neq """" goto begin

rem �I�� (�L�[���͑҂�)
:end
echo.
echo ���ׂẴ`�F�b�N���I�����܂����B
pause