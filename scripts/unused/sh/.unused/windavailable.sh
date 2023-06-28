#!/bin/sh

set -efu

windscribe locations | awk '!/*/'
