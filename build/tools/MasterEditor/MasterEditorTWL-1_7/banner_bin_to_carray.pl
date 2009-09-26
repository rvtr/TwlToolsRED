#!/usr/bin/perl

#=================================================================
# �o�i�[������񋓂���csv����C����̔z��𐶐�����X�N���v�g
#=================================================================

use strict;

# �o�C�i���̒��g��1�̕ϐ��Ƃ��ăQ�b�g
my $bin_size = -s $ARGV[0];
my $buf;
open(F, $ARGV[0]) or die "can not open file.\n";
binmode(F);
read(F, $buf, $bin_size);
close(F);

# �z�񖼂ƂȂ郊�[�W���������w��
my $region = $ARGV[1] or die "argv[1] is region name.";

# �z��̒��g�𐶐�
my @codes;
my $num_of_codes=0;
my $offset = 0;
while ($offset < $bin_size) {

    # 2�o�C�g�����o��
    my $b = substr($buf, $offset, 2);
    $offset += 2;

    # ���s
    my $suffix = ", ";
    if ( (${num_of_codes}%16) == 15 ) {
        $suffix = ",\n";
    }

    # �o�C�i���𐔒l�ɕϊ����Ă���ɕ�����ɕϊ�
    $codes[$num_of_codes++] = sprintf( "0x%04x", unpack("v*",$b) ) . $suffix;
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
