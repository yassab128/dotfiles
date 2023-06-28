#!/bin/sh

set -efu

printf "\e[1;33mInstalled \e[0m" && printf '\n' &&
yay -Ss --color always "$@" | awk '/installed/{print; getline; print }' &&
printf "\e[1;33mNotInstalled \e[0m" && printf '\n' &&
yay -Ss --color always "$@" | awk '/\[installed/ { getline; next } 1'
