#if 0
true; exec ccompile "$0" -D_GNU_SOURCE -- "$@"
#endif

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RECOMMENDED_ALIGNMENT1 16
struct properties {
	char *d_name;
	char gap1[4];
	char gap2[3];
	unsigned char d_type;
} __attribute__((aligned(RECOMMENDED_ALIGNMENT1)));

/* #define IS_ETC_DOTFILE 47 */
/* static void check_if_dotfile(char *d_name, */
	/* unsigned char *d_type) */
/* { */
	/* FILE* fp = fopen(d_name, "re"); */
	/* if (!fp) { */
		/* Skip files without read permission. */
		/* return; */
	/* } */
	/* getc(fp); */
	/* int c = getc(fp); */
	/* for (char *str = c == ' ' ? "vi!" : " vi!"; c != EOF && *str; ++str) { */
		/* c = getc(fp); */
		/* Ignore non-matching files. */
		/* if (c != *str) { */
			/* fclose(fp); */
			/* return; */
		/* } */
	/* } */
	/* Skip empty files. */
	/* if (c != EOF) { */
		/* *d_type = IS_ETC_DOTFILE; */
	/* } */
	/* fclose(fp); */
/* } */

static void get_files(char *directory)
{
	unsigned int total_files = 1;
	struct properties *stack =
		malloc(total_files * sizeof(struct properties));

	/* Append the trailing slash if it does not exist. */
	asprintf(&stack->d_name, directory[strlen(directory) - 1] == '/' ?
		"%s" : "%s/", directory);

	stack->d_type = DT_DIR;
	struct dirent **namelist = 0;
	unsigned int i = 0;
	for (int j = 0; i < total_files; ++i) {
		switch (stack[i].d_type) {
			case DT_DIR :
				break;
			/* case DT_REG : */
				/* check_if_dotfile(stack[i].d_name, */
					/* &stack[i].d_type); */
			default :
				continue;
		}
		j = scandir(stack[i].d_name, &namelist, 0, 0);
		if (j == -1) {
			/* Skip directories without read permission. */
			continue;
		}
		stack = realloc(stack,
			(total_files + (unsigned int)j - 2) *
				sizeof(struct properties));
		for (; j--; free(namelist[j])) {
			if (namelist[j]->d_name[0] == '.' && (
				(namelist[j]->d_name[1] == '.' &&
				namelist[j]->d_name[2] == 0 ) ||
				namelist[j]->d_name[1] == 0)
			) {
				continue;
			}
			stack[total_files].d_type =
				namelist[j]->d_type;
			asprintf(&stack[total_files].d_name,
				namelist[j]->d_type == DT_DIR ? "%s%s/" :
				"%s%s", stack[i].d_name, namelist[j]->d_name);
			total_files++;
		}
		free(namelist);
	}
	for (i = 0; i < total_files; ++i)  {
		printf("%s\n", stack[i].d_name);
		free(stack[i].d_name);
	}
	free(stack);
}

/* #include <benchmark.h> */
static void initialize()
{
	/* get_time_difference(); */
	get_files("/etc");
	/* get_time_difference(); */
}

int main()
{
	initialize();
}
