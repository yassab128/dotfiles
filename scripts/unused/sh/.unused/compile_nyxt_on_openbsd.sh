#!/bin/ksh

# Each one is needed for the compilation to work.
# build_deps=(sbcl make pkgconf automake)
# required_deps=(gobject-introspection-runtime webkit2gtk)

# su root -c "pacman -S --needed ${build_deps[*]}"
# su root -c "pacman -S --needed ${required_deps[*]}"
# libtool

pull() {
	git -C "$2" pull origin master ||
		git clone "https://github.com/$1/$2.git" --depth=1 "/tmp/$2"
	cd "/tmp/$2" || exit
}

# pull 'sionescu' 'libfixposix'
# AUTOCONF_VERSION=2.69 AUTOMAKE_VERSION=1.16 autoreconf -ifv
# ./configure --prefix="${HOME}/.local/"
# make
# make install
# exit

pull 'atlas-engineer' 'nyxt'
# export PKG_CONFIG_LIBDIR="${HOME}/.local/lib"
PKG_CONFIG_PATH="${HOME}/.local/lib/pkgconfig" gmake all
exit
make install PREFIX="${HOME}/.local/"

su root -c "pacman -Rcns ${build_deps[*]}"

optional_deps=(gst-plugins-bad)
su root -c "pacman -S --needed ${optional_deps[*]}"
