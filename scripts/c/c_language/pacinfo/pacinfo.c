#if 0
true; exec ccompile "$0" -D_GNU_SOURCE -- "$@"
#endif

#include <dirent.h>
#include <preprocessor_macros.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RECOMMENDED_ALIGNMENT 64
#define GAP_SIZE 9
struct packages {
	char *name;
	char *conflicts;
	char *optdepends_str;
	char *depends_str;
	unsigned long installdate;
	unsigned int size;

	unsigned short files;
	unsigned short manpages;
	unsigned short binaries;

	unsigned char explicit;
	unsigned char optdepends;
	unsigned char installed_optdepends;
	unsigned char depends;
	unsigned char rev_deps;
	char gap[GAP_SIZE];
} __attribute__((aligned(RECOMMENDED_ALIGNMENT)));

struct lengths {
	unsigned char name;
};

static void get_desc(struct packages *package, struct lengths *length,
	char *desc_path)
{
	FILE *fp = fopen(desc_path, "re");

	unsigned int n = 0;
	unsigned int m = 0;
	for (char *str1 = 0, *str2 = 0; fscanf(fp, "%%%m[^%%]%%\n%n%m[^%%]%n",
		&str1, &m, &str2, &n) != EOF;
	) {
		if (!strcmp(str1, "NAME")) {
			sscanf(str2, "%m[^\n]%n", &package->name, &n);
			if (n > length->name) {
				length->name = (unsigned char)n;
			}
		} else if (!strcmp(str1, "CONFLICTS")) {
			n -= m + 2;
			str2[n] = 0;
			package->conflicts = malloc(n + 3);
			memcpy(package->conflicts, "| ", 2);
			memcpy(package->conflicts + 2, str2, n + 1);
			while (--n) {
				if (package->conflicts[n] == '\n') {
					package->conflicts[n] = ' ';
				}
			}
		} else if (!strcmp(str1, "INSTALLDATE")) {
			sscanf(str2, "%lu", &package->installdate);
		} else if (!strcmp(str1, "SIZE")) {
			sscanf(str2, "%u", &package->size);
		} else if (!strcmp(str1, "REASON")) {
			package->explicit = 1;
		} else if (!strcmp(str1, "DEPENDS")) {
			n -= m + 2;
			str2[n] = 0;
			package->depends_str = malloc(n + 1);
			memcpy(package->depends_str, str2, n + 1);
		} else if (!strcmp(str1, "OPTDEPENDS")) {
			n -= m + 2;
			str2[n] = 0;
			package->optdepends_str = malloc(n + 1);
			memcpy(package->optdepends_str, str2, n + 1);
		}
		free(str1);
		free(str2);
	}
	fclose(fp);
}

static void get_files(struct packages *package, char *files_path)
{
	FILE *fp = fopen(files_path, "re");

	regmatch_t matches[1];
	struct re_pattern_buffer manpages_preg;
	/* regcomp(&manpages_preg, "^\\/usr\\/bin\\/.*[^/]\n", 0); */
	regcomp(&manpages_preg, "^usr/share/man/man[1-8]/[^\n]", 0);
	struct re_pattern_buffer binaries_preg;
	regcomp(&binaries_preg, "^usr/bin/.*[^/]\n", 0);

	unsigned long n = 0;
	char *line = 0;
	getline(&line, &n, fp);
	while (getline(&line, &n, fp) > 1) {
		if (!regexec(&manpages_preg, line, 1, matches, 0)) {
			++package->manpages;
		} else if (!regexec(&binaries_preg, line, 1, matches, 0)) {
			++package->binaries;
		}
		++package->files;
	}
	free(line);
	regfree(&binaries_preg);
	regfree(&manpages_preg);

	fclose(fp);
}

#include "sort_functions.h"

static void get_optdepends(struct packages *package, unsigned int index)
{
	char *optdepends_str = package[index].optdepends_str;
	for (char *str = 0; sscanf(optdepends_str, "%m[^:\n]", &str) != EOF;)
	{
		++package[index].optdepends;
		for (struct packages *ptr = package; ptr->name; ++ptr) {
			if (!strcmp(ptr->name, str)) {
				++package[index].installed_optdepends;
				break;
			}
		}
		free(str);

		optdepends_str = strchr(optdepends_str, '\n');
		if (optdepends_str) {
			++optdepends_str;
		} else {
			break;
		}
	}
	free(package[index].optdepends_str);
	asprintf(&package[index].optdepends_str, "%hhu/%hhu",
		package[index].installed_optdepends, package[index].optdepends);
	if (package[index].optdepends == 0) {
		printf("%s\n", package[index].name);
	}
}


static void get_depends(struct packages *package, unsigned int index)
{
	char *ptr = package[index].depends_str;
	char *dependency = 0;
	for (unsigned int n = 0;
		sscanf(ptr, "%m[^\n]\n%n", &dependency, &n) != EOF;
		ptr += n)
	{
		for (struct packages *str_ptr = package; str_ptr->name;
			++str_ptr)
		{
			if (!strcmp(str_ptr->name, dependency)) {
				++str_ptr->rev_deps;
				++package[index].depends;
				break;
			}
		}
		free(dependency);
	}
}

int main(int n, char **argv)
{
	if (n != 2) {
		return 1;
	}
	sscanf(argv[1], "%*[bdefimors]%n", &n);
	if (n != 1) {
		return 1;
	}

	struct dirent **namelist = 0;
	n = scandir("/var/lib/pacman/local/", &namelist, 0, 0);
	free(namelist[0]);
	free(namelist[1]);


	struct lengths length = {0};
	struct packages *package =
		calloc((unsigned int)n - 2, sizeof(struct packages));

	unsigned int len = 0;
	unsigned int m = 0;
	for (char *tmp_path = 0; n-- > 2; free(namelist[n])) {
		if (namelist[n]->d_type != DT_DIR) {
			continue;
		}
		len = (unsigned int)asprintf(&tmp_path,
			"/var/lib/pacman/local/%s/files", namelist[n]->d_name);
		get_files(&package[m], tmp_path);
		memcpy(tmp_path + len - sizeof("files") + 1,
			"desc", sizeof("desc"));
		get_desc(&package[m], &length, tmp_path);
		free(tmp_path);
		++m;
	}
	free(namelist);

	len = m;
	/* Post processing. */
	while (len--) {
		if (package[len].optdepends_str) {
			get_optdepends(package, len);
		} else {
			package[len].optdepends_str = "0/0";
		}

		if (package[len].depends_str) {
			get_depends(package, len);
		}
	}

	qsort(package, m, sizeof(struct packages),
		*argv[1] == 'b' ? sort_binaries :
		*argv[1] == 'd' ? sort_depends :
		*argv[1] == 'e' ? sort_explicit :
		*argv[1] == 'f' ? sort_files :
		*argv[1] == 'i' ? sort_installdate :
		*argv[1] == 'm' ? sort_manpages :
		*argv[1] == 'o' ? sort_optdepends :
		*argv[1] == 'r' ? sort_rev_depends :
		sort_size);


	/* for (len = 0; len < m; ++len) { */
	len = m;
	for (char unit = 0; len--;) {
		printf("%d| %-*s| %7.2f%C | %2dd| %4so| %3hhur| %3db| "
			"%4dm| %4df\n",
			package[len].explicit,
			length.name,
			package[len].name,
			HUMAN_SIZE((double)package[len].size, unit),
			package[len].depends,
			package[len].optdepends_str,
			package[len].rev_deps,
			package[len].binaries,
			package[len].manpages,
			package[len].files);

		if (package[len].conflicts) {
			free(package[len].conflicts);
		}
		if (package[len].optdepends_str[2] != '0') {
			free(package[len].optdepends_str);
		}
		if (package[len].depends_str) {
			free(package[len].depends_str);
		}
		free(package[len].name);
	}
	free(package);
}
