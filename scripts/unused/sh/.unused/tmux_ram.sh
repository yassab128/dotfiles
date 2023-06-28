#!/bin/sh

set -efu

while :; do
    #ram="$(awk '{if(NR==1) a=$2; if(NR==2) b=$2}END {print int((a-b)/1024) " MB"}' /proc/meminfo)"
    #ram="$(awk '{if(NR==2) a=$2}END {print int(100 - (a/16777216)*100) " %"}' /proc/meminfo)"
    used_memory_percentage="$(awk '{if(NR==2) a=$3;
        if(NR==2) b=$2}END {print int(a/b * 100) " %"}' <<<"$(free -m)")"
    printf '%s\r' "$used_memory_percentage"
    sleep 1
done
