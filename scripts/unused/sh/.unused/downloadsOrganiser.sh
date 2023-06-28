#!/bin/sh

set -efu

# XXX THIS SHIT IS BROKEN, CANNOT BE DONE IN SH, USE PERL
downloadsFolder=/home/pactrag/Downloads
cd "$downloadsFolder" || exit
mkdir ./Compressed ./Documents ./Pictures ./Music,Executables ./Videos

# backup incomplete downloads by changing their names
find "$downloadsFolder" -type f -name \*.aria2 |
	while read -r downloadable; do
		withoutAria2="$(
			sed 's/\.aria2//' <<-EOF
				$downloadable
			EOF
		)"
		downloadable="$downloadable"temp
		mv "$withoutAria2" "$downloadable" 2>/dev/null
	done

# some known extensions
ext_comp=' zip rar 7z gzip gz tar arj tar.gz tar.xz '
ext_doc=' docx pdf doc txt rtf odt tex docm wks wps ' \
	' ppt ods pptx xlr xlt xls xlsx xml key rss cer ' \
	' djvu fb2 fb3 mobi epub azw lit odf kfx src.rpm '
ext_pic=' jpg jpeg peg png gif tiff tif svg bmp ' \
	' ai ico icns ppm pgm pnm pbm bgp webp '
ext_music=' mp3 aac wav ogg flac wma ac3 pcm aiff ' \
	' alac wpl aa act ape m4a m4p oga mogg tta '
ext_exe=' exe apk com deb msi dmg bin vcd ' \
	' pl cgi jar py wsf rpm '
ext_vid=' mkv avi 3gp 3g2 mov bik wmv flv swf m2v ' \
	' m2p vob ifo mp4 m4v mpg asf mpeg mpv qt webm ogv '

find . -maxdepth 1 -type f |
	while read -r file; do
		# awk gets the extension
		# do not remove " " around $NF
		extension="$(awk 'BEGIN{ FS="." } {print " "$NF" "}' <<<"$file")"
		if [ "$ext_comp" =~ $extension ]; then
			mv "$file" Compressed/
		elif [[ "$ext_doc" =~ $extension ]]; then
			mv "$file" Documents
		elif [[ "$ext_pic" =~ $extension ]]; then
			mv "$file" Pictures
		elif [[ "$ext_music" =~ $extension ]]; then
			mv "$file" Music
		elif [[ "$ext_exe" =~ $extension ]]; then
			mv "$file" Executables
		elif [[ "$ext_vid" =~ $extension ]]; then
			mv "$file" Videos
		fi
	done

# remove empty directories
find . -maxdepth 1 -type d -empty -delete

# return the previous file names
for downloadable in "$downloadsFolder"/*; do
	mv -- "$downloadable" "${downloadable%.aria2temp}" 2>/dev/null
done
