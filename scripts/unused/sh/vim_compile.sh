#!/bin/sh

cd ${TMPDIR-/tmp}
git clone --depth=1 'https://github.com/vim/vim.git'
cd vim
make uninstall
make distclean
./configure --prefix=${HOME}/.local --with-features=tiny
make VIMTARGET=vi
make install VIMTARGET=vi
