EXECUTABLE := reddit
CFLAGS := -O3
# CFLAGS := -O3 -D_GNU_SOURCE
# CFLAGS := -D_POSIX_C_SOURCE=200809L
LDFLAGS := -lcurl
INSTALLDIR := ~/.local/bin

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

${EXECUTABLE}:
	clang ${CFLAGS} ${LDFLAGS} ${SRCS}

.PHONY: lint
lint:
	clang-tidy --extra-arg='-Weverything -Werror' ${SRCS} 2>/dev/null

.PHONY: sanitize
sanitize:
	clang ${CFLAGS} ${LDFLAGS} -g -fsanitize=leak,address,undefined,cfi \
		-fsanitize-stats -flto -fvisibility=default ${SRCS};\
	su -c ./a.out

.PHONY: valgrind
valgrind:
	clang ${CFLAGS} ${LDFLAGS} -g ${SRCS};\
	valgrind --leak-check=full --show-leak-kinds=all --trace-children=yes \
		--track-origins=yes ./a.out

.PHONY: install
install: ${EXECUTABLE}
	mkdir -p ${INSTALLDIR};\
	cp $< ${INSTALLDIR}

.PHONY: uninstall
uninstall:
	unlink ${INSTALLDIR}/${EXECUTABLE};\
	rmdir ${INSTALLDIR}

.PHONY: clean
clean:
	@${RM} -v ./a.out ${EXECUTABLE} ${OBJS} 2>/dev/null;\
	exit 0
