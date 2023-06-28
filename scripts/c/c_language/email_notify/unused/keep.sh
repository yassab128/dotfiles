#!/bin/sh

CLIENT_ID="575205653021-ivlgjpvhdm0mp9id16fc3t4irhmfclfk.apps.googleusercontent.com"
CLIENT_SECRET='6OslLIoWUNYZ_u2qsydIvSA7'
REDIRECT_URI='https://www.google.com'
CSRF_PROTECTION_STRING='NA'

obtain_authorization_code () {
firefox "https://accounts.google.com/o/oauth2/v2/auth?client_id=${CLIENT_ID}&\
scope=https://www.googleapis.com/auth/tasks&\
response_type=code&\
access_type=offline&\
state=${CSRF_PROTECTION_STRING}&\
redirect_uri=${REDIRECT_URI}"
}

AUTHORIZATION_CODE='4/0AX4XfWhgZWlOTdDbgGxq8wnxKjoQexm_hb3OfuhXMYp3nAfsr1QPlLd4ptJpyc_xSJ0oLg'

obtain_refresh_token () {
curl --silent --data "code=${AUTHORIZATION_CODE}&\
redirect_uri=${REDIRECT_URI}&\
client_id=${CLIENT_ID}&\
client_secret=${CLIENT_SECRET}&\
grant_type=authorization_code" 'https://oauth2.googleapis.com/token'
}

REFRESH_TOKEN="1//03NnWcUtGO5t0CgYIARAAGAMSNwF-L9Ir8oXECmxy-W4qYlztCSz-4nvhw-"\
"_UPexCISxs0YwE_7OVRlry2buA0qeLxxlFWcyOuLw"
