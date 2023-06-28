#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define DECIMAL_BASE 10

#define AUDIO_FILE_ENVIRONMENT_VARIABLE "LANG"

static int
convert_hh_mm_to_seconds(const char *const nptr)
{
	unsigned long input_hours;
	unsigned long input_minutes;
	char *endptr;

	input_hours = strtoul(nptr, &endptr, DECIMAL_BASE);
	input_minutes = strtoul(endptr + 1, 0, DECIMAL_BASE);
	if (errno) {
		return -1;
	}
	if (input_hours > 24 || input_minutes >= 60) {
		return -1;
	}
	return (int)(input_minutes * 60 + input_hours * 3600);
}

static int
get_seconds_since_midnight(void)
{
	struct tm result;
	time_t unix_time;
	unix_time = time(0);
	localtime_r(&unix_time, &result);
	return result.tm_sec + result.tm_min * 60 + result.tm_hour * 3600;
}

static unsigned int
seconds_till_alarm(const char *const input_hh_mm)
{
	int seconds_since_midnight;
	int alarm_time_in_seconds;
	int seconds_to_wait;

	alarm_time_in_seconds = convert_hh_mm_to_seconds(input_hh_mm);
	if (alarm_time_in_seconds == -1) {
		return 0;
	}
	seconds_since_midnight = get_seconds_since_midnight();

	seconds_to_wait = alarm_time_in_seconds - seconds_since_midnight;
	if (seconds_to_wait < 0) {
		seconds_to_wait += 86400;
	}
	return (unsigned int)seconds_to_wait;
}

static void
play_the_audio_sound(const char *const audio_file_path)
{
	puts(audio_file_path);
}

static char *
get_audio_file_path(const char *const *envp)
{
	char *env_value;
	for (; *envp; ++envp) {
		env_value = strstr(*envp, AUDIO_FILE_ENVIRONMENT_VARIABLE);
		if (env_value) {
			return env_value +
			       sizeof(AUDIO_FILE_ENVIRONMENT_VARIABLE);
		}
	}
	return 0;
}

int
main(
 const int argc, const char *const *const argv, const char *const *const envp)
{
	unsigned int seconds_to_wait;
	const char *audio_file_path;
	if (argc != 2) {
		return EXIT_FAILURE;
	}

	audio_file_path = get_audio_file_path(envp);
	if (!audio_file_path) {
		return EXIT_FAILURE;
	}
	play_the_audio_sound(audio_file_path);

	seconds_to_wait = seconds_till_alarm(argv[1]);
	if (!seconds_to_wait) {
		return EXIT_FAILURE;
	}
	printf("wait: %d\n", seconds_to_wait);
	sleep(seconds_to_wait);
}
