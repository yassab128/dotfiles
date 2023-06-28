#!/bin/sh

PATH=/usr/bin/
set -efu

clipboardShow() {
	xterm -class "fzfMenu" \
		-e "greenclip print |
	fzf | xsel --input --clipboard"
}

installedPackages() {
	eval "$(
		find \
			/usr/bin \
			-printf '%f\n' |
			"${HOME}/Programme/usw/shell/zfz.sh"
	)"
}

case "$1" in
--clipboard)
	clipboardShow
	;;
--binaries)
	installedPackages
	;;
*)
	printf 'Illegal option!\n'
	;;
esac
