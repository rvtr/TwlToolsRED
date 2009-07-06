/*---------------------------------------------------------------------------*
  Project:  TwlFirmware - tools - mastering
  File:     mastering.c

  Copyright 2003 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: #$
  $Rev$
  $Author$
 *---------------------------------------------------------------------------*/
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>                      // strcasecmp()
#include    <getopt.h>                      // getopt()
#include    <sys/stat.h>                    // stat()
#include    <stdarg.h>
#include    <conio.h>
#include    <twl/types.h>
#include    <twl/os/common/format_rom.h>
#include    <twl/os/common/ownerInfoEx.h>
#include    <acsign/include/acsign.h>
#include    "revision.h"
#include    "keys.h"


/*---------------------------------------------------------------------------*


 �錾 & �萔


 *---------------------------------------------------------------------------*/

// -- �����p�����[�^

// CRC (makerom.TWL/misc.c��makerom.TWL/format_rom_private.h����R�s�y)
#define  CRC16_INIT_VALUE    (0xffff)
#define  CALC_CRC16_SIZE     (0x15e)

// -- Context

typedef struct _SContext
{
    // ���o�̓t�@�C��
    FILE *ifp;
    FILE *ofp;
    
    // �I�v�V����
    BOOL isBitEnable;       // �r�b�g�グ���[�h
    BOOL isBitDisable;      // �r�b�g�������[�h
    BOOL isByteClear;       // �o�C�g�N���A���[�h
    BOOL isByteAssign;      // �o�C�g������[�h
    BOOL isMultiBitEnable;  // �����r�b�g�グ���[�h
    BOOL isMultiBitDisable; // �����r�b�g�������[�h
    
    // ����
    u32  byteAddr;      // �s���}���o�C�g�A�h���X
    u32  bitAddr;       // �s���}���r�b�g�A�h���X(�I�v�V����)
    u32  assignValue;   // ����l(�I�v�V����)
    u32  bitMask;

    // �s���l������ӏ������o�C�g�Ƃ��Ĉ�����
    u32  targetSize;
}
SContext;

// -- internal function

static BOOL iMain( SContext *pContext );
BOOL CopyFile( FILE *dst, FILE *src );
u16 CalcCRC16(u16 start, u8 *data, int size);
BOOL SignRomHeader( ROM_Header *prh );
BOOL DecryptSignRomHeader( ROM_Header *prh );

/*---------------------------------------------------------------------------*


 �O���[�o���ϐ�


 *---------------------------------------------------------------------------*/

BOOL DebugMode = FALSE;

/*---------------------------------------------------------------------------*


 �������Ƃ̖{��


 *---------------------------------------------------------------------------*/

void usage()
{
    printf( "-----------------------------------------------------------------------------\n" );
    printf( "Usage: RatingAllFreeTool.exe input_file output_file ByteAddr [option]\n" );
    printf( "  input_file  : a ROM data file.\n" );
    printf( "  output_file : a destination file.\n" );
    printf( "  ByteAddr    : an address of Byte which will be insert an illegal data. [00000000-FFFFFFFF]\n" );
    printf( "\nOption:\n" );
    printf( "-e bitAddr : enable the specific bit. [0-7/15/31]\n" );
    printf( "-d bitAddr : desable the specific bit. [0-7/15/31]\n" );
    printf( "-c         : clear the byte.\n" );
    printf( "-a value   : assign the value into the byte. [00-FF/FFFF/FFFFFFFF]\n" );
    printf( "-E bitMask : enable multipul bits. [00-FF/FFFF/FFFFFFFF]\n" );
    printf( "-D bitMask : diasble multipul bits. [00-FF/FFFF/FFFFFFFF]\n" );
    printf( "-2         : treat the target as 2 byte.\n" );
    printf( "-4         : treat the target as 4 byte.\n" );
    printf( "-h         : print help only.\n" );
    printf( "-f         : force to overwrite a output_file.\n" );
	printf( "-----------------------------------------------------------------------------\n" );
}

int main(int argc, char *argv[])
{
    SContext        context;
    char           *pSrc, *pDst;
    int             opt;
    extern char    *optarg;
    extern int      optind, opterr;
    struct stat     st;
    char           *errptr;
    BOOL            bResult = TRUE;
    BOOL            bForceOverwrite = FALSE;
    
    printf( "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n" );
    printf( "         IllegalRomMaker [%s-%s]\n", SDK_REVISION, IPL_REVISION );
    printf( "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n" );

    // context �̏�����
    memset( &context, 0, sizeof(SContext) );

    // �f�t�H���g�ł͕s���l������ӏ���1�o�C�g�Ƃ��Ĉ���
    context.targetSize = 1;

    // �I�v�V����
    while( (opt = getopt(argc, argv, "e:d:ca:E:D:24hf")) >= 0 )
    {
        switch( opt )
        {
            case 'e':
                context.isBitEnable = TRUE;
                context.bitAddr = atoi(optarg);
            break;

            case 'd':
                context.isBitDisable = TRUE;
                context.bitAddr = atoi(optarg);
            break;

            case 'c':
                context.isByteClear = TRUE;
            break;

            case 'a':
                context.isByteAssign = TRUE;
                context.assignValue = strtol(optarg, &errptr, 16);
            break;

            case 'E':
                context.isMultiBitEnable = TRUE;
                context.bitMask = strtol(optarg, &errptr, 16);
            break;

            case 'D':
                context.isMultiBitDisable = TRUE;
                context.bitMask = strtol(optarg, &errptr, 16);
            break;

            case '2':
                context.targetSize = 2;
            break;

            case '4':
                context.targetSize = 4;
            break;

            case 'h':
                usage();
                return 0;
            break;

            case 'f':
                bForceOverwrite = TRUE;
            break;

            default:            // �I�v�V�����������w�肳��Ă��Ȃ��Ƃ��ɂ����s�����
                usage();
                fprintf( stdout, "\n*** Error: illegal option. ***\n" );
                exit(1);
            break;
        }
    }
    argc = argc - optind;
    argv = argv + optind;

    // ��������
    if( argc != 3 )
    {
        usage();
        printf( "\n*** Error: Auguments. ***\n" );
        return -1;
    }
    pSrc = argv[0];
    pDst = argv[1];
    context.byteAddr = strtol(argv[2], &errptr, 16);

    printf( "Auguments:\n" );
    printf( "  input_file  : %s\n", pSrc );
    printf( "  output_file : %s\n", pDst );
    printf( "--\n" );

    // ���̓t�@�C���Əo�̓t�@�C�����������Ȃ�_��
    if( (strlen(pSrc) == strlen(pDst)) && (memcmp(pSrc, pDst, strlen(pSrc)) == 0) )
    {
        printf( "\n*** Error: Same filename between the input_file and the output_file. ***\n" );
        printf( "\n" );
        printf( "error.\n" );
        return -1;
    }

    // �o�̓t�@�C�������݂���Ƃ��㏑�����邩�ǂ������m�F����
    if( !bForceOverwrite && stat( pDst, &st )==0 )
    {
        char  ans;
        printf( "The %s already exists. Overwrite it? (y or n) ", pDst );
        //fscanf( stdin, "%c", &ans );
        ans = (char)getch();    // ���^�[���L�[�Ȃ�
        printf( "%c\n", ans );  // getch �ł͓��͂��\������Ȃ��̂Ŏ����ŕ\������K�v����
        printf( "\n" );
        if( !(ans=='y') && !(ans=='Y') )
        {
            printf( "\n" );
            printf( "canceled.\n" );
            return -1;
        }
    }

    // �t�@�C�����J��
    context.ifp = fopen(pSrc, "rb");
    context.ofp = fopen(pDst, "wb+");
    if( !(context.ifp) || !(context.ofp) )
    {
        printf( "\n*** Error: Failed to open the file. ***\n" );
        bResult = FALSE;
        goto FINALIZE;
    }
    
    // �t�@�C���R�s�[
    if( !CopyFile( context.ofp, context.ifp ) )
    {
        printf( "\n*** Error: Failed to copy the file. ***\n" );
        bResult = FALSE;
        goto FINALIZE;
    }
    
    // �t���O���Ă��w�肳��Ă��Ȃ��Ƃ��̓R�s�[�������Ă��̂܂܏I��
    bResult = iMain( &context );

// �I������
FINALIZE:

    if( !bResult )
    {
        printf( "\n" );
        printf( "error.\n" );
    }
    else
    {
        printf( "\n" );
        printf( "succeeded.\n" );
    }
    if( context.ifp ) fclose(context.ifp);  // �����̉��
    if( context.ofp ) fclose(context.ofp);

    if( (!bResult) && (stat( pDst, &st ) == 0) )
    {
        unlink( pDst );                 // �o�̓t�@�C�����폜����
    }
    return ((bResult == TRUE)?0:-1);    // �����̂Ƃ�0
}

/*---------------------------------------------------------------------------*

 �t���O���Ė{��

 *---------------------------------------------------------------------------*/

static BOOL iMain( SContext *pContext )
{
    u32         target = 0;     // 1,2,4�o�C�g�̂����ꂩ������������̂ōő��4�o�C�g�ɂ��Ă���
    
    // �w�肳�ꂽ�o�C�g�����[�h
    printf("Byte Address     : 0x%X\n", (unsigned int)pContext->byteAddr);
    fseek( pContext->ifp, pContext->byteAddr, SEEK_SET );
    if( pContext->targetSize != fread( &target, 1, pContext->targetSize, pContext->ifp ) )
    {
        printf( "\n*** Error: Failed to read the byte from input_file. ***\n" );
        return FALSE;
    }

    if( pContext->isBitEnable )
    {
        u32 mask = 1 << pContext->bitAddr;
        u32 old  = target;
        target |= mask;
        printf("Enable bit %2d    : 0x%08X -> 0x%08X\n", (int)pContext->bitAddr, (unsigned int)old, (unsigned int)target);
    }
    if( pContext->isBitDisable )
    {
        u32 mask = 1 << pContext->bitAddr;
        u32 old  = target;
        target &= ~mask;
        printf("Disable bit %2d   : 0x%08X -> 0x%08X\n", (int)pContext->bitAddr, (unsigned int)old, (unsigned int)target);
    }
    if( pContext->isMultiBitEnable )
    {
        u32 old = target;
        target |= pContext->bitMask;
        printf("Enable bits      : 0x%08X -> 0x%08X\n", (unsigned int)old, (unsigned int)target);
    }
    if( pContext->isMultiBitDisable )
    {
        u32 old = target;
        target &= ~pContext->bitMask;
        printf("Disable bits     : 0x%08X -> 0x%08X\n", (unsigned int)old, (unsigned int)target);
    }
    if( pContext->isByteClear )
    {
        u32 old  = target;
        target = 0;
        printf("Clear byte       : 0x%08X -> 0x%08X\n", (unsigned int)old, (unsigned int)target);
    }
    if( pContext->isByteAssign )
    {
        printf("Assign           : 0x%08X -> 0x%08X\n", (unsigned int)target, (unsigned int)pContext->assignValue);
        target = pContext->assignValue;
    }

    // �ύX�����o�C�g���o�̓t�@�C���ɏ����o��
    fseek( pContext->ofp, pContext->byteAddr, SEEK_SET );
    if( pContext->targetSize != fwrite( &target, 1, pContext->targetSize, pContext->ofp ) )
    {
        printf( "\n*** Error: Failed to write the byte to output_fie. ***\n" );
        return FALSE;
    }
    
    // �ύX�����o�C�g��CRC�̑Ώۗ̈�Ȃ�CRC�Čv�Z
    if( (0 <= pContext->byteAddr) && (pContext->byteAddr < 0x15e) )
    {
        ROM_Header rh;
        u16        curr_crc;

        // ROM�w�b�_�����[�h(�ύX�͏o�̓t�@�C���ɉ�����ꂽ�̂ŏo�̓t�@�C�����烊�[�h����)
        fseek( pContext->ofp, 0, SEEK_SET );
        if( sizeof(ROM_Header) != fread( &rh, 1, sizeof(ROM_Header), pContext->ofp ) )
        {
            printf( "\n*** Error: Failed to read the ROM Header. ***\n" );
            return FALSE;
        }
        
        // �w�b�_CRC�Čv�Z
        curr_crc = rh.s.header_crc16;
        rh.s.header_crc16 = CalcCRC16( CRC16_INIT_VALUE, (u8*)&rh, CALC_CRC16_SIZE );
        printf( "Header CRC       : 0x%04X -> 0x%04X\n", curr_crc, rh.s.header_crc16 );
        
        // ROM�w�b�_���㏑��
        fseek( pContext->ofp, 0, SEEK_SET );
        if( sizeof(ROM_Header) != fwrite( &rh, 1, sizeof(ROM_Header), pContext->ofp ) )
        {
            printf( "\n*** Error: Failed to write the ROM Header. ***\n" );
            return FALSE;
        }
    }
    
    // �ύX�����o�C�g�������̑Ώۗ̈�Ȃ�ď���
    if( (0 <= pContext->byteAddr) && (pContext->byteAddr < 0xe00) )
    {
        ROM_Header rh;

        // ROM�w�b�_�����[�h(�ύX�͏o�̓t�@�C���ɉ�����ꂽ�̂ŏo�̓t�@�C�����烊�[�h����)
        fseek( pContext->ofp, 0, SEEK_SET );
        if( sizeof(ROM_Header) != fread( &rh, 1, sizeof(ROM_Header), pContext->ofp ) )
        {
            printf( "\n*** Error: Failed to read the ROM Header. ***\n" );
            return FALSE;
        }

        // �ď���
        if( !SignRomHeader( &rh ) )
        {
            printf( "Encrypt the sign : Failed\n" );
            printf("\n*** Error: Failed to encrypt the sigunature. ***\n");
            return FALSE;
        }
        printf( "Encrypt the sign : Succeeded\n" );

        // ROM�w�b�_���㏑��
        fseek( pContext->ofp, 0, SEEK_SET );
        if( sizeof(ROM_Header) != fwrite( &rh, 1, sizeof(ROM_Header), pContext->ofp ) )
        {
            printf( "\n*** Error: Failed to write the ROM Header. ***\n" );
            return FALSE;
        }
    }
    return TRUE;
}


/*---------------------------------------------------------------------------*

 �t�@�C���R�s�[

 *---------------------------------------------------------------------------*/

// �t�@�C���R�s�[�p�o�b�t�@
#define  BUFSIZE             (16*1024)

BOOL CopyFile( FILE *dst, FILE *src )
{
    static u8  buf[ BUFSIZE ];
    int        size;

    fseek( src, 0, SEEK_END );
    size = ftell( src );
    fseek( src, 0, SEEK_SET );
    fseek( dst, 0, SEEK_SET );
    while( size > 0 )
    {
        int len = (BUFSIZE < size)?BUFSIZE:size;
        
        if( len != fread( buf, 1, len, src ) )
        {
            printf( "\n*** Error! failed to read the file in copying. ***\n" );
            return FALSE;
        }
        if( len != fwrite( buf, 1, len, dst ) )
        {
            printf( "\n*** Error! failed to write the file in copying. ***\n" );
            return FALSE;
        }
        
        size -= len;
    }
    return TRUE;
}


/*---------------------------------------------------------------------------*

 CRC�̌v�Z

 *---------------------------------------------------------------------------*/

static u16 crc16_table[16] = {
    0x0000, 0xCC01, 0xD801, 0x1400,
    0xF001, 0x3C00, 0x2800, 0xE401,
    0xA001, 0x6C00, 0x7800, 0xB401,
    0x5000, 0x9C01, 0x8801, 0x4400
};

u16 CalcCRC16(u16 start, u8 *data, int size)
{
    u16     r1;
    u16     total = start;

    while (size-- > 0)
    {
        // ����4bit
        r1 = crc16_table[total & 0xf];
        total = (total >> 4) & 0x0fff;
        total = total ^ r1 ^ crc16_table[*data & 0xf];

        // ���4bit
        r1 = crc16_table[total & 0xf];
        total = (total >> 4) & 0x0fff;
        total = total ^ r1 ^ crc16_table[(*data >> 4) & 0xf];

        data++;
    }
    return total;
}


/*---------------------------------------------------------------------------*

 ROM�w�b�_�̏��� (�}�X�^�G�f�B�^����ڐA)

 *---------------------------------------------------------------------------*/

BOOL SignRomHeader( ROM_Header *prh )
{
	SignatureData   signSrc;						// �����̂��ƂƂȂ�_�C�W�F�X�g�l
	u8              signDst[ RSA_KEY_LENGTH ];		// �����̊i�[��Tmp�o�b�t�@
	u8              decryptBlock[ RSA_KEY_LENGTH ];	// ������������u���b�N�o�b�t�@
	BOOL            result;
	int             pos;
    const u8       *privatekey = NULL;
    const u8       *publickey = NULL;
	u8             *idL = prh->s.titleID_Lo;
	u32             idH = prh->s.titleID_Hi;

	// ROM�w�b�_�̃_�C�W�F�X�g���Z�o(�擪����ؖ����̈�̒��O�܂ł��Ώ�)
	ACSign_DigestUnit(
		signSrc.digest,
		prh,
		(u32)&(prh->certificate) - (u32)prh
	);

    // ���I��
	if( (idL[3]=='H') && (idL[2]=='N') && (idL[1]=='A') )	// �����`���A�v�����ǂ�����TitleID_Lo�̒l�Ō���
    {
        printf("Key type         : Launcher\n");
        privatekey = g_devPrivKey_DER_Launcher;
        publickey  = g_devPubKey_DER_Launcher;
    }
    else if( idH & TITLE_ID_HI_SECURE_FLAG_MASK )				// �e�r�b�g�͔r���I�Ƃ͌���Ȃ��̂�else if�ɂ͂Ȃ�Ȃ�
    {
        printf("Key type         : Secure\n");
        privatekey = g_devPrivKey_DER_Secure;
        publickey  = g_devPubKey_DER_Secure;
    }
    else if( (idH & TITLE_ID_HI_APP_TYPE_MASK) == 1 )
    {
        printf("Key type         : System\n");
        privatekey = g_devPrivKey_DER_System;
        publickey  = g_devPubKey_DER_System;
    }
    else if( (idH & TITLE_ID_HI_APP_TYPE_MASK) == 0 )
    {
        printf("Key type         : User\n");
        privatekey = g_devPrivKey_DER_User;
        publickey  = g_devPubKey_DER_User;
    }
    if( !privatekey || !publickey )
    {
        printf( "\n*** Error: Illegal key. ***\n" );
        return FALSE;
    }

	// �_�C�W�F�X�g�ɏ�����������
	result = ACSign_Encrypto( signDst, privatekey, &signSrc, sizeof(SignatureData) ); 
	if( !result )
	{
        printf( "\n*** Error: Failed to encrypt the signature. ***\n" );
		return FALSE;
	}

	// �������������ă_�C�W�F�X�g�ƈ�v���邩�x���t�@�C����
	result = ACSign_Decrypto( decryptBlock, publickey, signDst, RSA_KEY_LENGTH );
	for( pos=0; pos < RSA_KEY_LENGTH; pos++ )
	{
		if( decryptBlock[pos] == 0x0 )			// ������u���b�N������f�[�^���T�[�`
			break;
	}
	if( !result || (memcmp( &signSrc, &(decryptBlock[pos+1]), sizeof(SignatureData) ) != 0) )
	{
        printf( "\n*** Error: Failed to verify the signature. ***\n" );
		return FALSE;
	}

	// ROM�w�b�_�ɏ������㏑��
	memcpy( prh->signature, signDst, RSA_KEY_LENGTH );

	return TRUE;
} // ECSrlResult RCSrl::signRomHeader(void)

// �������O��
BOOL DecryptSignRomHeader( ROM_Header *prh )
{
	u8     original[ RSA_KEY_LENGTH ];	// �����O������̃f�[�^�i�[��
	s32    pos = 0;						// �u���b�N�̐擪�A�h���X
	u8     digest[ DIGEST_SIZE_SHA1 ];	// ROM�w�b�_�̃_�C�W�F�X�g
    const u8       *publickey = NULL;
	u8             *idL = prh->s.titleID_Lo;
	u32             idH = prh->s.titleID_Hi;

	// <�f�[�^�̗���>
	// (1) ���J���ŕ�����������(�u���b�N)�����[�J���ϐ�(original)�Ɋi�[
	// (2) �u���b�N����]���ȕ�������菜���Ĉ���(pDst)�ɃR�s�[

    // ���I��
	if( (idL[3]=='H') && (idL[2]=='N') && (idL[1]=='A') )	// �����`���A�v�����ǂ�����TitleID_Lo�̒l�Ō���
    {
        publickey  = g_devPubKey_DER_Launcher;
    }
    else if( idH & TITLE_ID_HI_SECURE_FLAG_MASK )				// �e�r�b�g�͔r���I�Ƃ͌���Ȃ��̂�else if�ɂ͂Ȃ�Ȃ�
    {
        publickey  = g_devPubKey_DER_Secure;
    }
    else if( (idH & TITLE_ID_HI_APP_TYPE_MASK) == 1 )
    {
        publickey  = g_devPubKey_DER_System;
    }
    else if( (idH & TITLE_ID_HI_APP_TYPE_MASK) == 0 )
    {
        publickey  = g_devPubKey_DER_User;
    }
    if( !publickey )
    {
        printf( "\n*** Error: Illegal key. ***\n" );
        return FALSE;
    }

	// �����̉��� = ���J���ŕ���
	if( !ACSign_Decrypto( original, publickey, prh->signature, RSA_KEY_LENGTH ) )
	{
        printf( "\n*** Error: Failed to decrypt the signature. ***\n" );
		return FALSE;
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
	ACSign_DigestUnit( digest,	prh, (u32)&(prh->certificate) - (u32)prh );
	if( memcmp( &(original[pos+1]), digest, DIGEST_SIZE_SHA1 ) != 0 )
	{
		return FALSE;
	}
	return TRUE;
}

// end of file