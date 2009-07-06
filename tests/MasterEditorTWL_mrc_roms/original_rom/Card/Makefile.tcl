#! make -f
#----------------------------------------------------------------------------
# Project:  TwlSDK - tests - appjumpTest - Card
# File:     Makefile
#
# Copyright 2008 Nintendo.  All rights reserved.
#
# These coded instructions, statements, and computer programs contain
# proprietary information of Nintendo of America Inc. and/or Nintendo
# Company Ltd., and are protected by Federal copyright law.  They may
# not be disclosed to third parties or copied or duplicated in any form,
# in whole or in part, without the prior written consent of Nintendo.
#
# $Date::            $
# $Rev$
# $Author$
#----------------------------------------------------------------------------

TARGET_PLATFORM := TWL

SUBDIRS		= 	../common/banner \
				$(ROOT)/build/demos/gx/UnitTours/DEMOLib

TARGET_BIN	= tcl_card.srl
#INCDIR		= ../common/include
SRCDIR		= ../common/src
SRCS		= main_tcl.c

ROM_SPEC	=	./ROM-TS.rsf

#----------------------------------------------------------------------------

include $(TWLSDK_ROOT)/build/buildtools/commondefs
include	$(TWLSDK_ROOT)/build/buildtools/commondefs.gx.demolib

# �C���X�g�[���w��
#ifneq	($(TWL_IPL_RED_ROOT),)
#INSTALL_DIR	= $(TWL_IPL_RED_ROOT)/debugsoft/CardRomHeaderChecker
#INSTALL_TARGETS	= $(BINDIR)/$(TARGET_BIN)
#endif

TWL_LIBS_EX		+=	libtcl$(TWL_LIBSUFFIX).a

do-build:	$(TARGETS)

#----------------------------------------------------------------------------

LIBSYSCALL			 = ../common/0CZA/libsyscall.a

include	$(TWLSDK_ROOT)/build/buildtools/modulerules

#===== End of Makefile =====
