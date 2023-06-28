#!/bin/sh

set -efu
read -r W H X Y <<-EOF
	$(slop -f "%w %h %x %y")
EOF
# gif
# ffmpeg -f x11grab -video_size "$W"x"$H" -r 1 -i :0.0+"$X","$Y" output.gif
# screenshot
ffmpeg -f x11grab -video_size "$W"x"$H" -i :0.0+"$X","$Y" -vframes 1 output.png
