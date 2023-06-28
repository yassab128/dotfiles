#!/bin/sh -eu

IFS=':'
find ${PATH} -maxdepth 1 -type f -exec file --mime-type '{}' '+' |
	tr -s ' ' | sort -k2
