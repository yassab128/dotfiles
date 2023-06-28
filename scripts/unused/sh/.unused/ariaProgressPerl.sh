#!/usr/bin/env bash

set -efu

# battle tested script

while true; do
	url="$(curl -s -X POST \
		--data '{"id": "", "method": "aria2.tellActive"}' \
		http://localhost:6800/jsonrpc)"

	perl -e '
		use strict;
		use warnings FATAL => "all";
		while (<>) {
			my $count           = 0;
			my $completedLength = 0;
			my $downloadSpeed   = 0;
			my $totalLength     = 0;
			while (
				/(?<=,"completedLength":")(.*?)(?=")|
						(?<="downloadSpeed":")(.*?)(?=")|
						(?<="totalLength":")(.*?)(?=")/xg
			  )
			{
				#increment $count by one
				$count++;

				if ( ( $count % 3 ) == 1 ) {
					$completedLength += $&;

				}
				elsif ( ( $count % 3 ) == 2 ) {
					$downloadSpeed += $&;

				}
				else {
					$totalLength += $&;
				}
			}
			print int( $downloadSpeed / 1024 ),
			  " KB/s | ",
			  $count / 3,
			  " | ",
			  sprintf( "%.2f", $completedLength / $totalLength * 100 ),
			  " % \n";
		}
	' <<<"$url" || break
	sleep 1
done
