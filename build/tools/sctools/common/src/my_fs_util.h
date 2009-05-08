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


typedef struct {
  u64 tid;
  int is_personalized;
  BOOL install_success_flag;
} MY_USER_APP_TID;



BOOL my_fs_Tid_To_GameCode(u64 tid, char *gcode);

char *my_fs_util_get_fs_result_word( FSResult res );
s32 my_fs_crypto_write(FSFile *f, void *ptr, s32 size);
s32 my_fs_crypto_read(FSFile *f, void *ptr, s32 size);

int find_title_save_data(MY_DIR_ENTRY_LIST **headp, const char *path_dst, 
			 const char *path_src, int *save_dir_info, char *log_file_name , int level);
int copy_r( MY_DIR_ENTRY_LIST **headp, const char *path_dst, const char *path_src, char *log_file_name, int level );
int get_title_id(MY_DIR_ENTRY_LIST **headp, const char *path_src, 
		 int *save_parent_dir_info_flag, char *log_file_name, int level );

void PrintDirEntryListForward( MY_DIR_ENTRY_LIST *head, FSFile *log_fd );
void PrintDirEntryListBackward( MY_DIR_ENTRY_LIST *head, FSFile *log_fd );
void PrintSrcDirEntryListBackward( MY_DIR_ENTRY_LIST *head, FSFile *log_fd);

BOOL SaveDirEntryList( MY_DIR_ENTRY_LIST *head , char *path, int *list_count, int *error_count, char *log_file_name);

BOOL RestoreDirEntryList( char *path , char *log_file_name, int *list_count, int *error_count);
BOOL RestoreDirEntryList_System_And_InstallSuccessApp(char *path , char *log_file_name, int *list_count, int *error_count,
							MY_USER_APP_TID *title_id_buf, int title_id_count );


  //BOOL GetUserAppTitleList( MY_DIR_ENTRY_LIST *head, u64 **pBuffer, int *size, char *log_file_name);
BOOL GetUserAppTitleList( MY_DIR_ENTRY_LIST *head, MY_USER_APP_TID **pBuffer, int *size, char *log_file_name);

BOOL ClearDirEntryList( MY_DIR_ENTRY_LIST **headp );

void write_debug_data(void);

BOOL SDCardValidation(void);
BOOL TWLCardValidation(void);
BOOL CheckShopRecord(FSFile *log_fd);

BOOL CleanSDCardFiles(char *log_file_name);

  //BOOL MydataSave(const char *path, void *pData, int size, FSFile *log_fd);
  //BOOL MydataLoad(const char *path, void *pBuffer, int size, FSFile *log_fd);

BOOL MydataLoadDecrypt(const char *path, void *pBuffer, int size, FSFile *log_fd);
BOOL MydataSaveEncrypt(const char *path, void *pData, int size, FSFile *log_fd);


// BOOL TitleIDSave(const char *path, u64 *pData, int count, char *log_file_name);
// BOOL TitleIDLoad(const char *path, u64 **pBuffer, int *count, char *log_file_name);
BOOL TitleIDSave(const char *path, MY_USER_APP_TID *pData, int count, char *log_file_name);
BOOL TitleIDLoad(const char *path, MY_USER_APP_TID **pBuffer, int *count, char *log_file_name);

BOOL CopyFile(const char *dst_path, const char *src_path, FSFile *log_fd );

void Log_File_Close(FSFile *log_fd);
BOOL Log_File_Open(FSFile *log_fd, const char *log_file_name);


void Miya_debug_OFF(void);
void Miya_debug_ON(void);
void my_fs_print_debug_ON(void);
void my_fs_print_debug_OFF(void);
BOOL my_fs_get_print_debug_flag(void);

#ifdef __cplusplus
}
#endif


#endif /* _MY_FS_UTIL_H_ */
