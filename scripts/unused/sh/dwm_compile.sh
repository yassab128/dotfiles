#!/bin/sh -eu

clear
make clean
make
make install PREFIX=~/.local
pkill -x dwm
