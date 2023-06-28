#!/bin/sh

set -efu
TERMUX=192.168.1.173

battery="$(
	/usr/bin/awk '
{
	# if ($0 > 70) {
	# 	print " "
	# 	exit
	# }
	percentage = $0 "%"
	getline
	if ($0 == "Charging") {
		status = "+"
	} else {
		status = "-"
	}
	print status percentage
	exit
}
' <<-EOF
		$(
			/usr/bin/ssh -C -q -c aes128-ctr user@"${TERMUX}" -p 8022 "
		/system/bin/su -c '
			/system/bin/cat /sys/class/power_supply/battery/capacity
			/system/bin/cat /sys/class/power_supply/battery/status
			'
		"
		)
	EOF
)"

[ "${battery}" != " " ] &&
	DBUS_SESSION_BUS_ADDRESS='unix:path=/run/user/1000/bus' /usr/bin/dunstify \
	--urgency low --timeout=2000 "Termux ${battery}"
