#!/bin/sh

set -efu

grep -riL "$1" .
