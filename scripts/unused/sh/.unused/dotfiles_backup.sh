#!/bin/sh

set -fu

PATH=/usr/bin/
# do not set `set -e`, if git did not found one file list down, it will abort
# the whole script and no backup will be done until I pay attention to it

# It has to be defined in a function otherwise it will not work on startup

yadm() {
	"${HOME}/Programme/usw/shell/inpath/yadm" "$@"
}

"${HOME}/Programme/usw/shell/backup_root_files.sh"

eval "$(
	du \
		"${HOME}/.config" \
		"${HOME}/Programme" \
		"${HOME}" \
		--apparent-size --all --max-depth=1 |
		awk '
BEGIN {
	ORS = " "
	print "yadm add"
}

# 10240 is 10M
($1 < 10240) {
	print $2
}

END {
	print "${HOME}/Programme/usw/\n"
}
'
)"

# remove non-existing files from the git repo
while read -r path; do
	absolutePath="${HOME}/${path}"
	if ! [ -e "${absolutePath}" ]; then
		yadm rm -rf --quiet "${absolutePath}"
	fi
done <<-EOF
	$(yadm ls-tree --full-tree -r --name-only HEAD)
EOF

# push the changes with an empty commit message
yadm commit --all --message='‏‏‎'
yadm push --set-upstream origin master
