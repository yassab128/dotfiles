#!/bin/sh

awk '
BEGIN {
	package_name_length_max = 0
	status_length_max = 0
	description_length_max = 0
}

{
	if (NR == FNR) {
		arch_packages[$0] = ""
	} else {
		package = $0
		sub(/:.*/, "", package)
		status = $0
		sub(/^[^\[]+\[/, "", status)
		sub(/\].*/, "", status)
		description = $0
		sub(/[^[:space:]]+[[:space:]]/, "", description)
		if (package in arch_packages) {
			if (length(package) > package_name_length_max) {
				package_name_length_max = length(package)
			}
			if (length(status) > status_length_max) {
				status_length_max = length(status)
			}
			if (length(description) > description_length_max) {
				description_length_max = length(description)
			}
			stat[package] = status
			reason[package] = description
		}
	}
}

END {
	for (i = 0; i <= package_name_length_max / 2; ++i) {
		printf "_"
	}
	printf "Name"
	for (i = 0; i <= (4 + status_length_max) / 2; ++i) {
		printf "_"
	}
	printf "Status"
	for (i = 0; i <= (6 + description_length_max) / 2; ++i) {
		printf "_"
	}
	printf "Description"
	for (i = 0; i <= (6 + description_length_max) / 2; ++i) {
		printf "_"
	}
	printf "\n"
	for (package in reason) {
		printf "%s", package
		for (i = length(package); i <= package_name_length_max; ++i) {
			printf " "
		}
		printf "| %s", stat[package]
		for (i = length(stat[package]); i <= status_length_max; ++i) {
			printf " "
		}
		printf "| %s\n", reason[package]
	}
}
' /dev/fd/3 /dev/fd/4 /dev/fd/5 3<<-EOF 4<<-EOF 5<<-EOF
	$(pacman --query --explicit --quiet)
EOF
	$(curl -s https://git.parabola.nu/blacklist.git/plain/blacklist.txt)
EOF
	$(curl -s https://git.parabola.nu/blacklist.git/plain/aur-blacklist.txt)
EOF
