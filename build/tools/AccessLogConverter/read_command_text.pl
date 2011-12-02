#!/usr/bin/perl --

use strict;

my $file = $ARGV[0] or die "Usage: $0 [TXT File]\n\n";

open IN, $file or die "Cannot open $file.\n";

my $start = 0;
my $size = 0;

my $step = 0;
my $addr = 0;

my $bkup = 0;

while (<IN>) {
    my @data = split /\t/;
    # pre-read
    if ( $step == 0 ) {
        $step = 1  if ( $data[2] == 1 && $data[3] == 1 );
        # backup access
        if ( $data[2] == 1 && $data[3] == 0 )
        {
            $bkup = 1;
        }
    }
    # read
    elsif ( $step == 1 ) {
        if ( $data[4] =~ /B7/ ) {
            $step++;
            $addr = 0; # initialize
        } else {
            $step = 0; # restart
        }
    }
    # first address
    elsif ( $step == 2 || $step == 3 || $step == 4 || $step == 5 ) {
        $addr = $addr*256 + hex($data[4]);
        if ( $step == 5 ) {
            if ( $start + $size == $addr && $bkup == 0 ) {
                $size += 512;
            } else {
                printf "Read: 0x%08x-0x%08x (%6d bytes)\n", $start, $start+$size-1, $size if ($start);
                $start = $addr;
                $size = 512;
                if ( $bkup == 1 ) {
                    print "<<BACKUP ACCESS>>\n" ;
                    $bkup = 0;
                }
            }
            $step = 0; # restart
        } else {
            $step++;
        }
    }
}
close IN;

if ($start) {
    printf "Read: 0x%08x-0x%08x (%6d bytes)\n", $start, $start+$size-1, $size
} else {
    printf "Never read.\n";
}
if ( $bkup == 1 ) {
    print "<<BACKUP ACCESS>>\n" ;
}
