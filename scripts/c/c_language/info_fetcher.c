#if 0
true; exec ccompile "$0" -D_GNU_SOURCE -- "$@"
#endif

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RECOMMENDED_ALIGNMENT 8
struct netspeed {
	unsigned int value;
	char unit;
	char gap[3];
} __attribute__((aligned(RECOMMENDED_ALIGNMENT)));


#define DECIMAL_BASE 10U

static void get_rate(struct netspeed *x_bytes,
	unsigned int old_rate, unsigned int new_rate)
{
	new_rate -= old_rate;
	if (new_rate >> DECIMAL_BASE < 1) {
		x_bytes->value = new_rate;
		x_bytes->unit = 'B';
	} else if (new_rate >> (DECIMAL_BASE << 1U) < 1) {
		x_bytes->value = new_rate >> DECIMAL_BASE;
		x_bytes->unit = 'K';
	} else {
		x_bytes->value = new_rate >> (DECIMAL_BASE << 1U);
		x_bytes->unit = 'M';
	}
}

static void get_netspeed(struct netspeed *rx, struct netspeed *tx,
	char identifier)
{
	FILE *fp = fopen("/proc/net/dev", "re");
	fscanf(fp, "%*[^\n]\n%*[^\n]\n");
	unsigned int new_rx_bytes = 0;
	unsigned int new_tx_bytes = 0;
	for (unsigned int down_bytes = 0, up_bytes = 0;
		fscanf(fp, "%*s %d %*s %*s %*s %*s %*s %*s %*s %d %*[^\n]\n",
		&down_bytes, &up_bytes) != -1; ) {

		new_rx_bytes += down_bytes;
		new_tx_bytes += up_bytes;
	}
	fclose(fp);

	char *temp_path = 0;
	asprintf(&temp_path, "/tmp/proc_net_dev_%c", identifier);

	fp = fopen(temp_path, "re");
	unsigned int old_rx_bytes = new_rx_bytes;
	unsigned int old_tx_bytes = new_tx_bytes;
	if (fp) {
		fscanf(fp, "%u %u", &old_rx_bytes, &old_tx_bytes);
		fclose(fp);
	}

	fp = fopen(temp_path, "we");
	free(temp_path);
	fprintf(fp, "%u %u\n", new_rx_bytes, new_tx_bytes);
	fclose(fp);

	get_rate(rx, old_rx_bytes, new_rx_bytes);
	get_rate(tx, old_tx_bytes, new_tx_bytes);
}

static unsigned char get_cpu_usage(char identifier)
{
	FILE *fp = fopen("/proc/stat", "re");
	unsigned int user = 0;
	unsigned int nice = 0;
	unsigned int system = 0;
	unsigned int idle = 0;
	unsigned int iowait = 0;
	unsigned int irq = 0;
	unsigned int softirq = 0;
	fscanf(fp, "%*s %u %u %u %u %u %u %u",
		&user, &nice, &system, &idle, &iowait, &irq, &softirq);
	unsigned int new_total = user + nice + system + idle + iowait
		+ irq + softirq;
	fclose(fp);

	char *temp_path = 0;
	asprintf(&temp_path, "/tmp/proc_stat_%c", identifier);

	fp = fopen(temp_path, "re");
	unsigned int old_total = 0;
	unsigned int old_idle = 0;
	if (fp) {
		fscanf(fp, "%u %u", &old_idle, &old_total);
		fclose(fp);
	}

	fp = fopen(temp_path, "we");
	free(temp_path);
	fprintf(fp, "%u %u\n", idle, new_total);
	fclose(fp);

	return (unsigned char)(
		(1 - (float)(idle - old_idle) /
		(float)(new_total - old_total)) * DECIMAL_BASE * DECIMAL_BASE
	);
}


static unsigned short get_processes_number()
{
	struct dirent **namelist = 0;
	int n = scandir("/proc", &namelist, 0, 0);
	unsigned short processes_number = 0;
	while (n--) {
		if (*namelist[n]->d_name >= '0' &&
			*namelist[n]->d_name <= '9') {
			++processes_number;
		}
		free(namelist[n]);
	}
	free(namelist);
	return processes_number;
}

/* static void get_last_executed_process(char **last_executed_process) */
/* { */
	/* FILE *fp = fopen("/proc/loadavg", "re"); */
	/* char *line = 0; */

	/* getline(&line, &(unsigned long){0}, fp); */
	/* fclose(fp); */

	/* struct re_pattern_buffer preg; */
	/* regcomp(&preg, "([^ ]+)\n$", REG_EXTENDED); */
	/* regmatch_t matches[2]; */
	/* (void)regexec(&preg, line, 2, matches, 0); */
	/* regfree(&preg); */

	/* char *comm_path = 0; */
	/* asprintf(&comm_path, "/proc/%.*s/comm", */
		/* matches[1].rm_eo - matches[1].rm_so, */
		/* line + matches[1].rm_so); */
	/* free(line); */

	/* fp = fopen(comm_path, "re"); */
	/* long nread = getline(last_executed_process, &(unsigned long){0}, fp); */
	/* *(*last_executed_process + nread - 1) = 0; */
	/* fclose(fp); */
	/* free(comm_path); */
/* } */

static unsigned int get_mem_usage()
{
	FILE *fp = fopen("/proc/meminfo", "re");
	unsigned int mem_total = 0;
	unsigned int mem_available = 0;
	fscanf(fp, "%*[^0-9] %u %*[^\n]\n%*[^\n]\n%*[^0-9] %u",
		&mem_total, &mem_available);
	fclose(fp);

	unsigned int mem_used = (mem_total - mem_available) >> DECIMAL_BASE;
	return mem_used;
}

static char get_battery_status()
{
	FILE *fp = fopen("/sys/class/power_supply/BAT0/uevent", "re");

	char *status = 0;
	char capacity = 0;
	for (char *str = 0; fscanf(fp, "%m[^=]=", &str) != -1; free(str)) {
		if (!strcmp(str, "POWER_SUPPLY_STATUS")) {
			fscanf(fp, "%ms", &status);
		}
		if (!strcmp(str, "POWER_SUPPLY_CAPACITY")) {
			fscanf(fp, "%hhd", &capacity);

		} else {
			fscanf(fp, "%*[^\n]\n");
		}
	}
	if (status && !strcmp(status, "Discharging")) {
		capacity *= -1;
	}
	free(status);
	fclose(fp);
	return capacity;
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		return 1;
	}

	struct netspeed rx = {0};
	struct netspeed tx = {0};
	get_netspeed(&rx, &tx, argv[1][1]);

	unsigned char cpu_usage = get_cpu_usage(argv[1][1]);

	unsigned short processes_number = get_processes_number();

	unsigned int mem_usage = get_mem_usage();

	char battery_status = get_battery_status();

	printf("%d pid | %d%c | %d%c | %dM | %d%% | %d%%\n", processes_number,
		rx.value, rx.unit, tx.value, tx.unit,
		mem_usage,
		cpu_usage,
		battery_status);
}
