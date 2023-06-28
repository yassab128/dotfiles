#!/bin/sh

set -efu

empty="$(find . -maxdepth 1 -empty -exec ls --color -d {} \+ 2>/dev/null)"
printf "\e[1;33mempty \e[0m\n%s\n" "$empty"

directories="$(find . -maxdepth 1 \
    -type d -exec du -sh "{}" \+ 2>/dev/null | sort -h)"
printf "\e[1;33mdirectories \e[0m\n%s\n" "$directories"

files="$(find . -maxdepth 1 \
    -type f -exec du -sh "{}" \+ 2>/dev/null | sort -h)"
printf "\e[1;33mfiles \e[0m\n%s\n" "$files"
