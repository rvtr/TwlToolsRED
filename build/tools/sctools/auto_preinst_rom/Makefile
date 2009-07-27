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
SUBDIRS			 = ./banner ../files/tads ../files/taddevs

TARGET_PLATFORM		:= TWL
TWL_ARCHGEN		:= LIMITED

SRCDIR			=	./src ../common/src

SRCS			=	main.c key.c font.c text.c mprintf.c logprintf.c \
				gfx.c hwi.c mynvram.c my_fs_util.c \
				miya_mcu.c error_report.c \
				myfilename.c menu_version.c \
				sitedefs.c wcm_control.c netconnect.c mywlan.c \
				myimport.c pre_install.c \
				my_rsa_sign.c my_aes.c my_sign.c 

# hatamotolib.cpp 


TARGET_BIN		=	auto_preinst_rom.srl
ROM_SPEC		=	auto_preinst_rom.rsf
# TARGET_AUTOBOOT		=	./bin/$(TWL_BUILDTYPE)/auto_preinst_rom.srl


ES_DIR			=	$(ROOT)/add-ins/es
ES_LATEST		=	$(shell ls -d $(ES_DIR)/es-sdk-*|sort|tail -n 1)
ES_ROOT			=	$(ES_LATEST)

LINCLUDES           = 	../common/src ../copy_dst/src \
			$(TWLSDK_ROOT)/build/libraries/lcfg/ARM9.TWL/include \
			$(TWLSDK_ROOT)/build/libraries/nam/ARM9.TWL/include \
			$(TWLSDK_ROOT)/build/libraries/ese/common/include \
			$(ES_ROOT)/twl/include	$(ES_ROOT)/common/lib/ec/include

LLIBRARY_DIRS	+=	$(ES_ROOT)/twl/lib/$(TWL_BUILDTYPE) ../wifilib/$(TWL_BUILDTYPE)
# LLIBRARY_DIRS	+=	$(ES_ROOT)/twl/lib/$(TWL_BUILDTYPE)


LLIBRARIES		+=	libecx$(TWL_LIBSUFFIX).a   \
				liblcfg$(TWL_LIBSUFFIX).a  \
				libnhttp.nssl$(TWL_LIBSUFFIX).a	\
				libshr$(TWL_LIBSUFFIX).a   \
				libnuc$(TWL_LIBSUFFIX).a	\
				libnup$(TWL_LIBSUFFIX).a	\
				libnam$(TWL_LIBSUFFIX).a   \
				libes$(TWL_LIBSUFFIX).a    \
	                        libsea$(TWL_LIBSUFFIX).a   \
				libboc$(TWL_LIBSUFFIX).a   \
				libsfs$(TWL_LIBSUFFIX).a   \
				libna$(TWL_LIBSUFFIX).a    \


MAKEROM_ROMROOT  = ../files

TAD_TABLE_TXT		= ../files/tads/tad_table.txt

TADDEV_TABLE_TXT	= ../files/taddevs/taddev_table.txt

MAKE_TAD_INCLUDE_FILE		= ../copy_dst/MakefileTad.inc

MAKE_TADDEV_INCLUDE_FILE	= ../copy_dst/MakefileTadDev.inc

-include $(MAKE_TAD_INCLUDE_FILE)

-include $(MAKE_TADDEV_INCLUDE_FILE)

MAKEROM_ROMFILES = tads/tad_table.txt taddevs/taddev_table.txt $(MAKEROM_TAD_ROMFILES) $(MAKEROM_TADDEV_ROMFILES)

MY_TWL_COMPONENTSDIR	= ../my_armadillo.TWL/bin

COMPONENT_NAME      =   armadillo

MAKEROM_ARM7_BASE   =   $(MY_TWL_COMPONENTSDIR)/$(TWL_BUILDTYPE_ARM7)/$(COMPONENT_NAME)

MAKEROM_ARM7        =   $(MAKEROM_ARM7_BASE).$(TWL_ELF_EXT)


LDIRT_CLEAN	=


include $(TWLWIFI_ROOT)/build/buildtools/commondefs
include $(TWLSDK_ROOT)/build/buildtools/commondefs


#----------------------------------
# セキュアアプリ指定

MAKEROM			 := $(TWL_TOOLSDIR)/bin/makerom.TWL.secure.exe


#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# $(TARGET_AUTOBOOT): ./bin/$(TWL_BUILDTYPE)/$(TARGET_BIN)
#		../bin/FakeRomHeader.secure.exe  -c -f $< $@


# do-build:		$(TARGETS) $(TARGET_AUTOBOOT)

do-build:		$(TARGETS)


$(BINDIR)/$(TARGET_BIN_BASENAME).$(TWL_ELF_EXT): $(MAKE_TAD_INCLUDE_FILE) $(MAKE_TADDEV_INCLUDE_FILE) $(TAD_TABLE_TXT) $(TADDEV_TABLE_TXT)


#----------------------------------------------------------------------------

include $(TWLWIFI_ROOT)/build/buildtools/modulerules
include $(TWLSDK_ROOT)/build/buildtools/modulerules
