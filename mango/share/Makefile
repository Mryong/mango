TARGET = share.o
CC = gcc
CFLAGS  = -c -g -Wall -DEBUG
INCLUDE = -I../include
OBJS = wchar.o

${TARGET}:${OBJS}
	ld -r -o $@ ${OBJS}
.c.o:
	${CC} ${CFLAGS} ${INCLUDE} $*.c
clean:
	-rm ${TARGET} ${OBJS}

wchar.o: wchar.c ../include/DVM.h

