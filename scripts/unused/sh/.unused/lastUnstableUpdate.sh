#!/bin/sh

set -efu

# I am not rewriting this in grep/sed/awk yet before I am totally sure
# that this is the right way to find update time
curl --silent https://hydra.nixos.org/job/nixos/trunk-combined/tested |
	perl -e '
    use strict;
    use warnings FATAL => "all";
    while (<>) {
        if (/Succeeded/) {

            # if the latest update is released less than 7 days
            # the date will show something like 3h or 5d
            # we need to match that

            # look-around assertions ">" and "<" are used here (they
            # help searching but are not included in the matched string)
            if (
                /(?<=\>)
    # match xx or x digit
    ((\d|\d\d)
    # match either h (hours) or d (days) && " ago"
    (d|h)\sago|
    # in the other case, it will show a date
    # like 2020-09-23, we need to match that too
    202\d-\d\d-\d\d)(?=\<)/x
              )
            {
                print "$&\n";
                exit 0;
            }
        }
    }
    '
