#!/bin/sh -u

readonly git_dir="${HOME}/.local/share/dotgit"
dotgit() {
	git --git-dir="${git_dir}" --work-tree="${HOME}" $@
}

# dotgit diff --exit-code && exit

trap '
	exit_code=$?
	case ${exit_code} in
		# 128: Network problem.
		0|128)
			exit
			;;
		*)
			;;
	esac
	# tmux display-popup "echo file: $0\nexit_code: ${exit_code}"
	exit
' EXIT

dotgit add\
	~/.config/git/config\
	~/.config/alsa/\
	~/.config/bspwm/\
	~/.config/sxhkd/\
	~/.config/gtk-3.0/settings.ini\
	~/.config/nvim/init.vim\
	~/.vim/vimrc\
	~/.env\
	~/.bashrc\
	~/.tmux.conf\
	~/.config/mimeapps.list\
	~/src/\
	~/.local/bin/sh/\
	~/.local/bin/awk/\
	~/.local/bin/pl/\
	~/.local/bin/py/\
	~/.profile\

# find ${HOME} -maxdepth 1 -type f -size -10M -exec dotgit add '{}' +

# dotgit rm --ignore-unmatch --quiet "${path}"
# remove non-existing files from the git repo
while read -r path; do
	path="${HOME}/${path}"
	if ! [ -e "${path}" ]; then
		dotgit rm --quiet --ignore-unmatch "${path}"
	fi
done <<-EOF
	$(dotgit ls-tree --full-tree -r --name-only HEAD)
EOF

# dotgit commit --quiet --allow-empty-message --message=''
# dotgit push --quiet --force --set-upstream origin master
dotgit commit --verbose --allow-empty-message --message=''
dotgit push --verbose --force --set-upstream origin master
