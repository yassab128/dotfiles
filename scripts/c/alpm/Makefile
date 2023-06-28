.POSIX:

TARGET := proof
SOURCES := ${TARGET}.c
OBJECTS := ${SOURCES:.c=.o}
INSTALLDIR := ~/.local/bin

CFLAGS := -g
LDFLAGS := -g -lalpm

.c.o:
	${CC} ${CFLAGS} -o $@ -c $<

${TARGET}: ${OBJECTS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJECTS}

.PHONY: valgrind
valgrind: ${TARGET}
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes\
		--track-fds=yes --vgdb=full --trace-children=yes\
		-- ./$?

.PHONY: sanitize
sanitize:
	${CC} -g ${CFLAGS} ${LDFLAGS} \
	-fsanitize=leak,address,undefined,pointer-subtract,pointer-compare \
	${SOURCES}

.PHONY: install
install: ${TARGET}
	mkdir -p ${INSTALLDIR};\
	cp $< ${INSTALLDIR}

.PHONY: uninstall
uninstall:
	rm ${INSTALLDIR}/${TARGET};\
	rmdir ${INSTALLDIR}

.PHONY: clean
clean:
	rm -f ${TARGET} ${OBJECTS} a.out
