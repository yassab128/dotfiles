#!/bin/sh

set -efu

/usr/bin/time -f \
	"\nZeit: %es, Prozessor: %P, Ausgangscode: %x" \
	tar cf - "$1" | tar xvf - -C "$2"
