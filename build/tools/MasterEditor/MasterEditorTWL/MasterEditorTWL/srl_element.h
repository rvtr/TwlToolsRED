#pragma once

// ROM�f�[�^(SRL)�N���X�̊֘A�N���X�̐錾�ƒ�`

#include <apptype.h>
#include "common.h"
#include <twl/types.h>
#include <twl/os/common/format_rom.h>
#include <twl/os/common/ownerInfoEx.h>
#include "srl_const.h"
#include "utility.h"

namespace MasterEditorTWL
{
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
		RCSDKVersion( System::UInt32 code, System::Boolean isStatic )	// �������ɂ̂݃t�B�[���h��ݒ�\
		{
			this->code     = code;
			this->isStatic = isStatic;
			this->hVersion = MasterEditorTWL::analyzeSDKVersion( this->code );
		}
	public:
		property System::String ^Version	// ������ɂ̓t�B�[���h�� Read Only
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
	}; //RCSDKVersion

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
	}; //RCLicense

	// -------------------------------------------------------------------
	// Type : value class
	// Name : VCReservedArea
	//
	// Description : �\��̈�͈̔̓N���X
	// 
	// Role : �\���̂Ƃ��ăf�[�^���܂Ƃ߂Ă���
	// -------------------------------------------------------------------
	value class VCReservedArea
	{
	private:
		System::UInt32  begin;
		System::UInt32  end;
		// value class �ł̓f�t�H���g�R���X�g���N�^�� private �ɂ��ĕ����邱�Ƃ͂ł��Ȃ�
	public:
		VCReservedArea( System::UInt32 begin, System::UInt32 end )
		{
			this->begin = begin;
			this->end   = end;
		}
	public:
		property System::UInt32 Begin		// read only
		{
			System::UInt32 get(){ return this->begin; }
		}
		property System::UInt32 End
		{
			System::UInt32 get(){ return this->end; }
		}
	}; //VCReservedArea

	// -------------------------------------------------------------------
	// Type : ref class
	// Name : RCNandUsedSize
	//
	// Description : �A�v���̃C���|�[�g��NAND����T�C�Y�N���X
	// 
	// Role : �\���̂Ƃ��ăf�[�^���܂Ƃ߂Ă���
	// -------------------------------------------------------------------
	ref class RCNandUsedSize
	{
		// �l�ւ̃A�N�Z�X���@
		// - ���̃f�[�^�T�C�Y : public�����o�ϐ�
		// - �؂�グ���T�C�Y : property (ReadOnly�ȃ����o�ϐ��̂悤�Ɍ�����)
	public:
		property System::UInt32  SrlSize;
		property System::UInt32  PublicSaveSize;
		property System::UInt32  PrivateSaveSize;
		property System::Boolean IsMediaNand;		// NAND�A�v�����ǂ���
		property System::Boolean IsUseSubBanner;	// �T�u�o�i�[���g�p���邩�ǂ���
	private:
		static const System::UInt32  cTmdSize = 16 * 1024;
		static const System::UInt32  cSubBannerSize  = 16  * 1024;
		static const System::UInt32  NandClusterSize = 16  * 1024;
		static const System::UInt32  ShopBlockSize   = 128 * 1024;
	public:
		property System::UInt32  SrlSizeRoundUp		// �؂�グ���l�̓����o�ϐ��Ƃ��Ă͎������v�Z�����l�� property �Œ�
		{
			System::UInt32 get()
			{
				if( !this->IsMediaNand )
				{
					return 0;
				}
				return (MasterEditorTWL::roundUp( this->SrlSize, NandClusterSize ));
			}
		}
		property System::UInt32  PublicSaveSizeRoundUp
		{
			System::UInt32 get(){ return (MasterEditorTWL::roundUp( this->PublicSaveSize, NandClusterSize )); }
		}
		property System::UInt32  PrivateSaveSizeRoundUp
		{
			System::UInt32 get(){ return (MasterEditorTWL::roundUp( this->PrivateSaveSize, NandClusterSize )); }
		}
		property System::UInt32  SubBannerSize	// �T�u�o�i�[�̃T�C�Y�͎g�p���邩���Ȃ����ňقȂ�̂Ōv�Z�����l�� property �Œ�
		{
			System::UInt32 get()
			{
				if( !this->IsUseSubBanner )
				{
					return 0;
				}
				return this->cSubBannerSize;
			}
		}
		property System::UInt32  SubBannerSizeRoundUp
		{
			System::UInt32 get()
			{
				if( !this->IsUseSubBanner )
				{
					return 0;
				}
				return (MasterEditorTWL::roundUp( this->SubBannerSize, NandClusterSize ));
			}
		}
		property System::UInt32  TmdSize
		{
			System::UInt32 get()
			{
				if( !this->IsMediaNand )
				{
					return 0;
				}
				return this->cTmdSize;
			}
		}
		property System::UInt32  TmdSizeRoundUp
		{
			System::UInt32 get()
			{
				if( !this->IsMediaNand )
				{
					return 0;
				}
				return (MasterEditorTWL::roundUp( this->TmdSize, NandClusterSize ));
			}
		}
		property System::UInt32  NandUsedSize		// �S�̂�NAND����T�C�Y�͕ϐ��Ŏ����� property �Œ�
		{
			System::UInt32 get()
			{
				// NAND����ʂ̌v�Z��
				// �ȉ��̃t�@�C����NAND�N���X�^�P��(=16KB)�ɐ؂�グ�č��v����
				//
				//     SRL�̎��t�@�C���T�C�Y
				//     Public�Z�[�u�f�[�^�T�C�Y
				//     Private�Z�[�u�f�[�^�T�C�Y
				//     TMD�T�C�Y(=16KB�Œ�)
				//     �T�u�o�i�[�T�C�Y(�g�p���ɂ�16KB�Œ� �s�g�p���ɂ�0KB)
				if( !this->IsMediaNand )
				{
					return 0;	// �J�[�h�A�v���̂Ƃ�0
				}
				System::UInt32 size = this->SrlSizeRoundUp + this->PublicSaveSizeRoundUp + this->PrivateSaveSizeRoundUp
									  + this->TmdSizeRoundUp + this->SubBannerSizeRoundUp;
				return size;
			}
		}
		property System::UInt32  NandUsedSizeBlock	// �V���b�v�ł̃u���b�N��
		{
			System::UInt32 get()
			{
				System::UInt32 blocks = MasterEditorTWL::roundUp( this->NandUsedSize, this->ShopBlockSize)
										/ this->ShopBlockSize;
				return blocks;
			}
		}
	}; //RCNandUsedSize

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
		RCMrcError(			// ���̌`���ł����C���X�^���X���쐬�ł��Ȃ�
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
			if( (this->begin == 0) && (this->end == 0) )
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
	}; //RCMrcError

	// -------------------------------------------------------------------
	// Type : ref class
	// Name : RCMrcExternalCheckItems
	//
	// Description : MRC�`�F�b�N���ڂ̊O���p�����[�^�N���X
	// 
	// Role : �\���̂Ƃ��ăf�[�^���܂Ƃ߂Ă���
	// -------------------------------------------------------------------
	ref class RCMrcExternalCheckItems
	{
	public:
		property System::Boolean  IsAppendCheck;						// �ǉ��`�F�b�N�����邩�ǂ���(���ׂĂ̍��ڂ��ǉ��`�F�b�N�Ƃ͌���Ȃ�)
		property System::UInt32   SDKVer;								// SDK�̃o�[�W����
		property System::UInt32   SDKVerNotStaticTWL;					// SDK�̃o�[�W����(TWL�풓���W���[���ȊO)
		property System::UInt32   SDKVerNotStaticNTR;					// SDK�̃o�[�W����(NTR�풓���W���[���ȊO)
		property System::Boolean  IsPermitNormalJump;					// �m�[�}���W�����v���A�N�Z�X������Ă��邩
		property cli::array<System::Boolean> ^hIsPermitShared2Array;	// Shared2�t�@�C���A�N�Z�X��������Ă��邩
		property cli::array<System::UInt32>  ^hShared2SizeArray;		// Shared2�t�@�C���T�C�Y
		property System::Collections::Generic::List<VCReservedArea> ^hReservedAreaList;	// �\��̈�͈̔�
	public:
		RCMrcExternalCheckItems()
		{
			this->IsAppendCheck         = false;
			this->SDKVer                = 0;
			this->SDKVerNotStaticTWL    = 0;
			this->SDKVerNotStaticNTR    = 0;
			this->IsPermitNormalJump    = false;
			this->hIsPermitShared2Array = gcnew cli::array<System::Boolean>(METWL_NUMOF_SHARED2FILES);
			this->hShared2SizeArray     = gcnew cli::array<System::UInt32>(METWL_NUMOF_SHARED2FILES);
			System::Int32 i;
			for( i=0; i < METWL_NUMOF_SHARED2FILES; i++ )
			{
				this->hIsPermitShared2Array[i] = false;
				this->hShared2SizeArray[i]     = 0;
			}
			this->hReservedAreaList = gcnew System::Collections::Generic::List<VCReservedArea>;
			this->hReservedAreaList->Clear();
		}
	}; //RCMrcExternalCheckItems

} // end of namespace MasterEditorTWL
