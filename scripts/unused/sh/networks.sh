#!/bin/sh

ifconfig | sed -En '
s/.*nwid (.*) chan .*([0-9]+%).*/\1 \2/
t a
q
:a
p
'
