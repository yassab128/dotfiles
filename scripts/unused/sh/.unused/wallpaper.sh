#!/bin/sh

set -efu

PATH=/usr/bin/
# Needed by ImageMagick.
export DISPLAY=':0'
# Needed by Dunst.
export XAUTHORITY='/home/pactrag/.Xauthority'

url_only=0
while getopts 'u' option; do
	case "${option}" in
	u)
		url_only=1
		;;
	*)
		printf 'Illegal option!\n'
		exit 1
		;;

	esac
done

# single quotes trigger shfmt
url="https://www.reddit.com/r/\
AbandonedPorn+\
AdrenalinePorn+\
raining+\
AerialPorn+\
AgriculturePorn+\
AmateurEarthPorn+\
AmateurPhotography+\
ArchitecturePorn+\
AutumnPorn+\
Beachporn+\
CabinPorn+\
CemeteryPorn+\
CityPorn+\
EarthPorn+\
EarthlingPorn+\
ExposurePorn+\
FreezingFuckingCold+\
InfrastructurePorn+\
LandscapePhotography+\
MegalithPorn+\
MostBeautiful+\
Outdoors+\
Pictures+\
RoadPorn+\
SkyPorn+\
SpringPorn+\
SummerPorn+\
ViewPorn+\
VillagePorn+\
WeatherPorn+\
bridgeporn+\
churchporn+\
hiking+\
itookapicture+\
lakeporn+\
landscaping+\
nature+\
naturepics+\
natureporn+\
pic+\
ruralporn+\
telephotolandscapes+\
travel+\
waterporn+\
winterporn\
/search?restrict_sr=1&q=\
nsfw%3Ano%20self%3Ano%20(title%3AGerman%20OR%20title%3AGermany)&sort=new"

if [ "${url_only}" -eq 1 ]; then
	"${BROWSER-}" "${url}"
	exit
fi

eval "$(
	curl --silent --user-agent "" "${url}" | awk '
{
	for (i = 1; i < NF; ++i) {
		if ($i ~ /^post=/) {
			picture_url = $(i - 1)
			sub(/^href="/, "", picture_url)
			sub(/"$/, "", picture_url)
			print "curl --silent --output /dev/stdout",
			picture_url,
			# the bang is very important
			"| display -resize \0471366x768!\047 -window root /dev/stdin"
			exit
		}
	}
}
'
)" && dunstify 'The wallpaper has been updated'
