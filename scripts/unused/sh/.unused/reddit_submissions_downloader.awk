#!/usr/bin/awk -Pf

# TODO gifs are not downloadable yet.

BEGIN {
	 LINT = "fatal"

	if (ARGC != 2) {
		exit
	}

	print_only = 0

	user = ARGV[1]
	# --location is needed for some users, somehow.
	curl_command = "/usr/bin/curl --silent --user-agent '' "
	curl_cmd = curl_command "'https://old.reddit.com/search?q=author%3A" user\
		"+nsfw%3A1+self%3A0&include_over_18=on&limit=100'"

	keep_going = 1
	while (keep_going) {
		go_to_next_link = 0

		curl_cmd | getline
		split($0, arr, /"/)
		for (i in arr) {

			if (print_only) {
				print arr[i]
				continue
			}

			# Get the next page, if it exists.
			if (arr[i] == "nofollow next") {
				next_curl_cmd = curl_command "'" arr[i - 2] "'"
				go_to_next_link = 1
			}

			# Skip if not a media link.
			if (arr[i] !~ /^search-result-icon/) {
				continue
			}

			if (arr[i + 1] != "></span><a href=") {
				continue
			}

			# Sometimes, reddit links such as 'https://www.reddit.com/gallery',
			# we got to prevent them from showing up.
			if (arr[i + 2] ~ /reddit\.com\//) {
				continue
			}

			urls[arr[i + 2]] = ""
		}
		close(curl_cmd)

		if (go_to_next_link) {
			keep_going = 1
			curl_cmd = next_curl_cmd
		} else {
			keep_going = 0
		}
	}

	if(print_only) {
		exit
	}
	total = 0
	sh_cmd = "/usr/bin/mkdir '" ARGV[1] "'; cd '" ARGV[1] "'; "\
		   "/usr/bin/curl --parallel --continue-at -"
	for (i in urls) {
		sh_cmd = sh_cmd " --remote-name --remote-header-name '" i "'"
		++total
	}

	printf "Download %s files (y/n): ", total
    getline choice < "/dev/stdin"
	close("/dev/stdin")

	if (choice == "y") {
		system(sh_cmd)
	}
}
