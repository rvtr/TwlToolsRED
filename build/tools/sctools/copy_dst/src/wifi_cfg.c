#include <twl.h>
#include <nitro/types.h>
#include <nitro/wm.h>

#include <nitroWiFi.h>
#include <nitroWiFi/ncfg/ncfg_backup.h>
#include <nitroWiFi/ncfg/ncfg_info.h>

#include        "mynvram.h"

#include "wifi_cfg.h"

static u8 my_work[sizeof(NCFGConfigEx)];
static NCFGConfigEx *my_configEx = NULL;
static MATHCRC16Table my_tbl; // CRC 計算テーブル


static char SSID_STR[256];
static int SSID_LEN = 0;

static int key_mode = 0; /* 0->str 1->bin */
static char KEY_STR[256];
static u8 KEY_BIN[80];
static int KEY_STR_LEN = 0;
static int KEY_BIN_LEN = 0;
static int MODE = 0;

static int DHCP_MODE = 0;
static u32 IPADDR = 0;

static u32 NETMASK = 0;
static u32 GATEWAY = 0;
static u32 DNS1 = 0;
static u32 DNS2 = 0;

BOOL wifi_config_nvram_get( void )
{
  //  OS_TPrintf("sizeof(NCFGConfigEx) = %d(0x%0x)\n", sizeof(NCFGConfigEx),sizeof(NCFGConfigEx));
  if( TRUE == nvram_get(my_work) ) {
    my_configEx = (NCFGConfigEx *)my_work;
  }
  else {
    my_configEx = (NCFGConfigEx *)NULL;
    return FALSE;
  }
  return TRUE;
}


BOOL wifi_config_nvram_set( void )
{
  int i;
  if( my_configEx != NULL ) {

    MATH_CRC16InitTable(&my_tbl);

    for (i = 0; i < 3; i++) {
      my_configEx->slot[i].crc = MATH_CalcCRC16(&my_tbl, &my_configEx->slot[i], sizeof(my_configEx->slot[i])-2);
    }
    my_configEx->rsv.crc = MATH_CalcCRC16(&my_tbl, &my_configEx->rsv, sizeof(my_configEx->rsv)-2);

    for (i = 0; i < 3; i++)  {
      my_configEx->slotEx[i].crc   = MATH_CalcCRC16(&my_tbl, &my_configEx->slotEx[i],              0xFE);
      my_configEx->slotEx[i].crcEx = MATH_CalcCRC16(&my_tbl, (u8*)&my_configEx->slotEx[i] + 0x100, 0xFE);
    }

    if( TRUE == nvram_set(my_work) ) {
      return TRUE;      
    }
  }
  return FALSE;
}

BOOL wifi_config_clear_slot(int slot_no)
{
  if( my_configEx != NULL ) {
    if( (0 <= slot_no) && (slot_no <= 2) )  {
      STD_MemSet((void *)&(my_configEx->slotEx[slot_no]), 0, sizeof(NCFGSlotInfoEx) );
      return TRUE;
    }
    else if( (3 <= slot_no) && (slot_no <= 5) )  {
      STD_MemSet((void *)&(my_configEx->slot[slot_no]), 0, sizeof(NCFGSlotInfo) );
      return TRUE;
    }
  }
  return FALSE;
}


#if 0
enum  tagWEPNOTATION {
	WEP_NOTATION_HEX,          // 16進数表記
	WEP_NOTATION_ASCII,        // ASCII コード表記
};
// WPA モード種別
typedef enum
{
    NCFG_WPAMODE_WPA_PSK_TKIP   =   0x04,     /* WPA-PSK  ( TKIP ) 暗号 */
    NCFG_WPAMODE_WPA2_PSK_TKIP  =   0x05,     /* WPA2-PSK ( TKIP ) 暗号 */
    NCFG_WPAMODE_WPA_PSK_AES    =   0x06,     /* WPA-PSK  ( AES  ) 暗号 */
    NCFG_WPAMODE_WPA2_PSK_AES   =   0x07      /* WPA2-PSK ( AES  ) 暗号 */
} NCFGWPAMode
#endif

// BOOL wifi_config_set_proxy(int slot_no, u8 proxymode, u8 authtype, u8 *host, int host_len, u16 port)


BOOL wifi_config_set_wpa_key_str(int slot_no, int wpamode, char *wpakey )
{
  // if(  wpakey_len > 0x20 ) error;
  u8 ncfg_wpamode;
  
  if( my_configEx != NULL ) {
    if( (0 <= slot_no) && (slot_no <= 2) )  {
      switch(wpamode) {
      case WCM_WEPMODE_WPA_TKIP:
	ncfg_wpamode = NCFG_WPAMODE_WPA_PSK_TKIP;
	break;
      case WCM_WEPMODE_WPA2_TKIP:
	ncfg_wpamode = NCFG_WPAMODE_WPA2_PSK_TKIP;
	break;
      case WCM_WEPMODE_WPA_AES:
	ncfg_wpamode = NCFG_WPAMODE_WPA_PSK_AES;
	break;
      case WCM_WEPMODE_WPA2_AES:
	ncfg_wpamode = NCFG_WPAMODE_WPA2_PSK_AES;
	break;
      default:
	return FALSE;
      }

      my_configEx->slotEx[slot_no].ap.setType = NCFG_SETTYPE_MANUAL_WPA;
      /* NCFGWPAMode: WPA-AES | WPA2-AES | WPA-TKIP  WPA2-TKIP */
      my_configEx->slotEx[slot_no].apEx.wpa.wpaMode = ncfg_wpamode;

      STD_MemSet((void *)(my_configEx->slotEx[slot_no].apEx.wpa.wpaPassphrase), 0, 0x40 );
      // u8  wpaPassphrase[0x40];   // WPA パスフレーズ (NULL 終端文字列 or 64桁 HEX)
      STD_MemCpy((void *)(my_configEx->slotEx[slot_no].apEx.wpa.wpaPassphrase), (void *)wpakey, 
		 (unsigned long)STD_StrLen(wpakey) + 1 );

      NWM_Passphrase2PSK(my_configEx->slotEx[slot_no].apEx.wpa.wpaPassphrase, 
			 my_configEx->slotEx[slot_no].ap.ssid[0],
			 my_configEx->slotEx[slot_no].ap.ssidLength[0],
			 my_configEx->slotEx[slot_no].apEx.wpa.wpaKey);

      return TRUE;
    }
  }
  return FALSE;
}

BOOL wifi_config_set_wpa_key_bin(int slot_no, int wpamode, u8 *wpakey , int wpakey_len)
{
  // if(  wpakey_len > 0x20 ) error;
  u8 ncfg_wpamode;
  
  if( my_configEx != NULL ) {
    if( (0 <= slot_no) && (slot_no <= 2) )  {
      switch(wpamode) {
      case WCM_WEPMODE_WPA_TKIP:
	ncfg_wpamode = NCFG_WPAMODE_WPA_PSK_TKIP;
	break;
      case WCM_WEPMODE_WPA2_TKIP:
	ncfg_wpamode = NCFG_WPAMODE_WPA2_PSK_TKIP;
	break;
      case WCM_WEPMODE_WPA_AES:
	ncfg_wpamode = NCFG_WPAMODE_WPA_PSK_AES;
	break;
      case WCM_WEPMODE_WPA2_AES:
	ncfg_wpamode = NCFG_WPAMODE_WPA2_PSK_AES;
	break;
      default:
	return FALSE;
      }

      my_configEx->slotEx[slot_no].ap.setType = NCFG_SETTYPE_MANUAL_WPA;
      /* NCFGWPAMode: WPA-AES | WPA2-AES | WPA-TKIP  WPA2-TKIP */
      my_configEx->slotEx[slot_no].apEx.wpa.wpaMode = ncfg_wpamode;
      // u8  wpaPassphrase[0x40];   // WPA パスフレーズ (NULL 終端文字列 or 64桁 HEX)

      STD_MemSet((void *)(my_configEx->slotEx[slot_no].apEx.wpa.wpaPassphrase), 0, 0x40 );
      STD_MemCpy((void *)(my_configEx->slotEx[slot_no].apEx.wpa.wpaPassphrase), (void *)wpakey, (unsigned long)wpakey_len );
      NWM_Passphrase2PSK(my_configEx->slotEx[slot_no].apEx.wpa.wpaPassphrase, 
			 my_configEx->slotEx[slot_no].ap.ssid[0],
			 my_configEx->slotEx[slot_no].ap.ssidLength[0],
			 my_configEx->slotEx[slot_no].apEx.wpa.wpaKey);

      return TRUE;
    }
  }
  return FALSE;
}


BOOL wifi_config_set_wep_key_bin(int slot_no, int wepmode, u8 *wepkey, int wepkey_len )
{
  u8 ncfg_wepmode;

  if( my_configEx != NULL ) {
    switch(wepmode) {
    case WCM_WEPMODE_40:
      ncfg_wepmode = NCFG_WEPMODE_40;
      break;
    case WCM_WEPMODE_104:
      ncfg_wepmode = NCFG_WEPMODE_104;
      break;
    case WCM_WEPMODE_128:
      ncfg_wepmode = NCFG_WEPMODE_128;
      break;
    case WCM_WEPMODE_NONE:
      ncfg_wepmode = NCFG_WEPMODE_NONE;
      break;
    default:
      return FALSE;
    }


    if( (0 <= slot_no) && (slot_no <= 2) )  {
      my_configEx->slotEx[slot_no].ap.wepNotation = 0; // WEP_NOTATION_HEX
      my_configEx->slotEx[slot_no].ap.wepMode = ncfg_wepmode;
      STD_MemSet(my_configEx->slotEx[slot_no].ap.wep[0], 0, 0x10 );
      STD_MemCpy((char *)(my_configEx->slotEx[slot_no].ap.wep[0]), wepkey, (unsigned long)wepkey_len);
      return TRUE;
    }
    else if( (3 <= slot_no) && (slot_no <= 5) )  {
      my_configEx->slot[slot_no].ap.wepNotation = 0; // WEP_NOTATION_HEX
      my_configEx->slot[slot_no].ap.wepMode = ncfg_wepmode;
      STD_MemSet(my_configEx->slot[slot_no].ap.wep[0], 0, 0x10 );
      STD_MemCpy((char *)(my_configEx->slot[slot_no].ap.wep[0]), wepkey, (unsigned long)wepkey_len);
      return TRUE;
    }
  }
  return FALSE;
}

BOOL wifi_config_set_wep_key_str(int slot_no, int wepmode, char *wepkey )
{
  u8 ncfg_wepmode;

  if( my_configEx != NULL ) {
    switch(wepmode) {
    case WCM_WEPMODE_40:
      ncfg_wepmode = NCFG_WEPMODE_40;
      break;
    case WCM_WEPMODE_104:
      ncfg_wepmode = NCFG_WEPMODE_104;
      break;
    case WCM_WEPMODE_128:
      ncfg_wepmode = NCFG_WEPMODE_128;
      break;
    case WCM_WEPMODE_NONE:
      ncfg_wepmode = NCFG_WEPMODE_NONE;
      break;
    default:
      return FALSE;
    }


    if( (0 <= slot_no) && (slot_no <= 2) )  {
      my_configEx->slotEx[slot_no].ap.wepNotation = 1; // WEP_NOTATION_ASCII
      my_configEx->slotEx[slot_no].ap.wepMode = ncfg_wepmode;
      STD_MemSet(my_configEx->slotEx[slot_no].ap.wep[0], 0, 0x10 );
      STD_StrCpy((char *)(my_configEx->slotEx[slot_no].ap.wep[0]), wepkey);
      return TRUE;
    }
    else if( (3 <= slot_no) && (slot_no <= 5) )  {
      my_configEx->slot[slot_no].ap.wepNotation = 1; // WEP_NOTATION_ASCII
      my_configEx->slot[slot_no].ap.wepMode = ncfg_wepmode;
      STD_MemSet(my_configEx->slot[slot_no].ap.wep[0], 0, 0x10 );
      STD_StrCpy((char *)(my_configEx->slot[slot_no].ap.wep[0]), wepkey);
      return TRUE;
    }
  }
  return FALSE;
}

BOOL wifi_config_set_netmask(int slot_no, u32 mask)
{
  u8 buf[4];
  if( my_configEx != NULL ) {

    NCFGi_ConvMaskAddr((int)mask, buf);

    if( (0 <= slot_no) && (slot_no <= 2) )  {
      my_configEx->slotEx[slot_no].ap.netmask = NCFGi_ConvMaskCidr(buf);
      return TRUE;
    }
    else if( (3 <= slot_no) && (slot_no <= 5) )  {
      my_configEx->slot[slot_no].ap.netmask = NCFGi_ConvMaskCidr(buf);
      return TRUE;
    }
  }
  return FALSE;

}

BOOL wifi_config_set_ip_addr(int slot_no, u32 ipaddr)
{
  /* auto設定のときは ipaddr = 0 */
  int i;
  if( my_configEx != NULL ) {
    if( (0 <= slot_no) && (slot_no <= 2) )  {
      for( i = 0 ; i < 4 ; i++ ) {
	my_configEx->slotEx[slot_no].ap.ip[i] = (u8)(0xff & (ipaddr >> (24 - i*8)) );
      }
      return TRUE;
    }
    else if( (3 <= slot_no) && (slot_no <= 5) )  {
      for( i = 0 ; i < 4 ; i++ ) {
	my_configEx->slot[slot_no].ap.ip[i] = (u8)(0xff & (ipaddr >> (24 - i*8)) );
      }
      return TRUE;
    }
  }
  return FALSE;
}

BOOL wifi_config_set_gateway(int slot_no, u32 ipaddr)
{
  /* auto設定のときは ipaddr = 0 */
  int i;
  if( my_configEx != NULL ) {
    if( (0 <= slot_no) && (slot_no <= 2) )  {
      for( i = 0 ; i < 4 ; i++ ) {
	my_configEx->slotEx[slot_no].ap.gateway[i] = (u8)(0xff & (ipaddr >> (24 - i*8)) );
      }
      return TRUE;
    }
    else if( (3 <= slot_no) && (slot_no <= 5) )  {
      for( i = 0 ; i < 4 ; i++ ) {
	my_configEx->slot[slot_no].ap.gateway[i] = (u8)(0xff & (ipaddr >> (24 - i*8)) );
      }
      return TRUE;
    }
  }
  return FALSE;
}


BOOL wifi_config_set_dns(int slot_no, int dns_no, u32 dnsaddr)
{
  /* auto設定のときは dnsaddr = 0 */
  int i;
  if( my_configEx != NULL ) {
    if( (0 <= slot_no) && (slot_no <= 2) )  {
      for( i = 0 ; i < 4 ; i++ ) {
	my_configEx->slotEx[slot_no].ap.dns[dns_no][i] = (u8)(0xff & (dnsaddr >> (24 - i*8)) );
      }
      return TRUE;
    }
    else if( (3 <= slot_no) && (slot_no <= 5) )  {
      for( i = 0 ; i < 4 ; i++ ) {
	my_configEx->slot[slot_no].ap.dns[dns_no][i] = (u8)(0xff & (dnsaddr >> (24 - i*8)) );
      }
      return TRUE;
    }
  }
  return FALSE;
}

BOOL wifi_config_set_ssid(int slot_no, char *ssid_str, int ssid_len)
{
  int i;
  char *str;
  if( (ssid_len <= 0) || (0x20 <= ssid_len) ) {
    return FALSE;
  }
  if( ssid_str == NULL ) {
    return FALSE;
  }
  str = ssid_str;


  if( my_configEx != NULL ) {
    if( (0 <= slot_no) && (slot_no <= 2) )  {

      for( i = 0 ; i < ssid_len ; i++ ) {
	my_configEx->slotEx[slot_no].ap.ssid[0][i] = (u8)*str++;
      }
      my_configEx->slotEx[slot_no].ap.ssidLength[0] = (u8)ssid_len;
      return TRUE;
    }
    else if( (3 <= slot_no) && (slot_no <= 5) )  {
      for( i = 0 ; i < ssid_len ; i++ ) {
	my_configEx->slot[slot_no].ap.ssid[0][i] = (u8)*str++;
      }
      my_configEx->slot[slot_no].ap.ssidLength[0] = (u8)ssid_len;
      return TRUE;
    }
  }
  return FALSE;
}



void wifi_config_print(void)
{
  int i,j;
  int slot_no;


  OS_TPrintf("sizeof= %d, %x\n",sizeof(NCFGConfigEx), sizeof(NCFGConfigEx)) ;

  if( my_configEx != NULL ) {
    slot_no = 0;
    for( j = 0 ; j < 3 ; j++ ) {
      OS_TPrintf("%d SSID:\n", slot_no);
      for( i = 0 ; i < 0x10 ; i++ ) {
	OS_TPrintf("0x%02x ",my_configEx->slotEx[j].ap.ssid[0][i]);
      }
      OS_TPrintf("\n");
    
      for( i = 0x10 ; i < 0x20 ; i++ ) {
	OS_TPrintf("0x%02x ",my_configEx->slotEx[j].ap.ssid[0][i]);
      }
      OS_TPrintf("\n");

      OS_TPrintf("wpaKey:0x");
      for( i = 0 ; i < 0x20 ; i++ ) {      
	OS_TPrintf("%02x",my_configEx->slotEx[j].apEx.wpa.wpaKey[i]);
      }
      OS_TPrintf("\n");

      OS_TPrintf("wpaPassphrase:0x");
      for( i = 0 ; i < 0x40 ; i++ ) {      
	OS_TPrintf("%02x",my_configEx->slotEx[j].apEx.wpa.wpaPassphrase[i]);
      }
      OS_TPrintf("\n");

      
#if 0
      OS_TPrintf("IP addr:");
    
      for( i = 0 ; i < 4 ; i++ ) {
	OS_TPrintf("%3d.",my_configEx->slotEx[j].ap.ip[i]);
      }
      OS_TPrintf("\n");
#endif
      slot_no++;
    }

#if 0
    for( j = 0 ; j < 3 ; j++ ) {
      OS_TPrintf("%d SSID:\n", slot_no);
      for( i = 0 ; i < 0x10 ; i++ ) {
	OS_TPrintf("0x%02x ",my_configEx->slot[j].ap.ssid[0][i]);
      }
      OS_TPrintf("\n");
    
      for( i = 0x10 ; i < 0x20 ; i++ ) {
	OS_TPrintf("0x%02x ",my_configEx->slot[j].ap.ssid[0][i]);
      }
      OS_TPrintf("\n");
    
      OS_TPrintf("IP addr:");
    
      for( i = 0 ; i < 4 ; i++ ) {
	OS_TPrintf("%3d.",my_configEx->slot[j].ap.ip[i]);
      }
      OS_TPrintf("\n");
      slot_no++;
    }
#endif
  }
  else {
    OS_TPrintf("NCFG_ReadConfigEx failed.\n");
  }


  //BOOL nvram_set(my_work);

}




