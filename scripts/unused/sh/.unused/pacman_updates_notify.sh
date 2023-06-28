#!/bin/sh

# Download updates without installing. Nah, I do not need it.
# /usr/bin/pacman --sync --downloadonly --noprogressbar --noconfirm \
#	--quiet --refresh --sysupgrade

available_updates=$(/usr/bin/pacman --query --quiet --upgrades)
[ -z "${available_updates}" ] && exit
set -- ${available_updates}

${0%/*}/notify.sh "'$# updates are available for downloading'" 0 10000
