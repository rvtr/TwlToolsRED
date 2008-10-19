#include <twl.h>
#include <nitroWiFi.h>

#include        "text.h"
#include        "mprintf.h"
#include        "logprintf.h"

#include        "mywlan.h"

static char SSID_STR[256];
static int SSID_LEN = 0;
static char KEY_STR[256];
static u8 KEY_BIN[256];
static int KEY_STR_LEN = 0;
static int KEY_BIN_LEN = 0;
static int MODE = 0;

char *GetWlanSSID(void)
{
  return SSID_STR;
}

char *GetWlanKEYSTR(void)
{
  return KEY_STR;
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


BOOL LoadWlanConfigFile(char *path)
{
  FSFile f;
  FSResult res;
  s32 readSize;
  BOOL bSuccess;
  int count = 0;
  int count2;
  int count3;
  BOOL ssid_flag;
  BOOL key_str_flag;
  BOOL key_bin_flag;
  BOOL mode_flag;
  BOOL ret_flag = FALSE;
  int lo_hi;
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
  key_str_flag = FALSE;
  key_bin_flag = FALSE;
  mode_flag = FALSE;


  while( 1 ) {
    readSize = ReadLine(&f, line_buf, LINE_BUF_SIZE );
    if( readSize == 0 ) {
      /* EOF */
      break;
    }


    if( readSize > 5 ) {
      if( !ssid_flag  &&  !STD_StrNCmp( line_buf, "SSID:" , STD_StrLen("SSID:")) ) {
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
	      MODE = WCM_WEPMODE_NONE;
	      mode_flag = TRUE;
	    }
	  }
	  else if( !STD_StrNCmp( &(line_buf[count2]), "WEP40" , STD_StrLen("WEP40")) ) {
	    if( line_buf[ count2 + STD_StrLen("WEP40") ] == '\"' /* ‚QŒÂ–Ú */) {
	      MODE = WCM_WEPMODE_40;
	      mode_flag = TRUE;
	    }
	  }
	  else if( !STD_StrNCmp( &(line_buf[count2]), "WEP104" , STD_StrLen("WEP104")) ) {
	    if( line_buf[ count2 + STD_StrLen("WEP104") ] == '\"' /* ‚QŒÂ–Ú */) {
	      MODE = WCM_WEPMODE_104;
	      mode_flag = TRUE;
	    }
	  }

	  else if( !STD_StrNCmp( &(line_buf[count2]), "WEP128" , STD_StrLen("WEP128")) ) {
	    if( line_buf[ count2 + STD_StrLen("WEP128") ] == '\"' /* ‚QŒÂ–Ú */) {
	      MODE = WCM_WEPMODE_128;
	      mode_flag = TRUE;
	    }
	  }
	  else if( !STD_StrNCmp( &(line_buf[count2]),  "WPA-TKIP", STD_StrLen("WPA-TKIP")) ) {
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

      }
      else if( !key_str_flag && !STD_StrNCmp( line_buf, "KEY-STR:" , STD_StrLen("KEY-STR:")) ) {
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
	      key_str_flag = TRUE;
	      break;
	    }
	    KEY_STR[count - count2] = line_buf[count];
	    count++;
	  }
	}
      }
      else if( !key_bin_flag && !STD_StrNCmp( line_buf, "KEY-BIN:" , STD_StrLen("KEY-BIN:")) ) {
	count = STD_StrLen("KEY-BIN:");
	lo_hi  = 0;
	hex = 0;
	if( line_buf[count] == '\"' /* ‚PŒÂ–Ú */) {
	  count++;
	  count2 = count;
	  count3 = 0;
	  while( readSize > count ) {
	    if( line_buf[count] == '\"' /* ‚QŒÂ–Ú */) {
	      if( lo_hi == 0 ) {
		KEY_BIN_LEN = count3;
		key_bin_flag = TRUE;
	      }
	      break;
	    }
	    c = line_buf[count];

	    if( ('a' <= c) && (c <= 'f') ) {
	      if( lo_hi == 0 ) {
		hex = (u8)(( c - 'a' + 10 ) * 16);
	      }
	      else {
		hex += (u8)( c - 'a' + 10 );
	      }
	    }
	    else if( ('A' <= c) && (c <= 'F') ) {
	      if( lo_hi == 0 ) {
		hex = (u8)(( c - 'A' + 10 ) * 16);
	      }
	      else {
		hex += (u8)( c - 'a' + 10 );
	      }
	    }
	    else if( ('0' <= c) && (c <= '9') ) {
	      if( lo_hi == 0 ) {
		hex = (u8)( (c - '0' ) * 16 );
	      }
	      else {
		hex += (u8)(c - '0');
	      }
	    }
	    else {
	      /* error! */
	      break;
	    }
	    if( lo_hi == 1 ) {
	      KEY_BIN[count3] = hex;
	      count3++;
	    }
	    lo_hi ^= 1;
	    count++;
	  }
	}
      }
    }

    if( ssid_flag == TRUE && (key_str_flag == TRUE || key_bin_flag == TRUE) && mode_flag == TRUE ) {
      ret_flag = TRUE;
      break;
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
  
  if( FS_CloseFile(&f) == FALSE) {
    res = FS_GetArchiveResultCode(path);
    return FALSE;
  }

  return ret_flag;
}



