#!/bin/sh

set -efu

exec "$(/usr/bin/find \
	/var/usr/* \
	/usr/bin/* \
	-printf '%f\n' | /usr/bin/fzf --tac)"' &'
