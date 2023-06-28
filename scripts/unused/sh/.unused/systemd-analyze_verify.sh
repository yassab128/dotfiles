#!/bin/sh

set -efu
PATH=/usr/bin
export DBUS_SESSION_BUS_ADDRESS='unix:path=/run/user/1000/bus'

systemd_analyze_verify_output="$(
	find "${HOME}/.config/systemd/user/" \
		/etc/systemd/system/ \
		/etc/systemd/user/ \
		-maxdepth 1 -type f -name '*\.service' \
		-exec systemd-analyze verify '{}' \+ 2>&1
	# Notice 2>&1, `wc` will not work without
)" || true

# echo "${systemd_analyze_verify_output}" | wc -l

errors_number="$(
	wc --lines <<-EOF
		${systemd_analyze_verify_output}
	EOF
)"

main() {
	# `systemd-analyze verify` outputs an empty line when there
	# are no errors
	if [ "${systemd_analyze_verify_output}" = "\n" ]; then
		dunstify --urgency low --timeout=2000 \
			"systemd-analyze errors: ${errors_number}"
	fi
}

while getopts 'p' option; do
	case "${option}" in
	p)
		printf "%s\n" "${systemd_analyze_verify_output}"
		exit
		;;
	*)
		printf 'Illegal option!\n'
		exit 1
		;;

	esac
done

main
