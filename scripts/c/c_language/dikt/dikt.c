#if 0
f="${0#*/}"; f="$HOME/.local/bin/${f%%.c}"; clang -Weverything -lreadline \
	-lcurl -lpcre2-8 -D_GNU_SOURCE "$0" -o"$f" && exec "$f"
#endif

#include <curl/curl.h>
#include <fcntl.h>
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static int i;
static unsigned long len;
static char *nouns[] = {
#	include "nouns.h"
	0
};

static char *character_name_generator(const char *text, int state)
{
	if (!state) {
		i = 0;
		len = strlen(text);
	}
	for (; nouns[i]; ++i) {
		if (!strncasecmp(nouns[i], text, len)) {
			return strdup(nouns[i++]);
		}
	}
	return 0;
}

static char **character_name_completion(const char *text, int start, int end)
{
	(void)end;
	/* Never complete on file names. */
	rl_attempted_completion_over = 1;
	/* Only complete on the first word. */
	return start ? 0 : rl_completion_matches(text, character_name_generator);
}

static unsigned int write_function(char *ptr, unsigned int size,
	unsigned int nmemb, char **output)
{
	*output = realloc(*output, nmemb + len + 1);
	memcpy(*output + len, ptr, nmemb + 1);
	len += nmemb;
	return size * nmemb;
}

static unsigned char dikt_run(struct pcre2_real_code_8 *re)
{
	char *output = malloc(1);
	{
		char *buffer = readline("> ");
		if (!buffer) {
			free(buffer);
			return 1;
		}
		char *noun = 0;
		sscanf(buffer, "%m[^,]", &noun);
		free(buffer);
		if (!noun) {
			free(noun);
			return 0;
		}
		char *url = 0;
		asprintf(&url, "https://de.wiktionary.org/wiki/%s?action=raw",
			noun);
		free(noun);

		CURL *curl = curl_easy_init();
		if (!curl) {
			return 1;
		}
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_function);
		len = 0;
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output);
		curl_easy_perform(curl);
		free(url);
		curl_easy_cleanup(curl);
	}

	pcre2_substitute(re,
		(unsigned char *)output,
		PCRE2_ZERO_TERMINATED,
		0,
		PCRE2_SUBSTITUTE_EXTENDED | PCRE2_SUBSTITUTE_GLOBAL,
		0,
		0,
		(unsigned char *)"${n1:+:\t}",
		PCRE2_ZERO_TERMINATED,
		(unsigned char *)output,
		&len);

	{
		int pipe_to_child[2];
		pipe2(pipe_to_child, O_CLOEXEC);
		pid_t pid = fork();
		if (pid == 0) {
			close(pipe_to_child[1]);
			dup2(pipe_to_child[0], 0);
			close(pipe_to_child[0]);
			execlp("less", "less", (char *)0);
			_exit(0);
		}
		close(pipe_to_child[0]);
		write(pipe_to_child[1], output, len);
		close(pipe_to_child[1]);
		free(output);
		waitpid(pid, 0, 0);
	}
	return 0;
}

int main()
{
	struct pcre2_real_code_8 *re = pcre2_compile((unsigned char *)
		"(?<n1>"
		"{{Quellen}}\n\n?|"
		"^{{Referenzen}}.*?\n\n|"
		"^{{Aussprache}}.*?\n\n|"
		"^{{Ü-Tabelle.*?(?=^\\*{{en}})|"
		"^\\*{{[^e][^n]}}.*?\n(?=\n)|"
		"Ü\\|en\\||"
		"{|"
		"}|"
		"==== .bersetzungen ====|"
		"\\[\\[|"
		"\\]\\]|"
		"''|"
		"<ref>[^\n]*"
		")|(?<n2>"
		"^\\||^:)",
		PCRE2_ZERO_TERMINATED,
		PCRE2_MULTILINE | PCRE2_DOTALL,
		&(int){0},
		&(unsigned long){0},
		0);

	rl_attempted_completion_function = character_name_completion;
	rl_variable_bind("colored-completion-prefix", "on");
	while (1) {
		if (dikt_run(re)) {
			break;
		}
	}
	pcre2_code_free(re);
}
