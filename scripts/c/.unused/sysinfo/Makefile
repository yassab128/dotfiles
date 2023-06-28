.POSIX:
CC = `command -v clang || echo cc`
OS_TYPE = `sleep 2; uname -o`
CFLAGS = -O3 -D_GNU_SOURCE

SRC = sysinfo.c running_processes.c
OBJ = ${SRC:.c=.o}

Android: clean sysinfo

detect_os:
	-${MAKE} ${OS_TYPE} CC=${CC} OS_TYPE=${OS_TYPE}

sysinfo: ${OBJ}
	${CC} -o $@ ${OBJ} ${CFLAGS}

.c.o:
	${CC} ${CFLAGS} -c $<

clean:
	@rm -v sysinfo ${OBJ} 2>/dev/null || exit 0
	echo ${CC}

.PHONY: all options clean
