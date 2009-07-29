#include <twl.h>
#include <nitro/os.h>
#include <nitro/crypto.h>
#include <nitro/std.h>
#include <string.h>



#define MY_RSA_SIGN_DEBUG 1


#ifdef MY_RSA_SIGN_DEBUG
#include        "text.h"
#include        "mprintf.h"
#endif	/* MY_RSA_SIGN_DEBUG */

#include        "my_rsa_sign.h"
#include        "my_sha.h"


#include <pk.h>
#include <rsa.h>
#include <r_prod.h>
#include <evp.h>
#include <berobj.h>
#include <ber.h>

/* 
 */
int R_CDECL CRYPTOi__RSA_public_decrypt(int flen, unsigned char *from, unsigned char *to,
					RSA *pkey, int padding);

PK_METH * R_CDECL CRYPTOi__PK_METH_rsa_pkcs1_public(void);


//#define RSA_PublicKey_from_binary     d2i_RSAPublicKey
RSA * R_CDECL CRYPTOi__d2i_RSAPublicKey(RSA **key, unsigned char **buf, long length);


typedef struct CRYPTORSAContext_local
{
    RSA        *key;
}
CRYPTORSAContext_local;


static s32 miya_CRYPTO_RSA_DecryptInit(CRYPTORSAContext *context,
						   CRYPTORSADecryptInitParam *param)
{
  CRYPTORSAContext_local *lc;
  
    unsigned char   *p;
    
    // �^�T�C�Y�`�F�b�N
    // �����ŃG���[�����������ꍇ��CRYPTO_RSA_CONTEXT_SIZE���C�����邱��{
    SDK_ASSERT(sizeof(CRYPTORSAContext) == sizeof(CRYPTORSAContext_local));
    
    lc = (CRYPTORSAContext_local*)context;
    
    p   = (unsigned char *)param->key;  //test_keys->test_key;

    lc->key = CRYPTOi__d2i_RSAPublicKey(NULL, &p, (long)param->key_len);
    if(lc->key == NULL)
      {
	return -1;
      }
    
    return 0;
}


static int get_dec_shift_len(void *buf, int buf_len)
{
  u8 *p;
  
  for(p = buf; p < (u8*)buf + buf_len; p++)
    {
      if(*p != '\0')
	{
	  return (int)(p - buf);
	}
    }
  
  return -1;
}

static s32 miya_CRYPTO_RSA_Decrypt(CRYPTORSAContext *context,
				   CRYPTORSADecryptParam *param)
{
  //  int                       len, shift;
  int                       len;
  CRYPTORSAContext_local    *lc;
  



  lc = (CRYPTORSAContext_local*)context;

  lc->key->rsa_meth = CRYPTOi__PK_METH_rsa_pkcs1_public();

#if (RSA_GENERATE_TEST == 1)
  if (PK_CTX_set(lc->key->pk_ctx, lc->key->rsa_meth, lc->key->key, 0, 0) != 0)
    {
      return -1;
    }
#endif
  
  memset(param->out, 0, param->out_size);


  if ((len = CRYPTOi__RSA_public_decrypt((int)param->in_len, param->in, param->out, lc->key, RSA_PKCS1_PADDING)) <= 0)
    {
      return -1;
    }
  
  // ����������̌�ɂ��f�[�^���c���Ă��邽�߁A����̒l��0�ɂ���
  *((u8*)param->out + len) = 0;

  return len;
}


#if 1
static const unsigned char rsa512_pub[]={
        0x30,0x81,0x89,0x02,0x81,0x81,0x00,0xeb,0x72,0xe2,0x05,0x45,0x37,0xd9,0x78,0x35,
        0xb5,0x8d,0x70,0x9a,0xe7,0x17,0x42,0xe9,0xf3,0x26,0x73,0x4e,0xdf,0x5e,0x96,0x5f,
        0xcf,0xf2,0xf1,0x2d,0xc0,0x41,0x31,0xb6,0x3b,0xe8,0xa4,0xd7,0x70,0xdb,0x3c,0xfd,
        0x66,0x0d,0xea,0x2f,0xb8,0x7b,0xf0,0x2d,0x70,0xe0,0xf1,0x05,0x55,0xe6,0x33,0x8f,
        0x3a,0xde,0x79,0xce,0xd0,0x11,0xbf,0xda,0x78,0xe1,0xef,0x8b,0x0e,0x2e,0xa7,0xe2,
        0x61,0x88,0x58,0x90,0x1d,0x0c,0x6d,0x5b,0x40,0xbf,0x6f,0xc7,0x18,0xde,0xe8,0xfd,
        0xd9,0xd9,0x1e,0xb9,0xe4,0xa3,0x4d,0x04,0x39,0x4b,0x8f,0x5b,0x13,0xad,0x14,0x0b,
        0xf8,0x53,0xbc,0xae,0x72,0x91,0x6b,0xcd,0xf9,0x39,0x8d,0x17,0x3d,0xc8,0xee,0xc9,
        0xcc,0x95,0x35,0x38,0xb0,0x80,0x53,0x02,0x03,0x01,0x00,0x01
};
#else

static const unsigned char rsa512_pub[]={
        0x30,0x81,0x9f,0x30,0x0d,0x06,0x09,0x2a,0x86,0x48,0x86,0xf7,0x0d,0x01,0x01,0x01,
        0x05,0x00,0x03,0x81,0x8d,0x00,0x30,0x81,0x89,0x02,0x81,0x81,0x00,0xeb,0x72,0xe2,
        0x05,0x45,0x37,0xd9,0x78,0x35,0xb5,0x8d,0x70,0x9a,0xe7,0x17,0x42,0xe9,0xf3,0x26,
        0x73,0x4e,0xdf,0x5e,0x96,0x5f,0xcf,0xf2,0xf1,0x2d,0xc0,0x41,0x31,0xb6,0x3b,0xe8,
        0xa4,0xd7,0x70,0xdb,0x3c,0xfd,0x66,0x0d,0xea,0x2f,0xb8,0x7b,0xf0,0x2d,0x70,0xe0,
        0xf1,0x05,0x55,0xe6,0x33,0x8f,0x3a,0xde,0x79,0xce,0xd0,0x11,0xbf,0xda,0x78,0xe1,
        0xef,0x8b,0x0e,0x2e,0xa7,0xe2,0x61,0x88,0x58,0x90,0x1d,0x0c,0x6d,0x5b,0x40,0xbf,
        0x6f,0xc7,0x18,0xde,0xe8,0xfd,0xd9,0xd9,0x1e,0xb9,0xe4,0xa3,0x4d,0x04,0x39,0x4b,
        0x8f,0x5b,0x13,0xad,0x14,0x0b,0xf8,0x53,0xbc,0xae,0x72,0x91,0x6b,0xcd,0xf9,0x39,
        0x8d,0x17,0x3d,0xc8,0xee,0xc9,0xcc,0x95,0x35,0x38,0xb0,0x80,0x53,0x02,0x03,0x01,
        0x00,0x01
};

#endif

static const int rsa512_pub_len = sizeof rsa512_pub;


static void* MyAlloc(u32 size)
{
    void* ptr;
    ptr = OS_Alloc(size);
    return ptr;
}

static void MyFree(void* ptr)
{
    OS_Free(ptr);
}

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

static void* MyRealloc(void* ptr, u32 new_size, u32 old_size)
{
    void *buf;
    if ((buf = OS_Alloc(new_size)) == NULL)
    {
        return NULL;
    }
    MI_CpuCopy8(ptr, buf, MIN(new_size, old_size));
    OS_Free(ptr);
    return buf;
}


#define PrintResultEq( a, b, f ) \
    { OS_TPrintf( ((a) == (b)) ? "[--OK--] " : "[**NG**] " ); (f) = (f) && ((a) == (b)); }

#define CheckAndPrintErr( result, str ) \
    { if(OS_IsRunOnTwl() == TRUE){      \
          if(result < 0) { OS_TPrintf( "Error: %d (%s)\n", result, str ); return FALSE;}  \
      }else{                            \
          if(result >= 0){ OS_TPrintf( "Error: %d (%s)\n", result, str ); return FALSE;}  \
      }  \
    }

BOOL RsaTestInit(void)
{
  static BOOL init_flag = FALSE;

  if( init_flag == TRUE ) {
    return TRUE;
  }

  CRYPTO_SetMemAllocator(MyAlloc, MyFree, MyRealloc);

  init_flag = TRUE;
  return TRUE;
}


int RsaTestDecrypt(char *input, int in_len, char *output, int outlen)
{
  s32                          result;
  s32                          outcount = 0;
  CRYPTORSAContext             context;
  CRYPTORSADecryptInitParam    decinitparam;
  CRYPTORSADecryptParam        decparam;

  void *key;

  memset((void *)&context, 0, sizeof(CRYPTORSAContext));
  memset((void *)&decinitparam, 0, sizeof(CRYPTORSADecryptInitParam));
  memset((void *)&decparam, 0, sizeof(CRYPTORSADecryptParam));

  /* RSA decode */
  /*
    typedef struct CRYPTORSADecryptInitParam {
    void    *key;
    u32     key_len;
    } CRYPTORSADecryptInitParam;
		
    �v�f
    key RSA�閧�����i�[����������̃A�h���X���w�肵�܂��B 
    key_len RSA�閧���̕����񒷂��w�肵�܂��B 
	  
    ����
    RSA�����̂��߂̏����������ɕK�v�ȏ����w�肷�邽�߂̍\���̂ł��B
    �A�v���P�[�V����������e�����o�ɒl��ݒ肵�Ă���֐����Ăяo���K�v������܂��B 
	  
    �����o key�Ŏw�肵���A�h���X�ɂ�DER�`����RSA�閧���C���[�W�S�̂��i�[���Ă��������B 
  */
  key = (void *)OS_Alloc(rsa512_pub_len);
  if( key == NULL ) {
    OS_TPrintf("Error:alloc %s %d",__FUNCTION__,__LINE__);
    return 0;
  }
  memcpy(key, (void*)rsa512_pub, rsa512_pub_len);

  //  decinitparam.key     = (void*)rsa512_sec;
  decinitparam.key     = key;
  decinitparam.key_len = rsa512_pub_len;
  result = miya_CRYPTO_RSA_DecryptInit(&context, &decinitparam);

  CheckAndPrintErr(result, "CRYPTO_RSA_DecryptInit");

  /*
    typedef struct CRYPTORSADecryptParam {
    void    *in;
    u32     in_len;
    void    *out;
    u32     out_size;
    } CRYPTORSADecryptParam;
		
    �v�f
    in RSA�ŕ������镶����̃A�h���X���w�肵�܂��B 
    in_len RSA�ŕ������镶����̒������w�肵�܂��B 
    out �������镶����̏o�͐�o�b�t�@�A�h���X���w�肵�܂� 
    out_size out�̃o�b�t�@�T�C�Y���w�肵�܂� 
	  
    ����
    RSA�����������s�������񂨂�яo�͐�Ɋւ�������w�肷�邽�߂̍\���̂ł��B
    �A�v���P�[�V����������e�����o�ɒl��ݒ肵�Ă���֐����Ăяo���K�v������܂��B 
  */

  //BOOL RsaTestDecrypt(char *input, int in_len, char *output, int outlen)

  decparam.in      = input;
  decparam.in_len  = (u32)in_len;
  decparam.out     = output;
  decparam.out_size = (u32)outlen;

  //  outcount = 0;
  outcount = miya_CRYPTO_RSA_Decrypt(&context, &decparam);

  CheckAndPrintErr(result, "CRYPTO_RSA_Decrypt");


  result = CRYPTO_RSA_DecryptTerminate(&context);
  CheckAndPrintErr(result, "CRYPTO_RSA_DecryptTerminate");

  OS_Free(key);
  //  OS_TPrintf("in_len = %d outcount = %d\n",in_len, outcount);

  return outcount;
}



int my_rsa_sign(  u8 *inbuf, u8 *outbuf, int buf_size,  int *output_size )
{
  int i;
  u8 hash_data[MY_RSA_SIGN_HASH_SIZE];
  u8 hash_calc[MY_RSA_SIGN_HASH_SIZE];
  BOOL hash_check_flag = TRUE;
  int decrypt_size = 0;
  u8 decrypt_buf[MY_RSA_SIGN_RSA_SIZE];
  int org_file_size = 0;

  if( TRUE != RsaTestInit() ) {
#ifdef MY_RSA_SIGN_DEBUG
    mprintf("RSA Init failed\n");
#endif	/* MY_RSA_SIGN_DEBUG */
    return MY_RSA_SIGN_ERROR_RSA_INIT_FAILED;
  }
#ifdef MY_RSA_SIGN_DEBUG
  mprintf("RSA Init success\n");
#endif	/* MY_RSA_SIGN_DEBUG */

  if( 0 == RsaTestDecrypt((char *)inbuf, MY_RSA_SIGN_RSA_SIZE, (char *)outbuf, buf_size) ) {
#ifdef MY_RSA_SIGN_DEBUG
    mprintf("RSA Decrypt failed\n");
#endif	/* MY_RSA_SIGN_DEBUG */
    return MY_RSA_SIGN_ERROR_RSA_HASH_DECRYPT_FAILED;
  }
#ifdef MY_RSA_SIGN_DEBUG
  mprintf("RSA Decrypt success 1\n");
#endif	/* MY_RSA_SIGN_DEBUG */

  memcpy(hash_data, outbuf, MY_RSA_SIGN_HASH_SIZE);

  SHA256( inbuf + MY_RSA_SIGN_RSA_SIZE, (u32)(buf_size - MY_RSA_SIGN_RSA_SIZE), hash_calc);

  for( i = 0 ; i < MY_RSA_SIGN_HASH_SIZE ; i++ ) {
    if( hash_calc[i] != hash_data[i] ) {
      hash_check_flag = FALSE;
    }
  }
  if( hash_check_flag == FALSE ) {
    OS_TPrintf("invalid hash\n");
    return MY_RSA_SIGN_ERROR_HASH_CHECK_FAILED;
  }
#ifdef MY_RSA_SIGN_DEBUG
  mprintf("hash check pass!\n");
#endif	/* MY_RSA_SIGN_DEBUG */
  
  org_file_size = *(int *)(inbuf + MY_RSA_SIGN_RSA_SIZE);	
  *output_size = 0;

  for( i = 0 ; i < org_file_size ; i += MY_RSA_SIGN_RSA_SIZE ) {
    decrypt_size = RsaTestDecrypt((char *)(inbuf + MY_RSA_SIGN_RSA_SIZE + 4 + i), MY_RSA_SIGN_RSA_SIZE, 
				  (char *)decrypt_buf, MY_RSA_SIGN_RSA_SIZE );
    if( 0 != decrypt_size )  {
      //OS_TPrintf("RSA Decrypt success dec_size=%d\n",decrypt_size);
      //mprintf("RSA Decrypt success 2\n");
      memcpy( (char *)(outbuf + *output_size), decrypt_buf, (u32)decrypt_size);
      *output_size += decrypt_size;
    }
    else {
#ifdef MY_RSA_SIGN_DEBUG
      mprintf("RSA Decrypt Error dec_size=%d\n",decrypt_size);
#endif	/* MY_RSA_SIGN_DEBUG */
      return MY_RSA_SIGN_ERROR_RSA_CONTENT_DECRYPT_FAILED;
    }
  }

  OS_TPrintf("output_size = %d\n",*output_size );
  OS_TPrintf("org_file_size = %d\n",org_file_size );

  return MY_RSA_SIGN_SUCCESS;
}

/* ************************ */
