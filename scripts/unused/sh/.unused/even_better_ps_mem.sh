#!/bin/sh

set -efu

eval "$(ps -eo size=,pid= | awk '
BEGIN {
	ORS = " "
	print "pmap "
}

{
	if ($1 != 0) {
		print $2
	}
}

')"
