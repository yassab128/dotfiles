#!/bin/sh

# This script shows files in /etc/ that probably (big error margin) do not
# belong to any package (according to pacman). It is good to run this script
# to know the files that will NOT be removed if their corresponding packages
# were removed
awk '
{
	if (NR == FNR) {
		arr[$0] = ""
	} else if ($2 ~ /^\/etc\// && $2 in arr) {
		delete arr[$2]
	}
}
END {
	for (i in arr) {
		print i
	}
}
' /dev/fd/3 /dev/fd/4 3<<-EOF 4<<-EOF
	$(
		find /etc -exec ls -1 --directory \
			--indicator-style=slash '{}' \+ 2>/dev/null
	)
EOF
	$(pacman --query --list)
EOF
