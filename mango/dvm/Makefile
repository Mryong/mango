TARGET = dvm.o
OBJS = load.o native.o nativeif.o util.o
CC = gcc
CFLAGS = -c -Wall -DEBUG -I../include


${TARGET}: ${OBJS}
	ld -r -o $@ $^

.o.c:
	${CC} ${CFLAGS} $*.c	

clean:
	-rm ${TARGET} ${OBJS}

load.o: load.c dvm_pri.h ../include/DVM.h
native.o: native.c dvm_pri.h ../include/DVM.h
nativeif.o: native.c dvm_pri.h ../include/DVM.h
util.o: util.c  dvm_pri.h ../include/DVM.h
