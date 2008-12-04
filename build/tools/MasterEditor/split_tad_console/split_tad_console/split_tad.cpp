#include "stdafx.h"
#include "twl/types.h"
#include <cstring>
#include <cstdio>
#include "split_tad.h"
#include "split_tad_util.h"


// ------------------------------------------------------
// �錾�ƒ萔
// ------------------------------------------------------

// internal functions
cli::array<System::Byte>^ readTitleKey( cli::array<System::Byte> ^ticket );
cli::array<System::Byte>^ decCBC( cli::array<System::Byte> ^ Key, cli::array<System::Byte> ^ IV, cli::array<System::Byte> ^cipherText  );
cli::array<rcContentsInfo^> ^readContentsInfo( cli::array<System::Byte> ^tmd );

// tad�O���p�̌�
const u8 commonKey[] =
{
	0xA1,0x60,0x4A,0x6A,0x71,0x23,0xB5,0x29,0xAE,0x8B,0xEC,0x32,0xC8,0x16,0xFC,0xAA
};


// ------------------------------------------------------
// tad�O�������{��
// ------------------------------------------------------

//
// tad �t�@�C������ srl(0�Ԗڂ̃R���e���c)�𔲂��o��
// (split_tad_dev.pl �̈ڐA)
//
// @arg [in]  ���� tad �t�@�C����
// @arg [out] �o�� srl �t�@�C����
//
// @ret ���������Ƃ�0 ���s�����畉�̒l
//
int splitTad( System::String ^tadpath, System::String ^srlpath )
{
	FILE       *fp = NULL;
	const char *pchFilename = 
		(const char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( tadpath ).ToPointer();

	if( fopen_s( &fp, pchFilename, "rb" ) != NULL )
	{
		return -1;
	}

	cli::array<System::Byte> ^mbuf = subStr( fp, 0, 32 );

	u32 hdrSize     = reverseEndian( unpack32(mbuf, 0) );	// ��{�I�Ƀr�b�O�G���f�B�A��
	u16 tadType     = reverseEndian( unpack16(mbuf, 4) );
	u16 tadVersion  = reverseEndian( unpack16(mbuf, 6) );
	u32 certSize    = reverseEndian( unpack32(mbuf, 8) );
	u32 crlSize     = reverseEndian( unpack32(mbuf, 12) );
	u32 ticketSize  = reverseEndian( unpack32(mbuf, 16) );
	u32 tmdSize     = reverseEndian( unpack32(mbuf, 20) );
	u32 contentSize = reverseEndian( unpack32(mbuf, 24) );
	u32 metaSize    = reverseEndian( unpack32(mbuf, 28) );

	printf( "hdrSize     %d\n", hdrSize );
	printf( "tadType     %c%c\n", tadType>>8, tadType&0xFF );
	printf( "tadVersion  %d\n", tadVersion );
	printf( "certSize    %d\n", certSize );
	printf( "crlSize     %d\n", crlSize );
	printf( "ticketSize  %d\n", ticketSize );
	printf( "tmdSize     %d\n", tmdSize );
	printf( "contentSize %d\n", contentSize );
	printf( "metaSize    %d\n", metaSize );

	u32 certOffset 		= roundUp( hdrSize, 64);
	u32 crlOffset  		= roundUp( certOffset 	 + certSize, 	64);
	u32 ticketOffset 	= roundUp( crlOffset 	 + crlSize, 	64);
	u32 tmdOffset  		= roundUp( ticketOffset  + ticketSize, 	64);
	u32 contentOffset	= roundUp( tmdOffset 	 + tmdSize, 	64);
	u32 metaOffset 		= roundUp( contentOffset + contentSize, 64);
	u32 fileSize		= roundUp( metaOffset 	 + metaSize, 	64);

	fseek( fp, 0, SEEK_END );
	u32 orgFileSize = ftell( fp );
	if( fileSize != orgFileSize )
	{
		printf( "file size is not expected size(=%d)", fileSize );
		fclose( fp );
		return -1;
	}
	cli::array<System::Byte> ^ticket  = subStr( fp, ticketOffset, ticketSize );
	cli::array<System::Byte> ^tmd     = subStr( fp, tmdOffset, tmdSize );
	cli::array<System::Byte> ^content = subStr( fp, contentOffset, contentSize );

	//saveFile( "cert.bin", subStr( fp, certOffset, certSize ) );
	//saveFile( "crl.bin", 	subStr( fp, crlOffset,  crlSize ) );
	//saveFile( "ticket.bin", ticket );
	//saveFile( "tmd.bin", 	tmd );
	//saveFile( "meta.bin",	subStr( fp, metaOffset, metaSize ) );

	cli::array<System::Byte>    ^title_key = readTitleKey( ticket );
	cli::array<rcContentsInfo^> ^rci = readContentsInfo( tmd );
	dumpBytes( title_key );

	// �ʏ�� tad �� srl (�R���e���c No.0) �����܂܂Ȃ���
	// �}���`�R���e���c ���܂ޏꍇ�̂��߂� No.1 �ȍ~���ʃt�@�C���Ƃ��ĕۑ�����
	// srl ���� out.srl �̂Ƃ� out_1.bin out_2.bin ... �Ƃ��ďo�͂���
	System::String ^srl_dir    = System::IO::Path::GetDirectoryName( srlpath );				// �i�[�f�B���N�g����
	System::String ^srl_prefix = System::IO::Path::GetFileNameWithoutExtension( srlpath );	// �g���q�����O�̃t�@�C����
	System::String ^srl_ext    = System::IO::Path::GetExtension( srlpath );					// �g���q

	int result = 0;
	u32 offset = 0;
	for each( rcContentsInfo ^ci in rci )
	{
		u32 size = roundUp( (u32)ci->size, 16 );
		cli::array<System::Byte> ^enc_content_x = subStr( content, offset, size );
		cli::array<System::Byte> ^content_x_iv  = resizeBytes( pack16( reverseEndian(ci->idx) ), 14 );	// �r�b�O�G���f�B�A���ɂ��Ă���
		cli::array<System::Byte> ^dec_content_x = decCBC( title_key, content_x_iv, enc_content_x );
		cli::array<System::Byte> ^dec_content   = subStr( dec_content_x, 0, ci->size );
		System::Security::Cryptography::SHA1 ^sha1 = gcnew System::Security::Cryptography::SHA1Managed();
		cli::array<System::Byte> ^hash = sha1->ComputeHash( dec_content );

		dumpBytes( hash );
		pin_ptr<unsigned char> calc = &hash[0];		// �v�Z�ŋ��߂��n�b�V��
		pin_ptr<unsigned char> extr = &ci->hash[0];	// ���o�����n�b�V��
		if( memcmp( calc, extr, 20 ) == 0 )
		{
			printf( "hash OK\n" );
		}
		else
		{
			printf( "hash mismatch\n" );
			result = -1;							// �G���[�Ƃ��� ���f�͂����Ō�܂ō쐬
		}

		//saveFile( "content_" + ci->idx.ToString() + ".encrypted.bin", enc_content_x );
		//saveFile( "content_" + ci->idx.ToString() + ".bin", dec_content );
		if( ci->idx == 0 )
		{
			saveFile( srlpath, dec_content );		// �R���e���c No.0 �� srl �ɂ�����
		}
		else
		{
			System::String ^tmppath = srl_dir + "\\" + srl_prefix + "_" + ci->idx.ToString() + ".bin";
			saveFile( tmppath, dec_content );
		}
		offset += roundUp( size, 64 );
	}
	fclose( fp );
	return result;
}

// ------------------------------------------------------
// internal functions
// ------------------------------------------------------

//
// title_key �̕���
//
// @ret title_key �̃o�C�g��
//
cli::array<System::Byte>^ readTitleKey( cli::array<System::Byte> ^ticket )
{
	cli::array<System::Byte> ^encTitleKey = subStr( ticket, 0x1BF, 16 );
	cli::array<System::Byte> ^IV = resizeBytes( subStr( ticket, 0x1DC, 8 ), 8 );	// 16�o�C�g�Ɋg�����ăP�c��8�o�C�g��0�Ŗ��߂�

	cli::array<System::Byte> ^comKey = gcnew cli::array<System::Byte>(16);
	pin_ptr<unsigned char> pComKey = &comKey[0];
	memcpy( pComKey, commonKey, 16 );

	cli::array<System::Byte> ^plain;
	try
	{
		plain = decCBC( comKey, IV, encTitleKey );
	}
	catch (System::Exception ^ e)
    {
		System::Console::WriteLine("Exception in readTitleKey(): {0}", e->Message);
    }
	return plain;
}

//
// tmd ����e�R���e���c�t�@�C���̏��𔲂��o��
//
// @ret �e�R���e���c�t�@�C���̏����܂Ƃ߂� Array
//
cli::array<rcContentsInfo^> ^readContentsInfo( cli::array<System::Byte> ^tmd )
{
	u16 nContent = reverseEndian( unpack16(tmd, 0x1DE) );
	cli::array<rcContentsInfo^> ^ci = gcnew cli::array<rcContentsInfo^>( nContent );

	u16 i;
	for( i=0; i < nContent; i++ )
	{
		u32 offset = 0x1E4 + 36*i;
		ci[i] = gcnew rcContentsInfo;
		ci[i]->cid  = reverseEndian( unpack32(tmd, offset) );
		ci[i]->idx  = reverseEndian( unpack16(tmd, offset + 4) );
		ci[i]->type = reverseEndian( unpack16(tmd, offset + 6) );
		ci[i]->size = reverseEndian( unpack32(tmd, offset + 12) );
		ci[i]->hash = subStr( tmd, offset + 16, 20 );
	}
	return ci;
}

//
// AES���� : System::Security::Cryptography::RijndaelManaged �̃w���v�̃T���v�����R�s�y
//
// @ret ������̃f�[�^
//
cli::array<System::Byte>^ decCBC( cli::array<System::Byte> ^ Key, cli::array<System::Byte> ^ IV, cli::array<System::Byte> ^cipherText  )
{
    // Check arguments.
    if (!cipherText || cipherText->Length <= 0)
		throw gcnew System::ArgumentNullException("cipherText");
    if (!Key || Key->Length <= 0)
        throw gcnew System::ArgumentNullException("Key");
    if (!IV || IV->Length <= 0)
        throw gcnew System::ArgumentNullException("Key");

    // TDeclare the streams used
    // to decrypt to an in memory
    // array of bytes.
	System::IO::MemoryStream ^msDecrypt;
	System::Security::Cryptography::CryptoStream ^csDecrypt;

    // Declare the RijndaelManaged object
    // used to decrypt the data.
    System::Security::Cryptography::RijndaelManaged ^aesAlg;

    // Declare the string used to hold
    // the decrypted text.
	cli::array<System::Byte> ^plain = gcnew cli::array<System::Byte>(cipherText->Length);

    try
    {
        // Create a RijndaelManaged object
        // with the specified key and IV.
        aesAlg = gcnew System::Security::Cryptography::RijndaelManaged();
		aesAlg->Mode = System::Security::Cryptography::CipherMode::CBC;		// CBC���[�h
        aesAlg->Key = Key;
        aesAlg->IV = IV;
		aesAlg->Padding = System::Security::Cryptography::PaddingMode::Zeros;

        // Create a decrytor to perform the stream transform.
		System::Security::Cryptography::ICryptoTransform ^ decryptor 
			= aesAlg->CreateDecryptor(aesAlg->Key, aesAlg->IV);

        // Create the streams used for decryption.
        msDecrypt = gcnew System::IO::MemoryStream(cipherText);
		csDecrypt = gcnew System::Security::Cryptography::CryptoStream
			(msDecrypt, decryptor, System::Security::Cryptography::CryptoStreamMode::Read);
		csDecrypt->Read( plain, 0, cipherText->Length );
        //srDecrypt = gcnew System::IO::StreamReader(csDecrypt);

        //// Read the decrypted bytes from the decrypting stream
        //// and place them in a string.
        //plaintext = srDecrypt->ReadToEnd();
    }
    finally
    {
        // Clean things up.

        // Close the streams.
        if (csDecrypt)
            csDecrypt->Close();
        if (msDecrypt)
            msDecrypt->Close();

        // Clear the RijndaelManaged object.
        if (aesAlg)
            aesAlg->Clear();
    }
    return plain;
}

// end of file
