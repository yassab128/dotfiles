#!/bin/sh

set -efu
PATH=/usr/bin/

brightnessLevelNotify() {
	dunstify \
		--replace=991049 \
		--timeout=1000 \
		--urgency low "$(brightnessctl |
			awk 'BEGIN{ FS="[()]" } NR==2 {print $2}')"
}

volumeLevelNotify() {
	dunstify \
		--replace=991049 \
		--timeout=1000 \
		--urgency low "$(
			awk '
			BEGIN {
				FS = "[][]"
			}

			NR == 5 {
				print $2
			}
			' <<-EOF
				$(amixer sget Master)
			EOF
		)"
}

case "$1" in
volume_increase)
	amixer -q set Master 5%+ unmute
	volumeLevelNotify
	;;
volume_decrease)
	amixer -q set Master 5%- unmute
	volumeLevelNotify
	;;
brightness_increase)
	brightnessctl --quiet set 5%+
	brightnessLevelNotify
	;;
brightness_decrease)
	brightnessctl --quiet set 5%-
	brightnessLevelNotify
	;;
*)
	printf 'Illegal option!\n'
	;;
esac
