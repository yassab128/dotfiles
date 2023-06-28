#!/bin/sh

set -efu

# if `find .` is used instead of `find ./*`,
# mv will try operating on '.' itself (as a string)
# which is not wanted
find ./* -maxdepth "$1" -exec sh \
	-c 'mv "$1" "$(iconv -f UTF-8 -t ASCII//TRANSLIT<<<"$1" |
	sed "s/\s/_/g")"' -- {} \+
