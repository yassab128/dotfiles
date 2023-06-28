#!/usr/bin/gawk -f

# Script abandonned: AWK does not work with 'env -i'.

BEGIN {
	LINT = "fatal"
	url_only = ARGC == 2 && ARGV[1] == "url" ? 1 : 0
	url="https://www.reddit.com/r/"\
	"AbandonedPorn+"\
	"AdrenalinePorn+"\
	"raining+"\
	"AerialPorn+"\
	"AgriculturePorn+"\
	"AmateurEarthPorn+"\
	"wallpapers+"\
	"ultrahdwallpapers+"\
	"AmateurPhotography+"\
	"ArchitecturePorn+"\
	"AutumnPorn+"\
	"Beachporn+"\
	"CabinPorn+"\
	"CemeteryPorn+"\
	"CityPorn+"\
	"EarthPorn+"\
	"EarthlingPorn+"\
	"ExposurePorn+"\
	"FreezingFuckingCold+"\
	"InfrastructurePorn+"\
	"LandscapePhotography+"\
	"MegalithPorn+"\
	"MostBeautiful+"\
	"Outdoors+"\
	"Pictures+"\
	"RoadPorn+"\
	"SkyPorn+"\
	"SpringPorn+"\
	"SummerPorn+"\
	"ViewPorn+"\
	"VillagePorn+"\
	"WeatherPorn+"\
	"bridgeporn+"\
	"churchporn+"\
	"hiking+"\
	"itookapicture+"\
	"lakeporn+"\
	"landscaping+"\
	"nature+"\
	"naturepics+"\
	"natureporn+"\
	"pic+"\
	"ruralporn+"\
	"telephotolandscapes+"\
	"travel+"\
	"waterporn+"\
	"winterporn"\
	"/search?restrict_sr=1&q="\
	"nsfw%3Ano%20self%3Ano%20(title%3AGerman%20OR%20title%3AGermany)&sort=new"
	curl_cmd = "/usr/bin/curl --silent --user-agent '' '" url "'" " || true"

	if(url_only) {
		print url
		exit
	}

	# This is the only way to break out of a parent loop in AWK.
	picture_not_found_yet = 1

	while (picture_not_found_yet && (curl_cmd | getline) > 0) {
		for (i = 1; i < NF; ++i) {
			if ($i ~ /^post=/) {
				picture_url = $(i - 1)
				sub(/^href="/, "", picture_url)
				sub(/"$/, "", picture_url)
				set_wallpaper_cmd =  "export DISPLAY=':0' "\
				"XAUTHORITY='/home/pactrag/.Xauthority';"\
				"/usr/bin/curl --silent '" picture_url "' | /usr/bin/display "\
				"-resize '1366x768!' -window root /dev/stdin&&"\
				"/usr/bin/dunstify 'The wallpaper has been updated'"
				picture_not_found_yet = 0
				break
			}
		}
	}
	close(curl_cmd)

	system(set_wallpaper_cmd)
}
