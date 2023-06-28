//true; exec ccompile "$0" -D_GNU_SOURCE -lX11 -- "$@"

#include "set_status_bar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DECIMAL_BASE 10U

int main(int argc, char **argv)
{
	if (argc != 2) {
		return 1;
	}
	long value = strtol(argv[1], 0, DECIMAL_BASE);
	if (!value) {
		return 1;
	}

	FILE *file =
		fopen("/sys/class/backlight/intel_backlight/max_brightness",
			"re");
	char *line = 0;
	getline(&line, &(unsigned long){0}, file);
	fclose(file);
	long max_brightness = strtol(line, 0, DECIMAL_BASE);
	free(line);

	file = fopen("/sys/class/backlight/intel_backlight/brightness",
		"re");
	getline(&line, &(unsigned long){0}, file);
	fclose(file);
	long brightness = strtol(line, 0, DECIMAL_BASE);
	free(line);

	long new_brightness = brightness + value;

	if (new_brightness < 0) {
		new_brightness = 0;
	} else if (new_brightness > max_brightness) {
		new_brightness = max_brightness;
	}

	file = fopen("/sys/class/backlight/intel_backlight/brightness",
		"we");
	fprintf(file, "%ld", new_brightness);
	fclose(file);

	char *msg = 0;
	asprintf(&msg, "%ld/%ld", new_brightness, max_brightness);
	set_status(msg);
	free(msg);
}
