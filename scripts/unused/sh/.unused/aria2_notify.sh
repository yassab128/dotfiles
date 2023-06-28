#!/bin/sh

set -efu

export PATH=/usr/bin

# Exit if already running
awk '
BEGIN {
	instance = 0
}

{
	if (NF == 2 && $2 == "'"$0"'") {
		if (++instance == 3) {
			exit 1
		}
	}
}
' <<-EOF
	$(ps -eo command=)
EOF

cleanup() {
 dunstify --timeout=1000 "ended"
 exit
}
trap cleanup EXIT INT TERM

dunstify --timeout=1000 "started"

sleep 1
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
		beingDownloaded = 0
	}
	{
		for (i = 1; i <= NF; ++i) {
			if ($i == "totalLength") {
				++beingDownloaded
				totalLength += $(i + 2)
			} else if ($i == "downloadSpeed") {
				downloadSpeed += $(i + 2)
			# there are two "completedLength", we have to match the right one
			# and ignore the other that is not proceeded by a comma
			# NOTE `/,$/` used instead of just `","` because the latter 
			# does not work with torrents
			} else if ($i == "completedLength" && $(i - 1) ~ /,$/) {
				completedLength += $(i + 2)
			}
		}
	}
	END {
		if (totalLength == 0) {
			exit 1
		}
		printf "%.2fk | %s | %.2f%%\n",
		downloadSpeed / 1024,
		beingDownloaded,
		(completedLength / totalLength) * 100
	}
	'
 )" || break
 dunstify --replace=494 --timeout=1000 "${summary}"
 sleep 1
done
