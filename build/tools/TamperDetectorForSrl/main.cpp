
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "searcharg.h"
#include "nitro_romheader.h"
#include "checker.h"


extern char*  output_fname;
extern char*  genuine_fname;
extern char*  magicon_fname;
extern char*  log_fname;

#define BUFFER_SIZE (0x4000)

Entry gEntry;
Entry mEntry;

RomHeader gHeaderBuf;
RomHeader mHeaderBuf;
char gBuf[BUFFER_SIZE];
char mBuf[BUFFER_SIZE];


/*--- 型のビット数をチェック ---*/
bool int_bits(void)
{
    int	 count  = 0;
    unsigned short usi = ~0U;
    unsigned long  uli = ~0U;

    while (uli) {
        if (uli & 1U) count++;
        uli >>= 1;
    }
    if( count != 32)
    {
        printf("ERROR! unsigned long int : %d bits\n", count);
        return false;
    }

    count = 0;
    while (usi) {
        if (usi & 1U) count++;
        usi >>= 1;
    }
    if( count != 16)
    {
        printf("ERROR! unsigned short int : %d bits\n", count);
        return false;
    }

    return true;
}


int main (int argc, char *argv[])
{
    // 処理系の unsignedビット数が想定外ならエラー終了（types.hを変更してビルドし直してください）
    if( !int_bits())
    {
        return 1;
    }
    
    SA_searchopt(argc, argv);

    printf("[output_fname]%s\n", output_fname);
    printf("[genuine_fname]%s\n", genuine_fname);
    printf("[magicon_fname]%s\n", magicon_fname);
    printf("[log_fname]%s\n", log_fname);

    if( !genuine_fname)
    {
        SA_Usage();
    }

    if( magicon_fname)
    {
        FILE* gfp;
        FILE* mfp;
        Checker checker;

        gfp = fopen( genuine_fname, "r");
        mfp = fopen( magicon_fname, "r");

        checker.Initialize( gfp, mfp, gBuf, mBuf, BUFFER_SIZE);
        checker.LoadHeader( &gHeaderBuf, &mHeaderBuf);
        
        checker.AnalyzeHeader( &gHeaderBuf, &gEntry, &mHeaderBuf, &mEntry);

        printf( "------------------\n");
        printf( "Nitro Rom Header\n");
        checker.Diff( 0, sizeof(RomHeader), 0, sizeof(RomHeader), false, PRINT_LEVEL_1);
        printf( "------------------\n");

        printf( "ARM9 Static Module\n");
        checker.Diff( (u32)(gHeaderBuf.arm9.romAddr),
                      (u32)(gHeaderBuf.arm9.romSize),
                      (u32)(mHeaderBuf.arm9.romAddr),
                      (u32)(mHeaderBuf.arm9.romSize),
                      false, PRINT_LEVEL_1);
        printf( "------------------\n");
        
        printf( "ARM7 Static Module\n");
        checker.Diff( (u32)(gHeaderBuf.arm7.romAddr),
                      (u32)(gHeaderBuf.arm7.romSize),
                      (u32)(mHeaderBuf.arm7.romAddr),
                      (u32)(mHeaderBuf.arm7.romSize),
                      false, PRINT_LEVEL_1);
        printf( "------------------\n");

        printf( "File Name Table\n");
        checker.Diff( (u32)(gHeaderBuf.fnt_offset),
                      (u32)(gHeaderBuf.fnt_size),
                      (u32)(mHeaderBuf.fnt_offset),
                      (u32)(mHeaderBuf.fnt_size),
                      false, PRINT_LEVEL_1);
        printf( "------------------\n");

        printf( "File Allocation Table\n");
        checker.Diff( (u32)(gHeaderBuf.fat_offset),
                      (u32)(gHeaderBuf.fat_size),
                      (u32)(mHeaderBuf.fat_offset),
                      (u32)(mHeaderBuf.fat_size),
                      false, PRINT_LEVEL_1);
        printf( "------------------\n");
        
        printf( "ARM9 Overlay Table\n");
        checker.Diff( (u32)(gHeaderBuf.main_ovt_offset),
                      (u32)(gHeaderBuf.main_ovt_size),
                      (u32)(mHeaderBuf.main_ovt_offset),
                      (u32)(mHeaderBuf.main_ovt_size),
                      false, PRINT_LEVEL_1);
        printf( "------------------\n");

        printf( "ARM7 Overlay Table\n");
        checker.Diff( (u32)(gHeaderBuf.sub_ovt_offset),
                      (u32)(gHeaderBuf.sub_ovt_size),
                      (u32)(mHeaderBuf.sub_ovt_offset),
                      (u32)(mHeaderBuf.sub_ovt_size),
                      false, PRINT_LEVEL_1);
        printf( "------------------\n");

        printf( "\nBanner\n");
        checker.AnalyzeBanner( &gHeaderBuf, &gEntry, &mHeaderBuf, &mEntry);
        
        printf( "\nOverlay\n");
        checker.AnalyzeOverlay( &gHeaderBuf, &gEntry, &mHeaderBuf, &mEntry);
        
        printf( "\nFNT & FAT\n");
        checker.AnalyzeFNT( &gHeaderBuf, gfp, &gEntry, PRINT_LEVEL_0);
        gEntry.FollowParent();
        gEntry.AutoSetFullPath();

        if( checker.AnalyzeFNT( &mHeaderBuf, mfp, &mEntry, PRINT_LEVEL_0))
        {
            mEntry.FollowParent();
            mEntry.AutoSetFullPath();
            checker.CheckAllEntries( &gEntry, &mEntry);
        }
        checker.ExportGenuineBmpFiles( &gEntry, PRINT_LEVEL_0);
/*        
        gEntry.PrintAllDirEntry();
        gEntry.PrintAllFileEntry();
        
        mEntry.PrintAllDirEntry();
        mEntry.PrintAllFileEntry();
  */
        printf( "------------------\n");

        
//        AnalyzeFNT( mHeaderBuf, mfp);

        if( log_fname)
        {
            FILE* lfp;
        
            lfp = fopen( log_fname, "r");
            printf( "\n\n\nACCESS LOG\n");
            checker.FindAccessLogFile( &gEntry, lfp);
            printf( "------------------\n");
        }

        fclose( gfp);
        fclose( mfp);
    }

    return 0;
}

