#ifndef _MY_RSA_SIGN_H_
#define _MY_RSA_SIGN_H_

#ifdef __cplusplus
extern "C" {
#endif

#define MY_RSA_SIGN_HASH_SIZE 32
#define MY_RSA_SIGN_RSA_SIZE 128



#define MY_RSA_SIGN_SUCCESS                  0
#define MY_RSA_SIGN_ERROR_RSA_INIT_FAILED    1
#define MY_RSA_SIGN_ERROR_RSA_HASH_DECRYPT_FAILED 2
#define MY_RSA_SIGN_ERROR_HASH_CHECK_FAILED  3
#define MY_RSA_SIGN_ERROR_RSA_CONTENT_DECRYPT_FAILED 4


int my_rsa_sign(  u8 *inbuf, u8 *outbuf, int buf_size,  int *output_size );


BOOL RsaTestInit(void);
int RsaTestDecrypt(char *input, int in_len, char *output, int outlen);

#ifdef __cplusplus
}
#endif


#endif /* _MY_RSA_SIGN_H_ */
