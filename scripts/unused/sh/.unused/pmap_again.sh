#!/bin/sh

# pmap ignores processes (like aria2c, sshd), it is absolutely bad
# so much work, just to find out pmap is absoltely broken
# run this script to see by yourself that some commands have no
# memory usage size besides them (guess what, pmap cannot calculate their
# rss usage based on pid, but ps can, USE PS)

cd /proc || exit
awk '
{
	if (NR == FNR) {
		if ($0 ~ /^[0-9]+:[[:blank:]]/) {
			sub(/:[[:blank:]].*/, "", $0)
			pid = $0
		} else if ($0 ~ /^total/) {
			# printf "%s %.02fM\n", pid, $4 / 1024
			rss = sprintf("%.02f", $4 / 1024)
			fd3[pid] = rss
		}
	} else {
		pid = $1
		comm = $0
		sub(/^[[:blank:]]*[0-9]+[[:blank:]]/, "", comm)
		fd4[pid] = comm
		arr[comm] = ""
	}
}

END {
	for (i in fd4) {
		if (i in fd3) {
			rss = fd3[i]
			comm = fd4[i]
			if (i in fd3 && comm in arr) {
				arr[comm] += rss
				# ++instances[comm]
			} else {
				arr[comm] = rss
				# instances[comm] = 1
			}
		}
	}
	for (i in arr) {
		if (arr[i] > 0) {
			print arr[i], i
		}
	}
}
' /dev/fd/3 /dev/fd/4 3<<-EOF 4<<-EOF
	$(find -- * -maxdepth 0 -name '[0-9]*' -exec pmap -x {} \+)
EOF
	$(ps -eo pid=,comm=)
EOF
