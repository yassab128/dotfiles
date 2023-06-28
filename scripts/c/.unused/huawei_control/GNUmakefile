EXECUTABLE := huawei_control
CFLAGS := -DNO_MAIN
# CFLAGS := -O3 -D_GNU_SOURCE
# CFLAGS := -D_POSIX_C_SOURCE=200809L
LDFLAGS := -lcurl
INSTALLDIR := ~/.local/bin

SRCS := $(wildcard *.c) ../get_router_ip/get_router_gateway.c
OBJS := $(SRCS:.c=.o)

${EXECUTABLE}: $(OBJS)
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ $^

.PHONY: lint
lint:
	clang-tidy --extra-arg='-Weverything -Werror' ${SRCS} 2>/dev/null

.PHONY: sanitize
sanitize:
	clang -Weverything ${CFLAGS} ${LDFLAGS} -g -fsanitize=leak,address,undefined \
		 ${SRCS};\
	./a.out

.PHONY: valgrind
valgrind:
	clang ${CFLAGS} ${LDFLAGS} -g ${SRCS};\
	valgrind --vgdb=full --leak-check=full --show-leak-kinds=all \
		--exit-on-first-error=yes \
		--track-fds=all --trace-children=yes --track-origins=yes ./a.out

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
