#!/bin/sh -eu

if pgrep -x Xorg; then
	if [ -z "${TMUX-}" ]; then
		exec tmux new -As 'tty'
	else
		exit 1
	fi
fi

export DISPLAY=":0"

if [ -n "${XDG_VTNR-}" ]; then
	xorg_args="-keeptty ${DISPLAY} vt${XDG_VTNR}"
	window_manager='dwm'
else
	# If it does not work, run `chmod +s` on the Xorg binary
	xorg_args="-br ${DISPLAY}"
	# xclock_bar.sh&
	window_manager='cwm'
fi

{
	Xorg ${xorg_args} &
} 3>&1 1>&2 2>&3 3>&- | read

exec ${window_manager}
