#include "running_processes.h"
#include <dirent.h>
#include <stdlib.h>

void *get_processes_number(void *arg)
{
	struct dirent **namelist;
	unsigned short processes_number = 0;
#pragma unroll 2
	for (int n = scandir("/proc", &namelist, 0, 0); n--;
	     free(namelist[n])) {
		if (*namelist[n]->d_name >= '0' &&
		    *namelist[n]->d_name <= '9') {
			++processes_number;
		}
	}
	free(namelist);
	return (void *)(long)processes_number;
}
