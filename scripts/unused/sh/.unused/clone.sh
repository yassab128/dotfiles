#!/bin/sh

set -efu

#!/bin/bash
find . -iname "*sh" -type f -exec sed -i "1s/usr\/bin\/env bash/bin\/sh/" {} +


URL="$1"
DIR_OR_FILE="$(awk -F / '{ print $(NF-6) }' <<<"$URL")"
repoName="$(awk 'BEGIN { FS = "/" } { print $(NF-6) }' <<<"$URL")"

if [ "$DIR_OR_FILE" = "blob" ]; then
	FILE_URL="$(sed -e 's/github/raw\.githubusercontent/; s/blob\///' <"$URL")"
	curl -O "$FILE_URL"
else
	MASTER_URL="$(sed 's/tree.*/archive\/master\.tar\.gz/' <<<"$URL")"
	repoName="$(awk -F / '{ print $(NF-5) }' <<<"$URL")"
	DIR_PATH="$(sed -n -e 's/^.*master\///p' <<<"$URL")"
	curl -sL "$MASTER_URL" | tar xvz "$repoName"-master/"$DIR_PATH"
fi
