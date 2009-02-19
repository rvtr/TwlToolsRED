#include "stdafx.h"
#include "check.h"
#include <common.h>
#include <utility.h>
#include <twl/types.h>
#include <twl/os/common/format_rom.h>
#include <twl/os/common/ownerInfoEx.h>
#include <cstring>
#include <cstdio>

//
// �t�@�C�����̕�����ƃ��[�W����/���[�e�B���O�̒l�̑Ή��Â�
//

u32 getRegionBitmap( System::String ^region )
{
	u32  bitmap = 0;
	if( region == "JP" )
	{
		bitmap = METWL_MASK_REGION_JAPAN;
	}
	else if( region == "US" )
	{
		bitmap = METWL_MASK_REGION_AMERICA;
	}
	else if( region == "EU" )
	{
		bitmap = METWL_MASK_REGION_EUROPE;
	}
	else if( region == "AU" )
	{
		bitmap = METWL_MASK_REGION_AUSTRALIA;
	}
	else if( region == "EUAU" )
	{
		bitmap = (METWL_MASK_REGION_EUROPE | METWL_MASK_REGION_AUSTRALIA);
	}
	else if( region == "USAU" )
	{
		bitmap = (METWL_MASK_REGION_AMERICA | METWL_MASK_REGION_AUSTRALIA);
	}
	else if( region == "USEUAU" )
	{
		bitmap = (METWL_MASK_REGION_AMERICA | METWL_MASK_REGION_EUROPE | METWL_MASK_REGION_AUSTRALIA);
	}
	else if( region == "CN" )
	{
		bitmap = (METWL_MASK_REGION_CHINA);
	}
	else if( region == "KR" )
	{
		bitmap = (METWL_MASK_REGION_KOREA);
	}
	else if( region == "ALL" )
	{
		bitmap = METWL_MASK_REGION_ALL;
	}
	return bitmap;
}

int getOgnNumber( System::String ^ogn )
{
	int num = -1;
	if( ogn == "CERO" )
	{
		num = OS_TWL_PCTL_OGN_CERO;
	}
	if( ogn == "ESRB" )
	{
		num = OS_TWL_PCTL_OGN_ESRB;
	}
	if( ogn == "USK" )
	{
		num = OS_TWL_PCTL_OGN_USK;
	}
	if( ogn == "PEGI_GEN" )
	{
		num = OS_TWL_PCTL_OGN_PEGI_GEN;
	}
	if( ogn == "PEGI_PRT" )
	{
		num = OS_TWL_PCTL_OGN_PEGI_PRT;
	}
	if( ogn == "PEGI_BBFC" )
	{
		num = OS_TWL_PCTL_OGN_PEGI_BBFC;
	}
	if( ogn == "OFLC" )
	{
		num = OS_TWL_PCTL_OGN_OFLC;
	}
	if( ogn == "GRB" )
	{
		num = OS_TWL_PCTL_OGN_GRB;
	}
	return num;
}

u8 getRatingValue( System::String ^rating )
{
	u8 val = 0;
	if( rating == "RP" )
	{
		val = OS_TWL_PCTL_OGNINFO_ENABLE_MASK | OS_TWL_PCTL_OGNINFO_ALWAYS_MASK;
	}
	else
	{
		val = OS_TWL_PCTL_OGNINFO_ENABLE_MASK | (System::Byte::Parse(rating));
	}
	return val;
}

//
// �o��SRL�̃`�F�b�N
//
System::Void checkRom( FilenameItem ^fItem, System::String ^srlpath )
{
	const char *chpath = 
		(const char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( srlpath ).ToPointer();

	// ROM�w�b�_�̓ǂݍ���
	ROM_Header rh;
	FILE       *fp = NULL;
	if( fopen_s( &fp, chpath, "rb" ) != NULL )
	{
		throw (gcnew System::Exception("Fail to Open SRL File."));
		return;
	}
	// 1�o�C�g��sizeof(~)�������[�h (�t���ƕԂ�l��sizeof(~)�ɂȂ�Ȃ��̂Œ���)
	(void)fseek( fp, 0, SEEK_SET );		// ROM�w�b�_��srl�̐擪����
	if( fread( (void*)&rh, 1, sizeof(ROM_Header), fp ) != sizeof(ROM_Header) )
	{
		throw (gcnew System::Exception("Fail to Read ROM Header."));
		fclose(fp);
		return;
	}
	fclose(fp);

	// ���[�W�����̃`�F�b�N
	u32 region = getRegionBitmap( fItem->region );
	if( rh.s.card_region_bitmap != region )
	{
		throw (gcnew System::Exception("Illegal Region in ROM Header."));
		return;
	}

	// ���[�W�����Ɋ܂܂��c�̂̃��[�e�B���Oenable�t���O�������Ă��邩�`�F�b�N
	System::Collections::Generic::List<int> ^ognlist = MasterEditorTWL::getOgnListInRegion( region );
	for each ( int ogn in ognlist )
	{
		if( (rh.s.parental_control_rating_info[ogn] & OS_TWL_PCTL_OGNINFO_ENABLE_MASK) == 0 )
		{
			throw (gcnew System::Exception("Rating Ogn " + ogn.ToString() + " is not Enabled."));
			return;
		}
	}

	// �ݒ肵�����[�e�B���O�����������ǂ������`�F�b�N
	int ogn = getOgnNumber( fItem->ogn );			// �t�@�C��������c�̂ƃ��[�e�B���O�l�̐^�l������
	u8  rating = getRatingValue( fItem->rating );
	if( rh.s.parental_control_rating_info[ ogn ] != rating )
	{
		throw (gcnew System::Exception("Mismatch Rating Ogn " + ogn.ToString() + ". "
										+ "filename = " + rating.ToString("2X")
										+ "srl = " + rh.s.parental_control_rating_info[ ogn ].ToString("2X") + "."));
		return;
	}

	// ���[�W�����Ɋ܂܂�Ȃ��c�̂̃��[�e�B���O���N���A����Ă��邩�`�F�b�N
	int i;
	for( i=0; i < PARENTAL_CONTROL_INFO_SIZE; i++ )
	{
		if( (ognlist->IndexOf(i) < 0) && (rh.s.parental_control_rating_info[i] != 0) )
		{
			throw (gcnew System::Exception("Rating Ogn " + i.ToString() + " is not Cleared in ROM Header."));
			return;
		}
	}
}
