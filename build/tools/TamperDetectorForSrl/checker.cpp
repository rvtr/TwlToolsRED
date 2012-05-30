
#include <string.h>
#include "checker.h"
//#include "nitro_romheader.h"
#include "twl_format_rom.h"
#include "card_hash.h"

extern Entry gEntry;
extern Entry mEntry;

void Checker::Initialize( FILE* myGfp, FILE* myMfp, void* myGbuf, void* myMbuf, u32 size)
{
    gfp = myGfp;
    mfp = myMfp;
    gBuf = myGbuf;
    mBuf = myMbuf;
    buffer_size = size;

    gEntry.Initialize();
    mEntry.Initialize();
}

bool Checker::LoadHeader( void* gHeaderBuf, void* mHeaderBuf)
{
    size_t readed;

    fseek( gfp, 0, SEEK_SET);
    readed = fread( gHeaderBuf, sizeof(RomHeader), 1, gfp);
    if( (readed == 1)&&(mHeaderBuf))
    {
        fseek( mfp, 0, SEEK_SET);
        readed = fread( mHeaderBuf, sizeof(RomHeader), 1, mfp);
        if( readed == sizeof(RomHeader))
        {
            return true;
        }
    }
    return false;
}

bool Checker::Diff( DiffLevel* diffLevel, u32 g_offset, u32 g_size, u32 m_offset, u32 m_size, bool isDataOnly, PrintLevel print_enable)
{
    long nowgfp, nowmfp;
    int result = 0;
    u32 check_size, rest_size;
    u32 i, loop_num;

    *diffLevel = DIFF_NOT_TOUCHED;
    check_size = (g_size < m_size)? g_size : m_size;
    rest_size = check_size;

    if( !isDataOnly)
    {
        /* 指定アドレスとサイズのチェック */
        if( g_offset == m_offset)
        {
            if( (print_enable)&&(print_enable < PRINT_LEVEL_2)) {
                printf( "  offset:0x%lx\n", g_offset);
//                printf( "  offset:0x%lx（改竄されていない）\n", g_offset);
            }
        }
        else
        {
            *diffLevel |= DIFF_LOCATION_MODIFIED;
            if( print_enable) {
                printf( "  offset:0x%lx ---> offset:0x%lx（改竄されている）\n", g_offset, m_offset);
            }
        }

        if( g_size == m_size)
        {
            if( (print_enable)&&(print_enable < PRINT_LEVEL_2)) {
                printf( "  size:0x%lx\n", g_size);
//                printf( "  size:0x%lx（改竄されていない）\n", g_size);
            }
        }
        else
        {
            *diffLevel |= DIFF_SIZE_MODIFIED;
            if( print_enable) {
                printf( "  size:0x%lx ---> size:0x%lx（改竄されている）\n", g_size, m_size);
            }
        }
    }

    bool filled = true;
    int  totalResult = 0;
    u32  j;

    nowgfp = ftell( gfp);
    nowmfp = ftell( mfp);

    /* OUT_OF_RANGE */
    fseek( gfp, 0, SEEK_END);
    fseek( mfp, 0, SEEK_END);
    if( ((g_offset + g_size) > ftell(gfp)) || ((m_offset + m_size) > ftell(mfp)))
    {
        *diffLevel |= DIFF_OUT_OF_RANGE;
        return false;
    }
    
    /* メモリ内容のチェック（サイズが異なる場合は小さいサイズで） */
    fseek( gfp, g_offset, SEEK_SET);
    fseek( mfp, m_offset, SEEK_SET);

    if( rest_size > buffer_size)
    {
        loop_num = (rest_size / buffer_size);
        for( i=0; i<loop_num; i++)
        {
            fread( gBuf, buffer_size, 1, gfp);
            fread( mBuf, buffer_size, 1, mfp);
            result = memcmp( gBuf, mBuf, buffer_size);
            if( result != 0)
            {
                totalResult = 1;
            }
            if( filled) // FILLチェック
            {
                for( j=0; j<buffer_size; j++)
                {
                    if( *((u8*)mBuf + j) != *((u8*)mBuf))
                    {
//                        printf( ".%d, %d, 0x%x, 0x%x\n", i, j, *((u8*)mBuf+j), *((u8*)mBuf));
                        filled = false;
                    }
                }
            }
        }
        rest_size = (rest_size % buffer_size);
    }

    if( rest_size)
    {
        fread( gBuf, rest_size, 1, gfp);
        fread( mBuf, rest_size, 1, mfp);
        result = memcmp( gBuf, mBuf, rest_size);
        if( result != 0)
        {
            totalResult = 1;
        }
        if( filled) // FILLチェック
        {
            for( j=0; j<rest_size; j++)
            {
                if( *((u8*)mBuf + j) != *((u8*)mBuf))
                {
//                    printf( "..%d, %d, 0x%x, 0x%x\n", i, j, *((u8*)mBuf+j), *((u8*)mBuf));
                    filled = false;
                }
            }
        }
    }

    /* メモリ内容チェック結果 */
    if( totalResult == 0)
    {
        if( (print_enable)&&(print_enable < PRINT_LEVEL_2)) {
            printf( "  data:\n");
//            printf( "  data:（改竄されていない）\n");
        }
    }
    else
    {
        if( filled)
        {
            *diffLevel |= DIFF_DATA_FILLED;
            if( print_enable) {
                printf( "  data:（0x%xでフィルされている）\n", *((u8*)mBuf));
            }
        }
        else
        {
            *diffLevel |= DIFF_DATA_MODIFIED;
            if( print_enable) {
                printf( "  data:（改竄されている）\n");
            }
        }
    }

    // ファイルポインタを戻す
    fseek( gfp, nowgfp, SEEK_SET);
    fseek( mfp, nowmfp, SEEK_SET);
    return true;
}


void Checker::AnalyzeHeader( RomHeader* gHeaderBuf, Entry* gEntry, RomHeader* mHeaderBuf, Entry* mEntry)
{
    DiffLevel diffLevel;
    MyAreaEntry     *tmpAreaEntry;

        if( gHeaderBuf->platform_code & 0x03)
        {   // TWL-ROMヘッダの範囲をチェック
            Diff( &diffLevel, 0, sizeof(RomHeader), 0, sizeof(RomHeader), false, PRINT_LEVEL_1);
            printf( "[support TWL]\n");
        }
        else
        {   // NITRO-ROMヘッダの範囲だけチェック
            Diff( &diffLevel, 0, 0x180, 0, 0x180, false, PRINT_LEVEL_1);
        }
        printf( "------------------\n");

        printf( "ARM9 Static Module\n");
        Diff( &diffLevel,
              (u32)(gHeaderBuf->arm9.romAddr),
              (u32)(gHeaderBuf->arm9.romSize),
              (u32)(mHeaderBuf->arm9.romAddr),
              (u32)(mHeaderBuf->arm9.romSize),
              false, PRINT_LEVEL_1);
        printf( "------------------\n");

        printf( "ARM7 Static Module\n");
        Diff( &diffLevel,
              (u32)(gHeaderBuf->arm7.romAddr),
              (u32)(gHeaderBuf->arm7.romSize),
              (u32)(mHeaderBuf->arm7.romAddr),
              (u32)(mHeaderBuf->arm7.romSize),
              false, PRINT_LEVEL_1);
        printf( "------------------\n");

        printf( "File Name Table\n");
        Diff( &diffLevel,
              (u32)(gHeaderBuf->fnt_offset),
              (u32)(gHeaderBuf->fnt_size),
              (u32)(mHeaderBuf->fnt_offset),
              (u32)(mHeaderBuf->fnt_size),
              false, PRINT_LEVEL_1);
        printf( "------------------\n");

        printf( "File Allocation Table\n");
        Diff( &diffLevel,
              (u32)(gHeaderBuf->fat_offset),
              (u32)(gHeaderBuf->fat_size),
              (u32)(mHeaderBuf->fat_offset),
              (u32)(mHeaderBuf->fat_size),
              false, PRINT_LEVEL_1);
        printf( "------------------\n");

        printf( "ARM9 Overlay Table\n");
        Diff( &diffLevel,
              (u32)(gHeaderBuf->main_ovt_offset),
              (u32)(gHeaderBuf->main_ovt_size),
              (u32)(mHeaderBuf->main_ovt_offset),
              (u32)(mHeaderBuf->main_ovt_size),
              false, PRINT_LEVEL_1);
        printf( "------------------\n");

        printf( "ARM7 Overlay Table\n");
        Diff( &diffLevel,
              (u32)(gHeaderBuf->sub_ovt_offset),
              (u32)(gHeaderBuf->sub_ovt_size),
              (u32)(mHeaderBuf->sub_ovt_offset),
              (u32)(mHeaderBuf->sub_ovt_size),
              false, PRINT_LEVEL_1);
        printf( "------------------\n");


        printf( "------------------\n");
        printf( "TWL Rom Header\n");
        if( gHeaderBuf->platform_code & 0x03)
        {
            /* TWL専用部分 */
            printf( "ARM9 Ltd Static Module\n");
            Diff( &diffLevel,
                  (u32)(gHeaderBuf->ltd_arm9.romAddr),
                  (u32)(gHeaderBuf->ltd_arm9.romSize),
                  (u32)(mHeaderBuf->ltd_arm9.romAddr),
                  (u32)(mHeaderBuf->ltd_arm9.romSize),
                  false, PRINT_LEVEL_1);
            printf( "------------------\n");

            printf( "ARM7 Ltd Static Module\n");
            Diff( &diffLevel,
                  (u32)(gHeaderBuf->ltd_arm7.romAddr),
                  (u32)(gHeaderBuf->ltd_arm7.romSize),
                  (u32)(mHeaderBuf->ltd_arm7.romAddr),
                  (u32)(mHeaderBuf->ltd_arm7.romSize),
                  false, PRINT_LEVEL_1);
            printf( "------------------\n");

            printf( "(NITRO Digest Area)\n");
            Diff( &diffLevel,
                  (u32)(gHeaderBuf->nitro_digest_area_rom_offset),
                  (u32)(gHeaderBuf->nitro_digest_area_size),
                  (u32)(mHeaderBuf->nitro_digest_area_rom_offset),
                  (u32)(mHeaderBuf->nitro_digest_area_size),
                  false, PRINT_LEVEL_1);
            printf( "------------------\n");

            printf( "(TWL Digest Area)\n");
            Diff( &diffLevel,
                  (u32)(gHeaderBuf->twl_digest_area_rom_offset),
                  (u32)(gHeaderBuf->twl_digest_area_size),
                  (u32)(mHeaderBuf->twl_digest_area_rom_offset),
                  (u32)(mHeaderBuf->twl_digest_area_size),
                  false, PRINT_LEVEL_1);
            printf( "------------------\n");

            printf( "Digest1 Table\n");
            Diff( &diffLevel,
                  (u32)(gHeaderBuf->digest1_table_offset),
                  (u32)(gHeaderBuf->digest1_table_size),
                  (u32)(mHeaderBuf->digest1_table_offset),
                  (u32)(mHeaderBuf->digest1_table_size),
                  false, PRINT_LEVEL_1);
            printf( "------------------\n");

            printf( "Digest2 Table\n");
            Diff( &diffLevel,
                  (u32)(gHeaderBuf->digest2_table_offset),
                  (u32)(gHeaderBuf->digest2_table_size),
                  (u32)(mHeaderBuf->digest2_table_offset),
                  (u32)(mHeaderBuf->digest2_table_size),
                  false, PRINT_LEVEL_1);
            printf( "------------------\n");

            printf( "AES TARGET\n");
            Diff( &diffLevel,
                  (u32)(gHeaderBuf->aes_target_rom_offset),
                  (u32)(gHeaderBuf->aes_target_size),
                  (u32)(mHeaderBuf->aes_target_rom_offset),
                  (u32)(mHeaderBuf->aes_target_size),
                  false, PRINT_LEVEL_1);
            printf( "------------------\n");
        };

    // genuine 領域を登録
    tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
    gEntry->InitializeEntry( tmpAreaEntry);
    gEntry->SetName( tmpAreaEntry, (char*)"A9-Static", 9);
    tmpAreaEntry->top = (u32)(gHeaderBuf->arm9.romAddr);
    tmpAreaEntry->bottom = (u32)(gHeaderBuf->arm9.romAddr + gHeaderBuf->arm9.romSize);
    gEntry->addAreaEntry( tmpAreaEntry);

    tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
    gEntry->InitializeEntry( tmpAreaEntry);
    gEntry->SetName( tmpAreaEntry, (char*)"A7-Static", 9);
    tmpAreaEntry->top = (u32)(gHeaderBuf->arm7.romAddr);
    tmpAreaEntry->bottom = (u32)(gHeaderBuf->arm7.romAddr + gHeaderBuf->arm7.romSize);
    gEntry->addAreaEntry( tmpAreaEntry);

    tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
    gEntry->InitializeEntry( tmpAreaEntry);
    gEntry->SetName( tmpAreaEntry, (char*)"FNT", 3);
    tmpAreaEntry->top = (u32)(gHeaderBuf->fnt_offset);
    tmpAreaEntry->bottom = (u32)((u32)(gHeaderBuf->fnt_offset) + gHeaderBuf->fnt_size);
    gEntry->addAreaEntry( tmpAreaEntry);

    tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
    gEntry->InitializeEntry( tmpAreaEntry);
    gEntry->SetName( tmpAreaEntry, (char*)"FAT", 3);
    tmpAreaEntry->top = (u32)(gHeaderBuf->fat_offset);
    tmpAreaEntry->bottom = (u32)((u32)(gHeaderBuf->fat_offset) + gHeaderBuf->fat_size);
    gEntry->addAreaEntry( tmpAreaEntry);

    tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
    gEntry->InitializeEntry( tmpAreaEntry);
    gEntry->SetName( tmpAreaEntry, (char*)"A9-OVT", 6);
    tmpAreaEntry->top = (u32)(gHeaderBuf->main_ovt_offset);
    tmpAreaEntry->bottom = (u32)((u32)(gHeaderBuf->main_ovt_offset) + gHeaderBuf->main_ovt_size);
    gEntry->addAreaEntry( tmpAreaEntry);

    tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
    gEntry->InitializeEntry( tmpAreaEntry);
    gEntry->SetName( tmpAreaEntry, (char*)"A7-OVT", 6);
    tmpAreaEntry->top = (u32)(gHeaderBuf->sub_ovt_offset);
    tmpAreaEntry->bottom = (u32)((u32)(gHeaderBuf->sub_ovt_offset) + gHeaderBuf->sub_ovt_size);
    gEntry->addAreaEntry( tmpAreaEntry);


    // magicon 領域を登録
    tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
    mEntry->InitializeEntry( tmpAreaEntry);
    mEntry->SetName( tmpAreaEntry, (char*)"A9-Static", 9);
    tmpAreaEntry->top = (u32)(mHeaderBuf->arm9.romAddr);
    tmpAreaEntry->bottom = (u32)(mHeaderBuf->arm9.romAddr + mHeaderBuf->arm9.romSize);
    mEntry->addAreaEntry( tmpAreaEntry);

    tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
    mEntry->InitializeEntry( tmpAreaEntry);
    mEntry->SetName( tmpAreaEntry, (char*)"A7-Static", 9);
    tmpAreaEntry->top = (u32)(mHeaderBuf->arm7.romAddr);
    tmpAreaEntry->bottom = (u32)(mHeaderBuf->arm7.romAddr + mHeaderBuf->arm7.romSize);
    mEntry->addAreaEntry( tmpAreaEntry);

    tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
    mEntry->InitializeEntry( tmpAreaEntry);
    mEntry->SetName( tmpAreaEntry, (char*)"FNT", 3);
    tmpAreaEntry->top = (u32)(mHeaderBuf->fnt_offset);
    tmpAreaEntry->bottom = (u32)((u32)(mHeaderBuf->fnt_offset) + mHeaderBuf->fnt_size);
    mEntry->addAreaEntry( tmpAreaEntry);

    tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
    mEntry->InitializeEntry( tmpAreaEntry);
    mEntry->SetName( tmpAreaEntry, (char*)"FAT", 3);
    tmpAreaEntry->top = (u32)(mHeaderBuf->fat_offset);
    tmpAreaEntry->bottom = (u32)((u32)(mHeaderBuf->fat_offset) + mHeaderBuf->fat_size);
    mEntry->addAreaEntry( tmpAreaEntry);

    tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
    mEntry->InitializeEntry( tmpAreaEntry);
    mEntry->SetName( tmpAreaEntry, (char*)"A9-OVT", 6);
    tmpAreaEntry->top = (u32)(mHeaderBuf->main_ovt_offset);
    tmpAreaEntry->bottom = (u32)((u32)(mHeaderBuf->main_ovt_offset) + mHeaderBuf->main_ovt_size);
    mEntry->addAreaEntry( tmpAreaEntry);

    tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
    mEntry->InitializeEntry( tmpAreaEntry);
    mEntry->SetName( tmpAreaEntry, (char*)"A7-OVT", 6);
    tmpAreaEntry->top = (u32)(mHeaderBuf->sub_ovt_offset);
    tmpAreaEntry->bottom = (u32)((u32)(mHeaderBuf->sub_ovt_offset) + mHeaderBuf->sub_ovt_size);
    mEntry->addAreaEntry( tmpAreaEntry);


    if( gHeaderBuf->platform_code & 0x03)
    {
        // genuine 領域を登録
        tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
        gEntry->InitializeEntry( tmpAreaEntry);
        gEntry->SetName( tmpAreaEntry, (char*)"A9-Ltd-Static", 13);
        tmpAreaEntry->top = (u32)(gHeaderBuf->ltd_arm9.romAddr);
        tmpAreaEntry->bottom = (u32)(gHeaderBuf->ltd_arm9.romAddr + gHeaderBuf->ltd_arm9.romSize);
        gEntry->addAreaEntry( tmpAreaEntry);

        tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
        gEntry->InitializeEntry( tmpAreaEntry);
        gEntry->SetName( tmpAreaEntry, (char*)"A7-Ltd-Static", 13);
        tmpAreaEntry->top = (u32)(gHeaderBuf->ltd_arm7.romAddr);
        tmpAreaEntry->bottom = (u32)(gHeaderBuf->ltd_arm7.romAddr + gHeaderBuf->ltd_arm7.romSize);
        gEntry->addAreaEntry( tmpAreaEntry);

        tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
        gEntry->InitializeEntry( tmpAreaEntry);
        gEntry->SetName( tmpAreaEntry, (char*)"(NTR-Digest Area)", 17);
        tmpAreaEntry->top = (u32)(gHeaderBuf->nitro_digest_area_rom_offset);
        tmpAreaEntry->bottom = (u32)(gHeaderBuf->nitro_digest_area_rom_offset + gHeaderBuf->nitro_digest_area_size);
        gEntry->addAreaEntry( tmpAreaEntry);

        tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
        gEntry->InitializeEntry( tmpAreaEntry);
        gEntry->SetName( tmpAreaEntry, (char*)"(TWL-Digest Area)", 17);
        tmpAreaEntry->top = (u32)(gHeaderBuf->twl_digest_area_rom_offset);
        tmpAreaEntry->bottom = (u32)(gHeaderBuf->twl_digest_area_rom_offset + gHeaderBuf->twl_digest_area_size);
        gEntry->addAreaEntry( tmpAreaEntry);

        tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
        gEntry->InitializeEntry( tmpAreaEntry);
        gEntry->SetName( tmpAreaEntry, (char*)"Digest1-Table", 13);
        tmpAreaEntry->top = (u32)(gHeaderBuf->digest1_table_offset);
        tmpAreaEntry->bottom = (u32)(gHeaderBuf->digest1_table_offset + gHeaderBuf->digest1_table_size);
        gEntry->addAreaEntry( tmpAreaEntry);

        tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
        gEntry->InitializeEntry( tmpAreaEntry);
        gEntry->SetName( tmpAreaEntry, (char*)"Digest2-Table", 13);
        tmpAreaEntry->top = (u32)(gHeaderBuf->digest2_table_offset);
        tmpAreaEntry->bottom = (u32)(gHeaderBuf->digest2_table_offset + gHeaderBuf->digest2_table_size);
        gEntry->addAreaEntry( tmpAreaEntry);

        // magicon 領域を登録
        tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
        mEntry->InitializeEntry( tmpAreaEntry);
        mEntry->SetName( tmpAreaEntry, (char*)"A9-Ltd-Static", 13);
        tmpAreaEntry->top = (u32)(mHeaderBuf->ltd_arm9.romAddr);
        tmpAreaEntry->bottom = (u32)(mHeaderBuf->ltd_arm9.romAddr + mHeaderBuf->ltd_arm9.romSize);
        mEntry->addAreaEntry( tmpAreaEntry);

        tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
        mEntry->InitializeEntry( tmpAreaEntry);
        mEntry->SetName( tmpAreaEntry, (char*)"A7-Ltd-Static", 13);
        tmpAreaEntry->top = (u32)(mHeaderBuf->ltd_arm7.romAddr);
        tmpAreaEntry->bottom = (u32)(mHeaderBuf->ltd_arm7.romAddr + mHeaderBuf->ltd_arm7.romSize);
        mEntry->addAreaEntry( tmpAreaEntry);

        tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
        mEntry->InitializeEntry( tmpAreaEntry);
        mEntry->SetName( tmpAreaEntry, (char*)"(NTR-Digest Area)", 17);
        tmpAreaEntry->top = (u32)(mHeaderBuf->nitro_digest_area_rom_offset);
        tmpAreaEntry->bottom = (u32)(mHeaderBuf->nitro_digest_area_rom_offset + mHeaderBuf->nitro_digest_area_size);
        mEntry->addAreaEntry( tmpAreaEntry);

        tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
        mEntry->InitializeEntry( tmpAreaEntry);
        mEntry->SetName( tmpAreaEntry, (char*)"(TWL-Digest Area)", 17);
        tmpAreaEntry->top = (u32)(mHeaderBuf->twl_digest_area_rom_offset);
        tmpAreaEntry->bottom = (u32)(mHeaderBuf->twl_digest_area_rom_offset + mHeaderBuf->twl_digest_area_size);
        mEntry->addAreaEntry( tmpAreaEntry);

        tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
        mEntry->InitializeEntry( tmpAreaEntry);
        mEntry->SetName( tmpAreaEntry, (char*)"Digest1-Table", 13);
        tmpAreaEntry->top = (u32)(mHeaderBuf->digest1_table_offset);
        tmpAreaEntry->bottom = (u32)(mHeaderBuf->digest1_table_offset + mHeaderBuf->digest1_table_size);
        mEntry->addAreaEntry( tmpAreaEntry);

        tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
        mEntry->InitializeEntry( tmpAreaEntry);
        mEntry->SetName( tmpAreaEntry, (char*)"Digest2-Table", 13);
        tmpAreaEntry->top = (u32)(mHeaderBuf->digest2_table_offset);
        tmpAreaEntry->bottom = (u32)(mHeaderBuf->digest2_table_offset + mHeaderBuf->digest2_table_size);
        mEntry->addAreaEntry( tmpAreaEntry);
    }
}


void Checker::AnalyzeBanner( RomHeader* gHeaderBuf, Entry* gEntry, RomHeader* mHeaderBuf, Entry* mEntry)
{
    DiffLevel diffLevel;
    MyAreaEntry  *tmpAreaEntry;
    BannerHeader gBannerHeader;
    BannerHeader mBannerHeader;
    u32 banner_size[3] = {
        sizeof( BannerFileV1),
        sizeof( BannerFileV1) + sizeof( BannerFileV2),
        sizeof( BannerFileV1) + sizeof( BannerFileV2) + sizeof( BannerFileV3),
    };

    fseek( gfp, (u32)(gHeaderBuf->banner_offset), SEEK_SET);
    fseek( mfp, (u32)(mHeaderBuf->banner_offset), SEEK_SET);

    fread( &gBannerHeader, sizeof(BannerHeader), 1, gfp);
    fread( &mBannerHeader, sizeof(BannerHeader), 1, mfp);

    printf( "------- Banner Header -------\n");
    Diff( &diffLevel,
          (u32)(gHeaderBuf->banner_offset), sizeof(BannerHeader),
          (u32)(mHeaderBuf->banner_offset), sizeof(BannerHeader),
          false, PRINT_LEVEL_1);
    if( (((gBannerHeader.version) < 1) || ((gBannerHeader.version) > 3)) ||
        (((mBannerHeader.version) < 1) || ((mBannerHeader.version) > 3)))
    {
        printf( "  invalid banner version!\n");
        return;
    }
    printf( "------- Banner Body -------\n");
    Diff( &diffLevel,
          (u32)(gHeaderBuf->banner_offset) + sizeof(BannerHeader), banner_size[gBannerHeader.version],
          (u32)(mHeaderBuf->banner_offset) + sizeof(BannerHeader), banner_size[mBannerHeader.version],
          false, PRINT_LEVEL_1);

    /* 領域を登録 */
    tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
    gEntry->InitializeEntry( tmpAreaEntry);
    gEntry->SetName( tmpAreaEntry, (char*)"BannerHeader", 12);
    tmpAreaEntry->top = (u32)(gHeaderBuf->banner_offset);
    tmpAreaEntry->bottom = (u32)(gHeaderBuf->banner_offset + sizeof(BannerHeader));
    gEntry->addAreaEntry( tmpAreaEntry);

    tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
    gEntry->InitializeEntry( tmpAreaEntry);
    gEntry->SetName( tmpAreaEntry, (char*)"BannerBody", 10);
    tmpAreaEntry->top = (u32)(gHeaderBuf->banner_offset + sizeof(BannerHeader));
    tmpAreaEntry->bottom = (u32)(tmpAreaEntry->top + banner_size[gBannerHeader.version]);
    gEntry->addAreaEntry( tmpAreaEntry);

    tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
    mEntry->InitializeEntry( tmpAreaEntry);
    mEntry->SetName( tmpAreaEntry, (char*)"BannerHeader", 12);
    tmpAreaEntry->top = (u32)(mHeaderBuf->banner_offset);
    tmpAreaEntry->bottom = (u32)(mHeaderBuf->banner_offset + sizeof(BannerHeader));
    mEntry->addAreaEntry( tmpAreaEntry);

    tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
    mEntry->InitializeEntry( tmpAreaEntry);
    mEntry->SetName( tmpAreaEntry, (char*)"BannerBody", 10);
    tmpAreaEntry->top = (u32)(mHeaderBuf->banner_offset + sizeof(BannerHeader));
    tmpAreaEntry->bottom = (u32)(tmpAreaEntry->top + banner_size[mBannerHeader.version]);
    mEntry->addAreaEntry( tmpAreaEntry);
}

void Checker::AnalyzeOverlay( RomHeader* gHeaderBuf, Entry* gEntry, RomHeader* mHeaderBuf, Entry* mEntry)
{
    int     i;
    int     g_ovt_entries, m_ovt_entries;
    long    nowgfp, nowmfp;
    DiffLevel diffLevel;
    ROM_OVT g_ovtBuf, m_ovtBuf;
    ROM_FAT g_fatBuf, m_fatBuf;
    MyAreaEntry  *tmpAreaEntry;
    char    areaStr[32];

    nowgfp = ftell( gfp);
    nowmfp = ftell( mfp);

    // ARM9 Overlay
    printf( "------- ARM9 Overlay -------\n");
    g_ovt_entries = (gHeaderBuf->main_ovt_size) / sizeof(ROM_OVT);
    m_ovt_entries = (mHeaderBuf->main_ovt_size) / sizeof(ROM_OVT);

    for( i=0; i<g_ovt_entries; i++)
    {
        // file-id を読み出す
        fseek( gfp, ((u32)(gHeaderBuf->main_ovt_offset) + (sizeof(ROM_OVT) * i)), SEEK_SET);
        fseek( mfp, ((u32)(mHeaderBuf->main_ovt_offset) + (sizeof(ROM_OVT) * i)), SEEK_SET);
        fread( &g_ovtBuf, sizeof(ROM_OVT), 1, gfp);
        fread( &m_ovtBuf, sizeof(ROM_OVT), 1, mfp);

        // FAT を読み出す
        fseek( gfp, ((u32)(gHeaderBuf->fat_offset) + (sizeof(ROM_FAT) * g_ovtBuf.file_id)), SEEK_SET);
        fseek( mfp, ((u32)(mHeaderBuf->fat_offset) + (sizeof(ROM_FAT) * m_ovtBuf.file_id)), SEEK_SET);
        fread( &g_fatBuf, sizeof(ROM_FAT), 1, gfp);
        fread( &m_fatBuf, sizeof(ROM_FAT), 1, mfp);

        printf( "- overlay:%d, file_id:0x%lx\n", i, g_ovtBuf.file_id);
        Diff( &diffLevel,
              (u32)(g_fatBuf.top), ((u32)(g_fatBuf.bottom) - (u32)(g_fatBuf.top)),
              (u32)(m_fatBuf.top), ((u32)(m_fatBuf.bottom) - (u32)(m_fatBuf.top)),
              false, PRINT_LEVEL_1);

        // 領域を登録
        tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
        gEntry->InitializeEntry( tmpAreaEntry);
        sprintf( areaStr, "Arm9Overlay%d", i);
        gEntry->SetName( tmpAreaEntry, areaStr, 11+((i/10)+1));
        tmpAreaEntry->top = (u32)(g_fatBuf.top);
        tmpAreaEntry->bottom = (u32)(g_fatBuf.bottom);
        gEntry->addAreaEntry( tmpAreaEntry);

        tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
        mEntry->InitializeEntry( tmpAreaEntry);
        sprintf( areaStr, "Arm9Overlay%d", i);
        mEntry->SetName( tmpAreaEntry, areaStr, 11+((i/10)+1));
        tmpAreaEntry->top = (u32)(m_fatBuf.top);
        tmpAreaEntry->bottom = (u32)(m_fatBuf.bottom);
        mEntry->addAreaEntry( tmpAreaEntry);
    }

    // ARM7 Overlay
    printf( "\n");
    printf( "------- ARM7 Overlay -------\n");
    g_ovt_entries = (gHeaderBuf->sub_ovt_size) / sizeof(ROM_OVT);
    m_ovt_entries = (mHeaderBuf->sub_ovt_size) / sizeof(ROM_OVT);

    for( i=0; i<g_ovt_entries; i++)
    {
        // file-id を読み出す
        fseek( gfp, ((u32)(gHeaderBuf->sub_ovt_offset) + (sizeof(ROM_OVT) * i)), SEEK_SET);
        fseek( mfp, ((u32)(mHeaderBuf->sub_ovt_offset) + (sizeof(ROM_OVT) * i)), SEEK_SET);
        fread( &g_ovtBuf, sizeof(ROM_OVT), 1, gfp);
        fread( &m_ovtBuf, sizeof(ROM_OVT), 1, mfp);

        // FAT を読み出す
        fseek( gfp, ((u32)(gHeaderBuf->fat_offset) + (sizeof(ROM_FAT) * g_ovtBuf.file_id)), SEEK_SET);
        fseek( mfp, ((u32)(mHeaderBuf->fat_offset) + (sizeof(ROM_FAT) * m_ovtBuf.file_id)), SEEK_SET);
        fread( &g_fatBuf, sizeof(ROM_FAT), 1, gfp);
        fread( &m_fatBuf, sizeof(ROM_FAT), 1, mfp);

        printf( "- overlay:%d, file_id:0x%lx\n", i, g_ovtBuf.file_id);
        Diff( &diffLevel,
              (u32)(g_fatBuf.top), ((u32)(g_fatBuf.bottom) - (u32)(g_fatBuf.top)),
              (u32)(m_fatBuf.top), ((u32)(m_fatBuf.bottom) - (u32)(m_fatBuf.top)),
              false, PRINT_LEVEL_1);

        // 領域を登録
        tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
        gEntry->InitializeEntry( tmpAreaEntry);
        sprintf( areaStr, "Arm7Overlay%d", i);
        gEntry->SetName( tmpAreaEntry, areaStr, 11+((i/10)+1));
        tmpAreaEntry->top = (u32)(g_fatBuf.top);
        tmpAreaEntry->bottom = (u32)(g_fatBuf.bottom);
        gEntry->addAreaEntry( tmpAreaEntry);

        tmpAreaEntry = (MyAreaEntry*)malloc( sizeof(MyAreaEntry));
        mEntry->InitializeEntry( tmpAreaEntry);
        sprintf( areaStr, "Arm7Overlay%d", i);
        mEntry->SetName( tmpAreaEntry, areaStr, 11+((i/10)+1));
        tmpAreaEntry->top = (u32)(m_fatBuf.top);
        tmpAreaEntry->bottom = (u32)(m_fatBuf.bottom);
        mEntry->addAreaEntry( tmpAreaEntry);
    }

    // ファイルポインタを戻す
    fseek( gfp, nowgfp, SEEK_SET);
    fseek( mfp, nowmfp, SEEK_SET);
}


bool Checker::AnalyzeFNT( RomHeader* headerBuf, FILE* fp, Entry* entry, PrintLevel print_enable)
{
    int i;
    ROM_FNTDir     currentDir;
    MyDirEntry     tmpDirEntry;
    MyDirEntry*    pDirEntry;

    // ルートディレクトリの情報を読む
    fseek( fp, (u32)(headerBuf->fnt_offset), SEEK_SET);
    fread( &currentDir, sizeof(ROM_FNTDir), 1, fp);

    if( (currentDir.parent_id) >= 4096)
    {
        printf( "invalid FNT! directory count over 4096.\n");
        return false;
    }

    // ディレクトリテーブル全体を読む
    fseek( fp, (u32)(headerBuf->fnt_offset), SEEK_SET);
    fread( &fntBuf, sizeof(ROM_FNTDir) * currentDir.parent_id, 1, fp);
    // ルートディレクトリのparent_idは総ディレクトリ数を表す
    for( i=0; i<currentDir.parent_id; i++)
    {
        entry->InitializeEntry( &tmpDirEntry);
        if( i == 0)
        {
            if( print_enable) {
                printf( "------- dir_id : 0xf000 (root) -------\n");
            }
            tmpDirEntry.self_id = 0xF000;
        }
        else
        {
            if( print_enable) {
                printf( "------- dir_id : 0x%x (child dir of 0x%x) -------\n", (0xF000 + i), fntBuf[i].parent_id);
            }
            tmpDirEntry.self_id = (0xF000 + i);
            tmpDirEntry.parent_id = fntBuf[i].parent_id;
            if( fntBuf[i].parent_id < 0xF000)
            {
                printf( "invalid FNT! illegal parent-directory-id.\n");
                return false;
            }
        }
        if( !entry->FindDirEntry( tmpDirEntry.self_id))
        {   // 見つからなかったら追加
            pDirEntry = (MyDirEntry*)malloc( sizeof(MyDirEntry));
            entry->CopyEntry( pDirEntry, &tmpDirEntry);
            entry->addDirEntry( pDirEntry);
        }

        if( !FindEntry( fntBuf[i].entry_start,
                        fntBuf[i].entry_file_id,
                        headerBuf, fp, entry, tmpDirEntry.self_id,
                        print_enable))
            {
                return false;
            }
    }
    return true;
}

bool Checker::FindEntry( u32 fnt_offset, u16 entry_id, RomHeader* headerBuf, FILE* fp, Entry* entry, u16 parent_id, PrintLevel print_enable)
{
    EntryInfo entryInfo;
    char      entryNames[FILE_NAME_LENGTH];
    u16       dir_id;
    MyDirEntry* dirEntry;
    MyFileEntry* fileEntry;

    fseek( fp, (u32)(headerBuf->fnt_offset) + fnt_offset, SEEK_SET);
    while( 1)
    {
        fread( &entryInfo, sizeof(char), 1, fp);
        if( (entryInfo.entry_type == 0) && (entryInfo.entry_name_length == 0)) // 終端チェック
        {
            break;
        }
        if( entryInfo.entry_name_length == 0)
        {
            continue;
        }
        fread( entryNames, entryInfo.entry_name_length, 1, fp);
        entryNames[entryInfo.entry_name_length] = '\0';
        if( entryInfo.entry_type == 0) // ファイル
        {
            if( print_enable) {
                printf( "- %s(file_id:0x%d)\n", entryNames, entry_id);
            }
            if( entry_id >= 61440)
            {
                printf( "invalid FNT! file count over 61440.\n");
                return false;
            }
            /* パス解析用 */
            fileEntry = (MyFileEntry*)malloc( sizeof(MyFileEntry));
            entry->InitializeEntry( fileEntry);
            fileEntry->self_id = entry_id;
            fileEntry->parent_id = parent_id;
            
            entry->SetName( fileEntry, entryNames, entryInfo.entry_name_length);
            entry->addFileEntry( fileEntry);

            FindAllocation( entry_id, headerBuf, fp, entry, print_enable);
            entry_id++;
        }
        else // ディレクトリ
        {
            fread( &dir_id, sizeof(u16), 1, fp);
            if( print_enable) {
                printf( "- [%s](dir_id:0x%x)\n", entryNames, dir_id);
            }
            /* パス解析用 */
            dirEntry = entry->FindDirEntry( dir_id);
            if( !dirEntry)
            {   // 見つからなかったら追加
                dirEntry = (MyDirEntry*)malloc( sizeof(MyDirEntry));
                entry->InitializeEntry( dirEntry);
                dirEntry->self_id = dir_id;
                dirEntry->parent_id = parent_id;
                entry->addDirEntry( dirEntry);
            }
            entry->SetName( dirEntry, entryNames, entryInfo.entry_name_length);
        }
    }
    return true;
}

void Checker::FindAllocation( u16 entry_id, RomHeader* headerBuf, FILE* fp, Entry* entry, PrintLevel print_enable)
{
    DiffLevel diffLevel;
    ROM_FAT currentRomFat;
    long nowfp;
    nowfp = ftell( fp);

    fseek( fp,
           (u32)(headerBuf->fat_offset) + (sizeof(ROM_FAT) * entry_id),
           SEEK_SET);
    fread( &currentRomFat, sizeof(ROM_FAT), 1, fp);
    if( print_enable) {
        printf( "  fat top:0x%lx, bottom:0x%lx, len:0x%lx\n",
                (u32)(currentRomFat.top), (u32)(currentRomFat.bottom),
                (u32)((u32)(currentRomFat.bottom) - (u32)(currentRomFat.top)));
    }
    Diff( &diffLevel,
          (u32)(currentRomFat.top), (u32)(currentRomFat.bottom) - (u32)(currentRomFat.top),
          (u32)(currentRomFat.top), (u32)(currentRomFat.bottom) - (u32)(currentRomFat.top),
          true, print_enable);

    /* パス解析用 */
    MyFileEntry* fileEntry = entry->FindFileEntry( entry_id);
    fileEntry->top = (u32)(currentRomFat.top);
    fileEntry->bottom = (u32)(currentRomFat.bottom);

    // ファイルポインタを戻す
    fseek( fp, nowfp, SEEK_SET);
}


/* ROM内のBMPファイルを全て切り出して出力する */
#define BMP_BUFFER_SIZE (512*1024)
u32 tmpBuf[BMP_BUFFER_SIZE / 4];
void Checker::ExportGenuineBmpFiles( Entry* gEntry, PrintLevel print_enable)
{
    int i;
    MyFileEntry *currentEntry = gEntry->fileEntry;
//    u32* tmpBuf = (u32*)malloc( BMP_BUFFER_SIZE);
    u32  file_size, rest_size;
    int  loop_num;
    FILE* fp;

    while( currentEntry)
    {
        if( currentEntry->name_length > 4)
        {
            if( memcmp( &currentEntry->name[currentEntry->name_length - 4], ".bmp", 4) == 0)
            {
                file_size = (currentEntry->bottom - currentEntry->top);
                rest_size = file_size;

                loop_num = file_size / BMP_BUFFER_SIZE;
                fp = fopen( currentEntry->name, "w");

                fseek( gfp, currentEntry->top, SEEK_SET);
                if( rest_size > BMP_BUFFER_SIZE)
                {
                    for( i=0; i<loop_num; i++)
                    {
                        fread( tmpBuf, BMP_BUFFER_SIZE, 1, gfp);
                        fwrite( tmpBuf, BMP_BUFFER_SIZE, 1, fp);
                    }
                    rest_size = (rest_size % BMP_BUFFER_SIZE);
                }
                if( rest_size)
                {
                    fread( tmpBuf, rest_size, 1, gfp);
                    fwrite( tmpBuf, rest_size, 1, fp);
                }
                fclose( fp);
            }
        }
        if( print_enable) {
            printf( "- %s exported.\n", currentEntry->full_path_name);
        }
        currentEntry = (MyFileEntry*)(currentEntry->next);
    };
//    free( tmpBuf);
}

/* ディレクトリとファイルをチェックする */
void Checker::CheckAllEntries( RomHeader* mHeaderBuf, CARDRomHashContext *context, Entry* gEntry, Entry* mEntry)
{
    DiffLevel diffLevel;
    {
        MyDirEntry *currentEntry = gEntry->dirEntry;
        MyDirEntry *hisEntry;
        bool       isExistAll = true;

        printf( "------- directory check -------\n");
        while( currentEntry)
        {
            printf( "- %s", currentEntry->full_path_name);

            hisEntry = mEntry->FindDirEntry( currentEntry->full_path_name);
            if( hisEntry)
            {
                printf( "\n");
            }
            else
            {
                printf( " --->（存在していない）\n");
                isExistAll = false;
            }

            currentEntry = (MyDirEntry*)(currentEntry->next);
        }

        if( isExistAll)
        {
            printf( "\n");
            printf( "（以上の ディレクトリは 全て マジコン側にも存在している）\n");
        }
    }

    printf( "\n\n");
    MyFileEntry *currentEntry = gEntry->fileEntry;
    MyFileEntry *hisEntry;
    printf( "------- file check -------\n");
    while( currentEntry)
    {
        printf( "- %s", currentEntry->full_path_name);
        printf( "\n  file-id : 0x%x", currentEntry->self_id);

        hisEntry = mEntry->FindFileEntry( currentEntry->full_path_name);
        if( hisEntry)
        {
            printf( "\n");
            Diff( &diffLevel,
                  currentEntry->top, (currentEntry->bottom - currentEntry->top),
                  hisEntry->top, (hisEntry->bottom - hisEntry->top),
                  false, PRINT_LEVEL_1);
            if( diffLevel & DIFF_DATA_MODIFIED)
            {
                currentEntry->modified = true; // 改竄フラグ
            }
            if( mHeaderBuf->platform_code & 0x03) // TWL対応アプリなら
            {
                u8 d1, d2;
                CARD_CheckFileDigest( context, hisEntry, &d1, &d2);
                if( d1)
                {
                    printf( "（ファイル - digest1 検証はOK）\n");
                }
                else
                {
                    printf( "（ファイル - digest1 検証はNG）\n");
                }
                if( d2)
                {
                    printf( "（digest1 - digest2 検証はOK）\n");
                }
                else
                {
                    printf( "（digest1 - digest2 検証はNG）\n");
                }
                printf( "\n");
            }
        }
        else
        {
            printf( " --->（存在していない）\n");
        }

        currentEntry = (MyFileEntry*)(currentEntry->next);
    }
}


u32 Checker::GetOctValue( char* hex_char)
{
    u8 num = (u32)(*(u8*)hex_char);

    if( (num >= '0')&&(num <= '9'))
    {
        return num - 0x30;
    }
    else if( (num >= 'a')&&(num <= 'f'))
    {
        return (num - 0x61) + 10;
    }
    else if( (num >= 'A')&&(num <= 'F'))
    {
        return (num - 0x41) + 10;
    }
    return 0;
}

char logBuf[0x46];
void Checker::FindAccessLogFile( RomHeader* gHeaderBuf, Entry* entry, Entry* mEntry, FILE* lfp, CARDRomHashContext *context)
{
    int i = 0;
    DiffLevel diffLevel;
    u8 d1, d2, dm1, dm2;
    u32 log_start_adr, log_end_adr;
    u32 m_log_start_adr, m_log_end_adr;
    MyFileEntry* gFileEntry;
    MyFileEntry* mFileEntry;

    while( fread( logBuf, 6, 1, lfp))
    {
        if( memcmp( logBuf, "Read: ", 4) == 0)
        {
            fread( logBuf, 0x25, 1, lfp);
            log_start_adr = (GetOctValue(&logBuf[0x9]) +
                             (GetOctValue(&logBuf[0x8]) * 0x10) +
                             (GetOctValue(&logBuf[0x7]) * 0x100) +
                             (GetOctValue(&logBuf[0x6]) * 0x1000) +
                             (GetOctValue(&logBuf[0x5]) * 0x10000) +
                             (GetOctValue(&logBuf[0x4]) * 0x100000) +
                             (GetOctValue(&logBuf[0x3]) * 0x1000000) +
                             (GetOctValue(&logBuf[0x2]) * 0x10000000));

            log_end_adr = (GetOctValue(&logBuf[0x14]) +
                           (GetOctValue(&logBuf[0x13]) * 0x10) +
                           (GetOctValue(&logBuf[0x12]) * 0x100) +
                           (GetOctValue(&logBuf[0x11]) * 0x1000) +
                           (GetOctValue(&logBuf[0x10]) * 0x10000) +
                           (GetOctValue(&logBuf[0x0F]) * 0x100000) +
                           (GetOctValue(&logBuf[0x0E]) * 0x1000000) +
                           (GetOctValue(&logBuf[0x0D]) * 0x10000000));
            printf( "%d   0x%lx - 0x%lx", i, log_start_adr, log_end_adr);

            if( (log_end_adr - log_start_adr)>= 1000000)
            {
                fread( logBuf, 1, 1, lfp);
            }

            gFileEntry = entry->FindFileLocation( log_start_adr, log_end_adr);
            if( gFileEntry)
            {   // 当該ファイルのアクセスログをマジコン側に変換（ファイルの位置が改竄されている場合のため）
                mFileEntry = mEntry->FindFileEntry( gFileEntry->full_path_name);
                if( mFileEntry)
                {   // マジコン側に存在している場合
                    m_log_start_adr = (log_start_adr - gFileEntry->top) + mFileEntry->top;
                    m_log_end_adr = (log_end_adr - gFileEntry->top) + mFileEntry->top;
                }
                else
                {   // マジコン側に存在していない場合
                    m_log_start_adr = log_start_adr;
                    m_log_end_adr = log_end_adr;
                }
                
                // アクセスログが異なる場合はそれを明示
                if( (log_start_adr != m_log_start_adr)||(log_end_adr != m_log_end_adr))
                {
                    printf( " -> (0x%lx - 0x%lx)", m_log_start_adr, m_log_end_adr);
                }
                // マジコン側に存在していなければその旨を表示
                if( !mFileEntry)
                {
                    printf( " マジコン側には存在していない,");
                }
                else
                {
                // 存在していれば、ファイル名とファイルとしての改竄有無を表示
                if( gFileEntry->modified)
                {
                    printf( " %s(*),", gFileEntry->full_path_name);
                }else{
                    printf( " %s,", gFileEntry->full_path_name);
                }
                }
                // 当該アクセスログにおける改竄の有無を表示
                Diff( &diffLevel,
                      log_start_adr, (log_end_adr - log_start_adr),
                      m_log_start_adr, (m_log_end_adr - m_log_start_adr),
                      true, PRINT_LEVEL_0);
                if( diffLevel & DIFF_OUT_OF_RANGE)
                {
                    printf( "[out of range]");
                }
                else if( diffLevel & DIFF_DATA_MODIFIED)
                {
                    printf( "[data(*)]");
                }
                else if( diffLevel & DIFF_DATA_FILLED)
                {
                    printf( "[data(f)]");
                }
                else
                {
                    printf( "[data]");
                }

                if( gHeaderBuf->platform_code & 0x03)
                {
                    GetDigestResult( context, log_start_adr, log_end_adr, &d1, &d2);
                    IsDigestModified( context, log_start_adr, log_end_adr, &dm1, &dm2);
                    if( dm1)
                    {
                        if( d1) { printf( "[d1:OK]");} else { printf( "[d1:NG]");};
                    }
                    else
                    {
                        if( d1) { printf( "[d1(*):OK]");} else { printf( "[d1(*):NG]");};
                    }
                    if( dm2)
                    {
                        if( d2) { printf( "[d2:OK]");} else { printf( "[d2:NG]");};
                    }
                    else
                    {
                        if( d2) { printf( "[d2(*):OK]");} else { printf( "[d2(*):NG]");};
                    }
                }
                // 領域名も表示
                entry->FindAreaLocation( log_start_adr, log_end_adr);
            }
            else
            {   // ファイルが該当しなかったら領域名の表示と内容比較
                entry->FindAreaLocation( log_start_adr, log_end_adr);
                // TODO:genuine側の対応アドレスはgenuineファイルエントリの先頭から計算し直す
                Diff( &diffLevel,
                      log_start_adr, (log_end_adr - log_start_adr),
                      log_start_adr, (log_end_adr - log_start_adr),
                      true, PRINT_LEVEL_0);
                if( diffLevel & DIFF_OUT_OF_RANGE)
                {
                    printf( "[out of range]");
                }
                else if( diffLevel & DIFF_DATA_MODIFIED)
                {
                    printf( "[data(*)]");
                }
                else if( diffLevel & DIFF_DATA_FILLED)
                {
                    printf( "[data(f)]");
                }
                else
                {
                    printf( "[data]");
                }
            }
            printf( "\n");
        }
        else if( memcmp( logBuf, "<<BACK", 4) == 0)
        {
            printf( "<<backup access>>\n");
            fread( logBuf, 12, 1, lfp);
        }
        else if( memcmp( logBuf, "<<INVA", 4) == 0)
        {
            printf( "<<INVALID access>>\n");
            fread( logBuf, 12, 1, lfp);
        }
        else
        {
            printf( "<<unknown access>>\n");
            fread( logBuf, 12, 1, lfp);
        }
        i++;
    };
}

void Checker::AnalyzeAccessLog( RomHeader* gHeaderBuf, Entry* entry, Entry* mEntry, FILE* lfp)
{
    int i = 0;
    DiffLevel diffLevel;
    u32 log_start_adr, log_end_adr;
    u32 total = 0;
    MyFileEntry* gFileEntry;

    while( fread( logBuf, 6, 1, lfp))
    {
        if( memcmp( logBuf, "Read: ", 4) == 0)
        {
            fread( logBuf, 0x25, 1, lfp);
            log_start_adr = (GetOctValue(&logBuf[0x9]) +
                             (GetOctValue(&logBuf[0x8]) * 0x10) +
                             (GetOctValue(&logBuf[0x7]) * 0x100) +
                             (GetOctValue(&logBuf[0x6]) * 0x1000) +
                             (GetOctValue(&logBuf[0x5]) * 0x10000) +
                             (GetOctValue(&logBuf[0x4]) * 0x100000) +
                             (GetOctValue(&logBuf[0x3]) * 0x1000000) +
                             (GetOctValue(&logBuf[0x2]) * 0x10000000));

            log_end_adr = (GetOctValue(&logBuf[0x14]) +
                           (GetOctValue(&logBuf[0x13]) * 0x10) +
                           (GetOctValue(&logBuf[0x12]) * 0x100) +
                           (GetOctValue(&logBuf[0x11]) * 0x1000) +
                           (GetOctValue(&logBuf[0x10]) * 0x10000) +
                           (GetOctValue(&logBuf[0x0F]) * 0x100000) +
                           (GetOctValue(&logBuf[0x0E]) * 0x1000000) +
                           (GetOctValue(&logBuf[0x0D]) * 0x10000000));

            total += log_end_adr - log_start_adr;
            printf( "%d,   0x%lx, 0x%lx, %ld", i, log_start_adr, log_end_adr, log_end_adr - log_start_adr);

            if( (log_end_adr - log_start_adr)>= 1000000)
            {
                fread( logBuf, 1, 1, lfp);
            }
/*            
            gFileEntry = entry->FindFileLocation( log_start_adr, log_end_adr);
            if( gFileEntry)
            {
                // 領域名も表示
                entry->FindAreaLocation( log_start_adr, log_end_adr);
            }
            else*/
            {   // ファイルが該当しなかったら領域名の表示と内容比較
//                entry->FindAreaLocation( log_start_adr, log_end_adr);
                // TODO:genuine側の対応アドレスはgenuineファイルエントリの先頭から計算し直す
/*                Diff( &diffLevel,
                      log_start_adr, (log_end_adr - log_start_adr),
                      log_start_adr, (log_end_adr - log_start_adr),
                      true, PRINT_LEVEL_0);
                if( diffLevel & DIFF_OUT_OF_RANGE)
                {
                    printf( "[out of range]");
                }
                else if( diffLevel & DIFF_DATA_MODIFIED)
                {
                    printf( "[data(*)]");
                }
                else if( diffLevel & DIFF_DATA_FILLED)
                {
                    printf( "[data(f)]");
                }
                else
                {
                    printf( "[data]");
                }*/
            }
            printf( "\n");
        }
        else if( memcmp( logBuf, "<<BACK", 4) == 0)
        {
            printf( "<<backup access>>\n");
            fread( logBuf, 12, 1, lfp);
        }
        else if( memcmp( logBuf, "<<INVA", 4) == 0)
        {
            printf( "<<INVALID access>>\n");
            fread( logBuf, 12, 1, lfp);
        }
        else
        {
            printf( "<<unknown access>>\n");
            fread( logBuf, 12, 1, lfp);
        }
        i++;
    };
    printf( "total, %ld\n", total);
}

void Checker::Finalize( void)
{
}

