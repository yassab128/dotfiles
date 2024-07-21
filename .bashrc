# if [ -z "${TMUX-}" ]; then
# 	if [ -n "${DISPLAY-}" ]; then
# 		exec tmux new -As 'display'
# 	# else
# 	# 	exec tmux new -As 'tty'
# 	fi
# fi

alias\
	ls='ls -F'\
	au='ffplay -autoexit -nodisp -loglevel quiet'\
	jine='LC_ALL=ja_JP.UTF-8 TZ="Asia/Tokyo" wine'\
	aria='aria2c --enable-rpc --rpc-listen-all'\
	cpu='ps -eo comm,rss,pcpu --sort=pcpu'\
	mem='ps -eo comm,rss,pcpu --sort=rss'\
	l='ls -Ahlt'\
	speed_test='curl -o/dev/null '\
	clear='printf "\033[H\033[J"; [ -n "${TMUX-}" ] && tmux clear-history'\
	download='curl -LOZC-'\
	Firefox='firefox >&- 2>&- &'\
	rmempty='find . -maxdepth 1 -empty -delete'\
	ascii='awk '\''BEGIN{for(i=0;i<128;++i)printf "%d|%c\n",i,i}'\''|less'\
	yt-dlp='yt-dlp --ignore-errors --retries infinite'\
	au-dlp='yt-dlp --no-mtime --restrict-filenames\
		--embed-metadata --embed-thumbnail\
		--format 140 --output "%(title)s.%(ext)s"'
case ${OS_TYPE} in
1)
	alias su='/system/bin/su -s /system/xbin/bash'\
		Download='cd /storage/emulated/0/Download'\
		Music='cd /storage/emulated/0/Music'\
		Movies='cd /storage/emulated/0/Movies'\
		0='cd /storage/emulated/0'\
		Screenshots='cd /storage/emulated/0/Pictures/Screenshots'\
		Pictures='cd /storage/emulated/0/Pictures'\
		update='apt-get update &&
			apt-get autoremove -y &&
			apt-get dist-upgrade -y'
	;;
*)
	;;
esac

nukepkg() {
	su -c "apt-get autoremove --purge $*"
}

m() {
	typeset -r option=$1
	[ ${#option} -ne 1 ] && return 1;
	typeset -r depository="${TMPDIR:-/tmp}/depository"
	typeset i
	shift
	case "${option}" in
	# truncate
	('t')
		true > "${depository}"
		return 0;
		;;
	# print
	('p')
		while read -r i; do
			echo "${i}"
		done <"${depository}"
		return 0;
		;;
	# add
	('a')
		for i in $@; do
			[ -e "${i}" ] &&
				readlink -f -- "${i}" >>"${depository}"
		done
		return 0
		;;
	# copy
	('c')
		[ $# -ne 0 ] && return 1
		typeset -r command='cp -av'
		;;
	# move
	('m' | 'move')
		[ $# -ne 0 ] && return 1
		typeset -r command='mv -v'
		;;
	(?)
		return 1
		;;
	esac

	# Store the file names in a one string.
	typeset files=''
	while read -r i; do
		files="${i} ${files}"
	done <"${depository}"

	${command} -- ${files} .
	return 0
}

c() {
	if [ "$1" = '*' ]; then
		x=0
	else
		x=$(($#-2))
	fi
}

j() {
	x=$#
}

# PS1='[`x=$?;[ $x -eq 0 ]&&echo -n 0||echo -n "\[\e[1;31m\]$x\[\e[0m\]"`]'\
# '[`c * .*;echo -n $x`][`j $(jobs -p); [ $x == 0 ]&&echo -n 0||'\
# 'echo -n "\[\e[1;32m\]$x\[\e[0m\]"`]\[\e[1;33m\]\$\[\e[0m\] '

PS1='[`x=$?;test $x -eq 0&&echo -n 0||echo -n "\[\e[1;31m\]$x\[\e[0m\]"`]'\
'[`c * .*;echo -n $x`][`j $(jobs -p);test $x == 0&&echo -n 0||'\
'echo -n "\[\e[1;32m\]$x\[\e[0m\]"`]\[\e[1;33m\]\$\[\e[0m\] '
