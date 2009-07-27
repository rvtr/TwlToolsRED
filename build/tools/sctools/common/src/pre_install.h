#ifndef	_PRE_INSTALL_H_
#define	_PRE_INSTALL_H_


typedef struct _PRE_INSTALL_FILE {
  struct _PRE_INSTALL_FILE *next;
  u64 tid;
  u8 region;
  u8 country;
  char file_name[FS_FILE_NAME_MAX];
} PRE_INSTALL_FILE;


#ifdef __cplusplus
extern "C" {
#endif

BOOL pre_install_Cleanup_User_Titles( FSFile *log_fd );
BOOL pre_install_process( FSFile *log_fd, MY_USER_APP_TID *title_id_buf_ptr, int tile_id_count,
			  u64 *ticket_id_array,  int ticket_id_count ,BOOL development_version_flag );

BOOL pre_install_check_download_or_pre_install(u64 tid, int *flag, FSFile *log_fd);
BOOL pre_install_get_version(u64 tid, u16 *version);

BOOL pre_install_debug(FSFile *log_fd, BOOL development_version_flag );
BOOL pre_install_command(FSFile *log_fd, u64 *tid_array,  int tid_count, int command, BOOL development_version_flag );

#ifdef __cplusplus
}
#endif


#endif	/* _PRE_INSTALL_H_ */
