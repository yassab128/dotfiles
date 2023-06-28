.POSIX:

EXE := watch_battery_capacity
SRC := ${EXE}.c
OBJ := ${SRC:.c=.o}
INSTALLDIR := ~/.local/bin

CFLAGS := -ansi -D_POSIX_C_SOURCE=200809L
LDFLAGS :=

.c.o:
	${CC} ${CFLAGS} -c $<

${EXE}: ${OBJ}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJ}

.PHONY: valgrind
valgrind: ${EXE}
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes\
		--track-fds=yes --vgdb=full --trace-children=yes\
		-- ./$?

.PHONY: sanitize
sanitize:
	${CC} -g ${CFLAGS} ${LDFLAGS} \
	-fsanitize=leak,address,undefined,pointer-subtract,pointer-compare,\
	pointer-subtract ${SRC}

.PHONY: install
install: ${EXE}
	mkdir -p ${INSTALLDIR};\
	cp $< ${INSTALLDIR}

.PHONY: uninstall
uninstall:
	rm ${INSTALLDIR}/${EXE};\
	rmdir ${INSTALLDIR}

.PHONY: clean
clean:
	rm -f ${EXE} ${OBJ} a.out
