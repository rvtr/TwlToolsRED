#!/usr/bin/perl

#=================================================================
# バナー文字を列挙したcsvからC言語の配列を生成するスクリプト
#=================================================================

use strict;

# 配列の中身を生成
my $region = "Undefined";
my @codes;
my $num_of_codes=0;
while (<STDIN>) {

    chomp;

    # コメントから始まる行はリージョン名
    if ( /^\#/ ) {
        s/^\#[\s]*//;
        $region = $_;
    } else {

        # C言語の配列になるように"0x"をつける
        my $prefix = "";
        if ( !/^0x/ ) {
            $prefix .= "0x";
        }
        my $suffix = ", ";
        # 改行
        if ( (${num_of_codes}%16) == 15 ) {
            $suffix = ",\n";
        }
        $codes[$num_of_codes] = $prefix . $_ . $suffix;
        $num_of_codes++;
    }
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
