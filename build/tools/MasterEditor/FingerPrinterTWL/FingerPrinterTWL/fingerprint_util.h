#pragma once

#include <twl/types.h>
#include <twl/os/common/format_rom.h>

// ----------------------------------------------------------------------
// �t�@�C������
// ----------------------------------------------------------------------

// �t�@�C���S�̂̓ǂݍ���
cli::array<System::Byte>^ ReadBin( System::String ^path );

// �t�@�C���S�̂̃��C�g
void WriteBin( System::String ^path, cli::array<System::Byte> ^bin );

// ROM�w�b�_�̃��[�h
void ReadRomHeader( System::String ^srlpath, ROM_Header *dstrh );

// ROM�w�b�_�̃��C�g
void WriteRomHeader( System::String ^srlpath, ROM_Header *srcrh );

// �o�C�i�������ROM�w�b�_�̒��o
void ExtractRomHeader( cli::array<System::Byte> ^bin, ROM_Header *dstrh );

// �o�C�i���ւ�ROM�w�b�_�̏㏑��
void OverwriteRomHeader( cli::array<System::Byte> ^bin, ROM_Header *srcrh );

// �t�@�C���R�s�[
void CopyFile( System::String ^srcpath, System::String ^dstpath );

// ----------------------------------------------------------------------
// ����
// ----------------------------------------------------------------------

// ROM�w�b�_�ɏ���������
void SignRomHeader( ROM_Header *rh );

// ROM�w�b�_�̏����`�F�b�N
void AuthenticateRomHeader( ROM_Header *rh );

// ----------------------------------------------------------------------
// �ϊ�
// ----------------------------------------------------------------------

// ��������o�C�g�z��ɕϊ�
cli::array<System::Byte>^ TransStringToBytes( System::String ^src, const int len );

// 16�i��������o�C�g�z��ɕϊ�
cli::array<System::Byte>^ TransHexStringToBytes( System::String ^src, const int len );

// �o�C�g�z��𕶎���ɕϊ�
System::String^ TransBytesToString( cli::array<System::Byte> ^bytes );

// �o�C�g�z���16�i������ɕϊ�
System::String^ TransBytesToHexString( cli::array<System::Byte> ^bytes );

// ----------------------------------------------------------------------
// GUI
// ----------------------------------------------------------------------

// �t�@�C�����_�C�A���O�Ŏ擾
// @arg [in] �f�t�H���g�̃f�B���N�g��
// @arg [in] �g���q�t�B���^
// @ret �擾�����t�@�C���� �G���[�̂Ƃ� nullptr
System::String^ OpenFileUsingDialog( System::String ^defdir, System::String ^filter );

// �Z�[�u����t�@�C�����_�C�A���O�Ŏ擾
// @arg [in] �f�t�H���g�̃f�B���N�g��
// @arg [in] �g���q�t�B���^
// @arg [in] �t�@�C���̊g���q���s���ȂƂ��ɒǉ�����Ƃ��̐������g���q
// @ret �擾�����t�@�C���� �G���[�̂Ƃ� nullptr
System::String^ SaveFileUsingDialog( System::String ^defdir, System::String ^filter, System::String ^extension );

// ----------------------------------------------------------------------
// �O���v���O�����̎��s
// ----------------------------------------------------------------------

// maketad�̎��s
// @arg [in] maketad�̃p�X
// @arg [in] ����SRL�̃p�X
// @arg [in] �o��SRL�̃p�X
void makeTad( System::String ^maketad_path, System::String ^srlpath, System::String ^tadpath );

// ----------------------------------------------------------------------
// �R���\�[��
// ----------------------------------------------------------------------

// getopt : ��������͂��ăI�v�V����������������1�Ԃ�
// @arg [in/out] ���� (�I�v�V���������������ꍇ�폜����Ă���)
// @arg [in]     �I�v�V���������X�g
// @arg [out]    �I�v�V���������̊i�[�ꏊ (�Ȃ��ꍇ nullptr������)
// @ret          �ŏ��Ɍ��������I�v�V����
char getopt( cli::array<System::String^> ^%args, System::String ^condition, System::String ^% optarg );

// args �������̗v�f���폜
// @arg [in] ����
// @arg [in] �폜����v�f�̃C���f�b�N�X
void shrinkArgs( cli::array<System::String^> ^%args, const int index );
