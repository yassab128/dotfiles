#!/bin/sh

set -efu

notification_output="$(
	/usr/bin/awk '
	(NR == 2) {
		print $1 ":", $5, "(" $3 "/" $4 ")"
	}
	' <<-EOF
		$(/usr/bin/df --human-readable \
			--portability /dev/disk/by-label/arch_linux)
	EOF
)"

${0%/*}/notify.sh "'${notification_output}'" 0 10000
