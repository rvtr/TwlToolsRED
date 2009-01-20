#=============================================================================#
#	@file		fonttbl.pl
#	@brief		xlorファイルからNITROインデックステーブルを作成
#	@author		Kazumasa Hirata
#	@note		usage:
#				perl tonttbl.pl [-o output-file] [-i imput-file]
#=============================================================================#

use strict;		
use Getopt::Std;
use File::Basename;

use vars qw( $opt_o $opt_i );

#------------------------------------------------------------#
#	メインプログラム
#------------------------------------------------------------#
{
	getopt('oi');
	
	#引数チェック
	if( !$opt_o || !$opt_i ){
		die <<EOF;
usage:  perl tonttbl.pl [-o output-file] [-i imput-file]
EOF
	}
	my $jarfile = $ARGV[0];
	
	print "output  = $opt_o\n";
	print "imput   = $opt_i\n";
	
	#入力ファイルを読み込み
	open FH, "$opt_i" or die "can't open file $opt_i: $!";
	my @imput = <FH>;
	close FH;

	#出力ファイルオープン
	open FH, ">$opt_o" or die "can't open file $opt_o: $!";
	
	#必要なデータをコンバートして出力する
	my $startflag = 0;
	my @namelist;
	my @titlelist;
	
	foreach my $line ( @imput ){
		
		chomp $line;
		$line = "//$line";
		
		my $str;
		
		#フォントグループ開始かどうか
		if( $line =~ /<!-- ((\w|\s|\/|-)+) -->/ )
		{
			$str = $1;
			
			$titlelist[ @titlelist ] = $str;
			
			#アルファベットと数字以外の文字はアンダーバーに置換
			$str =~ s/(\s|\/|-)+/_/g;
			
			$namelist[ @namelist ] = $str;
			
			#配列名前っぽく出力
			$str = "const unsigned short $str\[\] = {";
			
			#配列終端の挿入
			if( $startflag == 1 ){		#既に配列宣言済みなら、それを
				$str = "};\n$str";		#まずは終わらせる
			}
			$startflag = 1;				#配列宣言が始まったらフラグを立てる
		}
		#スペースとヌル以外で、HTMLタグ開始文字<がある行は対象外
		elsif( ($line =~ /</) && !($line =~ /<sp\/>/) && !($line =~ /<null\/>/) )
		{

		}
		#対象文字列を詳しく調べる
		else
		{
			$str = $line;
			
			#コメントを殺す
			$str =~ s/\/\///;
			
			#スペースをスプリットとして文字列を分解する
			my @array = split /\s+/, $str;
			
			#分解した文字要素ごとに置換作業
			foreach my $word ( @array ){
				
				#spの整形
				$word =~ s/<sp\/>/&#x0000;/;
				
				#nullの整形
				$word =~ s/<null\/>/&#x0000;/;
				
				#特殊な代替文字の整形
				$word =~ s/&quot;/&#x0022;/;
				$word =~ s/&amp;/&#x0026;/;
				$word =~ s/&apos;/&#x0027;/;
				$word =~ s/&lt;/&#x003C;/;
				$word =~ s/&gt;/&#x003E;/;
				
				#16進数の整形
				$word =~ s/&#x([0-9A-F]+);/0x$1/;				 
				
				#ASCII文字の16進変換
				if( length($word)==1 &&	0x20<=ord($word) && ord($word)<=0x7E ){
					$word = sprintf "0x%04x", ord($word);
				}
			}
			
			#デミリタを設定して結合する			
			$str = join ',', @array;
			$str =~ s/^,//;			#先頭に付着するデリミタ削除（なんか頭いい方法あったはずだが・・・）
			$str =~ s/$/,/;			
			$str =~ s/^,$//;		
		}
		
		#変換前の行を出力
		print FH "$line\n";
		
		#変換後文字列があるなら出力
		if( defined($str) ){
			print FH "$str\n";
		}
	}
	#最終の配列の終わりを出力
	print FH "};\n";
	
	#テーブルの要素数の計算	
	foreach ( @namelist ) {
		print FH "const unsigned short Count_of_$_ = sizeof\($_\)/sizeof\($_\[0\]\);\n";
	}
	
	#テーブルの名前の出力
	for( my $i=0; $i<@titlelist; $i++ ) {
		print FH "const char * const Title_of_$namelist[$i] = \"$titlelist[$i]\";\n";
	}
	
	close FH;
}

