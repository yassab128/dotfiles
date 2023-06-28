.POSIX:

EXE := search
SRC := ${EXE}.cpp
OBJ := ${SRC:.c=.o}
INSTALLDIR := ~/.local/bin

CXXFLAGS :=
LDFLAGS :=

.c.o:
	${CXX} ${CFLAGS} -o $@ -c $<

${EXE}: ${OBJ}
	${CXX} ${LDFLAGS} -o $@ ${OBJ}

.PHONY: valgrind
valgrind: ${EXE}
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
	--track-fds=yes --trace-children=yes \
	-- ./$?

.PHONY: sanitize
sanitize:
	${CXX} -g ${CFLAGS} ${LDFLAGS} \
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
