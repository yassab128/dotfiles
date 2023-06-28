#!/bin/sh

set -efu

# battle tested script

# use this if you want to get the progression of each download separately
#awk '{key=$0; getline; print sprintf("%.2f", (key / $0) * 100) }'

emptyResponse='{"id":"","jsonrpc":"2.0","result":[]}'

# this is dumb, but it is the only way to make shfmt behave right
# around strings
regexMatch='(?<="totalLength":")(.*?)(?=","uploadLength")|'
regexMatch+='(?<="downloadSpeed":")(.*?)(?=")|'
regexMatch+='(?<=,"completedLength":")(.*?)(?=")'

while true; do

    # '--quiet' is needed twice to make inotifywait shut up
    printf "\n"
    inotifywait --quiet /tmp/ariaParser.txt --quiet

    while true; do
        url="$(curl -s -X POST --data '{"id": "", "method": "aria2.tellActive"}' \
            http://localhost:6800/jsonrpc)"
        progression="$(grep -Po "$regexMatch" <<<"$url" |
            awk '{completedLength+=$0; getline; downloadSpeed+=$0; getline; totalLength+=$0}
        END {print sprintf("%.2f", downloadSpeed / 1024), "KB/s", "|",
        NR / 3, "|",  sprintf("%.2f", (completedLength / totalLength) * 100), "%"}')"

        printf "%s\n" "$progression"

        sleep 1
        # in case aria2c is not running ( $url will be then empty)
        # or if no download is running ( $url == "$emptyResponse)
        if [ -z "$url" ] || [ "$url" == "$emptyResponse" ]; then
            break
        fi
    done

done
