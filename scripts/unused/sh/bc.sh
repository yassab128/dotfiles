#!/bin/sh -eu

trap 'exec echo' EXIT
echo -n '>> '
while true; do
	read -r mathexp
	awk 'BEGIN{printf '"${mathexp}"' "\n\n>> "}'
done
