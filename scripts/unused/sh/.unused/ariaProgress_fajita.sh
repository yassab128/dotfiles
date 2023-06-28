#!/bin/sh

outputFile=/data/data/com.termux/files/usr/tmp/aria2c.txt

if [ -n "$(pgrep --exact aria2c)" ]; then
	pkill aria2c
fi

aria2c >"${outputFile}" 2>&1 &

while true; do

	# remove the notification when downloads are stopped
	termux-notification-remove 3355

	# inotify-wait without inotify-wait
	tail --quiet --lines=0 --follow=name "${outputFile}" |
		head --lines=1

	while true; do
		summary="$(
			curl --silent -X POST \
				--data '{"id": "", "method": "aria2.tellActive"}' \
				http://localhost:6800/jsonrpc | awk '
BEGIN {
	FS = "\""
	completedLength = 0
	downloadSpeed = 0
	totalLength = 0
}
{
	for (i = 1; i <= NF; ++i) {
		if ($i == "totalLength") {
			totalLength += $(i + 2)
		} else if ($i == "downloadSpeed") {
			downloadSpeed += $(i + 2)
		# there are two "completedLength", we have to match the right one
		# and ignore the other that is not proceeded by a comma
		} else if ($i == "completedLength" && $(i - 1) == ",") {
			completedLength += $(i + 2)
		}
	}
}
END {
	if (totalLength == 0) {
		exit 1
	}
	printf "%.2fk | %s | %.2f%%",
	downloadSpeed / 1024, NR, (completedLength / totalLength) * 100
}
		'
		)" || break
		termux-notification \
			--id=3355 \
			--icon file_download \
			--content="${summary}"
		sleep 1
	done

done
