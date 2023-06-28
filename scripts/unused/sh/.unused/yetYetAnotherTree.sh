#!/bin/sh

set -efu

clear && find . -maxdepth 3 -exec ls --group-directories-first -p -1 --color '{}' \+ | sed '
# using any non ascii character slows down sed considerably
1, /\.\/[^/]\+$/{
	d
}

/^[^/]\+:$/,/^$/{
	# contains 6
	//!s/^/|------/
	/\/$\|^$/ d
}

/\/[^/]\+$/,/^$/{
	/^$/ d

	/\/[^/]\+$/!{
		# contains 12
		//!s/^/|      |-------/
	}

	/\/[^/]\+$/{
		s/^[^/]\+\//|-------/
	}
}
'
