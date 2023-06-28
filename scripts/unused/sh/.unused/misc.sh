#!/bin/sh

set -efu

/usr/bin/mbsync --quiet -a
checkMail="$(/usr/bin/notmuch --config=/home/pactrag/.config/notmuch/notmuchrc new)"
if [ "$checkMail" != "No new mail." ]; then
    /usr/bin/dunstify -b 'new email'
    /usr/bin/echo '1' >/tmp/newMail.txt
fi

if [ "$(</sys/class/power_supply/AC/online)" == "0" ]; then
    /usr/bin/printf 'DISCHARGING\n'
fi
