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

#include "my_aes.h"
#include "my_keys.h"


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
    
    // 型サイズチェック
    // ここでエラーが発生した場合はCRYPTO_RSA_CONTEXT_SIZEを修正すること{
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
  
  // 復号文字列の後にもデータが残っているため、直後の値を0にする
  *((u8*)param->out + len) = 0;

  return len;
}






static const int rsa_key_pub_len = sizeof rsa_key_pub;


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
		
    要素
    key RSA秘密鍵を格納した文字列のアドレスを指定します。 
    key_len RSA秘密鍵の文字列長を指定します。 
	  
    説明
    RSA復号のための初期化処理に必要な情報を指定するための構造体です。
    アプリケーション側から各メンバに値を設定してから関数を呼び出す必要があります。 
	  
    メンバ keyで指定したアドレスにはDER形式のRSA秘密鍵イメージ全体を格納してください。 
  */
  key = (void *)OS_Alloc(rsa_key_pub_len);
  if( key == NULL ) {
    OS_TPrintf("Error:alloc %s %d",__FUNCTION__,__LINE__);
    return 0;
  }
  memcpy(key, (void*)rsa_key_pub, rsa_key_pub_len);

  //  decinitparam.key     = (void*)rsa512_sec;
  decinitparam.key     = key;
  decinitparam.key_len = rsa_key_pub_len;
  result = miya_CRYPTO_RSA_DecryptInit(&context, &decinitparam);

  CheckAndPrintErr(result, "CRYPTO_RSA_DecryptInit");

  /*
    typedef struct CRYPTORSADecryptParam {
    void    *in;
    u32     in_len;
    void    *out;
    u32     out_size;
    } CRYPTORSADecryptParam;
		
    要素
    in RSAで復号する文字列のアドレスを指定します。 
    in_len RSAで復号する文字列の長さを指定します。 
    out 復号する文字列の出力先バッファアドレスを指定します 
    out_size outのバッファサイズを指定します 
	  
    説明
    RSA復号処理を行う文字列および出力先に関する情報を指定するための構造体です。
    アプリケーション側から各メンバに値を設定してから関数を呼び出す必要があります。 
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
