/* cryptopc *.der infile outfile
   cryptopc miya1024.der main.c main.enc
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <unistd.h> 

#include <openssl/err.h>

#include <openssl/rsa.h>
#include <openssl/aes.h>
#include <openssl/pem.h>
#include <openssl/sha.h>

typedef signed char  s8;
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;
typedef unsigned long long u64;



#include "my_sign.h"
#include "my_sign_local.h"
/* 
   データのハッシュの単位は３２ＫＢ

   ・ヘッダー
   ・ヘッダーの署名（ハッシュは３２バイトだがRSAが１２８バイトなんで１２８バイト）
   ・

   オリジナルのファイルサイズ / 32KB -> ハッシュの数。
   num_of_block = org_file_size / 32KB;
   if( org_file_size % 32KB ) {
      num_of_block += 1
   }
   
   L2ハッシュサイズ * num_of_block -> L2ハッシュブロックサイズ
   L2_hash_block_size = 32B * num_of_block;


*/


/* 
   flen must be less than RSA_size(rsa) - 11 for the PKCS #1 v1.5 based
   padding modes, and less than RSA_size(rsa) - 41 for
   RSA_PKCS1_OAEP_PADDING. The random number generator must be seeded
   prior to calling RSA_public_encrypt().
 */


/* 
   FILE format:
   -----
   MY_SIGN_HEADER header;
   MY_SIGN_SIGNATURE header_sign;
   L2_sign[0];
   L2_sign[1];
   L2_sign[2];
   L2_sign[ ];
      .
      .
   enc_data_block[0](32KB)
   enc_data_block[1](32KB)
   enc_data_block[2](32KB)
      .
      .
 */

static FILE *fp_key = NULL;
static int rsaSize = 0;
static RSA *rsa_key = NULL;


/* 
#ifdef CHINA_LIMITED
#endif CHINA_LIMITED

common/src/my_keys.cと対

*/
static u8 my_sign_aes_key[AES_KEY_BYTE_LEN] = {
  0x02,0xB6,0x01,0xD8,0x01,0x80,0x01,0x77,0xB4,0x01,0xCB,0x01,0xBD,0x5F,0x18,0x0F,
  0xF6,0x39,0x9C,0xC6,0x90,0xAC,0xC1,0x0D,0x03,0x74,0x6E,0x8D,0xD1,0xBA,0x37,0x46
};

static u8 my_sign_aes_iv[AES_BLOCK_SIZE] = {
  0xC3,0x85,0x93,0xFE,0xA8,0x2D,0xBF,0xFB,0xED,0x42,0xE0,0x42,0xFD,0x17,0x04,0xB0
};


static int my_sign_make(MY_SIGN_SIGNATURE *encrypted_sign, u8 *buf, int buf_size)
{
  MY_SIGN_SIGNATURE temp_sign;
  int outlen = 0;
  // int i;
  if( encrypted_sign == NULL ) {
    return -1;
  }
  if( buf == NULL ) {
    return -1;
  }
  if( buf_size < 1 ) {
    return -1;
  }
  if( rsa_key == NULL ) {
    return -1;
  }
  if( rsaSize < 1 ) {
    return -1;
  }

  memset((u8 *)&temp_sign, 0 , sizeof(MY_SIGN_SIGNATURE));
  memset((u8 *)encrypted_sign, 0 , sizeof(MY_SIGN_SIGNATURE));

  //  printf( "%s buf_size = %d\n",__FUNCTION__, buf_size);
  SHA256(buf, buf_size, temp_sign.hash);
#if 0
   for( i = 0 ; i < 32 ; i++ ) {
    printf("hash[0x%02x]=0x%02x\n",i,temp_sign.hash[i]);
  }
#endif
  temp_sign.pad[0] = 1;
  temp_sign.pad[1] = 1;
  temp_sign.pad[2] = 1;
  temp_sign.pad[3] = 1;

      //RSA_PKCS1_OAEP_PADDING
      //RSA_NO_PADDING
#if 0
#define RSA_PKCS1_PADDING	1
#define RSA_SSLV23_PADDING	2
#define RSA_NO_PADDING		3
#define RSA_PKCS1_OAEP_PADDING	4
#define RSA_X931_PADDING	5

#define RSA_PKCS1_PADDING_SIZE	11
#endif

  if(rsaSize != (outlen = RSA_private_encrypt(rsaSize - RSA_PKCS1_PADDING_SIZE, 
					      (u8 *)&temp_sign, (u8 *)encrypted_sign,
					      rsa_key,  RSA_PKCS1_PADDING ))) {
    
    fprintf(stderr,"encrypt error rsaSize=%d outlen=%d\n",rsaSize, outlen);
    return -1;
  }

  return 0;
}

int cryptopc_init(char *key_file)
{
  ERR_load_crypto_strings(); 
  
  //  printf("hash offset = %d\n",offsetof(MY_SIGN_SIGNATURE, hash));
  if( (fp_key = fopen( key_file, "rb" )) == NULL ) {
    fprintf(stderr, "failed to fopen %s\n",key_file);
    return -1;
  }

  rsa_key = RSA_new();
      
  if( rsa_key == NULL ) {
    fprintf(stdout,"Error:RSA_new(key alloc) NULL!\n");
    return -1;
  }
  d2i_RSAPrivateKey_fp(fp_key, &rsa_key);
  // d2i_RSAPublicKey_fp(fp_key, &key);

  if( rsa_key != NULL ) {
    //	RSA_print_fp(stdout, key, 0);
    rsaSize = RSA_size( rsa_key );
    //    printf("rsaSize = %d bit\n",rsaSize * 8);
  }
  else {
    RSA_free(rsa_key);
    rsa_key = NULL;
    fprintf(stderr, "Error:d2i_RSAPrivateKey_fp(read key) NULL!\n");
    return -1;
  }
  return 0;
}

void cryptopc_end(void)
{
  if( fp_key ) {
    fclose(fp_key);
  }
  if( rsa_key ) {
    RSA_free(rsa_key);
  }
}

int cryptopc(char *input_file, char *output_file)
{
  FILE *fp_in = NULL;
  FILE *fp_out = NULL;

  struct stat st_buf;
  int readlen;
  int outlen = 0;
  int rsa_error_flag = 0;
  int i;
  MY_SIGN_HEADER header;
  MY_SIGN_SIGNATURE header_sign;
  MY_SIGN_SIGNATURE *L2_sign_table;
  MY_SIGN_SIGNATURE *L2_sign_table_temp;
  int L2_sign_table_size;
  int block_no;
  u8 block_buf_in[MY_SIGN_BLOCK_SIZE];
  u8 block_buf_out[MY_SIGN_BLOCK_SIZE];

  u8 aes_key_buf[AES_KEY_BYTE_LEN];
  AES_KEY aes_key;
  unsigned char aes_iv[ AES_BLOCK_SIZE ];


  if( 0 != stat( input_file, &st_buf) ) {
    fprintf(stderr, "failed to stat %s\n", input_file);
    rsa_error_flag = 1;
    goto end;
  }
    

  if( (fp_in = fopen( input_file, "rb" )) == NULL ) {
    fprintf(stderr, "Error:failed to fopen input file(%s)\n", input_file);
    rsa_error_flag = 1;
    goto end;
  }

  if( (fp_out = fopen( output_file, "wb" )) == NULL ) {
    fprintf(stderr, "failed to fopen output file(%s)\n",output_file);
    rsa_error_flag = 1;
    goto end;
  }
    
  /* 
     FILE format:
     -----
     MY_SIGN_HEADER header;
     MY_SIGN_SIGNATURE header_sign;
     L2_sign_table[0];
     L2_sign_table[1];
     L2_sign_table[2];
     L2_sign_table[ ];
     .
     .
     aes_enc_data_block[0](32KB)
     aes_enc_data_block[1](32KB)
     aes_enc_data_block[2](32KB)
     .
     .
     .
     .


     #define RSA_SIZE 128
     #define HASH_SIZE 0x20
     #define BLOCK_SIZE (32*1024)
       
     typedef struct {
     u8 hash[HASH_SIZE];
     u8 dummy[RSA_SIZE - HASH_SIZE];
     } SIGNATURE;
       
     typedef struct {
     u32 magic_code;
     u32 org_file_size;
     u32 num_of_block;
     u32 file_offset_L2_sign_table;
     u32 file_offset_data_block;
     SIGNATURE L2_sign;
     } HEADER;

  */

  memset(&header, 0 , sizeof(MY_SIGN_HEADER));
  header.magic_code = 0xdeadbeef;
  header.org_file_size = st_buf.st_size;
  header.num_of_block = st_buf.st_size / MY_SIGN_BLOCK_SIZE;
  if( st_buf.st_size % MY_SIGN_BLOCK_SIZE ) {
    header.num_of_block += 1;
  }

  header.file_offset_L2_sign_table = sizeof(MY_SIGN_HEADER) + sizeof(MY_SIGN_SIGNATURE);

  L2_sign_table_size = sizeof(MY_SIGN_SIGNATURE) * header.num_of_block;

  header.file_offset_data_block = header.file_offset_L2_sign_table + L2_sign_table_size;
   

  //  printf("header.org_file_size = %d\n",(int)header.org_file_size); 
  //  printf("header.num_of_block = %d\n",(int)header.num_of_block);


  L2_sign_table = (MY_SIGN_SIGNATURE *)malloc( L2_sign_table_size );
  if( L2_sign_table == NULL ) {
    fprintf(stderr,"L2_sign_table malloc error %s %d\n",__FUNCTION__,__LINE__);
    rsa_error_flag = 1;
    goto end;
  }
  memset(L2_sign_table, 0 , L2_sign_table_size );
  L2_sign_table_temp = L2_sign_table;



  if( 0 != fseek(fp_out,  header.file_offset_data_block , SEEK_SET) ) {
    fprintf(stderr,"fseek error %s %d\n",__FUNCTION__,__LINE__);
    rsa_error_flag = 1;
    goto end;
  }

  block_no = 0;

  while( (readlen = fread(block_buf_in, 1, MY_SIGN_BLOCK_SIZE, fp_in)) > 0 ) {
      
    if( readlen < MY_SIGN_BLOCK_SIZE ) {
      for( i = readlen ; i < MY_SIGN_BLOCK_SIZE ; i++ ) {
	block_buf_in[i] = 0;	/* padding.. */
      }
    }


    /* AESキーのセット */
    for( i = 0 ; i < AES_KEY_BYTE_LEN ; i++ ) {
      aes_key_buf[i] = my_sign_aes_key[i];
    }
    for( i = 0 ; i < AES_BLOCK_SIZE ; i++ ) {
      aes_iv[i] = my_sign_aes_iv[i];
    }

    AES_set_encrypt_key(aes_key_buf, AES_KEY_BIT_LEN, &aes_key);

    memset(block_buf_out, 0 , MY_SIGN_BLOCK_SIZE);
    for( i = 0 ; i < (MY_SIGN_BLOCK_SIZE / AES_BLOCK_SIZE) ; i++ ) {
      //	AES_encrypt( &(block_buf_in[AES_BLOCK_SIZE*i]), &(block_buf_out[AES_BLOCK_SIZE*i]),&aes_key);
      AES_cbc_encrypt( &(block_buf_in[AES_BLOCK_SIZE*i]), &(block_buf_out[AES_BLOCK_SIZE*i]), 
		       AES_BLOCK_SIZE, &aes_key, aes_iv, AES_ENCRYPT );

    }	

    if( MY_SIGN_BLOCK_SIZE != (outlen = fwrite( block_buf_out, 1, MY_SIGN_BLOCK_SIZE, fp_out)) ) {
      fprintf(stderr,"Error:fwrite line=%d outlen=%d\n", __LINE__,outlen);
      fprintf(stderr,"%s\n",  ERR_error_string(ERR_get_error(), NULL));
      rsa_error_flag = 1;
      goto end;
    }

    /* データブロックの署名計算 */
    if( 0 != my_sign_make( L2_sign_table_temp , block_buf_out, MY_SIGN_BLOCK_SIZE ) ) {
      fprintf(stderr,"make Data Block signature error line=%d\n",__LINE__);
      rsa_error_flag = 1;
      goto end;    
    }



    L2_sign_table_temp++;
    block_no++;
  }

  //miya  printf("last block no = %d\n",block_no);


  /* L2signテーブルの署名計算 */
  if( 0 != my_sign_make(&header.L2_sign, (u8 *)L2_sign_table, L2_sign_table_size ) ) {
    fprintf(stderr,"make L2_sign signature error line=%d\n",__LINE__);
    rsa_error_flag = 1;
    goto end;    
  }


  /* ヘッダーの署名計算 */
  if( 0 != my_sign_make(&header_sign, (u8 *)&header, sizeof(MY_SIGN_HEADER)) ) {
    fprintf(stderr,"make signature error line=%d\n",__LINE__);
    rsa_error_flag = 1;
    goto end;    
  }




  /* ヘッダーを出力ファイルに書き込み */
  if( 0 != fseek(fp_out,  0 , SEEK_SET) ) {
    fprintf(stderr,"fseek error %s %d\n",__FUNCTION__,__LINE__);
    goto end;
  }

  if( sizeof(MY_SIGN_HEADER) != (outlen = fwrite( &header, 1, sizeof(MY_SIGN_HEADER), fp_out)) ) {
    fprintf(stderr, "Error:fwrite line=%d outlen=%d\n", __LINE__,outlen);
    rsa_error_flag = 1;
    goto end;
  }    


  /* ヘッダーの署名を出力ファイルに書き込み */

  if( sizeof(MY_SIGN_SIGNATURE) != (outlen = fwrite( &header_sign, 1, sizeof(MY_SIGN_SIGNATURE), fp_out)) ) {
    fprintf(stderr, "Error:fwrite line=%d outlen=%d\n", __LINE__,outlen);
    rsa_error_flag = 1;
    goto end;
  }    

  /* L2_signテーブルを出力ファイルに書き込み */
  if( L2_sign_table_size != (outlen = fwrite( (u8 *)L2_sign_table, 1, L2_sign_table_size, fp_out)) ) {
    fprintf(stderr, "Error:fwrite line=%d outlen=%d\n", __LINE__,outlen);
    rsa_error_flag = 1;
    goto end;
  }    


  if( rsa_error_flag == 1 ) {
    printf("Error: %s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
  }

 end:

  if( L2_sign_table != NULL ) {
    free( L2_sign_table );
  }

  if( fp_in ) {
    fclose(fp_in);
  }

  if( fp_out ) {
    fclose(fp_out);
  }

  if( rsa_error_flag == 0 ) {
    //printf("success\n");
    return 0;
    
  }
  return -1; /* error */
}



