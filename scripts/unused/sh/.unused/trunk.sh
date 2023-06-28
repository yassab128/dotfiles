#!/bin/sh

set -efu

firstLevel() {
clear && find . -maxdepth 1 -mindepth 1 -exec ls --group-directories-first \
	--indicator-style=slash -1 --color '{}' \+ | sed '
/:$/, /^$/{
	//!s/^/|——————/
	/^$/d
}
'
}


secondLevel() {
clear && find . -maxdepth 2 -mindepth 1 -exec ls --group-directories-first \
	--indicator-style=slash -1 --color '{}' \+ | sed '
# match from the beginning to the first blank line
1, /^$/{
	# delete everything except level 1 files
	/m\.\/[^/]\+$/! {
		d
	}
}

# match lines ending with `:` to the next blank line
/:$/, /^$/{

	# add indentation with dashes(exclusive)
	//! s/^/|——————/

	# match level 1 directories
	/^\.\/[^/]\+:$/, /^$/{
	# delete subdirectories (recognisable due the appended slash)
		/\/$/ d
	}

	# match level 2 directories
	/^\.\/[^/]\+\/[^/]\+:$/, /^$/{
		# exclude the blank line (modifying it will bite later)
		/^$/! {
			# add indentation with spaces
			s/^/       /
		}
		# replace A/B: with |B:
		s/\.\/[^/]\+\//|/
	}

}

# delete all the blank lines
/^$/ d
'
}

while getopts '12' option; do
	case "$option" in
	1)
		firstLevel
		;;
	2)
		secondLevel
		;;
	?)
		;;
	esac
done
