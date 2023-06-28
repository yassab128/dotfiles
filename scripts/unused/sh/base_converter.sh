#!/bin/sh -eu

[ $# -ne 1 ] && exit

integer=$1
printf "0%o\n%d\n0x%x\n"\
	${integer} ${integer} ${integer}
