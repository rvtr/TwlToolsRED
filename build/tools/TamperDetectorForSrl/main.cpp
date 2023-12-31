
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "searcharg.h"
//#include "nitro_romheader.h"
#include "twl_format_rom.h"
#include "checker.h"
#include "card_hash.h"


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
    unsigned short usi = (unsigned short)(~0U);
    unsigned long  uli = (unsigned long)(~0U);

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
    static CARDRomHashContext context;
    
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

        // ダイジェスト検証(digest1, digest2)
        if( gHeaderBuf.platform_code & 0x03)
        {
            CARDi_Init( &context, &mHeaderBuf);
            CARD_DiffDigest( &context, &gHeaderBuf, gfp, &mHeaderBuf, mfp);
            CARD_CheckHash( &context, &mHeaderBuf, mfp);
        }
        
        printf( "------------------\n");
        printf( "Nitro Rom Header\n");
        checker.AnalyzeHeader( &gHeaderBuf, &gEntry, &mHeaderBuf, &mEntry);

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
            checker.CheckAllEntries( &mHeaderBuf, &context, &gEntry, &mEntry);
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
            checker.FindAccessLogFile( &gHeaderBuf, &gEntry, &mEntry, lfp, &context);
            printf( "------------------\n");
            fclose( lfp);
        }

        checker.Finalize();
        fclose( gfp);
        fclose( mfp);
    }

    // マジコン名なし かつ Outputファイルあり
    if( ((!magicon_fname) && output_fname) && log_fname)
    {
        FILE* gfp;
        FILE* lfp;
        Checker checker;

        gfp = fopen( genuine_fname, "r");
        lfp = fopen( log_fname, "r");

        checker.Initialize( gfp, NULL, gBuf, NULL, BUFFER_SIZE);

        checker.LoadHeader( &gHeaderBuf, NULL);

        checker.AnalyzeAccessLog( &gHeaderBuf, &gEntry, (Entry*)NULL, lfp);
        
        checker.Finalize();

        fclose( lfp);
        fclose( gfp);
    }

    return 0;
}

