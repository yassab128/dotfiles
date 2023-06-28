#!/bin/sh

# no need for a shebang, it might even confuse lemonbar (not sure though)

set -efu
while true; do
	ariaProgress="$(cat /home/pactrag/Programme/sysinfo/misc/ariaProgress)"
	bspwmStatus="$(cat /home/pactrag/Programme/sysinfo/misc/bspwmStatus)"
	batteryStatus="$(cat /home/pactrag/Programme/sysinfo/misc/battery)"
	fajitaInfo="$(cat /home/pactrag/Programme/sysinfo/misc/fajitaInfo)"
	printf '%s %%{c} %s %%{r} %%{F#3ddc84}%s %%{F#ffcc00}%s\n' \
		"${bspwmStatus}" "${batteryStatus}" "${fajitaInfo}" "${ariaProgress}"
	sleep 1
done
