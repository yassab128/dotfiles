#if 0
DIR=~/.local/bin exec ccompile "$0" -D_GNU_SOURCE -lcrypto -lcurl -- "$@"
#endif

#include "huawei.h"
#include <time.h>

/* sizeof("255.255.255.255") = 16 */
#define IPV4_ADDRESS_MAX_LENGTH 16

/*
 * All MAC addresses look similar to this F2:4A:D6:40:F2:6F
 sizeof("F2:4A:D6:40:F2:6F") = 18
 */
#define MAC_ADDRESS_MAX_LENGTH 18

/*
 * Hostnames cannot span more than 256 characters in general, but the
 * particular router I use does not allow more than ~64 hostname chracters;
 * despite this, HOSTNAME_MAX is set to 85 instead of 64 in order to make
 * the struct perfectly alligned.
 */
#define HOSTNAME_MAX 85

#define RECOMMENDED_ALIGNMENT 128
struct wifi {
	unsigned int associated_time;
	unsigned int lease_time;
	char ipv4_address[IPV4_ADDRESS_MAX_LENGTH];
	char mac_address[MAC_ADDRESS_MAX_LENGTH];
	char hostname[HOSTNAME_MAX];
	unsigned char blacklisted;
} __attribute__((aligned(RECOMMENDED_ALIGNMENT)));

static void extract_str(char *response, char *pattern,
	char *struct_element)
{
	struct re_pattern_buffer preg;
	regcomp(&preg, pattern, REG_EXTENDED);
	regmatch_t matches[2];
	for ((void)regexec(&preg, response, 2, matches, 0);
			!regexec(&preg, response, 2, matches, 0);
			struct_element +=
				sizeof(struct wifi) / sizeof(*struct_element)) {
		memcpy(struct_element,
			response + matches[1].rm_so,
			(unsigned long)(matches[1].rm_eo - matches[1].rm_so));
		response += matches->rm_eo;
	}
	regfree(&preg);
}

static void extract_int(char *response, char *pattern,
	unsigned int *struct_element)
{
	struct re_pattern_buffer preg;
	regcomp(&preg, pattern, REG_EXTENDED);
	regmatch_t matches[2];
	for ((void)regexec(&preg, response, 2, matches, 0);
			!regexec(&preg, response, 2, matches, 0);
			struct_element +=
				sizeof(struct wifi) / sizeof(*struct_element)) {
		*struct_element = (unsigned int)
			strtoul(response + matches[1].rm_so, 0, DECIMAL_BASE);
		response += matches->rm_eo;
	}
	regfree(&preg);
}

static void allocate_array(struct wifi **user, char *string)
{
	unsigned char count = 0;
	char substring[] = "<MacAddress>";
	unsigned char substring_size = sizeof(substring) - 1;
	for (char *ptr = strstr(string, substring); ptr;) {
		string = ptr + substring_size;
		++count;
		ptr = strstr(string, substring);
	}
	*user = calloc(count + 1, sizeof(struct wifi));
}

static void get_all_devices(struct wifi **user)
{
	char *response = 0;
	send_request(0, 1,
		ROUTER_URL "lan/HostInfo", 0, &response);
	allocate_array(user, response);
	extract_str(response, "<MacAddress>([^<]*)", (*user)->mac_address);
	extract_str(response, "<ActualName>([^<]*)", (*user)->hostname);
	extract_str(response, "<IpAddress>([^<;]*)", (*user)->ipv4_address);
	extract_int(response, "<AssociatedTime>([^<]*)",
		&(*user)->associated_time);
	extract_int(response, "<LeaseTime>([^<]*)", &(*user)->lease_time);
	free(response);
}


static void search_mac_address(char *mac_address, struct wifi *user)
{
	for (; *user->mac_address; ++user) {
		if (!strcmp(user->mac_address, mac_address)) {
			break;
		}
	}
	user->blacklisted = 1;
}

static void extract_blacklisted_devices(char *response,
	struct wifi *user)
{
	struct re_pattern_buffer preg;
	regcomp(&preg,
		"<WifiMacFilterMac[0-9]+>([^<]+)",
		REG_EXTENDED);
	regmatch_t matches[2];

	for ((void)regexec(&preg, response, 2, matches, 0);
			!regexec(&preg, response,
				2, matches, 0);) {
		response[matches[1].rm_eo] = 0;
		search_mac_address(response + matches[1].rm_so, user);
		response += matches->rm_eo + 1;
	}
	regfree(&preg);
}

static void get_blacklisted_devices(struct wifi *user)
{
	char *response = 0;
	send_request(0, 1, ROUTER_URL "wlan/multi-macfilter-settings-ex", 0,
		&response);
	extract_blacklisted_devices(response, user);
	free(response);
}

static void append_str(char **first_str, char *mac_address,
	unsigned char *blacklist_id)
{
	unsigned long first_str_size = strlen(*first_str);
	unsigned long new_size = first_str_size + MAC_ADDRESS_MAX_LENGTH +
		(unsigned long) snprintf(0, 0,
			"<WifiMacFilterMac%d></WifiMacFilterMac%d>",
			*blacklist_id, *blacklist_id);

	*first_str = realloc(*first_str, new_size + 1);
	snprintf(*first_str + first_str_size, new_size + 1,
		"<WifiMacFilterMac%d>%s</WifiMacFilterMac%d>", *blacklist_id,
		mac_address, *blacklist_id);
	++*blacklist_id;
}

static void blacklist_switch(struct wifi *user, short id)
{
	if (!id) {
		return;
	}
	char *unlock_data = calloc(1, 1);
	unsigned char blacklist_id = 0;
	for (short i = 0; *user[i].mac_address; ++i) {
		if ((-i - 1 == id || user[i].blacklisted) && i + 1 != id) {
			append_str(&unlock_data, user[i].mac_address,
				&blacklist_id);
		}
	}
	char *final_data = 0;
	asprintf(&final_data, "<request><Ssids><Ssid>"
	"<Index>0</Index>%s<WifiMacFilterStatus>2</WifiMacFilterStatus>"
	"</Ssid></Ssids></request>", unlock_data);
	free(unlock_data);

	char *response = 0;
	send_request(0, 1, ROUTER_URL "wlan/multi-macfilter-settings",
		final_data, &response);
	free(final_data);
	printf("%s\n", response);
	free(response);
}

#define DATE_LENGTH 13
static void seconds2time(long timer, char *buffer)
{
	struct tm *t = gmtime_r(&timer, &(struct tm){0});
	--t->tm_yday;
	strftime(buffer, DATE_LENGTH, "%j:%H:%M:%S", t);
}

int main()
{
	/* Clear the screen and move cursor to 0,0. */
	/* printf("\x1B[2J\x1B[H"); */

	struct wifi *user = 0;
	get_all_devices(&user);

	get_blacklisted_devices(user);

	char date[DATE_LENGTH];
	for (unsigned char i = 0; *user[i].mac_address; ++i) {

		/* Hide inactive devices. */
		if (!user[i].associated_time && !user[i].blacklisted) {
			continue;
		}
		seconds2time(user[i].associated_time ?
			user[i].associated_time :
			user[i].lease_time, date);

		printf("%.2u | %s | %s | %s%s\x1B[0m\n",
			i + 1,
			date,
			user[i].ipv4_address,
			user[i].associated_time ? "\x1B[32m" :
				user[i].blacklisted ? "\x1B[33m" : "\x1B[31m",
			user[i].hostname);
	}

	printf("Your choice: ");
	char input[4];
	scanf("%s", input);

	short choice = 0;
	switch (*input) {
		case '-' : case '0' : case '1' : case '2' : case '3' :
		case '4' : case '5' : case '6' : case '7' : case '8' :
		case '9' :
			choice = (short)atoi(input);
			blacklist_switch(user, choice);
			break;
		case 'r' :
			printf("Rebooting ...\n");
			send_request(0, 1, ROUTER_URL "device/control",
				"<request><Control>1</Control></request>", 0);
		default :
			free(user);
			return 0;
	}
	free(user);
	main();
}
