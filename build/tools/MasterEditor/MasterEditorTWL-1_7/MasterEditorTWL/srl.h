#pragma once

// ROM�f�[�^(SRL)�N���X�̐錾

#include <apptype.h>
#include "common.h"
#include <twl/types.h>
#include <twl/os/common/format_rom.h>
#include <twl/os/common/ownerInfoEx.h>
#include "srl_const.h"
#include "srl_element.h"
#include "utility.h"
#include "message_bank.h"

namespace MasterEditorTWL
{
	// -------------------------------------------------------------------
	// Type : ref class
	// Name : RCSrl
	//
	// Description : ROM�f�[�^(SRL)�̐ݒ���N���X
	// 
	// Role : ROM�f�[�^�̃t�@�C�����o�́E�������̍X�V
	// -------------------------------------------------------------------
	ref class RCSrl
	{
		// field
	private:
		// ROM�w�b�_
		ROM_Header *pRomHeader;

	public:
		// (GUI�ɕ\�������)ROM�w�b�_�ŗL���

		// NTR�݊���� ReadOnly
		property System::String  ^hTitleName;
		property System::String  ^hGameCode;
		property System::String  ^hMakerCode;
		property System::String  ^hPlatform;
		property System::String  ^hRomSize;
		property System::Byte     RomVersion;
		property System::UInt16   HeaderCRC;
		property System::String  ^hLatency;		// MROM/1TROM/Illegal

		// �y�A�����^���R���g���[��
		property cli::array<System::Int32> ^hArrayParentalIndex;	// �\���p�̃R���{�{�b�N�X�̃C���f�b�N�X
		property System::Boolean  IsUnnecessaryRating;				// ���[�e�B���O�\���s�v�t���O

		// TWL��p��� Read Only
		property System::UInt32   NormalRomOffset;
		property System::UInt32   KeyTableRomOffset;
		property System::String  ^hTitleIDLo;
		property System::UInt32   TitleIDHi;
		property System::Boolean  IsAppLauncher;	// TitleIDLo����킩��A�v�����
		property System::Boolean  IsAppUser;		// TitleIDHi����킩��A�v�����
		property System::Boolean  IsAppSystem;		//
		property System::Boolean  IsAppSecure;		//
		property System::Boolean  IsLaunch;			//
		property System::Boolean  IsMediaNand;		//
		property System::Boolean  IsDataOnly;		//
		property System::UInt16   PublisherCode;	//
		property System::Boolean  IsNormalJump;
		property System::Boolean  IsTmpJump;
		property System::Boolean  HasDSDLPlaySign;	// ROM�w�b�_�O��SRL����킩�鏐���̗L��
		property System::Boolean  IsOldDevEncrypt;	// ���J���p�Í��t���O�������Ă���
		property System::Boolean  IsSCFGAccess;		// SCFG���W�X�^�����b�N���Ă���
		property System::UInt32   PublicSize;		// �Z�[�u�f�[�^�T�C�Y
		property System::UInt32   PrivateSize;

		// NAND����T�C�Y
		RCNandUsedSize ^hNandUsedSize;

		// TWL�g���t���O Read Only
		property System::Boolean IsCodecTWL;
		property System::Boolean IsEULA;
		property System::Boolean IsSubBanner;
		property System::Boolean IsWiFiIcon;
		property System::Boolean IsWirelessIcon;
		property System::Boolean IsWL;

		// TWL�A�N�Z�X�R���g���[�� Read Only
		property System::Boolean IsCommonClientKey;
		property System::Boolean IsAesSlotBForES;
		property System::Boolean IsAesSlotCForNAM;
		property System::Boolean IsSD;
		property System::Boolean IsNAND;
		property System::Boolean IsGameCardOn;
		property System::Boolean IsShared2;
		property System::Boolean IsAesSlotBForJpegEnc;
		property System::Boolean IsAesSlotBForJpegEncUser;
		property System::Boolean IsGameCardNitro;
		property System::Boolean IsAesSlotAForSSL;
		property System::Boolean IsCommonClientKeyForDebugger;
		property System::Boolean IsPhotoWrite;
		property System::Boolean IsPhotoRead;
		property System::Boolean IsSDWrite;				// SD�J�[�h�A�N�Z�X�� : �t���O�Ƃ͕ʂɗp�ӂ��ꂽ(5.2 RELEASE)
		property System::Boolean IsSDRead;
		property System::Boolean IsOldSDK52Release;	// SD�A�N�Z�X���𒲂ׂ�K�v�����邩 : 5.2 RELEASE �ȍ~�̂Ƃ����ׂ�K�v����
		property System::Boolean IsOldSDK51PR;		// HYBRID-NAND�A�v�����֎~����K�v�����邩 : 5.1 PR �ȑO�̂Ƃ����ׂ�K�v����

		// Shared2�t�@�C���T�C�Y Read Only
		property cli::array<System::UInt32> ^hShared2SizeArray;

		// �J�[�h���[�W���� Read Only
		property System::Boolean IsRegionJapan;
		property System::Boolean IsRegionAmerica;
		property System::Boolean IsRegionEurope;
		property System::Boolean IsRegionAustralia;
		property System::Boolean IsRegionKorea;
		property System::Boolean IsRegionChina;

		// ���ؐݒ�t���O Read Only
		property System::Boolean IsForChina;
		property System::Boolean IsForKorea;

		// SDK�o�[�W�����Ǝg�p���C�u�����̃��X�g
		property System::Collections::Generic::List<RCSDKVersion^> ^hSDKList;
		property System::Collections::Generic::List<RCLicense^> ^hLicenseList;

		// MRC�@�\�Ń`�F�b�N���ꂽ�G���[���̃��X�g
		property System::Collections::Generic::List<RCMrcError^> ^hErrorList;
		property System::Collections::Generic::List<RCMrcError^> ^hWarnList;
		property System::Collections::Generic::List<RCMrcError^> ^hParentalErrorList;	// �ǂݍ��ݎ��ɔ��������y�A�����^���R���g���[�����̃G���[
		property System::Collections::Generic::List<RCMrcError^> ^hParentalWarnList;

		// MRC�ǉ�����
		property RCMrcExternalCheckItems ^hMrcExternalCheckItems;

		// MRC���b�Z�[�W
		property RCMessageBank ^hMrcMsg;

		// constructor / destructor / finalizer 
	private:
		RCSrl(){};
	public:
		RCSrl( System::String ^msgfileJ, System::String ^msgfileE );	// MRC���b�Z�[�W�̃t�@�C�����w�肷��
		~RCSrl();
		!RCSrl();

		// method
	public:

		//
		// ROM�w�b�_�̃t�@�C�����o��
		//
		// @arg [in/out] ���o�̓t�@�C����
		//
		ECSrlResult readFromFile( System::String ^srlfile );
		ECSrlResult writeToFile( System::String ^srlfile );
		
		// internal method
	private:
		// ROM�ŗL����ROM�w�b�_�̐ݒ�
		ECSrlResult setRomInfo(void);		// ROM�w�b�_����擾����ROM�ŗL�����t�B�[���h�ɔ��f������
		ECSrlResult setRomHeader(void);		// ROM�w�b�_��ROM�ŗL���t�B�[���h�̒l�𔽉f������

		// �y�A�����^���R���g���[���̐ݒ�
		bool setRegionInfo( u32 region );				// ���[�W�������擾(�G���[�����o�^) @ret ���[�W���������������ǂ���
		void setUnnecessaryRatingInfo( u32 region );	// ���[�e�B���O��񂪕s�v���ǂ������擾(�G���[�����o�^)
		void setRatingInfo( u32 region );				// ���[�W�����Ɋ܂܂��c�̂̃��[�e�B���O���擾
		void setOneRatingOrgInfo( int ogn );			// 1�̒c�̂̃��[�e�B���O���擾(�G���[�����o�^)
		void setRatingRomHeader( u32 region );			// ROM�w�b�_�Ƀt�B�[���h�̒l�𔽉f������

		// ROM�w�b�_�̍X�V
		ECSrlResult calcRomHeaderCRC(void);					// ROM�w�b�_��CRC���Čv�Z
		ECSrlResult signRomHeader(void);					// ROM�w�b�_�X�V��̍ď���
		ECSrlResult decryptRomHeader( ROM_Header *prh );	// ROM�w�b�_�̏������O��

		// SRL�o�C�i���������Ȑݒ�𒲂ׂ�
		ECSrlResult hasDSDLPlaySign( FILE *fp );
				// DS�_�E�����[�h������SRL�Ɋi�[����Ă��邩���ׂ�
				// @arg [in]  ���̓t�@�C����FP (->SRL�ǂݍ��ݎ��Ɏ��s�����ׂ�)]
		ECSrlResult searchSDKVersion( FILE *fp );		// SDK�o�[�W�������擾����
		ECSrlResult searchLicenses( FILE *fp );			// �g�p���C�Z���X���擾����
		ECAppType selectAppType( ROM_Header *prh );		// TitleID����A�v����ʂ����肷��
		void calcNandUsedSize( FILE *fp );				// NAND����T�C�Y���v�Z����

		// MRC(Master ROM Checker)�@�\
		ECSrlResult mrc( FILE *fp );
		ECSrlResult mrcNTR( FILE *fp );
		ECSrlResult mrcTWL( FILE *fp );
		void mrcAppType( FILE *fp );
		void mrcAccessControl( FILE *fp );
		void mrcPadding( FILE *fp );
		void mrcBanner( FILE *fp );
		void mrcReservedArea( FILE *fp );
		void mrcShared2( FILE *fp );
		void mrcSDKVersion( FILE *fp );
		void mrcChinaKorea(void);

		// MRC���b�Z�[�W��ǉ�
		RCMrcError^ makeMrcError( System::String ^tag, ... cli::array<System::String^> ^args );
		RCMrcError^ makeMrcError( System::UInt32 beg, System::UInt32 end, System::Boolean isEnableModify, System::Boolean isAffectRom,
								  System::String ^tag, ... cli::array<System::String^> ^args );

	}; // end of ref class RCSrl

} // end of namespace MasterEditorTWL
