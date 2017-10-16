TARGET = dvm.o
OBJS = load.o native.o nativeif.o util.o execute.o heap.o wchar.o error_message.o error.o
CC = gcc
CFLAGS = -c -Wall -DEBUG -I../include -fshort-enums


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
execute.o: execute.c   dvm_pri.h ../include/DVM.h
heap.o: heap.c dvm_pri.h ../include/DVM.h
wchar.o: wchar.c
error_message.o: error_message.c dvm_pri.h
error.o: error.c
