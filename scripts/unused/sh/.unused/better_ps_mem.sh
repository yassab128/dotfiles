#!/bin/sh

set -efu

eval "$(ps -eo size=,pmem=,comm= --sort=size | awk '
		{
		if ($1 != 0) {
			printf "%s, %s, ", $1, $2
			for (i = 3; i < NF; ++i) {
				printf "%s ", $i
			}
			printf "%s\n", $NF
		}
	}
' | awk '
BEGIN {
	FS = ","
}

{
	comm[$3]++
	size[$3] += $1
	pmem[$3] += $2
	PROCINFO["sorted_in"] = "@val_num_desc"
}

END {
	for (i in size) {
		printf "%-40s %-4s | %-4s | %.2fM\n",
		i, "(" comm[i] ")", pmem[i] "%", size[i] / 1024
	}
}
' | fzf --header="PRESS ENTER TO KILL" --nth="..-6" | awk '
BEGIN {
	FS = "("
}

{
	print "pkill -x " $1
}

')"
