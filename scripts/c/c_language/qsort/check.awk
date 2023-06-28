#!/usr/bin/gawk -f

BEGIN {
	LINT = "fatal"
	k = 0
	output = ""

	while ((getline < "/dev/stdin") > 0) {
		if (/^!$/) {
			++k
		} else {
			output = output $0 "\n"
		}
	}
	close("/dev/stdin")

	command = "sort -cn <<EOF||\n" output "EOF\ntrue"
	while((command | getline) > 0) {
	      print
	}
	close(command)
	print k
}
