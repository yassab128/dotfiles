#!/bin/sh

time /bin/sh -c '
i=0
while [ "${i}" -lt '"$1"' ]; do
	'"$2"'
	i=$((i + 1))
done
'
