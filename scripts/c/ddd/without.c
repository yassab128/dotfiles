#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <string.h>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define WARNIF(x)                                                              \
	do {                                                                   \
		if (x) {                                                       \
			perror(TOSTRING(__LINE__) ":" #x);                     \
			/* _exit(1); */                                        \
		}                                                              \
	} while (0)

#define PATTERN_MAX 64

#define SHOW_KEY_BINDINGS                                                      \
	WARNIF(                                                                \
	 addstr("\tKeys\n"                                                     \
		"\t\tcursor down              \tj\n"                           \
		"\t\tcursor up                \tk\n"                           \
		"\t\tgo to first line         \tg\n"                           \
		"\t\tgo to last line          \tG\n"                           \
		"\t\tscroll half a screen down\td\n"                           \
		"\t\tscroll half a screen up  \tu\n"                           \
		"\t\tselect current line      \tENTER\n"                       \
		"\t\tsearch forward           \t/\n"                           \
		"\t\tsearch again             \tn\n"                           \
		"\t\tsearch backward          \tN\n"                           \
		"\t\texit                     \t^C\n"                          \
		"\tNotes\n"                                                    \
		"\t\tPattern matching uses strstr(3)\n"                        \
		"\t\tThe mouse support is neither supported nor planned\n"     \
		"\n\t[Press any key to go back]") == ERR)

static int
search_forward(char *const *const items_arr, const int array_size, const int i,
 const char *const needle, WINDOW *const prompt_win)
{
	int j = i + 1;
	while (j < array_size) {
		if (strstr(items_arr[j], needle)) {
			return j;
		}
		++j;
	}
	j = 0;
	while (j < i) {
		if (strstr(items_arr[j], needle)) {
			return j;
		}
		++j;
	}
	WARNIF(wprintw(prompt_win, "\rPattern not found: %s", needle) == ERR);
	WARNIF(wclrtoeol(prompt_win) == ERR);
	WARNIF(wrefresh(prompt_win) == ERR);
	return i;
}

static int
search_backward(char *const *const items_arr, const int array_size, const int i,
 const char *const needle, WINDOW *const prompt_win)
{
	int j = i - 1;
	while (j >= 0) {
		if (strstr(items_arr[j], needle)) {
			return j;
		}
		--j;
	}
	j = array_size - 1;
	while (j > i) {
		if (strstr(items_arr[j], needle)) {
			return j;
		}
		--j;
	}
	WARNIF(wprintw(prompt_win, "\rPattern not found: %s", needle) == ERR);
	WARNIF(wclrtoeol(prompt_win) == ERR);
	WARNIF(wrefresh(prompt_win) == ERR);
	return i;
}

static void
test_locale(void)
{
	char *current_locale = setlocale(LC_CTYPE, "");
	if (strstr(current_locale, ".UTF-8")) {
		return;
	}
	if (setlocale(LC_CTYPE, "en_US.UTF-8")) {
		return;
	}
	puts("\nYou are not using a UTF-8 locale. "
	     "Umlauts will not be shown properly.\n"
	     "Press ENTER key to continue.");
	getchar();
}

int
main()
{
	int ch;
	int current_index = 0;
	int i;
	int j;
	WINDOW *main_win;
	WINDOW *menu_win;
	WINDOW *left_status_win;
	unsigned char ncols;
	unsigned char nlines;
	unsigned char menu_nlines;
	unsigned char menu_nlines_half;
	char *const items_arr[] = {
#include "./nouns.h"
	};
	const int nitems = sizeof(items_arr) / sizeof(char *);
	char pattern[PATTERN_MAX] = "\t";

	test_locale();

	main_win = initscr();
	WARNIF(!main_win);
	WARNIF(curs_set(0) == ERR);
	WARNIF(noecho() == ERR);
	WARNIF(set_tabsize(2) == FALSE);

	ncols = (unsigned char)COLS;
	nlines = (unsigned char)LINES;
	if (ncols < 64 || nlines < 4) {
		WARNIF(endwin() == ERR);
		puts("\nTerminal too small");
		return 1;
	}

	menu_nlines = (unsigned char)(nlines - 1);
	menu_nlines_half = (menu_nlines >> 1);

	menu_win = newwin(menu_nlines, ncols, 0, 0);
	WARNIF(!menu_win);
	left_status_win = newwin(1, ncols, menu_nlines, 0);
	WARNIF(!left_status_win);

	/* WARNIF(keypad(menu_win, TRUE) == ERR); */

	for (;;) {
		if (current_index < menu_nlines_half) {
			i = 0;
			j = menu_nlines;
		} else if (current_index >= nitems - menu_nlines_half) {
			i = nitems - menu_nlines;
			j = nitems;
		} else {
			i = current_index - menu_nlines_half;
			j = i + menu_nlines;
		}
		WARNIF(werase(menu_win) == ERR);
		while (i < current_index) {
			WARNIF(waddstr(menu_win, items_arr[i]) == ERR);
			waddch(menu_win, '\n');
			++i;
		}
		WARNIF(wattron(menu_win, A_REVERSE) == ERR);
		WARNIF(waddstr(menu_win, items_arr[i]) == ERR);
		waddch(menu_win, '\n');
		WARNIF(wattroff(menu_win, A_REVERSE) == ERR);
		++i;
		while (i < j) {
			WARNIF(waddstr(menu_win, items_arr[i]) == ERR);
			waddch(menu_win, '\n');
			++i;
		}

		ch = wgetch(menu_win);

		WARNIF(werase(left_status_win) == ERR);
		WARNIF(wrefresh(left_status_win) == ERR);
		switch (ch) {
		case '/':
			WARNIF(waddch(left_status_win, '/') == ERR);

			WARNIF(curs_set(1) == ERR);
			WARNIF(echo() == ERR);

			WARNIF(wgetnstr(left_status_win, pattern,
				PATTERN_MAX - 1) == ERR);

			WARNIF(curs_set(0) == ERR);
			WARNIF(noecho() == ERR);

			current_index = search_forward(items_arr, nitems,
			 current_index, pattern, left_status_win);
			continue;
		case 'n':
			WARNIF(
			 wprintw(left_status_win, "\r/%s", pattern) == ERR);
			WARNIF(wclrtoeol(left_status_win) == ERR);
			WARNIF(wrefresh(left_status_win) == ERR);

			current_index = search_forward(items_arr, nitems,
			 current_index, pattern, left_status_win);
			continue;
		case 'N':
			WARNIF(
			 wprintw(left_status_win, "\r/%s", pattern) == ERR);
			WARNIF(wclrtoeol(left_status_win) == ERR);
			WARNIF(wrefresh(left_status_win) == ERR);

			current_index = search_backward(items_arr, nitems,
			 current_index, pattern, left_status_win);
			continue;
		case 'd':
			current_index += menu_nlines_half;
			if (current_index > nitems - 1) {
				current_index = nitems - 1;
			}
			continue;
		case 'u':
			current_index -= menu_nlines_half;
			if (current_index < 0) {
				current_index = 0;
			}
			continue;
		case 'g':
			current_index = 0;
			continue;
		case 'G':
			current_index = nitems - 1;
			continue;
		/* case KEY_UP: */
		case 'k':
			if (current_index == 0) {
				current_index = nitems - 1;
			} else {
				--current_index;
			}
			continue;
		/* case KEY_DOWN: */
		case 'j':
			if (current_index == nitems - 1) {
				current_index = 0;
			} else {
				++current_index;
			}
			continue;
		case '\n':
			break;
		case ERR:
			perror(TOSTRING(__LINE__) ": wgetch()");
			break;
		case '?':
			WARNIF(erase() == ERR);
			SHOW_KEY_BINDINGS;
			WARNIF(getch() == ERR);
			continue;
		default:
			WARNIF(waddstr(left_status_win,
				"\rPress ? to see the supported keys") == ERR);
			WARNIF(wclrtoeol(left_status_win) == ERR);
			WARNIF(wrefresh(left_status_win) == ERR);
			continue;
		}
		break;
	}
	WARNIF(delwin(left_status_win) == ERR);
	WARNIF(delwin(menu_win) == ERR);
	WARNIF(delwin(main_win) == ERR);
	WARNIF(delwin(stdscr) == ERR);
	WARNIF(endwin() == ERR);
	puts(items_arr[current_index]);
}
