#if 0
true; exec ccompile "$0" -D_GNU_SOURCE -lcrypto -lcurl -- "$@"
#endif

#include "huawei.h"

int main()
{
	char *shit = 0;
	send_request(0, 1, ROUTER_URL "device/information", 0, &shit);
	printf("%s\n", shit);
	free(shit);
}
