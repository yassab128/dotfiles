EXECUTABLE := sysinfo
CFLAGS := -O3 -D_GNU_SOURCE
INSTALLDIR := ~/.local/bin

OSTYPE := $(shell uname -o)
ifeq (${OSTYPE}, GNU/Linux)
CFLAGS += -DUEVENT='"/sys/class/power_supply/BAT0/uevent"'
else
CFLAGS += -DUEVENT='"/sys/class/power_supply/battery/uevent"'
endif

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

${EXECUTABLE}: $(OBJS)
	${CC} ${CFLAGS} -o $@ $^

.PHONY: lint
lint:
	clang-tidy --extra-arg='-Weverything -Werror' ${SRCS} 2>/dev/null

.PHONY: debug
debug:
	clang ${CFLAGS} -g -fsanitize=leak,address,undefined,cfi -fsanitize-stats\
		-flto -fvisibility=default ${SRCS};\
	su -c ./a.out

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
	@${RM} -v ${EXECUTABLE} ${OBJS} 2>/dev/null;\
	echo ${BUILD}\;
	exit 0
