#!/usr/bin/perl -w

use File::Fetch;
use strict;

my $url = 'https://github.com/TeamNewPipe/NewPipe/archive/refs/tags/v0.22.1.tar.gz';
my $ff = File::Fetch->new(uri => $url);
my $file = $ff->fetch() || die $!;
