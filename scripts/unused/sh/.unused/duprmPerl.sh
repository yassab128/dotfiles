#!/bin/sh

set -efu

# NOTE: md5 gets the job done efficiently
# here, perl grabs the list of all files, something like
# HASH FILE
find . -maxdepth 1 -type f -exec md5sum {} \+ |
	perl -e '
		use strict;
		use warnings FATAL => "all";
		my %hash = ();
		while (<>) {
		my @F = split(" ");

		# in the next 4 lines, perl tries to keep only files with
		# duplicate hash values
		$hash{ $F[0] } .= $_;

		END {
			my @array = ();
			$hash{$_} =~ tr/\n// >= 2 &&

			  # add  the files with duplicate hash to an array
			  push @array, "$hash{$_}" for keys %hash;

			foreach (@array) {

				# sort the files by length
				# I do not know why split is needed in this case
				# but it is needed
				my @K = sort { length($a) <=> length($b) } split /^/m, $_;

				# index 0 will not be included, because it represents the file
				# with the shortest filename, it is to be kept
				foreach ( @K[ 1 .. $#K ] ) {
					my @separated = split( " ", $_ );
					print "$separated[1]\n";
				}
			}
		}
	} 
    '
	# | tee /dev/tty | xargs rm # tee /dev/tty prints to stdout, just in case
