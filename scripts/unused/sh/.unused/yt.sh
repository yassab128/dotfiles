#!/bin/sh

set -efu
# search videos on youtube and play them in mpv, without an API key
# https://raw.githubusercontent.com/sayan01/scripts/master/yt
# Sayan G

if [ "$1" == "" ]; then
    echo -n "Search: "
    read -r query
else
    query="$*"
fi
if [ "$query" == "" ]; then
    exit
fi
query=$(sed 's/ /+/g' <<<"$query")

videoids=$(
    curl -s https://www.youtube.com/results?search_query="$query" |
        grep -oP "\"videoRenderer\":{\"videoId\":\"...........\".+?\"text\":\".+?(?=\")" |
        awk -F\" '{ print $6 " " $NF}'
)
while true; do
    videoid=$(
        echo -e "$videoids" |
            #fzf --with-nth='2..-1' |
            fzf |
            cut -d' ' -f1
    )
    if [ "$videoid" == "" ]; then
        exit
    fi
    mpv --no-video https://youtu.be/"$videoid"
done
