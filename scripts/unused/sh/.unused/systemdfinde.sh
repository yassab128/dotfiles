#!/bin/sh

set -efu

# systemdse.sh is better
printf "\e[1;33mVerzeichnisse \e[0m"
printf '\n'
find /usr/lib/systemd/system/ -type d -iname "*$1*" 2>&-
printf "\e[1;33mDateien \e[0m"
printf '\n'
find /usr/lib/systemd/system/ -type f -iname "*$1*" 2>&-
