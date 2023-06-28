//bin/true; exec ccompile "$0" -- "$@"

#include <stdio.h>
/* sleep */
#include <unistd.h>

#define MEMINFO 29

#define BATTERY_CAPACITY 4
#define BATTERY_ONLINE 2

/* I think 150 is enough. */
#define NET_DEV 150

#define STAT 64

#define NETSPEED_FILE "/home/pactrag/Programme/aggregator/netspeed"
#define MEMORY_FILE "/home/pactrag/Programme/aggregator/memory"
#define BATTERY_FILE "/home/pactrag/Programme/aggregator/battery"
#define CPU_FILE "/home/pactrag/Programme/aggregator/cpu"

/* Used by Tmux. */
#define NETSPEED_FILE_READ 6
#define MEMORY_FILE_READ 6
#define CPU_FILE_READ 3
#define BATTERY_FILE_READ 4

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

void tmux_show(void);
void tmux_show(void) {
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
	fflush(stdout);
}

unsigned int convert_to_int(char *, unsigned, unsigned);
unsigned convert_to_int(char *string, unsigned start, unsigned end)
{
	unsigned integer = 0;
	for (unsigned i = end + 1, num = 1; i > start; --i, num *= 10) {
		if (string[i - 1] == ' ') {
			continue;
		}
		integer += (unsigned) (string[i - 1] - 48) * num;
	}
	return integer;
}

void ram_usage_in_Mi(void);
void ram_usage_in_Mi(void)
{
	FILE *meminfo;
	char mem_total_string[MEMINFO];
	char mem_free_string[MEMINFO];
	meminfo = fopen("/proc/meminfo", "r");
	fgets(mem_total_string, MEMINFO, meminfo);
	fgets(mem_free_string, MEMINFO, meminfo);
	fclose(meminfo);

	unsigned mem_total = convert_to_int(mem_total_string, 16, 23);
	unsigned mem_free = convert_to_int(mem_free_string, 16, 23);

	unsigned used_memory = (unsigned) (mem_total - mem_free) / 1024;
	/* printf("%dMi\n", used_memory); */

	FILE *memory;
	memory = fopen(MEMORY_FILE, "w");
	fprintf(memory, "%d\n", used_memory);
	fclose(memory);
}

void battery_status(void);
void battery_status(void)
{
	FILE *capacity;
	char battery_status[BATTERY_CAPACITY];
	capacity = fopen("/sys/class/power_supply/BAT0/capacity", "r");
	fgets(battery_status, BATTERY_CAPACITY, capacity);
	fclose(capacity);

	/* In case you want to check the battery percentage i.e. less than 40 %. */
	/*
	unsigned short percentage =
		(unsigned short) convert_to_int(battery_status, 0, 1);

	printf("%d\n", percentage);
	*/

	FILE *online;
	char status_string[BATTERY_ONLINE];
	online = fopen("/sys/class/power_supply/AC/online", "r");
	fgets(status_string, BATTERY_ONLINE, online);
	fclose(online);

	battery_status[2] = battery_status[1];
	battery_status[1] = battery_status[0];
	if (status_string[0] == '1') {
		battery_status[0] = '+';
	} else {
		battery_status[0] = '-';
	}

	FILE *battery;
	battery = fopen(BATTERY_FILE, "w");
	fprintf(battery, "%s\n", battery_status);
	fclose(battery);
}

void netspeed(unsigned *, unsigned *);
void netspeed(unsigned *prev_rx_bytes, unsigned *prev_tx_bytes)
{
	FILE *dev;
	char line[NET_DEV];
	dev = fopen("/proc/net/dev", "r");
	fgets(line, NET_DEV, dev);
	fgets(line, NET_DEV, dev);

	unsigned tx_bytes = 0;
	unsigned rx_bytes = 0;
	while(fgets(line, NET_DEV, dev)) {
		char position = 0;
		unsigned num;
		for (unsigned short i = 0, j = 0, k; line[i]; ++i) {

			if (line[i - 1] == ' ' &&
					47 < line[i] && line[i] < 58 &&
					(++position == 1 || position == 9)) {
				j = i;
			}
			else if (j && line[i] == ' ') {
				k = i - 1;
				num = convert_to_int(line, j, k);

				if (position == 1) {
					rx_bytes += num;
				} else {
					tx_bytes += num;
				}
				j = 0;
			}

		}
	}
	fclose(dev);

	unsigned rx_netspeed = rx_bytes - *prev_rx_bytes;
	unsigned tx_netspeed = tx_bytes - *prev_tx_bytes;
	*prev_rx_bytes = rx_bytes;
	*prev_tx_bytes = tx_bytes;


	FILE *netspeed;
	netspeed = fopen(NETSPEED_FILE, "w");
	fprintf(netspeed, "%d %d %d\n", rx_netspeed / 1024, tx_netspeed / 1024,
			(rx_netspeed + tx_netspeed) / 2048);
	fclose(netspeed);
}

void cpu_usage(unsigned *, unsigned *);
void cpu_usage(unsigned *prev_idle, unsigned *prev_total)
{
	FILE *stat;
	char cpu_utilization[STAT];
	stat = fopen("/proc/stat", "r");
	fgets(cpu_utilization, STAT, stat);
	fclose(stat);

	unsigned total = 0;
	unsigned idle = 0;
	unsigned num = 0;
	char position = 0;
	for (unsigned char i = 0, j = 0, k; cpu_utilization[i]; ++i) {

		if (cpu_utilization[i - 1] == ' ' &&
				47 < cpu_utilization[i] && cpu_utilization[i] < 58) {
			j = i;
			++position;
		}
		else if (j && cpu_utilization[i] == ' ') {
			k = i - 1;
			num = convert_to_int(cpu_utilization, j, k);

			total += num;

			if (position == 4) {
				idle = num;
			}
			j = 0;
		}

	}

	char cpu_usage = (char) ((1 - (float)(idle - *prev_idle) /
		 (float)(total - *prev_total)) * 100);
	*prev_total = total;
	*prev_idle = idle;

   FILE *cpu;
   cpu = fopen(CPU_FILE, "w");
   fprintf(cpu, "%d\n", cpu_usage);
   fclose(cpu);
}

int main(int argc, char *argv[])
{
	if (argc == 2 && argv[1][0] == '_') {
		while (1) {
			tmux_show();
			sleep(1);
		}
	}
	unsigned previous_total = 0;
	unsigned previous_idle = 0;

	unsigned previous_rx_bytes = 0;
	unsigned previous_tx_bytes = 0;
	while(1) {
		ram_usage_in_Mi();
		battery_status();

		cpu_usage(&previous_idle, &previous_total);
		netspeed(&previous_rx_bytes, &previous_tx_bytes);

		sleep(1);
	}
}
