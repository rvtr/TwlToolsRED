#include "stdafx.h"

#include <apptype.h>
#include <twl/types.h>
#include <twl/os/common/format_rom.h>
#include <acsign/include/acsign.h>
#include <cstring>
#include <cstdio>
#include <MasterEditorTWL/keys.h>
#include "fingerprint_util.h"

using namespace MasterEditorTWL;

// ----------------------------------------------------------------------
// �t�@�C������
// ----------------------------------------------------------------------

// �t�@�C���S�̂̓ǂݍ���
cli::array<System::Byte>^ ReadBin( System::String ^path )
{
	System::IntPtr ptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( path );
	const char *pch = (const char*)ptr.ToPointer();
	FILE       *fp = NULL;
	System::Exception ^ex = nullptr;
	cli::array<System::Byte> ^bin = nullptr;

	try
	{
		if( fopen_s( &fp, pch, "rb" ) != NULL )
		{
			ex = gcnew System::Exception( "Failed to open file:" + path );
			throw ex;
		}

		// �t�@�C���T�C�Y���̃��������蓖��
		fseek( fp, 0, SEEK_END );
		int size = ftell(fp);
		bin = gcnew cli::array<System::Byte>(size);
		pin_ptr<unsigned char> pbin = &bin[0];
		
		fseek( fp, 0, SEEK_SET );
		if( fread( (void*)pbin, 1, size, fp ) != size )
		{
			ex = gcnew System::Exception( "Failed to read data from " + path );
			throw ex;
		}
	}
	finally
	{
		System::Runtime::InteropServices::Marshal::FreeHGlobal( ptr );
		if( fp ) fclose(fp);
		if( ex )
		{
			throw ex;
		}
	}
	return bin;
}

// �t�@�C���S�̂̃��C�g
void WriteBin( System::String ^path, cli::array<System::Byte> ^bin )
{
	System::IntPtr ptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( path );
	const char *pch = (const char*)ptr.ToPointer();
	FILE       *fp = NULL;
	System::Exception ^ex = nullptr;

	try
	{
		if( fopen_s( &fp, pch, "w+b" ) != NULL )	// �㏑���E�o�C�i��
		{
			ex = gcnew System::Exception( "Failed to open file:" + path );
			throw ex;
		}

		int size = bin->Length;
		pin_ptr<unsigned char> pbin = &bin[0];

		(void)fseek( fp, 0, SEEK_SET );
		if( fwrite( (const void*)pbin, 1, size, fp ) != size )
		{
			ex = gcnew System::Exception( "Failed to write data to " + path );
			throw ex;
		}
	}
	finally
	{
		System::Runtime::InteropServices::Marshal::FreeHGlobal( ptr );
		if( fp ) fclose( fp );
		if( ex )
		{
			throw ex;
		}
	}
}

// ROM�w�b�_�̃��[�h
void ReadRomHeader( System::String ^srlpath, ROM_Header *dstrh )
{
	System::IntPtr ptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( srlpath );
	const char *pchFile = (const char*)ptr.ToPointer();
	FILE       *fp = NULL;
	System::Exception ^ex = nullptr;

	try
	{
		if( fopen_s( &fp, pchFile, "rb" ) != NULL )
		{
			ex = gcnew System::Exception( "Failed to open file:" + srlpath );
			throw ex;
		}
		fseek( fp, 0, SEEK_SET );
		if( fread( (void*)dstrh, 1, sizeof(ROM_Header), fp ) != sizeof(ROM_Header) )
		{
			ex = gcnew System::Exception( "Failed to read data from " + srlpath );
			throw ex;
		}
	}
	finally
	{
		System::Runtime::InteropServices::Marshal::FreeHGlobal( ptr );
		if( fp ) fclose(fp);
		if( ex )
		{
			throw ex;
		}
	}
}

// ROM�w�b�_�̃��C�g
void WriteRomHeader( System::String ^srlpath, ROM_Header *srcrh )
{
	System::IntPtr ptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( srlpath );
	const char *pchFile = (const char*)ptr.ToPointer();
	FILE       *fp = NULL;
	System::Exception ^ex = nullptr;

	try
	{
		if( fopen_s( &fp, pchFile, "r+b" ) != NULL )	// �㏑���E�o�C�i��
		{
			ex = gcnew System::Exception( "Failed to open file:" + srlpath );
			throw ex;
		}
		(void)fseek( fp, 0, SEEK_SET );

		if( fwrite( (const void*)srcrh, 1, sizeof(ROM_Header), fp ) != sizeof(ROM_Header) )
		{
			ex = gcnew System::Exception( "Failed to write data to " + srlpath );
			throw ex;
		}
	}
	finally
	{
		System::Runtime::InteropServices::Marshal::FreeHGlobal( ptr );
		if( fp ) fclose( fp );
		if( ex )
		{
			throw ex;
		}
	}
}

// �o�C�i�������ROM�w�b�_�̒��o
void ExtractRomHeader( cli::array<System::Byte> ^bin, ROM_Header *dstrh )
{
	int size = bin->Length;
	pin_ptr<unsigned char> pbin = &bin[0];

	if( size < sizeof(ROM_Header) )
	{
		throw gcnew System::Exception( "The binary size is less than size of the ROM Header." );
	}
	memcpy( dstrh, pbin, sizeof(ROM_Header) );
}

// �o�C�i���ւ�ROM�w�b�_�̏㏑��
void OverwriteRomHeader( cli::array<System::Byte> ^bin, ROM_Header *srcrh )
{
	int size = bin->Length;
	pin_ptr<unsigned char> pbin = &bin[0];

	if( size < sizeof(ROM_Header) )
	{
		throw gcnew System::Exception( "The binary size is less than size of the ROM Header." );
	}
	memcpy( pbin, srcrh, sizeof(ROM_Header) );
}

// �t�@�C���R�s�[
#define COPY_FILE_BUFSIZE   (10*1024*1024)
void CopyFile( System::String ^srcpath, System::String ^dstpath )
{
	System::IntPtr srcptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( srcpath );
	System::IntPtr dstptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( dstpath );
	const char *pchSrcFile = (const char*)srcptr.ToPointer();
	const char *pchDstFile = (const char*)dstptr.ToPointer();
	FILE       *ifp = NULL;
	FILE       *ofp = NULL;
	System::Exception ^ex = nullptr;

	try
	{
		if( fopen_s( &ifp, pchSrcFile, "rb" ) != NULL )
		{
			ex = gcnew System::Exception( "Failed to open file:" + srcpath );
			throw ex;
		}
		if( fopen_s( &ofp, pchDstFile, "wb" ) != NULL )	// �����t�@�C�����폜���ĐV�K�Ƀ��C�g�E�o�C�i��
		{
			ex = gcnew System::Exception( "Failed to open file:" + dstpath );
			throw ex;
		}

		// ���̓t�@�C���̃T�C�Y�擾
		fseek( ifp, 0, SEEK_END );
		System::Int32 size = ftell(ifp);

		cli::array<System::Byte> ^mbuf = gcnew cli::array<System::Byte>(COPY_FILE_BUFSIZE);	// ����̕K�v�Ȃ�
		pin_ptr<unsigned char> buf = &mbuf[0];
		fseek( ifp, 0, SEEK_SET );
		fseek( ofp, 0, SEEK_SET );
		while( size > 0 )
		{
			System::Int32 datasize = (COPY_FILE_BUFSIZE < size)?COPY_FILE_BUFSIZE:size;

			if( datasize != fread(buf, 1, datasize, ifp) )
			{
				ex = gcnew System::Exception( "Failed to read data from " + srcpath );
				throw ex;
			}
			if( datasize != fwrite(buf, 1, datasize, ofp) )
			{
				ex = gcnew System::Exception( "Failed to write data to " + dstpath );
				throw ex;
			}
			size -= datasize;
		}
	}
	finally
	{
		System::Runtime::InteropServices::Marshal::FreeHGlobal( srcptr );
		System::Runtime::InteropServices::Marshal::FreeHGlobal( dstptr );
		if( ifp ) fclose(ifp);
		if( ofp ) fclose(ofp);
		if( ex )
		{
			throw ex;
		}
	}
}

// ----------------------------------------------------------------------
// ����
// ----------------------------------------------------------------------

// ROM�w�b�_�ɏ���������
void SignRomHeader( ROM_Header *rh )
{
	SignatureData   signSrc;						// �����̂��ƂƂȂ�_�C�W�F�X�g�l
	u8              signDst[ RSA_KEY_LENGTH ];		// �����̊i�[��Tmp�o�b�t�@
	u8              decryptBlock[ RSA_KEY_LENGTH ];	// ������������u���b�N�o�b�t�@
	BOOL            result = false;
	int             pos;
	u8              *privateKey = (u8*)g_devPrivKey_DER;
	u8              *publicKey  = (u8*)g_devPubKey_DER;

	// ROM�w�b�_�̃_�C�W�F�X�g���Z�o(�擪����ؖ����̈�̒��O�܂ł��Ώ�)
	ACSign_DigestUnit(
		signSrc.digest,
		rh,
		(u32)&(rh->certificate) - (u32)rh
	);

	// ����I��
#ifdef METWL_VER_APPTYPE_USER
	{
		privateKey = (u8*)g_devPrivKey_DER;
		publicKey  = (u8*)g_devPubKey_DER;
	}
#endif //METWL_VER_APPTYPE_USER

	// �_�C�W�F�X�g�ɏ�����������
	result = ACSign_Encrypto( signDst, privateKey, &signSrc, sizeof(SignatureData) ); 
	if( !result )
	{
		throw gcnew System::Exception( "Exception: Failed to calc the hash of the ROM header." );
	}

	// �������������ă_�C�W�F�X�g�ƈ�v���邩�x���t�@�C����
	result = ACSign_Decrypto( decryptBlock, publicKey, signDst, RSA_KEY_LENGTH );
	for( pos=0; pos < RSA_KEY_LENGTH; pos++ )
	{
		if( decryptBlock[pos] == 0x0 )			// ������u���b�N������f�[�^���T�[�`
			break;
	}
	if( !result || (memcmp( &signSrc, &(decryptBlock[pos+1]), sizeof(SignatureData) ) != 0) )
	{
		throw gcnew System::Exception( "Exception: Failed to compare the hash of the ROM header." );
	}

	// ROM�w�b�_�ɏ������㏑��
	memcpy( rh->signature, signDst, RSA_KEY_LENGTH );

} // ECSrlResult RCSrl::signRomHeader(void)

// ROM�w�b�_�̏����`�F�b�N
void AuthenticateRomHeader( ROM_Header *rh )
{
	u8     original[ RSA_KEY_LENGTH ];	// �����O������̃f�[�^�i�[��
	s32    pos = 0;						// �u���b�N�̐擪�A�h���X
	u8     digest[ DIGEST_SIZE_SHA1 ];	// ROM�w�b�_�̃_�C�W�F�X�g
	u8    *publicKey = (u8*)g_devPubKey_DER;

	// <�f�[�^�̗���>
	// (1) ���J���ŕ�����������(�u���b�N)�����[�J���ϐ�(original)�Ɋi�[
	// (2) �u���b�N����]���ȕ�������菜���Ĉ���(pDst)�ɃR�s�[

#ifdef METWL_VER_APPTYPE_USER
	{
		publicKey  = (u8*)g_devPubKey_DER;
	}
#endif //METWL_VER_APPTYPE_USER

	// �����̉��� = ���J���ŕ���
	if( !ACSign_Decrypto( original, publicKey, rh->signature, RSA_KEY_LENGTH ) )
	{
		throw gcnew System::Exception("Failed to decrypt the ROM signature.");
	}
	// �����O�f�[�^�𕜍���u���b�N����Q�b�g
	for( pos=0; pos < (RSA_KEY_LENGTH-2); pos++ )   // �{���u���b�N�̐擪��0x00�����������̓��������ɂ���ď�����d�l
	{
		// �Í��u���b�N�`�� = 0x00, BlockType, Padding, 0x00, ���f�[�^
		if( original[pos] == 0x00 )                               // ���f�[�^�̒��O��0x00���T�[�`
		{
			break;
		}
	}
	// �x���t�@�C
	// ROM�w�b�_�̃_�C�W�F�X�g���Z�o(�擪����ؖ����̈�̒��O�܂ł��Ώ�)
	ACSign_DigestUnit( digest,	rh, (u32)&(rh->certificate) - (u32)rh );
	if( memcmp( &(original[pos+1]), digest, DIGEST_SIZE_SHA1 ) != 0 )
	{
		System::String ^msg =
			"Failed to verify the ROM digest data.\r\n" 
			+"Either this is not TWL-compatible or TWL-exclusive ROM data, or the ROM data may have been modified.";
		throw gcnew System::Exception(msg);
	}
}

// ----------------------------------------------------------------------
// �ϊ�
// ----------------------------------------------------------------------

// ��������o�C�g�z��ɕϊ�
cli::array<System::Byte>^ TransStringToBytes( System::String ^src, const int len )
{
	cli::array<System::Byte> ^bytes = gcnew cli::array<System::Byte>(len);

	int i;
	for( i=0; i < len; i++ )
	{
		bytes[i] = 0;
	}

	int n = (len < src->Length)?len:src->Length;
	for( i=0; i < src->Length; i++ )
	{
		if( i < len )
		{
			bytes[i] = (System::Byte)src[i];
		}
	}
	return bytes;
}

// 16�i��������o�C�g�z��ɕϊ�
cli::array<System::Byte>^ TransHexStringToBytes( System::String ^src, const int len )
{
	cli::array<System::Byte> ^bytes = gcnew cli::array<System::Byte>(len);

	int i;
	for( i=0; i < len; i++ )
	{
		bytes[i] = 0;
	}

	int j=0;
	for( i=0; i < src->Length; )
	{
		if( src[i] == ' ' )
		{
			i++;
			continue;
		}
		if( (src[i] != ' ') && (j < len) )
		{
			if( (i+1) >= src->Length )
			{
				throw gcnew System::Exception("One hexadecimal number must be expressed by 2 charactors, for example \"6B\".");
			}
			bytes[j++] = System::Byte::Parse(src->Substring(i,2), System::Globalization::NumberStyles::HexNumber);
			i += 2;
		}
	}
	return bytes;
}

// �o�C�g�z��𕶎���ɕϊ�
System::String^ TransBytesToString( cli::array<System::Byte> ^bytes )
{
	System::String ^str = gcnew System::String("");
	int i;
	for( i=0; i < bytes->Length; i++ )
	{
		if( (0x20 <= bytes[i]) && (bytes[i] <= 0x7E) )
		{
			char c[2];
			c[0] = bytes[i]; c[1] = 0;
			str += gcnew System::String( c );
		}
		else
		{
			str += " ";
		}
		if( !((i+1) % 16) )
		{
			str += "\r\n";
		}
	}
	return str;
}

// �o�C�g�z���16�i������ɕϊ�
System::String^ TransBytesToHexString( cli::array<System::Byte> ^bytes )
{
	System::String ^str = gcnew System::String("");

	int i;
	for( i=0; i < bytes->Length; i++ )
	{
		System::Byte b = bytes[i];
		str += System::String::Format( "{0:X02}", (char)b );
		if( (i+1) % 16 )
		{
			str += " ";
		}
		else
		{
			str += "\r\n";
		}
	}
	return str;
}


// ----------------------------------------------------------------------
// GUI
// ----------------------------------------------------------------------

// �t�@�C�����_�C�A���O�Ŏ擾
// @arg [in] �f�t�H���g�̃f�B���N�g��
// @arg [in] �g���q�t�B���^
// @ret �擾�����t�@�C���� �G���[�̂Ƃ� nullptr
System::String^ OpenFileUsingDialog( System::String ^defdir, System::String ^filter )
{
	System::Windows::Forms::OpenFileDialog ^dlg = gcnew (System::Windows::Forms::OpenFileDialog);
	if( System::String::IsNullOrEmpty( defdir ) || !System::IO::Directory::Exists( defdir ) )
	{
		dlg->InitialDirectory = System::Environment::GetFolderPath( System::Environment::SpecialFolder::Desktop );
	}
	else
	{
		dlg->InitialDirectory = defdir;	// �O�ɑI�񂾃f�B���N�g�����f�t�H���g�ɂ���
	}
	dlg->Filter      = filter;
	dlg->FilterIndex = 1;
	dlg->RestoreDirectory = true;

	if( dlg->ShowDialog() != System::Windows::Forms::DialogResult::OK )
	{
		return nullptr;
	}
	return System::String::Copy(dlg->FileName);
}

// �Z�[�u����t�@�C�����_�C�A���O�Ŏ擾
// @arg [in] �f�t�H���g�̃f�B���N�g��
// @arg [in] �g���q�t�B���^
// @arg [in] �t�@�C���̊g���q���s���ȂƂ��ɒǉ�����Ƃ��̐������g���q
// @ret �擾�����t�@�C���� �G���[�̂Ƃ� nullptr
System::String^ SaveFileUsingDialog( System::String ^defdir, System::String ^filter, System::String ^extension )
{
	return (SaveFileUsingDialog( defdir, nullptr, filter, extension ));
}

// �Z�[�u����t�@�C�����_�C�A���O�Ŏ擾
// @arg [in] �f�t�H���g�̃f�B���N�g��
// @arg [in] �f�t�H���g�̃t�@�C����
// @arg [in] �g���q�t�B���^
// @arg [in] �t�@�C���̊g���q���s���ȂƂ��ɒǉ�����Ƃ��̐������g���q
// @ret �擾�����t�@�C���� �G���[�̂Ƃ� nullptr
System::String^ SaveFileUsingDialog( System::String ^defdir, System::String ^defname, System::String ^filter, System::String ^extension )
{
	System::String ^retfile;
	System::Windows::Forms::SaveFileDialog ^dlg = gcnew (System::Windows::Forms::SaveFileDialog);

	if( System::String::IsNullOrEmpty( defdir ) || !System::IO::Directory::Exists( defdir ) )
	{
		dlg->InitialDirectory = System::Environment::GetFolderPath( System::Environment::SpecialFolder::Desktop );
	}
	else
	{
		dlg->InitialDirectory = defdir;
	}
	dlg->Filter      = filter;
	dlg->FilterIndex = 1;
	dlg->RestoreDirectory = true;
	if( defname )
	{
		dlg->FileName = defname;
	}

	if( dlg->ShowDialog() != System::Windows::Forms::DialogResult::OK )
	{
		return nullptr;
	}
	retfile = dlg->FileName;
	if( !System::String::IsNullOrEmpty(extension) && !(dlg->FileName->ToLower()->EndsWith( extension->ToLower() )) )
	{
		retfile += extension;
	}
	return retfile;
}

// ----------------------------------------------------------------------
// �O���v���O�����̎��s
// ----------------------------------------------------------------------

// maketad�̎��s
// @arg [in] maketad�̃p�X
// @arg [in] ����SRL�̃p�X
// @arg [in] �o��SRL�̃p�X
void makeTad( System::String ^maketad_path, System::String ^srlpath, System::String ^tadpath )
{
	System::Diagnostics::ProcessStartInfo ^info = gcnew System::Diagnostics::ProcessStartInfo();
	info->FileName  = maketad_path; 
	info->Arguments = "\"" + srlpath + "\" -o \"" + tadpath + "\"";
	info->CreateNoWindow  = true;
	info->UseShellExecute = false;
	info->RedirectStandardError  = true;

	System::Diagnostics::Process ^p = System::Diagnostics::Process::Start( info );
	//p->WaitForExit();
	System::String ^output = p->StandardError->ReadToEnd();
	output->Replace( (char)'\r\r\n', (char)'\n' );
	System::Console::WriteLine( "{0}", output );

	if( output != "" )
	{
		throw gcnew System::Exception("Failed to transform SRL to TAD.");
	}
	else
	{
		System::Console::WriteLine( "Succeeded to transform." );
	}
}

// ----------------------------------------------------------------------
// �R���\�[��
// ----------------------------------------------------------------------

// getopt : ��������͂��ăI�v�V����������������1�Ԃ�
// @arg [in/out] ���� (�I�v�V���������������ꍇ�폜����Ă���)
// @arg [in]     �I�v�V���������X�g
// @arg [out]    �I�v�V���������̊i�[�ꏊ (�Ȃ��ꍇ nullptr������)
// @ret          �ŏ��Ɍ��������I�v�V����
char getopt( cli::array<System::String^> ^%args, System::String ^condition, System::String ^% optarg )
{
	char opt=0;

	// ������
	optarg = nullptr;

	// �I�v�V������T���ď����ɍ������ǂ����𒲂ׂ�
	int ai;
	int ci = 0;
	for( ai=0; ai < args->Length; ai++ )
	{
		System::String ^arg = args[ai];
		if( arg->StartsWith("-") && arg->Length >= 2 )
		{
			// �����ɍ����I�v�V�������ǂ����𒲂ׂ�
			ci = condition->IndexOf( arg[1] );
			if( ci >= 0 )
			{
				opt = (char)condition[ci];
			}
			break;
		}
	}
	// �I�v�V�����͂������������ɂȂ��s���ȃI�v�V����������
	if( ci < 0 )
	{
		return ((char)0);
	}
	// �I�v�V���������݂��Ȃ�����
	if( ai == args->Length )
	{
		return ((char)-1);
	}

	// �I�v�V�������������邩�`�F�b�N
	if( ((ci+1) < condition->Length) && (condition[ci+1] == ':') )
	{
		if( ((ai+1) < args->Length) && !args[ai+1]->StartsWith("-") )
		{
			optarg = System::String::Copy(args[ai+1]);
			shrinkArgs( args, ai+1);
		}
		else
		{
			return ((char)0);	// �I�v�V�����������Ȃ�
		}
	}

	// �I�v�V�������폜����
	shrinkArgs( args, ai );
	return opt;	
}

// args �������̗v�f���폜
// @arg [in] ����
// @arg [in] �폜����v�f�̃C���f�b�N�X
void shrinkArgs( cli::array<System::String^> ^%args, const int index )
{
	int i;
	for( i=index+1; i < args->Length; i++ )
	{
		args[i-1] = args[i];
	}
	cli::array<System::String^>::Resize( args, args->Length - 1 );
}
