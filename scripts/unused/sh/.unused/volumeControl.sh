#!/usr/bin/bash

set -efu

# only works with absolute path for every command, even bash
case "$1" in
volumeUp)
    /usr/bin/amixer -q set Master 5%+ unmute
    ;;
volumeDown)
    /usr/bin/amixer -q set Master 5%- unmute
    ;;
esac

#-a "changeVolume" \
#-i audio-volume-muted \
/usr/bin/dunstify \
    --replace=991049 \
    --timeout=1000 \
    --urgency low "$(/usr/bin/awk -F "[][]" 'NR==6 { print $2 }' <<<"$(/usr/bin/amixer sget Master)")"
