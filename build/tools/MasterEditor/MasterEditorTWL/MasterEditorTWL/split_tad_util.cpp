#include "stdafx.h"
#include "twl/types.h"
#include <cstring>
#include <cstdio>
#include "split_tad_util.h"


// ------------------------------------------------------------------------
// �����o�C�g��𔲂��o��
// ------------------------------------------------------------------------

// @ret �����o�����o�C�g��
cli::array<System::Byte>^ subStr( FILE *fp, const int offset, const int size )
{
	if( size <= 0 )
	{
		return nullptr;
	}
	cli::array<System::Byte> ^mbuf = gcnew cli::array<System::Byte>(size);	// ����������̕K�v�Ȃ�
	pin_ptr<unsigned char> buf = &mbuf[0];		// fread �� unmanaged �z��������ɂ���̂ŕϊ�

	(void)fseek( fp, offset, SEEK_SET );
	if( fread( buf, 1, size, fp ) != size )
	{
		return nullptr;
	}
	return mbuf;	// managed �̂ق���Ԃ�
}

// @ret �����o�����o�C�g��
cli::array<System::Byte>^ subStr( cli::array<System::Byte> ^bytes, const int offset, const int size )
{
	cli::array<System::Byte> ^sub = gcnew cli::array<System::Byte>(size);

	cli::array<System::Byte>::Copy( bytes, offset, sub, 0, size );
	return sub;
}

// ------------------------------------------------------------------------
// �t�@�C�����쐬���ăo�C�g����i�[����
// ------------------------------------------------------------------------

// @ret ����������0 ���s�����畉�̒l
int saveFp( FILE *fp, cli::array<System::Byte> ^bytes )
{
	if( bytes == nullptr )
	{
		return 0;	// ��̃t�@�C�������肽���Ƃ������ƂȂ̂Ő���I���Ƃ݂Ȃ�
	}
	pin_ptr<unsigned char> tmp = &bytes[0];		// array�^�͂ӂ��̔z��ł͂Ȃ��̂Ńo�C�g�z��ɕϊ�
	int size = bytes->Length;

	if( fwrite( tmp, 1, size, fp ) != size )
	{
		return -1;
	}
	return 0;
}

// @ret ����������0 ���s�����畉�̒l
int saveFile( System::String ^filename, cli::array<System::Byte> ^bytes )
{
	FILE       *fp = NULL;
	const char *pchFilename = 
		(const char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( filename ).ToPointer();

	// �t�@�C����ROM�w�b�_�����C�g
	if( fopen_s( &fp, pchFilename, "wb" ) != NULL )	// �����t�@�C�����폜���ĐV�K�Ƀ��C�g�E�o�C�i��
	{
		return -1;
	}
	fseek( fp, 0, SEEK_SET );
	int r = saveFp( fp, bytes );	
	fclose( fp );

	return r;
}

// ------------------------------------------------------------------------
// �G���f�B�A�����t�]������(tad�̓r�b�O�G���f�B�A���Ȃ̂ł������ق����֗�)
// ------------------------------------------------------------------------

// @ret �G���f�B�A���t�]��̒l
u32 reverseEndian( const u32 v )
{
	u32 ret = (v<<24) | ((v<<8) & 0x00FF0000) | ((v>>8) & 0x0000FF00) | (v>>24);
	return ret;
}

u16 reverseEndian( const u16 v )
{
	u16 ret = (v<<8) | (v>>8);
	return ret;
}

// ------------------------------------------------------------------------
// ���o�C�g�l���o�C�g��ɕϊ�(���g���G���f�B�A��)
// *** perl��pack���ӎ� ***
// ------------------------------------------------------------------------

// @ret �ϊ���̃o�C�g��
cli::array<System::Byte> ^pack32( u32 v )
{
	cli::array<System::Byte> ^bytes = gcnew cli::array<System::Byte>(4);
	bytes[0] = v & 0xFF;
	bytes[1] = (v >> 8)  & 0xFF;
	bytes[2] = (v >> 16) & 0xFF;
	bytes[3] = (v >> 24) & 0xFF;
	return bytes;
}

cli::array<System::Byte> ^pack16( u16 v )
{
	cli::array<System::Byte> ^bytes = gcnew cli::array<System::Byte>(2);
	bytes[0] = v & 0xFF;
	bytes[1] = v >>8;
	return bytes;
}

// ------------------------------------------------------------------------
// �o�C�g��̕����o�C�g�񂩂瑽�o�C�g�l�Ƃ��ĉ���(���g���G���f�B�A��)
// *** perl��unpack���ӎ� ***
// ------------------------------------------------------------------------

// @arg [in] �o�C�g��
// @arg [in] ���o�C�g�ڂ���𑽃o�C�g�l�Ƃ݂Ȃ���
// @ret ���߂�����̑��o�C�g�l
u32 unpack32( cli::array<System::Byte> ^bytes, const int index )
{
	pin_ptr<unsigned char> tmp = &bytes[0];
	u32 v = (u32)*((u32*)(tmp+index));
	return v;
}

u16 unpack16( cli::array<System::Byte> ^bytes, const int index )
{
	pin_ptr<unsigned char> tmp = &bytes[0];
	u16 v = (u16)*((u16*)(tmp+index));
	return v;
}

// ------------------------------------------------------------------------
// �ۂ߂�
// ------------------------------------------------------------------------

u32 roundUp( const u32 v, const u32 align )
{
	u32 r = ((v + align - 1) / align) * align;
	return r;
}

u16 roundUp( const u16 v, const u16 align )
{
	u16 r = ((v + align - 1) / align) * align;
	return r;
}

// ------------------------------------------------------------------------
// �o�C�g��̒������g�����Ė�����0�Ŗ��߂�
// (0x12345678 => 0x1234567800000000)
// ------------------------------------------------------------------------

// @arg [in] �g���O�̃o�C�g��
// @arg [in] ���o�C�g�g�����邩(�g�����������w�� �Ⴆ��4�o�C�g��5�o�C�g�ɂ���ꍇ�ɂ�1���w��)
// @ret �g����̃o�C�g��(�V���ȃo�C�g�������Ő���)
cli::array<System::Byte> ^resizeBytes( cli::array<System::Byte> ^org, const int difSize )
{
	cli::array<System::Byte> ^r = gcnew cli::array<System::Byte>( org->Length + difSize );
	cli::array<System::Byte>::Copy( org, 0, r, 0, org->Length );
	return r;
}

// ------------------------------------------------------------------------
// �o�C�g���16�i�ŕ\��
// ------------------------------------------------------------------------

void dumpBytes( cli::array<System::Byte> ^bytes )
{
	for each( System::Byte b in bytes )
	{
		printf( "%02x", b );
	}
	printf( "\n" );
}

// end of file
