#!/bin/sh

set -efu

su -c '
# stop the display-manager otherwise lightdm will respawn again
systemctl stop display-manager.service
# stop pulseaudio (I do not need any music)
pkill -x pulseaudio
# kill X and all the processes that depend on it
pkill -x X
'
