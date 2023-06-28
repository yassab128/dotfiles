#!/bin/sh -u

# dotgit.sh reset --hard HEAD
# dotgit.sh pull origin master
# dotgit.sh fetch
# dotgit.sh merge origin/master

printf "Enter 'y' to proceed: "
read reply
[ "${reply}" != 'y' ] && exit 1

dotgit.sh fetch origin master
dotgit.sh reset --hard origin/master
