#!/bin/sh

PATH=/usr/bin/
set -efu

backup_directory="${HOME}/Programme/root"

[ -d "${backup_directory}" ] || mkdir "${backup_directory}"

rm -fr "${backup_directory}/*"

# You can remove `! -perm '-o=r' -prune -o` but you have to redirect stderr
# to /dev/null or run the script as root.
commands() {
	find /etc/ ! -perm '-o=r' -prune -o -type f ! -empty -exec awk '
BEGIN {
	to_backup = ""
}
{
	if ($0 ~ /vi!/) {
		to_backup = to_backup " " FILENAME
	}
	# Skip to the next file if the first line does not match
	nextfile
}

END {
	# `-P` for `tar` hides all "Removing leading `/` from member names"
	# messages. P is short `--absolute-paths` (the short form does not work
	# here somwhow).
	print "tar -Pcf /dev/stdout" to_backup,
	"| tar --directory '"${backup_directory}"' -xf /dev/stdin"
}
' '{}' \+
}

# commands && exit

eval "$(
	commands
)" 2>/dev/null
# redirecting /dev/stderr to /dev/null gets rid of the seconds tar message
# `tar: Removing leading `/' from member names` that I could not solve.
