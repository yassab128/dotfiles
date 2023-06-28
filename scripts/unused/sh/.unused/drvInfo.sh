#!/bin/sh

set -efu

# QUERYING INSTALLED PACKAGES UNDER NIXOS IS HELL

# NIX-* cannot into query
# nix-store is the only commands the can query installed packages
# nix-env is good at querying repository packages
# dependencies
drvId="$(readlink -e "$(which "$1")"| grep -Po '(?<=store\/)(.*?)(?=\/)')"
dependencies="$(nix-store --query --references "$(which "$1")" |
	sed -e "/$drvId/d" `# remove everything before first - (path)`\
	-e "s/^[^-]*-//" `# delete lines containing home-manager`\
	-e "s/-[0-9].*//" `# delete everthing after last - (version)` |
	sort --unique)"
	#-e "s/-[0-9][^-]\+$//" `# delete everthing after last - (version)`)"

# reverse dependencies
reverseDependencies="$(nix-store --query --referrers "$(which "$1")" |
	# delete lines containing home-manager\
	sed -e "/home-manager-path\|$drvId/d 
	s/^[^-]*-//" `# remove everything before first -`\
		"s/-[0-9].*//" `# delete everthing after last - (version)` |
		sort --unique)"

#total size; not an accurate representation of the drv size thought
#printf "total size\n"
#readlink -e "$(which "$1")" |
## get stuff between the first and and fourth slash
#grep -Eo "^\/[^/]+\/([^/]+)+\/([^/]+)\/" | xargs du -sh


# pr -m -t <(printf '\e[32mdependencies\e[0m\n%s' "$dependencies")
# 	<(printf '\e[36mreverse dependencies\e[0m\n%s' "$reverseDependencies") |
# 	most

{
  printf '\e[32mdependencies\e[0m\n%s' "$dependencies" 4<&- | {
    # in here fd 3 points to the reading end of the pipe
    # from cmd1, while fd 0 has been restored from the original
    # stdin (saved on fd 4, now closed as no longer needed)

    printf '\e[36mreverse dependencies\e[0m\n%s' "$reverseDependencies" 3<&- | pr -m -t /dev/fd/3 -

  } 3<&0 <&4 4<&- # restore the original stdin for cmd2

} 4<&0 | most

find /usr/bin/ -type l -exec readlink {} \+ | sed -E 's|[^-]+-([^/]+)/.*|\1|g' | sort -u

# this is the way
nix-store --query --references /var/usr /usr/bin/ | cut -d'-' -f2-


nix-store --query --references \
	/var/usr /usr/bin/ |
	awk '
BEGIN {
	FS = "-"
}

{
	for (i = 2; i <= NF - 1; i++) {
		printf "%s-", $i
	}
printf "\n"
}

'
