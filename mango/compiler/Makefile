TARGET = conpiler.o
CC = gcc
CFLAGS = -c -g -Wall -DDEBUG
OBJS = lex.yy.o y.tab.o create.o string.o util.o  fix_tree.o generate.o

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
util.o: util.c mangoc.h
fix_tree.o: fix_tree.c mangoc.h ../include/DVM.h
generate.o: generate.c mangoc.h ../include/DVM.h


clean:
	-rm ${TARGET} ${OBJS}







