#!/bin/sh

set -efu

# reduce file size by 1/4
clip=/home/pactrag/Programme/usw/bakup/user/dumpster/Zwischenablage.txt

# if the line number is > 200, decrease it by 50
if (("$(wc -l <$clip)" > 200)); then
    sed -i 1,50d "$clip"
fi

# remove 1/4 every time
#numberOfLines="$(wc -l <"$clip")"
#halfNumberOfLines=$((numberOfLines / 4))
#halfNumberOfLinesRounded="$(awk '{print int($1)}' <<<"$halfNumberOfLines")"
#sed -i 1,"$halfNumberOfLinesRounded"d "$clip"

clipboard_content_separator="$(for _ in {1..80}; do printf '#'; done)"
while clipnotify; do
    SelectedText="$(timeout 2 xsel -p)"
    CopiedText="$(timeout 2 xsel -b)"
    if [[ $CopiedText == "$SelectedText" ]]; then
        printf '%s\n%s\n' \
            "$clipboard_content_separator" "$CopiedText" >>"$clip"
    fi
done
