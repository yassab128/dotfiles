#!/bin/sh -eu

# for i in /proc/[0-9]*; do
# 	test -r $i/fd || continue
# 	files=$(readlink $i/fd/*) || continue
# 	read comm < $i/comm
# 	echo -e "\n${i##*/}, \e[1;31m$comm\e[0m\n$files"
# done

for i in /proc/[0-9]*; do
	test -r $i/fd || continue
	read comm < $i/comm
	echo -e "\n${i##*/}, \e[1;31m$comm\e[0m"
	ls -al --full-time $i/fd
done
