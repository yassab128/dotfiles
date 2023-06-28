#!/bin/sh

xrdb ~/.Xdefaults

# pkill -qx xclock

# xclock -geometry 1392x24+0+0 \
#          -update 1 \
#          -chime \
#          -d \
# 	-face 'DejaVuSans' \
#         -strftime "%a %dT%T" \
#          -padding 0 \
#          -fg green \
#          -bg black

xclock -strftime '%a                                                     '\
'                                                                     '\
'%T                                                                      '\
'                                                 %d'
