#!/bin/sh

set -efu

printf "\e[1;33mVerzeichnisse \e[0m"
printf '\n'
find / -type d -iname "*$1*" 2>&-
printf "\e[1;33mDateien \e[0m"
printf '\n'
find / -type f -iname "*$1*" 2>&-
