#!/bin/sh

set -efu

# Very inaccurate, use it only to know top-cpu-consuming program
su -c 'ps -eo pcpu,comm' | sed -e '1d' -e '/^\s0.0\s/d' -e 's/^/1 /' |
	awk '{ seen1[$3] += $1; seen2[$3] += $2 }
    END { for (i in seen1) print i, seen2[i], "%", "("seen1[i]")" }' |
	sed 's/\s(1)$//' | sort --key=2 --numeric-sort
