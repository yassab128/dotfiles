#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define DEBUG 0
#if DEBUG
#	define PRINTF(x, y) fprintf(stderr, x, y)
#	define FWRITE(x, y)                                                   \
		fwrite(x, 1, y, stderr);                                       \
		putc('\n', stderr)
#	define PUTS(x) fputs(x, stderr)
#else
#	define PRINTF(x, y)
#	define FWRITE(x, y)
#	define PUTS(x)
#endif

#define WAIT_TIME 30

#define POST_PART "16"

static void my_sleep(const long seconds)
{
	PRINTF("waiting for %ld seconds\n", seconds);
	struct timespec tm;
	tm.tv_sec = seconds;
	tm.tv_nsec = 0;
	nanosleep(&tm, 0);
}

unsigned char new_post;

static unsigned long get_response_data(const char *ptr,
				       const unsigned long size,
				       const unsigned long nmemb,
				       const void *userdata)
{
	++new_post;
	FWRITE(ptr, nmemb);
	return size * nmemb;
}

static void notify(void)
{
	PUTS("the match was found\n");
	int pid = fork();
	if (!pid) {
		close(1);
		setuid(2000);
		execl("/bin/cmd", "/bin/cmd", "notification", "post", "-t",
		      "Charging stopped", "", "", (char *)0);
	}
	waitpid(pid, 0, 0);
}

int main()
{
	CURL *curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL,
			 "https://www.reddit.com/"
			 "search.json?q=author%3AGiveMeYourDownv0tes%"
			 "20subreddit%3AYUROP%20title%3A%22Part%20" POST_PART
			 "%22");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, get_response_data);
	for (;;) {
		new_post = 0;
		if (curl_easy_perform(curl)) {
			PUTS("no connection\n");
			my_sleep(WAIT_TIME);
			continue;
		}

		if (new_post != 1) {
			notify();
		}

		my_sleep(WAIT_TIME);
	}
	curl_easy_cleanup(curl);
}
