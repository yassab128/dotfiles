#include "../my_macros.h"
#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DISCHARGING 'D'

static unsigned char
get_battery_status(const char *const status_file)
{
	FILE *fp;
	unsigned char status;

	fp = fopen(status_file, "r");
	CHECK(!fp);
	switch (getc(fp)) {
	case DISCHARGING:
		status = 0;
		break;
	case EOF:
		fputs("getc() encountered an error.\n", stderr);
		_exit(EXIT_FAILURE);
	default:
		status = 1;
		break;
	}
	CHECK(fclose(fp));
	return status;
}

static unsigned char
get_battery_capacity(const char *const capacity_file)
{
	FILE *fp;
	unsigned char capacity;

	fp = fopen(capacity_file, "r");
	CHECK(!fp);
	CHECK(fscanf(fp, "%hhu", &capacity) != 1);
	CHECK(fclose(fp));
	return capacity;
}

int
main()
{
	glob_t globbuf;
	unsigned char capacity;
	unsigned char status;

	switch (
	 glob("/sys/class/power_supply/*/capacity", GLOB_NOSORT, 0, &globbuf)) {
	case GLOB_ABORTED:
		fputs("glob() encountered an error.\n", stderr);
		return EXIT_FAILURE;
	case GLOB_NOSPACE:
		fputs("glob() run out of memory.\n", stderr);
		return EXIT_FAILURE;
	case GLOB_NOMATCH:
		fputs("glob() found no match.\n", stderr);
		return EXIT_FAILURE;
	default:
		break;
	}

	if (globbuf.gl_pathc != 1) {
		return EXIT_FAILURE;
	}

	capacity = get_battery_capacity(globbuf.gl_pathv[0]);
	memcpy(strrchr(globbuf.gl_pathv[0], '/'), "/status", sizeof("/status"));
	status = get_battery_status(globbuf.gl_pathv[0]);
	globfree(&globbuf);

	printf("%c%hhu\n", status == 1 ? '+' : '-', capacity);
}
