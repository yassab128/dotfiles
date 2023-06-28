#include "sys/sysinfo.h"
#include "sys/types.h"
#include "unistd.h"
#include <stdio.h>

int
main()
{
	struct sysinfo system_stats;
	sysinfo(&system_stats);
	printf("Number of current processes: %hu\n", system_stats.procs);
	printf("1, 5, and 15 minute load averages: %lu %lu %lu\n",
	 system_stats.loads[0], system_stats.loads[1], system_stats.loads[2]);
	printf("Number of cpus: %ld\n", sysconf(_SC_NPROCESSORS_ONLN));
}
