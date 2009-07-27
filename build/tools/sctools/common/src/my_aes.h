#ifndef _MY_AES_H_
#define _MY_AES_H_

#ifdef __cplusplus
extern "C" {
#endif


#define AES_ENCRYPT	1
#define AES_DECRYPT	0

/* Because array size can't be a const in C, the following two are macros.
   Both sizes are in bytes. */
#define AES_MAXNR 14
#define AES_BLOCK_SIZE 16


typedef struct {
    unsigned long rd_key[4 *(AES_MAXNR + 1)];
    int rounds;
} AES_KEY;


int AES_set_encrypt_key(const unsigned char *userKey, const int bits,
	AES_KEY *key);
int AES_set_decrypt_key(const unsigned char *userKey, const int bits,
	AES_KEY *key);

void AES_encrypt(const unsigned char *in, unsigned char *out,
	const AES_KEY *key);

void AES_decrypt(const unsigned char *in, unsigned char *out,
	const AES_KEY *key);

void AES_cbc_encrypt(const unsigned char *in, unsigned char *out,
	size_t length, const AES_KEY *key,
	unsigned char *ivec, const int enc);

#ifdef __cplusplus
}
#endif


#endif /* _MY_AES_H_ */
