#if 0
true; exec ccompile "$0" -D_GNU_SOURCE -- "$@"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define DATE_LENGTH 18
#define DATE_LENGTH_STR "17"

#define RECOMMENDED_ALIGNMENT 16
struct net {
	char *file_path;
	unsigned int rx_bytes;
	unsigned int tx_bytes;
} __attribute__((aligned(RECOMMENDED_ALIGNMENT)));

static char *get_size(char *argv0)
{
	unsigned long size = (unsigned long)(strrchr(argv0, '/') - argv0) + 1;
	char *str = malloc(size + 1);
	memcpy(str, argv0, size);
	str[size] = 0;
	return str;
}

#define KILO 1024
#define MEGA 1048576
#define GIGA 1073741824

static void human_size(unsigned int value, float *readable_size)
{
	if (value > GIGA) {
		readable_size[0] = (float)(value) / GIGA;
		readable_size[1] = 'G';
	} else if (value > MEGA) {
		readable_size[0] = (float)(value) / MEGA;
		readable_size[1] = 'M';
	} else {
		readable_size[0] = (float)(value) / KILO;
		readable_size[1] = 'K';
	}
}

static void print_all(char *current_date, struct net *netlog)
{
	FILE *fp = fopen(netlog->file_path, "r+e");
	if (!fp) {
		fopen(netlog->file_path, "we");

		/* This is needed. */
		fp = fopen(netlog->file_path, "r+e");

	}
	char date_temp[DATE_LENGTH];
	unsigned int rx_bytes = 0;
	unsigned int tx_bytes = 0;
	long offset = 0;
	for (long temp = 0; fscanf(fp,
		"%" DATE_LENGTH_STR "[^\t]\t%u\t%u%*[^\n]\n",
		date_temp, &rx_bytes, &tx_bytes) != -1;
	) {
		offset = temp;
		temp = ftell(fp);
	}
	if (!strcmp(date_temp, current_date)) {
		ftruncate(fileno(fp), offset);
		rx_bytes += netlog->rx_bytes;
		tx_bytes += netlog->tx_bytes;
	} else {
		rx_bytes = 0;
		tx_bytes = 0;
	}
	fseek(fp, 0, SEEK_END);

	float rx_bytes_readable[2];
	human_size(rx_bytes, rx_bytes_readable);
	float tx_bytes_readable[2];
	human_size(tx_bytes, tx_bytes_readable);
	fprintf(fp, "%s\t%u\t%u\t%.2f%c\t%.2f%c\n", current_date, rx_bytes,
		tx_bytes, (double)rx_bytes_readable[0],
		(char)rx_bytes_readable[1], (double)tx_bytes_readable[0],
		(char)tx_bytes_readable[1]);
	fclose(fp);
}

int main(int argc, char **argv)
{
	(void)argc;
	FILE *fp = fopen("/proc/net/dev", "re");
	unsigned char interfaces_number = 0;
	fscanf(fp, "%*[^\n]\n%*[^\n]\n");
	long offset = ftell(fp);
	while (fscanf(fp, "%*[^\n]\n") != -1) {
		++interfaces_number;
	}
	fseek(fp, offset, 0);

	if (!interfaces_number) {
		return 1;
	}
	char *current_path = get_size(*argv);

	char *interface = 0;
	struct net *netlog = malloc(interfaces_number * sizeof(struct net));
	for (unsigned char i = 0; i < interfaces_number; ++i) {
		fscanf(fp,
			"%m[^:]: %d %*s %*s %*s %*s %*s %*s %*s %d %*[^\n]\n",
			&interface,
			&netlog[i].rx_bytes,
			&netlog[i].tx_bytes
		);
			asprintf(&netlog[i].file_path, "%s%s", current_path,
				interface);
			free(interface);
			interface = 0;
	}
	fclose(fp);

	char current_date[DATE_LENGTH];
	strftime(current_date, sizeof(current_date), "%a, %b %d, %Y",
		localtime_r((time_t[]){time(0)}, &(struct tm){0}));

	for (unsigned char i = 0; i < interfaces_number; ++i) {
		print_all(current_date, &netlog[i]);
		free(netlog[i].file_path);
	}
	free(netlog);
	free(current_path);
}
