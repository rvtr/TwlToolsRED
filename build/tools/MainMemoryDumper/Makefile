#! make -f
#----------------------------------------------------------------------------
# Project:  TwlSDK - GX - demos - tips/BuildFinalRom
# File:	    Makefile
#
# Copyright 2003,2004 Nintendo.	 All rights reserved.
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

TARGET_PLATFORM	= TWL

SUBDIRS			= etc/banner


#----------------------------------------------------------------------------

SRCS			= main.c data.c misc_simple.c mmdumper.c mmd_parent.c mmd_client.c \
			  $(ROOT)/build/demos/wireless_shared/wh/wh.c

TARGET_BIN		= MainMemoryDumper.srl
LINCLUDES		= $(ROOT)/build/demos/gx/UnitTours/DEMOLib/include \
			  $(ROOT)/build/demos/wireless_shared/wh

LLIBRARY_DIRS   = $(ROOT)/build/demos/gx/UnitTours/DEMOLib/lib/$(TWL_BUILDTYPE)
LLIBRARIES      = libDEMO$(TWL_LIBSUFFIX).a

MAKEROM_ROMSPEED	= 1TROM

#SRCDIR			= # using default
#LCFILE			= # using default

#----------------------------------------------------------------------------
include	$(TWLSDK_ROOT)/build/buildtools/commondefs


###--------------------------------------------------------------------------
###  Setting for finalrom
###--------------------------------------------------------------------------
#### BEGIN
####
####  Please get your "libsyscall.a" and "rom header template" from Nintendo.
####  And put them in ./etc directory.
####
#
# ROM SPEC FILE
#
ROM_SPEC	= ./ROM-TS.TWL.rsf
#
# LIBSYSCALL: Provided to every product by NINTENDO
#
LIBSYSCALL		= ./040A/libsyscall.a
###
###
###
#### END

#----------------------------------------------------------------------------

do-build:		$(TARGETS)


include	$(TWLSDK_ROOT)/build/buildtools/modulerules


#===== End of Makefile =====
