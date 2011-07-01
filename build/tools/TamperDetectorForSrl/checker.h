
#ifndef CHECKER_H_
#define CHECKER_H_


#include <stdio.h>
#include <stdlib.h>
#include "types.h"
//#include "nitro_romheader.h"
#include "twl_format_rom.h"
#include "banner.h"
#include "entry.h"

typedef struct
{
  u8	entry_name_length:7;			// ファイル名の長さ (0-127)
  u8	entry_type       :1;			// ファイルエントリの場合は 0
} EntryInfo;

typedef enum
{
    PRINT_LEVEL_0 = 0,
    PRINT_LEVEL_1,
    PRINT_LEVEL_2
} PrintLevel;


class Checker
{
  private:
    bool  initialized;
    FILE* gfp;
    FILE* mfp;
    void* gBuf;
    void* mBuf;
    u32   buffer_size;
    ROM_FNTDir fntBuf[4096];
    void* dirTableBuf;

  public:
    void Initialize( FILE* myGfp, FILE* myMfp, void* myGbuf, void* myMbuf, u32 size);
    
    /* ヘッダを読むだけ */
    bool LoadHeader( void* gHeaderBuf, void* mHeaderBuf);

    /* ROMの特定領域に差がないかどうか調べる */
    bool Diff( u32 g_offset, u32 g_size, u32 m_offset, u32 m_size, bool isDataOnly, PrintLevel print_enable);
    
    void Finalize( void);

    /* ROMヘッダの各領域を管理リストに登録する */
    void AnalyzeHeader( RomHeader* gHeaderBuf, Entry* gEntry, RomHeader* mHeaderBuf, Entry* mEntry);

    /* ROMのバナー領域に対して Diff をかける */
    void AnalyzeBanner( RomHeader* gHeaderBuf, Entry* gEntry, RomHeader* mHeaderBuf, Entry* mEntry);

    /* Overlayテーブルに登録されている各ファイルに対して Diff をかける */
    void AnalyzeOverlay( RomHeader* gHeaderBuf, Entry* gEntry, RomHeader* mHeaderBuf, Entry* mEntry);

    /* FNT と FAT を解析して、各ファイルに対して Diff をかける */
    bool AnalyzeFNT( RomHeader* headerBuf, FILE* fp, Entry* entry, PrintLevel print_enable);
    bool FindEntry( u32 fnt_offset, u16 entry_id, RomHeader* headerBuf, FILE* fp, Entry* entry, u16 parent_id, PrintLevel print_enable);
    void FindAllocation( u16 entry_id, RomHeader* headerBuf, FILE* fp, Entry* entry, PrintLevel print_enable);

    void CheckAllEntries( Entry* gEntry, Entry* mEntry);

    /* ROM内のBMPファイルを全て切り出して出力する */
    void ExportGenuineBmpFiles( Entry* gEntry, PrintLevel print_enable);



    u32 GetOctValue( char* hex_char);
    void FindAccessLogFile( Entry* entry, FILE* lfp);
    
};

#endif
