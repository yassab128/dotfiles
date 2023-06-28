#!/bin/sh

set -efu

nix-store --query --referrers "$(which "$1")"
