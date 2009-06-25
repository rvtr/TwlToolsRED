#pragma once

#define   METWL_WHETHER_PLATFORM_CHECK		// プラットフォームがTWL対応以外かどうかをチェックする
#define   METWL_WHETHER_SIGN_DECRYPT		// 署名を外してダイジェストをベリファイする
#define   METWL_WHETHER_MRC					// MRC機能を含めるか
#define   METWL_LIGHT_CHECK					// 提出条件を緩くするか

// 定数
#define   METWL_ROMSIZE_MIN				256			// 最小のデバイス容量 256Mbit
#define   METWL_ROMSIZE_MAX				4096		// 最大の..           4Gbit
#define   METWL_ROMSIZE_MIN_NAND		1
#define   METWL_ROMSIZE_MAX_NAND		256
#define   METWL_ALLSIZE_MAX_NAND		(32*1024*1024)	// システム上のNANDアプリの容量制限(単位Byte)
#define   METWL_ALLSIZE_MAX_NAND_LIC	(16*1024*1024)	// 運用上の(契約上の)NANDアプリの容量制限(単位Byte)
#define   METWL_MASK_REGION_JAPAN       0x00000001
#define   METWL_MASK_REGION_AMERICA     0x00000002
#define   METWL_MASK_REGION_EUROPE      0x00000004
#define   METWL_MASK_REGION_AUSTRALIA   0x00000008
#define   METWL_MASK_REGION_CHINA       0x00000010
#define   METWL_MASK_REGION_KOREA       0x00000020
#define   METWL_MASK_REGION_ALL         0xffffffff
#define   METWL_NUMOF_SHARED2FILES		6
#define   METWL_SEG3_CRC                0x0254
#define   METWL_TAD_TMP_FILENAME        "tmp.srl"		// TAD読み込みの際に split_tad で一時的に作成するファイル名
#define   METWL_SDK52_RELEASE			((5 << 24)|(2 << 16)|30000)		// SDアクセス権の判定が必要なSDKバージョン (5.2 RELEASE)

// 特別にGUI表示/提出確認書に記述する文字列
#define	  METWL_STRING_UNNECESSARY_RATING_J		("レーティング表示不要(全年齢)")
#define	  METWL_STRING_UNNECESSARY_RATING_E		("Rating Not Required (All ages)")
#define   METWL_STRING_UNDEFINED_RATING_J		("不可")
#define   METWL_STRING_UNDEFINED_RATING_E		("Undefined")
#define   METWL_STRING_UNDEFINED_REGION_J		("不明")		// 起こり得ない
#define   METWL_STRING_UNDEFINED_REGION_E		("Undefined")
#define   METWL_STRING_CHINA_RATING_FREE_J		("全年齢(中国版対応)")
#define   METWL_STRING_CHINA_RATING_FREE_E		("All ages (China version support)")
#define   METWL_STRING_CHINA_REGION_J			("中国のみ")
#define   METWL_STRING_CHINA_REGION_E			("China only")
#define   METWL_STRING_KOREA_REGION_J			("韓国のみ")
#define   METWL_STRING_KOREA_REGION_E			("Korea only")
