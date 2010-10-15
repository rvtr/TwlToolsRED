#!/usr/bin/perl

#=================================================================
# �o�i�[������񋓂���csv����C����̔z��𐶐�����X�N���v�g
#=================================================================

use strict;

# �z��̒��g�𐶐�
my $region = "Undefined";
my @codes;
my $num_of_codes=0;
while (<STDIN>) {

    chomp;

    # �R�����g����n�܂�s�̓��[�W������
    if ( /^\#/ ) {
        s/^\#[\s]*//;
        $region = $_;
    } else {

        # C����̔z��ɂȂ�悤��"0x"������
        my $prefix = "";
        if ( !/^0x/ ) {
            $prefix .= "0x";
        }
        my $suffix = ", ";
        # ���s
        if ( (${num_of_codes}%16) == 15 ) {
            $suffix = ",\n";
        }
        $codes[$num_of_codes] = $prefix . $_ . $suffix;
        $num_of_codes++;
    }
}

# �w�b�_
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

# �t�b�^
my $footer =<<'EOF';
};
// end of file
EOF

# �o��
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
