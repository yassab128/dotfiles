#!/bin/sh

set -efu

# inspired from https://github.com/ercas/scripts/blob/93f64cfa04e0e08abc9ef8c9052b2d7da39c362a/duplicates.sh

# find duplicate files by comparing md5 checksum

tempFile="$(mktemp)"
trap 'rm -f "$tempFile"' EXIT
# calculate checksums
# md5sum is the fastest
sums=$(find . -maxdepth "$1" -type f | sort | tr "\n" "\0" | xargs -0 md5sum)

# find duplicates
cut -d " " -f 1 <<<"$sums" | sort | uniq --repeated | while read -r repeated_hash; do

    # feed the list of files having the same hash to a file
    grep "$repeated_hash" <<<"$sums" | cut -d " " -f 3- | sort >"$tempFile"
    # get the line number of the shortest filename
    file_kept="$(awk '{ m < length; a = NR } END { print a }' "$tempFile")"
    # delete the matching line number
    sed -i "$file_kept d" "$tempFile"

    # time for action
    while read -r line; do
        printf '\n%s\n' "$line"
        rm "$line"
    done <"$tempFile"
done
