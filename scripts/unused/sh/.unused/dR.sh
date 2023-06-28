#!/bin/sh

set -efu

if [ $# -eq 1 ]; then
	youtube-dl \
		-o "$HOME/Downloads/Music/%(title)s.%(ext)s" \
		-f 'bestaudio[ext=m4a]' \
		--embed-thumbnail \
		--add-metadata \
		--restrict-filenames \
		"$1"
else #--postprocessor-args "-id3v2_version 3" \
	youtube-dl \
		-o "$HOME/Downloads/Videos/%(title)s.%(ext)s" \
		-f "bestvideo[height<=$1]+bestaudio/best[height<=$1]" \
		--add-metadata \
		--restrict-filenames \
		"$2"
fi
