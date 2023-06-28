#!/bin/sh

set -efu

shellAliases="/home/pactrag/.config/nixpkgs/shellAliases.nix"

# delete old stuff
sed -i '/###################################################/ q' "$shellAliases"
find /home/pactrag/Programme/usw/shell/inpath -name '*.sh' -not -name \
    fajitaSshfsMount.sh -not -name fajitaMtpMount.sh |
    while read -r filename; do
        scriptameWithout_sh="$(
		sed -e 's/^.*\///' \
            -e "s/\.sh.*$//" <<-EOF
			$filename
			EOF
		)"
        printf "  %s = \"%s\";\n" "$scriptameWithout_sh" "$filename" >>"$shellAliases"
    done

printf "}\n" >>"$shellAliases"

home-manager switch
