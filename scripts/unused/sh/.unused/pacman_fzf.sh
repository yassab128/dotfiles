#!/bin/sh

set -efu
export LC_ALL=C
PATH=/usr/bin/

sort_by='size'
use_pager=0
explicit_only=0
order_by='desc'
while getopts '1EPbcdioprs' i; do
	case ${i} in
	1)
		order_by='asc'
		;;
	E)
		explicit_only=1
		;;
	P)
		use_pager=1
		;;
	b)
		sort_by='build_date'
		;;
	c)
		sort_by='binaries_number'
		;;
	d)
		sort_by='dependencies_number'
		;;
	i)
		sort_by='installation_date'
		;;
	o)
		sort_by='optional_dependencies_number'
		;;
	p)
		sort_by='package'
		;;
	r)
		sort_by='reverse_dependencies_number'
		;;
	s)
		sort_by='size'
		;;
	*)
		printf 'Illegal option!\n'
		exit 1
		;;
	esac
done

output="$(
	gawk '
function size_in_MiB(Size, Unit)
{
	if (Unit == "KiB") {
		return Size / 1024
	} else if (Unit == "MiB") {
		return Size
	} else {
		# The only unit left to check is "B", since there is no "GiB".
		return Size / 1048576
	}
}

function convert2epoch(date_string) {
	split(date_string, date)
	date[2] = (((index("JanFebMarAprMayJunJulAugSepOctNovDec",
		date[2]) - 1) / 3) + 1)
	gsub(/:/, " ", date[4])
	datespec = date[5] " " date[2] " " date[3] " " date[4]
	return mktime(datespec)
}


{
	if ($0 ~ /^Name/) {
		name = $3
		binaries_number[name] = 0
	} else if ($0 ~ /^Depends/) {
		if ($0 ~ /None$/) {
			dependencies_number[name] = 0
		} else {
			dependencies_number[name] = NF - 3
		}
	} else if ($0 ~ /^Optional Deps/) {
		optional_dependencies_number[name] = 0
		installed_optional_dependencies_number = 0
		if ($0 ~ /None$/) {
			getline
		} else {
			while ($0 !~ /^Required/) {
				++optional_dependencies_number[name]
				if ($0 ~ /\[installed\]$/) {
					++installed_optional_dependencies_number
				}
				getline
			}
		}
		optional_dependencies[name] = sprintf("(%s/%s)",
			installed_optional_dependencies_number,
			optional_dependencies_number[name])
		# /^Required/
		if ($0 ~ /None$/) {
			reverse_dependencies_number[name] = 0
		} else {
			reverse_dependencies_number[name] = NF - 3
		}
	} else if ($0 ~ /^Installed Size/) {
		size[name] = sprintf("%.2f", size_in_MiB($4, $5))
	} else if ($0 ~ /^Build Date/) {
		sub(/^Build Date      : /, "", $0)
		build_date[name] = convert2epoch($0)
	} else if ($0 ~ /^Install Date/) {
		sub(/^Install Date    : /, "", $0)
		installation_date[name] = convert2epoch($0)
	} else if ($0 ~ /^Install Reason/) {
		if ($0 ~ /installed$/) {
			package[name] = "[Explicit]"
		} else {
			package[name] = ""
		}
	} else if ($0 ~ /^\/usr\/bin\/.*[^/]$/) {
		# The path of the binary must not end with a trailing slash (hence the
		# the weird regex) since that means it is actually a directory
		# (e.g. /usr/bin/core_perl, /usr/bin/site_perl, /usr/bin/vendor_perl).
		++binaries_number[name]
	}
}

END {
	PROCINFO["sorted_in"] = "@val_num_'"${order_by}"'"
	for (i in '"${sort_by}"') {

		if ('"${explicit_only}"' && reverse_dependencies_number[i] != 0) {
			continue
		}

		printf "%.30s | %-3s %-6s | %-4s | %-4s | %-10s %s\n",
		i " ..............................",
		dependencies_number[i] "D",
		optional_dependencies[i],
		reverse_dependencies_number[i] "R",
		binaries_number[i] "B",
		size[i] " MiB",
		package[i]
	}
}
' <<EOF
$(pacman --query --info --list --quiet)
EOF
)"

if [ "${use_pager}" -eq 1 ]; then

	# # ${PAGER} will fallback to less if undefined
	# ${PAGER-less} <<-EOF
	# 	${output}
	# EOF
	echo "${output}"
	exit
fi

fzf --reverse --nth=1 --exact --preview "
pacman --query --info --list --quiet {1} | awk '
BEGIN {
	flag = 1
	line_length = 0
}

{
	if (flag == 1) {
		if (\$0 ~ /^Validated/) {
			printf \"Binaries        : \"
			flag = 0
			next
		}
		if (\$0 !~ /^Architecture/ &&
			\$0 !~ /^URL/ &&
			\$0 !~ /^Licenses/ &&
			\$0 !~ /^Groups/ &&
			\$0 !~ /^Packager/ &&
			\$0 !~ /^Install Reason/ &&
			\$0 !~ /^Install Script/) {
			print \$0
		}
	} else if (flag == -1) {
		if (!(sub(/^\/usr\/bin.*\//, \"\", \$0))) {
			# Exit once non-binary file is found.
			exit
		}
		line_length += length(\$0)
		if (line_length > 42) {
			printf \"\n                  \"
			line_length = 0
		}
		printf \"%s \", \$0
	} else if (flag == 0 && \$0 ~ /^\/usr\/bin\/$/) {
		flag = -1
	}
}

END {
	if (flag == -1) {
		printf \"\n\"
	} else {
		printf \"None\n\"
	}
}
'
" --multi --bind='ctrl-m:execute-multi(
	su -c "pacman --remove --cascade --nosave --recursive {1}"
)' <<-EOF
	${output}
EOF
