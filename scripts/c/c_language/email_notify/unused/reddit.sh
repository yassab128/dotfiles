#!/bin/sh

set -efu

CLIENT_SECRET='7r604XVeUjlGsKWF_D32PAqB_aMR8Q'
CLIENT_ID='xFAbVaCdesHMMFedjAk29A'
REDIRECT_URI='https://www.google.com'
OPTIONAL_SCOPES='read'
RANDOM_STRING='NA'

obtain_authorization_code () {
firefox "https://www.reddit.com/api/v1/authorize?client_id=${CLIENT_ID}&"\
"response_type=code&state=${RANDOM_STRING}&duration=permanent&"\
"redirect_uri=${REDIRECT_URI}&scope=privatemessages"
}

code='6pcSO1uzupPZDxq4sfdtB2eQmBxSoQ#_'

obtain_refresh_token () {
curl --silent --data "grant_type=authorization_code&code=${code}&"\
"client_id=${CLIENT_ID}&"\
"client_secret=${CLIENT_SECRET}&"\
"redirect_uri=${REDIRECT_URI}" 'https://www.reddit.com/api/v1/access_token'
}
REFRESH_TOKEN='INPUT HERE'

switch=0
if [ ${switch} -eq 1 ]; then
	obtain_authorization_code
else
	obtain_refresh_token
fi
