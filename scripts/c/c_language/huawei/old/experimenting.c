#if 0
true; exec ccompile "$0" -D_GNU_SOURCE -lcrypto -lcurl -- "$@"
#endif

#define ROUTER_URL "http://192.168.1.1/"
#define PASSWORD "Schlagmann47"
#define USERNAME "admin"

#include "huawei.h"

int main()
{
	char *response = 0;
	send_request(1, 0, ROUTER_URL "monitoring/status", 0, &response);
	// send_request(1, 1, ROUTER_URL "api/device/information", 0, &response);
	printf("%s\n", response);
	free(response);
}
