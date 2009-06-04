#!/usr/bin/perl

#=================================================================
# バナー文字を列挙したcsvからC言語の配列を生成するスクリプト
#=================================================================

use strict;

# バイナリの中身を1つの変数としてゲット
my $bin_size = -s $ARGV[0];
my $buf;
open(F, $ARGV[0]) or die "can not open file.\n";
binmode(F);
read(F, $buf, $bin_size);
close(F);

# 配列名となるリージョン名を指定
my $region = $ARGV[1] or die "argv[1] is region name.";

# 配列の中身を生成
my @codes;
my $num_of_codes=0;
my $offset = 0;
while ($offset < $bin_size) {

    # 2バイト抜き出す
    my $b = substr($buf, $offset, 2);
    $offset += 2;

    # 改行
    my $suffix = ", ";
    if ( (${num_of_codes}%16) == 15 ) {
        $suffix = ",\n";
    }

    # バイナリを数値に変換してさらに文字列に変換
    $codes[$num_of_codes++] = sprintf( "0x%04x", unpack("v*",$b) ) . $suffix;
}

# ヘッダ
my $header_format =<<'EOF';
#include "stdafx.h"
#include <twl/types.h>
#include <twl/os/common/format_rom.h>
#include <twl/os/common/ownerInfoEx.h>
#include "banner_char.h"

const int MasterEditorTWL::NumOf_BannerCharCodes_%1$s = %2$d;
const u16 MasterEditorTWL::BannerCharCodes_%1$s[%2$d] =
{
EOF

# フッタ
my $footer =<<'EOF';
};
// end of file
EOF

# 出力
my $header = sprintf ($header_format, $region, $num_of_codes);
printf STDOUT $header;
foreach my $code (@codes) {
    printf STDOUT $code;
}
if( ($num_of_codes%16) != 0 ) {
    printf STDOUT "\n";
}
printf STDOUT $footer;

# end of file
