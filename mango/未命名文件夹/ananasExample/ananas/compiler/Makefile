TARGET = compiler
CC = gcc
CFLAGS = -c -g -Wall -DDEBUG -fshort-enums
OBJS = lex.yy.o y.tab.o create.o stringUtil.o main.o

INCLUDES = -I ../include

$(TARGET):$(OBJS)
	${CC} -o $@ $(OBJS)

y.tab.h y.tab.c: ananas.y
	bison  --yacc -dv ananas.y
lex.yy.c: ananas.l ananas.y y.tab.h
	flex ananas.l
y.tab.o: y.tab.c
	${CC} -c -g $*.c ${INCLUDES}
lex.yy.o: lex.yy.c ananasc.h
	${CC} -c -g $*.c ${INCLUDES}

.c.o:
	${CC} ${CFLAGS} ${INCLUDES} $*.c

clean:
	-rm ${TARGET} ${OBJS} y.output y.tab.c y.tab.h lex.yy.c

