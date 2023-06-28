#!/bin/sh

set -efu

extension="$(printf "%s" "$@" | sed 's/^.*\.//')"
without_extension="$(printf "%s" "$@" | sed 's/\.[^.]*$//')"

if [ "$extension" == "c" ]; then
    gcc -o "$without_extension".eil "$@"

elif [ "$extension" = "cpp" ]; then
    g++ -o "$without_extension".eil "$@"

elif [ "$extension" = "rs" ]; then
    rustc -o "$without_extension".rut "$@"

elif [ "$extension" = "go" ]; then
    go run "$@"

elif [ "$extension" = "tex" ]; then
    latexmk "$@"
else
    printf 'not supported'
fi
