#!/bin/sh

set -efu

while true; do
	netspeed="$(awk 'NR==2' /home/pactrag/Programme/sysinfo/misc/netspeed)"
	ramUsed="$(cat /home/pactrag/Programme/sysinfo/misc/ramUsed)"
	cpu="$(cat /home/pactrag/Programme/sysinfo/misc/cpu)"
	battery="$(cat /home/pactrag/Programme/sysinfo/misc/battery)"
	printf "NS: %sk | MEM: %s | CPU: %s | BAT: %s\r" \
		"${netspeed}" \
		"${ramUsed}" \
		"${cpu}" \
		"${battery}"
	sleep 1
done
