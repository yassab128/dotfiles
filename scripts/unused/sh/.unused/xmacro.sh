#!/bin/sh

set -efu

PATH=/usr/bin/

sendNotification() {
	dunstify \
		--replace=991045 \
		--timeout=0 \
		--urgency normal "$1"
}

case "$1" in
record)
	# the reason why I do not check if record is running here
	# instead of play is because record locks ctrl+shift+Insert and so
	# it needs another keybinding to get the job done
	if pgrep --exact xmacroplay; then
		dunstify --close=991045
		pkill --exact xmacroplay
		dunstify --timeout=1000 "xmacroplay killed"
		exit 0
	fi
	sendNotification "xmacrorec2 is running"
	xmacrorec2 -k 118 -o /tmp/xmacro.majn
	dunstify --close=991045
	;;
play)
	# same situation as the previous comment
	if pgrep --exact xmacrorec2; then
		dunstify --close=991045
		pkill --exact xmacrorec2
		dunstify --timeout=1000 "xmacrorec2 killed"
		exit 0
	fi
	sendNotification "xmacroplay is running"
	xmacroplay </tmp/xmacro.majn
	dunstify --close=991045
	;;
*)
	printf 'Illegal option!\n'
	;;
esac
