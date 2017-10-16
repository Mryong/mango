TARGET = share.o
CC = gcc
CFLAGS  = -c -g -Wall -DEBUG -fshort-enums
INCLUDE = -I../include
OBJS = wchar.o util.o opcode.o

${TARGET}:${OBJS}
	ld -r -o $@ ${OBJS}
.c.o:
	${CC} ${CFLAGS} ${INCLUDE} $*.c
clean:
	-rm ${TARGET} ${OBJS}

wchar.o: wchar.c ../include/DVM.h
util.o: util.c ../include/DVM.h
opcode.o: opcode.c ../include/share.h

