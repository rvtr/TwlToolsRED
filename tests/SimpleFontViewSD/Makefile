#! make -f
#----------------------------------------------------------------------------
# Project:  NITRO-System - demos - g2d - Text - SimpleFontView
# File:     Makefile
#
# Copyright 2004-2008 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Revision$
#----------------------------------------------------------------------------

SUBDIRS		=	banner textdemolib

NNS_USELIBS     = g2d gfd fnd

G2D_TEXTDEMOLIB	=	./textdemolib

LINCLUDES       =   $(G2D_TEXTDEMOLIB)/include
LLIBRARY_DIRS   =   $(G2D_TEXTDEMOLIB)/lib/$(NITRO_BUILDTYPE)
LLIBRARIES      =   libg2d_textdemo.a

LIBSYSCALL		=	./030A/libsyscall.a

TWL_NANDAPP		=	TRUE

TITLEID_LO				=	030A

#----------------------------------------------------------------------------

SRCS		=	main.c
TARGET_BIN	=	SimpleFontViewSD.tad

LDEPENDS_NEF = data/fontd.NFTR

MAKEROM_ROMROOT  = ./
MAKEROM_ROMFILES = data/fontd.NFTR

ROM_SPEC				=	./ROM-TS_custom.rsf

#----------------------------------------------------------------------------
include	$(NITROSYSTEM_ROOT)/build/buildtools/commondefs


do-build:		$(TARGETS)


include $(TWLSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
