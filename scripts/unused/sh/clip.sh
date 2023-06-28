#!/bin/sh

# vim -es '+redir>>/dev/stdout | echo "foo\n" | redir END' '+q!'
vim --clean -nf -es\
	'+set clipboard=unnamedplus | %y | put | %p | q!'\
	/dev/stdin <<EOF
Guten Tag!
EOF

exit
clipboard_content="$(
vim --clean -nf -es\
	'+set clipboard=unnamedplus | 0put | 1,$-p | q!'\
	/dev/null
)"
printf "${clipboard_content}"

# vim --clean -f '+set clipboard=unnamedplus' '+put' '+q!'
# vim --clean -f '+set clipboard=unnamedplus' '+put' '+q!'
exit

# vim --clean -M -n -f -i NONE '+set clipboard=unnamedplus' '+1,$y' '+q!' - <<EOF
vim --clean -f '+set clipboard=unnamedplus' '+1,$y' '+q!' - <<EOF
echo "fjkdslklda"
echo "blyat"
Who are you
What are you talking about
XAXAXA
HOLY
EOF

exit
vim --clean -e '+:echo blyat+:q!'
vim --clean -ec '
echo "blyat"
q!
'
