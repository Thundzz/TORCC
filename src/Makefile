LEX=lex
YACC=yacc
CFLAGS=-Wall
CC=gcc

all:parse

parse:y.tab.c lex.yy.c map.o
	$(CC) $(CFLAGS) -o $@ $^

y.tab.c:grammar.y 
	$(YACC) -d $^

lex.yy.c:scanner.l
	$(LEX) -o $@ $^

map.o: map.c map.h
	$(CC) $(CFLAGS) -c map.c

clean:
	rm -f grammar.c scanner.c *.o *~ parse lex.yy.c y.tab.*
