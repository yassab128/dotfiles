#!/bin/sh

set -efu

#cd /home/pactrag/Downloads || exit

#tmp1="$(mktemp)"
tmp1=/tmp/temp1
tmp2=/tmp/temp2
#tmp2="$(mktemp)"

source=/home/pactrag/Downloads/

cd "$source" || exit

# send all .aria2 file to tmp1
find . -type f -name \*.aria2 >"$tmp1"

sed -i 's/\.aria2$//' "$tmp1"

cp "$tmp1" "$tmp2"

sed -i 's/$/.ariatmp/' "$tmp2"

#paste "$tmp1" "$tmp2" | while read -r tmp1 tmp2; do mv "$tmp1" $"$tmp2"; done

#/home/pactrag/Programme/usw/shell/dateiorganisator.sh

#paste "$tmp2" "$tmp1" | while read -r tmp2 tmp1; do mv "$tmp2" $"$tmp1"; done
