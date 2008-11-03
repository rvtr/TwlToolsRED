#ifndef _LOGPRINT_
#define _LOGPRINT_

#ifdef __cplusplus
extern "C" {
#endif

void miya_log_fprintf(FSFile *fd, const char *fmt, ...);

#ifdef __cplusplus
}
#endif


#endif /* _LOGPRINT_ */
