#pragma once

// ROM�f�[�^(SRL)�N���X�Ɗ֘A�N���X�̐錾

#include <apptype.h>
#include "common.h"
#include <twl/types.h>
#include <twl/os/common/format_rom.h>
#include <twl/os/common/ownerInfoEx.h>

namespace MasterEditorTWL
{
	// -------------------------------------------------------------------
	// Type : enum class
	// Name : ECSrlResult
	//
	// Description : RCSrl�N���X�̑���ł̃G���[��錾
	// -------------------------------------------------------------------
	enum class ECSrlResult
	{
		NOERROR   = 0,
		// �G���[���肵�Ȃ��Ă��������킩��Ƃ��̕Ԃ�l
		// (�G���[��������\���̂���ӏ���1�� etc.)
		ERROR,
		// �t�@�C������ł̃G���[
		ERROR_FILE_OPEN,
		ERROR_FILE_READ,
		ERROR_FILE_WRITE,
		// �����ł̃G���[
		ERROR_SIGN_ENCRYPT,	// �����ł��Ȃ�
		ERROR_SIGN_DECRYPT,	// ���������ł��Ȃ�
		ERROR_SIGN_VERIFY,	// ��������������̃_�C�W�F�X�g������Ȃ�
		// CRC�Z�o�ł̃G���[
		ERROR_SIGN_CRC,
		// SDK�o�[�W�����擾�ł̃G���[
		ERROR_SDK,
		// �v���b�g�t�H�[����NTR��p or �s���o�C�i��
		ERROR_PLATFORM,
	};

	// -------------------------------------------------------------------
	// Type : enum class
	// Name : ECAppType
	//
	// Description : RCSrl�N���X�̃A�v����ʂ�錾
	// -------------------------------------------------------------------
	enum class ECAppType
	{
		USER   = 0,
		SYSTEM,
		SECURE,
		LAUNCHER,
		ILLEGAL,	// �s��
	};

	// -------------------------------------------------------------------
	// Type : ref class
	// Name : RCSDKVersion
	//
	// Description : RCSrl�N���X�Ɏ�������SDK�o�[�W�������N���X
	// 
	// Role : �\���̂Ƃ��ăf�[�^���܂Ƃ߂Ă���
	// -------------------------------------------------------------------
	ref class RCSDKVersion
	{
	private:
		System::String  ^hVersion;
		System::UInt32   code;
		System::Boolean  isStatic;
	private:
		RCSDKVersion(){}	// ������
	public:
		RCSDKVersion( System::String ^ver, System::UInt32 code, System::Boolean isStatic )	// �������ɂ̂݃t�B�[���h��ݒ�\
		{
			if( ver == nullptr )
				this->hVersion = gcnew System::String("");	// NULL�Q�ƃo�O�������
			else
				this->hVersion  = ver;
			this->code     = code;
			this->isStatic = isStatic;
		}
	public:
		property System::String ^Version	// ������ɂ̓t�B�[���h��Read Only
		{
			System::String^ get(){ return System::String::Copy(this->hVersion); }
		}
	public:
		property System::Boolean IsStatic
		{
			System::Boolean get(){ return (this->isStatic); }
		}
	public:
		property System::UInt32 Code
		{
			System::UInt32 get(){ return (this->code); }
		}
	};

	// -------------------------------------------------------------------
	// Type : ref class
	// Name : RCLicense
	//
	// Description : RCSrl�N���X�Ɏ������郉�C�Z���X���N���X
	// 
	// Role : �\���̂Ƃ��ăf�[�^���܂Ƃ߂Ă���
	// -------------------------------------------------------------------
	ref class RCLicense
	{
	private:
		System::String ^hPublisher;
		System::String ^hName;
	private:
		RCLicense(){}	// ������
	public:
		RCLicense( System::String ^pub, System::String ^name )	// �������ɂ̂݃t�B�[���h��ݒ�\
		{
			if( pub == nullptr )
				this->hPublisher = gcnew System::String("");
			else
				this->hPublisher = pub;

			if( name == nullptr )
				this->hName = gcnew System::String("");
			else
				this->hName = name;
		}
	public:
		property System::String ^Name	// ������ɂ̓t�B�[���h��Read Only
		{
			System::String^ get(){ return System::String::Copy(this->hName); }
		}
	public:
		property System::String ^Publisher
		{
			System::String^ get(){ return System::String::Copy(this->hPublisher); }
		}
	};

	// -------------------------------------------------------------------
	// Type : ref class
	// Name : RCMrcError
	//
	// Description : RCSrl�N���X�Ɏ�������MRC�G���[���N���X
	// 
	// Role : �\���̂Ƃ��ăf�[�^���܂Ƃ߂Ă���
	// -------------------------------------------------------------------
	public ref class RCMrcError
	{
	private:
		System::String  ^hName;		// ���ږ�
		System::UInt32   begin;		// �J�n�A�h���X
		System::UInt32   end;		// �I���A�h���X
		System::String  ^hMsg;		// �G���[���b�Z�[�W
		System::String  ^hNameE;	// �p���
		System::String  ^hMsgE;
		System::Boolean  isEnableModify;	// �}�X�^�G�f�B�^�ŏC���\���ǂ���
		System::Boolean  isAffectRom;		// �ύX�����SRL(ROM�o�C�i��)���ύX����邩
	private:
		RCMrcError(){}		// ������
	public:
		RCMrcError( 
			System::String ^name,  System::UInt32 beg,   System::UInt32 end, System::String ^msg, 
			System::String ^nameE, System::String ^msgE, System::Boolean isEnableModify, System::Boolean isAffectRom )
		{
			if( name == nullptr )
				this->hName = gcnew System::String("");
			else
				this->hName = name;

			if( nameE == nullptr )
				this->hNameE = gcnew System::String("");
			else
				this->hNameE = nameE;

			this->begin = beg;
			this->end   = end;

			if( msg == nullptr )
				this->hMsg = gcnew System::String("");
			else
				this->hMsg = msg;

			if( msgE == nullptr )
				this->hMsgE = gcnew System::String("");
			else
				this->hMsgE = msgE;

			this->isEnableModify = isEnableModify;
			this->isAffectRom    = isAffectRom;
		}
	public:
		property System::Boolean IsEnableModify
		{
			System::Boolean get(){ return this->isEnableModify; }		// Read Only
		}
		property System::Boolean IsAffectRom
		{
			System::Boolean get(){ return this->isAffectRom; }
		}
	public:
		// gridView�̕\���`���ɂ��킹��
		cli::array<System::Object^>^ getAll( System::Boolean isJapanese )
		{
			if( (this->begin == METWL_ERRLIST_NORANGE) && (this->end == METWL_ERRLIST_NORANGE) )
			{
				if( isJapanese )
					return (gcnew array<System::Object^>{this->hName,  "-", "-", this->hMsg});
				else
					return (gcnew array<System::Object^>{this->hNameE,  "-", "-", this->hMsgE});
			}

			if( isJapanese )
				return (gcnew array<System::Object^>{this->hName,  this->begin.ToString("X04")+"h", this->end.ToString("X04")+"h", this->hMsg});
			else
				return (gcnew array<System::Object^>{this->hNameE, this->begin.ToString("X04")+"h", this->end.ToString("X04")+"h", this->hMsgE});
		}
	};

	// -------------------------------------------------------------------
	// Type : ref class
	// Name : RCMrcSpecialList
	//
	// Description : MRC�̒ǉ��G���[���ڃN���X
	// 
	// Role : �\���̂Ƃ��ăf�[�^���܂Ƃ߂Ă���
	// -------------------------------------------------------------------
	ref class RCMrcSpecialList
	{
	public:
		property System::Boolean  IsCheck;
		property System::UInt32   SDKVer;
		property System::Byte     EULAVer;
		property cli::array<System::UInt32> ^hShared2SizeArray;
	public:
		RCMrcSpecialList()
		{
			this->IsCheck = false;
			this->SDKVer  = 0;
			this->EULAVer = 0;
			this->hShared2SizeArray = gcnew cli::array<System::UInt32>(METWL_NUMOF_SHARED2FILES);	// �t�@�C���T�C�Y�̐��ɍ��킹��
			System::Int32 i;
			for( i=0; i < METWL_NUMOF_SHARED2FILES; i++ )
			{
				this->hShared2SizeArray[i] = 0;
			}
		}
	};

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

		// TWL��p��� �ꕔ�ҏW�\
		property System::UInt32   NormalRomOffset;
		property System::UInt32   KeyTableRomOffset;
		property System::String  ^hTitleIDLo;
		property System::UInt32   TitleIDHi;
		property System::Boolean  IsAppLauncher;	// TitleIDLo����킩��A�v�����
		property System::Boolean  IsAppUser;		// TitleIDHi����킩��A�v�����
		property System::Boolean  IsAppSystem;	//
		property System::Boolean  IsAppSecure;	//
		property System::Boolean  IsLaunch;		//
		property System::Boolean  IsMediaNand;	//
		property System::Boolean  IsDataOnly;	//
		property System::UInt16   PublisherCode;	//
		property System::UInt32   PublicSize;
		property System::UInt32   PrivateSize;
		property System::Boolean  IsNormalJump;
		property System::Boolean  IsTmpJump;
		property System::Boolean  HasDSDLPlaySign;	// ROM�w�b�_�O��SRL����킩�鏐���̗L��
		property System::Boolean  IsOldDevEncrypt;	// ���J���p�Í��t���O�������Ă���
		property System::Boolean  IsSCFGAccess;		// SCFG���W�X�^�����b�N���Ă���

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

		// Shared2�t�@�C���T�C�Y Read Only
		property cli::array<System::UInt32> ^hShared2SizeArray;

		// �J�[�h���[�W���� Read Only
		property System::Boolean IsRegionJapan;
		property System::Boolean IsRegionAmerica;
		property System::Boolean IsRegionEurope;
		property System::Boolean IsRegionAustralia;

		// SDK�o�[�W�����Ǝg�p���C�u�����̃��X�g
		property System::Collections::Generic::List<RCSDKVersion^> ^hSDKList;
		property System::Collections::Generic::List<RCLicense^> ^hLicenseList;

		// MRC�@�\�Ń`�F�b�N���ꂽ�G���[���̃��X�g
		property System::Collections::Generic::List<RCMrcError^> ^hErrorList;
		property System::Collections::Generic::List<RCMrcError^> ^hWarnList;
		property System::Collections::Generic::List<RCMrcError^> ^hParentalErrorList;	// �y�A�����^���R���g���[���̃`�F�b�N��set�Ɠ����ɍs�Ȃ�

		// MRC�ǉ�����
		property RCMrcSpecialList ^hMrcSpecialList;

		// constructor and destructor
	public:
		RCSrl();
		~RCSrl();

		// method
	public:

		//
		// ROM�w�b�_�̃t�@�C�����o��
		//
		// @arg [in/out] ���o�̓t�@�C����
		//
		ECSrlResult readFromFile ( System::String ^filename );
		ECSrlResult writeToFile( System::String ^filename );
		
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
		void setParentalControlHeader(void);			// ROM�w�b�_�Ƀt�B�[���h�̒l�𔽉f������

		// ROM�w�b�_�̍X�V
		ECSrlResult calcRomHeaderCRC(void);	// ROM�w�b�_��CRC���Čv�Z
		ECSrlResult signRomHeader(void);	// ROM�w�b�_�X�V��̍ď���
		ECSrlResult decryptRomHeader(void);	// ROM�w�b�_�̏������O��

		// SRL�o�C�i���������Ȑݒ�𒲂ׂ�
		ECSrlResult hasDSDLPlaySign( FILE *fp );
				// DS�_�E�����[�h������SRL�Ɋi�[����Ă��邩���ׂ�
				// @arg [in]  ���̓t�@�C����FP (->SRL�ǂݍ��ݎ��Ɏ��s�����ׂ�)]
		ECSrlResult searchSDKVersion( FILE *fp );		// SDK�o�[�W�������擾����
		ECSrlResult searchLicenses( FILE *fp );			// �g�p���C�Z���X���擾����
		ECAppType selectAppType(void);

		// MRC(Master ROM Checker)�@�\
		ECSrlResult mrc( FILE *fp );
		ECSrlResult mrcNTR( FILE *fp );
		ECSrlResult mrcTWL( FILE *fp );
		void mrcPadding( FILE *fp );
		void mrcBanner( FILE *fp );
		void mrcReservedArea( FILE *fp );

	}; // end of ref class RCSrl

} // end of namespace MasterEditorTWL
