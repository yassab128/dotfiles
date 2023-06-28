#!/bin/sh

set -efu

currentLayout="$(setxkbmap -print | awk 'BEGIN { FS = "+" }; NR==5 {print $2}')"
# the brackets are needed
if [ "$currentLayout" = "us" ]; then
    setxkbmap ar
else
    setxkbmap us
fi
