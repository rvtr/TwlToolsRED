#include <twl.h>
#include <nitroWiFi.h>

#include        "ntp.h"

#include        "font.h"
#include        "text.h"
#include        "mprintf.h"
#include        "logprintf.h"

#include        "mywlan.h"


#define BUFSIZE		256		/* �o�b�t�@�T�C�Y */
#define RECVSIZE	4096	/* ��M�o�b�t�@�T�C�Y */
#define TIMEOUT		2		/* �^�C���A�E�g�b�� */

struct NTP_Packet{			/* NTP�p�P�b�g */
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



static char svName[BUFSIZE];			/* �T�[�o�̃h���C���� */
static unsigned short port = 123;		/* �T�[�o�̃|�[�g�ԍ� */
static unsigned short local_port = 1024;	/* ��M����|�[�g�ԍ� */
static int soc;					/* �\�P�b�g�iSoket Descriptor�j */
static unsigned long serveraddr;		/* �T�[�o��IP�A�h���X */

//static struct hostent *serverhostent;		/* �T�[�o�̏����w���|�C���^ */
static SOCHostEnt *serverhostent;		/* �T�[�o�̏����w���|�C���^ */

//static struct  sockaddr_in     sockname;	/* �\�P�b�g�̃A�h���X */

static SOCSockAddrIn sockname;

// static struct  sockaddr_in     serversockaddr;	/* �T�[�o�̃A�h���X */

static SOCSockAddrIn serversockaddr;	/* �T�[�o�̃A�h���X */

static int sockaddr_Size;			/* �T�[�o�̃A�h���X�̃T�C�Y */
static struct NTP_Packet	NTP_Send;	/* ���M����NTP�p�P�b�g */
static struct NTP_Packet	NTP_Recv;	/* ��M����NTP�p�P�b�g */

static u32 ntp_time;	/* NTP�T�[�o����擾�������� */

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


  /* �\�P�b�g���쐬���鏈�� */
  /* UDP���[�h��soc�Ƀ\�P�b�g���쐬���܂� */
  soc = SOC_Socket(SOC_PF_INET, SOC_SOCK_DGRAM, 0);
  if(soc < 0 ){
    OS_TPrintf("Error: create Socket\n");
    return FALSE;
  }


  /* ��M�|�[�g���w�肷�鏈�� */
  /* �\�P�b�g�̃A�h���X�̍\���̂ɃT�[�o��IP�A�h���X�ƃ|�[�g�ԍ���ݒ肵�܂� */
  // SOSockAddrIn;
  // int SOC_Bind(int s, const void* sockAddr);
  // SOC_HtoNs(hostshort)   

  sockname.family	   = SOC_AF_INET;					/* �C���^�[�l�b�g�̏ꍇ */
  sockname.addr.addr  = SOC_INADDR_ANY;				/* ������IP�A�h���X���g���悤�ɂ��� */
  sockname.port	   = SOC_HtoNs((unsigned short)local_port);		/* ��M����|�[�g�ԍ� */
  //  STD_MemSet((void *)sockname.zero,(int)0,sizeof(sockname.zero));
  if(SOC_Bind(soc, (void *)&sockname) < 0 ){
    OS_TPrintf("Error: specify recv port\n");
    return FALSE;
  }


  /* �T�[�o��IP�A�h���X���擾���鏈�� */
  /* svName�Ƀh�b�g�ŋ�؂���10�i����IP�A�h���X�������Ă���ꍇ�Aserveraddr��32bit������IP�A�h���X���Ԃ�܂� */
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

  /* �T�[�o��(svName)����T�[�o�̃z�X�g�����擾���܂� */
  serverhostent = SOC_GetHostByName(svName);
  if(serverhostent == NULL) {
    OS_TPrintf("Error: SOC_GetHostByName %s\n",svName);
    /* �\�P�b�g��j������ */
    (void)SOC_Close(soc);
    return FALSE;
  } 
  else{
    /* �T�[�o�̃z�X�g��񂩂�IP�A�h���X��serveraddr�ɃR�s�[���܂� */
    serveraddr = *((unsigned long *)((serverhostent->addrList)[0]));
  }
  
#if 1
  OS_TPrintf("serveraddr = %d.%d.%d.%d\n",
	     ((serveraddr >> 0) & 0xff),
	     ((serveraddr >> 8) & 0xff),
	     ((serveraddr >> 16) & 0xff),
	     ((serveraddr >> 24) & 0xff) );
#endif


  /* �T�[�o�̃A�h���X�̍\���̂ɃT�[�o��IP�A�h���X�ƃ|�[�g�ԍ���ݒ肵�܂� */
  serversockaddr.family	 = SOC_AF_INET;				/* �C���^�[�l�b�g�̏ꍇ */
  serversockaddr.addr.addr  = serveraddr;				/* �T�[�o��IP�A�h���X */
  serversockaddr.port	 = SOC_HtoNs((unsigned short)port);		/* �|�[�g�ԍ� */

  /* �T�[�o�Ƀp�P�b�g�𑗐M���鏈�� */
  /* NTP�p�P�b�g��SNTP�p�ɏ��������� */
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

  /* �T�[�o���w�肵��NTP�p�P�b�g�𑗐M���� */
  if(SOC_SendTo(soc,(const void *)&NTP_Send, sizeof( NTP_Send ),0,(const void *)&serversockaddr) < 0 ) {
    OS_TPrintf("Error: �T�[�o�ւ̑��M���s\n");
    /* �\�P�b�g��j������ */
    (void)SOC_Close(soc);
    return FALSE;
  }


  /* �^�C���A�E�g���s������ */
  /* select�֐����g���ă^�C���A�E�g��ݒ肷�� */

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
    /* �\�P�b�g��j������ */
    (void)SOC_Close(soc);
    return FALSE;
  }
  switch( pfds[0].revents ) {
  case SOC_POLLERR: // �\�P�b�g�ɃG���[���������܂����B 
    OS_TPrintf("Error: SOC_POLLERR  %s %d\n",__FUNCTION__,__LINE__);
    break;
  case SOC_POLLHUP: // �X�g���[���E�\�P�b�g�����ڑ��ł��B 
    OS_TPrintf("Error: SOC_POLLHUP  %s %d\n",__FUNCTION__,__LINE__);
    break;
  case SOC_POLLNVAL: // �s���ȃ\�P�b�g�L�q�q�ł��B
    OS_TPrintf("Error: SOC_POLLNVAL %s %d\n",__FUNCTION__,__LINE__);
    break;
  default:
    break;
  }


  /* �T�[�o���玞��������M���鏈�� */
  /* �T�[�o���w�肵�Ď�M���s�� */
  // int SOC_RecvFrom(int s, void* buf, int len, int flags, void* sockFrom);
  sockaddr_Size = sizeof(serversockaddr);
  if(SOC_RecvFrom(soc, (char *)&NTP_Recv, sizeof(NTP_Recv), 0 ,(void *)&serversockaddr) < 0 ){
    OS_TPrintf("Error: �T�[�o����̎�M���s\n");
    /* �\�P�b�g��j������ */
    (void)SOC_Close(soc);
    return FALSE;
  }


  /* NTP�T�[�o����擾�������������n���Ԃɕϊ����� */
  // ntp_time = ntohl(NTP_Recv.transmit_timestamp_seconds) - 2208988800; /* 1970/01/01 ����̕b���ɕϊ� */
  ntp_time = SOC_NtoHl(NTP_Recv.transmit_timestamp_seconds) - 2208988800; /* 1970/01/01 ����̕b���ɕϊ� */

 OS_TPrintf("ntp_time = %d\n",ntp_time);

 OS_TPrintf("TimeZone %d\n", GetTimeZone());

 ntp_time += (60*GetTimeZone());

 rtc_time_to_tm(ntp_time, &tm);
 
 OS_TPrintf("Year %d\n", tm.tm_year+1900);
 OS_TPrintf("Mon %d\n", tm.tm_mon + 1); /* 0 -> 1�� */
 OS_TPrintf("Day %d\n", tm.tm_mday);
 OS_TPrintf("Week %d\n", tm.tm_wday);
 OS_TPrintf("Hour %d\n", tm.tm_hour);
 OS_TPrintf("Min %d\n", tm.tm_min);
 OS_TPrintf("Sec %d\n", tm.tm_sec);

 //   tm.tm_yday;
 //   tm.tm_isdst;

  /* �\�P�b�g��j�����鏈�� */
  (void)SOC_Close(soc);
  return TRUE;
}
