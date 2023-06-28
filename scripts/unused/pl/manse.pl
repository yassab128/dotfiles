#!/usr/bin/perl -w

use strict;

unless (@ARGV) {
	die "No argument has been given";
}

my $pattern;
foreach (@ARGV) {
	$pattern .= $_ . '|'
}
chop $pattern;

open my $fh, '-|', 'man', '-kw', '.' || die;
while (<$fh>) {
	chomp;
	open my $fd, '<', $_ || die $!;
	read $fd, my $buffer, -s $_;
	close $fd || die $!;
	print $_, "\n" if ($buffer =~ /$pattern/);
}
close $fh || die;
