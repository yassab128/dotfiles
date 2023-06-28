#!/bin/sh

set -efu

folders='Compressed Documents Pictures Music Executables Videos'
ext_comp="*.zip *.rar *.7z *.gzip *.gz *.tar *.arj *.tar.gz *.tar.xz"
ext_doc="*.docx *.pdf *.doc *.txt *.rtf *.odt *.tex *.docm *.wks *.wps
*.ppt *.ods *.pptx *.xlr *.xlt *.xls *.xlsx *.xml *.key *.rss cer
*.djvu *.fb2 *.fb3 *.mobi *.epub *.azw *.lit *.odf *.kfx *.src.rpm"
ext_pic="*.jpg *.jpeg *.peg *.png *.gif *.tiff *.tif *.svg *.bmp
*.ai *.ico *.icns *.ppm *.pgm *.pnm *.pbm *.bgp *.webp"
ext_music="*.mp3 *.aac *.wav *.ogg *.flac *.wma *.ac3 *.pcm *.aiff
*.alac *.wpl *.aa *.act *.ape *.m4a *.m4p *.oga *.mogg *.tta"
ext_exe="*.exe *.apk *.com *.deb *.msi *.dmg *.bin *.vcd
*.pl *.cgi *.jar *.py *.wsf *.rpm"
ext_vid="*.mkv *.avi *.3gp *.3g2 *.mov *.bik *.wmv *.flv *.swf *.m2v
*.m2p *.vob *.ifo *.mp4 *.m4v *.mpg *.asf *.mpeg *.mpv *.qt *.webm *.ogv"

for folder in $folders; do
    if [ "$folder" == 'Compressed' ]; then
        mkdir -p "$folder"
        # shellcheck disable=SC2086
        mv $ext_comp "$folder" 2>/dev/null

    elif [ "$folder" == 'Documents' ]; then
        mkdir -p "$folder"
        # shellcheck disable=SC2086
        mv $ext_doc "$folder" 2>/dev/null

    elif [ "$folder" == 'Pictures' ]; then
        mkdir -p "$folder"
        # shellcheck disable=SC2086
        mv $ext_pic "$folder" 2>/dev/null

    elif [ "$folder" == 'Music' ]; then
        mkdir -p "$folder"
        # shellcheck disable=SC2086
        mv $ext_music "$folder" 2>/dev/null

    elif [ "$folder" == 'Executables' ]; then
        mkdir -p "$folder"
        # shellcheck disable=SC2086
        mv $ext_exe $folder 2>/dev/null

    elif [ "$folder" == 'Videos' ]; then
        mkdir -p "$folder"
        mv "$ext_vid" "$folder" 2>/dev/null

    else
        echo "$(tput setaf 3)Problem creating folder..$(tput sgr0)"
    fi

done

echo "$(tput setaf 2)Successfully organized..$(tput sgr0)"
sleep 1 && find . -maxdepth 1 -type d -empty -delete
