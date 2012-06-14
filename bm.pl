#!/usr/bin/perl

use strict;
use warnings;

use Benchmark qw(cmpthese);

$| = 1;

my $data = 'test.data';
my $bin = './natsort';

my @opts = ('-a', '-A', '-c', '');
#my @opts = ('-c', '');

my %sub;
for my $opt (@opts) {
    my @args = (grep(length $_, split /\s+/, $opt), '-q', $data);
    $sub{$opt} = sub {
        system $bin, @args;
        print '.';
    };
}

cmpthese(5, \%sub);


