#!/bin/sh

set -efu

# ll.sh made this script redundant
du -hs -- .[!.]* * | sort -h
