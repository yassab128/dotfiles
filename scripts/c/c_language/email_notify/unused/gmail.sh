#!/bin/sh

CLIENT_ID="575205653021-ivlgjpvhdm0mp9id16fc3t4irhmfclfk.apps.googleusercontent.com"
CLIENT_SECRET='6OslLIoWUNYZ_u2qsydIvSA7'
REDIRECT_URI='https://www.google.com'
CSRF_PROTECTION_STRING='NA'

obtain_authorization_code () {
firefox "https://accounts.google.com/o/oauth2/v2/auth?client_id=${CLIENT_ID}&\
scope=https://mail.google.com&\
response_type=code&\
access_type=offline&\
state=${CSRF_PROTECTION_STRING}&\
redirect_uri=${REDIRECT_URI}"
}

AUTHORIZATION_CODE='INPUT HERE'

obtain_refresh_token () {
curl --silent --data "code=${AUTHORIZATION_CODE}&\
redirect_uri=${REDIRECT_URI}&\
client_id=${CLIENT_ID}&\
client_secret=${CLIENT_SECRET}&\
grant_type=authorization_code" 'https://oauth2.googleapis.com/token'
}

REFRESH_TOKEN='INPUT HERE'
