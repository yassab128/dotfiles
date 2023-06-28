#!/bin/sh

set -efu

#nvr -p %F ""it works""
#NVIM_LISTEN_ADDRESS=/tmp/nvimsocket nvim -p %F
#source /home/pactrag/Programme/usw/neovim-remote.sh; check %F

function check() {
if pgrep -x "nvim-qt" > /dev/null
then
    nvr --remote -p "$1" ; wmctrl -xa nvim-qt
else
    env NVIM_LISTEN_ADDRESS=/tmp/nvimsocket nvim-qt -- "$1"
fi
}
