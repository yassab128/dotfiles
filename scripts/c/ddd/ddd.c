#include <locale.h>
#include <menu.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define WARNIF(x)                                                              \
	do {                                                                   \
		if (x) {                                                       \
			perror(TOSTRING(__LINE__) ":" #x);                     \
			/* _exit(1); */                                        \
		}                                                              \
	} while (0)

#define WADDSTR(win, fmt)                                                      \
	do {                                                                   \
		WARNIF(waddstr(win, "\r" fmt) == ERR);                         \
		WARNIF(wclrtoeol(win) == ERR);                                 \
		WARNIF(wrefresh(win) == ERR);                                  \
	} while (0)

/* I had to switch from c89 to c99 just to use variadic macros. */
#define WPRINTW(win, fmt, ...)                                                 \
	do {                                                                   \
		WARNIF(wprintw(win, "\r" fmt, __VA_ARGS__) == ERR);            \
		WARNIF(wclrtoeol(win) == ERR);                                 \
		WARNIF(wrefresh(win) == ERR);                                  \
	} while (0)

#define SHOW_KEY_BINDINGS                                                      \
	WARNIF(                                                                \
	 addstr("\tKey bindings list\n"                                        \
		"\t\tcursor down        \tj\n"                                 \
		"\t\tcursor up          \tk\n"                                 \
		"\t\tgo to first line   \tg\n"                                 \
		"\t\tgo to last line    \tG\n"                                 \
		"\t\tscroll down        \te\n"                                 \
		"\t\tscroll up          \ty\n"                                 \
		"\t\tpage down          \td\n"                                 \
		"\t\tpage up            \tu\n"                                 \
		"\t\tselect current line\tENTER\n"                             \
		"\t\tsearch forward     \t/\n"                                 \
		"\t\tsearch again       \tn\n"                                 \
		"\t\tsearch reverse     \tN\n"                                 \
		"\t\texit               \t^C\n"                                \
		"\n\n"                                                         \
		"\tPattern matching:\n"                                        \
		"\t\tPattern matching is case insensitive\n"                   \
		"\t\tThe Pattern must be a fixed string and"                   \
		" not a regular expression\n"                                  \
		"\t\tThe Pattern matches the beginning of the line only\n"     \
		"\n\n"                                                         \
		"\t[Press any key to go back]") == ERR)

/*
 * I know this looks dumb, but this is the FASTEST way to know how many
 * digits an int has.
 */
#define NUMBER_OF_DIGITS_IN_INT(x)                                             \
	((x) < 10		 ? 1                                           \
	 : (x) < 100		 ? 2                                           \
	  : (x) < 1000		 ? 3                                           \
	   : (x) < 10000	 ? 4                                           \
	    : (x) < 100000	 ? 5                                           \
	     : (x) < 1000000	 ? 6                                           \
	      : (x) < 10000000	 ? 7                                           \
	       : (x) < 100000000 ? 8                                           \
				 : 9)

static unsigned char g_keep_going = 1;

static void
clean_exit(int signum)
{
	(void)signum;
	g_keep_going = 0;
}

#define PATTERN_MAX 255
int
main()
{
	ITEM **menu_items;
	MENU *menu;
	unsigned long items_number;
	unsigned long i = 0;
	char pattern[PATTERN_MAX];
	int ch;
	int current_item_index;
	unsigned char cursor_moved = 1;
	WINDOW *menu_win;
	WINDOW *menu_sub_win;
	WINDOW *left_status_win;
	WINDOW *right_status_win;
	char *choices[] = {
#include "nouns.h"
	};

	struct sigaction sa = {0};

	setlocale(LC_ALL, "en_US.UTF-8");

	sa.sa_handler = clean_exit;
	WARNIF(sigaction(SIGINT, &sa, 0) == -1);

	*pattern = 0;

	/* Prepare the menu items. */
	items_number = sizeof(choices) / sizeof(*choices);
	menu_items = malloc((items_number + 1) * sizeof(ITEM *));
	WARNIF(!menu_items);
	menu_items[items_number] = (ITEM *)0;
	for (; i < items_number; ++i) {
		menu_items[i] = new_item(choices[i], 0);
		WARNIF(!menu_items[i]);
	}
#if 0
	printf("%s\n", menu_items[0]->name.str);
	printf("%hu\n", menu_items[0]->name.length);
	/*printf("%s\n", menu_items[0]->description.str);
	printf("%hu\n", menu_items[0]->description.length);
	printf("%d\n", menu_items[0]->imenu);*/
	printf("%hd\n", menu_items[0]->x);
	printf("%hd\n", menu_items[0]->y);
	return 0;
#endif

	WARNIF(!initscr());
	WARNIF(curs_set(0) == ERR);
	WARNIF(noecho() == ERR);

	/* Create the window where the menu will be displayed. */
	/*{ */
	int nlines = LINES;
	const int ncols = COLS;
	menu_win = newwin(nlines, ncols, 0, 0);
	WARNIF(!menu_win);
#if 1
	menu = new_menu(menu_items);
#else
	ITEM *shit_items[2] = {0};
	shit_items[0]->name.str = "shit";
	shit_items[0]->name.length = 4;
	shit_items[0]->description.str = 0;
	shit_items[0]->description.length = 0;
	shit_items[0]->imenu = 0;
	shit_items[0]->userptr = 0;
	shit_items[0]->opt = 0;
	shit_items[0]->index = 0;
	shit_items[0]->x = 0;
	shit_items[0]->y = 0;
	shit_items[0]->value = 0;
	shit_items[0]->left = 0;
	shit_items[0]->right = 0;
	shit_items[0]->up = 0;
	shit_items[0]->down = 0;
	menu = new_menu((ITEM **)shit_items);
#endif
	WARNIF(!menu);
	WARNIF(set_menu_win(menu, menu_win) == ERR);
	--nlines;
	menu_sub_win = derwin(menu_win, nlines, ncols, 0, 0);
	WARNIF(!menu_sub_win);

	WARNIF(set_menu_sub(menu, menu_sub_win) == ERR);
	WARNIF(set_menu_format(menu, nlines, 0) != E_OK);
	/* } */

	/* Find the appropriate width for the right status window. */
	{
		int right_status_win_ncols;
		right_status_win_ncols =
		 (NUMBER_OF_DIGITS_IN_INT(items_number) << 1) + 2;

		left_status_win =
		 newwin(1, COLS - right_status_win_ncols, LINES - 1, 0);
		WARNIF(!left_status_win);

		right_status_win =
		 newwin(0, 0, LINES - 1, COLS - right_status_win_ncols);
		WARNIF(!right_status_win);
	}

#if 1
	WARNIF(endwin() == ERR);
	printf("%s\n", menu_items[0]->name.str);
	printf("%d\n", menu_items[0]->name.length);
	printf("%s\n", menu_items[0]->description.str);
	printf("%d\n", menu_items[0]->description.length);

	printf("LINES: %d\n", LINES);
	printf("COLS: %d\n", COLS);
	printf("imenu-height: %hd\n", menu_items[0]->imenu->height);
	printf("imenu-width: %hd\n", menu_items[0]->imenu->width);
	printf("imenu-rows: %hd\n", menu_items[0]->imenu->rows);
	printf("imenu-cols: %hd\n", menu_items[0]->imenu->cols);
	printf("imenu-frows: %hd\n", menu_items[0]->imenu->frows);
	printf("imenu-fcols: %hd\n", menu_items[0]->imenu->fcols);
	printf("imenu-arows: %hd\n", menu_items[0]->imenu->arows);
	printf("imenu-namelen: %hd\n", menu_items[0]->imenu->namelen);
	printf("imenu-desclen: %hd\n", menu_items[0]->imenu->desclen);
	printf("imenu-marklen: %hd\n", menu_items[0]->imenu->marklen);
	printf("imenu-itemlen: %hd\n", menu_items[0]->imenu->itemlen);

	printf("%d\n", menu_items[0]->userptr);
	printf("%d\n", menu_items[0]->opt);
	printf("%d\n", menu_items[0]->index);
	printf("%d\n", menu_items[0]->x);
	printf("%d\n", menu_items[0]->y);
	printf("%d\n", menu_items[0]->value);
	/*printf("%d\n", menu_items[0]->left);
	printf("%d\n", menu_items[0]->right);
	printf("%d\n", menu_items[0]->up);
	printf("%d\n", menu_items[0]->down);*/
	return 0;
#endif
	WARNIF(post_menu(menu) != E_OK);

	WARNIF(keypad(menu_win, TRUE) == ERR);
	while (g_keep_going) {
		current_item_index = item_index(current_item(menu));

		WPRINTW(right_status_win, "%d/%lu", current_item_index + 1,
		 items_number);

		ch = wgetch(menu_win);
		switch (ch) {
		case 'g':
			WARNIF(set_current_item(menu, menu_items[0]) != E_OK);
			break;
		case 'G':
			WARNIF(set_current_item(
				menu, menu_items[items_number - 1]) != E_OK);
			break;
		case '/':
			WARNIF(werase(left_status_win) == ERR);
			WARNIF(waddch(left_status_win, '/') == ERR);

			WARNIF(curs_set(1) == ERR);
			WARNIF(echo() == ERR);

			WARNIF(wgetnstr(
				left_status_win, pattern, PATTERN_MAX) == ERR);

			WARNIF(noecho() == ERR);
			WARNIF(curs_set(0) == ERR);

			if (!memchr(pattern, 0, PATTERN_MAX)) {
				WPRINTW(left_status_win,
				 "The pattern's length must not "
				 "exceed %u\n",
				 PATTERN_MAX);
				continue;
			}
			switch (set_menu_pattern(menu, pattern)) {
			case E_OK:
				cursor_moved = 0;
				break;
			default:
				*pattern = 0;
				break;
			}
			continue;
		case 'n':
		case 'N':
			if (!*pattern) {
				continue;
			}
			if (cursor_moved) {
				cursor_moved = 0;
				WPRINTW(left_status_win, "/%s", pattern);
				set_menu_pattern(menu, pattern);
			}
			switch (menu_driver(
			 menu, ch == 'n' ? REQ_NEXT_MATCH : REQ_PREV_MATCH)) {
			case E_OK:
				break;
			case E_NO_MATCH:
				WADDSTR(
				 left_status_win, "No other match was found");
				break;
			default:
				WADDSTR(
				 left_status_win, "Pattern matching failed");
				break;
			}
			continue;
		case 'd':
			switch (menu_driver(menu, REQ_SCR_DPAGE)) {
			case E_OK:
				break;
			case E_REQUEST_DENIED:
				menu_driver(menu, REQ_LAST_ITEM);
				break;
			default:
				break;
			}
			break;
		case 'u':
			switch (menu_driver(menu, REQ_SCR_UPAGE)) {
			case E_OK:
				break;
			case E_REQUEST_DENIED:
				menu_driver(menu, REQ_FIRST_ITEM);
				break;
			default:
				break;
			}
			break;
		case 'y':
			menu_driver(menu, REQ_SCR_ULINE);
			break;
		case 'e':
			menu_driver(menu, REQ_SCR_DLINE);
			break;
		case KEY_DOWN:
		case 'j':
			menu_driver(menu, REQ_DOWN_ITEM);
			break;
		case KEY_UP:
		case 'k':
			menu_driver(menu, REQ_UP_ITEM);
			break;
		case '\n':
			continue;
		case '?':
			WARNIF(erase() == ERR);
			SHOW_KEY_BINDINGS;
			WARNIF(getch() == ERR);
			WARNIF(redrawwin(menu_win) == ERR);
			WARNIF(redrawwin(right_status_win) == ERR);
			WARNIF(redrawwin(left_status_win) == ERR);
			cursor_moved = 1;
			continue;
		case ERR:
			g_keep_going = 0;
			break;
		default:
			WADDSTR(left_status_win,
			 "Press ? to list the supported key bindings");
			cursor_moved = 1;
			continue;
		}
		cursor_moved = 1;
		WARNIF(werase(left_status_win) == ERR);
		WARNIF(wrefresh(left_status_win) == ERR);
	}
	/*
	 * Restore some defaults so that the program will not break the
	 * terminal after exiting.
	 */
	WARNIF(curs_set(1) == ERR);
	WARNIF(echo() == ERR);
	/* Free it all */
	WARNIF(endwin() == ERR);
	WARNIF(delwin(left_status_win) == ERR);
	WARNIF(delwin(right_status_win) == ERR);

	WARNIF(unpost_menu(menu) != E_OK);
	for (i = 0; i < items_number; ++i) {
		WARNIF(!free_item(menu_items[i]));
	}
	WARNIF(free_menu(menu) != E_OK);
	free(menu_items);
	WARNIF(delwin(menu_sub_win) == ERR);
	WARNIF(delwin(menu_win) == ERR);
	WARNIF(delwin(stdscr) == ERR);

	exit_curses(0);
}
