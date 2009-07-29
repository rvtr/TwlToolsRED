/* ====================================================================
 * Copyright (c) 1998-2008 The OpenSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.openssl.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    openssl-core@openssl.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.openssl.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com).  This product includes software written by Tim
 * Hudson (tjh@cryptsoft.com).
 *
 */

/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 *
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 * 
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 * 
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from 
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
 * 
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */

#include <twl.h>
#include <string.h>
#include        "my_rsa_sign.h"
#include        "my_sha.h"

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
