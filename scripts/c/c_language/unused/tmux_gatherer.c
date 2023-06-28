//bin/true; exec ccompile "$0" -- "$@"

#include <stdio.h>
#include <unistd.h> /* sleep() */

#define NETSPEED_FILE_READ 6
#define MEMORY_FILE_READ 6
#define CPU_FILE_READ 3
#define BATTERY_FILE_READ 4

#define NETSPEED_FILE "/home/pactrag/Programme/aggregator/netspeed"
#define MEMORY_FILE "/home/pactrag/Programme/aggregator/memory"
#define BATTERY_FILE "/home/pactrag/Programme/aggregator/battery"
#define CPU_FILE "/home/pactrag/Programme/aggregator/cpu"

void chomp(char *);
void chomp(char *string)
{
	for (unsigned short i = 0; string[i]; ++i) {
		if (string[i] == '\n') {
			string[i] = 0;
			break;
		}
	}
}

int main() {

	FILE *file;

	char netspeed[NETSPEED_FILE_READ];
	file = fopen(NETSPEED_FILE, "r");
	fgets(netspeed, NETSPEED_FILE_READ, file);
	fclose(file);

	for (unsigned short i = 0; netspeed[i]; ++i) {
		if (netspeed[i] == ' ') {
			netspeed[i] = 0;
			break;
		}
	}
	chomp(netspeed);

	/* Memory */
	char memory[MEMORY_FILE_READ];
	file = fopen(MEMORY_FILE, "r");
	fgets(memory, MEMORY_FILE_READ, file);
	fclose(file);

	chomp(memory);

	/* CPU */
	char cpu[CPU_FILE_READ];
	file = fopen(CPU_FILE, "r");
	fgets(cpu, CPU_FILE_READ, file);
	fclose(file);
	chomp(cpu);

	/* Battery */
	char battery[BATTERY_FILE_READ];
	file = fopen(BATTERY_FILE, "r");
	fgets(battery, BATTERY_FILE_READ, file);
	fclose(file);

	/* Bring it together */
	printf("NS: %sk | MEM: %sMi | CPU: %s%% | BAT: %s%%\r",
			netspeed, memory, cpu, battery);
}
