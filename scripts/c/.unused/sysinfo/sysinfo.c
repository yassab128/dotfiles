#include <pthread.h>
#include <stdio.h>

#include "battery_status.h"
#include "mem_usage.h"
#include "running_processes.h"

#define THREADS_NUMBER 3

int main()
{
	pthread_t thread_id[THREADS_NUMBER];
	pthread_create(thread_id, 0, get_battery_status, 0);
	pthread_create(thread_id + 1, 0, get_mem_usage_in_MB, 0);
	pthread_create(thread_id + 2, 0, get_processes_number, 0);

	char battery_status;
	unsigned int used_memory;
	unsigned short processes_number;

	pthread_join(thread_id[0], (void *)&battery_status);
	pthread_join(thread_id[1], (void *)&used_memory);
	pthread_join(thread_id[2], (void *)&processes_number);

	printf("%hhd%%\n", battery_status);
	printf("%u MB\n", used_memory);
	printf("%hu processes\n", processes_number);
}
