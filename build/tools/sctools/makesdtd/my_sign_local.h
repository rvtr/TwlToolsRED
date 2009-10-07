#ifndef _MY_SIGN_LOCAL_H_
#define _MY_SIGN_LOCAL_H_


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
  u32 org_file_size;
  u32 num_of_block;
  u32 file_offset_L2_sign_table;
  u32 file_offset_data_block;
  u32 dummy[3];
  MY_SIGN_SIGNATURE L2_sign;
} MY_SIGN_HEADER;

#define AES_KEY_BIT_LEN 256
#define AES_KEY_BYTE_LEN (AES_KEY_BIT_LEN/8)


#ifdef __cplusplus
}
#endif

#endif /* _MY_SIGN_LOCAL_H_ */
