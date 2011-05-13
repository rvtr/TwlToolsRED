/*---------------------------------------------------------------------------*
  nitroeva3/card/rom_filesystem.h
 *---------------------------------------------------------------------------*/
#ifndef NITROEVA3_CARD_ROM_FILESYSTEM_H_
#define NITROEVA3_CARD_ROM_FILESYSTEM_H_

#include "types.h"

/*---------------------------------------------------------------------------*
  �萔��`
 *---------------------------------------------------------------------------*/
#define FILE_NAME_LENGTH 128

// �}�X�NROM�̃w�b�_�A�h���X���l��
#define GetRomHeaderAddr()  ((RomHeader *)HW_ROM_HEADER_BUF)

// ���L���[�N�̈�̃A�h���X�l��
#define GetSharedWorkAddr() ((SharedWork *)HW_RED_RESERVED)	// (HW_MAIN_MEM + 0x007ff800)  maybe change later

#define ROM_FILE_MAIN_PROCESSOR 0
#define ROM_FILE_SUB_PROCESSOR  1

#define ROM_FILE_CARD       0
#define ROM_FILE_CARTRIDGE  1



/*---------------------------------------------------------------------------*
  �^��`
 *---------------------------------------------------------------------------*/
// ���L���[�N�\����
typedef struct {
	u32 nCardID;				// NORMAL�J�[�hID
	u32 sCardID;				// SECURE�J�[�hID
	u16 cardHeaderCrc16;		// �J�[�h�w�b�_CRC16
	u16 cardSecureCrc16;		// �J�[�hSECURE�̈�CRC16
	s16 cardHeaderError;		// �J�[�h�w�b�_�G���[
} SharedWork;



/*---------------------------------------------------------------------------*
  �t�@�C�����e�[�u���Ƃ́A
  
  �t�@�C��������t�@�C��ID���擾���邽�߂̃e�[�u���B
 �u�f�B���N�g���E�e�[�u���v�Ɓu�G���g�����e�[�u���v�ō\�������B
 *---------------------------------------------------------------------------*/
typedef struct
{
  char	entry_name[FILE_NAME_LENGTH];	// �t�@�C���� (�I�[ \0 �͏Ȃ�)

} ROM_FNT;



/*---------------------------------------------------------------------------*
  �f�B���N�g���E�e�[�u���Ƃ́A
  
  �f�B���N�g���E�e�[�u���\���̂̔z��Ƃ��Ď�������Ă���B
  �e�e�[�u���ɂ̓f�B���N�g��ID�ƌĂ΂��ԍ�������U���A�i�[���ɃJ�E���g
  �A�b�v�����B�t�@�C��ID�Ƌ�ʂ��邽�߁A�f�B���N�g��ID�́A0xF000�`0xFFFF�B
  �i�t�@�C��ID�́A0x0000�`0xEFFF�j�f�B���N�g������4096�A�t�@�C������61440�܂�
  �ƂȂ�B���[�g�E�f�B���N�g���́A�f�B���N�g��ID���A0xF000�A�e�f�B���N�g��ID�ɂ�
  �f�B���N�g���E�G���g�������i�[�����B
  
  �� �f�B���N�g���E�e�[�u���\���̔z��̗v�f�� �� �f�B���N�g����
  �� �f�B���N�g���E�e�[�u���\���̔z��̓Y���� �� �f�B���N�g��ID �| 0xF000
  
  �ȉ��A�f�B���N�g���E�e�[�u���\���̂̌^��`
 *---------------------------------------------------------------------------*/
typedef struct
{
  u32	entry_start;	// �G���g�����̌����ʒu�i�t�@�C�����e�[�u���̐擪�ʒu����̃I�t�Z�b�g�j
  u16	entry_file_id;	// �擪�G���g���̃t�@�C�� ID
  u16	parent_id;		// �e�f�B���N�g���� ID

} ROM_FNTDir;



/*---------------------------------------------------------------------------*
  �G���g�����e�[�u���Ƃ́A
  
  ���L�̂Q��ށiROM_FNTStrFile, ROM_FNTStrDir�j�̉ϒ��f�[�^�̏W���B
  �G���g�����t�@�C��      --> ROM_FNTStrFile
  �G���g�����f�B���N�g��  --> ROM_FNTStrDir
  
  ����f�B���N�g�����Ɋ܂܂��G���g���͘A�������̈�ɔz�u�����i�t�@�C��ID���A���j�B
  ����f�B���N�g�����̍ŏI�G���g���̎��ɂ̓G���g�����̒�����0�̃t�@�C���G���g�����u�����
  
  ##########################################################################
  (�t�@�C���\���̗�F��P)
  /Nitro.ROM
  /BQ.DAT
  /image/APPLE.JPG
  
  �f�B���N�g���E�e�[�u��
  DIR-ID
  --------------------------------------------------------------------------
  0xF000 : entry_start = &(Nitro.ROM),  entry_file_id = 0,  parent_id = 2 (�f�B���N�g����)
  0xF001 : entry_start = &(APPLE.JPG),  entry_file_id = 2,  parent_id = 0xF000
  --------------------------------------------------------------------------
  
  �G���g�����E�e�[�u��
  FILE-ID
  ---(�f�B���N�g�� / )------------------------------------------------------
  0      : entry_type = 0,  entry_name_length = 9,  entry_name = "Nitro.ROM"
  1      : entry_type = 0,  entry_name_length = 6,  entry_name = "BQ.DAT"
         : entry_type = 1,  entry_name_length = 5,  entry_name = "image"     dir_id = 0xF001
  x      : entry_type = 0,  entry_name_length = 0
  ---(�f�B���N�g�� /image/ )------------------------------------------------
  2      : entry_type = 0,  entry_name_length = 9,  entry_name = "APPLE.JPG"
  x      : entry_type = 0,  entry_name_length = 0
  ##########################################################################
 *---------------------------------------------------------------------------*/
typedef struct
{
  u8	entry_type       :1;			// �t�@�C���G���g���̏ꍇ�� 0
  u8	entry_name_length:7;			// �t�@�C�����̒��� (0-127)
  char	entry_name[FILE_NAME_LENGTH];	// �t�@�C���� (�I�[ \0 �͏Ȃ�)

} ROM_FNTStrFile;

typedef struct
{
  u8	entry_type       :1;			// �f�B���N�g���G���g���̏ꍇ�� 1
  u8	entry_name_length:7;			// �f�B���N�g�����̒��� (0-127)
  char	entry_name[FILE_NAME_LENGTH];	// �f�B���N�g���� (�I�[ \0 �͏Ȃ�)
  u8	dir_id_L;						// �f�B���N�g�� ID Low  8bit
  u8	dir_id_H;						// �f�B���N�g�� ID High 8bit
  
} ROM_FNTStrDir;



/*---------------------------------------------------------------------------*
  FAT : �t�@�C���E�A���P�[�V�����E�e�[�u���Ƃ́A
  
  ���L�̍\���̂̔z��Ƃ��Ď�������Ă���B
  �z��̓Y�������t�@�C��ID�ƈ�v�B
  
  romFatTable �� ROM_FAT�^ �̔z��ƌ��Ȃ��ƁA
  romFat[5].top  �́A�t�@�C��ID=5 �̃t�@�C���̐擪ROM�A�h���X
  
  �g�p����Ă��Ȃ��t�@�C��ID�ɑΉ�����f�[�^�ɂ́Atop = bottom = 0 ������
 *---------------------------------------------------------------------------*/
typedef struct 
{
  void*		top;	// �t�@�C���̐擪 ROM �A�h���X 
  void*		bottom;	// �t�@�C���̍ŏI ROM �A�h���X

} ROM_FAT;



/*---------------------------------------------------------------------------*
  �I�[�o���C�E�w�b�_�E�e�[�u���Ƃ́A
  
  �I�[�o���C�E�t�@�C���̃��[�h�����܂񂾃e�[�u���B
  �����N�������� nef �t�@�C����I�[�o���C�E���W���[���Ɠ����Ƀo�C�i���ō쐬�����
  ���L�̍\���̃f�[�^�̔z��Ƃ��Ď�������A
  �z��̃T�C�Y �� �I�[�o���C�E�t�@�C����
  �z��̓Y���� �� �I�[�o���CID
  
  �I�[�o���CID�̓����N�������ɂ͉��̒l���ݒ肳��Ă��邪�Amakerom�ɂ����ۂ̒l�ƂȂ�
 *---------------------------------------------------------------------------*/
typedef struct
{
  u32		id;				// �I�[�o�[���C ID 
  void*		ram_address;	// ���[�h�擪�ʒu 
  u32		ram_size;		// ���[�h�T�C�Y 
  u32		bss_size;		// bss �̈�T�C�Y 
  void*		sinit_init;		// static initializer �擪�A�h���X 
  void*		sinit_init_end;	// static initializer �ŏI�A�h���X
  u32		file_id;		// �I�[�o�[���C�t�@�C��ID
  u32		rom_size;		// �I�[�o�[���C�t�@�C���T�C�Y
} ROM_OVT;



/*---------------------------------------------------------------------------*
  �풓���W���[���p�p�����^
 *---------------------------------------------------------------------------*/
typedef struct {

	u8*		rom_address;		// �]���� ROM �A�h���X
	u8*		entry_address;		// ���s�J�n�i�G���g���j�A�h���X
	u8*		ram_address;		// �]���� RAM �A�h���X
	s32 	rom_size;			// �]�� ROM �T�C�Y

} ROM_ResidentModuleParam;	// �풓�� ... resident

// ROM�w�b�_�E�u�[�g�p�����[�^�\���� (�I���W�i���̒�`)
typedef struct {
	u8 *romAddr;				// ROM�A�h���X
	u8 *entryAddr;				// �G���g���A�h���X
	u8 *ramAddr;				// RAM�A�h���X
	s32 romSize;				// ROM�T�C�Y
} BootUsrParam;



/*---------------------------------------------------------------------------*
  ROM�w�b�_�\����
 *---------------------------------------------------------------------------*/
typedef struct {

    //------------------------------------------------------------------------
    // 0x000 System Reserved
    //
    char		title_name[12];		// Soft title name
    u32			game_code;			// Game code
    
    u16			maker_code;			// Maker code
    u8			machine_code;		// Machine code
    u8			rom_type;			// Rom type
    u8			rom_size;			// Rom size
    
    u8			reserved_A[9];		// System Reserved A ( Set ALL 0 )
    
    u8			soft_version;		// Soft version
    u8			comp_arm9_boot_area:1;	// Compress arm9 boot area
    u8			comp_arm7_boot_area:1;	// Compress arm7 boot area
    u8			:0;
    
    
    //------------------------------------------------------------------------
    // 0x020 for Static modules (Section:B)
    //
    //	ARM9
    BootUsrParam	arm9;
    
    //	ARM7
    BootUsrParam	arm7;


    //------------------------------------------------------------------------
    // 0x040 for File Name Table[FNT] (Section:C)
    //
    ROM_FNT*	fnt_offset;		// ROM offset
    u32				fnt_size;		// Table size
    
    
    //------------------------------------------------------------------------
    // 0x048 for File Allocation Table[FAT] (Section:E)
    //
    ROM_FAT*	fat_offset;		// ROM offset
    u32				fat_size;		// Table size
    
    
    //------------------------------------------------------------------------
    // 0x050 for Overlay Tables[OVT] (Section:D)
    //
    //	ARM9
    ROM_OVT*	main_ovt_offset;	// ROM offset
    u32				main_ovt_size;		// Table size
    
    //	ARM7
    ROM_OVT*	sub_ovt_offset;		// ROM offset
    u32				sub_ovt_size;		// Table size
	
	
	//------------------------------------------------------------------------
    // 0x060 for ROM control parameter
    u32			game_cmd_param;			// Game command parameter
    u32			secure_cmd_param;		// Secure command parameter
    u32			banner_offset;		// Ctrl Reserved A (Set 0)
    u16			secure_area_crc16;		// Secure area CRC-16
    u16			secure_cmd_latency;		// Secure command latency ((param+2)*256 system cycles)
    u8			ctrl_reserved_B[16];	// Ctrl Reserved B (Set 0)
    
    
    //------------------------------------------------------------------------
    // 0x080 - 0x0C0 System Reserved
    u8			reserved_B[64];		// System Reserved B (Set 0)
    
    
    //------------------------------------------------------------------------
    // 0x0C0 for NINTENDO logo data
    u8			nintendo_logo[0x9c];	// NINTENDO logo data
    u16 		nintendo_logo_crc16;	//            CRC-16
    
    
    //------------------------------------------------------------------------
    // 0x15E ROM header CRC-16
    u16			header_crc16;		// ROM header CRC-16
    
    
    //------------------------------------------------------------------------
    // 0x0160 - 0x0180 System Reserved
    //
    u8	reserved_C[32];			// Debugger Reserved (Set ALL 0)
    
} RomHeader;



/*---------------------------------------------------------------------------*
  ���ϐ��錾
 *---------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------*
  �֐��v���g�^�C�v�錾
 *---------------------------------------------------------------------------*/
/*
BOOL get_overlay_info(int card_or_cart, int main_or_sub, u32 id, ROM_OVT *ovt);
BOOL my_romfile_load_overlay_segment(int card_or_cart, int main_or_sub, u32 id);
void print_overlay_info(int main_or_sub, u32 id);
*/

#endif // NITROEVA3_CARD_ROM_FILESYSTEM_H_



