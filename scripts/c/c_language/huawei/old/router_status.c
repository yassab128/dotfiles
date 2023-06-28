#if 0
true; exec ccompile "$0" -D_GNU_SOURCE -lcrypto -lcurl -- "$@"
#endif

#define ROUTER_URL "http://192.168.1.1/"
#define PASSWORD "Schlagmann47"
#define USERNAME "admin"

#include "new_huawei.h"

static unsigned long convert_b_to_kib(const char *str)
{
	return strtoul(str, 0, DECIMAL_BASE) >> DECIMAL_BASE;
}

int main()
{
	char *response = 0;
	send_request(1, 0,
		ROUTER_URL "api/monitoring/status", 0, &response);

	char *current_wifi_users = 0;
	extract_substring("<CurrentWifiUser>([^<]+)", response,
		&current_wifi_users);
	free(response);

	send_request(1, 0,
		ROUTER_URL "api/monitoring/traffic-statistics", 0, &response);
	char *current_download_rate = 0;
	extract_substring("<CurrentDownloadRate>([^<]+)",
		response, &current_download_rate);
	char *current_upload_rate = 0;
	extract_substring("<CurrentUploadRate>([^<]+)",
		response, &current_upload_rate);
	free(response);


	printf("%lu KiBps | %s | %lu KiBps\n",
		convert_b_to_kib(current_upload_rate),
		current_wifi_users, convert_b_to_kib(current_download_rate));

	free(current_download_rate);
	free(current_upload_rate);
	free(current_wifi_users);
}
