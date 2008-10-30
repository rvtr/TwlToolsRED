#include <twl.h>
#include <nitroWiFi.h>

#include        "text.h"
#include        "mprintf.h"
#include        "logprintf.h"

#include        "mywlan.h"

/*
IP addr = 192.168. 11.  6
NetMask = 255.255.255.  0
GW addr = 192.168. 11.  1
DNS[0]  = 192.168. 11.  1
DNS[1]  =   0.  0.  0.  0
*/

static char SSID_STR[256];
static int SSID_LEN = 0;
static int key_mode = 0; /* 0->str 1->bin */
static char KEY_STR[256];

static u8 KEY_BIN[MAX_KEY_BIN_BUF];
static int KEY_STR_LEN = 0;
static int KEY_BIN_LEN = 0;
static int MODE = 0;

static int DHCP_MODE = 0;
static u32 IPADDR = 0;
static u32 NETMASK = 0;
static u32 GATEWAY = 0;
static u32 DNS1 = 0;
static u32 DNS2 = 0;

BOOL GetKeyModeStr(void)
{
  if( key_mode ) {
    return FALSE;
  }
  return TRUE;
}


u32 GetGateway(void)
{
  return GATEWAY;
}

u32 GetNetmask(void)
{
  return NETMASK;
}

u32 GetIPAddr(void)
{
  return IPADDR;
}

BOOL GetDhcpMODE(void)
{
  if( DHCP_MODE ) {
    return TRUE;
  }
  return FALSE;
}


u32 GetDNS1(void)
{
  return DNS1;
}

u32 GetDNS2(void)
{
  return DNS2;
}

char *GetWlanSSID(void)
{
  return SSID_STR;
}

char *GetWlanKEYSTR(void)
{
  if( KEY_STR_LEN ) {
    return KEY_STR;
  }
  return NULL;
}

int GetWlanKEYBIN(u8 *buf)
{
  if( KEY_BIN_LEN ) {
    STD_CopyMemory((void *)buf, (void *)KEY_BIN, (unsigned long)KEY_BIN_LEN);
  }
  return KEY_BIN_LEN;
}

int GetWlanMode(void)
{
  return MODE;
}

/*
SSID:"001D731A8202"
MODE:"WEP128"
;MODE:"OPEN"
;MODE:"WPA-TKIP"
;MODE:"WPA2-TKIP"
;MODE:"WPA-AES"
;MODE:"WPA2-AES"
KEY-STR:"0123456789red"
KEY-BIN:"0123456789red"
*/

static int ReadLine(FSFile *f, char *buf, int buf_size)
{
  char c;
  s32 readSize;
  int count = 0;
  while( 1 ) {
    readSize = FS_ReadFile(f, (void *)&c, (s32)1 );
    if( readSize == 0 ) {
      /* EOF */
      break;
    }
    count++;
    if( c == '\r' ) {
      *buf = '\0';
      break;
    }
    else if(c == '\n' ) {
      *buf = '\0';
      break;
    }

    *buf = c;
    buf++;

    if( count > buf_size ) {
      break;
    }
  }
  return count;
}


static BOOL Addr_scan(char *buf, int size, int cnt, u32 *ret_addr)
{
  char c;
  int keta;
  int area;
  u32 num[3];
  u32 addr;
  int readSize;
  int count;
  int count2;
  char *line_buf;
  line_buf = buf;
  readSize = size;

  count = cnt;

  if( line_buf[count] == '\"' /* ‚PŒÂ–Ú */) {
    count++;
    count2 = count;
    
    addr = 0;
    keta = 0;
    area = 0;
    num[0] = 0;
    num[1] = 0;
    num[2] = 0;
    while( readSize > count ) {
      c = line_buf[count];
      if( c == '\"' /* ‚QŒÂ–Ú */) {
	if( area == 3 ) {
	  if( keta == 1 ) {
	    addr |= ( ( num[0] ) << (8*(3 - area)) );
	  }
	  else if( keta == 2 ) {
	    addr |= ( (num[0] * 10 + num[1]) << (8*(3 - area)) );
	  }
	  else if( keta == 3 ) {
	    addr |= ( (num[0] * 100 + num[1] * 10 + num[2]) << (8*( 3 - area)) );
	  }
	  else {
	    OS_TPrintf("Error: %s %d\n",__FUNCTION__,__LINE__);
	  }
	  *ret_addr = addr;
	  return TRUE;
	}
	else {
	  OS_TPrintf("Error: %s %d\n",__FUNCTION__,__LINE__);
	  return FALSE;
	}
      }
      if( ('0' <= c) && (c <= '9') ) {
	if( keta > 3 ) {
	  /* error */
	  OS_TPrintf("Error: %s %d\n",__FUNCTION__,__LINE__);
	  return FALSE;
	}
	num[keta] = (u32)( c - '0' );
	keta++;
      }
      else if( c == '.' ){
	if( keta == 1 ) {
	  addr |= ( ( num[0] ) << (8*(3 - area)) );
	}
	else if( keta == 2 ) {
	  addr |= ( (num[0] * 10 + num[1]) << (8*(3 - area)) );
	}
	else if( keta == 3 ) {
	  addr |= ( (num[0] * 100 + num[1] * 10 + num[2]) << (8*( 3 - area)) );
	}
	else {
	  return FALSE;
	  OS_TPrintf("Error: %s %d\n",__FUNCTION__,__LINE__);
	}
	keta = 0;
	area++;
      }
      else {
	OS_TPrintf("Error: %s %d\n",__FUNCTION__,__LINE__);
	return FALSE;
      }
      count++;
    }
  }
  return FALSE;
}




BOOL LoadWlanConfigFile(char *path)
{
  FSFile f;
  FSResult res;
  s32 readSize;
  BOOL bSuccess;
  int count = 0;
  int count2;
  BOOL ssid_flag;
  BOOL key_flag;
  BOOL mode_flag;

  BOOL dhcp_flag;
  BOOL ip_addr_flag;
  BOOL net_mask_flag;
  BOOL gw_flag;
  BOOL dns1_flag;
  BOOL dns2_flag;

  BOOL ret_flag = FALSE;
  u8 hex;
  char c;


#define LINE_BUF_SIZE 256
  char line_buf[LINE_BUF_SIZE];

  FS_InitFile(&f);
  bSuccess = FS_OpenFileEx(&f, path, FS_FILEMODE_R);
  if (bSuccess == FALSE) {
    res = FS_GetArchiveResultCode(path);
    return FALSE;
  }


  ssid_flag = FALSE;
  key_flag = FALSE;
  mode_flag = FALSE;
  dhcp_flag = FALSE;
  ip_addr_flag = FALSE;
  net_mask_flag = FALSE;
  gw_flag = FALSE;
  dns1_flag = FALSE;
  dns2_flag = FALSE;


  while( 1 ) {
    readSize = ReadLine(&f, line_buf, LINE_BUF_SIZE );
    if( readSize == 0 ) {
      /* EOF */
      break;
    }

    if( readSize > 5 ) {
      if( !dhcp_flag  &&  !STD_StrNCmp( line_buf, "DHCP:" , STD_StrLen("DHCP:")) ) {
	count = STD_StrLen("DHCP:");
	if( line_buf[count] == '\"' /* ‚PŒÂ–Ú */) {
	  count++;
	  count2 = count;
	  if( !STD_StrNCmp( &(line_buf[count2]), "OFF" , STD_StrLen("OFF")) ) {
	    if( line_buf[ count2 + STD_StrLen("OFF") ] == '\"' /* ‚QŒÂ–Ú */) {
	      dhcp_flag = TRUE;
	      DHCP_MODE = 0;
	      OS_TPrintf("DHCP OFF %s %d\n",__FUNCTION__,__LINE__);
	    }
	  }
	  else if( !STD_StrNCmp( &(line_buf[count2]), "ON" , STD_StrLen("ON")) ) {
	    if( line_buf[ count2 + STD_StrLen("ON") ] == '\"' /* ‚QŒÂ–Ú */) {
	      dhcp_flag = TRUE;
	      DHCP_MODE = 1;
	      OS_TPrintf("DHCP ON %s %d\n",__FUNCTION__,__LINE__);
	    }
	  }
	}
      }
      else if( !ip_addr_flag  &&  !STD_StrNCmp( line_buf, "IPADDR:" , STD_StrLen("IPADDR:")) ) {
	count = STD_StrLen("IPADDR:");
	if( TRUE == Addr_scan(line_buf, readSize, count, &IPADDR) ) {
	  OS_TPrintf("IPADDR %d.%d.%d.%d\n", (u32)((IPADDR >> 24) & 0xff),(u32)((IPADDR >> 16) & 0xff),
		     (u32)((IPADDR >> 8) & 0xff),(u32)(IPADDR & 0xff) );
	  ip_addr_flag = TRUE;
	}
      }
      else if( !net_mask_flag  &&  !STD_StrNCmp( line_buf, "NETMASK:" , STD_StrLen("NETMASK:")) ) {
	count = STD_StrLen("NETMASK:");
	if( TRUE == Addr_scan(line_buf, readSize, count, &NETMASK) ) {
	  OS_TPrintf("NETMASK %d.%d.%d.%d\n", (u32)((NETMASK >> 24) & 0xff),(u32)((NETMASK >> 16) & 0xff),
		     (u32)((NETMASK >> 8) & 0xff),(u32)(NETMASK & 0xff) );
	  net_mask_flag = TRUE;
	}

      }
      else if( !gw_flag  &&  !STD_StrNCmp( line_buf, "GATEWAY:" , STD_StrLen("GATEWAY:")) ) {
	count = STD_StrLen("GATEWAY:");
	if( TRUE == Addr_scan(line_buf, readSize, count, &GATEWAY) ) {
	  OS_TPrintf("GATEWAY %d.%d.%d.%d\n", (u32)((GATEWAY >> 24) & 0xff),(u32)((GATEWAY >> 16) & 0xff),
		     (u32)((GATEWAY >> 8) & 0xff),(u32)(GATEWAY & 0xff) );
	  gw_flag = TRUE;
	}
      }
      else if( !dns1_flag  &&  !STD_StrNCmp( line_buf, "DNS1:" , STD_StrLen("DNS1:")) ) {
	count = STD_StrLen("DNS1:");
	if( TRUE == Addr_scan(line_buf, readSize, count, &DNS1) ) {
	  OS_TPrintf("DNS1 %d.%d.%d.%d\n", (u32)((DNS1 >> 24) & 0xff),(u32)((DNS1 >> 16) & 0xff),
		     (u32)((DNS1 >> 8) & 0xff),(u32)(DNS1 & 0xff) );
	  dns1_flag = TRUE;
	}
      }
      else if( !dns2_flag  &&  !STD_StrNCmp( line_buf, "DNS2:" , STD_StrLen("DNS2:")) ) {
	count = STD_StrLen("DNS2:");
	if( TRUE == Addr_scan(line_buf, readSize, count, &DNS2) ) {
	  OS_TPrintf("DNS2 %d.%d.%d.%d\n", (u32)((DNS2 >> 24) & 0xff),(u32)((DNS2 >> 16) & 0xff),
		     (u32)((DNS2 >> 8) & 0xff),(u32)(DNS2 & 0xff) );
	  dns2_flag = TRUE;
	}
      }
      else if( !ssid_flag  &&  !STD_StrNCmp( line_buf, "SSID:" , STD_StrLen("SSID:")) ) {
	count = STD_StrLen("SSID:");
	if( line_buf[count] == '\"' /* ‚PŒÂ–Ú */) {
	  count++;
	  count2 = count;
	  while( readSize > count ) {
	    if( line_buf[count] == '\"' /* ‚QŒÂ–Ú */) {
	      SSID_LEN = count - 1;
	      SSID_STR[count - count2] = '\0';
	      ssid_flag = TRUE;
	      break;
	    }
	    SSID_STR[count - count2] = line_buf[count];
	    count++;
	  }
	}
      }
      else if( !mode_flag && !STD_StrNCmp( line_buf, "MODE:" , STD_StrLen("MODE:")) ) {
	count = STD_StrLen("MODE:");
	if( line_buf[count] == '\"' /* ‚PŒÂ–Ú */) {
	  count++;
	  count2 = count;
	  if( !STD_StrNCmp( &(line_buf[count2]), "NONE" , STD_StrLen("NONE")) ) {
	    // ;MODE:"NONE"
	    if( line_buf[ count2 + STD_StrLen("NONE") ] == '\"' /* ‚QŒÂ–Ú */) {
	      /* 0 */
	      MODE = WCM_WEPMODE_NONE;
	      mode_flag = TRUE;
	    }
	  }
	  else if( !STD_StrNCmp( &(line_buf[count2]), "WEP40" , STD_StrLen("WEP40")) ) {
	      /* 1 */
	    if( line_buf[ count2 + STD_StrLen("WEP40") ] == '\"' /* ‚QŒÂ–Ú */) {
	      MODE = WCM_WEPMODE_40;
	      mode_flag = TRUE;
	    }
	  }
	  else if( !STD_StrNCmp( &(line_buf[count2]), "WEP104" , STD_StrLen("WEP104")) ) {
	      /* 2 */
	    if( line_buf[ count2 + STD_StrLen("WEP104") ] == '\"' /* ‚QŒÂ–Ú */) {
	      MODE = WCM_WEPMODE_104;
	      mode_flag = TRUE;
	    }
	  }
	  else if( !STD_StrNCmp( &(line_buf[count2]), "WEP128" , STD_StrLen("WEP128")) ) {
	      /* 3 */
	    if( line_buf[ count2 + STD_StrLen("WEP128") ] == '\"' /* ‚QŒÂ–Ú */) {
	      MODE = WCM_WEPMODE_128;
	      mode_flag = TRUE;
	    }
	  }
	  else if( !STD_StrNCmp( &(line_buf[count2]),  "WPA-TKIP", STD_StrLen("WPA-TKIP")) ) {
	    /* 4 */
	    if( line_buf[ count2 + STD_StrLen("WPA-TKIP") ] == '\"' /* ‚QŒÂ–Ú */) {
	      MODE = WCM_WEPMODE_WPA_TKIP;
	      mode_flag = TRUE;
	    }
	  }
	  else if( !STD_StrNCmp( &(line_buf[count2]),  "WPA2-TKIP", STD_StrLen("WPA2-TKIP")) ) {
	    if( line_buf[ count2 + STD_StrLen("WPA2-TKIP") ] == '\"' /* ‚QŒÂ–Ú */) {
	      MODE = WCM_WEPMODE_WPA2_TKIP;
	      mode_flag = TRUE;
	    }
	  }
	  else if( !STD_StrNCmp( &(line_buf[count2]),  "WPA-AES", STD_StrLen("WPA-AES")) ) {
	    if( line_buf[ count2 + STD_StrLen("WPA-AES") ] == '\"' /* ‚QŒÂ–Ú */) {
	      MODE = WCM_WEPMODE_WPA_AES;
	      mode_flag = TRUE;
	    }
	  }
	  else if( !STD_StrNCmp( &(line_buf[count2]), "WPA2-AES" , STD_StrLen("WPA2-AES")) ) {
	    if( line_buf[ count2 + STD_StrLen("WPA2-AES") ] == '\"' /* ‚QŒÂ–Ú */) {
	      MODE = WCM_WEPMODE_WPA2_AES;
	      mode_flag = TRUE;
	    }
	  }
	}
      }	/* MODE */
      else if( !key_flag && !STD_StrNCmp( line_buf, "KEY-STR:" , STD_StrLen("KEY-STR:")) ) {
	count = STD_StrLen("KEY-STR:");
	if( line_buf[count] == '\"' /* ‚PŒÂ–Ú */) {
	  count++;
	  count2 = count;
	  while( readSize > count ) {
	    if( line_buf[count] == '\"' /* ‚QŒÂ–Ú */) {
	      KEY_STR_LEN = count - 1;
	      KEY_STR[count - count2] = '\0';
	      /* ‚±‚Ì‚Æ‚«wep128‚È‚çcount‚Íi‚P‚R•¶Žš‚È‚Ì‚Åj */
	      // 0123456789012345678901               
	      // KEY-STR:"0123456789red"
	      key_mode = 0; /* 0->str 1->bin */
	      key_flag = TRUE;
	      break;
	    }
	    KEY_STR[count - count2] = line_buf[count];
	    count++;
	  }
	}
      }
      else if( !key_flag && !STD_StrNCmp( line_buf, "KEY-BIN:" , STD_StrLen("KEY-BIN:")) ) {
	count = STD_StrLen("KEY-BIN:");
	hex = 0;
	if( line_buf[count] == '\"' /* ‚PŒÂ–Ú */) {
	  count++;
	  count2 = 0;
	  while( readSize > count ) {
	    if( line_buf[count] == '\"' /* ‚QŒÂ–Ú */) {
	      KEY_BIN_LEN = (count2+1)/2;
	      key_mode = 1; /* 0->str 1->bin */
	      key_flag = TRUE;
	      break;
	    }
	    c = line_buf[count];
	    OS_PutChar( c );

	    if( ('a' <= c) && (c <= 'f') ) {
	      hex = (u8)( c - 'a' + 10 );
	    }
	    else if( ('A' <= c) && (c <= 'F') ) {
	      hex = (u8)( c - 'a' + 10 );
	    }
	    else if( ('0' <= c) && (c <= '9') ) {
	      hex = (u8)(c - '0');
	    }
	    else {
	      OS_TPrintf("Error keybin: %s %d\n",__FUNCTION__,__LINE__);
	      /* error! */
	      break;
	    }
	    if( (count2 & 1) == 0 ) {
	      KEY_BIN[count2/2] = (u8)(hex << 4);
	    }
	    else {
	      KEY_BIN[count2/2] |= hex;
	    }
	    count2++;
	    if( count2 >= (MAX_KEY_BIN_BUF*2) ) {
	      OS_TPrintf("Error keybin: buffer overflow %s %d\n",__FUNCTION__,__LINE__);
	      break;
	    }
	    count++;
	  }
	}
      }
    }
    /*123456789012345678
      SSID:"001D731A8202"
      MODE:"WEP128"
      ;MODE:"NONE"
      ;MODE:"WPA-TKIP"
      ;MODE:"WPA2-TKIP"
      ;MODE:"WPA-AES"
      ;MODE:"WPA2-AES"
      KEY-STR:"0123456789red"
      KEY-BIN:"0123456789red"
    */


  }

  if( ssid_flag == TRUE && (key_flag == TRUE) && mode_flag == TRUE && (dhcp_flag == TRUE) ) {
    if( DHCP_MODE == 0 ) {
      if( (ip_addr_flag == TRUE)  && (net_mask_flag == TRUE) && (gw_flag == TRUE) && (dns1_flag == TRUE) ) {
	// dns2_flag = FALSE;
	ret_flag = TRUE;
      }
      else {
	ret_flag = FALSE;
      }
    }
    else {
      ret_flag = TRUE;
    }
  }


  
  if( FS_CloseFile(&f) == FALSE) {
    res = FS_GetArchiveResultCode(path);
    return FALSE;
  }

  return ret_flag;
}



