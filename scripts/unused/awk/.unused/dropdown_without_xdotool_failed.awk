#!/usr/bin/awk -f

function exec_xterm() {
	system("exec xterm -class dropdown -g '1366x25+0+0' 2>&- &")
	exit
}

BEGIN {
	cmd = "exec xwininfo -children -root"
	for (;;) {
		if ((cmd | getline) < 1) {
			exec_xterm()
		}
		if (/ "dropdown"\) /) {
			break
		}
	}
	close(cmd)
	print $1

	# cmd = "xprop -id " $1
	id = $1
	cmd = "exec xwininfo -id " id
	for (;;) {
		if ((cmd | getline) < 1) {
			exit
		}
		if (/^  M/) {
			break
		}
	}
	close(cmd)

		print system("xprop -id " id " -f _NET_WM_STATE 32a \
		      -set _NET_WM_STATE _NET_WM_STATE_FULLSCREEN")
		exit
	if ($3 == "IsUnMapped") {
		print system("exec xprop -id " id " -f _NET_WM_STATE 32a \
			-remove _NET_WM_STATE _NET_WM_STATE_SHOWN") 
		print 0
	} else {
		print system("exec xprop -id " id " -f _NET_WM_STATE 32a \
			-set _NET_WM_STATE _NET_WM_STATE_SHOWN") 
		print 1
	}
}
