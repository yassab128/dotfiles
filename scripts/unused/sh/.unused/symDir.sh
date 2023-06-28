#!/bin/sh

set -efu

path_with_binary="$(readlink -e ./"$1")" &&
	printf '%s\n' "$(sed "s/[^/]*$//g" <<<"$path_with_binary")" ||
	path_with_binary="$(readlink -e "$(which "$1")")" &&
	printf '%s\n' "$(sed "s/[^/]*$//g" <<<"$path_with_binary")" || exit
