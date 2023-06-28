//bin/true; exec ccompile "$0" -- "$@"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHAR_SIZE 4
#define SINGLE_BYTE_END 128
#define TWO_BYTE_END 224
#define THREE_BYTE_END 240
#define FOUR_BYTE_END 248
#define FF 255U

static char (*utf8len(char *string, unsigned short *utf8_characters))[CHAR_SIZE]
{
	char (*utf8_char_array)[CHAR_SIZE] = calloc(
		strlen(string) * CHAR_SIZE + 1, 1
	);
	for (char (*p)[CHAR_SIZE] = utf8_char_array,
		character_length = 0; *string;) {

		unsigned char first_byte = (unsigned char)*string & FF;
		if (first_byte < SINGLE_BYTE_END) {
			character_length = 1;
		} else if (first_byte < TWO_BYTE_END) {
			character_length = 2;
		} else if (first_byte < THREE_BYTE_END) {
			character_length = 3;
		} else if (first_byte < FOUR_BYTE_END) {
			character_length = 4;
		} else {
			printf("Invalid UTF-8 sequence!\n");
			quick_exit(1);
		}
		++*utf8_characters;
		memcpy(p++, string, (unsigned long)character_length);
		string += character_length;
	}
	return utf8_char_array;
}

#define ALIGNMENT_BYTES 8
struct characters_count_struct {
	char utf8_character[CHAR_SIZE];
	unsigned int occurances_count;
} __attribute__((aligned(ALIGNMENT_BYTES)));

static void is_in_struct(struct characters_count_struct *ptr,
	char *str)
{
	for (; ptr->occurances_count; ++ptr) {
		if (!strcmp(ptr->utf8_character, str)) {
			++ptr->occurances_count;
			return;
		}
	}
	memcpy(ptr->utf8_character, str, CHAR_SIZE);
	ptr->occurances_count = 1;
}

static void get_occurrences_count(char *string)
{
	unsigned short characters_number = 0;
	char (*shit)[CHAR_SIZE] = utf8len(string, &characters_number);
	struct characters_count_struct *characters_count = calloc(
		characters_number + 1, sizeof(struct characters_count_struct)
	);
	for (char (*ptr)[CHAR_SIZE] = shit; **ptr; ++ptr) {
		is_in_struct(characters_count, *ptr);
	}
	free(shit);
	for (struct characters_count_struct *ptr = characters_count;
		ptr->occurances_count; ++ptr) {
		printf("%.4s: %d\n",
			ptr->utf8_character, ptr->occurances_count);
	}
	free(characters_count);
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("Only one Argument is supported");
		return 1;
	}
	get_occurrences_count(argv[1]);
}
