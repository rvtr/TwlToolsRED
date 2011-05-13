/*---------------------------------------------------------------------------*
  nitroeva3/card/rom_filesystem.h
 *---------------------------------------------------------------------------*/
#ifndef NITROEVA3_CARD_ROM_FILESYSTEM_H_
#define NITROEVA3_CARD_ROM_FILESYSTEM_H_

#include "types.h"

/*---------------------------------------------------------------------------*
  定数定義
 *---------------------------------------------------------------------------*/
#define FILE_NAME_LENGTH 128

// マスクROMのヘッダアドレスを獲得
#define GetRomHeaderAddr()  ((RomHeader *)HW_ROM_HEADER_BUF)

// 共有ワーク領域のアドレス獲得
#define GetSharedWorkAddr() ((SharedWork *)HW_RED_RESERVED)	// (HW_MAIN_MEM + 0x007ff800)  maybe change later

#define ROM_FILE_MAIN_PROCESSOR 0
#define ROM_FILE_SUB_PROCESSOR  1

#define ROM_FILE_CARD       0
#define ROM_FILE_CARTRIDGE  1



/*---------------------------------------------------------------------------*
  型定義
 *---------------------------------------------------------------------------*/
// 共有ワーク構造体
typedef struct {
	u32 nCardID;				// NORMALカードID
	u32 sCardID;				// SECUREカードID
	u16 cardHeaderCrc16;		// カードヘッダCRC16
	u16 cardSecureCrc16;		// カードSECURE領域CRC16
	s16 cardHeaderError;		// カードヘッダエラー
} SharedWork;



/*---------------------------------------------------------------------------*
  ファイル名テーブルとは、
  
  ファイル名からファイルIDを取得するためのテーブル。
 「ディレクトリ・テーブル」と「エントリ名テーブル」で構成される。
 *---------------------------------------------------------------------------*/
typedef struct
{
  char	entry_name[FILE_NAME_LENGTH];	// ファイル名 (終端 \0 は省く)

} ROM_FNT;



/*---------------------------------------------------------------------------*
  ディレクトリ・テーブルとは、
  
  ディレクトリ・テーブル構造体の配列として実装されている。
  各テーブルにはディレクトリIDと呼ばれる番号が割り振られ、格納順にカウント
  アップされる。ファイルIDと区別するため、ディレクトリIDは、0xF000～0xFFFF。
  （ファイルIDは、0x0000～0xEFFF）ディレクトリ数は4096、ファイル数は61440まで
  となる。ルート・ディレクトリは、ディレクトリIDが、0xF000、親ディレクトリIDには
  ディレクトリ・エントリ数が格納される。
  
  ※ ディレクトリ・テーブル構造体配列の要素数 ＝ ディレクトリ数
  ※ ディレクトリ・テーブル構造体配列の添え字 ＝ ディレクトリID － 0xF000
  
  以下、ディレクトリ・テーブル構造体の型定義
 *---------------------------------------------------------------------------*/
typedef struct
{
  u32	entry_start;	// エントリ名の検索位置（ファイル名テーブルの先頭位置からのオフセット）
  u16	entry_file_id;	// 先頭エントリのファイル ID
  u16	parent_id;		// 親ディレクトリの ID

} ROM_FNTDir;



/*---------------------------------------------------------------------------*
  エントリ名テーブルとは、
  
  下記の２種類（ROM_FNTStrFile, ROM_FNTStrDir）の可変長データの集合。
  エントリがファイル      --> ROM_FNTStrFile
  エントリがディレクトリ  --> ROM_FNTStrDir
  
  同一ディレクトリ内に含まれるエントリは連続した領域に配置される（ファイルIDも連続）。
  同一ディレクトリ内の最終エントリの次にはエントリ名の長さが0のファイルエントリが置かれる
  
  ##########################################################################
  (ファイル構成の例：例１)
  /Nitro.ROM
  /BQ.DAT
  /image/APPLE.JPG
  
  ディレクトリ・テーブル
  DIR-ID
  --------------------------------------------------------------------------
  0xF000 : entry_start = &(Nitro.ROM),  entry_file_id = 0,  parent_id = 2 (ディレクトリ数)
  0xF001 : entry_start = &(APPLE.JPG),  entry_file_id = 2,  parent_id = 0xF000
  --------------------------------------------------------------------------
  
  エントリ名・テーブル
  FILE-ID
  ---(ディレクトリ / )------------------------------------------------------
  0      : entry_type = 0,  entry_name_length = 9,  entry_name = "Nitro.ROM"
  1      : entry_type = 0,  entry_name_length = 6,  entry_name = "BQ.DAT"
         : entry_type = 1,  entry_name_length = 5,  entry_name = "image"     dir_id = 0xF001
  x      : entry_type = 0,  entry_name_length = 0
  ---(ディレクトリ /image/ )------------------------------------------------
  2      : entry_type = 0,  entry_name_length = 9,  entry_name = "APPLE.JPG"
  x      : entry_type = 0,  entry_name_length = 0
  ##########################################################################
 *---------------------------------------------------------------------------*/
typedef struct
{
  u8	entry_type       :1;			// ファイルエントリの場合は 0
  u8	entry_name_length:7;			// ファイル名の長さ (0-127)
  char	entry_name[FILE_NAME_LENGTH];	// ファイル名 (終端 \0 は省く)

} ROM_FNTStrFile;

typedef struct
{
  u8	entry_type       :1;			// ディレクトリエントリの場合は 1
  u8	entry_name_length:7;			// ディレクトリ名の長さ (0-127)
  char	entry_name[FILE_NAME_LENGTH];	// ディレクトリ名 (終端 \0 は省く)
  u8	dir_id_L;						// ディレクトリ ID Low  8bit
  u8	dir_id_H;						// ディレクトリ ID High 8bit
  
} ROM_FNTStrDir;



/*---------------------------------------------------------------------------*
  FAT : ファイル・アロケーション・テーブルとは、
  
  下記の構造体の配列として実装されている。
  配列の添え字がファイルIDと一致。
  
  romFatTable を ROM_FAT型 の配列と見なすと、
  romFat[5].top  は、ファイルID=5 のファイルの先頭ROMアドレス
  
  使用されていないファイルIDに対応するデータには、top = bottom = 0 が入る
 *---------------------------------------------------------------------------*/
typedef struct 
{
  void*		top;	// ファイルの先頭 ROM アドレス 
  void*		bottom;	// ファイルの最終 ROM アドレス

} ROM_FAT;



/*---------------------------------------------------------------------------*
  オーバレイ・ヘッダ・テーブルとは、
  
  オーバレイ・ファイルのロード情報を含んだテーブル。
  リンク処理時に nef ファイルやオーバレイ・モジュールと同時にバイナリで作成される
  下記の構造体データの配列として実装され、
  配列のサイズ ＝ オーバレイ・ファイル数
  配列の添え字 ＝ オーバレイID
  
  オーバレイIDはリンク処理時には仮の値が設定されているが、makeromにより実際の値となる
 *---------------------------------------------------------------------------*/
typedef struct
{
  u32		id;				// オーバーレイ ID 
  void*		ram_address;	// ロード先頭位置 
  u32		ram_size;		// ロードサイズ 
  u32		bss_size;		// bss 領域サイズ 
  void*		sinit_init;		// static initializer 先頭アドレス 
  void*		sinit_init_end;	// static initializer 最終アドレス
  u32		file_id;		// オーバーレイファイルID
  u32		rom_size;		// オーバーレイファイルサイズ
} ROM_OVT;



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



/*---------------------------------------------------------------------------*
  ROMヘッダ構造体
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
  大域変数宣言
 *---------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------*
  関数プロトタイプ宣言
 *---------------------------------------------------------------------------*/
/*
BOOL get_overlay_info(int card_or_cart, int main_or_sub, u32 id, ROM_OVT *ovt);
BOOL my_romfile_load_overlay_segment(int card_or_cart, int main_or_sub, u32 id);
void print_overlay_info(int main_or_sub, u32 id);
*/

#endif // NITROEVA3_CARD_ROM_FILESYSTEM_H_



