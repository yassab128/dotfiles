#include "../get_router_ip/get_router_gateway.h"
#include "../my_macros.h"
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* 64 KiB ought to be enough for anybody. */
#define RESPONSE_LENGTH 65535

/* -Wdisabled-macro-expansion */
#ifdef MSG_OOB
#	undef MSG_OOB
#endif

#define GATEWAY_MAX 16
static char g_gateway[GATEWAY_MAX];

#define REQUEST_HEADERS_MAX_SIZE 8192

#define SES_INFO_SIZE 1024
static char g_ses_info[SES_INFO_SIZE];

static unsigned char g_users_num;

static unsigned long
extract_xml(char *haystack, unsigned long haystack_length,
    const char *const needle, const unsigned long needle_length,
    char *substring)
{
	unsigned long substring_length;
	char *str = memmem(haystack, haystack_length, needle, needle_length);
	if (!str) {
		return 0;
	}
	str += needle_length;

	substring_length =
	    (unsigned long)((char *)memchr(str, '<',
				haystack_length -
				    (unsigned long)(str - haystack)) -
			    str);

	str[substring_length] = 0;

	memcpy(substring, str, substring_length + 1);
	return substring_length;
}

static unsigned long
get_response(
    char *response_buf, char *host, char *header_data, int header_data_length)
{
	int sockfd;
	struct addrinfo *res = 0;
	struct addrinfo hints;
	unsigned long received_bytes;

	hints.ai_flags = AI_NUMERICHOST | AI_NUMERICSERV;
	hints.ai_family = AF_INET;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_socktype = SOCK_STREAM;
	WARNIF(getaddrinfo(host, "80", &hints, &res));

	sockfd = socket(hints.ai_family, hints.ai_socktype, 0);
	WARNIF(sockfd == -1);

	WARNIF(connect(sockfd, res->ai_addr, res->ai_addrlen));
	freeaddrinfo(res);

	WARNIF(send(sockfd, header_data, (unsigned long)header_data_length + 1,
		   MSG_OOB) == -1);
	received_bytes = (unsigned long)recv(
	    sockfd, response_buf, RESPONSE_LENGTH, MSG_WAITALL);
	WARNIF(received_bytes == -1UL);
	/* WARNIF(!fwrite(response, 1, received_bytes, stdout)); */

	WARNIF(close(sockfd));
	return received_bytes;
}

static void
get_ses_info()
{
	char response[RESPONSE_LENGTH];
	unsigned long response_length;
	char header_data[REQUEST_HEADERS_MAX_SIZE];
	int header_data_length;

	header_data_length = sprintf(header_data,
	    "GET /api/webserver/SesTokInfo HTTP/1.1\nHost: %s\n\n", g_gateway);

	response_length =
	    get_response(response, g_gateway, header_data, header_data_length);
#define NEEDLE "<SesInfo>"
#define NEEDLELEN (sizeof(NEEDLE) - 1)
	extract_xml(response, response_length, NEEDLE, NEEDLELEN, g_ses_info);
#undef NEEDLE
#undef NEEDLELEN
}

static void *
get_network_users(void *arg)
{
	char response[RESPONSE_LENGTH];
	unsigned long response_length;
	char header_data[REQUEST_HEADERS_MAX_SIZE];
	int header_data_length;
	char current_wifi_users[3];

	/* Unused function argument */
	(void)arg;

	header_data_length = sprintf(header_data,
	    "GET /api/monitoring/status HTTP/1.1\n"
	    "Host: %s\n"
	    "Cookie: %s\n"
	    "\n",
	    g_gateway, g_ses_info);

	response_length =
	    get_response(response, g_gateway, header_data, header_data_length);

#define NEEDLE "<CurrentWifiUser>"
#define NEEDLELEN (sizeof(NEEDLE) - 1)
	extract_xml(
	    response, response_length, NEEDLE, NEEDLELEN, current_wifi_users);
#undef NEEDLE
#undef NEEDLELEN
	g_users_num = (unsigned char)atoi(current_wifi_users);
	return 0;
}

int
main()
{
	pthread_t thread_id[2];

	{
		unsigned int int_gateway;
		unsigned char *gateway;

		int_gateway = get_gateway();
		gateway = (unsigned char *)&int_gateway;
		sprintf(g_gateway, "%hhu.%hhu.%hhu.%hhu", gateway[0],
		    gateway[1], gateway[2], gateway[3]);
	}

	get_ses_info();

	WARNIF(pthread_create(thread_id, 0, get_network_users, (void *)0));
	WARNIF(pthread_join(thread_id[0], 0));

	printf("%hhu\n", g_users_num);

	return 0;
}
