#!/bin/sh

set -efu

# root stuff
su -c 'tlmgr update --self --all ;
zypper -q ref ;
zypper -q dup --no-recommends'

# pip
#pip3 list --user --outdated --format=freeze |
#grep -v '^\-e' | cut -d = -f 1 | xargs -r -n1 pip3 install --quiet -U

# neovim
#nvim +PlugUpgrade +PlugUpdate +CocUpdate +qa

# flatpak
#flatpak update -y && flatpak upgrade -y

# yarn
#yarn global upgrade --latest --silent

# termux
#ssh user@"$TERMUX" -p \
#8022 'apt-get -qq autoremove -y &&
#apt-get -qq update -y && apt-get -qq upgrade -y'
#ssh user@"$TERMUX" -p \
#    8022 "python3 -m pip install --upgrade pip &&
#    pip3 list --user --outdated --format=freeze | grep -v '^\-e' |
#    cut -d = -f 1 | xargs -r -n1 pip3 install --quiet -U"
