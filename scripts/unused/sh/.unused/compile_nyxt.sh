#!/usr/bin/bash

# Each one is needed for the compilation to work.
build_deps=(sbcl make autoconf pkgconf automake)
required_deps=(gobject-introspection-runtime webkit2gtk)

su root -c "pacman -S --needed ${build_deps[*]}"
su root -c "pacman -S --needed ${required_deps[*]}"

pull() {
	git -C "$2" pull origin master ||
		git clone "https://github.com/$1/$2.git" --depth=1 "/tmp/$2"
	cd "/tmp/$2" || exit
}

pull 'sionescu' 'libfixposix'
autoreconf -ifv
./configure --prefix="${HOME}/.local/"
make
make install

pull 'atlas-engineer' 'nyxt'
make all
make install PREFIX="${HOME}/.local/"

su root -c "pacman -Rcns ${build_deps[*]}"

optional_deps=(gst-plugins-bad)
su root -c "pacman -S --needed ${optional_deps[*]}"
