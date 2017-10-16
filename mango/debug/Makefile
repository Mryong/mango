TARGET = dbg.o
OBJS = debug.o
CC = gcc
CFLAGS = -c -g -Wall -DEBUG -fshort-enums
INCLUDE = -I../include

${TARGET}: ${OBJS}
	ld -r -o $@ ${OBJS}


.c.o:
	${CC} ${CFLAGS} ${INCLUDE} $*.c


clean:
	-rm ${TARGET} ${OBJS}


debug.o: debug.c ../include/DBG.h
