TARGET = dvm.o
OBJS = load.o
CC = gcc
CFLAGS = -c -Wall -DEBUG -I../include


${TARGET}: ${OBJS}
	ld -r -o $@ $^

.o.c:
	${CC} ${CFLAGS} $*.c	

clean:
	-rm ${TARGET} ${OBJS}

load.o: load.c dvm_pri.h ../include/DVM.h 
