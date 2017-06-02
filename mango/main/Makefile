TARGET = mango
CC = gcc
CFLAGS = -c -g -DEBUG -Wall
INCLUDES = -I/usr/local/include -I../include
OBJS = main.o ../compiler/conpiler.o  ../memory/men.o ../share/share.o

${TARGET}:${OBJS}
	${CC} -o $@  ${OBJS} -ll


.c.o:
	$(CC) $(CFLAGS) ${INCLUDES} $*.c


clean:
	-rm -f ${OBJS} ${TARGET}

all:
	make clean; ${MAKE}


main.o: main.c ../include/MEN.h
../compiler/conpiler.o:
	cd ../compiler;$(MAKE)
../memory/men.o:
	cd ../memory;$(MAKE)
../share/share.o:
	cd ../share;$(MAKE)

