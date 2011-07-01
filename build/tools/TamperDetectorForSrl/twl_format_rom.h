/*---------------------------------------------------------------------------*
  Project:  TwlSDK - tools - makerom.TWL
  File:     format_rom.h

  Copyright 2007-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date::            $
  $Rev$
  $Author$
 *---------------------------------------------------------------------------*/

#ifndef FORMAT_ROM_H_
#define FORMAT_ROM_H_

#include "twl_format_rom_certificate.h"

#define FILE_NAME_LENGTH 128


#define ROMHEADER_CORP_ID                   "NINTENDO    "
#define TITLE_NAME_MAX                      12
#define GAME_CODE_MAX                       4
#define MAKER_CODE_MAX                      2

#define DIGEST_SIZE_SHA1                    20
#define AES_BLOCK_SIZE                      16

#define SECURE_AREA_START                   0x00004000
#define SECURE_AREA_END                     0x00008000
#define SECURE_AREA_SIZE                    (SECURE_AREA_END - SECURE_AREA_START)

// for out_romheader.c size check
#define BOOTABLE_SIZE_ARM9                  0x0027c000      // 2.5M - 16K (0x02004000 to 0x02280000)
#define BOOTABLE_SIZE_ARM7                  0x00040000      // 256K       (0x02380000 to 0x023c0000) // NITRO-IPL のロードサイズ制限
#define BOOTABLE_SIZE_ARM9_LTD              0x00280000      // 2.5M       (0x02400000 to 0x02680000)
#define BOOTABLE_SIZE_ARM7_LTD              0x000fc000      // 1M - 16K   (0x02e80000 to 0x02f80000 or 0x02f00000 to 0x02ffc000)

#define SIZE_OF_SIGN                        136
#define NITROCODE_LE                        0x2106c0de
#define NITROCODE_BE                        0xdec00621
#define TWLCODE_LE                          0x6314c0de
#define TWLCODE_BE                          0xdec01463
#define NINTENDO_LOGO_DATA_LENGTH           0x9c

// use PlatformCode
#define PLATFORM_CODE_FLAG_NTR              0x00    // ※既存のNTR-ROMヘッダのplatform_code値が"0"であるため、苦肉の策で。
#define PLATFORM_CODE_FLAG_NOT_NTR          0x01
#define PLATFORM_CODE_FLAG_TWL              0x02
#define PLATFORM_CODE_FLAG_NTR_MASK         0x01

#define PLATFORM_CODE_NTR                   ( PLATFORM_CODE_FLAG_NTR )
#define PLATFORM_CODE_TWL_HYBLID            ( PLATFORM_CODE_FLAG_NTR     | PLATFORM_CODE_FLAG_TWL )
#define PLATFORM_CODE_TWL_LIMITED           ( PLATFORM_CODE_FLAG_NOT_NTR | PLATFORM_CODE_FLAG_TWL )

// use TitleID_Hi
#define TITLE_ID_HI_APP_TYPE_SHIFT          0
#define TITLE_ID_HI_NOT_LAUNCH_FLAG_SHIFT   1
#define TITLE_ID_HI_MEDIA_CODE_SHIFT        2
#define TITLE_ID_HI_DATA_ONLY_FLAG_SHIFT    3
#define TITLE_ID_HI_SECURE_FLAG_SHIFT       4
#define TITLE_ID_HI_PUBLISHER_CODE_SHIFT    16

#define TITLE_ID_HI_APP_TYPE_MASK           ( 0x0001 << TITLE_ID_HI_APP_TYPE_SHIFT )
#define TITLE_ID_HI_NOT_LAUNCH_FLAG_MASK    ( 0x0001 << TITLE_ID_HI_NOT_LAUNCH_FLAG_SHIFT )
#define TITLE_ID_HI_MEDIA_MASK              ( 0x0001 << TITLE_ID_HI_MEDIA_CODE_SHIFT )
#define TITLE_ID_HI_DATA_ONLY_FLAG_MASK     ( 0x0001 << TITLE_ID_HI_DATA_ONLY_FLAG_SHIFT )
#define TITLE_ID_HI_SECURE_FLAG_MASK        ( 0x0001 << TITLE_ID_HI_SECURE_FLAG_SHIFT )
#define TITLE_ID_HI_PUBLISHER_CODE_MASK     ( 0xffff << TITLE_ID_HI_PUBLISHER_CODE_SHIFT )

// use TitleID
#define TITLE_ID_HI_SHIFT_OFFSET            32
#define TITLE_ID_APP_TYPE_SHIFT             ( TITLE_ID_HI_SHIFT_OFFSET + TITLE_ID_HI_APP_TYPE_SHIFT )
#define TITLE_ID_NOT_LAUNCH_FLAG_SHIFT      ( TITLE_ID_HI_SHIFT_OFFSET + TITLE_ID_HI_NOT_LAUNCH_FLAG_SHIFT )
#define TITLE_ID_MEDIA_CODE_SHIFT           ( TITLE_ID_HI_SHIFT_OFFSET + TITLE_ID_HI_MEDIA_CODE_SHIFT )
#define TITLE_ID_DATA_ONLY_FLAG_SHIFT       ( TITLE_ID_HI_SHIFT_OFFSET + TITLE_ID_HI_DATA_ONLY_FLAG_SHIFT )
#define TITLE_ID_SECURE_FLAG_SHIFT          ( TITLE_ID_HI_SHIFT_OFFSET + TITLE_ID_HI_SECURE_FLAG_SHIFT )
#define TITLE_ID_PUBLISHER_CODE_SHIFT       ( TITLE_ID_HI_SHIFT_OFFSET + TITLE_ID_HI_PUBLISHER_CODE_SHIFT )

#define TITLE_ID_APP_TYPE_MASK              ( 0x0001ULL << TITLE_ID_APP_TYPE_SHIFT )
#define TITLE_ID_NOT_LAUNCH_FLAG_MASK       ( 0x0001ULL << TITLE_ID_NOT_LAUNCH_FLAG_SHIFT )
#define TITLE_ID_MEDIA_MASK                 ( 0x0001ULL << TITLE_ID_MEDIA_CODE_SHIFT )
#define TITLE_ID_DATA_ONLY_FLAG_MASK        ( 0x0001ULL << TITLE_ID_DATA_ONLY_FLAG_SHIFT )
#define TITLE_ID_SECURE_FLAG_MASK           ( 0x0001ULL << TITLE_ID_SECURE_FLAG_SHIFT )
#define TITLE_ID_PUBLISHER_CODE_MASK        ( 0xffffULL << TITLE_ID_PUBLISHER_CODE_SHIFT )

#define PARENTAL_CONTROL_INFO_SIZE		0x10

/*---------------------------------------------------------------------------*
  常駐モジュール用パラメタ
 *---------------------------------------------------------------------------*/
typedef struct {

	u8*		rom_address;		// 転送元 ROM アドレス
	u8*		entry_address;		// 実行開始（エントリ）アドレス
	u8*		ram_address;		// 転送先 RAM アドレス
	s32 	rom_size;			// 転送 ROM サイズ

} ROM_ResidentModuleParam;	// 常駐の ... resident

// ROMヘッダ・ブートパラメータ構造体 (オリジナルの定義)
typedef struct {
	u8 *romAddr;				// ROMアドレス
	u8 *entryAddr;				// エントリアドレス
	u8 *ramAddr;				// RAMアドレス
	s32 romSize;				// ROMサイズ
} BootUsrParam;

/*===========================================================================*
 *  ROM FORMAT
 *===========================================================================*/

// ROM access control info
typedef struct RomAccessControl {
    u32     common_client_key :1;       // launcher deliver common client Key
    u32     hw_aes_slot_B :1;           // launcher deliver HW AES slot B setting for ES
    u32     hw_aes_slot_C :1;           // launcher deliver HW AES slot C setting for NAM
    u32     sd_card_access :1;          // sd card access control
    u32     nand_access :1;             // NAND access control
    u32     game_card_on :1;            // NANDアプリでゲームカード電源ON（ノーマルモード）
	u32     shared2_file :1;            // shared file in "nand:/shared2"
	u32     hw_aes_slot_B_SignJPEGForLauncher :1; // launcher deliver HW AES slot B setting for Sign JPEG for Launcher
    u32     game_card_nitro_mode :1;    // NANDアプリでゲームカードNTR互換領域へアクセス
	u32     hw_aes_slot_A_SSLClientCert :1; // launcher deliver HW AES slot A setting for SSL Client Certificate
	u32     hw_aes_slot_B_SignJPEGForUser :1; // launcher deliver HW AES slot B setting for Sign JPEG for User
    u32     photo_access_read :1;       // "photo:" archive read-access control
    u32     photo_access_write :1;      // "photo:" archive write-access control
    u32     sdmc_access_read :1;        // "sdmc:" archive read-access control
    u32     sdmc_access_write :1;       // "sdmc:" archive write-access control
    u32     backup_access_read :1;      // CARD-backup read-access control
    u32     backup_access_write :1;     // CARD-backup write-access control
    u32:    14;
    u32     common_client_key_for_debugger_sysmenu :1;  // launcher deliver common client Key
}RomAccessControl;

//  ROM expansion flags
typedef struct RomExpansionFlags {
    u8      codec_mode:1;                   // 0:NTR mode, 1:TWL mode       // undeveloped
    u8      agree_EULA:1;                   // 1: necessary agree EULA
    u8      availableSubBannerFile:1;       // 1: Available SubBannerFile
    u8      WiFiConnectionIcon :1;          // 1: WiFiConnectionをランチャーで表示
    u8      DSWirelessIcon :1;              // 1: DSWirelessIconをランチャーで表示
    u8      rsv_d5:1;
    u8      enable_nitro_whitelist_signature :1; // 1: NITROホワイトリスト署名有効フラグ
    u8      developer_encrypt:1;            // 1: 開発用セキュリティがかかっている場合に"1"。製品版では"0" (※TwlSDK UIG_branch/RC2以降はこちらが有効）
}RomExpansionFlags;

//---------------------------------------------------------------------------
//  Section A   ROM HEADER
//---------------------------------------------------------------------------

typedef struct ROM_Header_Short
{
    //==========================================================
    //
    // NTR/TWL common
    //
    //==========================================================

    //
    // 0x000 System Reserved
    //
    char    title_name[TITLE_NAME_MAX]; // Soft title name
    char    game_code[GAME_CODE_MAX];   // Game code
    char    maker_code[MAKER_CODE_MAX]; // Maker code
    char    platform_code;              // Platform code    bit0: not support NTR,  bit1: support TWL ( NTR_only=0x00, NTR/TWL=0x03, TWL_only=0x02 )
    u8      rom_type;                  // Rom type
    u8      rom_size;                  // Rom size (2のrom_size乗 Mbit: ex. 128Mbitのときrom_size = 7)

    u8      reserved_A[7];             // System Reserved A ( Set ALL 0 )

    u8      enable_signature:1;        // enable ROM Header signature
    u8      enable_aes:1;              // enable AES encryption
    u8      developer_encrypt_old:1;   // 開発用セキュリティがかかっている場合に"1"。製品版では"0" (※TwlSDK RC plusまではこちらが有効）
    u8      disable_debug:1;           // デバッグ禁止フラグ
    u8:     4;

    u8      permit_landing_normal_jump:1; // アプリジャンプのノーマルジャンプで呼び出されることを許可する( for TWL Application Jump )
    u8      permit_landing_tmp_jump:1;    // アプリジャンプのTMPジャンプで呼び出されることを許可する( for TWL Application Jump )
                                          //  ※NTR体験版アプリはDSダウンロードプレイの署名しかついていないので、このフラグはNTR-ROMヘッダの0x160bytes内の領域に格納する必要がある。
    u8:     4;
    u8      for_korea:1;               // For Korea
    u8      for_china:1;               // For China

    u8      rom_version;               // Rom version

    u8      comp_arm9_boot_area:1;     // Compress arm9 boot area
    u8      comp_arm7_boot_area:1;     // Compress arm7 boot area
    u8      inspect_card:1;            // Show inspect card
    u8      disable_clear_memory_pad:1;  // for Debugger
    u8      enable_twl_rom_cache_read:1; // Enable TWL ROM cacheRead command
    u8      :1;                          // reserved.
    u8      warning_no_spec_rom_speed:1;// Warning not to specify rom speed
    u8      disable_detect_pull_out:1;  //

    //
    // 0x020 for Static modules (Section:B)
    //
    //  ARM9
    union {
        BootUsrParam arm9;
        struct {
            u32     main_rom_offset;           // ROM offset
            void   *main_entry_address;        // Entry point
            void   *main_ram_address;          // RAM address
            u32     main_size;                 // Module size
        };
    };
    
    //  ARM7
    union {
        BootUsrParam arm7;
        struct {
            u32     sub_rom_offset;            // ROM offset
            void   *sub_entry_address;         // Entry point
            void   *sub_ram_address;           // RAM address
            u32     sub_size;                  // Module size
        };
    };

    //
    // 0x040 for File Name Table[FNT] (Section:C)
    //
    struct ROM_FNT *fnt_offset;        // ROM offset
    u32     fnt_size;                  // Table size

    //
    // 0x048 for File Allocation Table[FAT] (Section:E)
    //
    struct ROM_FAT *fat_offset;        // ROM offset
    u32     fat_size;                  // Table size

    //
    // 0x050 for Overlay Tables[OVT] (Section:D)
    //
    //  ARM9
    struct ROM_OVT *main_ovt_offset;   // ROM offset
    u32     main_ovt_size;             // Table size

    //  ARM7
    struct ROM_OVT *sub_ovt_offset;    // ROM offset
    u32     sub_ovt_size;              // Table size

    // 0x060 for ROM control parameter (Section:F)
    u32     game_cmd_param;            // Game command parameter
    u32     secure_cmd_param;          // Secure command parameter

    u32     banner_offset;             // Banner ROM offset

    u16     secure_area_crc16;         // Secure area CRC-16
    u16     secure_cmd_latency;        // Secure command latency ((param+2)*256 system cycles)

    //                  since NITRO-SDK 2.0PR4
    void   *main_autoload_done;        // ARM9 autoload done callback address (debug purpose)
    void   *sub_autoload_done;         // ARM7 autoload done callback address (debug purpose)

    u8      ctrl_reserved_B[8];        // Ctrl Reserved B (Set 0)

    //                  since NITRO-SDK 2.0PR6
    u32     rom_valid_size;            // ROM Original Size
    u32     rom_header_size;           // ROM Header size
    u32     main_module_param_offset;  // Offset for table of ARM9 module parameters
    u32     sub_module_param_offset;   // Offset for table of ARM7 module parameters

    // 0x090 - 0x0C0 System Reserved
    u16     twl_card_normal_area_rom_offset;        // undeveloped
    u16     twl_card_keytable_area_rom_offset;      // undeveloped
    u16     nand_card_dl_area_rom_offset;           // undeveloped
    u16     nand_card_bk_area_rom_offset;           // undeveloped
    u8      nand_card_flag;                         // undeveloped
    u8      reserved_B[39];

    // 0x0C0 for NINTENDO logo data
    u8      nintendo_logo[ NINTENDO_LOGO_DATA_LENGTH ];  // NINTENDO logo data
    u16     nintendo_logo_crc16;       //            CRC-16

    // 0x15E ROM header CRC-16
    u16     header_crc16;              // ROM header CRC-16

    // 0x160 - 0x180 Debugger Reserved
    u8      reserved_C[32];            // Debugger Reserved (Set ALL 0)

    //==========================================================
    //
    // TWL only
    //
    //==========================================================

    // 0x180 - 0x190 TWL-WRAM A/B/C ARM9 configuration data
    u32     main_wram_config_data[8];       // developing...

    // 0x1A0 - 0x1B0 TWL-WRAM A/B/C ARM7 configuration data
    u32     sub_wram_config_data[4];        // developing...

    // 0x1B0 - Card Region bitmap
    u32     card_region_bitmap;

    // 0x1B4 - AccessControl
	RomAccessControl	access_control;
    /* 注意： fatfs_command.c 内で 0x01b4 のアドレスを
              ハードコーディングしています。 このメンバのオフセットを変更しないで下さい。 */

    // 0x1B8 - ARM7-SCFG
    u32     arm7_scfg_ext;                  // SCFG-EXT

    // padding(3byte)
    u8      reserved_ltd_A2[ 3 ];

    // 0x1BF - TWL expansion flags
	RomExpansionFlags	exFlags;

    // 0x1C0 for EX Static modules
    //
    //  ARM9
    union {
        BootUsrParam ltd_arm9;
        struct {
            u32     main_ltd_rom_offset;           // ROM offset            // undeveloped
            u8      reserved_ltd_B[ 4 ];
            void   *main_ltd_ram_address;          // RAM address       // undeveloped  //
            u32     main_ltd_size;                 // Module size       // undeveloped  //
        };
    };
    
    //  ARM7
    union {
        BootUsrParam ltd_arm7;
        struct {
            u32     sub_ltd_rom_offset;            // ROM offset            // undeveloped
            void   *sub_mount_info_ram_address;    // ARM7 MountInfo RAM address.
            void   *sub_ltd_ram_address;           // RAM address       // undeveloped  //
            u32     sub_ltd_size;                  // Module size       // undeveloped  //
            /* 注意： os_reset.c / crt0.HYB.c / crt0.LTD.c 内で 0x01c0 ～ 0x01e0 のアドレスを
              ハードコーディングしています。 これら８つのメンバのオフセットを変更しないで下さい。 */
        };
    };

    // 0x01E0 - 0x01E8 for NITRO digest area offset & size
    u32     nitro_digest_area_rom_offset;
    u32     nitro_digest_area_size;

    // 0x01E8 - 0x01F0 for TWL   digest area offset & size
    u32     twl_digest_area_rom_offset;
    u32     twl_digest_area_size;

    // 0x01F0 - 0x01F8 for FS digest table1 offset & size
    u32     digest1_table_offset;
    u32     digest1_table_size;

    // 0x01F8 - 0x0200 for FS digest table2 offset & size
    u32     digest2_table_offset;
    u32     digest2_table_size;

    // 0x0200 - 0x0208 for FS digest config parameters
    u32     digest1_block_size;
    u32     digest2_covered_digest1_num;

    // 0x0208 - 0x020C for TWL banner size.
    u32     banner_size;

	// 0x020C - 0x020E for shared2 files size
	u8		shared2_file0_size;		// shared2 file [0]
	u8		shared2_file1_size;		// shared2 file [1]
    /* 注意： fatfs_command.c 内で 0x020C-0x020D のアドレスを
              ハードコーディングしています。 このメンバのオフセットを変更しないで下さい。 */

    // 0x020E          for Agree EULA version
	u8      agree_EULA_version;

    // 0x020F          TWL Administration flags
    u8      unnecessary_rating_display:1;   // レーティング表記がいらないROMのときに立てる(管理用:ランチャーでは不使用)
    u8:     7;

    // 0x0210 - 0x0214 for TWL rom valid size
    u32     twl_rom_valid_size;            // ROM Original Size
	
	// 0x0214 - 0x0218 for shared2 files size
	u8		shared2_file2_size;		// shared2 file [2]
	u8		shared2_file3_size;		// shared2 file [3]
	u8		shared2_file4_size;		// shared2 file [4]
	u8		shared2_file5_size;		// shared2 file [5]
    /* 注意： fatfs_command.c 内で 0x0214 - 0x0217 のアドレスを
              ハードコーディングしています。 このメンバのオフセットを変更しないで下さい。 */

    // 0x0218 - 0x0220 for TWL ltd module param offset
    u32     main_ltd_module_param_offset;  // Offset for table of ARM9 ltd module parameters
    u32     sub_ltd_module_param_offset;   // Offset for table of ARM7 ltd module parameters

    // 0x0220 - 0x0230 for AES target offset & size
    u32     aes_target_rom_offset;
    u32     aes_target_size;
    u32     aes_target2_rom_offset;         // 予約
    u32     aes_target2_size;               // 予約

    // 0x0230 - 0x0238 for TitleID
    union {
        u64 titleID;
        struct {
            u8      titleID_Lo[ 4 ];
            u32     titleID_Hi;
        };
    };
    /* 注意： os_reset.c / crt0.HYB.c / crt0.LTD.c / fatfs_command.c 内で 0x0234 をハードコーディングしています。
              titleID_Hi のオフセットを変更しないで下さい。 */

    // 0x0238 - 0x0240 for Public & Private Save Data Size
    u32     public_save_data_size;
    u32     private_save_data_size;

    // 0x0240 - 0x02f0 reserved.
    u8      reserved_ltd_F[ 0x2f0 - 0x240 ];

    // 0x02f0 - 0x0300 Parental Controls Rating Info
    u8      parental_control_rating_info[ PARENTAL_CONTROL_INFO_SIZE ];

    // 0x0300 - 0x03b4 Rom Segment Digest
    u8      main_static_digest[ DIGEST_SIZE_SHA1 ];
    u8      sub_static_digest[ DIGEST_SIZE_SHA1 ];
    u8      digest2_table_digest[ DIGEST_SIZE_SHA1 ];
    u8      banner_digest[ DIGEST_SIZE_SHA1 ];
    u8      main_ltd_static_digest[ DIGEST_SIZE_SHA1 ];
    u8      sub_ltd_static_digest[ DIGEST_SIZE_SHA1 ];
    u8      nitro_whitelist_phase1_digest[ DIGEST_SIZE_SHA1 ];
    u8      nitro_whitelist_phase2_diegst[ DIGEST_SIZE_SHA1 ];
    u8      main_static_without_secure_digest[ DIGEST_SIZE_SHA1 ];
}
ROM_Header_Short;
typedef ROM_Header_Short RomHeader;

typedef struct
{
    struct ROM_Header_Short s;

    ////
    ////  EXTRA SYSTEM REGION
    ////

    //
    // 0x0378 - 0x0e00 System Reserved
    //
    u8      reserved_D[4 * 1024 - sizeof(ROM_Header_Short) - sizeof(RomCertificate) - RSA_KEY_LENGTH ];       // Reserved D

    //
    // 0x0e00 - 0x0f80 ROM Certificate
    //
    RomCertificate certificate;

    //
    // 0x0f80 - 0x1000 ROM Header Signature
    //
    u8      signature[ RSA_KEY_LENGTH ];        // PKCS#1_v1.5の形式でフォーマットされたSignatureDataが、RSA暗号化された状態

    //
    // 0x01000 - 0x04000 System Reserved
    //
    u8      reserved_E[12 * 1024];     // Reserved E

}
ROM_Header;                            // 16KB



// 署名(ROM_Header.signature)内のデータ
typedef struct SignatureData
{
    u8  digest[ DIGEST_SIZE_SHA1 ];
}SignatureData;



//---------------------------------------------------------------------------
//  Section C   FNT: FILE NAME TABLE
//                      - DIRECTORY TABLE
//                      - ENTRY TABLE
//---------------------------------------------------------------------------

typedef struct ROM_FNTDir
{
    u32     entry_start;               // offset for entry
    u16     entry_file_id;             // file id for 1st file
    u16     parent_id;                 // parent directory id

}
ROM_FNTDir;


/*  P-SEUDO CODE for YOUR INFO.
typedef struct ROM_FNTStrFile
{
    u8  entry_type       :1;    // 0 if file entry
    u8  entry_name_length:7;    // length of filename (0-127)
    char    entry_name[length]; // file name (surpress '\0')

} ROM_FNTStrFile;


typedef struct ROM_FNTStrDir
{
    u8  entry_type       :1;    // 1 if directory entry
    u8  entry_name_length:7;    // length of directory name (0-127)
    char    entry_name[length]; // directory name (surpress '\0')
    u8  dir_id_L;       // low  byte of directory id
    u8  dir_id_H;       // high byte of directory id
} ROM_FNTStrDir;
*/

//---------------------------------------------------------------------------
//  Section D   OVT: OVERLAY TABLE
//---------------------------------------------------------------------------

typedef struct ROM_OVT
{
    void   *id;                        // Overlay ID
    void   *ram_address;               // ram address
    u32     ram_size;                  // ram size
    u32     bss_size;                  // bss size
    void   *sinit_init;                // static initializer start
    void   *sinit_init_end;            // static initializer end
    u32     file_id;                   // file id in FAT
    u32     compressed:24;             // オーバーレイ圧縮後のサイズ
    u32     flag:8;                    // オーバーレイ情報フラグ

}
ROM_OVT;


//---------------------------------------------------------------------------
//  Section E   FAT: FILE ALLOCATION TABLE
//---------------------------------------------------------------------------
/*
typedef struct ROM_FilePtr
{
    u32     offset;                    // offset

}
ROM_FilePtr;
*/

typedef struct ROM_FAT
{
    void* top;                   // file region start
    void* bottom;                //             end
}
ROM_FAT;


//---------------------------------------------------------------------------
//  Section F   LT: ROM LATENCY
//---------------------------------------------------------------------------

typedef enum
{
    ONETROM = 0,
    MROM
}
tRomSpeedType;

typedef struct ROM_LT
{
    u32     game_latency;
    u32     secure_hw_latency;
    u16     secure_sw_latency;
    u16     padding;

}
ROM_LT;


//---------------------------------------------------------------------------
//  Section X   ONT: OVERLAY NAME TABLE  (not include in ROM) generated by LD
//                      - OverlayDefs
//---------------------------------------------------------------------------

typedef struct ROM_ONTHeader
{
    void   *static_ram_address;        // static module ram_address
    void   *static_entry_address;      //               entry address
    u32     static_size;               //               size
    void   *static_autoload_done;      // autoload done callback address

}
ROM_ONTHeader;

typedef struct ROM_ONT
{
    ROM_ONTHeader header;
    char    file_list[1];
    char    padding[3];

}
ROM_ONT;

//---------------------------------------------------------------------------
//  Section Y   MDP: Module params
//---------------------------------------------------------------------------
typedef struct tModuleParam
{
    void   *autoload_list;
    void   *autoload_list_end;
    void   *autoload_start;
    void   *static_bss_start;
    void   *static_bss_end;
    void   *compressed_static_end;     // tCompFooter の 1要素分先を指す
    u32     version;
    u32     nitro_magic_be;
    u32     nitro_magic_le;

}
tModuleParam;

typedef struct tLtdModuleParam
{
    void   *autoload_list;
    void   *autoload_list_end;
    void   *autoload_start;
    void   *compressed_static_end;     // tCompFooter の 1要素分先を指す
    u32     twl_magic_be;
    u32     twl_magic_le;
}
tLtdModuleParam;


typedef struct
{
    u32     bufferTop:24;
    u32     compressBottom:8;
    u32     originalBottom;
}
tCompFooter;

typedef struct      // compstatic/component.hより抜粋
{
    u32     magicNumber;               // 認識子 ==MAGICNUMBER_FOOTER
    int     staticParamsOffset;        // StaticParams     へのオフセット
    int     digestParamsOffset;        // SDK_DIGEST_TABLE へのオフセット
    int     ltdStaticParamsOffset;     // LtdStaticParams  へのオフセット
}
StaticFooter;

#endif //FORMAT_ROM_H_
