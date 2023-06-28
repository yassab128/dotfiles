.POSIX:

EXE := xorg_clipboard
SRC := ${EXE}.c
OBJ := ${SRC:.c=.o}
INSTALLDIR := ~/.local/bin

# CFLAGS := -ansi `pkgconf --cflags gtk+-3.0`
# LDFLAGS := `pkgconf --libs gtk+-3.0` -lX11

CFLAGS :=
LDFLAGS := -lX11

.c.o:
	${CC} ${CFLAGS} -o $@ -c $<

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
	-fsanitize=leak,address,undefined,pointer-subtract,pointer-compare \
	${SRC}

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
