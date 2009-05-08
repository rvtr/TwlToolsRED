/**************************************************************************
 *               Copyright (C) 1997, Nintendo Co.,Ltd.                    *
 *************************************************************************/

/* c:/program files/Metrowerks/CodeWarrior for NITRO V0.4/ARM_EABI_Support/msl/MSL_C/MSL_ARM/Include/ */

//#pragma avoid_byte none
//#pragma warn_byte none

#define __std(ref) ref
// #define __std(ref) ::std::ref


#include <twl.h>



#define	__fourbytealign(n)	((((unsigned long) (n)) + 3U) & ~3U)
#define __va_start(parm)	((__std(va_list)) ((char*) ((unsigned long)(&parm) & ~3U) + __fourbytealign(sizeof(parm))))

#define va_start(ap, parm)	((ap) = __va_start(parm))

#define va_arg(ap, type)	(*(type *) ((ap += __fourbytealign(sizeof(type))) - __fourbytealign(sizeof(type))))
#define va_end(ap)	       ((void) 0)


#include "text.h"
#include "mprintf.h"

#define	CODE32


static inline u8 My_ReadByte( const void* address )
{
  if ( (u32)address & 1 )
  {
    return (u8)( *(u16*)((u32)address & ~1) >> 8 );
  }
  else
  {
    return (u8)( *(u16*)address & 0xff );
  }
}



static inline void My_WriteByte( void* address, u8 value )
{
  u16 val = *(u16*)((u32)address & ~1);

  if ( (u32)address & 1 )
  {
    *(u16*)((u32)address & ~1) = (u16)( ((value & 0xff)<< 8) | (val & 0xff) );
  }
  else
  {
    *(u16*)((u32)address & ~1) = (u16)( (val & 0xff00) | (value & 0xff) );
  }
}



void mprintf(const char *fmt, ...);

static void *memcpy(void *dest, const void *src, size_t n)
{
  unsigned char *s = (unsigned char *)dest;
  unsigned char *ss = (unsigned char *)src;
  
  while(n--) {
    My_WriteByte( s++,My_ReadByte(ss++));
  }
  return dest;
}



static const char *strchr(const char *s,int c)
{
  int a;
  while( (a = (int)My_ReadByte(s)) != NULL ) {
    if(a == c)
      return s;
    s++;
  }
  return (const char *)NULL;
}

static size_t strlen(const char *s)
{
  size_t n=0;
  while( My_ReadByte(s) != NULL) {
    n++;
    s++;
  }
  return n;
}



#define MIYA_PUTCHAR m_putchar

static int _Printf(void *(*pfn)(void *, const char *, size_t),
		   void *arg, const char *fmt, va_list ap);

static void *proutPrintf(void *txb, const char *buf, size_t n)
{
  /* write to file */
  /* return (fwrite(buf, 1, n, str) == n ? str : NULL);*/
  int i;
  
  for (i = 0; i < n; i++)
    MIYA_PUTCHAR( txb, My_ReadByte(&(buf[i])));
  /* return a fake pointer so that it's not NULL */
  return ((void *)txb);
}

void mputchar(int c)
{
  //  TEXT_CTRL *tc;
  m_putchar(tc[0], c );
}

void mprintf(const char *fmt, ...)
{
  void *tb = (void *)tc[0];
  int ans;
  va_list ap;
  va_start(ap, fmt);
  ans = _Printf(&proutPrintf, tb, fmt, ap);
  va_end(ap);
}

void mfprintf(TEXT_CTRL *tc, const char *fmt, ...)
{
  int ans;
  va_list ap;
  va_start(ap, fmt);
  ans = _Printf(&proutPrintf, (void *)tc, fmt, ap);
  va_end(ap);
}

/*******************************************************/

/* _Printf function */

#define _YVALS

/* float properties */
#define _D0	3
#define	_DBIAS	(0x400-1)
#define _DLONG	0
#define _DOFF	4
#define _LBIAS	(0x400-1)
#define _LONG_DOUBLE	0

/* pointer properties */
#define _NULL	(void *) 0
typedef unsigned int _Sizet;

/***********************************************************/
#define _FSP	0x01
#define _FPL	0x02
#define _FMI	0x04
#define _FNO	0x08
#define _FZE	0x10
#define _WMAX	999
#define _WANT	(EOF-1)

#if _LONG_DOUBLE
typedef long double ldouble;
#else
typedef double ldouble;
#endif

typedef struct {
  union {
    long long ll;
    ldouble ld;
  } v;
  char *s;
  int n0, nz0, n1, nz1, n2, nz2, prec, width;
  size_t nchar;
  unsigned int flags;
  char qual;
  char padding0[3];
} _Pft;

/* declarations */
static void miya_Ldtob(_Pft *, char);
static void miya_Litob(_Pft *, char);



/* macros */

#if _DLONG
#define LDSIGN(x) (((unsigned short *) &(x))[_D0 ? 4 : 0] & 0x8000)
#else
#define LDSIGN(x) (((unsigned short *) &(x))[_D0] & 0x8000)
#endif

#define ISDIGIT(c)	((c >= '0') && (c <= '9'))
#define MAX_PAD	(sizeof(spaces) - 1)
#define PAD(s, n) \
  if (0 < (n)) { \
		   int i, j = (n); \
		     for (; 0 < j; j -= i) { \
					       i = MAX_PAD < j ? MAX_PAD : j; \
						 PUT(s, i); \
						 } \
						 }
#define PUT(s, n) \
  if (0 < (n)) { \
		   if ((arg = (*pfn)(arg, s, n)) != NULL) \
		     x.nchar += (n); \
		   else \
		     return (x.nchar); \
		     }

static char spaces[] = "                                ";
static char zeroes[] = "00000000000000000000000000000000";

static void _Putfld(_Pft *, va_list *, char, char *);


typedef struct {
  long   quot;
  long   rem;
} miya_ldiv_t;

typedef struct {
  long long      quot;
  long long      rem;
} miya_lldiv_t;

static miya_lldiv_t miya_lldiv(long long a, long long b)
{
  miya_lldiv_t t;
  t.quot = a / b;
  t.rem = a % b;
  return t;
}

static miya_ldiv_t miya_ldiv(long a, long b)
{
  miya_ldiv_t t;
  t.quot = a / b;
  t.rem = a % b;
  return t;
}

// static void *proutPrintf(void *txb, const char *buf, size_t n)
// ans = _Printf(&proutPrintf, tb, (const char *)tc->buffer, ap);

static int _Printf(void *(*pfn)(void *, const char *, size_t),
		   void *arg, const char *fmt, va_list ap)
{	/* print formatted */
  _Pft x;
  
  x.nchar = 0;
  while (1) {	/* scan format string */
    const char *s = fmt;
    int c;
    const char *t;
    static const char fchar[] = {" +-#0"};
    static const unsigned int fbit[] = {
      _FSP, _FPL, _FMI, _FNO, _FZE, 0};
    char ac[32];
    
    /* copy any literal text */

    while ( (0 < (c = (int)My_ReadByte(s++)) ) && (c != '%')) {
    }
    --s;
#if 1
    PUT(fmt, s - fmt);
#else
    if (0 < (s-fmt)) {
      if ((arg = (*pfn)(arg, fmt, s-fmt)) != NULL) 
	x.nchar += (s-fmt);
      else
	return (x.nchar);
    }

#endif
    if (c == '\0')
      return (x.nchar);
    fmt = ++s;
    
    /* parse a conversion specifier */
    for (x.flags = 0; (t = strchr(fchar, *s)) != NULL; ++s)
      x.flags |= fbit[t - fchar];
    if (My_ReadByte(s) == '*') {	/* get width argument */
      x.width = va_arg(ap, int);
      if (x.width < 0) {	/* same as '-' flag */
	x.width = -x.width;
	x.flags |= _FMI;
      }
      ++s;
    } else	/* accumulate width digits */
      for (x.width = 0; ISDIGIT((int)*s); ++s)
	if (x.width < _WMAX)
	  x.width = x.width * 10 + *s - '0';
    if (My_ReadByte(s) != '.')
      x.prec = -1;
    else if ( My_ReadByte(++s) == '*') {	/* get precision argument */
      x.prec = va_arg(ap, int);
      ++s;
    } else	/* accumulate precision digits */
      for (x.prec = 0; ISDIGIT(*s); ++s)
	if (x.prec < _WMAX)
	  x.prec = x.prec * 10 + *s - '0';
    My_WriteByte(&(x.qual), 
		 (char)(strchr("hlL", My_ReadByte(s)) ? My_ReadByte(s++) : '\0'));
    if ((My_ReadByte(&(x.qual)) == 'l') && (My_ReadByte(s) == 'l')) {
      My_WriteByte(&(x.qual), 'L');	/* the %ll qualifier */
      s++;
    }
    
    /* do the conversion */
    _Putfld(&x, &ap, My_ReadByte(s), ac);
    x.width -= x.n0 + x.nz0 + x.n1 + x.nz1 + x.n2 + x.nz2;
    if (!(x.flags & _FMI))
      PAD(spaces, x.width);
    PUT(ac, x.n0);
    PAD(zeroes, x.nz0);
    PUT(x.s, x.n1);
    PAD(zeroes, x.nz1);
    PUT(x.s + x.n1, x.n2);
    PAD(zeroes, x.nz2);
    if (x.flags & _FMI)
      PAD(spaces, x.width);
    fmt = s + 1;
  }
  /* shouldn't reach here, only used to eliminate the compiler warning */
  return 0;
}

static void _Putfld(_Pft *px, va_list *pap, char code, char *ac)
{	/* convert a field for _Printf */
  
  px->n0 = px->nz0 = px->n1 = px->nz1 = px->n2 = px->nz2 = 0;
  switch (code) {	/* switch on conversion specifier */
  case 'c':
    My_WriteByte(&(ac[px->n0++]), (char)va_arg(*pap, int));
    break;
  case 'd':
  case 'i':	/* convert a signed decimal integer */
    if (px->qual == 'l')
      px->v.ll = va_arg(*pap, long);
    else if (px->qual == 'L')
      px->v.ll = va_arg(*pap, long long);
    else
      px->v.ll = va_arg(*pap, int);
    if (My_ReadByte(&(px->qual)) == 'h')
      px->v.ll = (short) px->v.ll;
    if (px->v.ll < 0)	/* negate safely in miya_Litob */
      My_WriteByte(&(ac[px->n0++]), '-');
    else if (px->flags & _FPL)
      My_WriteByte(&(ac[px->n0++]),'+');
    else if (px->flags & _FSP)
      My_WriteByte(&(ac[px->n0++]),' ');
    px->s = &ac[px->n0];
    miya_Litob(px, code);
    break;
  case 'o':
  case 'u':
  case 'x':
  case 'X':	/* convert unsigned */
    if (px->qual == 'l')
      px->v.ll = va_arg(*pap, long);
    else if (px->qual == 'L')
      px->v.ll = va_arg(*pap, long long);
    else
      px->v.ll = va_arg(*pap, int);
    if (px->qual == 'h')
      px->v.ll = (unsigned short) px->v.ll;
    else if (px->qual == '\0')
      px->v.ll = (unsigned int) px->v.ll;
    if (px->flags & _FNO) {	/* indicate base with prefix */
      My_WriteByte(&(ac[px->n0++]),'0');
      if (code == 'x' || code == 'X')
	My_WriteByte(&(ac[px->n0++]), code);
    }
    px->s = &ac[px->n0];
    miya_Litob(px, code);
    break;
  case 'e':
  case 'E':
  case 'f':
  case 'g':
  case 'G':	/* convert floating */
    px->v.ld = px->qual == 'L' ?
      va_arg(*pap, ldouble) : va_arg(*pap, double);
    if (LDSIGN(px->v.ld))
      My_WriteByte(&(ac[px->n0++]), '-');
    else if (px->flags & _FPL)
      My_WriteByte(&(ac[px->n0++]), '+');
    else if (px->flags & _FSP)
      My_WriteByte(&(ac[px->n0++]), ' ');
    px->s = &ac[px->n0];
    miya_Ldtob(px, code);
    break;
  case 'n':	/* return output count */
    if (px->qual == 'h')
      *va_arg(*pap, short *) = (short)(px->nchar);
    else if (px->qual == 'l')
      *va_arg(*pap, long *) = px->nchar;
    else if (px->qual == 'L')
      *va_arg(*pap, long long *) = px->nchar;
    else
      *va_arg(*pap, int *) = px->nchar;
    break;
  case 'p':	/* convert a pointer, hex long version */
    px->v.ll = (long) va_arg(*pap, void *);
    px->s = &ac[px->n0];
    miya_Litob(px, 'x');
    break;
  case 's':	/* convert a string */
    px->s = va_arg(*pap, char *);
    px->n1 = strlen(px->s);
    if (0 <= px->prec && px->prec < px->n1)
      px->n1 = px->prec;
    break;
  case '%':	/* put a '%' */
    My_WriteByte(&(ac[px->n0++]),'%');
    break;
  default:	/* undefined specifier, print it out */
    My_WriteByte(&(ac[px->n0++]), code);
    break;
  }
}

/*****************************************************************/

/* miya_Litob function */


/* IEEE 754 properties */
#define _DFRAC  ((1<<_DOFF)-1)
#define _DMASK  (0x7fff&~_DFRAC)
#define _DMAX   ((1<<(15-_DOFF))-1)
#define _DNAN   (0x8000|_DMAX<<_DOFF|1<<(_DOFF-1))
#define _DSIGN  0x8000
#define DSIGN(x)        (((unsigned short *)&(x))[_D0] & _DSIGN)
#define HUGE_EXP        (int)(_DMAX * 900L / 1000)
#define HUGE_RAD        3.14e30
#define SAFE_EXP        (_DMAX>>1)

/* word offsets within double */
#if _D0==3
#define _D1     2       /* little-endian order */
#define _D2     1
#define _D3     0
#else
#define _D1     1       /* big-endian order */
#define _D2     2
#define _D3     3
#endif

/* return values for _D functions */
#define FINITE  -1
#define INF     1
#define NAN     2

static char ldigs[] = "0123456789abcdef";
static char udigs[] = "0123456789ABCDEF";

static void miya_Litob(_Pft *px, char code)
{	/* convert unsigned long to text */
  char ac[24];	/* safe for 64-bit integers */
  char *digs = code == 'X' ? udigs : ldigs;
  int base = code == 'o' ? 8 : code != 'x' && code != 'X' ? 10 : 16;
  int i = sizeof(ac);
  unsigned long long ullval = px->v.ll;
  
  if ((code == 'd' || code == 'i') && px->v.ll < 0)
    ullval = -ullval;	/* safe against overflow */
  if (ullval || px->prec)
    My_WriteByte(&(ac[--i]), My_ReadByte(&(digs[ullval % base])));
  px->v.ll = ullval / base;
  while (0 < px->v.ll && 0 < i) {	/* convert digits */
    miya_lldiv_t qr = miya_lldiv(px->v.ll, (long long) base);
    
    px->v.ll = qr.quot;
    My_WriteByte(&(ac[--i]), My_ReadByte(&(digs[qr.rem])));
  }
  px->n1 = sizeof(ac) - i;
  (void)memcpy(px->s, &ac[i], px->n1);
  if (px->n1 < px->prec)
    px->nz0 = px->prec - px->n1;
  if (px->prec < 0 && (px->flags & (_FMI | _FZE)) == _FZE
      && 0 < (i = px->width - px->n0 - px->nz0 - px->n1))
    px->nz0 += i;
  
}


/*************************************************/
/* miya_Ldtob function */

#include "float.h"

/* macros */
#define NDIG	8

/* static data */
static const ldouble pows[] = {
  1e1L, 1e2L, 1e4L, 1e8L, 1e16L, 1e32L,
#if 0x100 < _LBIAS	/* assume IEEE 754 8- or 10-byte */
  1e64L, 1e128L, 1e256L,
#if _DLONG	/* assume IEEE 754 10-byte */
  1e512L, 1e1024L, 1e2048L, 1e4096L,
#endif
#endif
};

static short _Ldunscale(short *, ldouble *);
static void _Genld(_Pft *, char, char *, short, short);

static void miya_Ldtob(_Pft *px, char code)
{	/* convert long double to text */
  char ac[32];
  char *p = ac;
  ldouble ldval = px->v.ld;
  short errx, nsig, xexp;
  
  if (px->prec < 0)
    px->prec = 6;
  else if (px->prec == 0 && (code == 'g' || code == 'G'))
    px->prec = 1;
  if (0 < (errx = _Ldunscale(&xexp, &px->v.ld))) {
    /* x == Nan, x == INF */
    (void)memcpy(px->s, errx == NAN ? "NaN" : "Inf", px->n1 = 3);
    return;
  } else if (0 == errx)	/*x == 0 */
    nsig = 0, xexp = 0;
  else {	/* 0 < |x|, convert it */
    {	/* scale ldval to ~~10^(NDIG/2) */
      int i, n;
      
      if (ldval < 0.0)
	ldval = -ldval;
      if ((xexp = (short)(xexp * 30103L / 100000L - NDIG/2)) < 0) {
	/* scale up */
	n = (-xexp + (NDIG/2-1)) & ~(NDIG/2-1), xexp = (short)(-n);
	for (i = 0; 0 < n; n >>= 1, ++i)
	  if (n & 1)
	    ldval *= pows[i];
      } else if (0 < xexp) {	/* scale down */
	ldouble factor = 1.0;
	
	xexp &= ~(NDIG/2-1);
	for (n = xexp, i = 0; 0 < n; n >>= 1, ++i)
	  if (n & 1)
	    factor *= pows[i];
	ldval /= factor;
      }
    }
    {	/* convert significant digits */
      int gen = px->prec +
	(code == 'f' ? xexp + 2 + NDIG : 2 + NDIG / 2);
      
      if (LDBL_DIG + NDIG / 2 < gen)
	gen = LDBL_DIG + NDIG / 2;
      for (My_WriteByte(p++, '0') ; 0 < gen && 0.0 < ldval; p += NDIG) {
	/* convert NDIG at a time */
	int j;
	long lo = (long) ldval;
	
	if (0 < (gen -= NDIG))
	  ldval = (ldval - (ldouble) lo) * 1e8L;
	for (p += NDIG, j = NDIG; 0 < lo && 0 <= --j; ) {
	  /* convert NDIG digits */
	  miya_ldiv_t qr;
	  
	  qr = miya_ldiv(lo, 10);
	  My_WriteByte(--p , (char)(qr.rem + '0')), lo = qr.quot;
	}
	while (0 <= --j)
	  My_WriteByte(--p , '0');
      }
      gen = p - &ac[1];
      for (p = &ac[1], xexp += NDIG - 1; My_ReadByte(p) == '0'; ++p)
	--gen, --xexp;	/* correct xexp */

      /* miya
	 char code 
	 int prec
	 short nsig, xexp;
       */
      nsig = (short)(px->prec + (code == 'f' ? xexp + 1 :
			 code == 'e' || code == 'E' ? 1 : 0));
      if (gen < nsig)
	nsig = (short)gen;
      if (0 < nsig) {	/* round and strip trailing zeros */
	const char drop;
	int n;
	My_WriteByte((void *)&drop, (char)(nsig < gen && '5' <= My_ReadByte(&(p[nsig])) ? '9' : '0') );
	
	for (n = nsig; My_ReadByte(&(p[--n])) == My_ReadByte(&drop); )
	  --nsig;

	if (My_ReadByte(&drop) == '9') {
	  My_WriteByte(&(p[n]), My_ReadByte(&(p[n])+1));
	  
	  //  ++p[n];
	}
	if (n < 0)
	  --p, ++nsig, ++xexp;
      }
    }
  }
  _Genld(px, code, p, nsig, xexp);
}

#if _DLONG	/* 10-byte IEEE format */
#define _LMASK	0x7fff
#define _LMAX	0x7fff
#define _LSIGN	0x8000
#if _D0==3	/* little-endian order */
#define _L0	4
#define _L1	3
#define _L2	2
#define _L3	1
#define _L4	0
#else
#define _L0	0
#define _L1	1
#define _L2	2
#define _L3	3
#define _L4	4
#endif

static short dnorm(unsigned short *ps)
{	/* normalize long double fraction */
  short xchar;
  
  for (xchar = 0; ps[_L1] == 0; xchar -= 16) {	/* shift left by 16 */
    ps[_L1] = ps[_L2], ps[_L2] = ps[_L3];
    ps[_L3] = ps[_L4], ps[_L4] = 0;
  }
  for (; ps[_L1] < 1U<<_LOFF; --xchar) {	/* shift left by 1 */
    ps[_L1] = ps[_L1] << 1 | ps[_L2] >> 15;
    ps[_L2] = ps[_L2] << 1 | ps[_L3] >> 15;
    ps[_L3] = ps[_L3] << 1 | ps[_L4] >> 15;
    ps[_L4] <<= 1;
  }
  return (xchar);
}

static short _Ldunscale(short *pex, ldouble *px)
{	/* separate *px to |frac| < 1/2 and 2^*pex */
  unsigned short *ps = (unsigned short *) px;
  short xchar = ps[_L0] & _LMAX;
  
  if (xchar == _LMAX) {	/* NaN or INF */
    *pex = 0;
    return (ps[_L1] & 0x7fff || ps[_L2] || ps[_L3] || ps[_L4] ?
	    NAN : INF);
  } else if (ps[_L1] == 0 && ps[_L2] == 0 && ps[_L3] == 0 && ps[_L4] == 0) {
    /* zero */
    *pex = 0;
    return (0);
  } else {	/* finite, reduce to [1/2, 1) */
    xchar += dnorm(ps);
    ps[_L0] = ps[_L0] & _LSIGN | _LBIAS;
    *pex = xchar - _LBIAS;
    return (FINITE);
  }
}

#else	/* long double same as double */
/*
  static short _Dnorm(unsigned short *ps)
  {
  short xchar;
  unsigned short sign = ps[_D0] & _DSIGN;
  
  xchar = 0;
  if ((ps[_D0] &= _DFRAC) != 0 || ps[_D1] || ps[_D2] || ps[_D3]) {
  for (; ps[_D0] == 0; xchar -= 16) {
  ps[_D0] = ps[_D1], ps[_D1] = ps[_D2];
  ps[_D2] = ps[_D3], ps[_D3] = 0;
  }
  for (; ps[_D0] < 1<<_DOFF; --xchar) {
  ps[_D0] = ps[_D0] << 1 | ps[_D1] >> 15;
  ps[_D1] = ps[_D1] << 1 | ps[_D2] >> 15;
  ps[_D2] = ps[_D2] << 1 | ps[_D3] >> 15;
  ps[_D3] <<= 1;
  }
  for (; 1<<_DOFF+1 <= ps[_D0]; ++xchar) {
  ps[_D3] = ps[_D3] >> 1 | ps[_D2] << 15;
  ps[_D2] = ps[_D2] >> 1 | ps[_D1] << 15;
  ps[_D1] = ps[_D1] >> 1 | ps[_D0] << 15;
  ps[_D0] >>= 1;
  }
  ps[_D0] &= _DFRAC;
  }
  ps[_D0] |= sign;
  return (xchar);
  }
  */
static short _Ldunscale(short *pex, ldouble *px)
{	/* separate *px to |frac| < 1/2 and 2^*pex */
  unsigned short *ps = (unsigned short *) px;
  short xchar = (short)((ps[_D0] & _DMASK) >> _DOFF);
  
  if (xchar == _DMAX) {	/* NaN or INF */
    *pex = 0;
    return (short)(ps[_D0] & _DFRAC || ps[_D1] || ps[_D2] || ps[_D3] ?
	    NAN : INF);
  } else if (0 < xchar /* || (xchar = _Dnorm(ps)) != 0 */) {
    /* finite, reduce to [1/2, 1) */
    ps[_D0] = (short)(ps[_D0] & ~_DMASK | _DBIAS << _DOFF);
#if _LONG_DOUBLE
    *pex = (short)(xchar - _DBIAS);
#else
    *pex = (short)(xchar - _DBIAS + 1);	/* for SGI */
#endif
    return (FINITE);
  } else if (xchar < 0) {	/* error! */
    return (NAN);
  } else {	/* zero */
    *pex = 0;
    return (0);
  }
}

#endif

static void _Genld(_Pft *px, char code, char *p, short nsig, short xexp)
{	/* generate long double text */
  const char point = '.';
  
  if (nsig <= 0)
    nsig = 1, p = "0";
  if (code == 'f' || (code == 'g' || code == 'G') &&
      -4 <= xexp && xexp < px->prec) {	/* 'f' format */
    ++xexp;		/* change to leading digit count */
    if (code != 'f') {	/* fixup for 'g' */
      if (!(px->flags & _FNO) && nsig < px->prec)
	px->prec = nsig;
      if ((px->prec -= xexp) < 0)
	px->prec = 0;
    }
    if (xexp <= 0) {	/* digits only to right of point */
      My_WriteByte(&(px->s[px->n1++]), '0');

      if (0 < px->prec || px->flags & _FNO)
	My_WriteByte(&(px->s[px->n1++]), point);
      if (px->prec < -xexp)
	xexp = (short)(-px->prec);
      px->nz1 = -xexp;
      px->prec += xexp;
      if (px->prec < nsig)
	nsig = (short)(px->prec);
      (void)memcpy(&px->s[px->n1], p, px->n2 = nsig);
      px->nz2 = px->prec - nsig;
    } else if (nsig < xexp) {	/* zeros before point */
      (void)memcpy(&px->s[px->n1], p, nsig);
      px->n1 += nsig;
      px->nz1 = xexp - nsig;
      if (0 < px->prec || px->flags & _FNO)
	My_WriteByte(&(px->s[px->n1]), point), ++px->n2;
      px->nz2 = px->prec;
    } else {	/* enough digits before point */
      (void)memcpy(&px->s[px->n1], p, xexp);
      px->n1 += xexp;
      nsig -= xexp;
      if (0 < px->prec || px->flags & _FNO)
	My_WriteByte(&(px->s[px->n1++]),point);
      if (px->prec < nsig)
	nsig = (short)(px->prec);
      (void)memcpy(&px->s[px->n1], p + xexp, nsig);
      px->n1 += nsig;
      px->nz1 = px->prec - nsig;
    }
  } else {	/* 'e' format */
    if (code == 'g' || code == 'G') {	/* fixup for 'g' */
      if (nsig < px->prec)
	px->prec = nsig;
      if (--px->prec < 0)
	px->prec = 0;
      code = (char)(code == 'g' ? 'e' : 'E');
    }
    My_WriteByte(&(px->s[px->n1++]), My_ReadByte(p++));
    if (0 < px->prec || px->flags & _FNO)
      My_WriteByte(&(px->s[px->n1++]), point);
    if (0 < px->prec) {	/* put fraction digits */
      if (px->prec < --nsig)
	nsig = (short)(px->prec);
      (void)memcpy(&px->s[px->n1], p, nsig);
      px->n1 += nsig;
      px->nz1 = px->prec - nsig;
    }
    p = &px->s[px->n1];	/* put exponent */
    My_WriteByte(&p, code);
    p++;
    if (0 <= xexp)
      My_WriteByte(p++, '+');
    else {	/* negative exponent */
      My_WriteByte(p++, '-');
      xexp = (short)(-xexp);
    }
    if (100 <= xexp) {	/* put oversize exponent */
      if (1000 <= xexp)
	My_WriteByte(p, (char)(xexp / 1000 + '0')),p++, xexp %= 1000;
      My_WriteByte(p, (char)(xexp / 100 + '0')), p++, xexp %= 100;
    }
    My_WriteByte(p, (char)(xexp / 10 + '0')),p++, xexp %= 10;
    My_WriteByte(p, (char)(xexp + '0')); p++;
    px->n2 = p - &px->s[px->n1];
  }
  if ((px->flags & (_FMI | _FZE)) == _FZE) {	/* pad with leading zeros */
    int n = px->n0 + px->n1 + px->nz1 + px->n2 + px->nz2;
    
    if (n < px->width)
      px->nz0 = px->width - n;
  }
}
