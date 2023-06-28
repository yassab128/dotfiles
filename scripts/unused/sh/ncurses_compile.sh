#!/bin/sh -eu

cd /tmp

rm -rf ncurses-6.3/
# curl -O 'https://invisible-island.net/datafiles/release/ncurses.tar.gz'
tar -xf /tmp/ncurses.tar.gz
cd ncurses-6.3/

./configure --prefix=/usr\
	--with-install-prefix=${HOME}/.local\
	--without-manpages\
	--without-cxx\
	--without-cxx-binding\
	--without-cxx-shared\
	--without-tests\
	--disable-leaks\
	--disable-db-install\
	--without-debug\
	--with-pkg-config-libdir=/usr/lib/pkgconfig\
	--disable-rpath-hack\
	--without-ada\

make
