#!/bin/sh

set -efu

shellScript="$1"
perlOccurances="$(grep -c "perl -e '$" "$1")"
printf '%s\n' "$perlOccurances"
perlScript=/tmp/perlScript.pl

# backup the shellScript, just in case
cp "$1" /tmp/recycle_bin/

# c-style for loop, because the bash version of for loop
# does not work well when the counter relies on variables

i=0 && while [ $i -le "$perlOccurances" ]; do
	i=$((i + 1))
	# for ((c = 1; c <= "$perlOccurances"; c++)); do
	printf 'running\n'

	# change the first occurance of 'perl -e' to a random string,
	# in this case it is  finibusElementum, but only if the line
	# also contains '
	# perl -i -pe '$a=1 if(!$a && /'"'"'/ && s/perl/finibusElementum/);'\
	# "$shellScript"
	sed -i "0,/perl -e '$/s//finibusElementum '/" "$shellScript"

	# extract and output perlScript to a random temp file
	# in this case, it is /tmp/perlScript.pl
	awk '/finibusElementum/{flag=1; next} /'"'"'/{flag=0} flag' \
		"$shellScript" >"$perlScript"

	# delete perlScript
	sed -i '/finibusElementum/,/'"'"'/{/finibusElementum/!{/'"'"'/!d}}' \
		"$shellScript"

	# check if 'use warnings;' and 'use strict;' are on the top of perlScript
	if ! grep --quiet --fixed-strings "use warnings FATAL" \
		--max-count=1 "$perlScript"; then
		# if not add them to perlScript
		# printf and '>' are useless here since we
		# want to add these two lines to the top of the file, not the bottom
		# printf 'use strict;\nuse warnings FATAL => "all";\n'\
		# >/tmp/perlScript.pl
		# to accomplish that, we will sed to replace ^ with what we want
		sed -i '1s/^/use strict;\nuse warnings FATAL => "all";\n/' \
			"$perlScript"
	fi

	# run perltidy
	# the difference between `perltidy` and `perltidy -b -bext='/'`
	# is that the latter will get rid of the backup file '.ptd' if
	# formatting was done succesful I do not need the backup files since
	# the chances of messing up something is slim
	perltidy -b -bext='/' "$perlScript"

	# -e: redirect the output of the formatted perlScript to shellScript
	# right after finibusElementum keyword. -e: change finibusElementum
	# to a random string, in this case it is perl -e, so that
	# the next iteration goes smoothly
	sed -i -e "/finibusElementum/r $perlScript" "$shellScript" \
		-e '0,/finibusElementum/s//perl -e/' "$shellScript"
done

# change all occurances of 'perl -e' to 'perl'
sed -i 's/perl -e/perl -e/' "$shellScript"
