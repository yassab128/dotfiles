#!/bin/sh

set -efu

# these next two lines make it possible to press ctrl+c while in the fzf menu
# and actually exit the bash script instead of proceeding to ffplay. Yeah, both
# are needed
set -e
set -o pipefail

# replaces spaces with '+'
query="$*"
query="${query// /+}"

fetchPlaylists() {
	perl -e '
use strict;
use warnings FATAL => "all";
while (<>) {
    my $count = 0;
    while (
        /
		# playlistName
		(?<=,"title":\{"simpleText":")(.*?)(?="},"thumbnails")|
		 #channelName
		(?<="shortBylineText":\{"runs":\[\{"text":")(.*?)
		(?=","navigationEndpoint")|
		# videosNumber
		(?<="videoCount":")(.*?)(?=","navigationEndpoint")|
		# playlistId
		(?<="playlistRenderer":\{"playlistId":")(.*?)(?=","title")/xg
      )
    {
        $count++;

        # set brackets around videosNumber
        if ( $count == 3 ) {
            print "[\e[1;34m$&\e[0m] ";

            # \e[1;34m is blue,
            # \e[0m resets to the default color
        }
        elsif ( $count == 4 ) {
            print "$&\n";
            $count = 0;
        }
        else {
            print "$& ";
        }
    }
}
    ' <<<"$(curl --silent \
		"https://www.youtube.com/results?search_query=$query&sp=EgIQAw")"
}

fetchVideos() {
	perl -e '
	while (<>) {
		my $count = 0;
		while (/
		# videoName
		(?<=}\]},"title":\{"runs":\[\{"text":")(.*?)(?="}\],"accessibility")|
		# viewCount
		(?<="},"viewCountText":\{"simpleText":")(.*?)(?=\s)|
		# channelName
		(?<="ownerText":\{"runs":\[\{"text":")(.*?)(?=")|
		# videoLength
		(?<="}},"simpleText":")(?:(\d{1,2}):)?(\d{1,2}):(\d{2})(?="},"style":")|
		# videoId
		(?<="addedVideoId":")(.*?)(?=","action")/xg){
			$count++;

			# set parentheses around viewCount
			if ( $count == 2 ) {
				print "(\e[1;34m$&\e[0m) ";
				# \e[1;34m is blue, \e[0m resets to the default color
			}

			# set parentheses around the videoLength
			elsif ( $count == 4 ) {
				print "(\e[1;33m$&\e[0m) ";
				# \e[1;34m is yellow, \e[0m resets to the default color
				   # add new line every 5 videoLength (after the videoId)
			}
			elsif ( $count == 5 ) {
				print "$&\n";

				# set count to 0, otherwise the modulo operator has to be used
				$count = 0;

				# add a space in the other cases (videoName and channelName)
			}
			else {
				print "$& ";
			}
		}
	}
    ' <<<"$(curl --silent \
		https://www.youtube.com/results?search_query="$query")"
}

# check if flags were given (currently there is only `-p`)
if [ $# -ne 0 ]; then
	case "$1" in
	-p | --playlist)

		playlists="$(fetchPlaylists)"
		# the while loop is useful when mpv is used instead of ffplay
		# since you can press q while in mpv and return to fzf menu
		# I do not use mpv anymore so it is commented out
		# while true; do
		URL="$(fzf --ansi --with-nth="2.." <<<"$playlists" |
			# get the first column (videoId)
			awk '{print "https://youtube.com/playlist?list="$1}')"
		youtube-dl "$URL" -f bestaudio -o - |
			ffplay - -nodisp -loglevel quiet
		# done
		exit 0
		;;
	esac
fi

# in case no flags are given
videos="$(fetchVideos)"
# while true; do
videoId="$(fzf --ansi --with-nth="2.." <<<"$videos" |
	# get the last column (videoId)
	awk '{print $NF}')"
curl -o /tmp/viuTemp.jpg http://img.youtube.com/vi/"$videoId"/hqdefault.jpg
viu /tmp/viuTemp.jpg
youtube-dl https://youtu.be/"$videoId" -f bestaudio -o - |
	ffplay - -nodisp -loglevel quiet
# done
