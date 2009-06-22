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
#include    <acsign/include/acsign.h>
#include    "revision.h"
#include    "keys.h"
#include    "verify_area.h"


/*---------------------------------------------------------------------------*


 宣言 & 定数


 *---------------------------------------------------------------------------*/

// -- 内部パラメータ

// メニュースキップフラグのマスク
#define  MASK_MENUSKIP_FLAG     (0x04)

// CRC (makerom.TWL/misc.cとmakerom.TWL/format_rom_private.hからコピペ)
#define  CRC16_INIT_VALUE    (0xffff)
#define  CALC_CRC16_SIZE     (0x15e)

// -- Context

typedef struct _SContext
{
    // オプション
    BOOL isDisable;     // フラグを下ろすモード

    // 入出力ファイル
    FILE *before_fp;
    FILE *after_fp;
}
SContext;

// -- internal function

static BOOL iCheckFlag( SContext *pContext );
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
    printf( "Usage: MenuSkipFlagTool.exe input_file output_file enable-flag\n" );
    printf( "  before_file : input_file of MenuSkipFlagTool.exe.\n" );
    printf( "  after_file  : output_file of MenuSkipFlagTool.exe.\n" );
    printf( "  enable-flag : 1 or 0. When 1 has set in MenuSkipFlagTool.exe, set 1.\n" );
    printf( "\nOption:\n" );
    printf( "-h    : print help only.\n" );
    printf( "-f    : force to overwrite a output_file.\n" );
    printf( "-d    : considering that the menu-skip flag is disable in MenuSkipFlatTool.exe.\n" );
	printf( "-----------------------------------------------------------------------------\n" );
}

int main(int argc, char *argv[])
{
    SContext        context;
    char           *pSrc, *pDst;
    int             opt;
    extern char    *optarg;
    extern int      optind, opterr;
//    struct stat     st;
    BOOL            bResult = TRUE;
    BOOL            bEnableFlag = FALSE;        // フラグを立てるかどうか
    ROM_Header      rh;
    u32             filesize;
    
    printf( "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n" );
    printf( "         ManuSkipFlagTool_Checker [%s-%s]\n", SDK_REVISION, IPL_REVISION );
    printf( "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n" );

    // context の初期化
    memset( &context, 0, sizeof(SContext) );

    // オプション
    while( (opt = getopt(argc, argv, "hd")) >= 0 )
    {
        switch( opt )
        {
            case 'h':
                usage();
                return 0;
            break;

            case 'd':
                context.isDisable = TRUE;
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
    // フラグを立てるかどうかの指定がおかしいときエラー
    if( (strcmp(argv[2], "0") != 0) && (strcmp(argv[2], "1") != 0) )
    {
        printf( "\n*** Error: The enable-flag must be either 1 or 0. ***\n" );
        return -1;
    }

    pSrc = argv[0];
    pDst = argv[1];
    bEnableFlag = atoi(argv[2]);

    printf( "Auguments:\n" );
    printf( "  input_file  : %s\n", pSrc );
    printf( "  output_file : %s\n", pDst );
    printf( "  enable-flag : %d (%s)\n", bEnableFlag, (bEnableFlag)?"Enable":"Not enable" );
    printf( "--\n" );

    // ファイルを開く
    context.before_fp = fopen(pSrc, "rb");
    context.after_fp = fopen(pDst, "rb");    // 両方 read only
    if( !(context.before_fp) || !(context.after_fp) )
    {
        printf( "\n*** Error: Failed to open the file. ***\n" );
        bResult = FALSE;
        goto FINALIZE;
    }
    
    // SRLかどうかのチェック
    fseek( context.after_fp, 0, SEEK_SET );
    if( sizeof(ROM_Header) != fread( &rh, 1, sizeof(ROM_Header), context.after_fp ) )
    {
        printf( "\n*** Error: Failed to read the ROM Header. ***\n" );
        bResult = FALSE;
        goto FINALIZE;
    }
    
    // ファイルサイズが一致するかチェック
    fseek( context.before_fp, 0, SEEK_END );
    fseek( context.after_fp, 0, SEEK_END );
    filesize = ftell(context.before_fp);
    if( filesize != ftell(context.after_fp) )
    {
        printf( "\n*** Error: The size of files are different.\n" );
        bResult = FALSE;
        goto FINALIZE;
    }
    
    // フラグ立てが指定されていないときは全領域のチェック
    bResult = TRUE;
    if( bEnableFlag )
    {
        bResult = iCheckFlag( &context );
    }
    else
    {
        // 全領域のチェック
        bResult = VerifyFiles( context.before_fp, context.after_fp, 0, filesize );
    }

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
    if( context.before_fp ) fclose(context.before_fp);  // 資源の解放
    if( context.after_fp ) fclose(context.after_fp);

    return ((bResult == TRUE)?0:-1);    // 成功のとき0
}

/*---------------------------------------------------------------------------*

 フラグ立て本体

 *---------------------------------------------------------------------------*/

static BOOL iCheckFlag( SContext *pContext )
{
    ROM_Header after_rh, before_rh;
    u16        h_crc;
    
    // ROMヘッダをリード
    fseek( pContext->after_fp, 0, SEEK_SET );
    if( sizeof(ROM_Header) != fread( &after_rh, 1, sizeof(ROM_Header), pContext->after_fp ) )
    {
        printf( "\n*** Error: Failed to read the ROM Header. ***\n" );
        return FALSE;
    }
    fseek( pContext->before_fp, 0, SEEK_SET );
    if( sizeof(ROM_Header) != fread( &before_rh, 1, sizeof(ROM_Header), pContext->before_fp ) )
    {
        printf( "\n*** Error: Failed to read the ROM Header. ***\n" );
        return FALSE;
    }

    // CRC のチェック
    h_crc = CalcCRC16( CRC16_INIT_VALUE, (u8*)&after_rh, CALC_CRC16_SIZE );
    if( h_crc != after_rh.s.header_crc16 )
    {
        printf( "\n*** Error: Invalid header CRC. ***\n" );
        return FALSE;
    }

    // 不正なプラットフォーム
    if( (after_rh.s.platform_code != PLATFORM_CODE_NTR) &&
        (after_rh.s.platform_code != PLATFORM_CODE_TWL_HYBLID) &&
        (after_rh.s.platform_code != PLATFORM_CODE_TWL_LIMITED) )
    {
        printf("\n*** Error: Illegal platform (%d). ***\n", after_rh.s.platform_code);
        return FALSE;
    }

    printf("InitialCode      : %c%c%c%c\n", after_rh.s.game_code[0], after_rh.s.game_code[1], after_rh.s.game_code[2], after_rh.s.game_code[3]);

    // フラグが立っているかチェック
    printf( "Menu-skip flag   : %s\n", (after_rh.s.inspect_card==0)?"Disable":"Enable");
    if( pContext->isDisable )
    {
        // Disableモードのときはフラグが下りているかをチェック
        if( after_rh.s.inspect_card != 0 )
        {
            printf("\n*** Error: Menu-skip flag is enabled. ***\n");
            return FALSE;
        }
    }
    else
    {
        if( after_rh.s.inspect_card == 0 )
        {
            printf("\n*** Error: Menu-skip flag is disabled. ***\n");
            return FALSE;
        }
    }
    
    // フラグ以外のビットが変更されていないかチェック
    {
        u8 *ap = (u8*)&after_rh;
        u8 *bp = (u8*)&before_rh;
        if( (ap[0x1f] & ~MASK_MENUSKIP_FLAG) != (bp[0x1f] & ~MASK_MENUSKIP_FLAG) )
        {
            printf("\n*** Error: Other bit is rewrited. ***\n");
            return FALSE;
        }
    }

    // TWLとNTRとで分岐
    if( (after_rh.s.platform_code == 0) && (after_rh.s.enable_signature == 0) )    // enable_signature が立っている特殊なアプリはTWLとみなす (PictoChat/DS-download-play)
    {
        printf( "Platform(TWL/NTR): 0x%02X (NTR)\n", after_rh.s.platform_code );

        // 変更される箇所以外をべリファイ
        {
            SOutRange *list = InitOutRange();
            
            // 変更された箇所を登録していく
            RegisterOutRange( list, 0x1f,  0x1f, "Menu-skip flag" );
            RegisterOutRange( list, 0x15e, 0x15f, "Rom-header CRC" );
            
            // 変更された箇所以外をベリファイ
            if( !VerifyExceptOutRange( list, pContext->before_fp, pContext->after_fp ) )
            {
                printf("\n*** Error: verify error ***\n");
                FinalizeOutRange( list );   // リストのメモリ解放
                return FALSE;
            }
            FinalizeOutRange( list );   // リストのメモリ解放
        }
    }
    else
    {
        switch(after_rh.s.platform_code)
        {
            case PLATFORM_CODE_NTR:     // PictoChat/DlPlay
                printf( "Platform(TWL/NTR): 0x%02X (NTR for TWL, PictoChat or DS-Download-play)\n", after_rh.s.platform_code );
            break;

            case PLATFORM_CODE_TWL_HYBLID:
                printf( "Platform(TWL/NTR): 0x%02X (TWL Hybrid)\n", after_rh.s.platform_code );
            break;

            case PLATFORM_CODE_TWL_LIMITED:
                printf( "Platform(TWL/NTR): 0x%02X (TWL Limited)\n", after_rh.s.platform_code );
            break;

            default:    // unreachable
                printf( "Platform(TWL/NTR): 0x%02X (Illegal)\n", after_rh.s.platform_code );
                printf("\n*** Error: Illegal platform (%d). ***\n", after_rh.s.platform_code);
                return FALSE;
            break;
        }

        // 署名を外す
        if( !DecryptSignRomHeader( &after_rh ) )
        {
            printf( "Decrypt the sign : Failed\n" );
            printf( "\n*** Error: Invalid signature. ***\n" );
            return FALSE;
        }
        printf( "Decrypt the sign : Succeeded\n" );

        // 変更される箇所以外をべリファイ
        {
            SOutRange *list = InitOutRange();
            
            // 変更された箇所を登録していく
            RegisterOutRange( list, 0x1f,  0x1f,  "Menu-skip flag" );
            RegisterOutRange( list, 0x15e, 0x15f, "Rom-header CRC" );
            RegisterOutRange( list, 0xf80, 0xfff, "Signature" );
            
            // 変更される箇所以外をベリファイ
            if( !VerifyExceptOutRange( list, pContext->before_fp, pContext->after_fp ) )
            {
                printf("\n*** Error: verify error ***\n");
                FinalizeOutRange( list );   // リストのメモリ解放
                return FALSE;
            }
            FinalizeOutRange( list );   // リストのメモリ解放
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
