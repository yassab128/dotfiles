#!/bin/sh

{
	curl --silent --request POST \
		--data '{
  		"id": "",
			"method": "aria2.tellStopped",
			"params": [0, 999]
		}' \
		http://localhost:6800/jsonrpc

	curl --silent --request POST \
		--data '{
   		"id": "",
			"method": "aria2.tellWaiting",
			"params": [0, 999]
		}' \
		http://localhost:6800/jsonrpc

	curl --silent --request POST \
		--data '{
			"id": "",
			"method": "aria2.tellActive"
		}' \
		http://localhost:6800/jsonrpc
} | tee /dev/tty |
	awk '
BEGIN {
	FS = "\""
	ORS = " "
}

{
	for (i = 1; i <= NF; ++i) {
		if ($i == "gid") {
			print $(i + 2)
		} else if ($i == "completedLength" && 
			$(i - 1) == ",") {
			print $(i + 2)
		} else if ($i == "downloadSpeed") {
			print $(i + 2)
		} else if ($i == "status" && 
			$(i - 1) == ",") {
			print $(i + 2)
		} else if ($i == "totalLength") {
			print $(i + 2), "\n"
		} else if ($i == "path") {
			filename = $(i + 2)
			sub(/.*\\\//, "", filename)
			print filename
		}
	}
}

'
