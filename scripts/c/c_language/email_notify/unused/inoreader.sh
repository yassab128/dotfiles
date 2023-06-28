#!/bin/sh

CLIENT_SECRET='YEAWn2F1y8bcw5T7UzrUsqBxhy2PArKO'
CLIENT_ID='999998025'
REDIRECT_URI='https://www.google.com'
OPTIONAL_SCOPES='read write'
CSRF_PROTECTION_STRING='NA'

obtain_authorization_code () {
# You have to this manually.
firefox "https://www.inoreader.com/oauth2/auth?client_id=${CLIENT_ID}"\
"&redirect_uri=${REDIRECT_URI}&response_type=code&scope=${OPTIONAL_SCOPES}"\
"&state=${CSRF_PROTECTION_STRING}"
}

AUTHORIZATION_CODE='ed2d07c086d236cadd193fd6b3370c2324b685bf'

obtain_refresh_token () {
curl --silent --data "code=${AUTHORIZATION_CODE}&\
redirect_uri=${REDIRECT_URI}&\
client_id=${CLIENT_ID}&\
client_secret=${CLIENT_SECRET}&\
scope=&\
grant_type=authorization_code" 'https://www.inoreader.com/oauth2/token'
}
REFRESH_TOKEN='b26bdec943091e739e6930f766ee44c5849479ea'

if false; then
	obtain_authorization_code
else
	obtain_refresh_token
fi
