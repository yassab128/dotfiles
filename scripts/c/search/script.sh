#!/bin/sh

clear
make clean
make

valgrind /home/aptrug/src/c/search/last_try "$@"
