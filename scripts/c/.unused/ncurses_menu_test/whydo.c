#include <menu.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

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
	WARNIF(addstr("\tKey bindings list\n\n"                                \
		      "\tcursor down             \tj\n"                        \
		      "\tcursor up               \tk\n"                        \
		      "\tgo to first line        \tg\n"                        \
		      "\tgo to last line         \tG\n"                        \
		      "\tscroll down             \te\n"                        \
		      "\tscroll up               \ty\n"                        \
		      "\tpage down               \td\n"                        \
		      "\tpage up                 \tu\n"                        \
		      "\ttoggle selection status \tSPACE\n"                    \
		      "\toperate on the selection\tENTER\n"                    \
		      "\tsearch forward          \t/\n"                        \
		      "\tsearch again            \tn\n"                        \
		      "\tsearch reverse          \tN\n"                        \
		      "\texit                    \t^C\n"                       \
		      "\n"                                                     \
		      "\tNote: Pattern matching is case insensitive\n"         \
		      "\tand does not support regular expressions\n\n"         \
		      "\tPress any key to go back") == ERR)

/*
#define WIN_PRINT(win, fmt, ...)                                               \
	do {                                                                   \
		WARNIF(werase(win) == ERR); \
		WARNIF(wprintw(win, fmt, __VA_ARGS__) == ERR); \
		WARNIF(wrefresh(win) == ERR); \ } while (0)
*/

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

static int g_pid;

static unsigned char g_keep_going = 1;

static void
clean_exit(int signum)
{
	(void)signum;
	g_keep_going = 0;
}

static void
kill_external_process(int signum)
{
	(void)signum;
	WARNIF(kill(g_pid, SIGINT));
}

#if 0
static void
spawn_external_process(char *str)
{
	int status;

	g_pid = fork();
	if (g_pid == -1) {
		perror("fork");
	} else if (!g_pid) {
		WARNIF(
		 execlp("echo", "echo", "You have chosen", str, (char *)0));
		_exit(EXIT_FAILURE);
	}
	if (waitpid(g_pid, &status, 0) == -1) {
		perror("\nwaitpid");
	}
	WARNIF(printf("\n"
		      "\033[0;33m"
		      "[Process exited %d]"
		      "\033[0m"
		      "\n",
		WEXITSTATUS(status)) < 0);
}
#endif

static void
spawn_external_process(char **str_arr, unsigned int arr_len)
{
	unsigned int i;

	for (i = 0; i < arr_len; ++i) {
		puts(str_arr[i]);
	}
}

#define PATTERN_MAX 255
int
main()
{
	ITEM **menu_items;
	MENU *menu;
	unsigned int items_number;
	unsigned int i;
	unsigned int j;
	char pattern[PATTERN_MAX];
	int ch;
	unsigned int current_item_index;
	ITEM *current_menu_item;
	unsigned char cursor_moved = 1;
	WINDOW *main_win;
	WINDOW *menu_win;
	WINDOW *preview_win;
	WINDOW *menu_sub_win;
	WINDOW *left_status_win;
	WINDOW *right_status_win;
	char *choices[] = {"nmap", "emacs", "rust", "Choice 4", "Choice 5",
	 "Choice 6", "Choice 7", "Choice 8", "Choice 9", "Choice 10",
	 "Choice 11", "Choice 12", "Choice 13", "Choice 14", "Choice 15",
	 "Choice 16", "Choice 17", "Choice 18", "Choice 19", "Choice 20",
	 "Choice 21", "Choice 22", "Choice 23", "Choice 24", "Choice 25",
	 "Choice 26", "Choice 27", "Choice 28", "Choice 29", "Choice 30",
	 "Choice 31", "Choice 32", "Choice 33", "Choice 34", "Choice 35",
	 "Choice 36", "Choice 37", "Choice 38", "Choice 39", "Choice 40",
	 "Choice 41", "Choice 42", "Choice 43", "Choice 44", "Choice 45",
	 "Choice 46", "Choice 47", "Choice 48", "Choice 49", "Choice 50",
	 "Choice 51", "Choice 52", "Choice 53", "Choice 54", "Choice 55",
	 "Choice 56", "This is the exception", "Choice 58", "Choice 59",
	 "Choice 60", "Choice 61", "Choice 62", "Choice 63", "Choice 64",
	 "Choice 65", "Choice 66", "Choice 67", "Choice 68", "Choice 69",
	 "Choice 70", "Choice 71", "Choice 72", "Choice 73", "Choice 74",
	 "Choice 75", "Choice 76", "Choice 77", "Choice 78", "Choice 79",
	 "Choice 80", "Choice 81", "Choice 82", "Choice 83", "Choice 84",
	 "Choice 85", "Choice 86", "Choice 87", "Choice 88", "Choice 89",
	 "Choice 90", "Choice 91", "Choice 92", "Choice 93", "Choice 94",
	 "Choice 95", "Choice 96", "Choice 97", "Choice 98", "Choice 99",
	 "Exit"};

	/*
	 * The array will be enlarged dynamically, which is slow, but
	 * it doesn't matter since user input is slower, which means that the
	 * constant calls to realloc will have negligible cost.
	 */
	unsigned int selection_size = 0;
	char **selected_names;
	ITEM **selected_items;

	struct sigaction sa = {0};

	sa.sa_handler = clean_exit;
	WARNIF(sigaction(SIGINT, &sa, 0) == -1);

	*pattern = 0;

	/* Initialization stuff. */
	main_win = initscr();
	WARNIF(!main_win);
	/* Disable line buffering. */
	/* WARNIF(cbreak() == ERR); */
	/* Hide the annoying cursor. */
	WARNIF(curs_set(0) == ERR);
	/* This does something */
	/* WARNIF(intrflush(stdscr, TRUE) == ERR); */
	/* Disable printing input characters on screen. */
	WARNIF(noecho() == ERR);
	/* Enable function keys (including arrow keys). */
	/* WARNIF(keypad(stdscr, TRUE) == ERR); */

	/* Prepare the menu items. */
	items_number = (unsigned int)ARRAY_SIZE(choices);
	menu_items = malloc((items_number + 1) * sizeof(ITEM *));
	WARNIF(!menu_items);
	for (i = 0; i < items_number; ++i) {
		menu_items[i] = new_item(choices[i], "*");
		WARNIF(!menu_items[i]);
	}
	menu_items[items_number] = (ITEM *)0;

	/* Create the window where the menu will be displayed. */
	{
		int nlines;
		int ncols = 30;

		nlines = LINES - 1;

		menu_win = newwin(nlines, ncols, 0, 0);
		WARNIF(!menu_win);
		WARNIF(box(menu_win, 0, 0) != OK);
		menu = new_menu((ITEM **)menu_items);
		WARNIF(!menu);
		/* Enable multi-selection. */
		menu_opts_off(menu, O_ONEVALUE);
		WARNIF(set_menu_win(menu, menu_win) == ERR);
		menu_sub_win = derwin(menu_win, --nlines, ncols, 1, 0);
		WARNIF(!menu_sub_win);

		preview_win = newwin(nlines, COLS - ncols, 1, ncols);
		WARNIF(!preview_win);

		WARNIF(set_menu_sub(menu, menu_sub_win) == ERR);
		WARNIF(set_menu_format(menu, nlines - 1, 0) != E_OK);
	}

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
	/*
	 * Add a ruler that fills in the gap between left and right status
	 * windows.
	 */
	/* WARNIF(
	 whline(left_status_win, '-', COLS - right_status_win_ncols - 1) != OK);
       */

	WARNIF(post_menu(menu) != E_OK);

	while (g_keep_going) {
		current_menu_item = current_item(menu);
		current_item_index =
		 (unsigned int)item_index(current_menu_item);

		WPRINTW(preview_win, "%s: %lu", choices[current_item_index],
		 strlen(choices[current_item_index]));

		WPRINTW(right_status_win, "%d/%d", current_item_index + 1,
		 items_number);

		ch = wgetch(menu_win);
		switch (ch) {
		case ' ':
			if (item_value(current_menu_item)) {
				--selection_size;
				set_item_value(current_menu_item, FALSE);
			} else {
				++selection_size;
				set_item_value(current_menu_item, TRUE);
			}
			break;
		case 'a':
			selection_size = items_number;
			for (i = 0; i < items_number; ++i) {
				set_item_value(menu_items[i], TRUE);
			}
			break;
		case 'A':
			selection_size = 0;
			for (i = 0; i < items_number; ++i) {
				set_item_value(menu_items[i], FALSE);
			}
			break;
		case '\n':
			sa.sa_handler = kill_external_process;
			WARNIF(sigaction(SIGINT, &sa, 0) == -1);
			def_prog_mode();
			WARNIF(endwin() == ERR);
			if (selection_size) {
				selected_names =
				 malloc((selection_size) * sizeof(char *));
				selected_items =
				 malloc(selection_size * sizeof(ITEM *));
				for (i = 0, j = 0; i < items_number; ++i) {
					if (item_value(menu_items[i]) ==
					    FALSE) {
						continue;
					}
					selected_items[j] = menu_items[i];
					selected_names[j] = choices[i];
					++j;
				}
				spawn_external_process(
				 selected_names, selection_size);
				free(selected_names);
				for (i = 0; i < selection_size; ++i) {
					set_item_value(
					 selected_items[i], FALSE);
				}
				selection_size = 0;
				free(selected_items);
			} else {
				spawn_external_process(
				 &choices[current_item_index], 1);
			}
			reset_prog_mode();
			sa.sa_handler = clean_exit;
			WARNIF(sigaction(SIGINT, &sa, 0) == -1);
			if (wgetch(preview_win) == ERR) {
				WARNIF(refresh() == ERR);
				g_keep_going = 0;
			}
			continue;
		case 'g':
			WARNIF(set_current_item(menu, menu_items[0]) != E_OK);
			break;
		case 'G':
			WARNIF(set_current_item(menu, menu_items[items_number - 1]) != E_OK);
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
		/* case KEY_DOWN: */
		case 'j':
			menu_driver(menu, REQ_DOWN_ITEM);
			break;
		/* case KEY_UP: */
		case 'k':
			menu_driver(menu, REQ_UP_ITEM);
			break;
		case '?':
			WARNIF(erase() == ERR);
			SHOW_KEY_BINDINGS;
			WARNIF(getch() == ERR);
			WARNIF(redrawwin(menu_win) == ERR);
			WARNIF(redrawwin(preview_win) == ERR);
			WARNIF(redrawwin(right_status_win) == ERR);
			WARNIF(redrawwin(left_status_win) == ERR);
			cursor_moved = 1;
			continue;
		case ERR:
			g_keep_going = 0;
			break;
		default:
			WADDSTR(left_status_win,
			 "Press ? to list supported key bindings");
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
	/* Free it all. */
	WARNIF(endwin() == ERR);

	WARNIF(delwin(left_status_win) == ERR);

	WARNIF(delwin(right_status_win) == ERR);

	WARNIF(delwin(preview_win) == ERR);

	WARNIF(unpost_menu(menu) != E_OK);
	for (i = 0; i < items_number; ++i) {
		WARNIF(!free_item(menu_items[i]));
	}
	WARNIF(free_menu(menu) != E_OK);
	WARNIF(delwin(menu_sub_win) == ERR);
	WARNIF(delwin(menu_win) == ERR);

	WARNIF(delwin(main_win) == ERR);

	free(menu_items);

	exit_curses();
	return 0;
}
