#!/bin/sh

set -efu

# the site is no longer reachable
curl metaphorpsum.com/paragraphs/"$1"/10
