#!/usr/bin/awk -f

BEGIN {
	file = "/sys/class/power_supply/BAT0/capacity"
	getline capacity < file
	file = "/sys/class/power_supply/AC/online"
	getline < file
	printf "%s%d\n", /^1$/ ? "+" : "-", capacity
}
