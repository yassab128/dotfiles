#!/bin/sh

set -efu

# pip
#pip3 list --user --outdated --format=freeze |
#grep -v '^\-e' | cut -d = -f 1 | xargs -r -n1 pip3 install --quiet -U

# neovim
#nvim +PlugUpgrade +PlugUpdate +CocUpdate +qa

# yarn
#yarn global upgrade --latest --silent

# termux
#ssh user@"$TERMUX" -p 8022 \
#'apt-get -qq autoremove -y &&
#apt-get -qq update -y && apt-get -qq upgrade -y'
#ssh user@"$TERMUX" -p 8022 \
#"pip3 install --upgrade \
#https://github.com/mps-youtube/pafy/archive/develop.zip \
#https://github.com/mps-youtube/mps-youtube/archive/develop.zip"
#ssh user@"$TERMUX" -p 8022 \
#'nvim +PlugUpgrade +PlugUpdate +CocUpdate +qa'
#ssh user@"$TERMUX" -p 8022 \
#'crontab -l' >/home/pactrag/Programme/usw/fajita/crontab.txt

###Tmux###
#plug() {
#git clone https://github.com/"$1"/"$2".git --depth=1 ||
#(cd "$2" && git pull origin --rebase=false && cd ..)
#}

#mkdir -p ~/Programme/plugins/tmux
#cd ~/Programme/plugins/tmux || return
#plug tmux-plugins tmux-prefix-highlight
#plug tmux-plugins tmux-resurrect
#plug tmux-plugins tmux-yank

###Vim###

#for i in */.git; do (
#printf "%s\n" "$i"
#cd "$i"/.. || return
#git pull --rebase
#); done

#for p in ./*; do
#basename "$p"
#cd "$p" || return
#git pull --rebase
#done

#mkdir -p ~/.local/share/nvim/site/pack/default/start
#cd ~/.local/share/nvim/site/pack/default/start || return
#plug preservim nerdcommenter
#plug dense-analysis ale
#plug lervag vimtex
#plug tpope vim-surround
#plug vim-airline vim-airline
#plug mhinz vim-startify
#plug tpope vim-repeat
#plug svermeulen vim-easyclip
#plug vimwiki vimwiki
#plug tpope vim-unimpaired
#plug inkarkat vim-UnconditionalPaste
#plug tpope vim-eunuch
#plug svermeulen vim-subversive
#plug kassio neoterm
#plug AndrewRadev undoquit.vim
