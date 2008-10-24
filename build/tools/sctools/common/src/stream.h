#ifndef _STREAM_H_
#define _STREAM_H_

#ifdef	__cplusplus
extern "C" {
#endif


extern void channel_play(void);
extern void stream_main(void);
extern void stream_play0(void);
extern void stream_play1(void);
extern void stream_play2(void);
extern void stream_play3(void);
extern BOOL stream_play_is_end(void);

#ifdef	__cplusplus
}          /* extern "C" */
#endif

#endif  /* _STREAM_H_ */
