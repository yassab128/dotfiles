#!/usr/bin/perl

use strict;
use warnings FATAL => 'all';

# Perl core modules can only deal with tars and zips (extraction only).

# Adapted from "https://github.com/davidbmitchell77/Perl-Sample-Scripts/" \
# "blob/e6aba512b664b897e31b2dee386b9a79030c28ed/unzip.pl"

use IO::Uncompress::Unzip qw(unzip $UnzipError);

my $zipfile = $ARGV[0];

my $u = IO::Uncompress::Unzip->new($zipfile);

my $basename = $zipfile;
$basename =~ s/.*\///;

for ( my $i = 1 ; $i > 0 ; $i = $u->nextStream() ) {
    my $name = $u->getHeaderInfo->{Name};
    if ( $name =~ /\/$/ ) {
        mkdir "$name";
    }
    else {
        # `Name' in the content of the file.
        unzip $zipfile => "$name", Name => "$name";
        print "$zipfile => $name\n";
    }
}
