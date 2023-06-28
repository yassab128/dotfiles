#!/bin/sh

set -efu

url="$(curl -s -X POST \
	--data '{"id": "", "method": "aria2.getGlobalStat"}' \
	http://localhost:6800/jsonrpc)"

tellActive="$(curl -s -X POST \
	--data '{"id": "", "method": "aria2.tellActive"}' \
	http://localhost:6800/jsonrpc)"

emptyResponse='{"id":"","jsonrpc":"2.0","result":[]}'

getGlobalStat() {
	progression="$(perl -e '
	while (<>) {
		my $count = 0;
		while(/(?<="downloadSpeed":")(.*?)(?=",")|
					(?<="numActive":")(.*?)(?=",")|
					(?<="numStopped":")(.*?)(?=",")|
					(?<="numStoppedTotal":")(.*?)(?=",")|
					(?<="numWaiting":")(.*?)(?=",")/xg)
		{
			$count++;

			if ( $count == 1 ) {
				print "downloadSpeed:", sprintf( "%.2f", $& / 1024 ), "KB/s \n";
			}
			elsif ( $count == 2 ) {
				print "numActive: $&\n";
			}
			elsif ( $count == 3 ) {
				print "numStopped: $&\n";
			}
			elsif ( $count == 4 ) {
				print "numStoppedTotal: $&\n";
			}
			else {
				print "numWaiting: $&\n";
			}
		}

	}
	' <<<"$url")"

	printf "%s\n" "$progression"
}

case "$1" in
t | toggleDownloads)
	if [ "$tellActive" == "$emptyResponse" ]; then
		curl -s -X POST \
			--data '{"id": "", "method": "aria2.unpauseAll"}' \
			http://localhost:6800/jsonrpc &>/dev/null
		printf 'all downloads are now resuming\n'
	else
		curl -s -X POST \
			--data '{"id": "", "method": "aria2.pauseAll"}' \
			http://localhost:6800/jsonrpc &>/dev/null
		printf 'all downloads are now paused\n'
	fi
	;;
g | getGlobalStat)
	getGlobalStat
	;;
esac
