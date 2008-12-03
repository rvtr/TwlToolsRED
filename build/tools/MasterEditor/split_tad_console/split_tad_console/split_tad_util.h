#pragma once

#include "twl/types.h"
#include <cstring>
#include <cstdio>


// ------------------------------------------------------
// APIs
// ------------------------------------------------------

//
// �t�@�C�����畔���o�C�g��𔲂��o��
//
// @ret �����o�����o�C�g��
//
cli::array<System::Byte>^ subStr( FILE *fp, const int offset, const int size );

//
// �o�C�g�񂩂畔���o�C�g��𔲂��o��
//
// @ret �����o�����o�C�g��
//
cli::array<System::Byte>^ subStr( cli::array<System::Byte> ^bytes, const int offset, const int size );

//
// �t�@�C�����쐬���ăo�C�g����i�[����
//
// @ret ����������0 ���s�����畉�̒l
//
int saveFp( FILE *fp, cli::array<System::Byte> ^bytes );
int saveFile( System::String ^filename, cli::array<System::Byte> ^bytes );

//
// �G���f�B�A�����t�]������(tad�̓r�b�O�G���f�B�A���Ȃ̂ł������ق����֗�)
//
// @ret �G���f�B�A���t�]��̒l
//
u32 reverseEndian( const u32 v );
u16 reverseEndian( const u16 v );

//
// ���o�C�g�l���o�C�g��ɕϊ�(���g���G���f�B�A��)  *** perl��pack���ӎ� ***
//
// @ret �ϊ���̃o�C�g��
//
cli::array<System::Byte> ^pack32( u32 v );
cli::array<System::Byte> ^pack16( u16 v );

//
// �o�C�g��̕����o�C�g�񂩂瑽�o�C�g�l�Ƃ��ĉ���(���g���G���f�B�A��)  *** perl��unpack���ӎ� ***
//
// @arg [in] �o�C�g��
// @arg [in] ���o�C�g�ڂ���𑽃o�C�g�l�Ƃ݂Ȃ���
//
// @ret ���߂�����̑��o�C�g�l
//
u32 unpack32( cli::array<System::Byte> ^bytes, const int index );
u16 unpack16( cli::array<System::Byte> ^bytes, const int index );

//
// �ۂ߂�
//
u32 roundUp( const u32 v, const u32 align );
u16 roundUp( const u16 v, const u16 align );

//
// �o�C�g��̒������g�����Ė�����0�Ŗ��߂�
// (0x12345678 => 0x1234567800000000)
//
// @arg [in] �g���O�̃o�C�g��
// @arg [in] ���o�C�g�g�����邩(�g�����������w�� �Ⴆ��4�o�C�g��5�o�C�g�ɂ���ꍇ�ɂ�1���w��)
//
// @ret �g����̃o�C�g��(�V���ȃo�C�g�������Ő���)
//
cli::array<System::Byte> ^resizeBytes( cli::array<System::Byte> ^org, const int difSize );

// �o�C�g���16�i�ŕ\��
void dumpBytes( cli::array<System::Byte> ^bytes );

