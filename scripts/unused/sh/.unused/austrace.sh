#!/bin/sh -eu

# Do not forget to enable auditd.service

# show_report_only=false
# while getopts "c" opt; do
# 	case "${opt}" in
# 	r)
# 		show_report_only=true
# 		;;
# 	*)
# 		;;
# 	esac
# done

# auditctl -D >&-

command=$(command -v $1)
shift
pid=$(autrace ${command} $@)
pid=${pid##* }
pid=${pid%?}

# ausearch -r -p "${pid}" | aureport
ausearch -p "${pid}" | less
# ausearch -r -p "${pid}"
