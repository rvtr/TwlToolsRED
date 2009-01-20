/*---------------------------------------------------------------------------*
  Project:  NITRO-System - build - demos - g2d - Text - textdemolib
  File:     cmn.h

  Copyright 2005 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: cmn.h,v $
  Revision 1.1  2005/05/20 05:52:37  hatamoto_minoru
  NitroSystem への登録。

  Revision 1.5  2005/03/10 08:18:55  hatamoto_minoru
  API の変更に対応。
  コメントの追加。

  Revision 1.4  2005/02/08 04:06:59  hatamoto_minoru
  ファイルヘッダの修正。

  Revision 1.3  2005/02/01 11:52:06  hatamoto_minoru
  typo修正。

  Revision 1.2  2005/01/25 01:49:34  hatamoto_minoru
  デバック用CallTraceを無効に。

  Revision 1.1  2005/01/21 01:03:09  hatamoto_minoru
  TextDraw への登録。

  Revision 1.4  2004/11/30 09:18:25  hatamoto_minoru
  include が足りなかったので追加。

  Revision 1.3  2004/09/29 08:46:57  hatamoto_minoru
  ファイル操作関数、時間定数の追加。

  Revision 1.2  2004/09/15 08:57:52  hatamoto_minoru
  CMN_LoadFile の追加。

  Revision 1.1  2004/09/10 11:21:25  hatamoto_minoru
  初版の登録。

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#ifndef CMN_H_
#define CMN_H_


#include <nitro/types.h>
#include <nitro/os/common/interrupt.h>
#include <nitro/os/common/arena.h>
#include <nitro/os/common/alloc.h>
#include <nnsys/fnd/allocator.h>
#include <nnsys/fnd/archive.h>

#ifdef __cplusplus
extern "C" {
#endif



#define ARY_SIZEOF(ary)     ( sizeof(ary) / sizeof( (ary)[0] ) )
#define ROUNDUP_DIV(a, b)   (( (a) + ((b) - 1) ) / (b))

const static float ONE_FRAME_US = 16715.1;  // 1フレームは 16715.1 us
const static float ONE_VBLANK_US = 4512.4;  // Vブランク期間は 4512.4 us


typedef struct CMNGamePad
{
    u16     trigger;
    u16     release;
    u16     button;
}
CMNGamePad;



extern CMNGamePad CMNGamePadState;



void CMN_InitInterrupt( void );
void CMN_BeginHVBlankIntr( OSIrqFunction hBlankFunc , OSIrqFunction vBlankFunc );
void CMN_InitAllocator( NNSFndAllocator* pAllocator );
void CMN_InitFileSystem( NNSFndAllocator* pAllocator );
void CMN_ClearVram( void );
void CMN_ReadGamePad(void);
u32 CMN_LoadFile(void** ppFile, const char* fpath, NNSFndAllocator* pAlloc);
void CMN_UnloadFile(void* pFile, NNSFndAllocator* pAlloc);
NNSFndArchive* CMN_LoadArchive(const char* name, const char* path, NNSFndAllocator* pAllocator);
void CMN_RemoveArchive(NNSFndArchive* archive, NNSFndAllocator* pAllocator);


static inline u16 CMN_IsTrigger(u16 key)
{
    return (u16)(CMNGamePadState.trigger & key);
}
static inline u16 CMN_IsRelease(u16 key)
{
    return (u16)(CMNGamePadState.release & key);
}
static inline u16 CMN_IsPress(u16 key)
{
    return (u16)(CMNGamePadState.button & key);
}

static inline void CMN_WaitVBlankIntr(void)
{
    OS_WaitIrq(TRUE, OS_IE_V_BLANK);
}

static inline void CMN_SetPlaneVisible(GXPlaneMask plane)
{
    GX_SetVisiblePlane(GX_GetVisiblePlane() | plane);
}

static inline void CMN_SetPlaneInvisible(GXPlaneMask plane)
{
    GX_SetVisiblePlane(GX_GetVisiblePlane() & ~plane);
}

static inline void CMN_SetPlaneVisibleSub(GXPlaneMask plane)
{
    GXS_SetVisiblePlane(GXS_GetVisiblePlane() | plane);
}

static inline void CMN_SetPlaneInvisibleSub(GXPlaneMask plane)
{
    GXS_SetVisiblePlane(GXS_GetVisiblePlane() & ~plane);
}





#ifdef __cplusplus
}/* extern "C" */
#endif

#endif // CMN_H_

