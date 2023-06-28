#if 0
true; exec ccompile "$0" -D_GNU_SOURCE -lcrypto \
-lcurl -target x86_64-pc-linux-gnu -- "$@"
#endif

#define ROUTER_URL "http://192.168.1.1/"
#define PASSWORD "Schlagmann47"
#define USERNAME "admin"

#include "huawei.h"

#define RESPONSE_MAX 20000
#define MAX 2
#define USERS_MAX 300

static double seconds_to_hours(char *seconds_str)
{
	return atoi(seconds_str) / 3600.0;
}

static void extract_xml(char *str, char *output[][MAX])
{
	char *(*ptr)[][MAX] = (char *(*)[][MAX]) output;
	for (char *i = str, *j = 0; *i; ++i) {
		if (*i == '<') {
			j = i + 1;
		} else if (j && *i == '>') {
			*i = 0;
			if (j[0] != '/') {
				***ptr = j;
				if (*(i + 1) == '<') {
					*(**ptr + 1) = "not_available";
					ptr = (char *(*)[][MAX]) (*ptr + 1);
					continue;
				}
				j = i + 1;
				while (*i != '<') {
					++i;
				}
				*i = 0;
				*(**ptr + 1) = j;
				ptr = (char *(*)[][MAX]) (*ptr + 1);
			}
			j = 0;
		}
	}
	***ptr = 0;
}

static unsigned char is_blocked(char *arr[], char *str)
{
	for (char **ptr = arr; *ptr; ++ptr) {
		if (!strcmp(*ptr, str)) {
			return 1;
		}
	}
	return 0;
}

static void chomp(char *str)
{
	for (char *p = str; *p; ++p) {
		if (*p == '\n') {
			*p = 0;
			return;
		}
	}
}

/*
static void switch_mac_filter_status(char *mac_address, unsigned char switch)
{
	char *request_data = 0;
	if (switch) {
	} else {
		asprintf(&request_data, "<request>"
		"<Ssids><Ssid>"
		"<WifiMacFilterStatus>2</WifiMacFilterStatus>"
		"<Index>0</Index>"
		"<WifiMacFilterMac0>%s</WifiMacFilterMac0>"
		"<wifihostname0></wifihostname0></Ssid></Ssids>"
		"</request>", mac_address);
	}
	send_request(0, 1,
		ROUTER_URL "api/wlan/multi-macfilter-settings",
		request_data, 0);
	free(request_data);
}
*/

int main()
{
	/* Remove ' = {0}' makes valgrind angry. */
	char response[RESPONSE_MAX] = {0};
	// send_request(0, 1, ROUTER_URL "api/wlan/host-list", 0, response);
	send_request(0, 1,
		ROUTER_URL "api/wlan/multi-macfilter-settings-ex", 0, response);
	send_request(0, 1, ROUTER_URL "api/lan/HostInfo", 0, response);
	printf("%s\n", response);

	char *associated_time[USERS_MAX][MAX];
	extract_xml(response, associated_time);

	printf("id|active| ip_address  |blocked|"
		"     hostname\n");
	char *blacklist[USERS_MAX];
	for (char i = 0, *(*ptr)[][MAX] = &associated_time, **q = blacklist;
		***ptr; ptr = (char *(*)[][MAX]) (*ptr + 1)) {
		if (!strcmp(***ptr, "WifiMacFilterMac0")) {
			*(q++) = *(**ptr + 1);
		} else if (!strcmp(***ptr, "Active")) {
			printf("%-2d|  %s   |", i++, *(**ptr + 1));
		} else if (!strcmp(***ptr, "IpAddress")) {
			printf("%.13s|", *(**ptr + 1));
		} else if (!strcmp(***ptr, "MacAddress")) {
			printf("   %d   |",
				is_blocked(blacklist, *(**ptr + 1)));
		} else if (!strcmp(***ptr, "HostName")) {
			printf("%s\n", *(**ptr + 1));
		}

		if (!strcmp(***ptr, "AssociatedTime")) {
			printf("%s\n", *(**ptr + 1));
		}
	}
	/*
	printf("\nid to block/toggle: ");
	char id[3];
	fgets(id, sizeof id, stdin);
	chomp(id);
	char device_index = (char) atoi(id);
	printf("%d\n", device_index);
	for (ptr = &associated_time; ***ptr; ptr = (char *(*)[][MAX]) (*ptr + 1)) {
		if (!strcmp(***ptr, "MacAddress") && device_index-- == 0) {
			if (is_blocked(blacklist, *(**ptr + 1)))
				unblock(*(**ptr + 1));
			} else {
				block(*(**ptr + 1));
			}
		}
	}
	*/
	/*
	send_request(0, 1, ROUTER_URL "api/device/information", 0, response);
	*/
	/*
	send_request(0, 1, ROUTER_URL "api/wlan/host-list", 0, response);
	*/
	/*
	send_request(0, 1, ROUTER_URL "api/dialup/profiles", 0, response);
	*/
	/*
	send_request(0, 1, ROUTER_URL "api/device/control",
		"<request><Control>1</Control></request>", response);
	*/
}
