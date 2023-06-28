#!/bin/sh

set -efu

realpath "$1" | sed "s|^$HOME|~|"
