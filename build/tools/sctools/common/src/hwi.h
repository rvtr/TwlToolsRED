/*---------------------------------------------------------------------------*
  Project:  TwlIPL
  File:     DS_Chat.h

  Copyright 2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-03-31#$
  $Rev: 1019 $
  $Author: kamikawa $
 *---------------------------------------------------------------------------*/

#ifndef	__HWI_LIB__
#define	__HWI_LIB__

#ifdef __cplusplus
extern "C" {
#endif


#include <twl.h>

#include <twl/lcfg/common/api.h>
#include <twl/lcfg/common/TWLSettings.h>

#include "TWLHWInfo_api.h"
#include "TWLSettings_api.h"


// define data----------------------------------------------------------

BOOL MiyaBackupHWNormalInfo(const char *path);
BOOL MiyaRestoreHWNormalInfo(const char *path);
BOOL MiyaBackupTWLSettings(const char *path);
BOOL MiyaRestoreTWLSettings(const char *path);
BOOL MiyaReadHWNormalInfo( LCFGTWLHWNormalInfo *Info );
BOOL MiyaReadHWSecureInfo( LCFGTWLHWSecureInfo *Info );
BOOL MiyaReadHWNormalInfo_From_SD(const char *path,   LCFGTWLHWNormalInfo *info);



#ifdef __cplusplus
}
#endif

#endif  // __HWI_LIB__
