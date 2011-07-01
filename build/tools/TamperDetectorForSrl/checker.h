
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
  u8	entry_name_length:7;			// �t�@�C�����̒��� (0-127)
  u8	entry_type       :1;			// �t�@�C���G���g���̏ꍇ�� 0
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
    
    /* �w�b�_��ǂނ��� */
    bool LoadHeader( void* gHeaderBuf, void* mHeaderBuf);

    /* ROM�̓���̈�ɍ����Ȃ����ǂ������ׂ� */
    bool Diff( u32 g_offset, u32 g_size, u32 m_offset, u32 m_size, bool isDataOnly, PrintLevel print_enable);
    
    void Finalize( void);

    /* ROM�w�b�_�̊e�̈���Ǘ����X�g�ɓo�^���� */
    void AnalyzeHeader( RomHeader* gHeaderBuf, Entry* gEntry, RomHeader* mHeaderBuf, Entry* mEntry);

    /* ROM�̃o�i�[�̈�ɑ΂��� Diff �������� */
    void AnalyzeBanner( RomHeader* gHeaderBuf, Entry* gEntry, RomHeader* mHeaderBuf, Entry* mEntry);

    /* Overlay�e�[�u���ɓo�^����Ă���e�t�@�C���ɑ΂��� Diff �������� */
    void AnalyzeOverlay( RomHeader* gHeaderBuf, Entry* gEntry, RomHeader* mHeaderBuf, Entry* mEntry);

    /* FNT �� FAT ����͂��āA�e�t�@�C���ɑ΂��� Diff �������� */
    bool AnalyzeFNT( RomHeader* headerBuf, FILE* fp, Entry* entry, PrintLevel print_enable);
    bool FindEntry( u32 fnt_offset, u16 entry_id, RomHeader* headerBuf, FILE* fp, Entry* entry, u16 parent_id, PrintLevel print_enable);
    void FindAllocation( u16 entry_id, RomHeader* headerBuf, FILE* fp, Entry* entry, PrintLevel print_enable);

    void CheckAllEntries( Entry* gEntry, Entry* mEntry);

    /* ROM����BMP�t�@�C����S�Đ؂�o���ďo�͂��� */
    void ExportGenuineBmpFiles( Entry* gEntry, PrintLevel print_enable);



    u32 GetOctValue( char* hex_char);
    void FindAccessLogFile( Entry* entry, FILE* lfp);
    
};

#endif
