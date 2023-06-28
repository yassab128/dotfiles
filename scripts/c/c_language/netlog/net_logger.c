#if 0
true; DIR=~/.local/bin exec ccompile "$0" -D_GNU_SOURCE -- "$@"
#endif

#include <preprocessor_macros.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define RECOMMENDED_ALIGNMENT 32
struct net {
	char *gap;
	char *file_path;
	unsigned long rx_bytes;
	unsigned long tx_bytes;
} __attribute__((aligned(RECOMMENDED_ALIGNMENT)));

#define DIR_PERM 0755
#define USERNAME_MAX 32
static void get_path(char *destination)
{
	char username[USERNAME_MAX];
	getlogin_r(username, USERNAME_MAX);
	sprintf(destination, "/home/%s/.local/share/net_logger", username);
	mkdir(destination, DIR_PERM);
}

#define DATE_LENGTH 18
static void print_all(unsigned long btime, char *current_date,
	struct net *netlog)
{
	FILE *fp = fopen(netlog->file_path, "r+e");
	if (!fp) {
		fopen(netlog->file_path, "we");
		/*
		 * Refresh fp again, otherwise an UB will pop up.
		 * when accessing the newly created file .
		*/
		fp = fopen(netlog->file_path, "r+e");
	}
	fseek(fp, 0, SEEK_END);

	for (;;) {
		if (fseek(fp, -2, SEEK_CUR) == -1) {
			/* This is important. */
			fseek(fp, -1, SEEK_CUR);
			break;
		}
		if (fgetc(fp) == '\n') {
			break;
		}
	}

	unsigned long btime_previous = 0;
	unsigned long rx_bytes = 0;
	unsigned long tx_bytes = 0;
	/* Not initializing this array leads to some weird stuff. */
	char date_temp[DATE_LENGTH] = {0};
	long offset = ftell(fp);

	fscanf(fp, "%17[^\t]\t%lu\t%lu\t%lu", date_temp,
		&btime_previous, &rx_bytes, &tx_bytes);

	if (!strncmp(date_temp, current_date, DATE_LENGTH - 1)) {
		ftruncate(fileno(fp), offset);
		printf("%lu | %lu\n", btime_previous, btime);
		if (btime > btime_previous) {
			printf("Jawohl\n");
			netlog->rx_bytes += rx_bytes;
			netlog->tx_bytes += rx_bytes;
			btime = btime_previous;
		}
	}
	fseek(fp, 0, SEEK_END);

	fprintf(fp, "%s\t%lu\t%lu\t%lu\t%.2f%c\t%.2f%c\n", current_date,
		btime, netlog->rx_bytes, netlog->tx_bytes,
		HUMAN_SIZE((double)netlog->rx_bytes, offset),
		HUMAN_SIZE((double)netlog->tx_bytes, offset));
	fclose(fp);
}

static unsigned long get_btime()
{
	FILE *fp = fopen("/proc/stat", "re");
	unsigned long btime = 0;
	fscanf(fp, "%*[^e]e %lu", &btime);
	fclose(fp);
	return btime;
}

int main(int interfaces_num, char **destination)
{
	FILE *fp = fopen("/proc/net/dev", "re");
	fscanf(fp, "%*[^\n]\n%*[^\n]\n");
	long offset = ftell(fp);

	interfaces_num = 0;
	while (fscanf(fp, "%*[^\n]\n") != -1) {
		++interfaces_num;
	}
	fseek(fp, offset, 0);
	if (!interfaces_num) {
		return 1;
	}

	get_path(*destination);
	unsigned long btime = get_btime();

	char *interface = 0;
	struct net *netlog =
		malloc((unsigned char)interfaces_num * RECOMMENDED_ALIGNMENT);
	for (int i = 0; i < interfaces_num; ++i) {
		fscanf(fp,
			"%m[^:]: %lu %*s %*s %*s %*s %*s %*s %*s %lu %*[^\n]\n",
			&interface,
			&netlog[i].rx_bytes,
			&netlog[i].tx_bytes
		);
			asprintf(&netlog[i].file_path, "%s/%s", *destination,
				interface);
			free(interface);
			interface = 0;
	}
	fclose(fp);

	char current_date[DATE_LENGTH];
	strftime(current_date, DATE_LENGTH, "%a, %b %d, %Y",
		localtime_r((time_t[]){time(0)}, &(struct tm){0}));

	while (interfaces_num--) {
		print_all(btime, current_date, &netlog[interfaces_num]);
		free(netlog[interfaces_num].file_path);
	}
	free(netlog);
}
