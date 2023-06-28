#!/usr/bin/awk -f

BEGIN {
	# LINT = "fatal"
	stderr = "/dev/stderr"
	if (ARGC == 2) {
		video_itag = ""
	} else if (ARGC == 3) {
		video_itag = ARGV[2]
		if (video_itag !~ /[0-9]+/) {
			print "Non-valid itag" > stderr
			exit 1
		}
	} else {
		print "ARGC must either be 2 or 3" > stderr
		exit
	}
	video_url = ARGV[1]
	sub(/\/$/, "", video_url);
	alltube_url = "https://alltubedownload.net/info?url=" video_url
	cmd = "curl -Ls '" alltube_url "'"
	video_title = ""
	while ((cmd | getline) > 0) {
		if (/<\/a><\/i>\. <\/p>$/) {
			sub(/<.*/, "")
			video_title = $0
			print > stderr
		} else if (/^<option value="[0-9]+"/) {
			sub(/<option value="/, "")
			sub(/\([^)]+\)<\/option>/, "")
			sub(/">/, " ")
			gsub(/&nbsp;/, " ")
			gsub(/ +/, " ")
			extension[$1] = $2
			print > stderr
		}
	}
	close(cmd)
	if (!video_title) {
		print "Video not found" > stderr
		exit 1
	}
	if (!video_itag) {
		printf "\nitag: " > stderr
		stdin = "/dev/stdin"
		getline video_itag < stdin
		close(stdin)
	}
	file_extension = ""
	for (i in extension) {
		if (i == video_itag) {
			file_extension = extension[i]
			break;
		}
	}
	if (!file_extension) {
		print "Non-valid itag" > stderr
		exit 1
	}
	gsub(/'/, "'\\''", video_title);
	gsub(/\//, "\\", video_title);
	download_url = "https://alltubedownload.net/download?url="\
				video_url "&format=" video_itag
	sh_cmd = "if [ -t 1 ]; then "\
			"exec curl -LC- -o'" video_title "." file_extension\
			"' '" download_url "';"\
		"else "\
			"exec curl -Ls '" download_url "';"\
		"fi"
	system(sh_cmd)
}
