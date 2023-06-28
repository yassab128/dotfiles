#!/bin/sh

set -efu

bspc subscribe node_add node_remove desktop_focus | while read -r _; do
    printf 'desktop: %s | windows: %s\n' \
        "$(bspc query --desktops -d focused --names)" \
        "$(bspc query --nodes -n '.window.!floating' -d focused | wc -l)" \
		>"${HOME}/Programme/sysinfo/misc/bspwmStatus"
done
