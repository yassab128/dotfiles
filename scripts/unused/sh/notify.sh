#!/bin/sh -eu

printf '\a'
send_notifcation() {
	xmessage\
		-buttons 'OK:0'\
		-bg black\
		-fg yellow\
		-nearmouse\
		-timeout 5\
		"$*"
}

# export XAUTHORITY='/home/pactrag/.Xauthority'
# export DISPLAY=':1'
send_notifcation Guten Tag!
