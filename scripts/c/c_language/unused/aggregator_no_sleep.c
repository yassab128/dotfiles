//bin/true; exec ccompile "$0" -- "$@"

#include <stdio.h>
/* strftime, localtime_r, nanosleep */
#include <time.h>

#define B_TO_K 1024

static void concatenate(char *str1, char *str2, char sep, char *str)
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

static unsigned fast_atoi(char *str)
{
    unsigned val = 0;
    while(is_a_digit(*str)) {
	val = val * 10 + ((unsigned)*str++ - '0');
    }
    return val;
}


#include <dirent.h>
static void processes_number()
{
	DIR *proc = opendir("/proc");
	/* Skip '.' */
	readdir(proc);
	/* Skip '..' */
	readdir(proc);
	unsigned short processes_number = 0;
	for (struct dirent *file = readdir(proc); file; file = readdir(proc)) {
		processes_number += (is_a_digit(file->d_name[0]));
	}
	closedir(proc);

	FILE *file = access_file("processes_number", "we");
	fprintf(file, "%d\n", processes_number);
	fclose(file);
}

static int my_sleep(long seconds)
{
	struct timespec req;
	req.tv_sec = seconds;
	req.tv_nsec = 0;
	return nanosleep(&req , 0);
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

	unsigned used_memory = (mem_total - mem_free) / B_TO_K;
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

#define DATE 16
static struct tm* my_localtime(time_t timep)
{
	struct tm result;
	return localtime_r(&timep, &result);
}

#define DEV_TX_BYTES_COLUMN 9
#define NET_DEV 150
static void get_x_bytes(char *file_path, unsigned *rx_bytes_session,
	unsigned *tx_bytes_session)
{
	char line[NET_DEV];
	FILE *dev = fopen(file_path, "re");
	fgets(line, NET_DEV, dev);
	fgets(line, NET_DEV, dev);

	while(fgets(line, NET_DEV, dev)) {
		char position = 0;
		unsigned num = 0;
		for (unsigned short i = 0, j = 0, k = 0; line[i]; ++i) {

			if (line[i - 1] == ' ' && is_a_digit(line[i]) &&
					(++position == 1 ||
					 position == DEV_TX_BYTES_COLUMN)) {
				j = i;
			}
			else if (j && line[i] == ' ') {
				k = i - 1;
				num = convert_to_int(line, j, k);

				if (position == 1) {
					*rx_bytes_session += num;
				} else {
					*tx_bytes_session += num;
				}
				j = 0;
			}

		}
	}
	fclose(dev);
}

static void netlog(void)
{
	unsigned rx_usage_bytes = 0;
	unsigned tx_usage_bytes = 0;

	get_x_bytes("/proc/net/dev", &rx_usage_bytes, &tx_usage_bytes);

	unsigned rx_usage = rx_usage_bytes / (B_TO_K * B_TO_K);
	unsigned tx_usage = tx_usage_bytes / (B_TO_K * B_TO_K);
	unsigned rx_tx_usage = rx_usage + tx_usage;

	char buff[DATE];
	strftime(buff, DATE, "%Y%m%dT%H%M%S", my_localtime(time(0)));

	FILE *file = access_file("netlog", "ae");
	fprintf(file, "%s %d %d %d\n", buff, rx_usage, tx_usage, rx_tx_usage);
	fclose(file);
}

static void netspeed()
{
	char prev_rx_bytes_str[16] = {0};
	char prev_tx_bytes_str[16] = {0};

	unsigned prev_rx_bytes = 0;
	unsigned prev_tx_bytes = 0;
	FILE *temp_file = fopen("/tmp/net_temp", "re");
	if (temp_file) {
		fgets(prev_rx_bytes_str, 16, temp_file);
		fgets(prev_tx_bytes_str, 16, temp_file);
		fclose(temp_file);
		prev_rx_bytes = fast_atoi(prev_rx_bytes_str);
		prev_tx_bytes = fast_atoi(prev_tx_bytes_str);
	}

	unsigned tx_bytes = 0;
	unsigned rx_bytes = 0;
	get_x_bytes("/proc/net/dev", &rx_bytes, &tx_bytes);

	unsigned rx_netspeed = rx_bytes - prev_rx_bytes;
	unsigned tx_netspeed = tx_bytes - prev_tx_bytes;

	temp_file = fopen("/tmp/net_temp", "we");
	fprintf(temp_file, "%d\n", rx_bytes);
	fprintf(temp_file, "%d\n", tx_bytes);
	fclose(temp_file);

	FILE *netspeed = access_file("netspeed", "we");
	fprintf(netspeed, "%d %d %d\n", rx_netspeed / B_TO_K,
			tx_netspeed / B_TO_K,
			(rx_netspeed + tx_netspeed) / 2 * B_TO_K);
	fclose(netspeed);
}

static unsigned my_strlen(char *str)
{
	unsigned i = 0;
	for (; str[i]; ++i) {
		if (str[i] == '\n') {
			break;
		}
	}
	return i;
}


#define PERCENTAGE 100
#define STAT 64
static void cpu_usage()
{
	char cpu_utilization[STAT];
	FILE *stat = fopen("/proc/stat", "re");
	fgets(cpu_utilization, STAT, stat);
	fclose(stat);

	unsigned stat_len = my_strlen(cpu_utilization);

	unsigned total = 0;
	unsigned idle = 0;
	for (unsigned i = stat_len - 1, l = 0, num = 0, ten = 1; i > 3; --i) {

		l += (cpu_utilization[i] == ' ') ;
		if (is_a_digit(cpu_utilization[i])) {
			ten *= 10;
			num += (unsigned) (cpu_utilization[i] - '0') * ten;	
		} else if (cpu_utilization[i] == ' ') {
			total += num;
			if (l == 7) {
				idle = num;
			}
			ten = 1;
			num = 0;
		}
	}

	char prev_total_str[16] = {0};
	char prev_idle_str[16] = {0};

	unsigned prev_total = 0;
	unsigned prev_idle = 0;
	FILE *temp_file = fopen("/tmp/cpu_temp", "re");
	if (temp_file) {
		fgets(prev_total_str, 16, temp_file);
		fgets(prev_idle_str, 16, temp_file);
		fclose(temp_file);
		prev_total = fast_atoi(prev_total_str);
		prev_idle = fast_atoi(prev_idle_str);
	}

	char cpu_usage = (char) ((1 - (float)(idle - prev_idle) /
				(float)(total - prev_total)) * PERCENTAGE);

	temp_file = fopen("/tmp/cpu_temp", "we");
	fprintf(temp_file, "%d\n", total);
	fprintf(temp_file, "%d\n", idle);
	fclose(temp_file);

	FILE *cpu = access_file("cpu", "we");
	fprintf(cpu, "%d\n", cpu_usage);
	fclose(cpu);
}

static char copy_file(char *path_to_read_file, char *path_to_write_file)
{
	FILE *stream_for_read = fopen(path_to_read_file, "re");
	FILE *stream_for_write = fopen(path_to_write_file, "we");
	if (!(stream_for_read && stream_for_write)) {
		printf("Something is wrong!\n");
		return 1;
	}
	for (int chr = fgetc(stream_for_read); chr != -1;
		chr = fgetc(stream_for_read)) {
		fputc(chr, stream_for_write);
	}
	fclose(stream_for_write);
	fclose(stream_for_read);
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc == 2) {
		if (argv[1][0] == 'p') {
			print_summary();
			return 0;
		} else if (argv[1][0] == 'n') {
			netlog();
			return 0;
		}
	}
	ram_usage_in_Mi();
	battery_status();
	last_executed_process();
	processes_number();

	cpu_usage();
	netspeed();
}
