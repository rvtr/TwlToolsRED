#!/bin/perl

if( @ARGV[0] !~ /^[0-9]+$/ || @ARGV[1] !~ /^[0-9]+$/ || @ARGV[0] > @ARGV[1] )
{
    print STDOUT "Usage : AutoBuild.pl start end [BUILDOPTIONS...] \n";
    print STDOUT "        start, end - positive number. must be ( start <= end ) \n";
	exit 1;
}

foreach $number(@ARGV[0]..@ARGV[1])
{
	system "make ID_NUMBER=$number @ARGV[2..$#ARGV]";
}