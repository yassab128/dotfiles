#!/bin/sh

cd my_dwm/
git pull upstream master --allow-unrelated-histories -s ours
git push origin master
