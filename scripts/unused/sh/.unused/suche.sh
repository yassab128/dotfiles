#!/bin/sh

set -efu

printf "\e[1;33mVerzeichnisse \e[0m"
printf '\n'
find / -name home -prune -o -type d -iname "*$1*" -print 2>&-
printf "\e[1;33mDateien \e[0m"
printf '\n'
find / -name home -prune -o -type f -iname "*$1*" -print 2>&-
