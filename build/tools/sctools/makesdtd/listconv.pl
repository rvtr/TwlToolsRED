#!/usr/local/bin/perl

# �����`�F�b�N
if( 0 != $#ARGV )
{
    print "Usage: %> perl listconv.pl [TableFile]\n";
    exit;
}

# table�t�@�C���̓ǂݍ��݁B
open(IN, $ARGV[0]) || die "can\'t open TableFile: $!\n";

# �\���ϊ����[�v
while ($line = <IN>)
{ 
# 0x000300044b574645, 0x0100 , 0x3031 , sdmc:/sdtads/en_TWL-KWFE-v256.tad.out,
# 0x000300044b475545, 0x0000 , 0x3031 , sdmc:/sdtads/en_TWL-KGUE-v0.tad.out,

    if( $line =~ /^0x(\w+),\s0x(\w+)\s,\s0x(\w+)\s,\s([\w|\-|\.|:|\/]+),/ )
    {
# $1 - �^�C�g��ID
# $2 - �o�[�W�����̎��o��
# $3 - �Y�ꂽ�E�E
# $4 - �t�@�C����
	if( 16 != length($1) )
	{
	    print "Format error 1!\n";
	    close(IN);
	    exit;
	}
	$title_id = substr($1, 8, 8);
	$gcode = pack("H8", $title_id );
	$disp_line = sprintf("%s,%3d\n",$gcode , hex($2) );
	push(@line_array, $disp_line);
    }
    else
    {
        # ���т��߂��邩�I�H
	print "Format error 2!\n";
	close(IN);
	exit;
    }
}

# �t�@�C���N���[�Y
close(IN);


# �A���t�@�x�b�g���Ƀ\�[�g
@sort_line_array = sort(@line_array);

#�\��
while ($line = shift(@sort_line_array) )
{ 
    print $line;
}



