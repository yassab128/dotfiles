#!/bin/sh

set -efu
for i in {1..1000}; do
    #sleep 0.1;
    #echo "Welcome $i times"
    ssh -p "$i" user@192.168.1.100
    sleep 0.1
done
