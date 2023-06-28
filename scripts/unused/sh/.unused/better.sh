#!/bin/sh

set -efu

eval "$(
	import - | awk '
    /^DisplayImage$/{
        getline
        print "X=" $1
        print "Y=" $2
        getline
        print "W=" $1
        print "H=" $2
        exit
        }
'
)"

choice="$(printf '\ngif\npng\n' | fzf)"
if [ "$choice" = "png" ]; then
	ffmpeg -y -f x11grab -s "$W"x"$H" -r 30 -i :0.0+"$X","$Y" -pix_fmt rgb24 -loop 0 output.gif
else
	ffmpeg -f x11grab -s "$W"x"$H" -i :0.0+"$X","$Y" -vframes 1 output.png
fi
