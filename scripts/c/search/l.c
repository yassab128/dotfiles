#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define USE_FWRITE 0

struct node {
	char *data;
	struct node *next;
};

static void
print_and_free(struct node *head, struct node *tail)
{
	struct node *temp;

	while (head != tail) {
		/* fputs(head->data, stdout); */
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

int
main(const int argc, const char *const *const argv)
{
	assert(argc == 2);

	read_file(argv[1]);
}
