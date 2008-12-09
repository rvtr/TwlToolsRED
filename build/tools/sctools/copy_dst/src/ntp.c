#include <twl.h>
#include <nitroWiFi.h>

#include        "ntp.h"

#include        "font.h"
#include        "text.h"
#include        "mprintf.h"
#include        "logprintf.h"

#include        "mywlan.h"


#define BUFSIZE		256		/* バッファサイズ */
#define RECVSIZE	4096	/* 受信バッファサイズ */
#define TIMEOUT		2		/* タイムアウト秒数 */

struct NTP_Packet{			/* NTPパケット */
	int Control_Word;
	int root_delay;
	int root_dispersion;
	int reference_identifier;
	s64 reference_timestamp;
	s64 originate_timestamp;
	s64 receive_timestamp;
	int transmit_timestamp_seconds;
	int transmit_timestamp_fractions;
};

struct rtc_time {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};

#define LEAPS_THRU_END_OF(y) ((y)/4 - (y)/100 + (y)/400)
#define LEAP_YEAR(year) ((!(year % 4) && (year % 100)) || !(year % 400))

static const unsigned char rtc_days_in_month[] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static int rtc_month_days(unsigned int month, unsigned int year)
{
	return rtc_days_in_month[month] + (LEAP_YEAR(year) && month == 1);
}

/* Convert seconds since 01-01-1970 00:00:00 to Gregorian date. */
static void rtc_time_to_tm(unsigned long time, struct rtc_time *tm)
{
  unsigned int month, year;
  int days;
  
  days = (int)(time / 86400 );
  time -= (unsigned int) days * 86400;
  
  /* day of the week, 1970-01-01 was a Thursday */
  tm->tm_wday = (days + 4) % 7;
  
  year = (unsigned int)(1970 + days / 365);
  days -= (year - 1970) * 365
    + LEAPS_THRU_END_OF(year - 1)
    - LEAPS_THRU_END_OF(1970 - 1);
  if (days < 0) {
    year -= 1;
    days += 365 + LEAP_YEAR(year);
  }
  tm->tm_year = (int)(year - 1900);
  tm->tm_yday = days + 1;
  
  for (month = 0; month < 11; month++) {
    int newdays;
    
    newdays = days - rtc_month_days(month, year);
    if (newdays < 0)
      break;
    days = newdays;
  }
  tm->tm_mon = (int)month;
  tm->tm_mday = days + 1;
  
  tm->tm_hour = (int)(time / 3600);
  time -= tm->tm_hour * 3600;
  tm->tm_min = (int)(time / 60);
  tm->tm_sec = (int)(time - tm->tm_min * 60);
}



static char svName[BUFSIZE];			/* サーバのドメイン名 */
static unsigned short port = 123;		/* サーバのポート番号 */
static unsigned short local_port = 1024;	/* 受信するポート番号 */
static int soc;					/* ソケット（Soket Descriptor） */
static unsigned long serveraddr;		/* サーバのIPアドレス */

//static struct hostent *serverhostent;		/* サーバの情報を指すポインタ */
static SOCHostEnt *serverhostent;		/* サーバの情報を指すポインタ */

//static struct  sockaddr_in     sockname;	/* ソケットのアドレス */

static SOCSockAddrIn sockname;

// static struct  sockaddr_in     serversockaddr;	/* サーバのアドレス */

static SOCSockAddrIn serversockaddr;	/* サーバのアドレス */

static int sockaddr_Size;			/* サーバのアドレスのサイズ */
static struct NTP_Packet	NTP_Send;	/* 送信するNTPパケット */
static struct NTP_Packet	NTP_Recv;	/* 受信するNTPパケット */

static u32 ntp_time;	/* NTPサーバから取得した時刻 */

static struct rtc_time tm;


int my_ntp_get_year(void)
{
  return tm.tm_year+1900;
}

int my_ntp_get_month(void)
{
  return tm.tm_mon;
}

int my_ntp_get_day(void)
{
  return tm.tm_mday;
}

int my_ntp_get_weekday(void)
{
  return tm.tm_wday;
}

int my_ntp_get_hour(void)
{
  return tm.tm_hour;
}

int my_ntp_get_min(void)
{
  return tm.tm_min;
}


int my_ntp_get_sec(void)
{
  return tm.tm_sec;
}


BOOL my_ntp_init(void)
{
  SOCPollFD pfds[1];  

  if( GetNTPSRV() == NULL ) {
    return FALSE;
  }

  OS_TPrintf("NTP srv %s\n",GetNTPSRV() );
  

  //  STD_StrCpy(svName, "ntp.jst.mfeed.ad.jp" );
  STD_StrCpy(svName, GetNTPSRV() );
  //Name:    ntp.jst.mfeed.ad.jp
  //Addresses:  210.173.160.57, 210.173.160.87, 210.173.160.27


  /* ソケットを作成する処理 */
  /* UDPモードでsocにソケットを作成します */
  soc = SOC_Socket(SOC_PF_INET, SOC_SOCK_DGRAM, 0);
  if(soc < 0 ){
    OS_TPrintf("Error: create Socket\n");
    return FALSE;
  }


  /* 受信ポートを指定する処理 */
  /* ソケットのアドレスの構造体にサーバのIPアドレスとポート番号を設定します */
  // SOSockAddrIn;
  // int SOC_Bind(int s, const void* sockAddr);
  // SOC_HtoNs(hostshort)   

  sockname.family	   = SOC_AF_INET;					/* インターネットの場合 */
  sockname.addr.addr  = SOC_INADDR_ANY;				/* 自分のIPアドレスを使うようにする */
  sockname.port	   = SOC_HtoNs((unsigned short)local_port);		/* 受信するポート番号 */
  //  STD_MemSet((void *)sockname.zero,(int)0,sizeof(sockname.zero));
  if(SOC_Bind(soc, (void *)&sockname) < 0 ){
    OS_TPrintf("Error: specify recv port\n");
    return FALSE;
  }


  /* サーバのIPアドレスを取得する処理 */
  /* svNameにドットで区切った10進数のIPアドレスが入っている場合、serveraddrに32bit整数のIPアドレスが返ります */
  // SOCHostEnt*     SOC_GetHostByName(const char* name);
  // typedef struct SOCHostEnt
  // {
  //     char*   name;                   // official name of host
  //     char **  aliases;               // alias list (zero-terminated)
  //     s16     addrType;               // always SO_PF_INET
  //     s16     length;                 // length of address
  //     u8 **    addrList;              // list of addresses
  // } SOCHostEnt;
  // 
  // int     SOC_InetAtoN(const char* cp, SOCInAddr* inp);
  //
  // typedef struct SOCInAddr
  // {
  //    u32 addr;
  // } SOCInAddr;
  //
  // int SOC_Close(int s);

  //  serveraddr = (210) | (173 << 8) | (160 << 16) | (57 << 24);

  /* サーバ名(svName)からサーバのホスト情報を取得します */
  serverhostent = SOC_GetHostByName(svName);
  if(serverhostent == NULL) {
    OS_TPrintf("Error: SOC_GetHostByName %s\n",svName);
    /* ソケットを破棄する */
    (void)SOC_Close(soc);
    return FALSE;
  } 
  else{
    /* サーバのホスト情報からIPアドレスをserveraddrにコピーします */
    serveraddr = *((unsigned long *)((serverhostent->addrList)[0]));
  }
  
#if 1
  OS_TPrintf("serveraddr = %d.%d.%d.%d\n",
	     ((serveraddr >> 0) & 0xff),
	     ((serveraddr >> 8) & 0xff),
	     ((serveraddr >> 16) & 0xff),
	     ((serveraddr >> 24) & 0xff) );
#endif


  /* サーバのアドレスの構造体にサーバのIPアドレスとポート番号を設定します */
  serversockaddr.family	 = SOC_AF_INET;				/* インターネットの場合 */
  serversockaddr.addr.addr  = serveraddr;				/* サーバのIPアドレス */
  serversockaddr.port	 = SOC_HtoNs((unsigned short)port);		/* ポート番号 */

  /* サーバにパケットを送信する処理 */
  /* NTPパケットをSNTP用に初期化する */
  //  NTP_Send.Control_Word = htonl(0x0B000000);
  NTP_Send.Control_Word = SOC_HtoNl(0x0B000000);

  NTP_Send.root_delay = 0;
  NTP_Send.root_dispersion = 0;
  NTP_Send.reference_identifier = 0;
  NTP_Send.reference_timestamp = 0;
  NTP_Send.originate_timestamp = 0;
  NTP_Send.receive_timestamp = 0;
  NTP_Send.transmit_timestamp_seconds = 0;
  NTP_Send.transmit_timestamp_fractions = 0;



  // int SOC_SendTo(int s, const void* buf, int len, int flags, const void* sockTo)

  /* サーバを指定してNTPパケットを送信する */
  if(SOC_SendTo(soc,(const void *)&NTP_Send, sizeof( NTP_Send ),0,(const void *)&serversockaddr) < 0 ) {
    OS_TPrintf("Error: サーバへの送信失敗\n");
    /* ソケットを破棄する */
    (void)SOC_Close(soc);
    return FALSE;
  }


  /* タイムアウトを行う処理 */
  /* select関数を使ってタイムアウトを設定する */

  //
  // typedef struct SOCPollFD
  // {
  //   int     fd;
  //     short   events;                 // input event flags
  //     short   revents;                // output event flags
  // } SOCPollFD;
  // 
  // int SOC_Poll(SOCPollFD fds[], unsigned nfds, OSTick timeout);
  pfds[0].fd = soc;
  //    pfds[0].events = SOC_POLLRDNORM | SOC_POLLWRNORM;
  pfds[0].events = SOC_POLLRDNORM;
  if( SOC_Poll( pfds, 1, OS_MilliSecondsToTicks( TIMEOUT * 1000 ) ) < 0 ) {
    OS_TPrintf("Error: recv error\n");
    /* ソケットを破棄する */
    (void)SOC_Close(soc);
    return FALSE;
  }
  switch( pfds[0].revents ) {
  case SOC_POLLERR: // ソケットにエラーが発生しました。 
    OS_TPrintf("Error: SOC_POLLERR  %s %d\n",__FUNCTION__,__LINE__);
    break;
  case SOC_POLLHUP: // ストリーム・ソケットが未接続です。 
    OS_TPrintf("Error: SOC_POLLHUP  %s %d\n",__FUNCTION__,__LINE__);
    break;
  case SOC_POLLNVAL: // 不正なソケット記述子です。
    OS_TPrintf("Error: SOC_POLLNVAL %s %d\n",__FUNCTION__,__LINE__);
    break;
  default:
    break;
  }


  /* サーバから時刻情報を受信する処理 */
  /* サーバを指定して受信を行う */
  // int SOC_RecvFrom(int s, void* buf, int len, int flags, void* sockFrom);
  sockaddr_Size = sizeof(serversockaddr);
  if(SOC_RecvFrom(soc, (char *)&NTP_Recv, sizeof(NTP_Recv), 0 ,(void *)&serversockaddr) < 0 ){
    OS_TPrintf("Error: サーバからの受信失敗\n");
    /* ソケットを破棄する */
    (void)SOC_Close(soc);
    return FALSE;
  }


  /* NTPサーバから取得した時刻を現地時間に変換する */
  // ntp_time = ntohl(NTP_Recv.transmit_timestamp_seconds) - 2208988800; /* 1970/01/01 からの秒数に変換 */
  ntp_time = SOC_NtoHl(NTP_Recv.transmit_timestamp_seconds) - 2208988800; /* 1970/01/01 からの秒数に変換 */

 OS_TPrintf("ntp_time = %d\n",ntp_time);

 OS_TPrintf("TimeZone %d\n", GetTimeZone());

 ntp_time += (60*GetTimeZone());

 rtc_time_to_tm(ntp_time, &tm);
 
 OS_TPrintf("Year %d\n", tm.tm_year+1900);
 OS_TPrintf("Mon %d\n", tm.tm_mon + 1); /* 0 -> 1月 */
 OS_TPrintf("Day %d\n", tm.tm_mday);
 OS_TPrintf("Week %d\n", tm.tm_wday);
 OS_TPrintf("Hour %d\n", tm.tm_hour);
 OS_TPrintf("Min %d\n", tm.tm_min);
 OS_TPrintf("Sec %d\n", tm.tm_sec);

 //   tm.tm_yday;
 //   tm.tm_isdst;

  /* ソケットを破棄する処理 */
  (void)SOC_Close(soc);
  return TRUE;
}
