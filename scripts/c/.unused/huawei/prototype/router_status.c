#if 0
true; ccompile "$0" -D_GNU_SOURCE -lcrypto -lcurl -- "$@"
#endif
#include "huawei.h"

static unsigned char get_current_wifi_users()
{
	char *response = 0;
	send_request(0, 0, ROUTER_URL "monitoring/status", 0, &response);
	char *current_wifi_users_str = 0;
	extract_substring("<CurrentWifiUser>([^<]+)", response,
			  &current_wifi_users_str);
	free(response);
	if (!current_wifi_users_str) {
		return 1;
	}
	unsigned char current_wifi_users =
	    (unsigned char)strtoul(current_wifi_users_str, 0, DECIMAL_BASE);
	free(current_wifi_users_str);
	return current_wifi_users;
}

static void get_traffic_statistics(double *x_bytes)
{
	char *response = 0;
	send_request(0, 0, ROUTER_URL "monitoring/traffic-statistics", 0,
		     &response);

	char *traffic = 0;
	extract_substring("<CurrentDownloadRate>([^<]+)", response, &traffic);
	sscanf(traffic, "%lf", &x_bytes[0]);

	extract_substring("<CurrentUploadRate>([^<]+)", response, &traffic);
	sscanf(traffic, "%lf", &x_bytes[1]);
	free(traffic);

	free(response);
}

int main()
{
	unsigned char current_wifi_users = get_current_wifi_users();

	double x_bytes[2];
	get_traffic_statistics(x_bytes);

	{
		char unit = 0;
		printf("%.1f%c | %d | %.1f%c\n", HUMAN_SIZE(x_bytes[0], unit),
		       current_wifi_users, HUMAN_SIZE(x_bytes[1], unit));
	}
}
