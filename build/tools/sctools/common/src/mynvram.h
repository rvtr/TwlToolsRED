#ifndef	_MY_NVRAM_H_
#define	_MY_NVRAM_H_

#ifdef __cplusplus
extern "C" {
#endif

// BOOL my_nvram_read( u32 offset, u32 size, void *buf);
BOOL nvram_backup(char *path);
BOOL nvram_restore(char *path);
BOOL nvram_get(u8 buf[0xa00]);
BOOL nvram_set(u8 buf[0xa00]);

#ifdef __cplusplus
}
#endif

#endif /* _MY_NVRAM_H_ */
