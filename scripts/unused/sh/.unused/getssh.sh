#!/bin/sh

set -efu

# redundant after connecting from Termux to NixOs
ssh -c aes128-ctr -p 8022 user@192.168.1.147 'tar cJf - /storage/emulated/0/Download/over_ssh/Teleporter/*' | tar xvJf -
