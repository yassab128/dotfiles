#!/bin/sh

# for more information, check:
# https://developer.accuweather.com/accuweather-locations-api/apis

# Get the API key from https://developer.accuweather.com/user/me/apps
api_key='eBB446WvJXzYg2rxSqFhReEm1pb7sPFn'

# Searched for 'El Helalma' and got the location key from the end of the url: 
# https://www.accuweather.com/en/ma/el-helalma/246067/weather-forecast/246067
# By the way, You cannot use lat & lon as it is the case with OpenWeatherMap.
el_helalma_location_key='246067'

# 1day could be changed to 5day, the free plan does not allow more.
curl -s "http://dataservice.accuweather.com/forecasts/v1/daily/\
1day/${el_helalma_location_key}?apikey=${api_key}"

# 1hour could be changed to 12hour, the free plan does not allow more.
# curl -s "http://dataservice.accuweather.com/forecasts/v1/hourly/\
# 1hour/${el_helalma_location_key}?apikey=${api_key}"
