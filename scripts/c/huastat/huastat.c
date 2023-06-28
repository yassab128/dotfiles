#if 0
	clear; make clean; make valgrind
	exit
#endif
#include "../get_router_ip/get_router_gateway.h"
#include <assert.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* 64 KiB ought to be enough for anybody. */
#define RESPONSE_LENGTH 65535

#define GATEWAY_MAX 16
static char g_gateway[GATEWAY_MAX];

#define REQUEST_HEADERS_MAX_SIZE 8192

#define SES_INFO_SIZE 1024
static char g_ses_info[SES_INFO_SIZE];

static size_t
extract_xml(char *haystack, size_t haystack_length, const char *const needle,
    const size_t needle_length, char *substring)
{
	size_t substring_length;
	char *str = memmem(haystack, haystack_length, needle, needle_length);
	if (str == NULL) {
		return 0;
	}
	str += needle_length;

	substring_length =
	    (size_t)((char *)memchr(
			 str, '<', haystack_length - (size_t)(str - haystack)) -
		     str);

	str[substring_length] = 0;

	memcpy(substring, str, substring_length + 1);
	return substring_length;
}

static size_t
get_response(char *response_buf, char *header_data, int header_data_size)
{
	int sockfd;
	struct addrinfo *res = 0;
	struct addrinfo hints;
	size_t received_bytes;
	ssize_t retval;

	hints.ai_flags = AI_NUMERICHOST | AI_NUMERICSERV;
	hints.ai_family = AF_INET;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_socktype = SOCK_STREAM;
	retval = getaddrinfo(g_gateway, "80", &hints, &res);
	assert(retval == 0);

	sockfd = socket(hints.ai_family, hints.ai_socktype, 0);
	assert(sockfd != -1);

	retval = connect(sockfd, res->ai_addr, res->ai_addrlen);
	assert(retval == 0);

	freeaddrinfo(res);

	retval = send(sockfd, header_data, (size_t)header_data_size, MSG_OOB);
	assert(retval == header_data_size);
	for (;;) {
		received_bytes = (size_t)recv(
		    sockfd, response_buf, RESPONSE_LENGTH, MSG_WAITALL);
		if (received_bytes != -1UL) {
			break;
		}
		sleep(1);
		fwrite("...\n", 1, 4, stderr);
	}

	retval = close(sockfd);
	assert(retval == 0);

	return received_bytes;
}

static void
get_ses_info()
{
	char response[RESPONSE_LENGTH];
	size_t response_length;
	char header_data[REQUEST_HEADERS_MAX_SIZE];
	int header_data_length;

	header_data_length = sprintf(header_data,
	    "GET /api/webserver/SesTokInfo HTTP/1.1\nHost: %s\n\n", g_gateway);

	response_length =
	    get_response(response, header_data, header_data_length + 1);
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
	size_t response_length;
	char header_data[REQUEST_HEADERS_MAX_SIZE];
	int header_data_length;
	char current_wifi_users[4];

	/* Unused function argument */
	(void)arg;

	header_data_length = sprintf(header_data,
	    "GET /api/monitoring/status HTTP/1.1\r\n"
	    "Host: %s\r\n"
	    "Cookie: %s\r\n"
	    "\r\n",
	    g_gateway, g_ses_info);

	response_length =
	    get_response(response, header_data, header_data_length + 1);

#define NEEDLE "<CurrentWifiUser>"
#define NEEDLELEN (sizeof(NEEDLE) - 1)
	extract_xml(
	    response, response_length, NEEDLE, NEEDLELEN, current_wifi_users);
#undef NEEDLE
#undef NEEDLELEN
	*(unsigned char *)arg = (unsigned char)atoi(current_wifi_users);
	return 0;
}

static int rx;
static int tx;

static void *
get_netspeed(void *arg)
{
	char response[RESPONSE_LENGTH];
	size_t response_length;
	char header_data[REQUEST_HEADERS_MAX_SIZE];
	int header_data_length;
	char substring[16];

	/* Unused function argument */
	(void)arg;

	header_data_length = sprintf(header_data,
	    "GET /api/monitoring/traffic-statistics HTTP/1.1\r\n"
	    "Host: %s\r\n"
	    "Cookie: %s\r\n"
	    "\r\n",
	    g_gateway, g_ses_info);

	response_length =
	    get_response(response, header_data, header_data_length + 1);

#define NEEDLE "<CurrentDownloadRate>"
#define NEEDLELEN (sizeof(NEEDLE) - 1)
	extract_xml(response, response_length, NEEDLE, NEEDLELEN, substring);
#undef NEEDLE
#undef NEEDLELEN
	rx = atoi(substring);

#define NEEDLE "<CurrentUploadRate>"
#define NEEDLELEN (sizeof(NEEDLE) - 1)
	extract_xml(response, response_length, NEEDLE, NEEDLELEN, substring);
#undef NEEDLE
#undef NEEDLELEN
	tx = atoi(substring);

	return 0;
}

struct readable_size {
	double value;
	char *unit;
};

static struct readable_size
humanize_size(const int bytes)
{
	struct readable_size size;

	if (bytes > 1048576) {
		size.value = bytes / 1048576.0;
		size.unit = "\033[1;31mMiB/s\033[0m";
	} else if (bytes > 1024) {
		size.value = bytes / 1024.0;
		size.unit = "\033[1;33mKiB/s\033[0m";
	} else {
		size.value = bytes;
		size.unit = "\033[1;32mB/s\033[0m";
	}
	return size;
}

int
main()
{
	pthread_t thread_id[2];
	int retval;
	struct readable_size rxh;
	struct readable_size txh;
	unsigned char nusers = 0;

	{
		unsigned int int_gateway;
		unsigned char *gateway;

		int_gateway = get_gateway();
		gateway = (unsigned char *)&int_gateway;
		sprintf(g_gateway, "%hhu.%hhu.%hhu.%hhu", gateway[0],
		    gateway[1], gateway[2], gateway[3]);
	}

	get_ses_info();

	for (;;) {
		retval = pthread_create(
		    thread_id, NULL, get_network_users, (void *)&nusers);
		assert(retval == 0);

		retval =
		    pthread_create(thread_id + 1, NULL, get_netspeed, NULL);
		assert(retval == 0);

		retval = pthread_join(thread_id[0], NULL);
		assert(retval == 0);

		retval = pthread_join(thread_id[1], NULL);
		assert(retval == 0);

		rxh = humanize_size(rx);
		txh = humanize_size(tx);

		printf(""
		       "RX: %.4g %s"
		       " | Users: \033[1;34m%hhu\033[0m"
		       " | TX: %.4g %s"
		       "\n",
		    rxh.value, rxh.unit, nusers, txh.value, txh.unit);
		sleep(2);
	}

	return 0;
}
