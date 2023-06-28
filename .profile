if test -d '/run'; then
	export OS_TYPE=0
elif test -d '/storage'; then
	export OS_TYPE=1
fi

unlink_some_files()
{
	files=''
	for i in\
			"${HOME}/.bash_history"\
			"${HOME}/.viminfo"\
			"${HOME}/.python_history"; do
		if [ -f "${i}" ]; then
			files="${files} ${i}"
		fi
	done
	[ -n "${files}" ] && rm ${files}
}

unlink_some_files

[ -r ~/.env ] && . ~/.env

if test -z "${TMUX-}"; then
	case ${OS_TYPE} in
	0)
		if test ${XDG_VTNR} -eq 1; then
			export DISPLAY=':0'
			{
				Xorg -keeptty ${DISPLAY} vt${XDG_VTNR} &
			} 3>&1 1>&2 2>&3 3>&- | read
			exec bspwm
		fi
		;;
	*)
		exec tmux
		;;
	esac
fi

test -r ~/.bashrc && . ~/.bashrc

export PROMPT_COMMAND='history -a'

# NEXINIT='set ruler '\
# 'number '\
# 'autoindent '\
# 'extended '\
# 'ignorecase '\
# 'searchincr '\
# 'showmatch '\
# 'showmode '\
# 'windowname '\
# 'wrapmargin=80 '\
# 'report=1 '\
# 'verbose
#
# map gg 1G'

# set -o emacs

sshmux() {
	ssh "$1" -t 'tmux -As ssh_session'
}

# {
# 	${HOME}/sh/git_pusher.sh
# } & disown

# Ignore duplicates and don't save
# commands starting with a space
export HISTCONTROL='ignoredups:ignorespace'

ulimit -c 0

# '[`[ \j == 0 ]&&echo -n 9||echo -n "\[\e[1;32m\]\j\[\033[0m\]"`]\$ '
# '[`x=\j;[ $x -eq 0 ]&&echo -n $x||echo -n "\[\e[1;32m\]$x\[\033[0m\]"`][\j] '
# PS1 will show:
# 1. The exit code of each command.
# 2. The number of files and directories in the current directory.
# 3. The number of jobs currently managed by the shell.

# Works only on oksh
# bind -m '^J=^Atime ^M'
# bind '^[[A='
# bind '^[[B='
# bind '^[[C='
# bind '^[[D='
# [ -f ~/.cargo/env ] && . "$HOME/.cargo/env"
