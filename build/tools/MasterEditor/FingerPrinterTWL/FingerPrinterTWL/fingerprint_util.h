#pragma once

#include <twl/types.h>
#include <twl/os/common/format_rom.h>

// ----------------------------------------------------------------------
// �t�@�C������
// ----------------------------------------------------------------------

// �t�@�C���R�s�[
void CopyFile( System::String ^srcpath, System::String ^dstpath );

// ROM�w�b�_�̃��[�h
void ReadRomHeader( System::String ^srlpath, ROM_Header *dstrh );

// ROM�w�b�_�̃��C�g
void WriteRomHeader( System::String ^srlpath, ROM_Header *srcrh );

// ----------------------------------------------------------------------
// ����
// ----------------------------------------------------------------------

// ROM�w�b�_�ɏ���������
void SignRomHeader( ROM_Header *rh );

// ----------------------------------------------------------------------
// �ϊ�
// ----------------------------------------------------------------------

// ��������o�C�g�z��ɕϊ�
cli::array<System::Byte>^ TransStringToBytes( System::String ^src, const int len );

// 16�i��������o�C�g�z��ɕϊ�
cli::array<System::Byte>^ TransHexStringToBytes( System::String ^src, const int len );

// �o�C�g�z���16�i������ɕϊ�
System::String^ TransBytesToString( cli::array<System::Byte> ^bytes );

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
