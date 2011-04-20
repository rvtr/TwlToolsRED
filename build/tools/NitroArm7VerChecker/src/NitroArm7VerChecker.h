/*---------------------------------------------------------------------------*
  Project:  NitroArm7VerChecker
  File:     CooperationA.h

  Copyright 2007 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date::            $
  $Rev$
  $Author$
 *---------------------------------------------------------------------------*/

#ifndef	__NITROARM7VERCHECKER_H__
#define	__NITROARM7VERCHECKER_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <twl.h>

// äàê¸ë}î≤Ç©ÇÁÇÃÉpÉNÇË
#define SLOT_STATUS_MODE_10                                0x08
#define REG_MC1_OFFSET                                     0x4010
#define REG_MC1_ADDR                                       (HW_REG_BASE + REG_MC1_OFFSET)
#define reg_MI_MC1                                         (*( REGType32v *) REG_MC1_ADDR)

// define data----------------------------------------------------------
void NitroArm7VerCheckerInit( void );
void NitroArm7VerCheckerMain( void );

#ifdef __cplusplus
}
#endif

#endif  // __NITROARM7VERCHECKER_H__
