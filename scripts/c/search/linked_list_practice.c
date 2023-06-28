#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define USE_A_LINKED_LIST 1
#define USE_FWRITE 1

#if USE_A_LINKED_LIST
struct node {
	char *data;
	struct node *next;
};

static void
print_and_free(struct node *head, struct node *tail)
{
	struct node *temp;

	while (head != tail) {
		fputs(head->data, stdout);
		free(head->data);
		temp = head;
		head = head->next;
		free(temp);
	}
	free(tail);
}

static void
read_file(const char *const file)
{
	FILE *fp;
	char *lineptr = 0;
	long nread;
	unsigned long len;
	struct node *head;
	struct node *tail;

	fp = fopen(file, "r");
	assert(fp);

	head = malloc(sizeof(struct node));
	tail = head;

	for (nread = getline(&lineptr, &len, fp); nread != -1;
	     nread = getline(&lineptr, &len, fp)) {
		tail->next = malloc(sizeof(struct node));
		tail->data = lineptr;
		lineptr = 0;
		tail = tail->next;
	}
	free(lineptr);
	fclose(fp);

	print_and_free(head, tail);
}
#else
struct string {
	char *data;
	unsigned long length;
};

static void
print_array_and_free(struct string *array, unsigned short i)
{
	unsigned short j = 0;
	while (j < i) {
#	if USE_FWRITE
#		if 1
		fwrite(array[j].data, 1, array[j].length, stdout);
#		else
		write(1, array[j].data, array[j].length, 1);
#		endif
#	else
		fputs(array[j].data, stdout);
#	endif
		free(array[j].data);
		++j;
	}
	free(array);
}

static void
read_file(const char *const file)
{
	FILE *fp;
	char *lineptr = 0;
	long nread;
	unsigned long len;
	struct string *array;
	unsigned short i = 0;
	unsigned short nmemb = 0;

	fp = fopen(file, "r");
	assert(fp);
	array = malloc(1);
	for (nread = getline(&lineptr, &len, fp); nread != -1;
	     nread = getline(&lineptr, &len, fp), ++i) {
		if (nmemb == i) {
			nmemb = (unsigned short)((i + 1) << 1);
			array = realloc(array, nmemb * sizeof(struct string));
		}
		array[i].data = lineptr;
		array[i].length = (unsigned long)nread;
		lineptr = 0;
	}
	free(lineptr);
	fclose(fp);
	print_array_and_free(array, i);
}
#endif

#define BILLION 1000000000

int
main(const int argc, const char *const *const argv)
{
	struct timespec begin;
	struct timespec end;

	assert(argc == 2);

	clock_gettime(CLOCK_MONOTONIC, &begin);

	read_file(argv[1]);

	clock_gettime(CLOCK_MONOTONIC, &end);
	if (end.tv_nsec < begin.tv_nsec) {
		printf("Total elapsed time %ld.%09ld\n",
		 end.tv_sec - begin.tv_sec - 1,
		 BILLION + end.tv_nsec - begin.tv_nsec);
	} else {
		printf("Total elapsed time %ld.%09ld\n",
		 end.tv_sec - begin.tv_sec, end.tv_nsec - begin.tv_nsec);
	}
}
