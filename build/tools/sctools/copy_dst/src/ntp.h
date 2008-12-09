#ifndef	_MY_NTP_H_
#define	_MY_NTP_H_

#ifdef __cplusplus
extern "C" {
#endif


BOOL my_ntp_init(void);

int my_ntp_get_year(void);
int my_ntp_get_month(void);
int my_ntp_get_day(void);
int my_ntp_get_weekday(void);
int my_ntp_get_hour(void);
int my_ntp_get_min(void);
int my_ntp_get_sec(void);



#ifdef __cplusplus
}
#endif

#endif /* _MY_NTP_H_ */
