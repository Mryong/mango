TARGET = mango
CC = gcc
CFLAGS = -c -g -DEBUG -Wall
INCLUDES = -I/usr/local/include -I../include
OBJS = main.o\
 ../compiler/conpiler.o\
 ../memory/men.o\
 ../share/share.o\
 ../debug/dbg.o\
 ../compiler/buildin/buildin.o

${TARGET}:${OBJS}
	${CC} -o $@  ${OBJS} -ll


.c.o:
	$(CC) $(CFLAGS) ${INCLUDES} $*.c


clean:
	-rm -f ${OBJS} ${TARGET}

all:
	make clean; ${MAKE}


main.o: main.c ../include/MEM.h
../compiler/conpiler.o:
	cd ../compiler;$(MAKE)
../memory/men.o:
	cd ../memory;$(MAKE)
../share/share.o:
	cd ../share;$(MAKE)
../debug/dbg.o:
	cd ../debug;${MAKE}
../compiler/buildin/buildin.o:
	cd ../compiler/buildin;${MAKE}

