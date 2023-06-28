#!/bin/sh

set -efu

# FIXME: there's no straight way to find the total RAM usage of a program
# in Linux. This way lies madness

#ps -eo pmem,size,command
su -c 'ps -eo size,pmem,comm' | sed -e '1d' -e '/^\s\s\s\s0/d' -e 's/^/1 /' |
	awk '{ seen1[$4] += $1; seen2[$4] += $2; seen3[$4] += $3 }
    END { for (i in seen2)
    print i, seen3[i], "%",
    sprintf("%.1f", seen2[i]/1024),"MB", # sprintf output 2 decimal places only
    "("seen1[i]")" }' |
	sort --key=2 --numeric-sort | sed 's/0\s\|(1)//'
