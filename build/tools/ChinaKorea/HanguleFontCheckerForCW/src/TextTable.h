#ifndef TEXTTABLE_H_INCLUDE
#define TEXTTABLE_H_INCLUDE

#define NNS_G2D_UNICODE			// •¶š—ñ•`‰æŠÖ”‚ªˆµ‚¤•¶š‚ğUnicode‚É‚·‚é
#include <nitro.h>
#include <nnsys/g2d/g2d_TextCanvas.h>
#include <nnsys/g2d/g2d_CharCanvas.h>

typedef const NNSG2dChar * Texts[3];
extern const Texts 		 g_TextTypes;
extern const Texts 		 g_TextTable[];
extern const int 	     g_TextTable_Num;

#endif // TEXTTABLE_H_INCLUDE
