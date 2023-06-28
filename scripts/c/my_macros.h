#pragma once

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define SHOW_SELF_RSS                                                          \
	do {                                                                   \
		FILE *file_ptr;                                                \
		char smaps_line[127];                                          \
		file_ptr = fopen("/proc/self/smaps_rollup", "r");        \
		fgets(smaps_line, 127, file_ptr);                              \
		fgets(smaps_line, 127, file_ptr);                              \
		puts(smaps_line);                                              \
		fclose(file_ptr);                                              \
	} while (0)

#ifdef stdout
#	undef stdout
#endif

#define DIEIF(x)                                                              \
	do {                                                                   \
		if (x) {                                                       \
			puts(__FILE__ ":" TOSTRING(__LINE__) ": " #x);         \
			_exit(1);                                              \
		}                                                              \
	} while (0)

#define WARNIF(x)                                                              \
	do {                                                                   \
		if (x) {                                                       \
			perror(__FILE__ ":" TOSTRING(__LINE__) ": " #x);       \
		}                                                              \
	} while (0)

#define CHECK(x)                                                               \
	do {                                                                   \
		if (x) {                                                       \
			perror(TOSTRING(__LINE__) ":" #x);                     \
			/* quick_exit() doesn't exist on some systems. */      \
			_exit(1);                                              \
		}                                                              \
	} while (0)

#define HUMAN_SIZE(x, y)                                                       \
	(x) > 1073741824 ? ((y) = 'G', (double)(x) / 1073741824)               \
	: (x) > 1048576	 ? ((y) = 'M', (double)(x) / 1048576)                  \
	: (x) > 1024	 ? ((y) = 'K', (double)(x) / 1024)                     \
	: ((y) = 'B', (double)(x)),                              \
	(y)

/* String stuff. */
#define STRLEN(x) (sizeof(x) - 1)
#define STR_WITH_STRLEN(x) x, STRLEN(x)
#define STR_WITH_SIZE(x) x, sizeof(x)

#define PRINT_STRING(x)                                                        \
	fprintf(stderr, "%s %lu %lu\n", (x).data, (x).length,                  \
			strlen((x).data ? (x).data : ""))

/* Disable the silly recursive macro expansion in <curl/curl.h>. */
#ifdef curl_easy_setopt
#	undef curl_easy_setopt
#endif

/* Network stuff */
/* This is only useful when dealing with RTM_GETNEIGH */
#define MY_RTA_NEXT(rta, attrlen)                                              \
	((attrlen) -= RTA_ALIGN((rta)->rta_len),                               \
	 (struct rtattr *)(((unsigned long)(rta)) +                            \
		 RTA_ALIGN((rta)->rta_len)))

#define MY_NLMSG_NEXT(nlh, len)                                                \
	((len) -= NLMSG_ALIGN((nlh)->nlmsg_len),                               \
	 (struct nlmsghdr *)(((unsigned long)(nlh)) +                          \
		 NLMSG_ALIGN((nlh)->nlmsg_len)))

#define INT_TO_IPV4_CONVERTER(x)                                               \
	((x)&0xFF), ((x) >> 8 & 0xFF), ((x) >> 16 & 0xFF), ((x) >> 24 & 0xFF)
