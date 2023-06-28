#!/bin/sh

PATH=/usr/bin/

# The terminal emulator I primarly use is XTerm. I did not want to 
# install another terminal emulator just for the drop-down functionality
# so I came up with this shell script. XTerm is used here, but you can
# use other terminal emulators as long as they support setting class names.

# Running this shell script will toggle the drop-down terminal's window.

# The terminal class to use.
class='dropdown'

# My screen has a 1366x768 resolution and 1366x360+0+0 roughly takes the top
# half of the screen, you can change '1366x360+0+0' to whatever you like.
# rule='sticky=on state=floating hidden=on rectangle=1366x360+0+0'

# Add the rule if it wasn't found. You can also add it to
# ~/.bspwm/bspwmrc, remove this part and be done with it.
# bspc rule --list | grep "${rule}" || bspc rule --add "${class}" "${rule}"

# Get the ID of the current workspace in order to move xterm to it.
currentDesktop="$(bspc query --desktops --desktop)"

# Get the window id of xterm it is running.
window_id=$(xdotool search --class "${class}")

if [ -n "${window_id}" ]; then

	# If xterm is already running, toggle it.
	bspc node "${window_id}" --flag hidden --focus \
		--to-desktop "${currentDesktop}" --follow
else
	# If xterm is not already running, run it asynchronously.
	XTERM_DROPDOWN=1 xterm -class "${class}" &

	# Wait for xterm to initialize (you might need to increase this duration
	# if it does not work for you).
	sleep 0.1

	# Get the window_id of xterm.
	window_id=$(xdotool search --class "${class}")

	# Show xterm.
	bspc node "${window_id}" --flag hidden --focus
fi
