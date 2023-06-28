#include <stdio.h>

void *get_battery_status(void *arg)
{
	FILE *fp = fopen(UEVENT, "re");

	if (!fp) {
		fputs("Cannot open " UEVENT " for reading\n", stderr);
		return 0;
	}

	char *status;
	char capacity;
	for (char *str = 0; fscanf(fp, "%m[^=]=", &str) == 1; free(str)) {
		if (!strcmp(str, "POWER_SUPPLY_STATUS")) {
			fscanf(fp, "%ms", &status);
		} else if (!strcmp(str, "POWER_SUPPLY_CAPACITY")) {
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
	return (void *)(long)capacity;
}
