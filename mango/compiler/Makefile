TARGET = conpiler.o
CC = gcc
ENCODING_FLAG = -DSHIFT_JIS_SOURCE
CFLAGS = -c -g -Wall -DDEBUG
OBJS = lex.yy.o y.tab.o create.o string.o

INCLUDES = -I ../include

$(TARGET):$(OBJS)
	ld -r -o $@ $(OBJS)

y.tab.h y.tab.c: mango.y
	bison  --yacc -dv mango.y
lex.yy.c: mango.l mango.y y.tab.h
	flex mango.l
y.tab.o: y.tab.c mangoc.h
	${CC} -c -g $*.c ${INCLUDES}
lex.yy.o: lex.yy.c mangoc.h
	${CC} -c -g $*.c ${INCLUDES}



.c.o:
	${CC} ${CFLAGS} ${INCLUDES} $*.c


create.o: create.c mangoc.h
string.o: string.c mangoc.h

clean:
	-rm ${TARGET} ${OBJS}







