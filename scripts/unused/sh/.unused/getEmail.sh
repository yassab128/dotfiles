#!/usr/bin/bash

set -efu

/usr/bin/mbsync --quiet -a
checkMail="$(/usr/bin/notmuch \
    --config=/home/pactrag/.config/notmuch/notmuchrc new)"
if [ "$checkMail" != "No new mail." ]; then
    /usr/bin/dunstify -b 'new email'
    /usr/bin/echo '1' >/tmp/newMail.txt
    #else
    #/usr/bin/dunstify 'xaxaxa' -t 10000
fi
# not needed, since `notmuch new' is used now instead of `mbsync -all` to check
#checkMail="$(mbsync --all)"
#newMail="$(awk '{print $10}' <<<"$checkMail")"
