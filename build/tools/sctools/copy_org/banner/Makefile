#! make -f
#----------------------------------------------------------------------------
# Project:  TwlSDK - nandApp - demos - SubBanner
# File:     Makefile
#
# Copyright 2007 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Date:: 2008-06-27#$
# $Rev: 6887 $
# $Author: nakasima $
#----------------------------------------------------------------------------
TARGET_PLATFORM		= TWL

include $(TWLSDK_ROOT)/build/buildtools/commondefs
MAKEBANNER			= $(TWL_TOOLSDIR)/bin/makebanner.TWL.exe

ICON_DIR			= ./icon

BANNER_ICON			= $(ICON_DIR)/gameIcon.bmp
BANNER_SPEC			= banner_v3.bsf

SUB_BANNER_ICON			= $(ICON_DIR)/subIcon.bmp
SUB_BANNER_SPEC			= sub_banner_v3.bsf


TARGETS				= banner.bnr
SUB_TARGETS			= sub_banner.bnr
INSTALL_DIR			= ./
INSTALL_TARGETS		= $(TARGETS) $(SUB_TARGETS)

BANNER_ICON_NAME	= $(basename $(BANNER_ICON))
BANNER_ICON_MIDDLE	= $(addprefix $(BANNER_ICON_NAME), .nbfs .nbfc .nbfp)
SUB_BANNER_ICON_NAME = $(basename $(SUB_BANNER_ICON))
SUB_BANNER_ICON_MIDDLE = $(addprefix $(SUB_BANNER_ICON_NAME), .nbfs .nbfc .nbfp)

LDIRT_CLEAN			= $(TARGETS) $(SUB_TARGETS) \
					  $(BANNER_ICON_MIDDLE) \
					  $(SUB_BANNER_ICON_MIDDLE) \
					  $(TARGETS:.bnr=.srl)

include $(TWLSDK_ROOT)/build/buildtools/modulerules

#----------------------------------------------------------------------------
#  build
#----------------------------------------------------------------------------
do-build:		$(TARGETS) $(SUB_TARGETS)

$(TARGETS):		$(BANNER_SPEC) $(BANNER_ICON) $(BANNER_ICON_MIDDLE)
				$(MAKEBANNER) -N $(BANNER_ICON_NAME) $(BANNER_SPEC) $(TARGETS)

$(SUB_TARGETS):	$(SUB_BANNER_SPEC) $(SUB_BANNER_ICON) $(SUB_BANNER_ICON_MIDDLE)
				$(MAKEBANNER) -s -N $(SUB_BANNER_ICON_NAME) $(SUB_BANNER_SPEC) $(SUB_TARGETS)
#
