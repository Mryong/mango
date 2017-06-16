TARGET = men.o
TEST_TARGET = men
CC = gcc
CFLAGS = -c -g -DEBUG -Wall
OBJS = memory.o

${TARGET}:${OBJS}
	ld -r -o ${TARGET} ${OBJS}

test: ${OBJS} main.o
	${CC} -o ${TEST_TARGET} ${OBJS} main.o

.c.o:
	$(CC) $(CFLAGS) -I ../include $*.c


clean:
	-rm ${TARGET} ${TEST_TARGET}  ${OBJS} main.o


main.o: main.c ../include/MEM.h



