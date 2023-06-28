#!/bin/sh

set -efu

# check if /tmp/recycle_bin exists and create it if it does not
# [ -d /tmp/recycle_bin ] || mkdir /tmp/recycle_bin

## backup /etc/nixos/configuration.nix
# cp /etc/nixos/configuration.nix /home/pactrag/Programme/usw/bakup/root

# start redshift, no need for & in the end
# redshift -O 3500 -m randr

# bspwm integration with lemonbar
# /home/pactrag/Programme/usw/shell/bspwmStatus.sh &

# /home/pactrag/Programme/usw/c_language/doesItAll.eil &

# start dunst notification server
# dunst 2>/dev/null &

# /home/pactrag/Programme/usw/shell/inpath/ariaProgress.sh &
#/usr/bin/aria2c >/tmp/ariaParser.txt 2>&1 &
# /usr/bin/aria2c
# /home/pactrag/Programme/usw/perl/ariaProgress.pl &

# for anything that requires network
i=1
while [ "${i}" -lt 20 ]; do
	sleep 0.5
	if [ -n "$(iwgetid --raw)" ]; then

		## start lemonbar
		# 	eval "$(/home/pactrag/Programme/usw/shell/lemonbarConfig.sh |
		# 		lemonbar -p &)" &

		# backup dotfiles
		# /home/pactrag/Programme/usw/shell/gitPusher.sh &

		break
	fi
	i=$((i + 1))
done

# LS_COLORS
# run it when you know LS_COLORS has been updated
# curl https://raw.githubusercontent.com/trapd00r/LS_COLORS/master/LS_COLORS \
# 	--output - | dircolors --sh - >/home/pactrag/.config/LS_COLORS
# obsolete
# cat "$(curl \
# 	--remote-name \
# 	https://raw.githubusercontent.com/trapd00r/LS_COLORS/master/LS_COLORS \
# 	--output-dir /home/pactrag/.config)"
