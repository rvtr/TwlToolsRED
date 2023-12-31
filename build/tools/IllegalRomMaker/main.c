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


 宣言 & 定数


 *---------------------------------------------------------------------------*/

// -- 内部パラメータ

// CRC (makerom.TWL/misc.cとmakerom.TWL/format_rom_private.hからコピペ)
#define  CRC16_INIT_VALUE    (0xffff)
#define  CALC_CRC16_SIZE     (0x15e)

// -- Context

typedef struct _SContext
{
    // 入出力ファイル
    FILE *ifp;
    FILE *ofp;
    
    // オプション
    BOOL isBitEnable;       // ビット上げモード
    BOOL isBitDisable;      // ビット下げモード
    BOOL isByteClear;       // バイトクリアモード
    BOOL isByteAssign;      // バイト代入モード
    BOOL isMultiBitEnable;  // 複数ビット上げモード
    BOOL isMultiBitDisable; // 複数ビット下げモード
    
    // 引数
    u32  byteAddr;      // 不正挿入バイトアドレス
    u32  bitAddr;       // 不正挿入ビットアドレス(オプション)
    u32  assignValue;   // 代入値(オプション)
    u32  bitMask;

    // 不正値を入れる箇所を何バイトとして扱うか
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


 グローバル変数


 *---------------------------------------------------------------------------*/

BOOL DebugMode = FALSE;

/*---------------------------------------------------------------------------*


 おおもとの本体


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

    // context の初期化
    memset( &context, 0, sizeof(SContext) );

    // デフォルトでは不正値を入れる箇所を1バイトとして扱う
    context.targetSize = 1;

    // オプション
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

            default:            // オプション引数が指定されていないときにも実行される
                usage();
                fprintf( stdout, "\n*** Error: illegal option. ***\n" );
                exit(1);
            break;
        }
    }
    argc = argc - optind;
    argv = argv + optind;

    // 引数処理
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

    // 入力ファイルと出力ファイル名が同じならダメ
    if( (strlen(pSrc) == strlen(pDst)) && (memcmp(pSrc, pDst, strlen(pSrc)) == 0) )
    {
        printf( "\n*** Error: Same filename between the input_file and the output_file. ***\n" );
        printf( "\n" );
        printf( "error.\n" );
        return -1;
    }

    // 出力ファイルが存在するとき上書きするかどうかを確認する
    if( !bForceOverwrite && stat( pDst, &st )==0 )
    {
        char  ans;
        printf( "The %s already exists. Overwrite it? (y or n) ", pDst );
        //fscanf( stdin, "%c", &ans );
        ans = (char)getch();    // リターンキーなし
        printf( "%c\n", ans );  // getch では入力が表示されないので自分で表示する必要あり
        printf( "\n" );
        if( !(ans=='y') && !(ans=='Y') )
        {
            printf( "\n" );
            printf( "canceled.\n" );
            return -1;
        }
    }

    // ファイルを開く
    context.ifp = fopen(pSrc, "rb");
    context.ofp = fopen(pDst, "wb+");
    if( !(context.ifp) || !(context.ofp) )
    {
        printf( "\n*** Error: Failed to open the file. ***\n" );
        bResult = FALSE;
        goto FINALIZE;
    }
    
    // ファイルコピー
    if( !CopyFile( context.ofp, context.ifp ) )
    {
        printf( "\n*** Error: Failed to copy the file. ***\n" );
        bResult = FALSE;
        goto FINALIZE;
    }
    
    // フラグ立てが指定されていないときはコピーだけしてそのまま終了
    bResult = iMain( &context );

// 終了処理
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
    if( context.ifp ) fclose(context.ifp);  // 資源の解放
    if( context.ofp ) fclose(context.ofp);

    if( (!bResult) && (stat( pDst, &st ) == 0) )
    {
        unlink( pDst );                 // 出力ファイルを削除する
    }
    return ((bResult == TRUE)?0:-1);    // 成功のとき0
}

/*---------------------------------------------------------------------------*

 フラグ立て本体

 *---------------------------------------------------------------------------*/

static BOOL iMain( SContext *pContext )
{
    u32         target = 0;     // 1,2,4バイトのいずれかを書き換えるので最大の4バイトにしておく
    
    // 指定されたバイトをリード
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

    // 変更したバイトを出力ファイルに書き出す
    fseek( pContext->ofp, pContext->byteAddr, SEEK_SET );
    if( pContext->targetSize != fwrite( &target, 1, pContext->targetSize, pContext->ofp ) )
    {
        printf( "\n*** Error: Failed to write the byte to output_fie. ***\n" );
        return FALSE;
    }
    
    // 変更したバイトがCRCの対象領域ならCRC再計算
    if( (0 <= pContext->byteAddr) && (pContext->byteAddr < 0x15e) )
    {
        ROM_Header rh;
        u16        curr_crc;

        // ROMヘッダをリード(変更は出力ファイルに加えられたので出力ファイルからリードする)
        fseek( pContext->ofp, 0, SEEK_SET );
        if( sizeof(ROM_Header) != fread( &rh, 1, sizeof(ROM_Header), pContext->ofp ) )
        {
            printf( "\n*** Error: Failed to read the ROM Header. ***\n" );
            return FALSE;
        }
        
        // ヘッダCRC再計算
        curr_crc = rh.s.header_crc16;
        rh.s.header_crc16 = CalcCRC16( CRC16_INIT_VALUE, (u8*)&rh, CALC_CRC16_SIZE );
        printf( "Header CRC       : 0x%04X -> 0x%04X\n", curr_crc, rh.s.header_crc16 );
        
        // ROMヘッダを上書き
        fseek( pContext->ofp, 0, SEEK_SET );
        if( sizeof(ROM_Header) != fwrite( &rh, 1, sizeof(ROM_Header), pContext->ofp ) )
        {
            printf( "\n*** Error: Failed to write the ROM Header. ***\n" );
            return FALSE;
        }
    }
    
    // 変更したバイトが署名の対象領域なら再署名
    if( (0 <= pContext->byteAddr) && (pContext->byteAddr < 0xe00) )
    {
        ROM_Header rh;

        // ROMヘッダをリード(変更は出力ファイルに加えられたので出力ファイルからリードする)
        fseek( pContext->ofp, 0, SEEK_SET );
        if( sizeof(ROM_Header) != fread( &rh, 1, sizeof(ROM_Header), pContext->ofp ) )
        {
            printf( "\n*** Error: Failed to read the ROM Header. ***\n" );
            return FALSE;
        }

        // 再署名
        if( !SignRomHeader( &rh ) )
        {
            printf( "Encrypt the sign : Failed\n" );
            printf("\n*** Error: Failed to encrypt the sigunature. ***\n");
            return FALSE;
        }
        printf( "Encrypt the sign : Succeeded\n" );

        // ROMヘッダを上書き
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

 ファイルコピー

 *---------------------------------------------------------------------------*/

// ファイルコピー用バッファ
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

 CRCの計算

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
        // 下位4bit
        r1 = crc16_table[total & 0xf];
        total = (total >> 4) & 0x0fff;
        total = total ^ r1 ^ crc16_table[*data & 0xf];

        // 上位4bit
        r1 = crc16_table[total & 0xf];
        total = (total >> 4) & 0x0fff;
        total = total ^ r1 ^ crc16_table[(*data >> 4) & 0xf];

        data++;
    }
    return total;
}


/*---------------------------------------------------------------------------*

 ROMヘッダの署名 (マスタエディタから移植)

 *---------------------------------------------------------------------------*/

BOOL SignRomHeader( ROM_Header *prh )
{
	SignatureData   signSrc;						// 署名のもととなるダイジェスト値
	u8              signDst[ RSA_KEY_LENGTH ];		// 署名の格納先Tmpバッファ
	u8              decryptBlock[ RSA_KEY_LENGTH ];	// 署名を解除後ブロックバッファ
	BOOL            result;
	int             pos;
    const u8       *privatekey = NULL;
    const u8       *publickey = NULL;
	u8             *idL = prh->s.titleID_Lo;
	u32             idH = prh->s.titleID_Hi;

	// ROMヘッダのダイジェストを算出(先頭から証明書領域の直前までが対象)
	ACSign_DigestUnit(
		signSrc.digest,
		prh,
		(u32)&(prh->certificate) - (u32)prh
	);

    // 鍵選択
	if( (idL[3]=='H') && (idL[2]=='N') && (idL[1]=='A') )	// ランチャアプリかどうかはTitleID_Loの値で決定
    {
        printf("Key type         : Launcher\n");
        privatekey = g_devPrivKey_DER_Launcher;
        publickey  = g_devPubKey_DER_Launcher;
    }
    else if( idH & TITLE_ID_HI_SECURE_FLAG_MASK )				// 各ビットは排他的とは限らないのでelse ifにはならない
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

	// ダイジェストに署名をかける
	result = ACSign_Encrypto( signDst, privatekey, &signSrc, sizeof(SignatureData) ); 
	if( !result )
	{
        printf( "\n*** Error: Failed to encrypt the signature. ***\n" );
		return FALSE;
	}

	// 署名を解除してダイジェストと一致するかベリファイする
	result = ACSign_Decrypto( decryptBlock, publickey, signDst, RSA_KEY_LENGTH );
	for( pos=0; pos < RSA_KEY_LENGTH; pos++ )
	{
		if( decryptBlock[pos] == 0x0 )			// 解除後ブロックから実データをサーチ
			break;
	}
	if( !result || (memcmp( &signSrc, &(decryptBlock[pos+1]), sizeof(SignatureData) ) != 0) )
	{
        printf( "\n*** Error: Failed to verify the signature. ***\n" );
		return FALSE;
	}

	// ROMヘッダに署名を上書き
	memcpy( prh->signature, signDst, RSA_KEY_LENGTH );

	return TRUE;
} // ECSrlResult RCSrl::signRomHeader(void)

// 署名を外す
BOOL DecryptSignRomHeader( ROM_Header *prh )
{
	u8     original[ RSA_KEY_LENGTH ];	// 署名外した後のデータ格納先
	s32    pos = 0;						// ブロックの先頭アドレス
	u8     digest[ DIGEST_SIZE_SHA1 ];	// ROMヘッダのダイジェスト
    const u8       *publickey = NULL;
	u8             *idL = prh->s.titleID_Lo;
	u32             idH = prh->s.titleID_Hi;

	// <データの流れ>
	// (1) 公開鍵で復号した結果(ブロック)をローカル変数(original)に格納
	// (2) ブロックから余分な部分を取り除いて引数(pDst)にコピー

    // 鍵選択
	if( (idL[3]=='H') && (idL[2]=='N') && (idL[1]=='A') )	// ランチャアプリかどうかはTitleID_Loの値で決定
    {
        publickey  = g_devPubKey_DER_Launcher;
    }
    else if( idH & TITLE_ID_HI_SECURE_FLAG_MASK )				// 各ビットは排他的とは限らないのでelse ifにはならない
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

	// 署名の解除 = 公開鍵で復号
	if( !ACSign_Decrypto( original, publickey, prh->signature, RSA_KEY_LENGTH ) )
	{
        printf( "\n*** Error: Failed to decrypt the signature. ***\n" );
		return FALSE;
	}
	// 署名前データを復号後ブロックからゲット
	for( pos=0; pos < (RSA_KEY_LENGTH-2); pos++ )   // 本来ブロックの先頭は0x00だが復号化の内部処理によって消える仕様
	{
		// 暗号ブロック形式 = 0x00, BlockType, Padding, 0x00, 実データ
		if( original[pos] == 0x00 )                               // 実データの直前の0x00をサーチ
		{
			break;
		}
	}
	// ベリファイ
	// ROMヘッダのダイジェストを算出(先頭から証明書領域の直前までが対象)
	ACSign_DigestUnit( digest,	prh, (u32)&(prh->certificate) - (u32)prh );
	if( memcmp( &(original[pos+1]), digest, DIGEST_SIZE_SHA1 ) != 0 )
	{
		return FALSE;
	}
	return TRUE;
}

// end of file
