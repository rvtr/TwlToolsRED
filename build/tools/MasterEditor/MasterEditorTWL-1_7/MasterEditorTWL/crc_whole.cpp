// crc_whole.h �̎���

#include "stdafx.h"
#include <apptype.h>
#include "common.h"
#include "crc_whole.h"
#include <cstring>
#include <cstdio>

#define CRCPOLY     0x1021
#define BUFLEN      0x1000

namespace MasterEditorTWL
{
	u16	crc_table[0x100];

	/*----------------------------------------------------------------------------
	  �֐���
		inittable - initialize table

	  �`��
		static void inittable(unsigned short *table);

	  ���
		CRC�Z�o�����������邽�߂̎Q�ƃe�[�u�����쐬����B
	----------------------------------------------------------------------------*/
	static void inittable(unsigned short *table)
	{
		unsigned short  i, j, r;

		for(i = 0; i < 0x100; i++) {
			r = i << 8;
			for(j = 0; j < 8; j++) {
				if(r & 0x8000U)
					r = (r << 1) ^ CRCPOLY;
				else
					r <<= 1;
			}
			*table++ = r;
		}
	}

	/*============================================================================*
	   CRC�v�Z
	   ���i�Z�p����CRC�e�[�u�����g����CRC�v�Z�֐�

	   �����o�C�g�Ɗ�o�C�g�����ւ��Ă���v�Z����B
	 *============================================================================*/
	static u16 newGetCRC
		( 
		u16  start,    // CRC�����l�i�ݐϒl�j 
		u16 *datap,    // �f�[�^�̐擪���w���|�C���^�A��������̂ƈႢ�Au8
		u32  size      // �o�C�g�P�ʂł̃f�[�^�T�C�Y
		)
	{
		u32 i;
		u16 crc;
		u8* byte;

		crc  = start;
		byte = (u8 *)datap;

		for (i=0; i<size; i+=2)
		{
			u8 byte0, byte1;

			byte0 = *byte;  byte++;
			byte1 = *byte;  byte++;

			crc = (crc << 8) ^ crc_table[(crc >> 8) ^ byte1];
			crc = (crc << 8) ^ crc_table[(crc >> 8) ^ byte0];
		}

		return crc;
	}

	/*---------------------------------------------------------------------------*

	 Name:        getSeg3CRCInFp

	 Description: Segment3(0x3000-0x3fff)��CRC���Z�o

	 Arguments:   [in]  srl�t�@�C���̃t�@�C���|�C���^
				  [out] CRC�i�[��

	 Return:      �����Ȃ�TRUE.

	 *---------------------------------------------------------------------------*/
	BOOL getSeg3CRCInFp( FILE *fp, u16 *pCRC )
	{
		u16   crc;
		u16   data[ 0x1000 / 2 ]; // 64K�o�C�g(512K�r�b�g)

		if( !fp )
		{
			return FALSE;
		}

		// CRC�e�[�u���������i���i�Z�p���̃R�[�h�j
		inittable(crc_table);

		// 0x3000 ���� 0x3fff �܂ł� CRC ���Ƃ�
		fseek( fp, 0x3000, SEEK_SET );
		fread( (void*)data, sizeof(data), 1, fp );
		crc = newGetCRC( 0, data, sizeof(data) );
		*pCRC = crc;

		return TRUE;
	}

	/*---------------------------------------------------------------------------*

	 Name:        getWholeCRCInFp

	 Description: srl�t�@�C���S�̂�CRC���Z�o

	 Arguments:   [in]  srl�t�@�C���̃t�@�C���|�C���^
				  [out] CRC�i�[��

	 Return:      �����Ȃ�TRUE.

	 *---------------------------------------------------------------------------*/
	BOOL getWholeCRCInFp( FILE *fp, u16 *pCRC )
	{
		u32   file_size;
		u16   crc;
		u32   i;
		u16   data[ 0x1000 / 2 ]; // 64K�o�C�g(512K�r�b�g)

		if( !fp )
		{
			return FALSE;
		}

		// �t�@�C���T�C�Y�`�F�b�N
		fseek( fp, 0, SEEK_END );
		file_size = ftell( fp );
		
		// CRC�e�[�u���������i���i�Z�p���̃R�[�h�j
		inittable(crc_table);

		// �t�@�C���S�̂�CRC ���Ƃ�
		fseek( fp, 0, SEEK_SET );
		crc = 0;
		for ( i=0; i<file_size; i+=sizeof(data) )
		{
			fread( (void*)data, sizeof(data), 1, fp );
			crc = newGetCRC( crc, data, sizeof(data) );
		}
		*pCRC = crc;

		return TRUE;
	}

	/*---------------------------------------------------------------------------*

	 Name:        getWholeCRCInFile

	 Description: srl�t�@�C���S�̂�CRC���Z�o

	 Arguments:   [in]  srl�t�@�C����(�Ăяo���O�ɕ��Ă�������)
				  [out] CRC�i�[��

	 Return:      None.

	 *---------------------------------------------------------------------------*/
	BOOL getWholeCRCInFile( System::String ^filename, u16 *pCRC )
	{
		FILE* fp;
		const char *pchFilename = 
			(const char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( filename ).ToPointer();

		fopen_s( &fp, pchFilename, "rb" );
		if ( fp == NULL )
		{
			return FALSE;
		}

		getWholeCRCInFp( fp, pCRC );

		fclose( fp );
		return TRUE;
	}

} // end of namespace MasterEditorTWL
