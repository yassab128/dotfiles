#!/bin/sh

set -efu

# I don't even know why I wrote this script

mkdir "$1"
for file in *; do
    if [[ "$file" == *"$1"* ]] && [ ! -d "$file" ]; then
        mv "$file" "$1"
    fi
done
