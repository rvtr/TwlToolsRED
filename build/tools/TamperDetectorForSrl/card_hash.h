
#ifndef CARD_HASH_H_
#define CARD_HASH_H_

#include "entry.h"


// �����_��CARD���C�u�������Ó��Ɣ��f�����萔�B
// �p�t�H�[�}���X�v���̌��ʂɂ���ēK�X�ύX���Ă��悢�B
static const u32    CARD_ROM_HASH_BLOCK_MAX = 4;
static const u32    CARD_ROM_HASH_SECTOR_MAX = 32;


// ROM�̈���\����
typedef struct CARDRomRegion
{
    u32     offset;
    u32     length;
}
CARDRomRegion;



// SRL�t�@�C���̃n�b�V���Ǘ��\���́B
// ���v�T�C�Y�̓v���O�������ƂɈقȂ�ÓI�ɎZ�o�ł��Ȃ�����
// ���������ɃA���[�i����K�ʂ������I�Ɋm�ۂ���\��B
// �K�v�ƂȂ郁�����͈ȉ��̒ʂ�B
//   - �}�X�^�[�n�b�V���e�[�u��:
//       ROM�w�b�_�ƃn�b�V����r���Đ������𔻒肷��K�v��A
//       ����������ROM����ꊇ���[�h���ď풓�����Ă����B
//       (ROM�T�C�Y/�Z�N�^�T�C�Y/�Z�N�^�P��)*20(SHA1)�o�C�g�K�v�ŁA
//       1Gbits:1024�o�C�g:32�Z�N�^�Ȃ�Ζ�80kB�ƂȂ�B
//   - �u���b�N�L���b�V��:
//       �u���b�N�P�ʂ�ROM�C���[�W�L���b�V���Ƃ��̃n�b�V�����Ǘ�����B
//       �u���b�N���E���܂��������U�I�ȃA�N�Z�X���l������
//       ��ɕ�����ێ��ł��郊�X�g�\���ɂ��Ă����K�v������B
//       (20 * �Z�N�^�P�� + ��) �̍\���̂����X�g�����̕������K�v�B
//   - �Z�N�^�L���b�V��:
//       ���ۂ̃C���[�W�L���b�V����ێ�����B
//       �u���b�N���S�Z�N�^�����܂˂��Q�Ƃ���Ƃ͌���Ȃ�����
//       �Z�N�^���ʓr���X�g�\���Ƃ��ĊǗ�����K�v������B
typedef struct CARDRomHashContext
{
    // ROM�w�b�_����擾�����{�ݒ�
    CARDRomRegion       area_ntr;
    CARDRomRegion       area_ltd;
    CARDRomRegion       sector_hash;
    CARDRomRegion       block_hash;
    u32                 bytes_per_sector;
    u32                 sectors_per_block;
    u32                 block_max;
    u32                 sector_max;

    // �f�[�^�ƃn�b�V�������[�h����f�o�C�X�C���^�t�F�[�X
//    void                   *userdata;
//    MIDeviceReadFunction    ReadSync;
//    MIDeviceReadFunction    ReadAsync;

    // ���[�h�������̃X���b�h�B
//    OSThread           *loader;
//    void               *recent_load;
/*
    // �Z�N�^�ƃu���b�N�̃L���b�V��
    CARDRomHashSector  *loading_sector; // ���f�B�A���[�h�҂��Z�N�^
    CARDRomHashSector  *loaded_sector;  // �n�b�V�����ؑ҂��Z�N�^
    CARDRomHashSector  *valid_sector;   // ���������؍ς݃Z�N�^
    CARDRomHashBlock   *loading_block;  // ���f�B�A���[�h�҂��u���b�N
    CARDRomHashBlock   *loaded_block;   // �n�b�V�����ؑ҂��u���b�N
    CARDRomHashBlock   *valid_block;    // ���������؍ς݃u���b�N
    */
    // �A���[�i����m�ۂ����z��
    u8                 *master_hash;    // �u���b�N�̃n�b�V���z��
    /*
    u8                 *images;         // �Z�N�^�C���[�W
    u8                 *hashes;         // �u���b�N���̃n�b�V���z��
    CARDRomHashSector  *sectors;        // �Z�N�^���
    CARDRomHashBlock   *blocks;         // �u���b�N���
    */
    u8                 *buffer;
    u8                 *hash;

    /* �_�C�W�F�X�g���؂��ʂ邩�ǂ����̃t���O */
    u8 *master_hash_correct;
    u8 *hash_correct;

    /* ��₂���Ă��邩�ǂ����̃t���O */
    u8* master_hash_original;
    u8* hash_original;
}
CARDRomHashContext;


void CARDi_Init( CARDRomHashContext *context, RomHeader* header);
void CARDi_CheckHash(CARDRomHashContext *context, FILE* fp, u32 sect, u32 size, RomHeader* header);
bool Digest2Check(CARDRomHashContext *context, FILE* fp, RomHeader* header);
void CARD_DiffDigest(CARDRomHashContext *context, RomHeader* gHeader, FILE* gfp, RomHeader* mHeader, FILE* mfp);
void CARD_CheckHash(CARDRomHashContext *context, RomHeader* header, FILE* fp);
void CARD_CheckFileDigest(CARDRomHashContext *context, MyFileEntry* file_entry, u8* ret_digest1, u8* ret_digest2);
/* �A�h���X�͈̔͂ɊY������_�C�W�F�X�g���؂̍��ۂ�\������ */
void GetDigestResult( CARDRomHashContext *context, u32 start_adr, u32 end_adr, u8* d1, u8* d2);
/* �A�h���X�͈̔͂ɊY������_�C�W�F�X�g�e�[�u���̉�ₗL����\������ */
void IsDigestModified( CARDRomHashContext *context, u32 start_adr, u32 end_adr, u8* d1, u8* d2);


#endif //CARD_HASH_H_
