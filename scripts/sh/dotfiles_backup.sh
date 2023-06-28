#!/bin/sh

set -fu

PATH=/usr/bin/
# do not set `set -e`, if git did not found one file list down, it will abort
# the whole script and no backup will be done until I pay attention to it

# It has to be defined in a function otherwise it will not work on startup

dotgit() {
	# git --git-dir=$HOME/.local/share/dotgit/ --work-tree=$HOME "$@"
	./dotgit.sh $@
}

# "$HOME/Programme/usw/shell/backup_root_files.sh"

eval "$(
	du \
		"$HOME/.config" \
		"$HOME/scripts" \
		--apparent-size --all --max-depth=1 |
		awk '
BEGIN {
	ORS = " "
	print "dotgit add"
}

# 10240 is 10M
($1 < 10240 && NF < 3) {
	print $2
}
'
)"
dotgit add "$HOME/.bashrc"\
	"$HOME/.profile"\
	"$HOME/.env"
# remove non-existing files from the git repo
while read -r path; do
	absolutePath="$HOME/${path}"
	if ! [ -e "${absolutePath}" ]; then
		dotgit rm -rf --quiet "${absolutePath}"
	fi
done <<-EOF
	$(dotgit ls-tree --full-tree -r --name-only HEAD)
EOF

# push the changes with an empty commit message
dotgit commit --all --allow-empty-message --no-edit
dotgit push --set-upstream origin master
