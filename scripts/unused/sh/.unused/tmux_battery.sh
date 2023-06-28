#!/bin/sh

set -efu

while true; do
    RXB=$(</sys/class/power_supply/AC/online)
    RXBN=$(</sys/class/power_supply/BAT0/capacity)
    if [ "$RXB" == "1" ]; then
        printf '+%s\r' "$RXBN"
    else
        printf -- '-%s\r' "$RXBN"
    fi
    sleep 1
done
