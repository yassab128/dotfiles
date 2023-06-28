#!/bin/sh

set -efu

MOUSE_ID="$(
	# I do not not know if this is even necessary: sed usually extracts `4` so I
	# might just hardcode it and be done with, but I do not know whether it is
	# really a constant
	xinput --list | sed --silent '
/XTEST/ {
	s/.[^0-9]\{1,\}\([0-9]\).*/\1/
	p
	q
}
'
)"

STATE=$(xinput --query-state "${MOUSE_ID}" | grep 'button\[1\]=up')

if [ -n "${STATE}" ]; then
	printf "UP\n"
	xdotool mousedown 1
else
	printf "DOWN\n"
	xdotool mouseup 1
fi
