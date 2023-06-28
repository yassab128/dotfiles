#if 0
true; exec ccompile "$0" -D_GNU_SOURCE -- "$@"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define DIR_PERM 0755

#define DATE_LENGTH 18
#define DATE_LENGTH_STR "17"

static char *get_file_path(char *argv0, char *interface)
{
	char *str = 0;
	char *offset = strrchr(argv0, '/');

	char directory[] = "interfaces";
	memcpy(offset + 1, directory, sizeof(directory));
	mkdir(directory, DIR_PERM);
	asprintf(&str, "%s/%s", argv0, interface);
	return str;
}

#define KILO 1024
#define MEGA 1048576
#define GIGA 1073741824
static void human_size(unsigned long value, float *readable_size)
{
	if (value > GIGA) {
		readable_size[0] = (float)value / GIGA;
		readable_size[1] = 'G';
	} else if (value > MEGA) {
		readable_size[0] = (float)value / MEGA;
		readable_size[1] = 'M';
	} else {
		readable_size[0] = (float)value / KILO;
		readable_size[1] = 'K';
	}
}

#define DECIMCAL_BASE 10
int main(int argc, char **argv)
{
	if (argc == 1) {
		return 1;
	}
	unsigned long rx_bytes = strtoul(argv[2], 0, DECIMCAL_BASE);
	unsigned long tx_bytes = strtoul(argv[3], 0, DECIMCAL_BASE);
	if (!rx_bytes) {
		return 1;
	}

	char *file_path = get_file_path(argv[0], argv[1]);
	FILE *fp = fopen(file_path, "r+e");
	if (!fp) {
		fopen(file_path, "we");
		/* This is needed. */
		fp = fopen(file_path, "r+e");

	}
	free(file_path);

	char current_date[DATE_LENGTH];
	strftime(current_date, sizeof(current_date), "%a, %b %d, %Y",
		localtime_r((time_t[]){time(0)}, &(struct tm){0}));

	char date_temp[DATE_LENGTH];
	unsigned long previous_rx_bytes = 0;
	unsigned long previous_tx_bytes = 0;
	long offset = 0;
	for (long temp = 0; fscanf(fp,
		"%" DATE_LENGTH_STR "[^\t]\t%lu\t%lu%*[^\n]\n",
		date_temp, &previous_rx_bytes, &previous_tx_bytes) != -1;
	) {
		offset = temp;
		temp = ftell(fp);
	}

	if (!strcmp(date_temp, current_date)) {
		ftruncate(fileno(fp), offset);
		rx_bytes += previous_rx_bytes;
		tx_bytes += previous_tx_bytes;
	}
	fseek(fp, 0, SEEK_END);

	float rx_bytes_readable[2];
	human_size(rx_bytes, rx_bytes_readable);
	float tx_bytes_readable[2];
	human_size(tx_bytes, tx_bytes_readable);
	fprintf(fp, "%s\t%lu\t%lu\t%.2f%c\t%.2f%c\n", current_date, rx_bytes,
		tx_bytes, (double)rx_bytes_readable[0],
		(char)rx_bytes_readable[1], (double)tx_bytes_readable[0],
		(char)tx_bytes_readable[1]);
	fclose(fp);
}
