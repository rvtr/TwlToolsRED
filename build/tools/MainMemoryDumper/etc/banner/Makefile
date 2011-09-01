#! make -f
#---------------------------------------------------------------------------
#  Project:  TwlSDK - tools
#  File:     Makefile
#
#  Copyright 2003-2008 Nintendo.  All rights reserved.
#
#  These coded instructions, statements, and computer programs contain
#  proprietary information of Nintendo of America Inc. and/or Nintendo
#  Company Ltd., and are protected by Federal copyright law.  They may
#  not be disclosed to third parties or copied or duplicated in any form,
#  in whole or in part, without the prior written consent of Nintendo.
#
#  $Date::            $
#  $Rev$
#  $Author$
#---------------------------------------------------------------------------

TARGET_PLATFORM	 = NITRO TWL

include $(TWLSDK_ROOT)/build/buildtools/commondefs

ifneq ($(filter TWL,$(TARGET_PLATFORM_LIST)),)
BANNER_ICON		+= myGameIcon.TWL.bin
BANNER_SPEC		+= myGameBanner.TWL.bsf
endif
ifneq ($(filter NITRO,$(TARGET_PLATFORM_LIST)),)
BANNER_ICON		+= myGameIcon.bmp
BANNER_SPEC		+= myGameBanner.bsf
endif


TARGETS			 = $(BANNER_SPEC:.bsf=.bnr)
INSTALL_DIR		 = ..
INSTALL_TARGETS	 = $(TARGETS)

BANNER_ICON_NAME = $(basename $(BANNER_ICON))
BANNER_ICON_MIDDLE	= $(addprefix $(BANNER_ICON_NAME), .nbfs .nbfc .nbfp)

LDIRT_CLEAN		 = $(TARGETS) \
				   $(BANNER_ICON_MIDDLE) \
				   $(TARGETS:.bnr=.srl)

include $(TWLSDK_ROOT)/build/buildtools/modulerules

#----------------------------------------------------------------------------
#  build
#----------------------------------------------------------------------------
do-build:		$(TARGETS)

ifeq ($(TARGET_PLATFORM),TWL)
$(TARGETS):		$(BANNER_SPEC) $(BANNER_ICON)
			$(MAKEBANNER) -p TWL $(BANNER_SPEC) $(TARGETS)
else
$(TARGETS):		$(BANNER_SPEC) $(BANNER_ICON) $(BANNER_ICON_MIDDLE)
			$(MAKEBANNER) -N $(BANNER_ICON_NAME) $(BANNER_SPEC) $(TARGETS)
endif

#----------------------------------------------------------------------------
#  test
#----------------------------------------------------------------------------
ifeq ($(TARGET_PLATFORM),TWL)
include $(TWLSDK_ROOT)/build/buildtools/twl/commondefs.emtype.IS
else
include $(TWLSDK_ROOT)/build/buildtools/nitro/commondefs.emtype.IS
endif

test:			$(TARGETS)
			$(ADDBANNER) $(TARGETS) $(TARGETS:.bnr=.srl)
ifdef	ISD_NITLOAD
			$(ISD_NITLOAD) $(TARGETS:.bnr=.srl)
endif

#
