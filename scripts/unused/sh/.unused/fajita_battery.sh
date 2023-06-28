#!/bin/sh

set -efu

# needed for polybar

charging_status="$(ssh -q -c aes128-ctr \
	user@"$TERMUX" -p 8022 'su -c "cat /sys/class/power_supply/battery/status"')"
charging_percentage="$(ssh -q -c aes128-ctr user@"$TERMUX" -p 8022 \
	'su -c "cat /sys/class/power_supply/battery/capacity"')"
#uevent="$(ssh -q -c aes128-ctr user@"$TERMUX" -p 8022 'dumpsys battery')"
#status="$()"

if [ "$charging_status" = "Charging" ]; then
	printf '+%s %%' "$charging_percentage"
else
	# not using `printf --` makes the dash interpreted as a command
	printf -- '-%s %%' "$charging_percentage"
fi
