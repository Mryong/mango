TARGET = buildin.o
CC = gcc

CFLAGS = -c -g -Wall -DEBUG -fshort-enums
INCLUDES = -I../../include

${TARGET}: buildin.c
	${CC} ${CFLAGS} ${INCLUDES} -o ${TARGET} buildin.c

clean:
	-rm ${TARGET}

