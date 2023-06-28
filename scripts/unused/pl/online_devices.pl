#!/usr/bin/perl

use strict;
use warnings;
use Net::Ping;

my $host;
for (my $i = 104; $i < 256; ++$i) {
	my $p = Net::Ping->new("udp", 0.5, 1) || die $!;
	$host = "192.168.8.$i";
	if ($p->ping($host)) {
		print "$host\n";
	}
	$p->close() || die $!;
	if ($i == 105) {
		last;
	}
}
