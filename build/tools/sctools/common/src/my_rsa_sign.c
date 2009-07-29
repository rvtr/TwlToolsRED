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
		
    要素
    key RSA秘密鍵を格納した文字列のアドレスを指定します。 
    key_len RSA秘密鍵の文字列長を指定します。 
	  
    説明
    RSA復号のための初期化処理に必要な情報を指定するための構造体です。
    アプリケーション側から各メンバに値を設定してから関数を呼び出す必要があります。 
	  
    メンバ keyで指定したアドレスにはDER形式のRSA秘密鍵イメージ全体を格納してください。 
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

/* ************* SHA256 ************** */

#define MD32_REG_T long

#define SHA_LONG unsigned long
#define SHA_LBLOCK	16
#define SHA_CBLOCK	(SHA_LBLOCK*4)	/* SHA treats input data as a
					 * contiguous array of 32 bit
					 * wide big-endian values. */
#define SHA_LAST_BLOCK  (SHA_CBLOCK-8)

#define SHA256_CBLOCK	(SHA_LBLOCK*4)	/* SHA-256 treats input data as a
					 * contiguous array of 32 bit
					 * wide big-endian values. */
#define SHA224_DIGEST_LENGTH	28
#define SHA256_DIGEST_LENGTH	32

typedef struct SHA256state_st
{
  SHA_LONG h[8];
  SHA_LONG Nl,Nh;
  SHA_LONG data[SHA_LBLOCK];
  unsigned int num,md_len;
} SHA256_CTX;


static int SHA256_Init(SHA256_CTX *c);
static int SHA256_Update(SHA256_CTX *c, const void *data, size_t len);
static int SHA256_Final(unsigned char *md, SHA256_CTX *c);


void SHA256(const unsigned char *d, size_t n, unsigned char *md)
{
  SHA256_CTX c;
  SHA256_Init(&c);
  SHA256_Update(&c,d,n);
  SHA256_Final(md,&c);
  //  OPENSSL_cleanse(&c,sizeof(c));
}


static int SHA256_Init (SHA256_CTX *c)
{
  memset (c,0,sizeof(*c));
  c->h[0]=0x6a09e667UL;	c->h[1]=0xbb67ae85UL;
  c->h[2]=0x3c6ef372UL;	c->h[3]=0xa54ff53aUL;
  c->h[4]=0x510e527fUL;	c->h[5]=0x9b05688cUL;
  c->h[6]=0x1f83d9abUL;	c->h[7]=0x5be0cd19UL;
  c->md_len=SHA256_DIGEST_LENGTH;
  return 1;
}


#define	HASH_BLOCK_DATA_ORDER	sha256_block_data_order

static void sha256_block_data_order (SHA256_CTX *ctx, const void *in, size_t num);


#define	DATA_ORDER_IS_BIG_ENDIAN

#define	HASH_LONG		SHA_LONG
#define	HASH_CTX		SHA256_CTX
#define	HASH_CBLOCK		SHA_CBLOCK



#if defined(DATA_ORDER_IS_BIG_ENDIAN)

#define HOST_c2l(c,l)	(l =(((unsigned long)(*((c)++)))<<24),		\
			 l|=(((unsigned long)(*((c)++)))<<16),		\
			 l|=(((unsigned long)(*((c)++)))<< 8),		\
			 l|=(((unsigned long)(*((c)++)))    ))


#define HOST_l2c(l,c)	(*((c)++)=(unsigned char)(((l)>>24)&0xff),	\
			 *((c)++)=(unsigned char)(((l)>>16)&0xff),	\
			 *((c)++)=(unsigned char)(((l)>> 8)&0xff),	\
			 *((c)++)=(unsigned char)(((l)    )&0xff))



#elif defined(DATA_ORDER_IS_LITTLE_ENDIAN)

#define HOST_c2l(c,l)	(l =(((unsigned long)(*((c)++)))    ),		\
			 l|=(((unsigned long)(*((c)++)))<< 8),		\
			 l|=(((unsigned long)(*((c)++)))<<16),		\
			 l|=(((unsigned long)(*((c)++)))<<24))


#define HOST_l2c(l,c)	(*((c)++)=(unsigned char)(((l)    )&0xff),	\
			 *((c)++)=(unsigned char)(((l)>> 8)&0xff),	\
			 *((c)++)=(unsigned char)(((l)>>16)&0xff),	\
			 *((c)++)=(unsigned char)(((l)>>24)&0xff))

#endif


#define	HASH_MAKE_STRING(c,s)	do {	\
	unsigned long ll;		\
	unsigned int  nn;		\
	switch ((c)->md_len)		\
	{   case SHA224_DIGEST_LENGTH:	\
		for (nn=0;nn<SHA224_DIGEST_LENGTH/4;nn++)	\
		{   ll=(c)->h[nn]; HOST_l2c(ll,(s));   }	\
		break;			\
	    case SHA256_DIGEST_LENGTH:	\
		for (nn=0;nn<SHA256_DIGEST_LENGTH/4;nn++)	\
		{   ll=(c)->h[nn]; HOST_l2c(ll,(s));   }	\
		break;			\
	    default:			\
		if ((c)->md_len > SHA256_DIGEST_LENGTH)	\
		    return 0;				\
		for (nn=0;nn<(c)->md_len/4;nn++)		\
		{   ll=(c)->h[nn]; HOST_l2c(ll,(s));   }	\
		break;			\
	}				\
	} while (0)




static int SHA256_Update(HASH_CTX *c, const void *data_, size_t len)
{
  const unsigned char *data=data_;
  unsigned char *p;
  HASH_LONG l;
  size_t n;

  if (len==0) return 1;

  l=(c->Nl+(((HASH_LONG)len)<<3))&0xffffffffUL;
  /* 95-05-24 eay Fixed a bug with the overflow handling, thanks to
   * Wei Dai <weidai@eskimo.com> for pointing it out. */
  if (l < c->Nl) /* overflow */
    c->Nh++;
  c->Nh+=(HASH_LONG)(len>>29);	/* might cause compiler warning on 16-bit */
  c->Nl=l;

  n = c->num;
  if (n != 0)
    {
      p=(unsigned char *)c->data;

      if (len >= HASH_CBLOCK || len+n >= HASH_CBLOCK)
	{
	  memcpy (p+n,data,HASH_CBLOCK-n);
	  HASH_BLOCK_DATA_ORDER (c,p,1);
	  n      = HASH_CBLOCK-n;
	  data  += n;
	  len   -= n;
	  c->num = 0;
	  memset (p,0,HASH_CBLOCK);	/* keep it zeroed */
	}
      else
	{
	  memcpy (p+n,data,len);
	  c->num += (unsigned int)len;
	  return 1;
	}
    }

  n = len/HASH_CBLOCK;
  if (n > 0)
    {
      HASH_BLOCK_DATA_ORDER (c,data,n);
      n    *= HASH_CBLOCK;
      data += n;
      len  -= n;
    }

  if (len != 0)
    {
      p = (unsigned char *)c->data;
      c->num = (unsigned int)len;
      memcpy (p,data,len);
    }
  return 1;
}


static int SHA256_Final(unsigned char *md, HASH_CTX *c)
{
  unsigned char *p = (unsigned char *)c->data;
  size_t n = c->num;

  p[n] = 0x80; /* there is always room for one */
  n++;

  if (n > (HASH_CBLOCK-8))
    {
      memset (p+n,0,HASH_CBLOCK-n);
      n=0;
      HASH_BLOCK_DATA_ORDER (c,p,1);
    }
  memset (p+n,0,HASH_CBLOCK-8-n);

  p += HASH_CBLOCK-8;
#if   defined(DATA_ORDER_IS_BIG_ENDIAN)
  (void)HOST_l2c(c->Nh,p);
  (void)HOST_l2c(c->Nl,p);
#elif defined(DATA_ORDER_IS_LITTLE_ENDIAN)
  (void)HOST_l2c(c->Nl,p);
  (void)HOST_l2c(c->Nh,p);
#endif
  p -= HASH_CBLOCK;
  HASH_BLOCK_DATA_ORDER (c,p,1);
  c->num=0;
  memset (p,0,HASH_CBLOCK);

#ifndef HASH_MAKE_STRING
#error "HASH_MAKE_STRING must be defined!"
#else
  HASH_MAKE_STRING(c,md);
#endif

  return 1;
}


static const SHA_LONG K256[64] = {
	0x428a2f98UL,0x71374491UL,0xb5c0fbcfUL,0xe9b5dba5UL,
	0x3956c25bUL,0x59f111f1UL,0x923f82a4UL,0xab1c5ed5UL,
	0xd807aa98UL,0x12835b01UL,0x243185beUL,0x550c7dc3UL,
	0x72be5d74UL,0x80deb1feUL,0x9bdc06a7UL,0xc19bf174UL,
	0xe49b69c1UL,0xefbe4786UL,0x0fc19dc6UL,0x240ca1ccUL,
	0x2de92c6fUL,0x4a7484aaUL,0x5cb0a9dcUL,0x76f988daUL,
	0x983e5152UL,0xa831c66dUL,0xb00327c8UL,0xbf597fc7UL,
	0xc6e00bf3UL,0xd5a79147UL,0x06ca6351UL,0x14292967UL,
	0x27b70a85UL,0x2e1b2138UL,0x4d2c6dfcUL,0x53380d13UL,
	0x650a7354UL,0x766a0abbUL,0x81c2c92eUL,0x92722c85UL,
	0xa2bfe8a1UL,0xa81a664bUL,0xc24b8b70UL,0xc76c51a3UL,
	0xd192e819UL,0xd6990624UL,0xf40e3585UL,0x106aa070UL,
	0x19a4c116UL,0x1e376c08UL,0x2748774cUL,0x34b0bcb5UL,
	0x391c0cb3UL,0x4ed8aa4aUL,0x5b9cca4fUL,0x682e6ff3UL,
	0x748f82eeUL,0x78a5636fUL,0x84c87814UL,0x8cc70208UL,
	0x90befffaUL,0xa4506cebUL,0xbef9a3f7UL,0xc67178f2UL };

#define ROTATE(a,n)     (((a)<<(n))|(((a)&0xffffffff)>>(32-(n))))


#define Sigma0(x)	(ROTATE((x),30) ^ ROTATE((x),19) ^ ROTATE((x),10))
#define Sigma1(x)	(ROTATE((x),26) ^ ROTATE((x),21) ^ ROTATE((x),7))
#define sigma0(x)	(ROTATE((x),25) ^ ROTATE((x),14) ^ ((x)>>3))
#define sigma1(x)	(ROTATE((x),15) ^ ROTATE((x),13) ^ ((x)>>10))

#define Ch(x,y,z)	(((x) & (y)) ^ ((~(x)) & (z)))
#define Maj(x,y,z)	(((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))


#define	ROUND_00_15(i,a,b,c,d,e,f,g,h)		do {	\
	T1 += h + Sigma1(e) + Ch(e,f,g) + K256[i];	\
	h = Sigma0(a) + Maj(a,b,c);			\
	d += T1;	h += T1;		} while (0)

#define	ROUND_16_63(i,a,b,c,d,e,f,g,h,X)	do {	\
	s0 = X[(i+1)&0x0f];	s0 = sigma0(s0);	\
	s1 = X[(i+14)&0x0f];	s1 = sigma1(s1);	\
	T1 = X[(i)&0x0f] += s0 + s1 + X[(i+9)&0x0f];	\
	ROUND_00_15(i,a,b,c,d,e,f,g,h);		} while (0)

static void sha256_block_data_order (SHA256_CTX *ctx, const void *in, size_t num)
{
  unsigned MD32_REG_T a,b,c,d,e,f,g,h,s0,s1,T1;
  SHA_LONG	X[16];
  int i;
  const unsigned char *data=in;
  const union { long one; char little; } is_endian = {1};

  while (num--) {

    a = ctx->h[0];	b = ctx->h[1];	c = ctx->h[2];	d = ctx->h[3];
    e = ctx->h[4];	f = ctx->h[5];	g = ctx->h[6];	h = ctx->h[7];

    if (!is_endian.little && sizeof(SHA_LONG)==4 && ((size_t)in%4)==0)
      {
	const SHA_LONG *W=(const SHA_LONG *)data;

	T1 = X[0] = W[0];	ROUND_00_15(0,a,b,c,d,e,f,g,h);
	T1 = X[1] = W[1];	ROUND_00_15(1,h,a,b,c,d,e,f,g);
	T1 = X[2] = W[2];	ROUND_00_15(2,g,h,a,b,c,d,e,f);
	T1 = X[3] = W[3];	ROUND_00_15(3,f,g,h,a,b,c,d,e);
	T1 = X[4] = W[4];	ROUND_00_15(4,e,f,g,h,a,b,c,d);
	T1 = X[5] = W[5];	ROUND_00_15(5,d,e,f,g,h,a,b,c);
	T1 = X[6] = W[6];	ROUND_00_15(6,c,d,e,f,g,h,a,b);
	T1 = X[7] = W[7];	ROUND_00_15(7,b,c,d,e,f,g,h,a);
	T1 = X[8] = W[8];	ROUND_00_15(8,a,b,c,d,e,f,g,h);
	T1 = X[9] = W[9];	ROUND_00_15(9,h,a,b,c,d,e,f,g);
	T1 = X[10] = W[10];	ROUND_00_15(10,g,h,a,b,c,d,e,f);
	T1 = X[11] = W[11];	ROUND_00_15(11,f,g,h,a,b,c,d,e);
	T1 = X[12] = W[12];	ROUND_00_15(12,e,f,g,h,a,b,c,d);
	T1 = X[13] = W[13];	ROUND_00_15(13,d,e,f,g,h,a,b,c);
	T1 = X[14] = W[14];	ROUND_00_15(14,c,d,e,f,g,h,a,b);
	T1 = X[15] = W[15];	ROUND_00_15(15,b,c,d,e,f,g,h,a);

	data += SHA256_CBLOCK;
      }
    else
      {
	SHA_LONG l;

	HOST_c2l(data,l); T1 = X[0] = l;  ROUND_00_15(0,a,b,c,d,e,f,g,h);
	HOST_c2l(data,l); T1 = X[1] = l;  ROUND_00_15(1,h,a,b,c,d,e,f,g);
	HOST_c2l(data,l); T1 = X[2] = l;  ROUND_00_15(2,g,h,a,b,c,d,e,f);
	HOST_c2l(data,l); T1 = X[3] = l;  ROUND_00_15(3,f,g,h,a,b,c,d,e);
	HOST_c2l(data,l); T1 = X[4] = l;  ROUND_00_15(4,e,f,g,h,a,b,c,d);
	HOST_c2l(data,l); T1 = X[5] = l;  ROUND_00_15(5,d,e,f,g,h,a,b,c);
	HOST_c2l(data,l); T1 = X[6] = l;  ROUND_00_15(6,c,d,e,f,g,h,a,b);
	HOST_c2l(data,l); T1 = X[7] = l;  ROUND_00_15(7,b,c,d,e,f,g,h,a);
	HOST_c2l(data,l); T1 = X[8] = l;  ROUND_00_15(8,a,b,c,d,e,f,g,h);
	HOST_c2l(data,l); T1 = X[9] = l;  ROUND_00_15(9,h,a,b,c,d,e,f,g);
	HOST_c2l(data,l); T1 = X[10] = l; ROUND_00_15(10,g,h,a,b,c,d,e,f);
	HOST_c2l(data,l); T1 = X[11] = l; ROUND_00_15(11,f,g,h,a,b,c,d,e);
	HOST_c2l(data,l); T1 = X[12] = l; ROUND_00_15(12,e,f,g,h,a,b,c,d);
	HOST_c2l(data,l); T1 = X[13] = l; ROUND_00_15(13,d,e,f,g,h,a,b,c);
	HOST_c2l(data,l); T1 = X[14] = l; ROUND_00_15(14,c,d,e,f,g,h,a,b);
	HOST_c2l(data,l); T1 = X[15] = l; ROUND_00_15(15,b,c,d,e,f,g,h,a);
      }

    for (i=16;i<64;i+=8)
      {
	ROUND_16_63(i+0,a,b,c,d,e,f,g,h,X);
	ROUND_16_63(i+1,h,a,b,c,d,e,f,g,X);
	ROUND_16_63(i+2,g,h,a,b,c,d,e,f,X);
	ROUND_16_63(i+3,f,g,h,a,b,c,d,e,X);
	ROUND_16_63(i+4,e,f,g,h,a,b,c,d,X);
	ROUND_16_63(i+5,d,e,f,g,h,a,b,c,X);
	ROUND_16_63(i+6,c,d,e,f,g,h,a,b,X);
	ROUND_16_63(i+7,b,c,d,e,f,g,h,a,X);
      }

    ctx->h[0] += a;	ctx->h[1] += b;	ctx->h[2] += c;	ctx->h[3] += d;
    ctx->h[4] += e;	ctx->h[5] += f;	ctx->h[6] += g;	ctx->h[7] += h;

  }
}

/* ******************* */


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
