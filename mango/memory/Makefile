TARGET = men.o
TEST_TARGET = men
CC = gcc
CFLAGS = -c -g -DEBUG -Wall
OBJS = memory.o storage.o

${TARGET}:${OBJS}
	ld -r -o ${TARGET} ${OBJS}


.c.o:
	$(CC) $(CFLAGS) -I ../include $*.c


clean:
	-rm ${TARGET} ${TEST_TARGET}  ${OBJS} main.o


memory.o: memory.c memory.h ../include/MEM.h
storage.o: storage.c ../include/MEM.h


