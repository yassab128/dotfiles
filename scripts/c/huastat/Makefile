EXE := huastat
SRC := ${EXE}.c ../get_router_ip/get_router_gateway.c
OBJ := ${SRC:.c=.o}
INSTALLDIR := ~/.local/bin

CFLAGS := -g -ansi -DNO_MAIN -D_GNU_SOURCE
LDFLAGS := -g -DNO_MAIN

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
