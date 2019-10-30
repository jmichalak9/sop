CC=gcc
CFLAGS=-Wall -Werror -pedantic -g
RM=rm
all: zad1
zad1: zad1.c
	${CC} ${CFLAGS} -o zad1 zad1.c
.PHONY: clean all
clean:
	-${RM} zad1
