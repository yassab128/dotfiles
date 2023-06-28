#!/bin/sh

set -efu

find . -type f -exec sed -i "s/$1/$2/g" {} +
find . -execdir perl-rename "s/$1/$2/" '{}' \+
# find . -iname "*sh" -type f -exec sed -i "1s/usr\/bin\/env bash/bin\/sh/" {} +
# find . -iname "*sh" -type f -exec sed -i '1 a \\nset -efu' '{}' \+
# find . -iname "*sh" -type f -exec sed -i "s/\<awk\>/awk/g" {} \+
#find . -iname "*sh" -type f -exec sed -i 's#/usr/bin#/usr/bin#g' {} \+
find . -iname "*sh" -type f -printf 'mv %f %f' {} \+
