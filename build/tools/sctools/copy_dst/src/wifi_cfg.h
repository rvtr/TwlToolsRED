#ifndef	_WIFI_CFG_H_
#define	_WIFI_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_CFG_WEP_NOTATION_HEX   0  // 16進数表記
#define WIFI_CFG_WEP_NOTATION_ASCII 1  // ASCII コード表記

#define WIFI_CFG_PROXY_MODE_NONE   0  /* プロキシを使用しない */
#define WIFI_CFG_PROXY_MODE_NORMAL 1  /* 通常のプロキシ */

#define WIFI_CFG_PROXY_AUTHTYPE_NONE  0  /* 認証なし */
#define WIFI_CFG_PROXY_AUTHTYPE_BASIC 1  /* BASIC 認証 */



BOOL wifi_config_nvram_get( void );
BOOL wifi_config_nvram_set( void );
BOOL wifi_config_clear_slot(int slot_no);
void wifi_config_print(void);
BOOL wifi_config_set_ip_addr(int slot_no, u32 ipaddr);
BOOL wifi_config_set_ssid(int slot_no, char *ssid_str, int ssid_len);
BOOL wifi_config_set_dns(int slot_no, int dns_no, u32 dnsaddr);
BOOL wifi_config_set_gateway(int slot_no, u32 ipaddr);
BOOL wifi_config_set_netmask(int slot_no, u32 mask);
BOOL wifi_config_set_wep_key_str(int slot_no, int wepmode, char *wepkey );
BOOL wifi_config_set_wep_key_bin(int slot_no, int wepmode, u8 *wepkey, int wepkey_len );
BOOL wifi_config_set_wpa_key_str(int slot_no, int wpamode, char *wpakey );
BOOL wifi_config_set_wpa_key_bin(int slot_no, int wpamode, u8 *wpakey , int wpakey_len);

#ifdef __cplusplus
}
#endif


#if 0

#define NCFG_SETTYPE_WPS NCFG_SETTYPE_WPS_WPA


// WEP 暗号化モード種別
// WCM_WEPMODE_* よりコピー
typedef enum
{
    NCFG_WEPMODE_NONE = 0,              // 暗号化なし
    NCFG_WEPMODE_40   = 1,              // RC4 (  40 ビット ) 暗号化モード
    NCFG_WEPMODE_104  = 2,              // RC4 ( 104 ビット ) 暗号化モード
    NCFG_WEPMODE_128  = 3               // RC4 ( 128 ビット ) 暗号化モード
} NCFGWEPMode;

// WEP 表記
// util/setting.h よりコピー
typedef enum
{
    NCFG_WEP_NOTATION_HEX,              // 16進数表記
    NCFG_WEP_NOTATION_ASCII             // ASCII コード表記
} NCFGWEPNotation;

// Proxy モード
typedef enum
{
    NCFG_PROXY_MODE_NONE               =    0x00,  /* プロキシを使用しない */
    NCFG_PROXY_MODE_NORMAL             =    0x01   /* 通常のプロキシ */
} NCFGProxyMode;

// Proxy 認証形式
typedef enum
{
    NCFG_PROXY_AUTHTYPE_NONE           =    0x00,  /* 認証なし */
    NCFG_PROXY_AUTHTYPE_BASIC          =    0x01   /* BASIC 認証 */
} NCFGProxyAuthType;

// WPA モード種別
typedef enum
{
    NCFG_WPAMODE_WPA_PSK_TKIP   =   0x04,     /* WPA-PSK  ( TKIP ) 暗号 */
    NCFG_WPAMODE_WPA2_PSK_TKIP  =   0x05,     /* WPA2-PSK ( TKIP ) 暗号 */
    NCFG_WPAMODE_WPA_PSK_AES    =   0x06,     /* WPA-PSK  ( AES  ) 暗号 */
    NCFG_WPAMODE_WPA2_PSK_AES   =   0x07      /* WPA2-PSK ( AES  ) 暗号 */
} NCFGWPAMode;

// 各種フラグ
typedef enum
{
    NCFG_FLAG_NONE              =   0x00,
    NCFG_FLAG_ALWAYS_ACTIVE     =   0x01,     /* PowerSave を無効にする (未使用) */
    NCFG_FLAG_SAFE_MTU          =   0x02      /* MTU を 576 バイトにする (未使用/削除予定) */
} NCFGFlag;    OS_TPrintf("IP addr:");


// ワークメモリサイズ
#define NCFG_CHECKCONFIG_WORK_SIZE      0x700
#define NCFG_READCONFIG_WORK_SIZE       0x700
#define NCFG_WRITECONFIG_WORK_SIZE      0x700

#define NCFG_CHECKCONFIGEX_WORK_SIZE    0x1000
#define NCFG_READCONFIGEX_WORK_SIZE     0x1000
#define NCFG_WRITECONFIGEX_WORK_SIZE    0x1000


/*---------------------------------------------------------------------------*
    マクロ 定義
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    構造体 定義
 *---------------------------------------------------------------------------*/

// アクセスポイント接続情報
typedef struct NCFGApInfo
{
    u8  ispId    [0x20];       // ISP 用 ID
    u8  ispPass  [0x20];       // ISP 用パスワード
    u8  ssid  [2][0x20];       // SSID (ssid[1] : AOSS WEP40 用)
    u8  wep   [4][0x10];       // WEP キー
    u8  ip       [0x04];       // IP アドレス
    u8  gateway  [0x04];       // デフォルトゲートウェイ
    u8  dns   [2][0x04];       // DNS
    u8  netmask;               // サブネットマスク
    u8  wep2  [4][0x05];       // WEP キー (AOSS 用)
    u8  authType;              // 認証方法
    u8  wepMode      : 2;      // WEP モード
    u8  wepNotation  : 6;      // WEP 表記
    u8  setType;               // 設定方法
    u8  ssidLength[2];         // SSID 長 (0 ならば ssid に NULL が来るまで)
    u16 mtu;                   // MTU 長 (0 ならばデフォルト値で解釈)
    u8  rsv      [0x02];       // 予備
    u8  flags;                 // フラグ (NCFG_FLAG_*)
    u8  state;                 // 使用状況
}  NCFGApInfo;

// 拡張アクセスポイント接続情報
typedef struct NCFGApInfoEx
{
    union {
        struct {
            u8  wpaKey[0x20];      // WPA PMK
            u8  wpaPassphrase[0x40];   // WPA パスフレーズ (NULL 終端文字列 or 64桁 HEX)
            u8  rsv[0x21];
            u8  wpaMode;           // WPA モード
        } wpa;
        struct {
            u8  wpaKey[2][0x20];   // WPA PMK (AOSS 用 [0]:TKIP, [1]:AES)
            u8  ssid[2][0x20];     // SSID (AOSS 用 [0]:TKIP, [1]:AES)
            u8  ssidLength[2];     // SSID 長 (AOSS 用 [0]:TKIP, [1]:AES)
        } aossEx;
    };
    u8  proxyMode;             // Proxy mode
    u8  proxyAuthType;         // Proxy 認証形式
    u8  proxyHost[0x64];       // Proxy hostname
    u16 proxyPort;             // Proxy port
    u8  rsvEx    [0x06];       // 予備
}  NCFGApInfoEx;

// 内蔵バックアップメモリ 1 スロット分 (1 ページ)
typedef struct NCFGSlotInfo
{
    NCFGApInfo ap;             // アクセスポイント接続情報
    u8         wifi[0x0E];     // WiFi 情報
    u16        crc;            // CRC
} NCFGSlotInfo;

// 拡張設定の内蔵バックアップメモリ 1 スロット分 (2ページ)
typedef struct NCFGSlotInfoEx
{
    NCFGApInfo ap;             // アクセスポイント接続情報
    u8         wifi[0x0E];     // WiFi 情報
    u16        crc;            // CRC
    NCFGApInfoEx apEx;         // 拡張アクセスポイント接続情報
    u8         rsvEx[0x0E];    // 予備
    u16        crcEx;          // CRC
} NCFGSlotInfoEx;

// 内蔵バックアップメモリの全体マップ
typedef struct NCFGConfig
{
    NCFGSlotInfo   slot[3];
    NCFGSlotInfo   rsv;
} NCFGConfig;

// 拡張設定を含んだ内蔵バックアップメモリの全体マップ
typedef struct NCFGConfigEx
{
    NCFGSlotInfoEx slotEx[3];
    union {
        NCFGConfig compat;
        struct {
            NCFGSlotInfo slot[3];
            NCFGSlotInfo rsv;
        };
    };
} NCFGConfigEx;

#define NCFG_CHECKCONFIG_WORK_SIZE      0x700
#define NCFG_READCONFIG_WORK_SIZE       0x700
#define NCFG_WRITECONFIG_WORK_SIZE      0x700

#define NCFG_CHECKCONFIGEX_WORK_SIZE    0x1000
#define NCFG_READCONFIGEX_WORK_SIZE     0x1000
#define NCFG_WRITECONFIGEX_WORK_SIZE    0x1000
#endif



#endif /* _WIFI_CFG_H_ */
