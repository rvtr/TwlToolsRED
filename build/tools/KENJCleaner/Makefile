#! make -f
#----------------------------------------------------------------------------
# Project:  Hello World!
# File:     Makefile
#
# Copyright 2003,2004 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $NoKeywords: $
#----------------------------------------------------------------------------

TARGET_PLATFORM	:= TWL
TWL_ARCHGEN		:= LIMITED

SUBDIRS		=	banner

#----------------------------------------------------------------------------

SRCS		=	main.c myFunc.c myChar.c
ROM_SPEC	=	demo.rsf

TARGET_BIN	=	KENJCleaner.srl

include	$(TWLSDK_ROOT)/build/buildtools/commondefs

MAKEROM_ARM7_BASE	=	$(TWL_COMPONENTSDIR)/armadillo/$(TWL_BUILDTYPE_ARM7)/armadillo
MAKEROM_ARM7		=	$(MAKEROM_ARM7_BASE).$(TWL_ELF_EXT)
MAKEROM				=	$(TWL_TOOLSDIR)/bin/makerom.TWL.secure.exe

GLIBRARIES			+=	\
						liblcfg$(TWL_LIBSUFFIX).a \
						libes$(TWL_LIBSUFFIX).a		\
						libboc$(TWL_LIBSUFFIX).a	\
						libsfs$(TWL_LIBSUFFIX).a	\
						libnam$(TWL_LIBSUFFIX).a	\
						libsea$(TWL_LIBSUFFIX).a	\

#----------------------------------------------------------------------------

do-build:		$(TARGETS)

include	$(TWLSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
