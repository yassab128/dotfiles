#!/bin/sh

curl https://raw.githubusercontent.com/trapd00r/LS_COLORS/master/LS_COLORS \
	--output - | dircolors --sh - >"${HOME}/.config/LS_COLORS"
