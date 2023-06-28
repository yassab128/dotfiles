#!/bin/sh

set -efu

sed -i "s/$1/$2/" "$1" && mv "$1" "$2"
