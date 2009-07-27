#ifndef _MY_SIGN_H_
#define _MY_SIGN_H_



// #define MY_RSA_SIGN_HASH_SIZE 32
// #define MY_RSA_SIGN_RSA_SIZE 128
#define MY_SIGN_RSA_SIZE 128
#define MY_SIGN_HASH_SIZE 0x20
#define MY_SIGN_BLOCK_SIZE (32*1024)


#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  u8 pad[16];
  u8 hash[MY_SIGN_HASH_SIZE];
  u8 dummy[MY_SIGN_RSA_SIZE - MY_SIGN_HASH_SIZE - 16];
} MY_SIGN_SIGNATURE;

typedef struct {
  u32 magic_code;
  s32 org_file_size;
  u32 num_of_block;
  u32 file_offset_L2_sign_table;
  u32 file_offset_data_block;
  u32 dummy[3];
  MY_SIGN_SIGNATURE L2_sign;
} MY_SIGN_HEADER;


typedef struct {
  FSFile f;
  BOOL open_flag;
  int pos;  /* original file pos */
  MY_SIGN_SIGNATURE *L2_sign_table;
  MY_SIGN_HEADER header;
  u8 *block_buf_in;
  u8 *block_buf_out;
} MY_SIGN_File;

BOOL my_sign_FS_OpenFile(MY_SIGN_File *msfile, char *path);
int my_sign_FS_ReadFile(MY_SIGN_File *msfile, u8 *buf, int buf_size);
BOOL my_sign_FS_CloseFile(MY_SIGN_File *msfile);
u32 my_sign_FS_GetLength(MY_SIGN_File *msfile);
BOOL my_sign_FS_SeekFileToBegin(MY_SIGN_File *msfile);
BOOL my_sign_FS_SeekFile(MY_SIGN_File *msfile, int offset, int whence);
void my_sign_FS_InitFile(MY_SIGN_File *msfile);

#ifdef __cplusplus
}
#endif

#endif /* _MY_SIGN_H_ */
