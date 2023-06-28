#!/bin/sh

# for more information, check:
# https://openweathermap.org/api

# Get the API key from https://home.openweathermap.org/api_keys
api_key='41c6bb7d4eb24f20199fe3222bbc6550'

# https://openweathermap.org/city/2555745
el_helalma_location_key='2555745'

# lat and lon are rounded to two digits after the decimial point.
lat='32.36'
lon='-6.57'

# curl -s "api.openweathermap.org/data/2.5/onecall?lat=${lat}&lon=${lon}&\
# appid=${api_key}&units=metric"

# curl -s "api.openweathermap.org/data/2.5/weather?lat=${lat}&lon=${lon}&\
# appid=${api_key}&units=metric"

curl -s "api.openweathermap.org/data/2.5/forecast?lat=${lat}&lon=${lon}&\
appid=${api_key}&units=metric"
