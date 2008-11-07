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
		System::UInt32  ^hCode;
		System::Boolean ^hIsStatic;
	private:
		RCSDKVersion(){}	// ������
	public:
		RCSDKVersion( System::String ^ver, System::UInt32 code, System::Boolean isStatic )	// �������ɂ̂݃t�B�[���h��ݒ�\
		{
			if( ver == nullptr )
				this->hVersion = gcnew System::String("");	// NULL�Q�ƃo�O�������
			else
				this->hVersion  = ver;
			this->hCode     = gcnew System::UInt32( code );
			this->hIsStatic = gcnew System::Boolean( isStatic );
		}
	public:
		property System::String ^Version	// ������ɂ̓t�B�[���h��Read Only
		{
			System::String^ get(){ return this->hVersion; }
		}
	public:
		property System::Boolean IsStatic
		{
			System::Boolean get(){ return *(this->hIsStatic); }
		}
	public:
		property System::UInt32 Code
		{
			System::UInt32 get(){ return *(this->hCode); }
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
			System::String^ get(){ return this->hName; }
		}
	public:
		property System::String ^Publisher
		{
			System::String^ get(){ return this->hPublisher; }
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
		System::UInt32  ^hBegin;	// �J�n�A�h���X
		System::UInt32  ^hEnd;		// �I���A�h���X
		System::String  ^hMsg;		// �G���[���b�Z�[�W
		System::String  ^hNameE;	// �p���
		System::String  ^hMsgE;
		System::Boolean ^hEnableModify;	// �}�X�^�G�f�B�^�ŏC���\���ǂ���
		System::Boolean ^hAffectRom;	// �ύX�����SRL(ROM�o�C�i��)���ύX����邩
	private:
		RCMrcError(){}		// ������
	public:
		RCMrcError( 
			System::String ^name,  System::UInt32 beg,   System::UInt32 end, System::String ^msg, 
			System::String ^nameE, System::String ^msgE, System::Boolean enableModify, System::Boolean affectRom )
		{
			if( name == nullptr )
				this->hName = gcnew System::String("");
			else
				this->hName = name;

			if( nameE == nullptr )
				this->hNameE = gcnew System::String("");
			else
				this->hNameE = nameE;

			this->hBegin = gcnew System::UInt32( beg );
			this->hEnd   = gcnew System::UInt32( end );

			if( msg == nullptr )
				this->hMsg = gcnew System::String("");
			else
				this->hMsg = msg;

			if( msgE == nullptr )
				this->hMsgE = gcnew System::String("");
			else
				this->hMsgE = msgE;

			this->hEnableModify = gcnew System::Boolean( enableModify );
			this->hAffectRom    = gcnew System::Boolean( affectRom );
		}
	public:
		property System::Boolean EnableModify
		{
			System::Boolean get(){ return *(this->hEnableModify); }		// Read Only
		}
		property System::Boolean AffectRom
		{
			System::Boolean get(){ return *(this->hAffectRom); }
		}
	public:
		// gridView�̕\���`���ɂ��킹��
		cli::array<System::Object^>^ getAll( System::Boolean isJapanese )
		{
			if( (*this->hBegin == METWL_ERRLIST_NORANGE) && (*this->hEnd == METWL_ERRLIST_NORANGE) )
			{
				if( isJapanese )
					return (gcnew array<System::Object^>{this->hName,  "-", "-", this->hMsg});
				else
					return (gcnew array<System::Object^>{this->hNameE,  "-", "-", this->hMsgE});
			}

			if( isJapanese )
				return (gcnew array<System::Object^>{this->hName,  this->hBegin->ToString("X04")+"h", this->hEnd->ToString("X04")+"h", this->hMsg});
			else
				return (gcnew array<System::Object^>{this->hNameE, this->hBegin->ToString("X04")+"h", this->hEnd->ToString("X04")+"h", this->hMsgE});
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
		property System::Boolean ^hIsCheck;
		property System::UInt32  ^hSDKVer;
		property System::Byte    ^hEULAVer;
		property cli::array<System::UInt32^> ^hShared2SizeArray;
	public:
		RCMrcSpecialList()
		{
			this->hIsCheck = gcnew System::Boolean( false );
			this->hSDKVer  = gcnew System::UInt32( 0 );
			this->hShared2SizeArray = gcnew cli::array<System::UInt32^>(METWL_NUMOF_SHARED2FILES);	// �t�@�C���T�C�Y�̐��ɍ��킹��
			System::Int32 i;
			for( i=0; i < METWL_NUMOF_SHARED2FILES; i++ )
			{
				this->hShared2SizeArray[i] = gcnew System::UInt32( 0 );
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
		property System::Byte    ^hRomVersion;
		property System::UInt16  ^hHeaderCRC;
		property System::String  ^hLatency;		// MROM/1TROM/Illegal

		// �y�A�����^���R���g���[��
		property cli::array<System::Int32> ^hArrayParentalIndex;		// �\���p�̃R���{�{�b�N�X�̃C���f�b�N�X

		// TWL��p��� �ꕔ�ҏW�\
		property System::UInt32  ^hNormalRomOffset;
		property System::UInt32  ^hKeyTableRomOffset;
		property System::String  ^hTitleIDLo;
		property System::UInt32  ^hTitleIDHi;
		property System::Boolean ^hIsAppLauncher;	// TitleIDLo����킩��A�v�����
		property System::Boolean ^hIsAppUser;		// TitleIDHi����킩��A�v�����
		property System::Boolean ^hIsAppSystem;		//
		property System::Boolean ^hIsAppSecure;		//
		property System::Boolean ^hIsLaunch;		//
		property System::Boolean ^hIsMediaNand;		//
		property System::Boolean ^hIsDataOnly;		//
		property System::UInt16  ^hPublisherCode;	//
		property System::UInt32  ^hPublicSize;
		property System::UInt32  ^hPrivateSize;
		property System::Boolean ^hIsNormalJump;
		property System::Boolean ^hIsTmpJump;
		property System::Boolean ^hHasDSDLPlaySign;	// ROM�w�b�_�O��SRL����킩�鏐���̗L��
		property System::Boolean ^hIsOldDevEncrypt;	// ���J���p�Í��t���O�������Ă���
		property System::Boolean ^hIsSCFGAccess;		// SCFG���W�X�^�����b�N���Ă���

		// TWL�g���t���O �ꕔ�ҏW�\
		property System::Boolean ^hIsCodecTWL;
		property System::Boolean ^hIsEULA;			// �ҏW�\
		property System::Boolean ^hIsSubBanner;
		property System::Boolean ^hIsWiFiIcon;		// �ҏW�\
		property System::Boolean ^hIsWirelessIcon;	// �ҏW�\
		property System::Boolean ^hIsWL;

		// TWL�A�N�Z�X�R���g���[�� Read Only
		property System::Boolean ^hIsCommonClientKey;
		property System::Boolean ^hIsAesSlotBForES;
		property System::Boolean ^hIsAesSlotCForNAM;
		property System::Boolean ^hIsSD;
		property System::Boolean ^hIsNAND;
		property System::Boolean ^hIsGameCardOn;
		property System::Boolean ^hIsShared2;
		property System::Boolean ^hIsAesSlotBForJpegEnc;
		property System::Boolean ^hIsAesSlotBForJpegEncUser;
		property System::Boolean ^hIsGameCardNitro;
		property System::Boolean ^hIsAesSlotAForSSL;
		property System::Boolean ^hIsCommonClientKeyForDebugger;

		// Shared2�t�@�C���T�C�Y Read Only
		property cli::array<System::UInt32^> ^hShared2SizeArray;

		// �J�[�h���[�W���� Read Only
		property System::Boolean ^hIsRegionJapan;
		property System::Boolean ^hIsRegionAmerica;
		property System::Boolean ^hIsRegionEurope;
		property System::Boolean ^hIsRegionAustralia;

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
		void setParentalControlInfo(void);
		void setOneRatingOrgInfo( int ogn );
		void setParentalControlHeader(void);

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
