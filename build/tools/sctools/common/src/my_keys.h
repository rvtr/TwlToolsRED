#ifndef _MY_KEYS_H_
#define _MY_KEYS_H_

#ifdef __cplusplus
extern "C" {
#endif


#define AES_KEY_BIT_LEN 256
#define AES_KEY_BYTE_LEN (AES_KEY_BIT_LEN/8)


extern u8 my_sign_aes_key[AES_KEY_BYTE_LEN];

extern u8 my_sign_aes_iv[AES_BLOCK_SIZE];

extern const unsigned char rsa_key_pub[0x8c];


#ifdef __cplusplus
}
#endif

#endif /* _MY_KEYS_H_ */
