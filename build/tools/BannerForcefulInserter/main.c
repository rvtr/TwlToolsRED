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
    FILE *banner_fp;
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
    printf( "Usage: RatingAllFreeTool.exe input_file output_file banner_file\n" );
    printf( "  input_file  : a ROM data file.\n" );
    printf( "  output_file : a destination file.\n" );
    printf( "  banner_file : a banner file.\n" );
    printf( "\nOption:\n" );
    printf( "-h    : print help only.\n" );
    printf( "-f    : force to overwrite a output_file.\n" );
    printf( "-c    : check whether the banner exists in the input_file. [EXIST/NOTEXIST]\n" );
	printf( "-----------------------------------------------------------------------------\n" );
}

int main(int argc, char *argv[])
{
    SContext        context;
    char           *pSrc, *pDst, *pBanner;
    int             opt;
    extern char    *optarg;
    extern int      optind, opterr;
    struct stat     st;
    BOOL            bResult = TRUE;
    BOOL            bForceOverwrite = FALSE;
    BOOL            bCheck = FALSE;
    
    printf( "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n" );
    printf( "         ManuSkipFlagTool [%s-%s]\n", SDK_REVISION, IPL_REVISION );
    printf( "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n" );

    // context の初期化
    memset( &context, 0, sizeof(SContext) );

    // オプション
    while( (opt = getopt(argc, argv, "hfc")) >= 0 )
    {
        switch( opt )
        {
            case 'h':
                usage();
                return 0;
            break;

            case 'f':
                bForceOverwrite = TRUE;
            break;

            case 'c':
                bCheck = TRUE;
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
    pBanner = argv[2];

    printf( "Auguments:\n" );
    printf( "  input_file  : %s\n", pSrc );
    printf( "  output_file : %s\n", pDst );
    printf( "  banner_file : %s\n", pBanner );
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
    if( !bCheck && !bForceOverwrite && stat( pDst, &st )==0 )
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

    // チェックモード
    if( bCheck )
    {
        FILE *fp = fopen(pSrc, "rb");
        ROM_Header rh;
        
        if( sizeof(ROM_Header) != fread( &rh, 1, sizeof(ROM_Header), fp) )
        {
            printf( "\n*** Error: Failed to read the ROM Header. ***\n" );
            return -1;
        }
        
        // オフセットが設定されているときバナーが存在するとみなす
        if( rh.s.banner_offset != 0 )
        {
            printf("EXIST");
        }
        else
        {
            printf("NOTEXIST");
        }
        return 0;
    }

    // ファイルを開く
    context.ifp = fopen(pSrc, "rb");
    context.ofp = fopen(pDst, "wb+");
    context.banner_fp = fopen(pBanner, "rb");
    if( !(context.ifp) || !(context.ofp) || !(context.banner_fp) )
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
    ROM_Header rh;
    int        banner_size;
    u8        *banner_buf;
    u8        *tmp;
    u16        curr_crc;
    
    // ROMヘッダをリード
    fseek( pContext->ifp, 0, SEEK_SET );
    if( sizeof(ROM_Header) != fread( &rh, 1, sizeof(ROM_Header), pContext->ifp ) )
    {
        printf( "\n*** Error: Failed to read the ROM Header. ***\n" );
        return FALSE;
    }

    // NTRのみ許可
    if( rh.s.platform_code != PLATFORM_CODE_NTR )
    {
        printf("\n*** Error: Illegal platform (%d). ***\n", rh.s.platform_code);
        return FALSE;
    }

    // バナーが存在するときダメ
    if( rh.s.banner_offset != 0 )
    {
        printf("\n*** Error: The banner has already exists (offset = 0x%08X). ***\n", (unsigned int)rh.s.banner_offset);
        return FALSE;
    }
    printf("InitialCode      : %c%c%c%c\n", rh.s.game_code[0], rh.s.game_code[1], rh.s.game_code[2], rh.s.game_code[3]);
    
    // バナーを読み込む
    fseek(pContext->banner_fp, 0, SEEK_END);
    banner_size = ftell(pContext->banner_fp);
    banner_buf = (u8*)malloc(banner_size);
    if( !banner_buf )
    {
        printf("\n*** Error: Failed to malloc. ***\n");
        return FALSE;
    }
    fseek( pContext->banner_fp, 0, SEEK_SET );
    if( banner_size != fread( banner_buf, 1, banner_size, pContext->banner_fp ) )
    {
        printf( "\n*** Error: Failed to read the Banner file. ***\n" );
        free(banner_buf);
        return FALSE;
    }

    // バナーをホワイトリストハッシュ以降の0x3C0からの領域に入れる(最大で0xDFFまでなので署名よりも前の領域に収まる)
    tmp = (u8*)&rh;
    memcpy( &tmp[0x3C0], banner_buf, banner_size );
    rh.s.banner_offset = 0x3C0;     // オフセットを上書き
    free(banner_buf);

    // ヘッダCRC計算
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
