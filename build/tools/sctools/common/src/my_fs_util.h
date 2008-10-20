#ifndef	_MY_FS_UTIL_H_
#define	_MY_FS_UTIL_H_

#define FILE_PATH_LEN 512

typedef struct _MY_DIR_ENTRY_LIST {
  struct _MY_DIR_ENTRY_LIST *prev;
  struct _MY_DIR_ENTRY_LIST *next;
  FSDirectoryEntryInfo content;
  char src_path[ FILE_PATH_LEN ];
  char dst_path[ FILE_PATH_LEN ];
} MY_DIR_ENTRY_LIST;


#ifdef __cplusplus
extern "C" {
#endif

char *my_fs_util_get_fs_result_word( FSResult res );

int find_title_save_data(MY_DIR_ENTRY_LIST **headp, const char *path_dst, 
			 const char *path_src, int *save_dir_info, char *log_file_name , int level);
int find_copy( MY_DIR_ENTRY_LIST **headp, const char *path_dst, const char *path_src,
	       char *extension, int max_level, int *save_info, char *log_file_name , int level);
int copy_r( MY_DIR_ENTRY_LIST **headp, const char *path_dst, const char *path_src, char *log_file_name, int level );
int get_title_id(MY_DIR_ENTRY_LIST **headp, const char *path_src, 
		 int *save_parent_dir_info_flag, char *log_file_name, int level );

void PrintDirEntryListForward( MY_DIR_ENTRY_LIST *head, FSFile *log_fd );
void PrintDirEntryListBackward( MY_DIR_ENTRY_LIST *head, FSFile *log_fd );
void PrintSrcDirEntryListBackward( MY_DIR_ENTRY_LIST *head, FSFile *log_fd);

int SaveDirEntryList( MY_DIR_ENTRY_LIST *head , char *path );
BOOL RestoreDirEntryList( char *path );
BOOL ClearDirEntryList( MY_DIR_ENTRY_LIST **headp );
void write_debug_data(void);
BOOL SDCardValidation(void);
BOOL CheckShopRecord(FSFile *log_fd);
BOOL MydataSave(const char *path, void *pData, int size, FSFile *log_fd);
BOOL MydataLoad(const char *path, void *pBuffer, int size, FSFile *log_fd);
void GetDirEntryList( MY_DIR_ENTRY_LIST *head, u64 **pBuffer, int *size);
BOOL TitleIDSave(const char *path, u64 *pData, int count, FSFile *log_fd);
BOOL TitleIDLoad(const char *path, u64 **pBuffer, int *count, FSFile *log_fd);

#ifdef __cplusplus
}
#endif


#endif /* _MY_FS_UTIL_H_ */
