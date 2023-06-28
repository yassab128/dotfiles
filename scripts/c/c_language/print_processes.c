//bin/true; exec ccompile "$0" -- "$@"

/* Print the pid and the command name of each running process */

#include <stdio.h>
#include <dirent.h>

static char is_a_digit(char character)
{
	return (character >= '0' && character <= '9') ? 1 : 0;
}

static void construct_path(char *str, char *num)
{
	str[0] = '/'; str[1] = 'p'; str[2] = 'r'; str[3] = 'o'; str[4] = 'c';
	str[5] = '/'; 

	short i = 6;
	for (; num[i - 6]; ++i) {
	     str[i] = num[i - 6];
	}
	str[i] = '/'; str[++i] = 'c'; str[++i] = 'o'; str[++i] = 'm';
	str[++i] = 'm';
}

void static get_pids()
{
	DIR *proc = opendir("/proc");
	/* Skip '.' */
	readdir(proc);
	/* Skip '..' */
	readdir(proc);
	char file_path[20] = {0};
	char line[150];
	FILE *comm;
	for (struct dirent *file = readdir(proc); file; file = readdir(proc)) {
		if (!is_a_digit(file->d_name[0])) {
			continue;
		}
		construct_path(file_path, file->d_name);
		comm = fopen(file_path, "re");
		fgets(line, 150, comm);
		fclose(comm);
		printf("%s: %s", file->d_name, line);
	}
	closedir(proc);
}

int main()
{
	get_pids();
}
