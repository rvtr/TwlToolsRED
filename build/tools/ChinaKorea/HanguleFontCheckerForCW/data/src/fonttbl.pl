#=============================================================================#
#	@file		fonttbl.pl
#	@brief		xlor�t�@�C������NITRO�C���f�b�N�X�e�[�u�����쐬
#	@author		Kazumasa Hirata
#	@note		usage:
#				perl tonttbl.pl [-o output-file] [-i imput-file]
#=============================================================================#

use strict;		
use Getopt::Std;
use File::Basename;

use vars qw( $opt_o $opt_i );

#------------------------------------------------------------#
#	���C���v���O����
#------------------------------------------------------------#
{
	getopt('oi');
	
	#�����`�F�b�N
	if( !$opt_o || !$opt_i ){
		die <<EOF;
usage:  perl tonttbl.pl [-o output-file] [-i imput-file]
EOF
	}
	my $jarfile = $ARGV[0];
	
	print "output  = $opt_o\n";
	print "imput   = $opt_i\n";
	
	#���̓t�@�C����ǂݍ���
	open FH, "$opt_i" or die "can't open file $opt_i: $!";
	my @imput = <FH>;
	close FH;

	#�o�̓t�@�C���I�[�v��
	open FH, ">$opt_o" or die "can't open file $opt_o: $!";
	
	#�K�v�ȃf�[�^���R���o�[�g���ďo�͂���
	my $startflag = 0;
	my @namelist;
	my @titlelist;
	
	foreach my $line ( @imput ){
		
		chomp $line;
		$line = "//$line";
		
		my $str;
		
		#�t�H���g�O���[�v�J�n���ǂ���
		if( $line =~ /<!-- ((\w|\s|\/|-)+) -->/ )
		{
			$str = $1;
			
			$titlelist[ @titlelist ] = $str;
			
			#�A���t�@�x�b�g�Ɛ����ȊO�̕����̓A���_�[�o�[�ɒu��
			$str =~ s/(\s|\/|-)+/_/g;
			
			$namelist[ @namelist ] = $str;
			
			#�z�񖼑O���ۂ��o��
			$str = "const unsigned short $str\[\] = {";
			
			#�z��I�[�̑}��
			if( $startflag == 1 ){		#���ɔz��錾�ς݂Ȃ�A�����
				$str = "};\n$str";		#�܂��͏I��点��
			}
			$startflag = 1;				#�z��錾���n�܂�����t���O�𗧂Ă�
		}
		#�X�y�[�X�ƃk���ȊO�ŁAHTML�^�O�J�n����<������s�͑ΏۊO
		elsif( ($line =~ /</) && !($line =~ /<sp\/>/) && !($line =~ /<null\/>/) )
		{

		}
		#�Ώە�������ڂ������ׂ�
		else
		{
			$str = $line;
			
			#�R�����g���E��
			$str =~ s/\/\///;
			
			#�X�y�[�X���X�v���b�g�Ƃ��ĕ�����𕪉�����
			my @array = split /\s+/, $str;
			
			#�������������v�f���Ƃɒu�����
			foreach my $word ( @array ){
				
				#sp�̐��`
				$word =~ s/<sp\/>/&#x0000;/;
				
				#null�̐��`
				$word =~ s/<null\/>/&#x0000;/;
				
				#����ȑ�֕����̐��`
				$word =~ s/&quot;/&#x0022;/;
				$word =~ s/&amp;/&#x0026;/;
				$word =~ s/&apos;/&#x0027;/;
				$word =~ s/&lt;/&#x003C;/;
				$word =~ s/&gt;/&#x003E;/;
				
				#16�i���̐��`
				$word =~ s/&#x([0-9A-F]+);/0x$1/;				 
				
				#ASCII������16�i�ϊ�
				if( length($word)==1 &&	0x20<=ord($word) && ord($word)<=0x7E ){
					$word = sprintf "0x%04x", ord($word);
				}
			}
			
			#�f�~���^��ݒ肵�Č�������			
			$str = join ',', @array;
			$str =~ s/^,//;			#�擪�ɕt������f���~�^�폜�i�Ȃ񂩓��������@�������͂������E�E�E�j
			$str =~ s/$/,/;			
			$str =~ s/^,$//;		
		}
		
		#�ϊ��O�̍s���o��
		print FH "$line\n";
		
		#�ϊ��㕶���񂪂���Ȃ�o��
		if( defined($str) ){
			print FH "$str\n";
		}
	}
	#�ŏI�̔z��̏I�����o��
	print FH "};\n";
	
	#�e�[�u���̗v�f���̌v�Z	
	foreach ( @namelist ) {
		print FH "const unsigned short Count_of_$_ = sizeof\($_\)/sizeof\($_\[0\]\);\n";
	}
	
	#�e�[�u���̖��O�̏o��
	for( my $i=0; $i<@titlelist; $i++ ) {
		print FH "const char * const Title_of_$namelist[$i] = \"$titlelist[$i]\";\n";
	}
	
	close FH;
}

