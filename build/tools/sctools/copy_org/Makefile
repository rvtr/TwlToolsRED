#! make -f
#----------------------------------------------------------------------------
# Project:  TwlSDK - demos - MI - ndma-1
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
# $Date:: 2008-01-17#$
# $Rev: 3650 $
# $Author: okubata_ryoma $
#----------------------------------------------------------------------------
SUBDIRS			 = ./banner

TARGET_PLATFORM		:= TWL
TWL_ARCHGEN		:= LIMITED

SRCDIR			=	../common/src ./src 

SRCS			=	main.c key.c font.c text.c mprintf.c logprintf.c \
				gfx.c hwi.c mynvram.c my_fs_util.c miya_mcu.c error_report.c \
				sitedefs.c wcm_control.c netconnect.c mywlan.c \
				stream.c myfilename.c menu_version.c pre_install.c myimport.c

TARGET_BIN		=	copy_org.srl
ROM_SPEC		=	copy_org.rsf
TARGET_AUTOBOOT		=	./bin/$(TWL_BUILDTYPE)/copy_org_autoboot.srl


ES_DIR			=	$(ROOT)/add-ins/es
ES_LATEST		=	$(shell ls -d $(ES_DIR)/es-sdk-*|sort|tail -n 1)
ES_ROOT			=	$(ES_LATEST)

LINCLUDES           = 	$(TWLSDK_ROOT)/build/libraries/lcfg/ARM9.TWL/include ../common/src \
			$(TWLSDK_ROOT)/build/libraries/nam/ARM9.TWL/include \
			$(TWLSDK_ROOT)/build/libraries/ese/common/include \
			$(ES_ROOT)/twl/include	$(ES_ROOT)/common/lib/ec/include

LLIBRARY_DIRS	+=	$(ES_ROOT)/twl/lib/$(TWL_BUILDTYPE) ../wifilib/$(TWL_BUILDTYPE)

# LLIBRARY_DIRS	+=	$(ES_ROOT)/twl/lib/$(TWL_BUILDTYPE)


LLIBRARIES		+=	libecx$(TWL_LIBSUFFIX).a   \
				liblcfg$(TWL_LIBSUFFIX).a  \
				libshr$(TWL_LIBSUFFIX).a   \
				libnhttp.nssl$(TWL_LIBSUFFIX).a	\
				libnuc$(TWL_LIBSUFFIX).a	\
				libnup$(TWL_LIBSUFFIX).a	\
				libnam$(TWL_LIBSUFFIX).a   \
				libes$(TWL_LIBSUFFIX).a    \
	                        libsea$(TWL_LIBSUFFIX).a   \
				libboc$(TWL_LIBSUFFIX).a   \
				libsfs$(TWL_LIBSUFFIX).a   \
				libna$(TWL_LIBSUFFIX).a    \

#				libnssl$(TWL_LIBSUFFIX).a	\
#				libnhttp$(TWL_LIBSUFFIX).a \


MAKEROM_ROMROOT  = ../files
MAKEROM_ROMFILES = fanfare.32.wav ok.wav ng.wav cursor.wav

MY_TWL_COMPONENTSDIR	= ../my_armadillo.TWL/bin
COMPONENT_NAME      =   armadillo

MAKEROM_ARM7_BASE   =   $(MY_TWL_COMPONENTSDIR)/$(TWL_BUILDTYPE_ARM7)/$(COMPONENT_NAME)
MAKEROM_ARM7        =   $(MAKEROM_ARM7_BASE).$(TWL_ELF_EXT)



include $(TWLWIFI_ROOT)/build/buildtools/commondefs
include $(TWLSDK_ROOT)/build/buildtools/commondefs


#----------------------------------
# セキュアアプリ指定

MAKEROM			 := $(TWL_TOOLSDIR)/bin/makerom.TWL.secure.exe


#----------------------------------------------------------------------------

#----------------------------------------------------------------------------

$(TARGET_AUTOBOOT): ./bin/$(TWL_BUILDTYPE)/$(TARGET_BIN)
		../bin/FakeRomHeader.secure.exe  -c -f $< $@

do-build:		$(TARGETS) $(TARGET_AUTOBOOT)

#----------------------------------------------------------------------------

include $(TWLWIFI_ROOT)/build/buildtools/modulerules
include $(TWLSDK_ROOT)/build/buildtools/modulerules




