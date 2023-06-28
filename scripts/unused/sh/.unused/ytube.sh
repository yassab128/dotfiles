#!/bin/sh

set -efu

#if [ "$1" == "" ]; then
    #echo -n "Search: "
    #read -r query
#fi
#else
#fi
#if [ "$query" == "" ]; then
    #exit
#fi

query="$*"
url="$(curl -s https://www.youtube.com/results?search_query="${query// /+}")"

# extract some information using grep with Perl's regex (it's better than POSIX's)
# '(?<=foo)(.*?)(?=bar)' in regex basically extracts every match between 'foo' and 'bar'
# this only works with Perl's regex though
videoId="$(grep --only-matching --perl-regexp '(?<="addedVideoId":")(.*?)(?=","action")|'\
'(?<=}\]},"title":{"runs":\[{"text":")(.*?)(?="}\],"accessibility")|'\
'(?<="}},"simpleText":")([0-9][0-9]:[0-9][0-9]:[0-9][0-9])(?="},"viewCountText")|'\
'(?<="}},"simpleText":")([0-9]:[0-9][0-9]:[0-9][0-9])(?="},"viewCountText")|'\
'(?<="}},"simpleText":")([0-9][0-9]:[0-9][0-9])(?="},"viewCountText")|'\
'(?<="}},"simpleText":")([0-9]:[0-9][0-9])(?="},"viewCountText")|'\
'(?<="ownerText":{"runs":\[{"text":")(.*?)(?=")' <<<"$url" |

# serround time with parentheses and join every four lines
# with space as a delimiter
#sed -e '2~4 s/^/(/' -e '2~4 s/$/)/' | sed 'N;N;N;s/\n/ /g' | # g in sed is necessary
sed -e '2~4 s/^/(/' -e '2~4 s/$/)/' | sed 'N;N;N;s/\n/ /g' | # g in sed is necessary
    # show the grepped stuff (that is on one line now) but don't show
    # the last field (that is the videoId) with fzf
    fzf --with-nth='1..-2' |
        # get the videoId of the selected video
        sed 's/.* //')"

mpv --no-video https://youtu.be/"$videoId"
