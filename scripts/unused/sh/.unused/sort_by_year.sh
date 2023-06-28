#!/bin/sh

set -efu

source /home/pactrag/.bashsrc/function.bash

for folder in {1970..2020}; do
    mvname "$folder"
done;

sleep 1	 && find . -maxdepth 1 -type d -empty -delete
