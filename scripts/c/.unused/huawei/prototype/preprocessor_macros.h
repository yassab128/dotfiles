#define DIE fprintf(stderr, "Died at %s() line %u\n", __func__, __LINE__);\
	quick_exit

#define TRACE(...) if (DEBUG) {\
	fprintf(stderr, "\033[31m%s:%d:\033[0m ", __func__, __LINE__);\
	fprintf(stderr, __VA_ARGS__);\
}

#define HUMAN_SIZE(x, y) ((x) > 1073741824UL) ? ((y) = 'G', (x) / 1073741824UL) :\
	((x) > 1048576) ? ((y) = 'M', (x) / 1048576) : ((x) > 1024) ? ((y) = 'K'),\
	((x) / 1024) : ((y) = 'B', (x)), (y)
