#!/bin/sh

set -efu

# list all running processes

ps -eo command --sort=command |
	sed '1d #remove the first line (it is "Command")
    /\[\|\]/d #remove everything before the last slash
    s/^.*[$/]// #remove all lines containing either [ or ]
    s/--.*$// #remove everything after -- (it is for command flags)
    /^(sd-pam)$\|ps\s-eo\scommand\|proc\.sh\|^sort$/d
    s/\s\+$// # remove trailing whitespace (otherwise sort -u' |
	# wont work very well)
	#sort | uniq -c
	awk '
	{
		# l[lines++] = $0
		# split("", tot)
		tot[$0]++
	}

	END {
		i = 0
		for (i in tot) {
			print i, "(" tot[i] ")"
		}
	}
	' |
	sed -e '1d' -e 's/\s(1)//' |
	sort
