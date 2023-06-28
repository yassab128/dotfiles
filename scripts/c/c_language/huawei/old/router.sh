#!/bin/sh

set -efu

username='admin'
password='Schlagmann47'

	router_ip='192.168.8.1'

ses_tok_info() {
	response="$(curl -s "http://${router_ip}/api/webserver/SesTokInfo")"
	ses_info="$(sed -n '
		s/.*<SesInfo>//
		T
		s/<.*//
		p
	' <<-EOF
	${response}
	EOF
	)"
	tok_info="$(sed -n '
		s/.*<TokInfo>//
		T
		s/<.*//
		p
	' <<-EOF
	${response}
	EOF
	)"

	password="$(printf '%s' "${password}" | sha256sum |
		awk '{printf "%s", $1}' | base64 -w0 )"
	password="$(printf '%s%s%s' ${username} ${password} ${tok_info} | sha256sum |
		awk '{printf "%s", $1}' | base64 -w0 )"
}

status() {
if false; then
	curl -s -w "%{http_code}" "http://${router_ip}/api/monitoring/status" \
		-H "Cookie: ${ses_info}"
else
	curl -s -w "%{http_code}" "http://${router_ip}/api/monitoring/status"
fi
}
traffic_statistics() {
	curl -s "http://${router_ip}/api/monitoring/traffic-statistics"
}

login_device() {
	curl -s "http://${router_ip}/api/user/login" \
		-H "__RequestVerificationToken: ${tok_info}" \
		-H "Cookie: SessionID=${ses_info}" \
		-d "<request><Username>${username}</Username><Password>${password}</Password></request>"
}

challenge_login() {
	firstnonce='0123456789012345678901234567890123456789012345678901234567890123'
	response="$(
	curl -si "http://${router_ip}/api/user/challenge_login" \
		-H "__RequestVerificationToken: ${tok_info}" \
		-H "Cookie: SessionID=${ses_info}" \
		-d "<request><username>${username}</username><firstnonce>\
		${firstnonce}</firstnonce></request>"
	)"
	salt="$(sed -n '
		s/.*<salt>//
		T
		s/<.*//
		p
	' <<-EOF
	${response}
	EOF
	)"

	servernonce="$(sed -n '
		s/.*<servernonce>//
		T
		s/<.*//
		p
	' <<-EOF
	${response}
	EOF
	)"
	Request_verification_token="$(sed -n '
		s/^__RequestVerificationToken: //
		T
		p
	' <<-EOF
	${response}
	EOF
	)"

	finalnonce='0123456789012345678901234567890123456789012345678901234567890123JqxC2XybldgfOo0IlVkImWeff59dmkqF'

	msg="$(printf "%s,%s,%s" "$firstnonce" "$finalnonce" "$finalnonce")"
	client_key="$(printf 'Schlagmann47' | nettle-pbkdf2 --iterations=100 \
		--hex-salt --length=32 "${salt}" --raw |
		openssl dgst -sha256 -hmac 'Client Key' -binary)"
	shit="$(printf '%s' "${client_key}" |
		openssl dgst -sha256 -binary |
		openssl dgst -sha256 -hmac "${msg}" -binary |
		awk '{printf "%s", $1}')"
	printf "%s\n" "${salt}"
}

authentication_login() {
	curl -s "http://${router_ip}/api/user/authentication_login" \
		-H "__RequestVerificationToken: ${tok_info}" \
		-H "Cookie: SessionID=${ses_info}" \
		-d "<request><clientproof>${clientproof}</clientproof><finalnonce>${finalnonce}</finalnonce></request>"
}

reboot_device() {
	curl -s "http://${router_ip}/api/device/control" \
		-H "__RequestVerificationToken: ${tok_info}" \
		-H "Cookie: SessionID=${ses_info}" \
		-d '<request><Control>1</Control></request>'
}

information() {
	curl -s "http://${router_ip}/api/device/information" \
		-H "__RequestVerificationToken: ${tok_info}" \
		-H "Cookie: SessionID=${ses_info}"
}


ses_tok_info

# authentication_login
# echo "${tok_info}"
# echo "${ses_info}"
# login_huawei
# login_device
# login_device
# challenge_login
# traffic_statistics
# information
# reboot_device
status
