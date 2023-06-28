//bin/true; exec ccompile "$0" -D_GNU_SOURCE -- "$@"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* strftime, localtime_r, nanosleep */
#include <time.h>

/* Reminder not to use the following functions:
 * sprintf, vsprintf, scanf, wscanf, fscanf, fwscanf, vscanf, vwscanf, vfscanf,
 * vfwscanf, sscanf, swscanf, vsscanf, vswscanf, swprintf, snprintf, vswprintf,
 * vsnprintf, memcpy, memmove, strncpy, strncat, memset, strcpy, strcat,
 * gmtime, localtime, ctime, ctime_r, asctime, asctime_r
 */

#define B_PER_KIB 1024
static void concatenate(const char *str1, const char *str2, char sep, char *str)
{
	unsigned char i = 0;
	for (; str1[i]; ++i) {
		str[i] = str1[i];
	}
	if (sep) {
		str[i++] = sep;
	}
	for (unsigned char j = 0; str2[j]; ++i, ++j) {
		str[i] = str2[j];
	}
	for (; str[i]; ++i) {
		str[i] = 0;
	}
}

#define GNU_LINUX_PARENT_DIRECTORY "/home/pactrag/Programme/aggregator/"
#define ANDROID_TERMUX_HOME "/data/data/com.termux/files/home/"
#define ANDROID_PARENT_DIRECTORY ANDROID_TERMUX_HOME "misc/aggregator/"
static FILE *access_file(char* filename, char* type)
{
	char path[64] = {0};
	concatenate(GNU_LINUX_PARENT_DIRECTORY, filename, 0, path);
	FILE* file = fopen(path, type);
	if (!file) {
		concatenate(ANDROID_PARENT_DIRECTORY, filename, 0, path);
		file = fopen(path, type);
	}
	return file;
}

static void read_until(char *string, FILE *file, char character) {
	for (int c = fgetc(file), i = -1; c != character; c = fgetc(file)) {
		string[++i] = (char) c;
	}
}

static void last_executed_process()
{
	char path[18] = {0};
	path[0] = '/'; path[1] = 'p'; path[2] = 'r'; path[3] = 'o';
	path[4] = 'c'; path[5] = '/';
	FILE *file = fopen("/proc/loadavg", "re");
	unsigned char flag = 0;
	for (int c = fgetc(file), space = 0; c != '\n'; c = fgetc(file)) {
		if ((c == ' ' && ++space == 4)) {
			flag = 5;
		} else if (flag) {
			path[++flag] = (char) c;
		}
	}
	fclose(file);
	path[++flag] = '/'; path[++flag] = 'c'; path[++flag] = 'o';
	path[++flag] = 'm'; path[++flag] = 'm';

	file = fopen(path, "re");
	if (!file) {
		return;
	}
	char last_process[64] = {0};
	read_until(last_process, file, '\n');
	fclose(file);

	file = access_file("last_process", "we");
	fprintf(file, "%s\n", last_process);
	fclose(file);
}

static char is_a_digit(char character)
{
	return (character >= '0' && character <= '9') ? 1 : 0;
}

#include <dirent.h>
static void processes_number()
{
	struct dirent **namelist = 0;
	int n = scandir("/proc", &namelist, 0, 0);

	unsigned short processes_number = 0;
	while (n--) {
		processes_number += (is_a_digit(*namelist[n]->d_name));
		free(namelist[n]);
	}
	free(namelist);

	FILE *file = access_file("processes_number", "we");
	fprintf(file, "%d\n", processes_number);
	fclose(file);
}

static int my_sleep(long seconds)
{
	return nanosleep(&(struct timespec){.tv_sec = seconds, .tv_nsec = 0},
		0);
}

#define NETSPEED_FILE_READ 6
#define MEMORY_FILE_READ 6
#define CPU_FILE_READ 3
#define BATTERY_FILE_READ 4
#define LAST_PROCESS_FILE_READ 64
#define PROCESSES_NUMBER_FILE_READ 4
static void print_summary(void)
{
	char netspeed[NETSPEED_FILE_READ] = {0};
	FILE *file = access_file("netspeed", "re");
	read_until(netspeed, file, ' ');
	fclose(file);

	/* Memory. */
	char memory[MEMORY_FILE_READ] = {0};
	file = access_file("memory", "re");
	read_until(memory, file, '\n');
	fclose(file);

	/* CPU. */
	char cpu[CPU_FILE_READ] = {0};
	file = access_file("cpu", "re");
	read_until(cpu, file, '\n');
	fclose(file);

	/* Battery. */
	char battery[BATTERY_FILE_READ] = {0};
	file = access_file("battery", "re");
	fgets(battery, BATTERY_FILE_READ, file);
	fclose(file);

	char last_process[LAST_PROCESS_FILE_READ] = {0};
	file = access_file("last_process", "re");
	read_until(last_process, file, '\n');
	fclose(file);

	char processes_number[PROCESSES_NUMBER_FILE_READ] = {0};
	file = access_file("processes_number", "re");
	read_until(processes_number, file, '\n');
	fclose(file);

	printf("%s | %s | NS: %sk | MEM: %sMi | CPU: %s%% | BAT: %s%%\n",
			last_process, processes_number, netspeed, memory,
			cpu, battery);

	/*
	* Nothing will be printed in the status bar of Tmux if the output
	* buffer has not been flushed?
	*/
	fflush(stdout);
}

#define DECIMAL_BASE 10
static unsigned convert_to_int(const char *string, unsigned start,
		unsigned end)
{
	unsigned integer = 0;
	for (unsigned i = end + 1, num = 1;
			i > start; --i, num *= DECIMAL_BASE) {
		if (string[i - 1] == ' ') {
			continue;
		}
		integer +=
			(unsigned) (string[i - 1] - '0') * num;
	}
	return integer;
}

#define MEMINFO 29
#define MEMINFO_NUMBER_START 16
#define MEMINFO_NUMBER_END 23
static void ram_usage_in_Mi(void)
{
	char mem_total_string[MEMINFO];
	char mem_free_string[MEMINFO];
	FILE *meminfo = fopen("/proc/meminfo", "re");
	fgets(mem_total_string, MEMINFO, meminfo);
	fgets(mem_free_string, MEMINFO, meminfo);
	fgets(mem_free_string, MEMINFO, meminfo);
	fclose(meminfo);

	unsigned mem_total = convert_to_int(mem_total_string,
			MEMINFO_NUMBER_START, MEMINFO_NUMBER_END);
	unsigned mem_free = convert_to_int(mem_free_string,
			MEMINFO_NUMBER_START, MEMINFO_NUMBER_END);

	unsigned used_memory = (mem_total - mem_free) / B_PER_KIB;
	/* printf("%dMi\n", used_memory); */

	FILE *memory = access_file("memory", "we");
	fprintf(memory, "%d\n", used_memory);
	fclose(memory);
}

#define GNU_LINUX_UEVENT "/sys/class/power_supply/BAT0/uevent"
#define ANDROID_UEVENT "/sys/class/power_supply/battery/uevent"
#define UEVENT_LENGTH 26
#define BATTERY_STATUS_CHARS 5
#define FIRST_IDENTIFIER_POSITION 13
#define SECOND_IDENTIFIER_POSITION 19
#define THIRD_IDENTIFIER_POSITION 20
#define FOURTH_IDENTIFIER_POSITION 21
#define CAPACITY_INITIAL_POSITION 22
static void battery_status(void)
{
	char line[UEVENT_LENGTH];
	FILE *uevent = fopen(GNU_LINUX_UEVENT, "re");
	if (!uevent) {
		uevent = fopen(ANDROID_UEVENT, "re");
	}

	char battery_status[BATTERY_STATUS_CHARS];
	while(fgets(line, UEVENT_LENGTH, uevent)) {
		if (line[FIRST_IDENTIFIER_POSITION] == 'S' &&
				line[SECOND_IDENTIFIER_POSITION] == '=') {
			if (line[THIRD_IDENTIFIER_POSITION] == 'D') {
				battery_status[0] = '-';
			} else {
				battery_status[0] = '+';
			}
		} else if (line[FIRST_IDENTIFIER_POSITION] == 'C' &&
				line[FOURTH_IDENTIFIER_POSITION] == '=') {
			battery_status[1] = line[CAPACITY_INITIAL_POSITION];
			battery_status[2] = line[CAPACITY_INITIAL_POSITION + 1];
			if (line[CAPACITY_INITIAL_POSITION + 2] == '\n') {
				battery_status[3] = 0;
			} else {
				battery_status[3] =
					line[CAPACITY_INITIAL_POSITION + 2];
			}
			break;
		}
	}
	fclose(uevent);

	FILE *battery = access_file("battery", "we");
	fprintf(battery, "%s\n", battery_status);
	fclose(battery);
}

#define DECIMAL_BASE 10
static void update_if_bigger(char *str, unsigned int *num)
{
	unsigned int temp = (unsigned int)strtol(str, 0, DECIMAL_BASE);
	if (temp > *num) {
		*num = temp;
	}
}

#define NET_DEV 150
#define RX_BYTES_COLUMN 1
#define TX_BYTES_COLUMN 9
static void get_bytes(char *file_path, unsigned *rx_bytes_session,
	unsigned *tx_bytes_session)
{
	char line[NET_DEV];
	FILE *dev = fopen(file_path, "re");
	fgets(line, NET_DEV, dev);
	fgets(line, NET_DEV, dev);

	for (unsigned char i = 0; fgets(line, NET_DEV, dev); i = 0) {
		for (char *p = line, *val = 0; p; ++i) {
			do {
				val = strsep(&p, " ");
			} while (!*val && val);
			if (i == RX_BYTES_COLUMN) {
				update_if_bigger(val, rx_bytes_session);
			} else if (i == TX_BYTES_COLUMN) {
				update_if_bigger(val, tx_bytes_session);
			}
		}
	}
	fclose(dev);
}


#define DATE 16
static void netlog(void)
{
	unsigned rx_usage_bytes = 0;
	unsigned tx_usage_bytes = 0;

	get_bytes("/proc/net/dev", &rx_usage_bytes, &tx_usage_bytes);

	unsigned rx_usage = rx_usage_bytes / (B_PER_KIB * B_PER_KIB);
	unsigned tx_usage = tx_usage_bytes / (B_PER_KIB * B_PER_KIB);

	char buff[DATE];
	strftime(buff, sizeof buff, "%Y%m%dT%H%M%S",
	localtime_r((time_t[]){time(0)}, &(struct tm){0}));

	FILE *file = access_file("netlog", "ae");
	fprintf(file, "%s %d %d\n", buff, rx_usage, tx_usage);
	fclose(file);
}

static void netspeed(unsigned *prev_rx_bytes, unsigned *prev_tx_bytes)
{
	unsigned tx_bytes = 0;
	unsigned rx_bytes = 0;

	get_bytes("/proc/net/dev", &rx_bytes, &tx_bytes);

	unsigned rx_netspeed = rx_bytes - *prev_rx_bytes;
	unsigned tx_netspeed = tx_bytes - *prev_tx_bytes;
	*prev_rx_bytes = rx_bytes;
	*prev_tx_bytes = tx_bytes;

	FILE *netspeed = access_file("netspeed", "we");
	fprintf(netspeed, "%d %d %d\n", rx_netspeed / B_PER_KIB,
			tx_netspeed / B_PER_KIB,
			(rx_netspeed + tx_netspeed) / 2 * B_PER_KIB);
	fclose(netspeed);
}

#define PERCENTAGE 100
#define STAT 64
static void cpu_usage(unsigned *prev_idle, unsigned *prev_total)
{
	char cpu_utilization[STAT];
	FILE *stat = fopen("/proc/stat", "re");
	fgets(cpu_utilization, STAT, stat);
	fclose(stat);

	unsigned total = 0;
	unsigned idle = 0;
	unsigned num = 0;
	char position = 0;
	for (unsigned char i = 0, j = 0, k = 0; cpu_utilization[i]; ++i) {

		if (cpu_utilization[i - 1] == ' ' &&
			is_a_digit(cpu_utilization[i])) {
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
				(float)(total - *prev_total)) * PERCENTAGE);
	*prev_total = total;
	*prev_idle = idle;

	FILE *cpu = access_file("cpu", "we");
	fprintf(cpu, "%d\n", cpu_usage);
	fclose(cpu);
}

int main(int argc, char *argv[])
{
	if (argc == 2) {
		if (argv[1][0] == 'p') {
			print_summary();
		} else if (argv[1][0] == 'n') {
			netlog();
		}
		return 0;
	}
	unsigned previous_rx_bytes = 0;
	unsigned previous_tx_bytes = 0;

	unsigned previous_total = 0;
	unsigned previous_idle = 0;
	while (1) {
		ram_usage_in_Mi();
		battery_status();
		last_executed_process();
		processes_number();

		cpu_usage(&previous_idle, &previous_total);
		netspeed(&previous_rx_bytes, &previous_tx_bytes);

		my_sleep(1);
	}
}
