#!/bin/sh -u

readonly remote_url='git@github.com:Aptrug/dotfiles.git'
readonly git_dir="${HOME}/.local/share/dotgit"
dotgit() {
	git --git-dir="${git_dir}" --work-tree="${HOME}" $@
}

if ! [ -d "${git_dir}" ]; then
	mkdir -p "${git_dir}"
	git init --bare "${git_dir}"
	dotgit config --local status.showUntrackedFiles no
fi

# dotgit ls-remote --exit-code --heads origin
# dotgit remote add origin "${remote_url}"

dotgit $@
