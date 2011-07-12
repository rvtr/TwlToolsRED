
#ifndef CARD_HASH_H_
#define CARD_HASH_H_

#include "entry.h"


// 現時点でCARDライブラリが妥当と判断した定数。
// パフォーマンス計測の結果によって適宜変更してもよい。
static const u32    CARD_ROM_HASH_BLOCK_MAX = 4;
static const u32    CARD_ROM_HASH_SECTOR_MAX = 32;


// ROM領域情報構造体
typedef struct CARDRomRegion
{
    u32     offset;
    u32     length;
}
CARDRomRegion;



// SRLファイルのハッシュ管理構造体。
// 所要サイズはプログラムごとに異なり静的に算出できないため
// 初期化時にアリーナから適量だけ動的に確保する予定。
// 必要となるメモリは以下の通り。
//   - マスターハッシュテーブル:
//       ROMヘッダとハッシュ比較して正当性を判定する必要上、
//       初期化時にROMから一括ロードして常駐させておく。
//       (ROMサイズ/セクタサイズ/セクタ単位)*20(SHA1)バイト必要で、
//       1Gbits:1024バイト:32セクタならば約80kBとなる。
//   - ブロックキャッシュ:
//       ブロック単位でROMイメージキャッシュとそのハッシュを管理する。
//       ブロック境界をまたいだ離散的なアクセスを考慮して
//       常に複数を保持できるリスト構造にしておく必要がある。
//       (20 * セクタ単位 + α) の構造体をリスト総数の分だけ必要。
//   - セクタキャッシュ:
//       実際のイメージキャッシュを保持する。
//       ブロックが全セクタをあまねく参照するとは限らないため
//       セクタも別途リスト構造として管理する必要がある。
typedef struct CARDRomHashContext
{
    // ROMヘッダから取得する基本設定
    CARDRomRegion       area_ntr;
    CARDRomRegion       area_ltd;
    CARDRomRegion       sector_hash;
    CARDRomRegion       block_hash;
    u32                 bytes_per_sector;
    u32                 sectors_per_block;
    u32                 block_max;
    u32                 sector_max;

    // データとハッシュをロードするデバイスインタフェース
//    void                   *userdata;
//    MIDeviceReadFunction    ReadSync;
//    MIDeviceReadFunction    ReadAsync;

    // ロード処理中のスレッド。
//    OSThread           *loader;
//    void               *recent_load;
/*
    // セクタとブロックのキャッシュ
    CARDRomHashSector  *loading_sector; // メディアロード待ちセクタ
    CARDRomHashSector  *loaded_sector;  // ハッシュ検証待ちセクタ
    CARDRomHashSector  *valid_sector;   // 正当性検証済みセクタ
    CARDRomHashBlock   *loading_block;  // メディアロード待ちブロック
    CARDRomHashBlock   *loaded_block;   // ハッシュ検証待ちブロック
    CARDRomHashBlock   *valid_block;    // 正当性検証済みブロック
    */
    // アリーナから確保した配列
    u8                 *master_hash;    // ブロックのハッシュ配列
    /*
    u8                 *images;         // セクタイメージ
    u8                 *hashes;         // ブロック内のハッシュ配列
    CARDRomHashSector  *sectors;        // セクタ情報
    CARDRomHashBlock   *blocks;         // ブロック情報
    */
    u8                 *buffer;
    u8                 *hash;

    /* ダイジェスト検証が通るかどうかのフラグ */
    u8 *master_hash_correct;
    u8 *hash_correct;

    /* 改竄されているかどうかのフラグ */
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
/* アドレスの範囲に該当するダイジェスト検証の合否を表示する */
void GetDigestResult( CARDRomHashContext *context, u32 start_adr, u32 end_adr, u8* d1, u8* d2);
/* アドレスの範囲に該当するダイジェストテーブルの改竄有無を表示する */
void IsDigestModified( CARDRomHashContext *context, u32 start_adr, u32 end_adr, u8* d1, u8* d2);


#endif //CARD_HASH_H_
