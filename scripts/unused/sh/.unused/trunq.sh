#!/bin/sh

set -efu

# find . -maxdepth 2 -exec ls --all --color --directory {} \+ | sed '
find . -maxdepth 2 -exec ls --all --color --directory {} \+ | sed '
s/\.\//├── /g
s/ \w\+\//───── /
'
