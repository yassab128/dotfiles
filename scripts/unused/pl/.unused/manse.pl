#!/usr/bin/perl -w

use strict;
use IO::Uncompress::Gunzip;

unless (@ARGV) {
	die "No argument has been given";
}

sub process_gzip {
	my $z = IO::Uncompress::Gunzip->new($_[0]) || die;
	$z->read(my $buffer) || die;
	foreach (@ARGV) {
		if ($buffer =~ /$_/) {
			my $pkg = `pacman -Qqo $_[0]`;
			$_[0] =~ /([^\/]+).gz$/;
			print "$_: $1: $pkg";
		}
	}
	close $z || die;
}

open my $fh, '-|', 'man', '-kw', '.' || die;
while (<$fh>) {
	chomp;
	process_gzip($_);
}
close $fh || die;
