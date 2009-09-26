#!/usr/bin/sh

maker="./IllegalRomMaker.exe"
faker="./FakeRomHeader.user.exe"
rating_free_tool="./RatingAllFreeTool.exe"

usr_card="./regular_card.srl"
usr_nand="./regular_nand.nand.srl"
sys_nand="./regular_sys.nand.srl"

tcl_card="./tcl_card.srl"
tcl_nand="./tcl_nand.nand.srl"
china_nand="./china_nand.nand.srl"

usr_nand_50rel="./regular_nand_50rel.srl"
usr_nand_52rc="./regular_nand_52rc.srl"
usr_card_52rel="./regular_card_52rel.srl"
usr_nand_52rel="./regular_nand_52rel.srl"
usr_nand_53pr="./regular_nand_53pr.srl"

outdir="../illegal_roms"
#outdir="./out"

tmp_srl="./tmp.srl"
tmp2_srl="./tmp2.srl"


#
# Check
#

if [ -e $maker ]
then
	echo $maker exists.
else
	echo $maker does not exist.
	exit 1
fi

if [ -e $faker ]
then
	echo $faker exists.
else
	echo $faker does not exist.
	exit 1
fi

if [ -e $usr_card ]
then
	echo $usr_card exists.
else
	echo $usr_card does not exist.
	exit 1
fi

if [ -e $usr_nand ]
then
	echo $usr_nand exists.
else
	echo $usr_nand does not exist.
	exit 1
fi

if [ -e $sys_nand ]
then
	echo $sys_nand exists.
else
	echo $sys_nand does not exist.
	exit 1
fi


#
# Basic info
#

$maker $usr_nand $outdir/title_name_nand1.srl 0 -a 73 -f
$maker $usr_nand $outdir/title_name_nand2.srl 6 -a 00 -f
$maker $usr_nand $outdir/title_name_nand3.srl A -2 -a 2020 -f
$maker $usr_nand $outdir/game_code_nand1.srl F -a 61 -f
$maker $usr_nand $outdir/game_code_nand2.srl C -4 -a 4A52544E -f
$maker $usr_nand $outdir/maker_code_nand1.srl 11 -a 61 -f
$maker $usr_nand $outdir/device_type_nand1.srl 13 -a 01 -f
$maker $usr_nand $outdir/rom_version_nand1.srl 1E -a 10 -f
$maker $usr_nand $outdir/rom_version_nand2.srl 1E -a E0 -f
$maker $usr_card $outdir/rom_speed_type_card1.srl 1F -e 6 -f
$maker $usr_nand $outdir/rom_speed_type_nand1.srl 1F -e 6 -f
$maker $usr_nand $outdir/banner_offset_nand1.srl 68 -4 -a 00000000 -f
$maker $usr_nand $outdir/logo_crc_nand1.srl 15C -2 -a 0000 -f
$maker $usr_nand $outdir/header_crc_nand1.srl 15E -2 -a 0000 -f
$maker $usr_nand $outdir/valid_size_nand1.srl 80 -4 -a 00000000 -f
$maker $usr_nand $tmp_srl 1C -d 0 -f
$maker $tmp_srl $outdir/platform_nand1.srl 12 -a 00 -f
$maker $usr_card $outdir/rom_size_card3.srl 14 -a 07 -f
$maker $usr_card $outdir/rom_size_card4.srl 14 -a 0D -f
$maker $usr_nand $outdir/rom_size_nand1.srl 14 -a 09 -f
$maker $usr_card $tmp_srl 60 -4 -a 00416017 -f
$maker $tmp_srl $outdir/mrom_card1.srl 64 -4 -a 001808F8 -f
$maker $usr_nand $outdir/disable_debug_nand_usr1.srl 1C -e 3 -f
$maker $sys_nand $outdir/disable_debug_nand_sys1.srl 1C -d 3 -f
$maker $usr_card $outdir/rom_offset_card1.srl 90 -2 -a 0000 -f
$maker $usr_card $outdir/rom_offset_card2.srl 92 -2 -a 0000 -f
$maker $usr_card $outdir/rom_offset_card3.srl 90 -4 -a 00010002 -f
$maker $usr_nand $outdir/load_address_nand1.srl 28 -4 -a 02000400 -f
$maker $usr_card $outdir/load_address_card1.srl 28 -4 -a 02000400 -f

#
# Region
#

$maker $usr_nand $outdir/region_nand1.srl 1B0 -4 -a 00000003 -f
$maker $usr_nand $outdir/region_nand2.srl 1B0 -4 -a FFFFFFFF -f

#
# App jump
#

$maker $usr_card $outdir/normal_app_jump_card1.srl 1d -e 0 -f
$maker $usr_card $outdir/tmp_app_jump_card1.srl 1d -e 1 -f
$maker $usr_nand $outdir/both_app_jump_nand1.srl 1d -E 03 -f

#
# Access Control
#

$maker $usr_card $outdir/access_card_usr0.srl 1B4 -4 -e 0 -f
$maker $usr_card $outdir/access_card_usr1.srl 1B4 -4 -e 1 -f
$maker $usr_card $outdir/access_card_usr2.srl 1B4 -4 -e 2 -f
$maker $usr_card $outdir/access_card_usr4.srl 1B4 -4 -e 4 -f
$maker $usr_card $outdir/access_card_usr5.srl 1B4 -4 -e 5 -f
$maker $usr_card $outdir/access_card_usr7.srl 1B4 -4 -e 7 -f
$maker $usr_card $outdir/access_card_usr8.srl 1B4 -4 -e 8 -f
$maker $usr_card $outdir/access_card_usr9.srl 1B4 -4 -e 9 -f
$maker $usr_card $outdir/access_card_usr10.srl 1B4 -4 -e 10 -f
$maker $usr_card $outdir/access_card_usr15.srl 1B4 -4 -e 15 -f
$maker $usr_card $outdir/access_card_usr16.srl 1B4 -4 -e 16 -f
$maker $usr_card $outdir/access_card_usr17.srl 1B4 -4 -e 17 -f
$maker $usr_card $outdir/access_card_usr31.srl 1B4 -4 -e 31 -f

$maker $usr_nand $outdir/access_nand_usr0.srl 1B4 -4 -e 0 -f
$maker $usr_nand $outdir/access_nand_usr1.srl 1B4 -4 -e 1 -f
$maker $usr_nand $outdir/access_nand_usr2.srl 1B4 -4 -e 2 -f
$maker $usr_nand $outdir/access_nand_usr4.srl 1B4 -4 -e 4 -f
$maker $usr_nand $outdir/access_nand_usr5.srl 1B4 -4 -e 5 -f
$maker $usr_nand $outdir/access_nand_usr7.srl 1B4 -4 -e 7 -f
$maker $usr_nand $outdir/access_nand_usr8.srl 1B4 -4 -e 8 -f
$maker $usr_nand $outdir/access_nand_usr9.srl 1B4 -4 -e 9 -f
$maker $usr_nand $outdir/access_nand_usr15.srl 1B4 -4 -e 15 -f
$maker $usr_nand $outdir/access_nand_usr16.srl 1B4 -4 -e 16 -f
$maker $usr_nand $outdir/access_nand_usr5_8.srl 1B4 -4 -E 00000120 -f
$maker $usr_nand $outdir/access_nand_usr17.srl 1B4 -4 -e 17 -f
$maker $usr_nand $outdir/access_nand_usr31.srl 1B4 -4 -e 31 -f

$maker $sys_nand $outdir/access_nand_sys0.srl 1B4 -4 -e 0 -f
$maker $sys_nand $outdir/access_nand_sys1.srl 1B4 -4 -e 1 -f
$maker $sys_nand $outdir/access_nand_sys2.srl 1B4 -4 -e 2 -f
$maker $sys_nand $outdir/access_nand_sys3.srl 1B4 -4 -e 3 -f
$maker $sys_nand $outdir/access_nand_sys5.srl 1B4 -4 -e 5 -f
$maker $sys_nand $outdir/access_nand_sys7.srl 1B4 -4 -e 7 -f
$maker $sys_nand $outdir/access_nand_sys8.srl 1B4 -4 -e 8 -f
$maker $sys_nand $outdir/access_nand_sys9.srl 1B4 -4 -e 9 -f
$maker $sys_nand $outdir/access_nand_sys10.srl 1B4 -4 -e 10 -f
$maker $sys_nand $outdir/access_nand_sys11.srl 1B4 -4 -e 11 -f
$maker $sys_nand $outdir/access_nand_sys12.srl 1B4 -4 -e 12 -f
$maker $sys_nand $outdir/access_nand_sys13.srl 1B4 -4 -e 13 -f
$maker $sys_nand $outdir/access_nand_sys14.srl 1B4 -4 -e 14 -f
$maker $sys_nand $outdir/access_nand_sys15.srl 1B4 -4 -e 15 -f
$maker $sys_nand $outdir/access_nand_sys16.srl 1B4 -4 -e 16 -f
$maker $sys_nand $outdir/access_nand_sys5_8.srl 1B4 -4 -E 00000120 -f
$maker $sys_nand $outdir/access_nand_sys17.srl 1B4 -4 -e 17 -f
$maker $sys_nand $outdir/access_nand_sys31.srl 1B4 -4 -e 31 -f

#
# SD Access
#

$maker $usr_card_52rel $outdir/sd_access_card1.srl 1B4 -4 -E 00006008 -f
$maker $usr_nand_50rel $outdir/sd_access_nand1.srl 1B4 -4 -E 00000008 -f
$maker $usr_nand_52rc  $outdir/sd_access_nand2.srl 1B4 -4 -E 00006008 -f
$maker $usr_nand_52rel $outdir/sd_access_nand3.srl 1B4 -4 -E 00000008 -f
$maker $usr_nand_53pr  $outdir/sd_access_nand4.srl 1B4 -4 -E 00002008 -f
$maker $usr_nand_52rel $outdir/sd_access_nand5.srl 1B4 -4 -E 00006008 -f
cp $usr_nand_52rc  $outdir/sd_access_nand6.srl -f

#
# Photo Access
#

$maker $usr_card $outdir/photo_access_card1.srl 1B4 -4 -e 11 -f
$maker $tcl_card $outdir/photo_access_card2.srl 1B4 -4 -e 12 -f
$maker $usr_nand $outdir/photo_access_nand1.srl 1B4 -4 -e 12 -f
$maker $tcl_nand $tmp_srl 1B4 -4 -e 11 -f
$maker $tmp_srl $outdir/photo_access_nand2.srl 1B4 -4 -e 10 -f

#
# Shared2
#

$maker $usr_nand $outdir/shared2_nand_usr1.srl 217 -a 7F -f
$maker $usr_nand $outdir/shared2_nand_usr2.srl 1B4 -4 -e 6 -f

$maker $usr_nand $tmp_srl 1B4 -4 -e 6 -f
$maker $tmp_srl $outdir/shared2_file0_nand_usr1.srl 20C -a 7F -f
$maker $usr_nand $tmp_srl 1B4 -4 -e 6 -f
$maker $tmp_srl $outdir/shared2_file1_nand_usr1.srl 20D -a 7F -f
$maker $usr_nand $tmp_srl 1B4 -4 -e 6 -f
$maker $tmp_srl $outdir/shared2_file2_nand_usr1.srl 214 -a 7F -f
$maker $usr_nand $tmp_srl 1B4 -4 -e 6 -f
$maker $tmp_srl $outdir/shared2_file3_nand_usr1.srl 215 -a 7F -f
$maker $usr_nand $tmp_srl 1B4 -4 -e 6 -f
$maker $tmp_srl $outdir/shared2_file4_nand_usr1.srl 216 -a 7F -f
$maker $usr_nand $tmp_srl 1B4 -4 -e 6 -f
$maker $tmp_srl $outdir/shared2_file5_nand_usr1.srl 217 -a 7F -f

$maker $sys_nand $outdir/shared2_nand_sys1.srl 217 -a 7F -f
$maker $sys_nand $outdir/shared2_nand_sys2.srl 1B4 -4 -e 6 -f

$maker $sys_nand $tmp_srl 1B4 -4 -e 6 -f
$maker $tmp_srl $outdir/shared2_file0_nand_sys1.srl 20C -a 7F -f
$maker $sys_nand $tmp_srl 1B4 -4 -e 6 -f
$maker $tmp_srl $outdir/shared2_file1_nand_sys1.srl 20D -a 7F -f
$maker $sys_nand $tmp_srl 1B4 -4 -e 6 -f
$maker $tmp_srl $outdir/shared2_file2_nand_sys1.srl 214 -a 7F -f
$maker $sys_nand $tmp_srl 1B4 -4 -e 6 -f
$maker $tmp_srl $outdir/shared2_file3_nand_sys1.srl 215 -a 7F -f
$maker $sys_nand $tmp_srl 1B4 -4 -e 6 -f
$maker $tmp_srl $outdir/shared2_file4_nand_sys1.srl 216 -a 7F -f
$maker $sys_nand $tmp_srl 1B4 -4 -e 6 -f
$maker $tmp_srl $outdir/shared2_file5_nand_sys1.srl 217 -a 7F -f

#
# TWL extended info
#

$maker $usr_nand $outdir/scfg_nand1.srl 1B8 -4 -e 31 -f
$maker $usr_nand $outdir/whitelist.srl 1BF -e 6 -f
$maker $usr_nand $outdir/icon_nand1.srl 1BF -E 18 -f
$maker $usr_nand $outdir/titleid_lo_nand1.srl 230 -a 5A -f
$faker $sys_nand $outdir/apptype_nand1.srl -f
$maker $sys_nand $tmp_srl 238 -4 -a 01000000 -f
$maker $tmp_srl $outdir/nand_size_nand_sys1.srl 23C -4 -a 01000000 -f
$maker $sys_nand $tmp_srl 238 -4 -a 00800000 -f
$maker $tmp_srl $outdir/nand_size_nand_sys2.srl 23C -4 -a 00800000 -f
$maker $usr_nand $tmp_srl 238 -4 -a 00800000 -f
$maker $tmp_srl $outdir/nand_size_nand_usr1.srl 23C -4 -a 00800000 -f
$maker $usr_card $outdir/seg3crc_card1.srl 3FFF -a 01 -f
$maker $usr_card $outdir/aes_card1.srl 1C -d 1 -f
$maker $usr_card $outdir/aes_card2.srl 224 -4 -a 00000000 -f
$maker $usr_nand $outdir/aes_nand1.srl 1C -d 1 -f
$maker $usr_nand $outdir/aes_nand2.srl 224 -4 -a 00000000 -f
$maker $usr_nand $outdir/aes_nand3.srl 22C -4 -a 00000000 -f
$maker $usr_card $outdir/main_ltd_rom_offset_card1.srl 92 -2 -a 02 -f
$maker $usr_card $outdir/public_save_data_card1.srl 238 -4 -a 00000001 -f
$maker $usr_card $outdir/private_save_data_card1.srl 23C -4 -a 00000001 -f
cp $usr_nand_53pr $outdir/sdk_pr.srl -f
cp $usr_nand_52rc $outdir/sdk_rc.srl -f

#
# Rating
#

$maker $usr_nand $outdir/rating_nand1.srl 2F0 -a 00 -f
$maker $usr_nand $outdir/rating_nand2.srl 2F1 -a 80 -f
$maker $usr_nand $outdir/rating_nand3.srl 2F0 -a 8E -f
$maker $usr_nand $tmp_srl 20F -e 0 -f
$maker $tmp_srl $outdir/rating_nand4.srl 2F0 -a 8C -f
$maker $usr_nand $tmp_srl 20F -e 0 -f
$maker $tmp_srl $outdir/rating_nand5.srl 2F0 -a 80 -f
$maker $usr_nand $outdir/rating_nand6.srl 2F0 -a C0 -f

#
# China
#

$rating_free_tool $china_nand $outdir/china_nand_usr1.srl -f
$maker $usr_nand $tmp_srl 1B0 -4 -a 00000010 -f
$rating_free_tool $tmp_srl $outdir/china_nand_usr2.srl -f
$maker $usr_nand $tmp_srl 1D -e 7 -f
$rating_free_tool $tmp_srl $outdir/china_nand_usr3.srl -f
$maker $china_nand $tmp_srl 1B0 -4 -a 00000001 -f
$maker $tmp_srl $tmp2_srl 1D -d 7 -f
$rating_free_tool $tmp2_srl $outdir/china_nand_usr4.srl -f
$rating_free_tool $china_nand $outdir/china_nand_usr5.srl -c -f
$maker $sys_nand $tmp_srl 1B0 -4 -a 00000010 -f
$maker $tmp_srl $tmp2_srl 1D -e 7 -f
$rating_free_tool $tmp2_srl $outdir/china_nand_sys1.srl -f

#
# Korea
#

$maker $usr_nand $tmp_srl 1B0 -4 -a 00000020 -f
$maker $tmp_srl $outdir/korea_nand_usr1.srl 1D -e 6 -f
$maker $usr_nand $outdir/korea_nand_usr2.srl 1B0 -4 -a 00000020 -f
$maker $usr_nand $outdir/korea_nand_usr3.srl 1D -e 6 -f

#
# Reserved area
#

$maker $usr_nand $outdir/reserved_nand01.srl 78 -a 01 -f
$maker $usr_nand $outdir/reserved_nand02.srl 7F -a 01 -f
$maker $usr_nand $outdir/reserved_nand03.srl 160 -a 01 -f
$maker $usr_nand $outdir/reserved_nand04.srl 17F -a 01 -f
$maker $usr_nand $outdir/reserved_nand05.srl 15 -a 01 -f
$maker $usr_nand $outdir/reserved_nand06.srl 1B -a 01 -f
$maker $usr_nand $outdir/reserved_nand07.srl 99 -a 01 -f
$maker $usr_nand $outdir/reserved_nand08.srl BF -a 01 -f
$maker $usr_nand $outdir/reserved_nand09.srl 240 -a 01 -f
$maker $usr_nand $outdir/reserved_nand10.srl 2EF -a 01 -f
$maker $usr_nand $outdir/reserved_nand11.srl 378 -a 01 -f
$maker $usr_nand $outdir/reserved_nand12.srl 39F -a 01 -f
$maker $usr_nand $outdir/reserved_nand13.srl 3B4 -a 01 -f
$maker $usr_nand $outdir/reserved_nand14.srl F7F -a 01 -f
$maker $usr_nand $outdir/reserved_nand15.srl 1BC -a 01 -f
$maker $usr_nand $outdir/reserved_nand16.srl 1BE -a 01 -f
