#!/bin/sh

set -efu

while true; do
    RXB=$(</sys/class/net/wlp3s0/statistics/rx_bytes)
    sleep 1
    RXBN=$(</sys/class/net/wlp3s0/statistics/rx_bytes)
    RXDIF=$(((RXBN - RXB) / 1024))
    printf '%s KiB/s\r' "$RXDIF"
done
