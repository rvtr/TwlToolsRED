#ifndef	_WIFI_CFG_H_
#define	_WIFI_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_CFG_WEP_NOTATION_HEX   0  // 16�i���\�L
#define WIFI_CFG_WEP_NOTATION_ASCII 1  // ASCII �R�[�h�\�L

#define WIFI_CFG_PROXY_MODE_NONE   0  /* �v���L�V���g�p���Ȃ� */
#define WIFI_CFG_PROXY_MODE_NORMAL 1  /* �ʏ�̃v���L�V */

#define WIFI_CFG_PROXY_AUTHTYPE_NONE  0  /* �F�؂Ȃ� */
#define WIFI_CFG_PROXY_AUTHTYPE_BASIC 1  /* BASIC �F�� */



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


// WEP �Í������[�h���
// WCM_WEPMODE_* ���R�s�[
typedef enum
{
    NCFG_WEPMODE_NONE = 0,              // �Í����Ȃ�
    NCFG_WEPMODE_40   = 1,              // RC4 (  40 �r�b�g ) �Í������[�h
    NCFG_WEPMODE_104  = 2,              // RC4 ( 104 �r�b�g ) �Í������[�h
    NCFG_WEPMODE_128  = 3               // RC4 ( 128 �r�b�g ) �Í������[�h
} NCFGWEPMode;

// WEP �\�L
// util/setting.h ���R�s�[
typedef enum
{
    NCFG_WEP_NOTATION_HEX,              // 16�i���\�L
    NCFG_WEP_NOTATION_ASCII             // ASCII �R�[�h�\�L
} NCFGWEPNotation;

// Proxy ���[�h
typedef enum
{
    NCFG_PROXY_MODE_NONE               =    0x00,  /* �v���L�V���g�p���Ȃ� */
    NCFG_PROXY_MODE_NORMAL             =    0x01   /* �ʏ�̃v���L�V */
} NCFGProxyMode;

// Proxy �F�،`��
typedef enum
{
    NCFG_PROXY_AUTHTYPE_NONE           =    0x00,  /* �F�؂Ȃ� */
    NCFG_PROXY_AUTHTYPE_BASIC          =    0x01   /* BASIC �F�� */
} NCFGProxyAuthType;

// WPA ���[�h���
typedef enum
{
    NCFG_WPAMODE_WPA_PSK_TKIP   =   0x04,     /* WPA-PSK  ( TKIP ) �Í� */
    NCFG_WPAMODE_WPA2_PSK_TKIP  =   0x05,     /* WPA2-PSK ( TKIP ) �Í� */
    NCFG_WPAMODE_WPA_PSK_AES    =   0x06,     /* WPA-PSK  ( AES  ) �Í� */
    NCFG_WPAMODE_WPA2_PSK_AES   =   0x07      /* WPA2-PSK ( AES  ) �Í� */
} NCFGWPAMode;

// �e��t���O
typedef enum
{
    NCFG_FLAG_NONE              =   0x00,
    NCFG_FLAG_ALWAYS_ACTIVE     =   0x01,     /* PowerSave �𖳌��ɂ��� (���g�p) */
    NCFG_FLAG_SAFE_MTU          =   0x02      /* MTU �� 576 �o�C�g�ɂ��� (���g�p/�폜�\��) */
} NCFGFlag;    OS_TPrintf("IP addr:");


// ���[�N�������T�C�Y
#define NCFG_CHECKCONFIG_WORK_SIZE      0x700
#define NCFG_READCONFIG_WORK_SIZE       0x700
#define NCFG_WRITECONFIG_WORK_SIZE      0x700

#define NCFG_CHECKCONFIGEX_WORK_SIZE    0x1000
#define NCFG_READCONFIGEX_WORK_SIZE     0x1000
#define NCFG_WRITECONFIGEX_WORK_SIZE    0x1000


/*---------------------------------------------------------------------------*
    �}�N�� ��`
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    �\���� ��`
 *---------------------------------------------------------------------------*/

// �A�N�Z�X�|�C���g�ڑ����
typedef struct NCFGApInfo
{
    u8  ispId    [0x20];       // ISP �p ID
    u8  ispPass  [0x20];       // ISP �p�p�X���[�h
    u8  ssid  [2][0x20];       // SSID (ssid[1] : AOSS WEP40 �p)
    u8  wep   [4][0x10];       // WEP �L�[
    u8  ip       [0x04];       // IP �A�h���X
    u8  gateway  [0x04];       // �f�t�H���g�Q�[�g�E�F�C
    u8  dns   [2][0x04];       // DNS
    u8  netmask;               // �T�u�l�b�g�}�X�N
    u8  wep2  [4][0x05];       // WEP �L�[ (AOSS �p)
    u8  authType;              // �F�ؕ��@
    u8  wepMode      : 2;      // WEP ���[�h
    u8  wepNotation  : 6;      // WEP �\�L
    u8  setType;               // �ݒ���@
    u8  ssidLength[2];         // SSID �� (0 �Ȃ�� ssid �� NULL ������܂�)
    u16 mtu;                   // MTU �� (0 �Ȃ�΃f�t�H���g�l�ŉ���)
    u8  rsv      [0x02];       // �\��
    u8  flags;                 // �t���O (NCFG_FLAG_*)
    u8  state;                 // �g�p��
}  NCFGApInfo;

// �g���A�N�Z�X�|�C���g�ڑ����
typedef struct NCFGApInfoEx
{
    union {
        struct {
            u8  wpaKey[0x20];      // WPA PMK
            u8  wpaPassphrase[0x40];   // WPA �p�X�t���[�Y (NULL �I�[������ or 64�� HEX)
            u8  rsv[0x21];
            u8  wpaMode;           // WPA ���[�h
        } wpa;
        struct {
            u8  wpaKey[2][0x20];   // WPA PMK (AOSS �p [0]:TKIP, [1]:AES)
            u8  ssid[2][0x20];     // SSID (AOSS �p [0]:TKIP, [1]:AES)
            u8  ssidLength[2];     // SSID �� (AOSS �p [0]:TKIP, [1]:AES)
        } aossEx;
    };
    u8  proxyMode;             // Proxy mode
    u8  proxyAuthType;         // Proxy �F�،`��
    u8  proxyHost[0x64];       // Proxy hostname
    u16 proxyPort;             // Proxy port
    u8  rsvEx    [0x06];       // �\��
}  NCFGApInfoEx;

// �����o�b�N�A�b�v������ 1 �X���b�g�� (1 �y�[�W)
typedef struct NCFGSlotInfo
{
    NCFGApInfo ap;             // �A�N�Z�X�|�C���g�ڑ����
    u8         wifi[0x0E];     // WiFi ���
    u16        crc;            // CRC
} NCFGSlotInfo;

// �g���ݒ�̓����o�b�N�A�b�v������ 1 �X���b�g�� (2�y�[�W)
typedef struct NCFGSlotInfoEx
{
    NCFGApInfo ap;             // �A�N�Z�X�|�C���g�ڑ����
    u8         wifi[0x0E];     // WiFi ���
    u16        crc;            // CRC
    NCFGApInfoEx apEx;         // �g���A�N�Z�X�|�C���g�ڑ����
    u8         rsvEx[0x0E];    // �\��
    u16        crcEx;          // CRC
} NCFGSlotInfoEx;

// �����o�b�N�A�b�v�������̑S�̃}�b�v
typedef struct NCFGConfig
{
    NCFGSlotInfo   slot[3];
    NCFGSlotInfo   rsv;
} NCFGConfig;

// �g���ݒ���܂񂾓����o�b�N�A�b�v�������̑S�̃}�b�v
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
