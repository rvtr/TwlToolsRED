#! make -f
#----------------------------------------------------------------------------
# Project:  TwlSDK - demos.TWL - nandApp - appliCooperation - cooperationA
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
# $Date:: $
# $Rev$
# $Author$
#----------------------------------------------------------------------------

SUBDIRS					=	banner

#-------------------------
#-- NAND アプリのビルドを指定します。

TWL_NANDAPP		=	TRUE


#----------------------------------------------------------------------------

ID_NUMBER				?=	0

GAME_CODE				=	0VIA

TARGET_PLATFORM			=	TWL
TWL_ARCHGEN				=	LIMITED

SRCS					=	main.c NitroArm7VerChecker.c \
							misc_simple.c \
							data.c

LIBSYSCALL				=	header/libsyscall.a

TARGET_BIN				=	ComponentSDKVersionIdentifier_$(ID_NUMBER).tad
BINDIR			= ./bin/$(TWL_BUILDTYPE)/$(ID_NUMBER)


#-------------------------
#-- NAND アプリではいくつかのパラメータの指定のために固有の RSF ファイルが必要です。

ROM_SPEC			=	demo.autogen.rsf
ROM_SPEC_TEMPLATE	=	ROM-TS_nand_alt.rsf
ROM_SPEC_PARAM		=	MakerCode=01					\
						GameCode=$(GAME_CODE)			\
						BannerFile="banner/banner.bnr"	\
						PermitLandingNormalJump=TRUE	\
						TitleName="CSDKVERIDER"


include $(TWLSDK_ROOT)/build/buildtools/commondefs
include	$(TWLSDK_ROOT)/build/buildtools/commondefs.gx.demolib

#----------------------------------------------------------------------------

do-build : include/revision.h $(TARGETS) 

include $(TWLSDK_ROOT)/build/buildtools/modulerules

include/revision.h ::
	echo "#define ID_NUM		"$(ID_NUMBER) > $@

#----------------------------------------------------------------------------

#===== End of Makefile =====
