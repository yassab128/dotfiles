#!/bin/sh

set -efu

templateParameters='id: %(id)s
title: %(title)s
url: %(url)s
ext: %(ext)s
alt_title: %(alt_title)s
display_id: %(display_id)s
uploader: %(uploader)s
license: %(license)s
creator: %(creator)s
release_date: %(release_date)s
timestamp: %(timestamp)s
upload_date: %(upload_date)s
uploader_id: %(uploader_id)s
channel: %(channel)s
channel_id: %(channel_id)s
location: %(location)s
duration: %(duration)s
view_count: %(view_count)s
like_count: %(like_count)s
dislike_count: %(dislike_count)s
repost_count: %(repost_count)s
average_rating: %(average_rating)s
comment_count: %(comment_count)s
age_limit: %(age_limit)s
is_live: %(is_live)s
start_time: %(start_time)s
end_time: %(end_time)s
format: %(format)s
format_id: %(format_id)s
format_note: %(format_note)s
width: %(width)s
height: %(height)s
resolution: %(resolution)s
tbr: %(tbr)s
abr: %(abr)s
acodec: %(acodec)s
asr: %(asr)s
vbr: %(vbr)s
fps: %(fps)s
vcodec: %(vcodec)s
container: %(container)s
filesize: %(filesize)s
filesize_approx: %(filesize_approx)s
protocol: %(protocol)s
extractor: %(extractor)s
extractor_key: %(extractor_key)s
epoch: %(epoch)s
autonumber: %(autonumber)s
playlist: %(playlist)s
playlist_index: %(playlist_index)s
playlist_id: %(playlist_id)s
playlist_title: %(playlist_title)s
playlist_uploader: %(playlist_uploader)s
playlist_uploader_id: %(playlist_uploader_id)s
ailable for the video that belongs to some logical chapter or section:: %(ailable)s
chapter: %(chapter)s
chapter_number: %(chapter_number)s
chapter_id: %(chapter_id)s
ailable for the video that is an episode of some series or programme:: %(ailable)s
series: %(series)s
season: %(season)s
season_number: %(season_number)s
season_id: %(season_id)s
episode: %(episode)s
episode_number: %(episode_number)s
episode_id: %(episode_id)s
ailable for the media that is a track or a part of a music album:: %(ailable)s
track: %(track)s
track_number: %(track_number)s
track_id: %(track_id)s
artist: %(artist)s
genre: %(genre)s
album: %(album)s
album_type: %(album_type)s
album_artist: %(album_artist)s
disc_number: %(disc_number)s
release_year: %(release_year)s'

playlistURL='https://www.youtube.com/playlist?list=PLOUzUrKhNae51rhMgkY_v1BWWISuvjmkk'
playlistFlat="$(youtube-dl --flat-playlist "$playlistURL" | wc -l)"
playlistVideosNumber=$((playlistFlat - 4))
echo "$playlistVideosNumber"

for ((i = 1; i <= "$playlistVideosNumber"; i++)); do
	youtube-dl "$playlistURL" --playlist-items "$i" --get-filename \
		--skip-download -o "$templateParameters" | grep -v " NA$"
	youtube-dl "$playlistURL" \
		--quiet --playlist-items "$i" --format bestaudio --get-url |
		xargs mpv --no-video --msg-level=cplayer=no
		clear; tmux clear-history
done
