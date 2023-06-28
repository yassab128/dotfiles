#!/bin/sh

set -efu

# all the 0s are necessary in order to handle directories with spaces
clear && find . -maxdepth 2 -exec ls -1 --indicator-style=slash --color --directory '{}' \+ |
	sed --silent '
	# match depth 1 files (they do not end with a slash)
/\.\/[^/]\+$/ {
	# add them to the `H`old space
	H
}
# if the match above was not found do this
/\.\/[^/]\+$/ ! {
	# add indentation to the contents of the directories
	s/\.\/[^/]\+\/\([^$]\)/├─────────\1/
	# and `p`rint them
	p
}
# match the last line
$ {
	# e`x`change the hold space with pattern space
	x
	# remove a blank line that gets added for some reason
	s/\n//
	p
}
'
