#!/bin/sh

message=$1
id=$2
timeout=$3

while read -r line; do
	set -- ${line}
	if [ "$1" -ne 0 ]; then
		break
	fi
done <<-EOF
	$(loginctl list-users --no-legend)
EOF

notification_command="DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/$1/bus \
	/usr/bin/gdbus call --session --dest=org.freedesktop.Notifications \
	--object-path=/org/freedesktop/Notifications \
	--method=org.freedesktop.Notifications.Notify \
	'' ${id} '' ${message} '' '[]' '{}' ${timeout}"

if [ "${UID}" -ne 0 ]; then
	eval "${notification_command}"
	exit
fi

runuser "$2" -c "${notification_command}"
# vi:noet
