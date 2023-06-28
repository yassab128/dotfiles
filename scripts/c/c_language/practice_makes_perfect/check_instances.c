//true; exec ccompile "$0" -D_GNU_SOURCE -- "$@"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
static int my_sleep(long seconds)
{
	return nanosleep(&(struct timespec){.tv_sec = seconds, .tv_nsec = 0},
		0);
}

static unsigned char compare(FILE *file, char *str) {

	int c = fgetc(file);
	for (; c && *str; ++str, c = fgetc(file)) {
		if (c != *str) {
			return 0;
		}
	}
	return !*str && c == '\n' ? 1 : 0;
}

static unsigned char is_already_running(char *cmdline_path)
{
	char *comm_file = strrchr(cmdline_path, '/');
	comm_file++;

	struct dirent **namelist = 0;
	FILE *file = 0;
	unsigned char exit_code = 0;
	char *comm_file_path = 0;
	for (int n = scandir("/proc", &namelist, 0, 0);
			n--; free(namelist[n])) {

		if (!exit_code) {
			asprintf(&comm_file_path,
				"/proc/%s/comm", namelist[n]->d_name);

			file = fopen(comm_file_path, "re");
			if (file) {
				exit_code = compare(file, comm_file);
				fclose(file);
			}
			free(comm_file_path);
		}
	}
	free(namelist);
	return exit_code;
}

int main(int argc, char *argv[]) {
	my_sleep(1);
	if (is_already_running(*argv) || argc > 1) {
		printf("More than one instance is running.\n");
	} else {
		printf("Only one instance is running.\n");
	}
}
