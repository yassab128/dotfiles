#!/bin/sh

# exec rustc --print target-list
rustc --target=mips-unknown-linux-musl ./hello_world.rs
