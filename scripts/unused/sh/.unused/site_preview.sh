#!/bin/sh

# Apparently, something like `chromium ./index.html` is enough.
exit

set -m

python -m http.server &
chromium --auto-open-devtools-for-tabs --app='http://0.0.0.0:8000/' &
chromium_pid=$!
fg 1
kill "${python_pid}"

# find . -maxdepth 1 -type f \
# 	-exec tail --quiet --lines=0 --follow=name '{}' \+ 2>/dev/null |
# 	# --bytes=0 is not possible
# 	head --bytes=1 >/dev/null

# fg 1
# kill "${chromium_pid}"
