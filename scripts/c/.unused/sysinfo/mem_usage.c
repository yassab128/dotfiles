#include "mem_usage.h"
#include <stdio.h>

void *get_mem_usage_in_MB(void *arg)
{
	FILE *fp = fopen(MEMINFO, "re");

	if (!fp) {
		fputs("Cannot open " MEMINFO " for reading\n", stderr);
		return 0;
	}
	unsigned long mem_total;
	unsigned long mem_free;
	unsigned long buffers;
	unsigned long cached;

#pragma unroll 2
	for (char *parameter_name;
	     fscanf(fp, "%m[^:]:", &parameter_name) == 1;) {
		if (!strcmp(parameter_name, "MemTotal")) {
			fscanf(fp, "%*[^0-9]%lu", &mem_total);
		} else if (!strcmp(parameter_name, "MemFree")) {
			fscanf(fp, "%*[^0-9]%lu", &mem_free);
		} else if (!strcmp(parameter_name, "Buffers")) {
			fscanf(fp, "%*[^0-9]%lu", &buffers);
		} else if (!strcmp(parameter_name, "Cached")) {
			fscanf(fp, "%*[^0-9]%lu", &cached);
		}
		free(parameter_name);
		fscanf(fp, "%*[^\n]\n");
	}
	fclose(fp);

	unsigned int mem_used =
	    CONVERT_FROM_KB_TO_MB(mem_total - mem_free - buffers - cached);
	return (void *)(long)mem_used;
}
