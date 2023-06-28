.POSIX:

SRC := get_router_gateway.c
EXE := ${SRC:.c=}
OBJ := ${SRC:.c=.o}

CFLAGS := -ansi
LDFLAGS :=

.c.o:
	${CC} ${CFLAGS} -c $<

${EXE}: ${OBJ}
	${CC} ${LDFLAGS} -o $@ ${OBJ}

.PHONY: valgrind
valgrind: ${EXE}
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes\
		--track-fds=yes --vgdb=full --trace-children=yes\
		-- ./$?

.PHONY: sanitize
sanitize:
	clang -g -Weverything ${CFLAGS} ${LDFLAGS}\
		-fsanitize=leak,address,undefined,cfi -fsanitize-stats\
		-flto -fvisibility=default ${SRC}

.PHONY: clean
clean:
	rm -f ${EXE} ${OBJ} a.out
