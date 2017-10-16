TARGET = mango
CC = gcc
CFLAGS = -c -g -DEBUG -Wall -fshort-enums
INCLUDES = -I/usr/local/include -I../include
OBJS = main.o\
 ../compiler/conpiler.o\
 ../share/share.o\
 ../debug/dbg.o\
 ../compiler/buildin/buildin.o\
 ../dvm/dvm.o


${TARGET}:${OBJS}
	${CC} -o $@  ${OBJS} -ll


.c.o:
	$(CC) $(CFLAGS) ${INCLUDES} $*.c


clean:
	cd ../compiler;$(MAKE) clean
	cd ../compiler/buildin;$(MAKE) clean
	cd ../share;$(MAKE) clean
	cd ../debug;$(MAKE) clean
	cd ../dvm;$(MAKE) clean
	-rm -f main.o ${TARGET}

all:
	make clean; ${MAKE}


main.o: main.c ../include/MEM.h
../compiler/conpiler.o:
	cd ../compiler;$(MAKE)
../share/share.o:
	cd ../share;$(MAKE)
../debug/dbg.o:
	cd ../debug;${MAKE}
../compiler/buildin/buildin.o:
	cd ../compiler/buildin;${MAKE}
../dvm/dvm.o:
	cd ../dvm;${MAKE}

