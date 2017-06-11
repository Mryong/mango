TARGET = share.o
CC = gcc
CFLAGS  = -c -g -Wall -DEBUG
INCLUDE = -I../include
OBJS = wchar.o util.o

${TARGET}:${OBJS}
	ld -r -o $@ ${OBJS}
.c.o:
	${CC} ${CFLAGS} ${INCLUDE} $*.c
clean:
	-rm ${TARGET} ${OBJS}

wchar.o: wchar.c ../include/DVM.h
util.o: util.c ../include/DVM.h

