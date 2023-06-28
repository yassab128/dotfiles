#!/bin/sh

set -efu
# XXX works perfectally, but it uses mark feature

# returns 1 if a marked pane exists in the current session
pane_marked_set="$(tmux display-message -p '#{pane_marked_set}')"
# returns 1 if the marked pane exists in the current window
window_marked_flag="$(tmux display-message -p '#{window_marked_flag}')"
# returns 1 if the current pane is a marked pane
pane_marked="$(tmux display-message -p '#{pane_marked}')"
# the base index of the current window
I="$(tmux display-message -p '#I')"

if [ "${pane_marked_set}" = 0 ]; then
	# create the pane
	tmux split-window -h -c "#{pane_current_path}"
	# mark it
	tmux select-pane -m
	# same pwd
elif [ "${window_marked_flag}" = 0 ]; then
	# move the marked pane to the current window
	tmux join-pane -h -t :"${I}"
	# mark it again, since it becomes unmarked
	tmux select-pane -m
	# same pwd
elif [ "${pane_marked}" = 0 ]; then
	# move focus to the marked pane
	# tmux select-pane -t '{marked}'
	# tmux resize-pane -Z -t 0
	tmux last-pane
else
	# hide the marked pane
	tmux resize-pane -Z -t 0
fi
