#include <dirent.h>
#include <stdio.h>		 /* fputs, printf, puts, snprintf */
#include <stdlib.h>		 /* atexit, getenv, malloc */
#include <string.h>		 /* memcpy */
#include <sys/stat.h>	 // for checking the modification time of files
#include <sys/sysinfo.h> /* I have been reinventing the whell */
#include <sys/types.h>
#include <sys/wait.h> // needed by waitpid
#include <time.h>	  // for date
#include <unistd.h>	  /* sleep, unlink */

/* DO NOT REMOVE `#include <ctype.h>`, it is needed by isspace */

int checkIfFileExists(const char *);
int checkIfFileExists(const char *filename) {
	FILE *file;
	if ((file = fopen(filename, "r"))) {
		fclose(file);
		return 1;
	}

	return 0;
}

void concatenate(char *, char *);
void concatenate(char *str1, char *str2) {
	unsigned long i = strlen(str1);
	int j = 0;

	while (str2[j] != '\0') {
		str1[i] = str2[j];
		i++;
		j++;
	}

	str1[i] = '\0'; // declaring the end of the string
}

int main() {
	FILE *dapacity;

	char up[64];
	dapacity = fopen("/sys/class/net/wlan0/statistics/tx_bytes", "r");
	fgets(up, 64, dapacity);
	fclose(dapacity);
	long long dummy_up = strtol(up, NULL, 10);

	char down[64];
	FILE *capacity;
	capacity = fopen("/sys/class/net/wlan0/statistics/rx_bytes", "r");
	fgets(down, 64, capacity);
	fclose(capacity);
	long long dummy_down = strtol(down, NULL, 10);

	/*[>for CPU. this snippet must stay on top, out of the while loop<]*/
	/*long double a[4], b[4], loadavg;*/
	/*FILE *fp;*/
	/*fp = fopen("/proc/stat", "r");*/
	/*fscanf(fp, "%*s %Lf %Lf %Lf %Lf", &a[0], &a[1], &a[2], &a[3]);*/
	/*fclose(fp);*/

	char str[100];
	long lastSum = 0;
	long lastIdle = 0;
	long idle = 0;
	/*int i = 0, j = 0;*/

	while (1) {
		int i = 0;
		/*date*/
		char date[100];
		time_t now = time(NULL);
		struct tm *t = localtime(&now);
		strftime(date, sizeof(date) - 1, "%Y-%m-%d", t);

		/*concatenate the two strings*/
		char datePath[100] = "/home/pactrag/Programme/sysinfo/nvstat/";
		concatenate(datePath, date);

		/*get new value from rx_bytes*/
		capacity = fopen("/sys/class/net/wlan0/statistics/rx_bytes", "r");
		fgets(down, 64, capacity);
		fclose(capacity);
		/*convert extracted string to number*/
		long long new_rx = strtol(down, NULL, 10);

		/*get new value from tx_bytes*/
		dapacity = fopen("/sys/class/net/wlan0/statistics/tx_bytes", "r");
		fgets(up, 64, dapacity);
		fclose(dapacity);
		/*convert extracted string to number*/
		long long new_tx = strtol(up, NULL, 10);

		FILE *date_file;
		if (!checkIfFileExists(datePath)) {
			date_file = fopen(datePath, "w");
			if (date_file) {
				fprintf(date_file, "0\n0\n0");
				fclose(date_file);
			}
		}

		/*get rx from date*/
		char rx_total[64];
		date_file = fopen(datePath, "r"); /*use ab+, otherwise segfaults*/
		for (i = 0; i < 2; i++)
			fgets(rx_total, 64, date_file);
		fclose(date_file);
		/*convert rx_total string to number*/
		long long old_rx = strtol(rx_total, NULL, 10);

		/*get tx from date*/
		char tx_total[64];
		date_file = fopen(datePath, "r");
		for (i = 0; i < 1; i++)
			fgets(tx_total, 64, date_file);
		fclose(date_file);
		/*convert tx_total string to number*/
		long long old_tx = strtol(tx_total, NULL, 10);

		/*calculate rx netspeed*/
		long long netspeed_down = (long long)(new_rx - dummy_down);
		/*get total rx usage*/
		long long output_down = (long long)(old_rx + netspeed_down);
		dummy_down = new_rx;
		/*calculate tx netspeed*/
		long long netspeed_up = (long long)(new_tx - dummy_up);
		/*get total tx usage*/
		long long output_up = (long long)(old_tx + netspeed_up);
		dummy_up = new_tx;

		/*output to sessionUsage*/
		FILE *sessionUsage =
			fopen("/home/pactrag/Programme/sysinfo/misc/sessionUsage", "w");
		if (sessionUsage) {
			fprintf(sessionUsage, "%lld\n%lld\n%lld", new_rx / 1048576,
					new_tx / 1048576, new_rx / 1048576 + new_tx / 1048576);
			fclose(sessionUsage);
		}

		/*output rx, tx and `rx + tx`'s netspeed to netspeed*/
		FILE *f1 = fopen("/home/pactrag/Programme/sysinfo/misc/netspeed", "w");
		if (f1) {
			fprintf(f1, "%lld\n%lld\n%lld", netspeed_up / 1024,
					netspeed_down / 1024,
					netspeed_up / 1024 + netspeed_down / 1024);
			fclose(f1);
		}

		/*output total usage*/
		date_file = fopen(datePath, "w");
		if (date_file) {
			fprintf(date_file, "%lld\n%lld\n%lld", output_up, output_down,
					output_up + output_down);
			fclose(date_file);
		}

		///////////////////*BATTERY*/
		char percentage[3];
		FILE *akku;
		akku = fopen("/sys/class/power_supply/BAT0/capacity", "r");
		fgets(percentage, 3, akku);
		fclose(akku);

		char power[3];
		FILE *online;
		online = fopen("/sys/class/power_supply/AC/online", "r");
		fgets(power, 3, online);
		char *key = strtok(power, "\n");
		fclose(online);

		FILE *f3 = fopen("/home/pactrag/Programme/sysinfo/misc/battery", "w");
		if (f3) {
			if (strcmp(key, "1") == 0) {
				fprintf(f3, "+%s%%", percentage);
			} else {
				char power_now[16];
				FILE *powerUsage;
				powerUsage =
					fopen("/sys/class/power_supply/BAT0/power_now", "r");
				fgets(power_now, 16, powerUsage);
				fclose(powerUsage);
				long long usageInWatts = strtol(power_now, NULL, 10) / 1000000;
				fprintf(f3, "-%s %%, %lld W", percentage, usageInWatts);
			}
			fclose(f3);
		}

		///////////////////*CPU*/
		/*fp = fopen("/proc/stat", "r");*/
		/*fscanf(fp, "%*s %Lf %Lf %Lf %Lf", &b[0], &b[1], &b[2], &b[3]);*/
		/*fclose(fp);*/

		/*loadavg = ((b[0] + b[1] + b[2]) - (a[0] + a[1] + a[2])) /*/
		/*((b[0] + b[1] + b[2] + b[3]) - (a[0] + a[1] + a[2] + a[3]));*/

		/*FILE *f4 = fopen("/home/pactrag/Programme/sysinfo/misc/cpu", "w");*/
		/*if (f4) {*/
		/*fprintf(f4, "%d %%", (int)(loadavg * 100));*/
		/*fclose(f4);*/
		/*}*/

		/*fp = fopen("/proc/stat", "r");*/
		/*fscanf(fp, "%*s %Lf %Lf %Lf %Lf", &a[0], &a[1], &a[2], &a[3]);*/
		/*fclose(fp);*/

		FILE *fp = fopen("/proc/stat", "r");
		fgets(str, 100, fp);
		fclose(fp);
		char *token = strtok(str, " ");
		long sum = 0;
		long idleFraction = 0;

		i = 0;
		while (token != NULL) {
			token = strtok(NULL, " ");
			if (token != NULL) {
				sum += atoi(token);
				if (i == 3) {
					idle = atoi(token);
				}

				i++;
				idleFraction = 100 - (idle - lastIdle) * 100 / (sum - lastSum);
			}
		}

		FILE *f4 = fopen("/home/pactrag/Programme/sysinfo/misc/cpu", "w");
		if (f4) {
			fprintf(f4, "%ld%%", idleFraction);
			fclose(f4);
		}

		lastIdle = idle;
		lastSum = sum;

		///////////////////*MEMORY*/

		/*GET RAM USAGE BY READING MEMINFO*/

		/*FILE *file = fopen("/proc/meminfo", "r");*/
		/*char line[256];*/
		/*char *e;*/

		/*while (fgets(line, sizeof(line), file)) {*/
		/*if (strstr(line, "MemFree") != NULL) {*/
		/*e = strchr(line, ':');*/
		/*e++;*/
		/*[>the unsigned char is necessary, do not change it<]*/
		/*while (isspace((unsigned char)*e)) {*/
		/*e++;*/
		/*}*/

		/*int ram = atoi(e);*/
		/*FILE *f5 = fopen("/home/pactrag/Programme/sysinfo/misc/ram", "w");*/
		/*if (f5) {*/
		/*fprintf(f5, "%d %%", (int)((1 - ram / 16279068.0) * 100));*/
		/*fclose(f5);*/
		/*}*/
		/*}*/
		/*}*/
		/*fclose(file);*/

		/*GET RAM USAGE BY USING SYSINFO LIBRARY*/
		struct sysinfo myinfo;
		if (sysinfo(&myinfo)) {
			return 1;
		}

		FILE *ramUsed =
			fopen("/home/pactrag/Programme/sysinfo/misc/ramUsed", "w");
		if (f4) {
			fprintf(ramUsed, "%luM\n",
					(myinfo.totalram - myinfo.freeram) / 1048576);
			fclose(ramUsed);
		}

		/* RUNNING PROCESSES{{{1 */
		/* NOTE everything inside the foldings works, just uncomment it */
		/* FILE *processesRunning = */
		/* 	fopen("/home/pactrag/Programme/sysinfo/misc/processesRunning",
		 * "w"); */
		/* if (f4) { */
		/* 	fprintf(processesRunning, "%hu\n", myinfo.procs); */
		/* 	fclose(processesRunning); */
		/* } */

		/* {{{2 UPTIME in ISO 8601 format */
		/* int d = (int)(myinfo.uptime / 86400); */
		/* long h = myinfo.uptime / 3600; */
		/* long m = (myinfo.uptime / 60) % 60; */

		/* FILE *uptime = */
		/* 	fopen("/home/pactrag/Programme/sysinfo/misc/uptime", "w"); */
		/* if (f4) { */
		/* 	fprintf(uptime, "P%dD%ldH%ldM\n", d, h, m); */
		/* 	fclose(uptime); */
		/* } */

		/* "{{{3 time */
		/*ISO 8601 date/time format*/
		/* time_t currently; */
		/* struct tm *tm; */

		/* currently = time(0); */
		/* if ((tm = localtime(&currently)) == NULL) { */
		/* 	return 1; */
		/* } */

		/* FILE *timeth = */
		/* 	fopen("/home/pactrag/Programme/sysinfo/misc/timeth", "w"); */
		/* if (f4) { */
		/* 	fprintf(timeth, "%04d-%02d-%02dT%02d:%02d\n", tm->tm_year + 1900, */
		/* 			tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min); */
		/* 	fclose(timeth); */
		/* } */

		/*fflush(stdout);*/
		sleep(1);
	}
}
// vi:fdm=marker
