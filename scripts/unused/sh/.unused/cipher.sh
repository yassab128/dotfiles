#!/bin/sh

set -efu

for cipher in 3des-cbc aes128-cbc aes128-ctr aes128-gcm@openssh.com chacha20-poly1305@openssh.com rijndael-cbc@lysator.liu.se; do
    echo "$cipher"
    for _ in 1 2; do
        scp -P 8022 -c "$cipher" ./Screenshot_20200912_231649.png user@"$TERMUX":/storage/emulated/0/Download/over_ssh
    done
done
