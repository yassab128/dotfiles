#!/bin/sh -eu

readonly class='bspdt'
readonly rule='sticky=on state=floating hidden=on rectangle=1360x400+0+0'

bspc rule --list | grep "${rule}" >/dev/null || {
	bspc rule --remove "${class}"
	bspc rule --add "${class}" "${rule}"
}

if window_id=$(bspc query --nodes --node .hidden.sticky.floating); then
	bspc node "${window_id}" --flag hidden=off --focus
elif window_id=$(bspc query --nodes --node .sticky.floating); then
	bspc node "${window_id}" --flag hidden=on --focus
else
	xterm -class "${class}" -e 'exec tmux new -As secondary' &
	while ! window_id=$(bspc query --nodes --node .sticky.floating); do
		true
	done
	bspc node "${window_id}" --flag hidden=off --focus
fi
