#ifndef _MPRINT_
#define _MPRINT_

#define NUM_OF_SCREEN 4

extern TEXT_CTRL *tc[NUM_OF_SCREEN];

#ifdef __cplusplus
extern "C" {
#endif

void mfprintf(TEXT_CTRL *tc, const char *fmt, ...);
void mprintf(const char *fmt, ...);

#ifdef __cplusplus
}
#endif


#endif /* _MPRINT_ */
