#!/bin/sh -eu

readonly term_name='dropdown'
readonly tmux_session_name='dropdown'

xwininfo_output=$(xwininfo -int -name dropdown) ||
	exec xterm\
		-T ${term_name}\
		-g '194x26+0+0'\
		-e "exec tmux new -As ${tmux_session_name}"

term_id=$(grep -Eom1 '[0-9]+' <<EOF
${xwininfo_output}
EOF)

if grep -qF '  Map State: IsViewable' <<EOF; then
${xwininfo_output}
EOF
	readonly active_window_info=$(xprop -root _NET_ACTIVE_WINDOW)
	readonly active_window_id=$(printf '%d' ${active_window_info##* })
	test ${term_id} -eq ${active_window_id} &&
		xdotool windowunmap ${term_id}
	exec xdotool windowactivate ${term_id}
fi
exec xdotool windowmap ${term_id}
