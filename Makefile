LEX=lex
YACC=yacc
CFLAGS=-Wall
CC=gcc

all:parse

parse:y.tab.c lex.yy.c
	$(CC) $(CFLAGS) -o $@ $^

y.tab.c:grammar.y
	$(YACC) -d $^

lex.yy.c:scanner.l
	$(LEX) -o $@ $^

clean:
	rm -f grammar.c scanner.c
