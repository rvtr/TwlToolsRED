
#include <string.h>
#include "checker.h"
#include "nitro_romheader.h"

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
    if( readed == 1)
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

bool Checker::Diff( u32 g_offset, u32 g_size, u32 m_offset, u32 m_size, bool isDataOnly, PrintLevel print_enable)
{
    long nowgfp, nowmfp;
    int result = 0;
    u32 check_size, rest_size;
    int i, loop_num;
    bool function_result = true;

    check_size = (g_size < m_size)? g_size : m_size;
    rest_size = check_size;
    
    if( !isDataOnly)
    {
        /* 指定アドレスとサイズのチェック */
        if( g_offset == m_offset)
        {
            if( (print_enable)&&(print_enable < PRINT_LEVEL_2)) {
                printf( "  offset:0x%x（改竄されていない）\n", g_offset);
            }
        }
        else
        {
            function_result = false;
            if( print_enable) {
                printf( "  offset:0x%x ---> offset:0x%x（改竄されている）\n", g_offset, m_offset);
            }
        }
    
        if( g_size == m_size)
        {
            if( (print_enable)&&(print_enable < PRINT_LEVEL_2)) {
                printf( "  size:0x%x（改竄されていない）\n", g_size);
            }
        }
        else
        {
            function_result = false;
            if( print_enable) {
                printf( "  size:0x%x ---> size:0x%x（改竄されている）\n", g_size, m_size);
            }
        }
    }

    bool filled = true;
    int  totalResult = 0;
    int  j;
    
    nowgfp = ftell( gfp);
    nowmfp = ftell( mfp);
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
            for( j=0; j<(int)rest_size; j++)
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
    if( result == 0)
    {
        if( (print_enable)&&(print_enable < PRINT_LEVEL_2)) {
            printf( "  data:（改竄されていない）\n");
        }
    }
    else
    {
        function_result = false;
        if( filled)
        {
            if( print_enable) {
                printf( "  data:（0x%xでフィルされている）\n", *((u8*)mBuf));
            }
        }
        else
        {
            if( print_enable) {
                printf( "  data:（改竄されている）\n");
            }
        }
    }

    // ファイルポインタを戻す
    fseek( gfp, nowgfp, SEEK_SET);
    fseek( mfp, nowmfp, SEEK_SET);
    return function_result;
}


void Checker::AnalyzeBanner( RomHeader* gHeaderBuf, RomHeader* mHeaderBuf)
{
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
    Diff( (u32)(gHeaderBuf->banner_offset), sizeof(BannerHeader),
          (u32)(mHeaderBuf->banner_offset), sizeof(BannerHeader),
          false, PRINT_LEVEL_1);
    if( (((gBannerHeader.version) < 1) || ((gBannerHeader.version) > 3)) ||
        (((mBannerHeader.version) < 1) || ((mBannerHeader.version) > 3)))
    {
        printf( "  invalid banner version!\n");
        return;
    }            
    printf( "------- Banner Body -------\n");
    Diff( (u32)(gHeaderBuf->banner_offset) + sizeof(BannerHeader), banner_size[gBannerHeader.version],
          (u32)(mHeaderBuf->banner_offset) + sizeof(BannerHeader), banner_size[mBannerHeader.version],
          false, PRINT_LEVEL_1);
}


void Checker::AnalyzeFNT( RomHeader* headerBuf, FILE* fp, Entry* entry, PrintLevel print_enable)
{
    int i;
    ROM_FNTDir     currentDir;
    MyDirEntry     tmpDirEntry;
    MyDirEntry*    pDirEntry;

    // ルートディレクトリの情報を読む
    fseek( fp, (u32)(headerBuf->fnt_offset), SEEK_SET);
    fread( &currentDir, sizeof(ROM_FNTDir), 1, fp);

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
        }
        if( !entry->FindDirEntry( tmpDirEntry.self_id))
        {   // 見つからなかったら追加
            pDirEntry = (MyDirEntry*)malloc( sizeof(MyDirEntry));
            entry->CopyEntry( pDirEntry, &tmpDirEntry);
            entry->addDirEntry( pDirEntry);
        }
        
        FindEntry( fntBuf[i].entry_start,
                   fntBuf[i].entry_file_id,
                   headerBuf, fp, entry, tmpDirEntry.self_id,
                   print_enable);
    }
}

void Checker::FindEntry( u32 fnt_offset, u16 entry_id, RomHeader* headerBuf, FILE* fp, Entry* entry, u16 parent_id, PrintLevel print_enable)
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
        fread( entryNames, entryInfo.entry_name_length, 1, fp);
        entryNames[entryInfo.entry_name_length] = '\0';
        if( entryInfo.entry_type == 0) // ファイル
        {
            if( print_enable) {
                printf( "- %s(file_id:0x%d)\n", entryNames, entry_id);
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
}

void Checker::FindAllocation( u16 entry_id, RomHeader* headerBuf, FILE* fp, Entry* entry, PrintLevel print_enable)
{
    ROM_FAT currentRomFat;
    long nowfp;
    nowfp = ftell( fp);

    fseek( fp,
           (u32)(headerBuf->fat_offset) + (sizeof(ROM_FAT) * entry_id),
           SEEK_SET);
    fread( &currentRomFat, sizeof(ROM_FAT), 1, fp);
    if( print_enable) {
        printf( "  fat top:0x%x, bottom:0x%x, len:0x%x\n",
                (u32)(currentRomFat.top), (u32)(currentRomFat.bottom),
                (u32)(currentRomFat.bottom) - (u32)(currentRomFat.top));
    }
    Diff( (u32)(currentRomFat.top), (u32)(currentRomFat.bottom) - (u32)(currentRomFat.top),
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
    MyFileEntry *hisEntry;
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
void Checker::CheckAllEntries( Entry* gEntry, Entry* mEntry)
{
    {
        MyDirEntry *currentEntry = gEntry->dirEntry;
        MyDirEntry *hisEntry;
        
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
            }
        
            currentEntry = (MyDirEntry*)(currentEntry->next);
        }
    
    }
    
    MyFileEntry *currentEntry = gEntry->fileEntry;
    MyFileEntry *hisEntry;
    printf( "------- file check -------\n");
    while( currentEntry)
    {
        printf( "- %s", currentEntry->full_path_name);
        
        hisEntry = mEntry->FindFileEntry( currentEntry->full_path_name);
        if( hisEntry)
        {
            printf( "\n");
            if( Diff( currentEntry->top, (currentEntry->bottom - currentEntry->top),
                      hisEntry->top, (hisEntry->bottom - hisEntry->top),
                      false, PRINT_LEVEL_1) == false)
            {
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


void Checker::Finalize( void)
{
}

