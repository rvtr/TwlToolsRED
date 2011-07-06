
#include "types.h"
#include <string.h>
#include <openssl/hmac.h> /* libcrypto.a */
#include "twl_format_rom.h"
#include "card_hash.h"


#define MATH_SHA1_DIGEST_SIZE   (160/8)
#define CARD_ROM_HASH_SIZE      (20)

static u8 CARDiHmacKey[] =
{
    0x21, 0x06, 0xc0, 0xde, 0xba, 0x98, 0xce, 0x3f,
    0xa6, 0x92, 0xe3, 0x9d, 0x46, 0xf2, 0xed, 0x01,
    0x76, 0xe3, 0xcc, 0x08, 0x56, 0x23, 0x63, 0xfa,
    0xca, 0xd4, 0xec, 0xdf, 0x9a, 0x62, 0x78, 0x34,
    0x8f, 0x6d, 0x63, 0x3c, 0xfe, 0x22, 0xca, 0x92,
    0x20, 0x88, 0x97, 0x23, 0xd2, 0xcf, 0xae, 0xc2,
    0x32, 0x67, 0x8d, 0xfe, 0xca, 0x83, 0x64, 0x98,
    0xac, 0xfd, 0x3e, 0x37, 0x87, 0x46, 0x58, 0x24,
};

/*
unsigned char *HMAC(const EVP_MD *evp_md, const void *key,
               int key_len, const unsigned char *d, int n,
               unsigned char *md, unsigned int *md_len);
*/

HMAC_CTX myHmacContext;


void MATH_CalcHMACSHA1(void *digest, const void *bin_ptr, u32 bin_len, const void *key_ptr, u32 key_len) 
{
    unsigned int res_len;
    
    HMAC( EVP_sha1(),
          key_ptr, key_len,
          (const unsigned char*)bin_ptr, (size_t)bin_len,
          (unsigned char*)digest, &res_len);
}


static bool CARDi_CompareHash(const void *hash, void *buffer, u32 length)
{
    bool ret = true;
    u8      tmphash[CARD_ROM_HASH_SIZE];

    MATH_CalcHMACSHA1(tmphash, buffer, length, CARDiHmacKey, sizeof(CARDiHmacKey));
    
    if (memcmp(hash, tmphash, sizeof(tmphash)) != 0)
    {
        ret = false;
        printf("ROM-hash comparation error!\n");
    }
    else
    {
//        printf("ROM-hash comparation success.\n");
    }
/*        
    u8* myhash = (u8*)hash;
    printf( "digest2:0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n",
            myhash[0], myhash[1], myhash[2], myhash[3], myhash[4], myhash[5],
            myhash[6], myhash[7], myhash[8], myhash[9], myhash[10], myhash[11],
            myhash[12], myhash[13], myhash[14], myhash[15], myhash[16], myhash[17],
            myhash[18], myhash[19]);
    printf( "tmphash:0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n",
            tmphash[0], tmphash[1], tmphash[2], tmphash[3], tmphash[4], tmphash[5],
            tmphash[6], tmphash[7], tmphash[8], tmphash[9], tmphash[10], tmphash[11],
            tmphash[12], tmphash[13], tmphash[14], tmphash[15], tmphash[16], tmphash[17],
            tmphash[18], tmphash[19]);
  */
    return ret;
}

void CARDi_Init( CARDRomHashContext *context, RomHeader* header)
{
    context->bytes_per_sector = header->digest1_block_size;
    context->sectors_per_block = header->digest2_covered_digest1_num;

    context->area_ntr.offset = header->nitro_digest_area_rom_offset;
    context->area_ntr.length = header->nitro_digest_area_size;
    context->area_ltd.offset = header->twl_digest_area_rom_offset;
    context->area_ltd.length = header->twl_digest_area_size;

    context->sector_hash.offset = header->digest1_table_offset;
    context->sector_hash.length = header->digest1_table_size;
    context->block_hash.offset = header->digest2_table_offset;
    context->block_hash.length = header->digest2_table_size;

    context->block_max = CARD_ROM_HASH_BLOCK_MAX;
    context->sector_max = CARD_ROM_HASH_SECTOR_MAX;

    // digest2
    context->master_hash = (u8*)malloc( (1024*1024*1024/8) /
                                   (header->digest1_block_size * header->digest2_covered_digest1_num) *
                                   CARD_ROM_HASH_SIZE);
    // digest1
    context->hash = (u8*)malloc( header->digest2_covered_digest1_num * CARD_ROM_HASH_SIZE);
    // rom image
    context->buffer = (u8*)malloc( header->digest1_block_size);

    // rom size
    {
        int i;
        u32 rom_size;
        for( i=0; i<header->rom_size; i++)
        {
            rom_size *= 2;
        }
        context->rom_size = (rom_size * 1024 / 8 * 1024);
    }
}


/*---------------------------------------------------------------------------*
  Name:         CARDi_GetHashSectorIndex

  Description:  指定のROMオフセットが属するセクタ番号を取得。

  Arguments:    context : CARDRomHashContext構造体
                offset  : ROMオフセット

  Returns:      指定のROMオフセットが属するセクタ番号。
 *---------------------------------------------------------------------------*/
u32 CARDi_GetHashSectorIndex(const CARDRomHashContext *context, u32 offset)
{
    offset -= context->area_ntr.offset;
    if (offset >= context->area_ntr.length)
    {
        offset += (context->area_ntr.offset - context->area_ltd.offset);
        if (offset < context->area_ltd.length)
        {
            offset += context->area_ntr.length;
        }
        else
        {
            printf("specified ROM address is outof-range.(unsafe without secure hash)\n");
            exit(1);
        }
    }
    return offset / context->bytes_per_sector;
}

//static CARDRomHashBlock* CARDi_TouchRomHashBlock(CARDRomHashContext *context, u32 sector)
//{
//}

void CARDi_CheckHash(CARDRomHashContext *context, FILE* fp, u32 start, u32 size, RomHeader* header)
{
    long nowfp;
    u32 offset;
    int linear_rom_sector;
    u32 i, j;
        
    nowfp = ftell( fp);

    for( j=0; j<(size/(context->bytes_per_sector * context->sectors_per_block)); j++)
    {
        for( i=0; i<context->sectors_per_block; i++)
        {
            offset = start +
                     (i * context->bytes_per_sector) +
                     (j * (context->sectors_per_block * context->bytes_per_sector));

            printf( "%s, %d (offset:0x%lx), 0x%lx, 0x%lx, %ld, %ld\n", __FUNCTION__, __LINE__, offset, context->bytes_per_sector, context->sectors_per_block, i, j);
            /* ROMデータを読む */
            fseek( fp, offset, SEEK_SET);
            fread( context->buffer, context->bytes_per_sector, 1, fp);
    
            /* Digest1を読む */
            linear_rom_sector = CARDi_GetHashSectorIndex( context, offset);
            fseek( fp, (header->digest1_table_offset + (linear_rom_sector * CARD_ROM_HASH_SIZE)), SEEK_SET);
            fread( &(context->hash[i * CARD_ROM_HASH_SIZE]), CARD_ROM_HASH_SIZE, 1, fp);

            printf( "-----digest1 check-----\n");
            printf( "i = %ld, linear = %d\n", i, linear_rom_sector);
            /* ROMデータのHashをDigest1と比較 */
            CARDi_CompareHash( &(context->hash[i * CARD_ROM_HASH_SIZE]),
                               context->buffer, context->bytes_per_sector);
            printf( "-----------------------\n");
        }
    }

    fseek( fp, nowfp, SEEK_SET);
}

bool Digest1Check(CARDRomHashContext *context, FILE* fp, RomHeader* header, u32 start_offset, u32 size)
{
    int digest1_index;
    u32 offset, rest;
    bool ret = true;

    rest = size;
    offset = start_offset;

    do {
        if( !(((start_offset >= header->aes_target_rom_offset)&&
            (start_offset <= (header->aes_target_rom_offset + header->aes_target_size))) ||
            (((start_offset + size) >= header->aes_target_rom_offset)&&
             ((start_offset + size) <= (header->aes_target_rom_offset + header->aes_target_size)))))
        {

            /* ROMデータを読む */
            fseek( fp, offset, SEEK_SET);
            fread( context->buffer, context->bytes_per_sector, 1, fp);

            /* Digest1を読む */
            digest1_index = CARDi_GetHashSectorIndex( context, offset);
            fseek( fp, (header->digest1_table_offset + (digest1_index * CARD_ROM_HASH_SIZE)), SEEK_SET);
            fread( context->hash, CARD_ROM_HASH_SIZE, 1, fp);

            /* ROMデータのHashをDigest1と比較 */
            if( !CARDi_CompareHash( context->hash, context->buffer, context->bytes_per_sector))
            {
                ret = false;
            }
        }

        rest -= context->bytes_per_sector;
        offset += context->bytes_per_sector;
    }
    while( rest);
    return ret;
}

bool Digest2Check(CARDRomHashContext *context, FILE* fp, RomHeader* header)
{
    bool ret = true;
    int i, j;
    int digest1_index_num = (header->digest1_table_size / header->digest1_block_size);
    
    for( i=0; i<digest1_index_num; )
    {
        for( j=0; j<(int)(header->digest2_covered_digest1_num); j++)
        {
            fseek( fp, (header->digest1_table_offset + ((i+j) * CARD_ROM_HASH_SIZE)), SEEK_SET);
            fread( &(context->hash[j * CARD_ROM_HASH_SIZE]), CARD_ROM_HASH_SIZE, 1, fp);
        }
        /* Digest1をDigest2と比較 */
        if( !CARDi_CompareHash( &(context->master_hash[(i/context->sectors_per_block) * CARD_ROM_HASH_SIZE]),
                                context->hash, (CARD_ROM_HASH_SIZE * context->sectors_per_block)))
        {
            ret = false;
        }
        
        i+= header->digest2_covered_digest1_num;
    }
    return ret;
}

void CARD_CheckHash(CARDRomHashContext *context, RomHeader* header, FILE* fp)
{
    long nowfp;
    nowfp = ftell( fp);
    fseek( fp, header->digest2_table_offset, SEEK_SET);
    fread( context->master_hash, header->digest2_table_size, 1, fp);
    fseek( fp, nowfp, SEEK_SET);

    printf( "\n");
    printf( "nitro digest area check\n");
    printf( "-----------------------\n");
    // NITROダイジェストエリア検証
    if( Digest1Check( context, fp, header, header->nitro_digest_area_rom_offset, header->nitro_digest_area_size))
    {
        printf( "（検証OK.）\n");
    }
    printf( "-----------------------\n\n");

    printf( "twl digest area check\n");
    printf( "-----------------------\n");
    // TWLダイジェストエリア検証
    if( Digest1Check( context, fp, header, header->twl_digest_area_rom_offset, header->twl_digest_area_size))
    {
        printf( "（検証OK.）\n");
    }
    printf( "-----------------------\n\n");

    printf( "digest2 check\n");
    printf( "-----------------------\n");
    if( Digest2Check( context, fp, header))
    {
        printf( "（検証OK.）\n");
    }
    printf( "-----------------------\n\n");
}

/*
static void CARDi_ReadRomHashImageDirect(CARDRomHashContext *context, void *buffer, u32 offset, u32 length)
{
    const u32   sectunit = context->bytes_per_sector;
    const u32   blckunit = context->sectors_per_block;
    u32         position = offset;
    u32         end = length + offset;
    u32         sector = CARDi_GetHashSectorIndex(context, position);
    long nowgfp;
    FILE* gfp;
    
    while (position < end)
    {
        // 今回取得可能な最小単位のイメージを同期読み込み。
        nowgfp = ftell( gfp);
        fseek( gfp, position, SEEK_SET);
        u32     available = fread( buffer, end - position, 1, gfp);
        // 今回の検証中に必要となりそうなブロックを前もってアクセス。
//        (void)CARDi_TouchRomHashBlock(context, sector);
        // 取得したイメージの正当性を検証。
        while (available >= sectunit)
        {
            CARDRomHashBlock   *block = CARDi_TouchRomHashBlock(context, sector);
            u32                 slot = sector - block->index * blckunit;
            while ((slot < blckunit) && (available >= sectunit))
            {
                // 必要ならここでブロック単位のハッシュテーブルを検証。
                if (block != context->valid_block)
                {
                    OSIntrMode  bak_cpsr = OS_DisableInterrupts();
                    while (context->loading_block)
                    {
                        OS_SleepThread(NULL);
                    }
                    if (block == context->loaded_block)
                    {
                        context->loaded_block = block->next;
                    }
                    (void)OS_RestoreInterrupts(bak_cpsr);
                    CARDi_CompareHash(&context->master_hash[block->index * CARD_ROM_HASH_SIZE],
                                      block->hash, CARD_ROM_HASH_SIZE * blckunit);
                    block->next = context->valid_block;
                    context->valid_block = block;
                }
                // イメージのハッシュを計算。
                CARDi_CompareHash(&block->hash[slot * CARD_ROM_HASH_SIZE], buffer, sectunit);
                position += sectunit;
                available -= sectunit;
                buffer = ((u8 *)buffer) + sectunit;
                slot += 1;
                sector += 1;
            }
        }
    }
}
*/

#if 0
/*---------------------------------------------------------------------------*
  Name:         CARDi_ReadRomHashImageDirect

  Description:  ハッシュコンテキストへキャッシュせずに転送先へ直接コピー。

  Arguments:    context : CARDRomHashContext構造体。
                buffer  : 転送先バッファ。(4バイト整合されている必要がある)
                offset  : アクセスするROMオフセット。
                length  : 転送サイズ。

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_ReadRomHashImageDirect(CARDRomHashContext *context, void *buffer, u32 offset, u32 length)
{
    const u32   sectunit = context->bytes_per_sector;
    const u32   blckunit = context->sectors_per_block;
    u32         position = offset;
    u32         end = length + offset;
    u32         sector = CARDi_GetHashSectorIndex(context, position);
    while (position < end)
    {
        // 今回取得可能な最小単位のイメージを同期読み込み。
        u32     available = (u32)(*context->ReadSync)(context->userdata, buffer, position, end - position);
        // 今回の検証中に必要となりそうなブロックを前もってアクセス。
        (void)CARDi_TouchRomHashBlock(context, sector);
        // 次回に必要となる分の読み込み準備を要求。
        if (context->ReadAsync && (position + available < end))
        {
            (void)(*context->ReadAsync)(context->userdata, NULL, position + available, end - (position + available));
        }
        // 取得したイメージの正当性を検証。
        while (available >= sectunit)
        {
            CARDRomHashBlock   *block = CARDi_TouchRomHashBlock(context, sector);
            u32                 slot = sector - block->index * blckunit;
            while ((slot < blckunit) && (available >= sectunit))
            {
                // 必要ならここでブロック単位のハッシュテーブルを検証。
                if (block != context->valid_block)
                {
                    OSIntrMode  bak_cpsr = OS_DisableInterrupts();
                    while (context->loading_block)
                    {
                        OS_SleepThread(NULL);
                    }
                    if (block == context->loaded_block)
                    {
                        context->loaded_block = block->next;
                    }
                    (void)OS_RestoreInterrupts(bak_cpsr);
                    CARDi_CompareHash(&context->master_hash[block->index * CARD_ROM_HASH_SIZE],
                                      block->hash, CARD_ROM_HASH_SIZE * blckunit);
                    block->next = context->valid_block;
                    context->valid_block = block;
                }
                // イメージのハッシュを計算。
                CARDi_CompareHash(&block->hash[slot * CARD_ROM_HASH_SIZE], buffer, sectunit);
                position += sectunit;
                available -= sectunit;
                buffer = ((u8 *)buffer) + sectunit;
                slot += 1;
                sector += 1;
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         MATH_CalcHMACSHA1

  Description:  HMAC-SHA-1 を計算する。
  
  Arguments:    digest  HMAC-SHA-1 値を格納する場所へのポインタ
                data    入力データのポインタ
                dataLength  入力データ長
                key     鍵のポインタ
                keyLength   鍵の長さ
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MATH_CalcHMACSHA1(void *digest, const void *bin_ptr, u32 bin_len, const void *key_ptr, u32 key_len) 
{
    MATHSHA1Context context;
    unsigned char   hash_buf[ MATH_SHA1_DIGEST_SIZE ]; /* ハッシュ関数から得るハッシュ値 */
    
    MATHiHMACFuncs hash2funcs = {
        MATH_SHA1_DIGEST_SIZE,
        (512/8),
    };
    
    hash2funcs.context       = &context;
    hash2funcs.hash_buf      = hash_buf;
    hash2funcs.HashReset     = (void (*)(void*))                   MATH_SHA1Init;
    hash2funcs.HashSetSource = (void (*)(void*, const void*, u32)) MATH_SHA1Update;
    hash2funcs.HashGetDigest = (void (*)(void*, void*))            MATH_SHA1GetHash;
    
    MATHi_CalcHMAC(digest, bin_ptr, bin_len, key_ptr, key_len, &hash2funcs);
}


/*---------------------------------------------------------------------------*
  Name:         CARDi_CompareHash

  Description:  ハッシュによる正当性チェック。(HMAC-SHA1)

  Arguments:    hash : 比較基準となるハッシュ値
                src  : チェック対象のイメージ
                len　: チェック対象のサイズ

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void CARDi_CompareHash(const void *hash, void *buffer, u32 length)
{
    u8      tmphash[CARD_ROM_HASH_SIZE];

    MATH_CalcHMACSHA1(tmphash, buffer, length, CARDiHmacKey, sizeof(CARDiHmacKey));
    
    if (MI_CpuComp8(hash, tmphash, sizeof(tmphash)) != 0)
    {
        printf("ROM-hash comparation error!\n");
    }
}
#endif
