#!/bin/sh

set -efu

case "$1" in
clear)
	clear && tmux clear-history
	;;
history)
	fzf --no-sort --reverse <"${HOME}/.cache/bash_history" |
		perl -e '
ioctl STDOUT, 0x5412, $_ for split //, do { chomp($_ = <>); $_ }; print "\n"
'
	;;
clipboardHistory)
	tac "${HOME}/.cache/clipboard_history.txt" | fzf --no-sort --reverse |
		xsel -bi
	;;
*)
	printf 'Illegal option!\n'
	;;
esac
