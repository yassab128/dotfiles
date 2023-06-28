#!/bin/sh

terminal_window_id=$(xdotool getwindowfocus)

get_programs="$(
	awk '
{
	if ($0 ~ /^Name=/) {
		sub(/^Name=/, "", $0)
		Name = $0
	} else if ($0 ~ /^Exec=/) {
		# e.g. `/usr/lib/firefox/firefox %u`
		sub(/^Exec=/, "", $0)
		sub(/ %[FUcfiku].*/, "", $0)
		Exec = $0
	} else if ($0 ~ /^Icon=/) {
		sub(/^Icon=/, "", $0)
		# sub(/.*\//, "", FILENAME)
		# sub(/\.desktop$/, "", FILENAME)
		print Name, Exec, $0
		nextfile
	}
}
' /usr/share/applications/*
)"

fzf_menu() {
	fzf --with-nth='1..-3' --preview-window=0% --preview "
	file_path=\$(
	awk '
	BEGIN {
		biggest_file_size = 0
	}

	{
		if (\$1 > biggest_file_size) {
			biggest_file_size = \$1
			biggest_file = \$2
		}
	}

	END {
		print biggest_file
	}
	' <<-EOF 2>/dev/null
	# The dot in '.*' is litteral (trust me).
	\$(find /usr/share/icons/ /usr/share/pixmaps -type f -name '{-1}.*' \
		-exec identify -format '%w %i\n' \{\} \+)
	EOF
	)
	if [ -n \"\${file_path}\" ]; then
		display \${file_path} &
		sleep 0.1
		xdotool windowactivate ${terminal_window_id}
	fi
" <<-EOF
		${get_programs}
	EOF
}

command2run() {
	awk '
{
	print $(NF - 1), "&"
}
' <<-EOF
		$(fzf_menu)
	EOF
}

run_command() {
	eval "$(command2run)"
}

run_command
