#!/bin/sh


volume_up() {
	ssh -q -c aes128-ctr user@"${TERMUX-}" -p 8022 "$@"
}


case "$1" in
volume_increase)
	amixer -q set Master 5%+ unmute
	volume_up
	;;
volume_decrease)
	amixer -q set Master 5%- unmute
	volume_down
	;;
*)
	printf 'Illegal option!\n'
	;;
esac
