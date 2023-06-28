#!/usr/bin/env bash

set -efu

# the script is split to two
# I should have actually made it two separate
# scripts, but I got to work, pfui

outputFile=/tmp/qualityOfLife.txt
# if more than one argument is given
if [ $# -gt 1 ]; then
	# I am using a string here because an array did not work
	# do not try to implement it using an array
	# you will waste your time

	# why ${@:2} instead of just $@ you might ask
	# well, ${@:2} makes it possible to skip
	# the first given element, that is the flag
	paths="$(realpath "${@:2}")"

	while read -r line; do
		if [ ! -d "$line" ] && [ ! -f "$line" ]; then
			printf 'not a valid path\n'
			exit 1
		fi
	done <<<"$paths"

	case "$1" in
	-o | --overwrite)
		truncate -s 0 "$outputFile"
		while read -r line; do
			printf '%s\n' "$line" >>"$outputFile"
		done <<<"$paths"
		printf 'overwritten successfully\n'
		;;
	-a | --add)
		while read -r line; do
			printf '%s\n' "$line" >>"$outputFile"
		done <<<"$paths"
		printf 'added successfully\n'
		;;
	esac
fi

if [ $# -eq 1 ]; then
	case "$1" in
	-p | --print)
		cat "$outputFile"
		;;
	-c | --copy)
		xargs --arg-file="$outputFile" cp --target-directory=./.
		printf 'copied successfully\n'
		;;
	-y | --yank-path)
		xsel -ib <<<"$(cat "$outputFile")"
		printf 'path(s) yanked successfully\n'
		;;
	-m | --move)
		xargs --arg-file="$outputFile" mv --target-directory=./.
		truncate -s 0 "$outputFile"
		printf '\e[1;31mmoved\e[0m successfully\n'
		;;
	esac
fi
